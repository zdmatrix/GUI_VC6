//*************************************************************************
// 文件: Devices.c
//*************************************************************************
#define WINVER 0x0500


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
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
// 数据结构定义

// 设备类型定义
typedef enum _tagDEVTYPE
{
	DEVTYPE_USBHID = 0,			// USB接口HID类型设备
	DEVTYPE_USBPCSC,			// USB接口PC/SC类型设备
	DEVTYPE_END					// 定义结束
} DEVTYPE, *LPDEVTYPE;


//*************************************************************************
// 数据结构定义

// HID类型设备ID结构
typedef struct _tagHID_ID
{
	CHAR	cVID[5];
	CHAR	cPID[5];
	CHAR	cUID[9];
} HID_ID, *LPHID_ID;

// 硬件设备挂载信息
typedef struct _tagDEVINFO
{
	// HID类型
	HID_ID		sHID_ID;
	HANDLE		hWrite;				// 2
	HANDLE		hRead;				// 3
	HANDLE		hEventObject;		// 4
	HIDP_CAPS	sCapabilities;		// 1
	OVERLAPPED	sHIDOverlapped;		// 5

	// PC/SC类型
	CHAR		cSCID[256];			// Smart Card 描述符
	SCARDHANDLE	hCard;				// 设备句柄

} DEVINFO, *LPDEVINFO;

// 硬件集信息
typedef struct _tagDEVSET
{
	HWND		hMainDlg;			// DIALOG窗口句柄(设备消息接收窗口)

	LPDULNODE	lpRunDev;			// 已挂接设备描述信息
	UINT32		nRunDevTotal;		// 已挂接设备总数
	UINT32		nRunDevIndex;		// 当前正在使用的设备序号
	UINT32		nRunDevChange;		// 添加或拔除设备的索引号
	HANDLE		hManagerThread;		// PC/SC与UART设备管理线程
	UINT32		nManagerThreadID;	// 线程ID
	BOOL		bThreadRun;			// 线程运行与否
	HANDLE		semDeviceOp;		// 设备使用信号量
} DEVSET, *LPDEVSET;


//*************************************************************************
// 函数声明
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
// 全局变量
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
// 函数: Devece_HID_RegisterNotify
// 功能: 注册HID设备的通知消息
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
// 函数: Device_FreeDev
// 功能: 注册HID设备的通知消息
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
// 函数: Device_GetChange
// 功能: 取变化设备索引
//*************************************************************************
UINT32 Device_GetChange ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return -1;

	return lpDevSet->nRunDevChange;
}

//*************************************************************************
// 函数: Device_GetName
// 功能: 取设备显示用名称
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
// 函数: Device_HID_CmpID
// 功能: 比较2个HID类型设备是否相同
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
// 函数: Device_HID_GetID
// 功能: 分解HID设备的ID结构
//*************************************************************************
LPHID_ID Device_HID_GetID ( PCHAR pDevName )
{
	LPHID_ID	pHID_ID = NULL;
	PCHAR		pStr, pStr1;

	// 查找VID
	pStr = strstr ( pDevName, "Vid_" );
	if ( pStr == NULL )
		pStr = strstr ( pDevName, "VID_" );
	if ( pStr == NULL )
		pStr = strstr ( pDevName, "vid_" );
	if ( pStr == NULL )
		return NULL;
	
	// 为ID分配空间
	MacroAlloc ( pHID_ID, sizeof(HID_ID) );

	strncpy ( pHID_ID->cVID, &pStr[4], 4 );		// 复制VID
	strupr ( pHID_ID->cVID );
	strncpy ( pHID_ID->cPID, &pStr[13], 4 );	// 复制PID
	strupr ( pHID_ID->cPID );

	pStr1 = strchr ( &pStr[20], '&' );			// UID的长度不固定，是以&为结束
	strncpy ( pHID_ID->cUID, &pStr[20], pStr1 - &pStr[20] );	// 复制UID
	strupr ( pHID_ID->cUID );

	return pHID_ID;
}


































//*************************************************************************
// 函数: Device_HID_FindDevice
// 功能: 设备查找
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
// 函数: Device_RunTab_Delete
// 功能: 从运行库中删除设备数据
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
// 函数: Device_RunTab_Add
// 功能: 添加设备数据到运行库中
//*************************************************************************
void Device_RunTab_Add ( LPDEVSET lpDevSet, LPDEVINFO lpDeviceInfo, LPHID_ID pHID_ID, DEVTYPE nType )
{
	LPDEVINFO	lpDevInfo = NULL;

	// 分配空间
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
// 函数: Device_RunTab_Query
// 功能: 设备运行库查询
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
// 函数: Device_HID_RemoveDevice
// 功能: 卸载HID类型设备
//*************************************************************************
LRESULT Device_HID_RemoveDevice ( LPDEVSET lpDevSet, HDEVINFO hDevInfo, LPHID_ID pHID_ID )
{
	// 查询运行库中是否已存在
	if ( Device_RunTab_Query ( lpDevSet, pHID_ID ) == FALSE )
		return FALSE;

	Device_RunTab_Delete ( lpDevSet, pHID_ID );
	return TRUE;
}

//*************************************************************************
// 函数: Device_HID_AddDevice
// 功能: 添加HID类型设备
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

	// 查询运行库中是否已存在
	if ( pHID_ID )
		if ( Device_RunTab_Query ( lpDevSet, pHID_ID ) == TRUE )
			return FALSE;

	// 查找USB HID类型的UKEY设备
	devInfoData.cbSize = sizeof(devInfoData);
	do
	{
		// 枚举设备，成功后返回设备的SP_DEVICE_INTERFACE_DATA型结构指针，否则枚举结束退出循环
		lResult = SetupDiEnumDeviceInterfaces ( hDevInfo, 0, &GUID_DEVINTERFACE_HID, MemberIndex, &devInfoData );

		if ( lResult != 0 )
		{
			// 获取设备接口的详细信息，SetupDiGetDeviceInterfaceDetail函数需要调用2次
			// 1. 获得请求的缓冲大小并分配一个合适的缓冲
			lResult = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, NULL, 0, &Length, NULL );
			MacroAlloc ( detailData, Length );
			//detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc();
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 2. 并再次调用函数来获得接口细节
			lResult = SetupDiGetDeviceInterfaceDetail ( hDevInfo, &devInfoData, detailData, Length, &Required, NULL );
			if ( lResult == TRUE )
			{
				// 获取设备接口的详细信息时出错
				LPHID_ID	pHID_ID2 = NULL;

				// 取设备ID
				pHID_ID2 = Device_HID_GetID ( detailData->DevicePath );
				if ( !pHID_ID || Device_HID_CmpID ( pHID_ID2, pHID_ID ) )
				{
					// 如果设备ID相同
					// *** 添加设备数据到运行库中
					HANDLE					hDeviceHandle;
					PHIDP_PREPARSED_DATA	PreparsedData;

					hDeviceHandle = CreateFile ( detailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );
					HidD_GetPreparsedData ( hDeviceHandle, &PreparsedData );

					// 添加1: 设备能力信息
					HidP_GetCaps ( PreparsedData, &lpDeviceInfo->sCapabilities );	// HIDP_CAPS	sCapabilities;
					HidD_FreePreparsedData ( PreparsedData );

					// 添加2: 文件写入句柄
					lpDeviceInfo->hWrite = CreateFile ( detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );

					// 添加3: 文件读出句柄
					lpDeviceInfo->hRead = CreateFile ( detailData->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL );

					// 添加4: 挂起等待事件
					if ( lpDeviceInfo->hEventObject == 0 )
					{
						lpDeviceInfo->hEventObject				= CreateEvent ( NULL, TRUE, TRUE, "" );		// HANDLE hEventObject;

						// 添加5: 读取设备时所需
						lpDeviceInfo->sHIDOverlapped.hEvent		= lpDeviceInfo->hEventObject;				// OVERLAPPED HIDOverlapped;
						lpDeviceInfo->sHIDOverlapped.Offset		= 0;
						lpDeviceInfo->sHIDOverlapped.OffsetHigh	= 0;
					}

					// *** 添加设备数据到运行库中
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
// 函数: Device_HID_Update
// 功能: 设备变化通知
//*************************************************************************
LRESULT Device_HID_Update ( LPDEVSET lpDevSet, WPARAM wParam, LPARAM lParam )
{
	PDEV_BROADCAST_DEVICEINTERFACE	pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;

	LPHID_ID	pHID_ID = NULL;
	HDEVINFO	hDevInfo;
	DWORD		dwFlag;

	// 取设备ID
	pHID_ID = Device_HID_GetID ( pDevInf->dbcc_name );
	if ( pHID_ID == NULL )
		goto Updata_Quit;

	// 获取一个指定HID类别的所有已安装设备的信息
	dwFlag = DBT_DEVICEARRIVAL != wParam ? DIGCF_INTERFACEDEVICE : (DIGCF_INTERFACEDEVICE | DIGCF_PRESENT);
	hDevInfo = SetupDiGetClassDevs ( &GUID_DEVINTERFACE_HID, NULL, NULL, dwFlag );
	if ( INVALID_HANDLE_VALUE == hDevInfo )
		goto Updata_Quit;

	// 查找设备
	if ( Device_HID_FindDevice ( hDevInfo, pHID_ID ) )
	{
		if ( DBT_DEVICEARRIVAL == wParam )
		{
			// 设备插入
			if ( Device_HID_AddDevice ( lpDevSet, hDevInfo, pHID_ID ) == TRUE )
			{
				PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_MOUNT, 0, 0 );
				goto Updata_Quit;
			}
		}
		else
		{
			// 设备拔除
			if ( Device_HID_RemoveDevice ( lpDevSet, hDevInfo, pHID_ID ) == TRUE )
			{
				PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_UNMOUNT, 0, 0 );
				goto Updata_Quit;
			}
		}
	}

	MacroFree ( pHID_ID );
	SetupDiDestroyDeviceInfoList ( hDevInfo );	// 释放取得的设备信息列表
	return TRUE;

Updata_Quit:
	MacroFree ( pHID_ID );
	SetupDiDestroyDeviceInfoList ( hDevInfo );	// 释放取得的设备信息列表
	return FALSE;
}

//*************************************************************************
// 函数: Device_OnChange
// 功能: 设备变化通知
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
// 函数: Device_Destroy
// 功能: 卸载硬件相关空间
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
// 函数: Device_Send
// 功能: 传送数据
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

	// 将数据写入设备
	bResult = WriteFile ( lpDevInfo->hWrite, pWrite, lpDevInfo->sCapabilities.OutputReportByteLength, &dwBytesWritten, NULL );
	if ( !bResult )
		return -1;
	
	// 获取Report
	bResult = ReadFile ( lpDevInfo->hRead, pRead, lpDevInfo->sCapabilities.InputReportByteLength, &dwNumberOfBytesRead, (LPOVERLAPPED)&lpDevInfo->sHIDOverlapped ); 
	
	// 读取数据出现错误时进行特殊处理
	if ( bResult == 0 )
	{
		// 读取错误信息
		dwResult = GetLastError ();
		
		// I/O端口繁忙
		if ( dwResult == ERROR_IO_PENDING )
		{
			// 需挂起等待
			dwResult = WaitForSingleObject ( lpDevInfo->hEventObject, 1000 );
			
			// I/O端口正常
			if ( dwResult == WAIT_OBJECT_0 )
			{
				// 去读取数据的正常返回值
				GetOverlappedResult ( lpDevInfo->hRead, &lpDevInfo->sHIDOverlapped, &dwNumberOfBytesRead, FALSE );
			}
			else
			{
				return -1;	// 等待超时
			}
		}
		else
		{
			return -1;	// 不明错误
		}
	}
	
	return MAKEWORD ( pRead[pRead[2] + 2], pRead[pRead[2] + 1] );
}

//*************************************************************************
// 函数: Devece_HID_Search
// 功能: 应用启动后主动查找一次已连接的HID设备
//*************************************************************************
VOID Devece_HID_Search ( LPDEVSET lpDevSet )
{
	HDEVINFO	hDevInfo;

	hDevInfo = SetupDiGetClassDevs ( &GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_INTERFACEDEVICE | DIGCF_PRESENT );

	if ( Device_HID_AddDevice ( lpDevSet, hDevInfo, NULL ) == TRUE )
		PostMessage ( lpDevSet->hMainDlg, WM_DEVICE_MOUNT, 0, 0 );

	SetupDiDestroyDeviceInfoList ( hDevInfo );	// 释放取得的设备信息列表

	return;
}

//*************************************************************************
// 函数: Device_PCSC_GetNames
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
// 函数: Device_PCSC_PlugOut
// 功能: 判断是否有设备拔除
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
// 函数: Device_ManagerThrd
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
		// 等待设备操作空闲时进行扫描操作
		dwSem = WaitForSingleObject ( lpDevSet->semDeviceOp, 1000 );
		if ( dwSem != WAIT_OBJECT_0 )
		{
			Sleep ( 1000 );
			continue;
		}

		// 开始扫描
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

				// 判断是否有设备拔除
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
// 函数: Device_Init
// 功能: 初始化硬件检测
//*************************************************************************
LPVOID Device_Init ( HWND hMainDlg )
{
	LPDEVSET	lpDevSet = NULL;
	
	// 为设备集分配空间
	MacroAlloc ( lpDevSet, sizeof(DEVSET) );
	if ( lpDevSet == NULL )
		return NULL;

	// 设置设备用变量
	lpDevSet->hMainDlg = hMainDlg;
	lpDevSet->semDeviceOp = CreateSemaphore ( NULL, 1, 1, NULL );
	if ( NULL == lpDevSet->semDeviceOp )
		return NULL;

	GNODEOP.Create ( &lpDevSet->lpRunDev );

	// 主动搜索已连接的HID设备
	Devece_HID_Search ( lpDevSet );

	// 注册HID设备的通知消息
	if ( Devece_HID_RegisterNotify ( hMainDlg ) == -1 )
		return NULL;

	// 创建PC/SC与UART设备管理线程
	lpDevSet->bThreadRun = TRUE;
	lpDevSet->hManagerThread = (HANDLE)_beginthreadex ( NULL, 0, &Device_ManagerThrd, (void *)lpDevSet, 0, &lpDevSet->nManagerThreadID );

	// 返回设备指针
	return (LPVOID)lpDevSet;
}

//*************************************************************************
// 函数: Device_GetNumber
// 功能: 取可使用设备数量
//*************************************************************************
UINT32 Device_GetNumber ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return 0;

	return lpDevSet->nRunDevTotal;
}

//*************************************************************************
// 函数: Device_GetIndex
// 功能: 取当前设备索引
//*************************************************************************
UINT32 Device_GetIndex ( LPVOID lpVoid )
{
	LPDEVSET	lpDevSet = (LPDEVSET)lpVoid;

	if ( lpDevSet == NULL )
		return 0;

	return lpDevSet->nRunDevIndex;
}

//*************************************************************************
// 函数: Device_SetIndex
// 功能: 设置使用设备索引
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
