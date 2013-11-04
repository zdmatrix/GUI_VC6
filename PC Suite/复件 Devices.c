//*************************************************************************
// 文件: Devices.c
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#define WINVER 0x0500

#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <dbt.h>

#include "SubFunc.h"
#include "Devices.h"


//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量
static VPID gDevHid[] =
{
	{ 0x0925, 0x1234, "HED USB-Key" },		// USB-HID方式UKEY
	{ 0xFFFF, 0xFFFF }						// 定义结束
};


//*************************************************************************
// 此代码模块中包含的函数的向前声明

//*************************************************************************
// 函数: PrepareForOverlappedTransfer
// 功能: 为设备进行异步工作进行设置
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
// 函数: GetDeviceCapabilities
// 功能: 取设备能力信息
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
// 函数: RegisterForDeviceNotifications
// 功能: 注册设备消息通知
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
// 函数: HidLibMatchID
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
// 函数: DevHidAdd2Run
//*************************************************************************
static PUSBHID DevHidAdd2Run ( LPUSERDATA lpUD,
							   PVPID pDevHid,
							   PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData )
{
	PUSBHID		pUsbHid = NULL;

	MacroAlloc ( lpUD->sRunDev[lpUD->nRunDevIndex].pVoid, sizeof(USBHID) );
	pUsbHid = lpUD->sRunDev[lpUD->nRunDevIndex].pVoid;

	lpUD->sRunDev[lpUD->nRunDevIndex].nType = DEV_TYPE_USBHID;

	// 设置相关变量
	memcpy ( &pUsbHid->sVPID, pDevHid, sizeof(VPID) );		// VPID		sVPID;
	pUsbHid->pHidGuid			= &lpUD->sHidGuid;			// GUID		*pHidGuid;
	strcpy ( pUsbHid->szDevPath, detailData->DevicePath );	// CHAR		szDevPath[256];

	lpUD->nRunDevTotal ++;

	return pUsbHid;
}

//*************************************************************************
// 函数: DeviceManagerThrd
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

	
	// 取得HID设备的GUID
	if ( !lpUD->sHidGuid.Data1 )
		HidD_GetHidGuid ( &lpUD->sHidGuid );

	// 获取一个指定类别的所有已安装设备的信息
	hDevInfo = SetupDiGetClassDevs ( &lpUD->sHidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
	if ( INVALID_HANDLE_VALUE == hDevInfo )
		return FALSE;

	// 查找USB HID类型的UKEY设备
	devInfoData.cbSize = sizeof(devInfoData);
	do
	{
		// 枚举设备，成功后返回设备的SP_DEVICE_INTERFACE_DATA型结构指针，否则枚举结束退出循环
		Result = SetupDiEnumDeviceInterfaces ( hDevInfo, 0, &lpUD->sHidGuid, MemberIndex, &devInfoData );

		if ( Result != 0 )
		{
			// 获取设备接口的详细信息，SetupDiGetDeviceInterfaceDetail函数需要调用2次
			// 1. 获得请求的缓冲大小并分配一个合适的缓冲
			Result = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, NULL, 0, &Length, NULL );
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 2. 并再次调用函数来获得接口细节
			Result = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, detailData, Length, &Required, NULL );
			if ( Result == FALSE )
				;	// 获取设备接口的详细信息时出错

			// 开放与设备的通信
			DeviceHandle = CreateFile ( detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );
			if ( DeviceHandle == INVALID_HANDLE_VALUE )
				;	// 无法打开通信端口

			// 取设备属性
			Attributes.Size = sizeof(Attributes);
			Result = HidD_GetAttributes ( DeviceHandle, &Attributes );

			// 在设备匹配库中查找有效的设备
			if ( (pHidInfo = HidLibMatchID ( lpUD, &Attributes, detailData )) != NULL )
			{
				PUSBHID		pUsbHid = NULL;

				// 将有效设备加入运行库
				pUsbHid = DevHidAdd2Run ( lpUD, pHidInfo, detailData );

				// 注册设备消息通知
				RegisterForDeviceNotifications ( lpUD );

				// 取设备能力信息
				GetDeviceCapabilities ( lpUD, pUsbHid, DeviceHandle);

				// 为设备创建文件写入句柄 HANDLE hWrite;
				pUsbHid->hWrite = CreateFile ( detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );

				// 为设备进行异步工作进行设置
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
// 函数: DeviceManagerThrd
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
// 函数: DeviceManager
//*************************************************************************
void DeviceManager ( LPUSERDATA lpUD )
{
	lpUD->hDevManagerThread = (HANDLE)_beginthreadex ( NULL, 0, &DeviceManagerThrd, lpUD, 0, &lpUD->nDevManagerThreadID );
	return;
}


//***************************************************************************
// END
//***************************************************************************
