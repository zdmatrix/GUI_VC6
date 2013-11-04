//*************************************************************************
// �ļ�: Devices.c
//*************************************************************************
#define WINVER 0x0500


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#include <windows.h>
#include <dbt.h>
#include <setupapi.h>
#include <winscard.h>
#include <process.h>
#include "hidsdi.h"
#include "hidpi.h"
#include "Global.h"
#include "Devices.h"


//*************************************************************************
// ���ݽṹ����

// �豸���Ͷ���
typedef enum _tagDEVTYPE
{
	DEVTYPE_USBHID = 0,			// USB�ӿ�HID�����豸
	DEVTYPE_USBPCSC,			// USB�ӿ�PC/SC�����豸
	DEVTYPE_END					// �������
} DEVTYPE, *LPDEVTYPE;


//*************************************************************************
// ���ݽṹ����

// HID�����豸ID�ṹ
typedef struct _tagHID_ID
{
	CHAR	cVID[5];
	CHAR	cPID[5];
	CHAR	cUID[9];
} HID_ID, *LPHID_ID;

// Ӳ���豸������Ϣ
typedef struct _tagDEVINFO
{
	// HID����
	HID_ID		sHID_ID;
	HANDLE		hWrite;				// 2
	HANDLE		hRead;				// 3
	HANDLE		hEventObject;		// 4
	HIDP_CAPS	sCapabilities;		// 1
	OVERLAPPED	sHIDOverlapped;		// 5

	// PC/SC����
	CHAR		cSCID[256];			// Smart Card ������
	SCARDHANDLE	hCard;				// �豸���

} DEVINFO, *LPDEVINFO;

// Ӳ������Ϣ
typedef struct _tagDEVSET
{
	HWND		hMainDlg;			// DIALOG���ھ��(�豸��Ϣ���մ���)

	LPDULNODE	lpRunDev;			// �ѹҽ��豸������Ϣ
	UINT32		nRunDevTotal;		// �ѹҽ��豸����
	UINT32		nRunDevIndex;		// ��ǰ����ʹ�õ��豸���
	UINT32		nRunDevChange;		// ��ӻ�γ��豸��������
	HANDLE		hManagerThread;		// PC/SC��UART�豸�����߳�
	UINT32		nManagerThreadID;	// �߳�ID
	BOOL		bThreadRun;			// �߳��������
	HANDLE		semDeviceOp;		// �豸ʹ���ź���
} DEVSET, *LPDEVSET;


//*************************************************************************
// ��������
LPVOID	Device_Init ( HWND hMainDlg );
VOID	Device_Destroy ( LPVOID *lppData );
LRESULT	Device_OnChange ( LPVOID lpVoid, WPARAM wParam, LPARAM lParam );
WORD	Device_Send ( LPVOID lpVoid, BYTE *pWrite, BYTE *pRead );
PCHAR	Device_GetName ( LPVOID lpVoid, UINT32 nIndex );
UINT32	Device_GetNumber ( LPVOID lpVoid );
UINT32	Device_GetChange ( LPVOID lpVoid );
UINT32	Device_GetIndex ( LPVOID lpVoid );
VOID	Device_SetIndex ( LPVOID lpVoid, UINT32 nIndex );


//*************************************************************************
// ȫ�ֱ���
static GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

const DEVOP GDEVOP =
{
	Device_Init,
	Device_Destroy,
	Device_OnChange,
	Device_Send,
	Device_GetName,
	Device_GetNumber,
	Device_GetChange,
	Device_GetIndex,
	Device_SetIndex
};


//*************************************************************************
//*************************************************************************


//*************************************************************************
// ����: Devece_HID_RegisterNotify
// ����: ע��HID�豸��֪ͨ��Ϣ
//*************************************************************************
static LRESULT Devece_HID_RegisterNotify ( HWND hMainDlg )
{
	HDEVNOTIFY						hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE	NotificationFilter;

	ZeroMemory ( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;
	hDevNotify = RegisterDeviceNotification ( hMainDlg, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE );
	if ( !hDevNotify )
		return -1;

	return 0;
}

//*************************************************************************
// ����: Device_FreeDev
// ����: ע��HID�豸��֪ͨ��Ϣ
//*************************************************************************
VOID Device_FreeDev ( LPVOID lpVoid, UINT32 nCustom )
{
	LPDEVINFO	lpDevInfo = (LPDEVINFO)lpVoid;

	if ( nCustom == DEVTYPE_USBHID )
	{
		CloseHandle ( lpDevInfo->hWrite );
		CloseHandle ( lpDevInfo->hRead );
		CloseHandle ( lpDevInfo->hEventObject );
		MacroFree ( lpDevInfo );
	}

	return;
}

//*************************************************************************
// ����: Device_GetChange
// ����: ȡ�仯�豸����
//*************************************************************************
UINT32 Device_GetChange ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return -1;

	return lpDevSet->nRunDevChange;
}

//*************************************************************************
// ����: Device_GetName
// ����: ȡ�豸��ʾ������
//*************************************************************************
PCHAR Device_GetName ( LPVOID lpVoid, UINT32 nIndex )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;
	LPDULNODE	lpDuLNode = NULL;
	LPDEVINFO	lpDevInfo = NULL;
	PCHAR		pString = NULL;
	UINT32		nLen = 0;

	lpDuLNode = GNODEOP.GetNode ( lpDevSet->lpRunDev, nIndex );
	lpDevInfo = lpDuLNode->lpData;

	if ( lpDuLNode->nCustom == DEVTYPE_USBHID )
	{
		if (   !strcmp ( lpDevInfo->sHID_ID.cVID, "0925" )
			&& !strcmp ( lpDevInfo->sHID_ID.cPID, "1234" ) )
		{
			nLen = strlen ( "(USB) - HID UKey (" ) + 2;
			nLen += strlen ( lpDevInfo->sHID_ID.cUID );
			MacroAlloc ( pString, nLen );
			strcpy ( pString, "(USB) - HID UKey (" );
			strcat ( pString, lpDevInfo->sHID_ID.cUID );
			strcat ( pString, ")");
		}
	}
	else if ( lpDuLNode->nCustom == DEVTYPE_USBPCSC )
	{
		MacroAlloc ( pString, 64 );
		strcpy ( pString, "(USB) - PCSC 01" );
	}

	return pString;
}

//*************************************************************************
// ����: Device_HID_CmpID
// ����: �Ƚ�2��HID�����豸�Ƿ���ͬ
//*************************************************************************
BOOL Device_HID_CmpID ( LPHID_ID pHID_ID1, LPHID_ID pHID_ID2 )
{
	if (   !strcmp ( pHID_ID1->cVID, pHID_ID2->cVID )
		&& !strcmp ( pHID_ID1->cPID, pHID_ID2->cPID )
		&& !strcmp ( pHID_ID1->cUID, pHID_ID2->cUID ) )
	{
		return TRUE;
	}

	return FALSE;
}

//*************************************************************************
// ����: Device_HID_GetID
// ����: �ֽ�HID�豸��ID�ṹ
//*************************************************************************
LPHID_ID Device_HID_GetID ( PCHAR pDevName )
{
	LPHID_ID	pHID_ID = NULL;
	PCHAR		pStr, pStr1;

	// ����VID
	pStr = strstr ( pDevName, "Vid_" );
	if ( pStr == NULL )
		pStr = strstr ( pDevName, "VID_" );
	if ( pStr == NULL )
		pStr = strstr ( pDevName, "vid_" );
	if ( pStr == NULL )
		return NULL;
	
	// ΪID����ռ�
	MacroAlloc ( pHID_ID, sizeof(HID_ID) );

	strncpy ( pHID_ID->cVID, &pStr[4], 4 );		// ����VID
	strupr ( pHID_ID->cVID );
	strncpy ( pHID_ID->cPID, &pStr[13], 4 );	// ����PID
	strupr ( pHID_ID->cPID );

	pStr1 = strchr ( &pStr[20], '&' );			// UID�ĳ��Ȳ��̶�������&Ϊ����
	strncpy ( pHID_ID->cUID, &pStr[20], pStr1 - &pStr[20] );	// ����UID
	strupr ( pHID_ID->cUID );

	return pHID_ID;
}


































//*************************************************************************
// ����: Device_HID_FindDevice
// ����: �豸����
//*************************************************************************
BOOL Device_HID_FindDevice ( HDEVINFO hDevInfo, LPHID_ID pHID_ID )
{
	DWORD		i;
	DWORD		nSize=0 ;
	BOOL		bRet = FALSE;
	TCHAR		buf[MAX_PATH];
	LPHID_ID	pHID_ID2 = NULL;

	SP_DEVINFO_DATA	spDevInfoData;

	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for ( i = 0; SetupDiEnumDeviceInfo ( hDevInfo, i, &spDevInfoData ); i++ )
	{
		if ( !SetupDiGetDeviceInstanceId ( hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize ) )
			return FALSE;

		pHID_ID2 = Device_HID_GetID ( buf );

		if ( (pHID_ID2 != NULL) && Device_HID_CmpID ( pHID_ID, pHID_ID2 ) )
			bRet = TRUE;

		MacroFree ( pHID_ID2 );
	}

	return bRet;
}

//*************************************************************************
// ����: Device_RunTab_Delete
// ����: �����п���ɾ���豸����
//*************************************************************************
void Device_RunTab_Delete ( LPDEVSET lpDevSet, LPHID_ID pHID_ID )
{
	LPDULNODE	lpDuLNode = NULL;
	LPDEVINFO	lpDevInfo = NULL;
	UINT32		i, nTatal;

	if ( lpDevSet == NULL )
		return;

	nTatal = lpDevSet->nRunDevTotal;
	for ( i = 0; i < nTatal; i++ )
	{
		lpDuLNode = GNODEOP.GetNode ( lpDevSet->lpRunDev, i );
		lpDevInfo = (LPDEVINFO)lpDuLNode->lpData;

		if ( Device_HID_CmpID ( &lpDevInfo->sHID_ID, pHID_ID ) )
		{
			GNODEOP.Delete ( &lpDevSet->lpRunDev, i, Device_FreeDev );

			if ( lpDevSet->nRunDevIndex == i )
				lpDevSet->nRunDevIndex = 0;
			else if ( lpDevSet->nRunDevIndex > i )
				lpDevSet->nRunDevIndex--;

			lpDevSet->nRunDevTotal --;
			break;
		}
	}

	return;
}

//*************************************************************************
// ����: Device_RunTab_Add
// ����: ����豸���ݵ����п���
//*************************************************************************
void Device_RunTab_Add ( LPDEVSET lpDevSet, LPDEVINFO lpDeviceInfo, LPHID_ID pHID_ID, DEVTYPE nType )
{
	LPDEVINFO	lpDevInfo = NULL;

	// ����ռ�
	MacroAlloc ( lpDevInfo, sizeof(DEVINFO) );

	memcpy ( lpDevInfo, lpDeviceInfo, sizeof(DEVINFO) );

	switch ( nType )
	{
	case DEVTYPE_USBHID:
		memcpy ( &lpDevInfo->sHID_ID, pHID_ID, sizeof(HID_ID) );
		break;
	}
	lpDevSet->nRunDevChange = lpDevSet->nRunDevTotal ++;

	GNODEOP.Append ( lpDevSet->lpRunDev, lpDevInfo, nType );

	return;
}

//*************************************************************************
// ����: Device_RunTab_Query
// ����: �豸���п��ѯ
//*************************************************************************
BOOL Device_RunTab_Query ( LPDEVSET lpDevSet, LPHID_ID pHID_ID )
{
	LPDULNODE	lpDuLNode = NULL;
	LPDEVINFO	lpDevInfo = NULL;
	UINT32		i;

	if ( lpDevSet == NULL )
		return FALSE;

	for ( i = 0; i < lpDevSet->nRunDevTotal; i++ )
	{
		lpDuLNode = GNODEOP.GetNode ( lpDevSet->lpRunDev, i );
		lpDevInfo = (LPDEVINFO)lpDuLNode->lpData;
		if ( Device_HID_CmpID ( &lpDevInfo->sHID_ID, pHID_ID ) )
			return TRUE;
	}

	return FALSE;
}

//*************************************************************************
// ����: Device_HID_RemoveDevice
// ����: ж��HID�����豸
//*************************************************************************
LRESULT Device_HID_RemoveDevice ( LPDEVSET lpDevSet, HDEVINFO hDevInfo, LPHID_ID pHID_ID )
{
	// ��ѯ���п����Ƿ��Ѵ���
	if ( Device_RunTab_Query ( lpDevSet, pHID_ID ) == FALSE )
		return FALSE;

	Device_RunTab_Delete ( lpDevSet, pHID_ID );
	return TRUE;
}

//*************************************************************************
// ����: Device_HID_AddDevice
// ����: ���HID�����豸
//*************************************************************************
LRESULT Device_HID_AddDevice ( LPDEVSET lpDevSet, HDEVINFO hDevInfo, LPHID_ID pHID_ID )
{
	SP_DEVICE_INTERFACE_DATA			devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData = NULL;

	LRESULT		lResult;
	int			MemberIndex = 0;
	ULONG		Length;
	ULONG		Required;
	BOOL		LastDevice = FALSE;
	BOOL		MyDeviceDetected = FALSE; 

	LPDEVINFO	lpDeviceInfo = NULL;

	MacroAlloc ( lpDeviceInfo, sizeof(DEVINFO) );

	// ��ѯ���п����Ƿ��Ѵ���
	if ( pHID_ID )
		if ( Device_RunTab_Query ( lpDevSet, pHID_ID ) == TRUE )
			return FALSE;

	// ����USB HID���͵�UKEY�豸
	devInfoData.cbSize = sizeof(devInfoData);
	do
	{
		// ö���豸���ɹ��󷵻��豸��SP_DEVICE_INTERFACE_DATA�ͽṹָ�룬����ö�ٽ����˳�ѭ��
		lResult = SetupDiEnumDeviceInterfaces ( hDevInfo, 0, &GUID_DEVINTERFACE_HID, MemberIndex, &devInfoData );

		if ( lResult != 0 )
		{
			// ��ȡ�豸�ӿڵ���ϸ��Ϣ��SetupDiGetDeviceInterfaceDetail������Ҫ����2��
			// 1. �������Ļ����С������һ�����ʵĻ���
			lResult = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, NULL, 0, &Length, NULL );
			MacroAlloc ( detailData, Length );
			//detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc();
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 2. ���ٴε��ú�������ýӿ�ϸ��
			lResult = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, detailData, Length, &Required, NULL );
			if ( lResult == TRUE )
			{
				// ��ȡ�豸�ӿڵ���ϸ��Ϣʱ����
				LPHID_ID	pHID_ID2 = NULL;

				// ȡ�豸ID
				pHID_ID2 = Device_HID_GetID ( detailData->DevicePath );
				if ( !pHID_ID || Device_HID_CmpID ( pHID_ID2, pHID_ID ) )
				{
					// ����豸ID��ͬ
					// *** ����豸���ݵ����п���
					HANDLE					hDeviceHandle;
					PHIDP_PREPARSED_DATA	PreparsedData;

					hDeviceHandle = CreateFile ( detailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );
					HidD_GetPreparsedData ( hDeviceHandle, &PreparsedData );

					// ���1: �豸������Ϣ
					HidP_GetCaps ( PreparsedData, &lpDeviceInfo->sCapabilities );	// HIDP_CAPS	sCapabilities;
					HidD_FreePreparsedData ( PreparsedData );

					// ���2: �ļ�д����
					lpDeviceInfo->hWrite = CreateFile ( detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );

					// ���3: �ļ��������
					lpDeviceInfo->hRead = CreateFile ( detailData->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL );

					// ���4: ����ȴ��¼�
					if ( lpDeviceInfo->hEventObject == 0 )
					{
						lpDeviceInfo->hEventObject				= CreateEvent ( NULL, TRUE, TRUE, "" );		// HANDLE hEventObject;

						// ���5: ��ȡ�豸ʱ����
						lpDeviceInfo->sHIDOverlapped.hEvent		= lpDeviceInfo->hEventObject;				// OVERLAPPED HIDOverlapped;
						lpDeviceInfo->sHIDOverlapped.Offset		= 0;
						lpDeviceInfo->sHIDOverlapped.OffsetHigh	= 0;
					}

					// *** ����豸���ݵ����п���
					Device_RunTab_Add ( lpDevSet, lpDeviceInfo, pHID_ID2, DEVTYPE_USBHID );
					MyDeviceDetected = TRUE;
					CloseHandle ( hDeviceHandle );
				}
				MacroFree ( pHID_ID2 );
			}

			MacroFree ( detailData );
		}
		else
		{
			LastDevice = TRUE;
		}
		
		MemberIndex = MemberIndex + 1;
	} while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));

	MacroFree ( lpDeviceInfo );
	return MyDeviceDetected;
}

//*************************************************************************
// ����: Device_HID_Update
// ����: �豸�仯֪ͨ
//*************************************************************************
LRESULT Device_HID_Update ( LPDEVSET lpDevSet, WPARAM wParam, LPARAM lParam )
{
	PDEV_BROADCAST_DEVICEINTERFACE	pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;

	LPHID_ID	pHID_ID = NULL;
	HDEVINFO	hDevInfo;
	DWORD		dwFlag;

	// ȡ�豸ID
	pHID_ID = Device_HID_GetID ( pDevInf->dbcc_name );
	if ( pHID_ID == NULL )
		goto Updata_Quit;

	// ��ȡһ��ָ��HID���������Ѱ�װ�豸����Ϣ
	dwFlag = DBT_DEVICEARRIVAL != wParam ? DIGCF_INTERFACEDEVICE : (DIGCF_INTERFACEDEVICE | DIGCF_PRESENT);
	hDevInfo = SetupDiGetClassDevs ( &GUID_DEVINTERFACE_HID, NULL, NULL, dwFlag );
	if ( INVALID_HANDLE_VALUE == hDevInfo )
		goto Updata_Quit;

	// �����豸
	if ( Device_HID_FindDevice ( hDevInfo, pHID_ID ) )
	{
		if ( DBT_DEVICEARRIVAL == wParam )
		{
			// �豸����
			if ( Device_HID_AddDevice ( lpDevSet, hDevInfo, pHID_ID ) == TRUE )
			{
				PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_MOUNT, 0, 0 );
				goto Updata_Quit;
			}
		}
		else
		{
			// �豸�γ�
			if ( Device_HID_RemoveDevice ( lpDevSet, hDevInfo, pHID_ID ) == TRUE )
			{
				PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_UNMOUNT, 0, 0 );
				goto Updata_Quit;
			}
		}
	}

	MacroFree ( pHID_ID );
	SetupDiDestroyDeviceInfoList ( hDevInfo );	// �ͷ�ȡ�õ��豸��Ϣ�б�
	return TRUE;

Updata_Quit:
	MacroFree ( pHID_ID );
	SetupDiDestroyDeviceInfoList ( hDevInfo );	// �ͷ�ȡ�õ��豸��Ϣ�б�
	return FALSE;
}

//*************************************************************************
// ����: Device_OnChange
// ����: �豸�仯֪ͨ
//*************************************************************************
LRESULT Device_OnChange ( LPVOID lpVoid, WPARAM wParam, LPARAM lParam )
{
	if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam )
	{
		PDEV_BROADCAST_HDR	pHdr = (PDEV_BROADCAST_HDR)lParam;

		if ( pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
			Device_HID_Update ( (LPDEVSET)lpVoid, wParam, lParam );
	}

	return 0;
}


//*************************************************************************
// ����: Device_Destroy
// ����: ж��Ӳ����ؿռ�
//*************************************************************************
VOID Device_Destroy ( LPVOID *lppData )
{
	LPDEVSET	*lppDevSet = (LPDEVSET *)lppData;

	(*lppDevSet)->bThreadRun = FALSE;
	WaitForSingleObject ( (*lppDevSet)->hManagerThread, INFINITE );
	GNODEOP.Destroy ( &(*lppDevSet)->lpRunDev, Device_FreeDev );
	MacroFree ( *lppDevSet );

	return;
}

//*************************************************************************
// ����: Device_Send
// ����: ��������
//*************************************************************************
WORD Device_Send ( LPVOID lpVoid, BYTE *pWrite, BYTE *pRead )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;
	LPDULNODE	lpDuLNode = NULL;
	LPDEVINFO	lpDevInfo = NULL;
	DWORD		dwBytesWritten = 0;
	DWORD		dwNumberOfBytesRead;
	BOOL		bResult;
	DWORD		dwResult;

	lpDuLNode = GNODEOP.GetNode ( lpDevSet->lpRunDev, lpDevSet->nRunDevIndex );
	lpDevInfo = (LPDEVINFO)lpDuLNode->lpData;

	// ������д���豸
	bResult = WriteFile ( lpDevInfo->hWrite, pWrite, lpDevInfo->sCapabilities.OutputReportByteLength, &dwBytesWritten, NULL );
	if ( !bResult )
		return -1;
	
	// ��ȡReport
	bResult = ReadFile ( lpDevInfo->hRead, pRead, lpDevInfo->sCapabilities.InputReportByteLength, &dwNumberOfBytesRead, (LPOVERLAPPED)&lpDevInfo->sHIDOverlapped ); 
	
	// ��ȡ���ݳ��ִ���ʱ�������⴦��
	if ( bResult == 0 )
	{
		// ��ȡ������Ϣ
		dwResult = GetLastError ();
		
		// I/O�˿ڷ�æ
		if ( dwResult == ERROR_IO_PENDING )
		{
			// �����ȴ�
			dwResult = WaitForSingleObject ( lpDevInfo->hEventObject, 1000 );
			
			// I/O�˿�����
			if ( dwResult == WAIT_OBJECT_0 )
			{
				// ȥ��ȡ���ݵ���������ֵ
				GetOverlappedResult ( lpDevInfo->hRead, &lpDevInfo->sHIDOverlapped, &dwNumberOfBytesRead, FALSE );
			}
			else
			{
				return -1;	// �ȴ���ʱ
			}
		}
		else
		{
			return -1;	// ��������
		}
	}
	
	return MAKEWORD ( pRead[pRead[2] + 2], pRead[pRead[2] + 1] );
}

//*************************************************************************
// ����: Devece_HID_Search
// ����: Ӧ����������������һ�������ӵ�HID�豸
//*************************************************************************
VOID Devece_HID_Search ( LPDEVSET lpDevSet )
{
	HDEVINFO	hDevInfo;

	hDevInfo = SetupDiGetClassDevs ( &GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_INTERFACEDEVICE | DIGCF_PRESENT );

	if ( Device_HID_AddDevice ( lpDevSet, hDevInfo, NULL ) == TRUE )
		PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_MOUNT, 0, 0 );

	SetupDiDestroyDeviceInfoList ( hDevInfo );	// �ͷ�ȡ�õ��豸��Ϣ�б�

	return;
}

//*************************************************************************
// ����: Device_PCSC_GetNames
//*************************************************************************
PCHAR Device_PCSC_GetNames ( LPTSTR mszReaders, DWORD dwReaders, int nIndex )
{
	PCHAR	szName = mszReaders;
	int		i = 0;

	while ( nIndex )
	{
		if ( mszReaders[i] == 0x00 )
		{
			if ( (mszReaders[i] == 0x00) && (mszReaders[i + 1] == 0x00) )
				return NULL;
			szName = &mszReaders[i];
			nIndex --;
		}
		i++;
	}

	return szName;
}

//*************************************************************************
// ����: Device_PCSC_PlugOut
// ����: �ж��Ƿ����豸�γ�
//*************************************************************************
int Device_PCSC_PlugOut ( LPDEVSET lpDevSet, LPTSTR mszReaders, DWORD dwReaders )
{
	UINT32		i, k;
	PCHAR		szName = NULL;
	LPDULNODE	lpDNode = NULL;
	LPDEVINFO	lpDevInfo = NULL;

	if ( !lpDevSet->nRunDevTotal )
		return 1;

	for ( i = 0; i < lpDevSet->nRunDevTotal; i++ )
	{
		lpDNode = GNODEOP.Find ( lpDevSet->lpRunDev, DEVTYPE_USBPCSC );
		if ( lpDNode == NULL )
			return 0;
		lpDevInfo = lpDNode->lpData;

		szName = Device_PCSC_GetNames ( mszReaders, dwReaders, i );

		k = strcmp ( lpDevInfo->cSCID, szName );
		if ( !k )
			continue;
	}

	return 0;
}

//*************************************************************************
// ����: Device_ManagerThrd
//*************************************************************************
BYTE pbSendBuffer[] = { 0xBF, 0xAB, 0x00, 0x00, 0x03, 0x36, 0x36, 0x36 };
//BYTE pbSendBuffer[] = { 0xC0, 0xA4, 0x00, 0x00, 0x02, 0x7F, 0x20 };
BYTE pbRecvBuffer[512];

static unsigned __stdcall Device_ManagerThrd ( void *pArglist )
{
	LPDEVSET		lpDevSet = (LPDEVSET)pArglist;
	SCARDCONTEXT	hContext;
	LONG			rv;
	DWORD			dwReaders;
	LPTSTR			mszReaders = NULL;
	SCARDHANDLE		hCard;
	DWORD			dwActiveProtocol;
//	SCARD_IO_REQUEST	pioRecvPci;
	DWORD			dwSem;
	LPDEVINFO		lpDeviceInfo = NULL;

	rv = SCardEstablishContext ( SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext );
	if ( rv != SCARD_S_SUCCESS )
		return 0;

	while ( lpDevSet->bThreadRun )
	{
		// �ȴ��豸��������ʱ����ɨ�����
		dwSem = WaitForSingleObject ( lpDevSet->semDeviceOp, 1000 );
		if ( dwSem != WAIT_OBJECT_0 )
		{
			Sleep ( 1000 );
			continue;
		}

		// ��ʼɨ��
		dwReaders = 0;
		rv = SCardListReaders ( hContext, NULL, NULL, &dwReaders );
		if ( dwReaders != 0 )
		{
			MacroAlloc ( mszReaders, sizeof(char) * dwReaders );
			rv = SCardListReaders ( hContext, NULL, mszReaders, &dwReaders );
			if ( rv == SCARD_S_SUCCESS )
			{
				PCHAR	szName = NULL;
				int		nIndex = 0;

				// �ж��Ƿ����豸�γ�
				Device_PCSC_PlugOut ( lpDevSet, mszReaders, dwReaders );

				szName = Device_PCSC_GetNames ( mszReaders, dwReaders, nIndex );
//				Device_PCSC_
				rv = SCardConnect ( hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &dwActiveProtocol);
				if ( rv == SCARD_S_SUCCESS )
				{
					rv = SCardBeginTransaction ( hCard );
					if ( rv == SCARD_S_SUCCESS )
					{
						MacroAlloc ( lpDeviceInfo, sizeof(DEVINFO) );
						lpDeviceInfo->hCard = hCard;
 						memcpy ( lpDeviceInfo->cSCID, mszReaders, dwReaders );
 						Device_RunTab_Add ( lpDevSet, lpDeviceInfo, NULL, DEVTYPE_USBPCSC );
						MacroFree ( lpDeviceInfo );
						MacroFree ( mszReaders );
						ReleaseSemaphore ( lpDevSet->semDeviceOp, 1, NULL );
						PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_MOUNT, 0, 0 );
						continue;
// 						dwSendLength = sizeof ( pbSendBuffer );
// 						dwRecvLength = sizeof ( pbRecvBuffer );
// 						memset ( pbRecvBuffer, 0x00, dwRecvLength );
// 						rv = SCardTransmit ( hCard, SCARD_PCI_T0, pbSendBuffer, dwSendLength, NULL, pbRecvBuffer, &dwRecvLength );
					}
					rv = SCardEndTransaction ( hCard, SCARD_LEAVE_CARD );
				}
				rv = SCardDisconnect ( hCard, SCARD_UNPOWER_CARD );
			}
			MacroFree ( mszReaders );
		}

		ReleaseSemaphore ( lpDevSet->semDeviceOp, 1, NULL );
		Sleep ( 1000 );
	}

	rv = SCardReleaseContext ( hContext );
	return 0;
}

//*************************************************************************
// ����: Device_Init
// ����: ��ʼ��Ӳ�����
//*************************************************************************
LPVOID Device_Init ( HWND hMainDlg )
{
	LPDEVSET	lpDevSet = NULL;
	
	// Ϊ�豸������ռ�
	MacroAlloc ( lpDevSet, sizeof(DEVSET) );
	if ( lpDevSet == NULL )
		return NULL;

	// �����豸�ñ���
	lpDevSet->hMainDlg = hMainDlg;
	lpDevSet->semDeviceOp = CreateSemaphore ( NULL, 1, 1, NULL );
	if ( NULL == lpDevSet->semDeviceOp )
		return NULL;

	GNODEOP.Create ( &lpDevSet->lpRunDev );

	// �������������ӵ�HID�豸
	Devece_HID_Search ( lpDevSet );

	// ע��HID�豸��֪ͨ��Ϣ
	if ( Devece_HID_RegisterNotify ( hMainDlg ) == -1 )
		return NULL;

	// ����PC/SC��UART�豸�����߳�
	lpDevSet->bThreadRun = TRUE;
	lpDevSet->hManagerThread = (HANDLE)_beginthreadex ( NULL, 0, &Device_ManagerThrd, (void *)lpDevSet, 0, &lpDevSet->nManagerThreadID );

	// �����豸ָ��
	return (LPVOID)lpDevSet;
}

//*************************************************************************
// ����: Device_GetNumber
// ����: ȡ��ʹ���豸����
//*************************************************************************
UINT32 Device_GetNumber ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return 0;

	return lpDevSet->nRunDevTotal;
}

//*************************************************************************
// ����: Device_GetIndex
// ����: ȡ��ǰ�豸����
//*************************************************************************
UINT32 Device_GetIndex ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return 0;

	return lpDevSet->nRunDevIndex;
}

//*************************************************************************
// ����: Device_SetIndex
// ����: ����ʹ���豸����
//*************************************************************************
VOID Device_SetIndex ( LPVOID lpVoid, UINT32 nIndex )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return;

	if ( nIndex < lpDevSet->nRunDevTotal )
		lpDevSet->nRunDevIndex = nIndex;

	return;
}


//***************************************************************************
// END
//***************************************************************************
