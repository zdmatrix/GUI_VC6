//*************************************************************************
// �ļ�: Devices.c
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#define WINVER 0x0500

#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <dbt.h>

#include "SubFunc.h"
#include "Devices.h"


//*************************************************************************
// �궨��


//*************************************************************************
// ȫ�ֱ���
static VPID gDevHid[] =
{
	{ 0x0925, 0x1234, "HED USB-Key" },		// USB-HID��ʽUKEY
	{ 0xFFFF, 0xFFFF }						// �������
};


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����

//*************************************************************************
// ����: PrepareForOverlappedTransfer
// ����: Ϊ�豸�����첽������������
//*************************************************************************
static void PrepareForOverlappedTransfer ( LPUSERDATA lpUD, PUSBHID pUsbHid, PSP_DEVICE_INTERFACE_DETAIL_DATA detailData )
{
	// HANDLE		hRead;
	pUsbHid->hRead = CreateFile ( detailData->DevicePath,
								  GENERIC_READ,
								  FILE_SHARE_READ | FILE_SHARE_WRITE,
								  (LPSECURITY_ATTRIBUTES)NULL,
								  OPEN_EXISTING,
								  FILE_FLAG_OVERLAPPED,
								  NULL );

	if ( pUsbHid->hEventObject == 0 )
	{
		pUsbHid->hEventObject				= CreateEvent ( NULL, TRUE, TRUE, "" );		// HANDLE hEventObject;
		pUsbHid->HIDOverlapped.hEvent		= pUsbHid->hEventObject;					// OVERLAPPED HIDOverlapped;
		pUsbHid->HIDOverlapped.Offset		= 0;
		pUsbHid->HIDOverlapped.OffsetHigh	= 0;
	}
}



//*************************************************************************
// ����: GetDeviceCapabilities
// ����: ȡ�豸������Ϣ
//*************************************************************************
static void GetDeviceCapabilities ( LPUSERDATA lpUD, PUSBHID pUsbHid, HANDLE DeviceHandle )
{
	PHIDP_PREPARSED_DATA	PreparsedData;

	HidD_GetPreparsedData ( DeviceHandle, &PreparsedData );

	HidP_GetCaps ( PreparsedData, &pUsbHid->sCapabilities );	// HIDP_CAPS	sCapabilities;

	HidD_FreePreparsedData ( PreparsedData );

	return;
}

//*************************************************************************
// ����: RegisterForDeviceNotifications
// ����: ע���豸��Ϣ֪ͨ
//*************************************************************************
static void RegisterForDeviceNotifications ( LPUSERDATA lpUD )
{
	DEV_BROADCAST_DEVICEINTERFACE	DevBroadcastDeviceInterface;
	HDEVNOTIFY						DeviceNotificationHandle;
	
	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	DevBroadcastDeviceInterface.dbcc_classguid = lpUD->sHidGuid;
	
	DeviceNotificationHandle =
		RegisterDeviceNotification ( lpUD->hMainDlg, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE );

	return;
}

//*************************************************************************
// ����: HidLibMatchID
//*************************************************************************
static PVOID HidLibMatchID ( LPUSERDATA lpUD, PHIDD_ATTRIBUTES pAttributes, PSP_DEVICE_INTERFACE_DETAIL_DATA detailData )
{
	UINT32		i, j;

	for ( i = 0; (gDevHid[i].nProductID != 0xFFF) && (gDevHid[i].nVendorID != 0xFFFF); i++ )
	{
		if (   (pAttributes->ProductID == gDevHid[i].nProductID)
			&& (pAttributes->VendorID  == gDevHid[i].nVendorID) )
		{
			for ( j = 0; j < lpUD->nRunDevTotal; j++ )
			{
				if ( lpUD->sRunDev[j].nType == DEV_TYPE_USBHID )
				{
					PUSBHID		pUsbHid = lpUD->sRunDev[j].pVoid;

					if ( strcmp( pUsbHid->szDevPath, detailData->DevicePath ) == 0 )
						return NULL;
				}
			}

			return &gDevHid[i];
		}
	}

	return NULL;
}

//*************************************************************************
// ����: DevHidAdd2Run
//*************************************************************************
static PUSBHID DevHidAdd2Run ( LPUSERDATA lpUD,
							   PVPID pDevHid,
							   PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData )
{
	PUSBHID		pUsbHid = NULL;

	MacroAlloc ( lpUD->sRunDev[lpUD->nRunDevIndex].pVoid, sizeof(USBHID) );
	pUsbHid = lpUD->sRunDev[lpUD->nRunDevIndex].pVoid;

	lpUD->sRunDev[lpUD->nRunDevIndex].nType = DEV_TYPE_USBHID;

	// ������ر���
	memcpy ( &pUsbHid->sVPID, pDevHid, sizeof(VPID) );		// VPID		sVPID;
	pUsbHid->pHidGuid			= &lpUD->sHidGuid;			// GUID		*pHidGuid;
	strcpy ( pUsbHid->szDevPath, detailData->DevicePath );	// CHAR		szDevPath[256];

	lpUD->nRunDevTotal ++;

	return pUsbHid;
}

//*************************************************************************
// ����: DeviceManagerThrd
//*************************************************************************
static UINT32 DeviceSearch_HID ( LPUSERDATA lpUD )
{
	HIDD_ATTRIBUTES						Attributes;
	SP_DEVICE_INTERFACE_DATA			devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;

	HANDLE		hDevInfo;
	LONG		Result;	
	int			MemberIndex = 0;
	BOOL		LastDevice = FALSE;
	ULONG		Length;
	ULONG		Required;
	HANDLE		DeviceHandle;
	LPVOID		pHidInfo;
	BOOL		MyDeviceDetected = FALSE; 

	
	// ȡ��HID�豸��GUID
	if ( !lpUD->sHidGuid.Data1 )
		HidD_GetHidGuid ( &lpUD->sHidGuid );

	// ��ȡһ��ָ�����������Ѱ�װ�豸����Ϣ
	hDevInfo = SetupDiGetClassDevs ( &lpUD->sHidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
	if ( INVALID_HANDLE_VALUE == hDevInfo )
		return FALSE;

	// ����USB HID���͵�UKEY�豸
	devInfoData.cbSize = sizeof(devInfoData);
	do
	{
		// ö���豸���ɹ��󷵻��豸��SP_DEVICE_INTERFACE_DATA�ͽṹָ�룬����ö�ٽ����˳�ѭ��
		Result = SetupDiEnumDeviceInterfaces ( hDevInfo, 0, &lpUD->sHidGuid, MemberIndex, &devInfoData );

		if ( Result != 0 )
		{
			// ��ȡ�豸�ӿڵ���ϸ��Ϣ��SetupDiGetDeviceInterfaceDetail������Ҫ����2��
			// 1. �������Ļ����С������һ�����ʵĻ���
			Result = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, NULL, 0, &Length, NULL );
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 2. ���ٴε��ú�������ýӿ�ϸ��
			Result = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, detailData, Length, &Required, NULL );
			if ( Result == FALSE )
				;	// ��ȡ�豸�ӿڵ���ϸ��Ϣʱ����

			// �������豸��ͨ��
			DeviceHandle = CreateFile ( detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );
			if ( DeviceHandle == INVALID_HANDLE_VALUE )
				;	// �޷���ͨ�Ŷ˿�

			// ȡ�豸����
			Attributes.Size = sizeof(Attributes);
			Result = HidD_GetAttributes ( DeviceHandle, &Attributes );

			// ���豸ƥ����в�����Ч���豸
			if ( (pHidInfo = HidLibMatchID ( lpUD, &Attributes, detailData )) != NULL )
			{
				PUSBHID		pUsbHid = NULL;

				// ����Ч�豸�������п�
				pUsbHid = DevHidAdd2Run ( lpUD, pHidInfo, detailData );

				// ע���豸��Ϣ֪ͨ
				RegisterForDeviceNotifications ( lpUD );

				// ȡ�豸������Ϣ
				GetDeviceCapabilities ( lpUD, pUsbHid, DeviceHandle);

				// Ϊ�豸�����ļ�д���� HANDLE hWrite;
				pUsbHid->hWrite = CreateFile ( detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );

				// Ϊ�豸�����첽������������
				PrepareForOverlappedTransfer ( lpUD, pUsbHid, detailData );

				MyDeviceDetected = TRUE;
			}
			else
			{
				CloseHandle ( DeviceHandle );
			}

			free ( detailData );
		}
		else
		{
			LastDevice = TRUE;
		}
		
		MemberIndex = MemberIndex + 1;
	} while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));

	if ( MyDeviceDetected )
		PostMessage ( lpUD->hMainDlg, WM_NEWDEV_MOUNT, 0, 0 );

	return 0;
}


//*************************************************************************
// ����: DeviceManagerThrd
//*************************************************************************
static unsigned __stdcall DeviceManagerThrd ( void *pArglist )
{
	LPUSERDATA lpUD = (LPUSERDATA)pArglist;

	while ( TRUE )
	{
		Sleep ( 5000 );
		DeviceSearch_HID ( lpUD );
	}

	return 0;
}


//*************************************************************************
// ����: DeviceManager
//*************************************************************************
void DeviceManager ( LPUSERDATA lpUD )
{
	lpUD->hDevManagerThread = (HANDLE)_beginthreadex ( NULL, 0, &DeviceManagerThrd, lpUD, 0, &lpUD->nDevManagerThreadID );
	return;
}


//***************************************************************************
// END
//***************************************************************************
