//*************************************************************************
// 工程：模态对话框处理模块
// 文件：Dialog.c
// 内容：???
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#define WINVER 0x0500

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <dbt.h>
#include <richedit.h>
#include <ctype.h>
#include "resource.h"

#include "SubFunc.h"
//#include "Devices.h"
#include "TabCtrl.h"
#include "Dialog.h"						// 


//*************************************************************************
// 宏定义

typedef		unsigned char		UINT8;

UINT8	g_D1A;
UINT8	g_D1B;
UINT8	g_D10A;
UINT8	g_D10B;
UINT8	g_BG;
UINT8	g_CMN;
UINT8	g_CheckSum;


static UINT8 NUM0[] = { 2, 3, 4, 5, 6, 7, 0xFF };
static UINT8 NUM1[] = { 5, 6, 0xFF };
static UINT8 NUM2[] = { 1, 3, 4, 6, 7, 0xFF };
static UINT8 NUM3[] = { 1, 4, 5, 6, 7, 0xFF };
static UINT8 NUM4[] = { 1, 2, 5, 6, 0xFF };
static UINT8 NUM5[] = { 1, 2, 4, 5, 7, 0xFF };
static UINT8 NUM6[] = { 1, 2, 3, 4, 5, 7, 0xFF };
static UINT8 NUM7[] = { 5, 6, 7, 0xFF };
static UINT8 NUM8[] = { 1, 2, 3, 4, 5, 6, 7, 0xFF };
static UINT8 NUM9[] = { 1, 2, 4, 5, 6, 7, 0xFF };

static UINT8 *g_NUM[] = { NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9 };


typedef struct _tagPINBUF
{
	UINT8	index;
	UINT8	offset;
} PINBUF, *PPINBUF;

static PINBUF D2[] =	// 第0位
{
	{ 11, 7 },	// 第0位
	{ 11, 4 },	// 第1位
	{ 11, 2 },	// 第2位
	{ 11, 1 },	// 第3位
	{ 11, 3 },	// 第4位
	{ 11, 5 },	// 第5位
	{ 11, 6 },	// 第6位
	{ 11, 0 },	// 第7位
};

static PINBUF D3[] =	// 第1位
{
	{ 10, 5 },	// 第0位
	{ 10, 2 },	// 第1位
	{ 10, 0 },	// 第2位
	{  9, 7 },	// 第3位
	{ 10, 1 },	// 第4位
	{ 10, 3 },	// 第5位
	{ 10, 4 },	// 第6位
	{  9, 6 },	// 第7位
};

static PINBUF D4[] =	// 第2位
{
	{  9, 3 },	// 第0位
	{  9, 0 },	// 第1位
	{  8, 6 },	// 第2位
	{  8, 5 },	// 第3位
	{  8, 7 },	// 第4位
	{  9, 1 },	// 第5位
	{  9, 2 },	// 第6位
	{  8, 4 },	// 第7位
};

static PINBUF D5[] =	// 第3位
{
	{  8, 1 },	// 第0位
	{  7, 6 },	// 第1位
	{  7, 4 },	// 第2位
	{  7, 3 },	// 第3位
	{  7, 5 },	// 第4位
	{  7, 7 },	// 第5位
	{  8, 0 },	// 第6位
	{  7, 2 },	// 第7位
};

static PINBUF D6[] =	// 第4位
{
	{  6, 7 },	// 第0位
	{  6, 4 },	// 第1位
	{  6, 2 },	// 第2位
	{  6, 1 },	// 第3位
	{  6, 3 },	// 第4位
	{  6, 5 },	// 第5位
	{  6, 6 },	// 第6位
	{  6, 0 },	// 第7位
};

static PINBUF D7[] =	// 第5位
{
	{  5, 5 },	// 第0位
	{  5, 2 },	// 第1位
	{  5, 0 },	// 第2位
	{  4, 7 },	// 第3位
	{  5, 1 },	// 第4位
	{  5, 3 },	// 第5位
	{  5, 4 },	// 第6位
	{  4, 6 },	// 第7位
};

static PINBUF D8[] =	// 第6位
{
	{  4, 3 },	// 第0位
	{  4, 0 },	// 第1位
	{  3, 6 },	// 第2位
	{  3, 5 },	// 第3位
	{  3, 7 },	// 第4位
	{  4, 1 },	// 第5位
	{  4, 2 },	// 第6位
	{  3, 4 },	// 第7位
};

static PINBUF D9[] =	// 第7位
{
	{  3, 1 },	// 第0位
	{  2, 6 },	// 第1位
	{  2, 4 },	// 第2位
	{  2, 3 },	// 第3位
	{  2, 5 },	// 第4位
	{  2, 7 },	// 第5位
	{  3, 0 },	// 第6位
	{  2, 2 },	// 第7位
};

static PINBUF *DD[] = { D2, D3, D4, D5, D6, D7, D8, D9 };

static UINT8 g_Mask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

#define		D0OFST	15
unsigned char g_Send[] =
{
	0x00,	// 00/00 - WA - Wake Up Byte
	0x00,	// 01/01 - CAL - UART calibration Byte
	0x00,	// 02/02 - CAL - UART calibration Byte
	0x00,	// 03/03 - Buffer
	0x00,	// 04/04 - Buffer
	0xF4,	// 05/05 - CLA - The class byte.
	0x00,	// 06/06 -  - Dummy byte
	0x51,	// 07/07 - INS - The instruction type.
	0x00,	// 08/08 -  - Dummy byte
	0x48,	// 09/09 - Data Count - Number of the data byte counted from the next byte to Checksum byte ( totally 72 bytes )
	0x00,	// 10/0A -  - Dummy byte

	// Waveform time
	// 11/0B ~ 14/0E
	0x00, 0x00,		// First data mapping code of Waveform time
	0x00, 0x00,		// Second data mapping code of Waveform time

	// P_EPD_SD1_PIXEL
	// D0 - 15/0F ~ 18/12
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[0]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[0]
	
	// D1 - 19/13 ~ 22/16
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[1]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[1]
	
	// D2 - 23/17 ~ 26/1A
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[2]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[2]
	
	// D3 - 27/1B ~ 30/1E
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[3]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[3]
	
	// D4 - 31/1F ~ 34/22
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[4]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[4]
	
	// D5 - 35/23 ~ 38/26
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[5]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[5]
	
	// D6 - 39/27 ~ 42/2A
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[6]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[6]
	
	// D7 - 43/2B ~ 46/2E
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[7]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[7]
	
	// D8 - 47/2F ~ 50/32
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[8]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[8]
	
	// D9 - 51/33 ~ 54/36
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[9]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[9]
	
	// D10 - 55/37 ~ 58/3A
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[10]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[10]
	
	// D11 - 59/3B ~ 62/3E
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[11]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[11]
	
	// D12 - 63/3F ~ 66/42
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[12]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[12]
	
	// D13 - 67/43 ~ 70/46
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[13]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[13]
	
	// D14 - 71/47 ~ 74/4A
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[14]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[14]
	
	// 15 - 75/4B ~ 78/4E
	0x00, 0x00,		// First data mapping code of P_EPD_SD1_PIXEL[15]
	0x00, 0x00,		// Second data mapping code of P_EPD_SD1_PIXEL[15]

	// Checksum + 0xAA
	// 79/4F ~ 81/51
	0x00, 0x00,		// First data mapping code of [Checksum + 0xAA]
	0x00			// Second data mapping code of [Checksum + 0xAA]
};


//*************************************************************************
// 全局变量

//*************************************************************************
// 此代码模块中包含的函数的向前声明




//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_Num2Buff ( UINT8 *pBuff, PPINBUF pPinBuf, UINT8 num )
{
	UINT8	i;
	UINT8	nIndex, nOffset;

	for ( i = 0; g_NUM[num][i] != 0xFF; i++ )
	{	// 取数字7段码的点

		// 取数字
		nIndex = pPinBuf[g_NUM[num][i]].index;
		nOffset = pPinBuf[g_NUM[num][i]].offset;

		// 
		pBuff[nIndex] |= g_Mask[nOffset];
	}

	return 0;
}

//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_Dot2Buff ( UINT8 *pBuff, PPINBUF pPinBuf )
{
	UINT8	nIndex, nOffset;

	nIndex = pPinBuf[0].index;
	nOffset = pPinBuf[0].offset;

	pBuff[nIndex] |= g_Mask[nOffset];

	return 0;
}

//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_Symbol2Buff ( UINT8 *pBuff )
{
	if ( g_D1A ) pBuff[13] |= g_Mask[5];
	if ( g_D1B ) pBuff[13] |= g_Mask[6];
	if ( g_D10A ) pBuff[2] |= g_Mask[1];
	if ( g_D10B ) pBuff[2] |= g_Mask[0];

	return 0;
}

//*************************************************************************
//*************************************************************************
UINT8 * HED_EINK_MakeBuff ( UINT8 *pString )
{
	UINT8	*pBuff = NULL;
	UINT8	num, i, j = 0;

	pBuff = malloc ( 16 );
	memset ( pBuff, 0x00, 16 );

	num = strlen ( pString );

	for ( i = 0; i < num; i++ )
	{
		if ( isdigit ( pString[num - 1 - i] ) )
		{
			HED_EINK_Num2Buff ( pBuff, DD[j], (UINT8)(pString[num - 1 - i] - '0') );
			j++;
		}
		else if ( pString[num - 1 - i] == '.' )
		{
			HED_EINK_Dot2Buff ( pBuff, DD[j] );
		}
	}

	HED_EINK_Symbol2Buff ( pBuff );

	return pBuff;
}

#define		RIGHT(n)	(n & 0x0F)
#define		LEFT(n)		((n & 0xF0) >> 4)

static UINT8 LOOKUP[] =
{
	0x00, 0xD2, 0x54, 0x86,
	0x98, 0x4A, 0xCC, 0x1E,
	0xE0, 0x32, 0xB4, 0x66,
	0x78, 0xAA, 0x2C, 0xFE
};

//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_Mapping ( UINT8 *pMap, UINT8 *pPin )
{
	UINT8	i, lowhalf;

	for ( i = 0; i < 16; i++ )
	{
		lowhalf = RIGHT(pPin[i]);
		pMap[15 + (i * 4)] = LOOKUP[lowhalf];
	}

	for ( i = 0; i < 16; i++ )
	{
		lowhalf = LEFT(pPin[i]);
		pMap[17 + (i * 4)] = LOOKUP[lowhalf];
	}

	pMap[11] = 0x86;

	// 填写 CheckSum
	lowhalf = RIGHT(g_CheckSum);
	pMap[79] = LOOKUP[lowhalf];
	lowhalf = LEFT(g_CheckSum);
	pMap[81] = LOOKUP[lowhalf];

	return 0;
}

//*************************************************************************
//*************************************************************************
static void HED_EINK_Send ( LPUSERDATA lpUserData, UINT8 *pSendData )
{
#if 0
	BOOL		bResult;
	BYTE		OutputReport[256];
	DWORD		BytesWritten = 0;
	PUSBHID		pUsbHid;

	pUsbHid = lpUserData->sRunDev[lpUserData->nRunDevIndex].pVoid;

	// 如果设备未加载，则显示提示信息
	if ( lpUserData->nRunDevTotal == 0 )
	{
		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device not detected\n" );
		return;
	}

	// 加载将要传送的数据
	memset ( OutputReport, 0x00, 256 );
	OutputReport[0] = 0x00;		// 报告ID 必须为 0
	OutputReport[1] = 0x57;		// 数据长度(包括命令头)
	OutputReport[2] = 0xBF;
	OutputReport[3] = 0xAA;
	OutputReport[4] = 0x00;
	OutputReport[5] = 0x00;
	OutputReport[6] = 0x52;
	memcpy ( &OutputReport[7], pSendData, 82 );

	// 将数据写入设备
	bResult = WriteFile ( pUsbHid->hWrite, OutputReport, pUsbHid->sCapabilities.OutputReportByteLength, &BytesWritten, NULL );

	// 如写入成功，则尝试获取Report
	if ( bResult )
	{
		PostMessage ( lpUserData->hMainDlg, WM_READ_HID_REPORT, 0, 0 );
	}
#endif
	return;
}

//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_MakeSendData ( LPUSERDATA lpUserData )
{
//	UINT8	pString[] = "8.8.8.8.8.8.8.8.";
	UINT8	pString[20];
	UINT8	*pPinBuff = NULL;
	UINT8	pSendBuff[82] = { 0 };
	UINT8	nCheckSum = 0;
	UINT8	i = 0;

	GetDlgItemText ( lpUserData->hMainDlg, IDC_EDIT3, pString, 19 );

	pPinBuff = HED_EINK_MakeBuff ( pString );
	memcpy ( pSendBuff, g_Send, 82 );
	
	// checksum
	for ( i = 0; i < 16; i++ )
	{
		nCheckSum += pPinBuff[i];
	}
	g_CheckSum = nCheckSum + 0xAA + 0xF4 + 0x51 + 0x48 + 0x03;

	HED_EINK_Mapping ( pSendBuff, pPinBuff );
	HED_EINK_Send ( lpUserData, pSendBuff );

	return 0;
}



//*************************************************************************
// 函数: Dialog_OnHID_ReadReport
// 功能: ???
//*************************************************************************
LRESULT Dialog_OnHID_ReadReport ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
#if 0
	LPUSERDATA		lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	BOOL			bResult;
	BYTE			InputReport[256] = { 0 };
	DWORD			NumberOfBytesRead;
	DWORD			dwResult;
	PUSBHID			pUsbHid;
	
	pUsbHid = lpUserData->sRunDev[lpUserData->nRunDevIndex].pVoid;

	// 不加这句取不到Report
	//	Sleep ( 100 );
	// 如获取错误则再次发送WM_READ_HID_REPORT消息尝试
	//		PostMessage ( lpUserData->hMainDlg, WM_READ_HID_REPORT, 0, 0 );
	

	// 获取Report
	bResult = ReadFile ( pUsbHid->hRead, InputReport, pUsbHid->sCapabilities.InputReportByteLength, &NumberOfBytesRead, (LPOVERLAPPED) &pUsbHid->HIDOverlapped ); 

	// 读取数据出现错误时进行特殊处理
	if ( bResult == 0 )
	{
		// 读取错误信息
		dwResult = GetLastError ();

		// I/O端口繁忙
		if ( dwResult == ERROR_IO_PENDING )
		{
			// 需挂起等待
			dwResult = WaitForSingleObject ( pUsbHid->hEventObject, 1000 );

			// I/O端口正常
			if ( dwResult == WAIT_OBJECT_0 )
			{
				// 去读取数据的正常返回值
				GetOverlappedResult ( pUsbHid->hRead, &pUsbHid->HIDOverlapped, &NumberOfBytesRead, FALSE );
			}
			else
			{
				return FALSE;	// 等待超时
			}
		}
		else
		{
			return FALSE;	// 不明错误
		}
	}

	// 显示获取到得Report
//wait	SetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_STATUS, MAKEWORD(InputReport[4], InputReport[3]) );
#endif
	return TRUE;
}

//*************************************************************************
// 函数: Hid_UKey_Send
// 功能: ???
//*************************************************************************
static void Hid_UKey_Send ( LPUSERDATA lpUserData )
{
#if 0
	BOOL			bResult;
	BYTE			OutputReport[256];
	DWORD			BytesWritten = 0;
	PUSBHID			pUsbHid;
	
	pUsbHid = lpUserData->sRunDev[lpUserData->nRunDevIndex].pVoid;

	// 如果设备未加载，则显示提示信息
	if ( lpUserData->nRunDevTotal == 0 )
	{
		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device not detected\n" );
		return;
	}

	// 加载将要传送的数据
	memset ( OutputReport, 0x00, 256 );
	OutputReport[0] = 0x00;		// 报告ID 必须为 0
	OutputReport[1] = 0x01;		// ??? APDU收到这字节似乎不做处理
	OutputReport[2] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_CLA, 2 );
	OutputReport[3] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_INS, 2 );
	OutputReport[4] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P1,  2 );
	OutputReport[5] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P2,  2 );
	OutputReport[6] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P3,  2 );

	// 将数据写入设备
	bResult = WriteFile ( pUsbHid->hWrite, OutputReport, pUsbHid->sCapabilities.OutputReportByteLength, &BytesWritten, NULL );

	// 如写入成功，则尝试获取Report
	if ( bResult )
	{
		PostMessage ( lpUserData->hMainDlg, WM_READ_HID_REPORT, 0, 0 );
	}
#endif
	return;
}
// 
// //*************************************************************************
// // 函数: PrepareForOverlappedTransfer
// // 功能: ???
// //*************************************************************************
// void PrepareForOverlappedTransfer ( LPUSERDATA lpUserData )
// {
// 	LPDIALOGINFO	lpdi = &lpUserData->DialogInfo;
// 
// 	lpdi->ReadHandle = CreateFile 
// 		(lpdi->detailData->DevicePath, 
// 		GENERIC_READ, 
// 		FILE_SHARE_READ|FILE_SHARE_WRITE,
// 		(LPSECURITY_ATTRIBUTES)NULL, 
// 		OPEN_EXISTING, 
// 		FILE_FLAG_OVERLAPPED, 
// 		NULL);
// 
// 	if ( lpdi->hEventObject == 0 )
// 	{
// 		lpdi->hEventObject = CreateEvent ( NULL, TRUE, TRUE, "" );
// 
// 		lpdi->HIDOverlapped.hEvent = lpdi->hEventObject;
// 		lpdi->HIDOverlapped.Offset = 0;
// 		lpdi->HIDOverlapped.OffsetHigh = 0;
// 	}
// }
// 
// //*************************************************************************
// // 函数: RegisterForDeviceNotifications
// // 功能: ???
// //*************************************************************************
// void RegisterForDeviceNotifications ( LPUSERDATA lpUserData )
// {
// 	LPDIALOGINFO					lpdi = &lpUserData->DialogInfo;
// 
// 	DEV_BROADCAST_DEVICEINTERFACE	DevBroadcastDeviceInterface;
// 	HDEVNOTIFY						DeviceNotificationHandle;
// 	
// 	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
// 	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
// 	DevBroadcastDeviceInterface.dbcc_classguid = lpdi->sHidGuid;
// 	
// 	DeviceNotificationHandle =
// 		RegisterDeviceNotification ( lpUserData->hMainDlg, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE );
// 
// 	return;
// }
// 
// //*************************************************************************
// // 函数: GetDeviceCapabilities
// // 功能: ???
// //*************************************************************************
// void GetDeviceCapabilities ( LPUSERDATA lpUserData )
// {
// 	LPDIALOGINFO			lpdi = &lpUserData->DialogInfo;
// 	PHIDP_PREPARSED_DATA	PreparsedData;
// 
// 	HidD_GetPreparsedData ( lpdi->DeviceHandle, &PreparsedData );
// 
// 	HidP_GetCaps ( PreparsedData, &lpdi->Capabilities );
// 
// 	HidD_FreePreparsedData ( PreparsedData );
// 
// 	return;
// }
// 
// //*************************************************************************
// // 函数: Display_GUID
// // 功能: 显示HID设备的GUID信息
// //*************************************************************************
// static LRESULT Display_GUID ( LPUSERDATA lpUserData )
// {
// 	LPDIALOGINFO	lpdi = &lpUserData->DialogInfo;
// 	LPSTR			lpText = NULL;
// 	
// 	lpText = (LPSTR)malloc ( 256 );
// 	memset ( lpText, 0x00, 256 );
// 
// 	sprintf ( lpText,
// 			  "HID类的GUID标识为: %08X-%04X-%04X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
// 			  lpdi->sHidGuid.Data1,
// 			  lpdi->sHidGuid.Data2,
// 			  lpdi->sHidGuid.Data3,
// 			  lpdi->sHidGuid.Data4[0],
// 			  lpdi->sHidGuid.Data4[1],
// 			  lpdi->sHidGuid.Data4[2],
// 			  lpdi->sHidGuid.Data4[3],
// 			  lpdi->sHidGuid.Data4[4],
// 			  lpdi->sHidGuid.Data4[5],
// 			  lpdi->sHidGuid.Data4[6],
// 			  lpdi->sHidGuid.Data4[7] );
// 
// 	SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)lpText );
// 
// 	free ( lpText );
// 	return TRUE;
// }
// 
// //*************************************************************************
// // 函数: Hid_UKey_Connect
// // 功能: HID UKEY 设备连接
// //*************************************************************************
// static LRESULT Hid_UKey_Connect ( LPUSERDATA lpUserData )
// {
// 	LPDIALOGINFO				lpdi = &lpUserData->DialogInfo;
// 	SP_DEVICE_INTERFACE_DATA	devInfoData;
// 	int							MemberIndex = 0;
// 	BOOL						LastDevice = FALSE;
// 	LONG						Result;	
// 	HIDD_ATTRIBUTES				Attributes;
// 	DWORD						DeviceUsage;
// 
// 	lpdi->MyDeviceDetected = FALSE;
// 
// 	// 取得HID设备的GUID
// 	HidD_GetHidGuid ( &lpdi->sHidGuid );
// 	Display_GUID ( lpUserData );	// 显示HID设备的GUID信息
// 
// 	// 获取一个指定类别或全部类别的所有已安装设备的信息
// 	lpdi->hDevInfo = SetupDiGetClassDevs ( &lpdi->sHidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
// 	if ( INVALID_HANDLE_VALUE == lpdi->hDevInfo )
// 	{
// 		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"\n设置安装设备句柄失败!!!\n" );
// 		return FALSE;
// 	}
// 	SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"设置安装设备句柄成功。\n" );
// 
// 	// 查找USB HID类型的UKEY设备
// 	SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"正在查找USB HID类型的UKEY设备...\n" );
// 	devInfoData.cbSize = sizeof(devInfoData);
// 
// 	do
// 	{
// 		// 枚举设备，成功后返回设备的SP_DEVICE_INTERFACE_DATA型结构指针，否则枚举结束退出循环
// 		Result = SetupDiEnumDeviceInterfaces ( lpdi->hDevInfo, 0, &lpdi->sHidGuid, MemberIndex, &devInfoData );
// 
// 		if ( Result != 0 )
// 		{
// 			// 获取设备接口的详细信息，SetupDiGetDeviceInterfaceDetail函数需要调用2次
// 			// 1. 获得请求的缓冲大小并分配一个合适的缓冲
// 			Result = SetupDiGetDeviceInterfaceDetail ( lpdi->hDevInfo, &devInfoData, NULL, 0, &lpdi->Length, NULL );
// 			lpdi->detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(lpdi->Length);
// 			lpdi->detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
// 
// 			// 2. 并再次调用函数来获得接口细节
// 			Result = SetupDiGetDeviceInterfaceDetail ( lpdi->hDevInfo, &devInfoData, lpdi->detailData, lpdi->Length, &lpdi->Required, NULL );
// 			if ( Result == FALSE )
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"获取设备接口的详细信息时出错!!!\n" );
// 			}
// 			else
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"获取设备接口的详细信息成功。可以通过下面路径访问设备。\n" );
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)lpdi->detailData->DevicePath );
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"\n" );
// 			}
// 
// 			// 开放与设备的通信
// 			lpdi->DeviceHandle = CreateFile ( lpdi->detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
// 			if ( lpdi->DeviceHandle == INVALID_HANDLE_VALUE )
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"无法打开通信端口!!!\n" );
// 			}
// 
// 			Attributes.Size = sizeof(Attributes);
// 			Result = HidD_GetAttributes ( lpdi->DeviceHandle, &Attributes );
// 			lpdi->MyDeviceDetected = FALSE;
// 
// 			if ( Attributes.VendorID == lpdi->nVendorID )
// 			{
// 				if ( Attributes.ProductID == lpdi->nProductID )
// 				{
// 					int		len = 0;
// 
// 					lpdi->MyDeviceDetected = TRUE;
// 
// 					len = strlen ( lpdi->detailData->DevicePath );
// 					lpdi->MyDevicePathName = malloc ( len + 1 );
// 					memset ( lpdi->MyDevicePathName, 0x00, len + 1 );
// 					strcpy ( lpdi->MyDevicePathName, lpdi->detailData->DevicePath );
// 
// 					RegisterForDeviceNotifications ( lpUserData );
// 					GetDeviceCapabilities ( lpUserData );
// 
// 					DeviceUsage = (lpdi->Capabilities.UsagePage * 256) + lpdi->Capabilities.Usage;
// 
// 					lpdi->WriteHandle = CreateFile ( lpdi->detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL );
// 
// 					PrepareForOverlappedTransfer ( lpUserData );
// 				}
// 				else
// 				{
// 					CloseHandle ( lpdi->DeviceHandle );
// 				}
// 			}
// 			else
// 			{
// 				CloseHandle ( lpdi->DeviceHandle );
// 			}
// 
// 			free ( lpdi->detailData );
// 		}
// 		else
// 		{
// 			SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"未找到可用的UKEY设备!!!\n" );
// 			LastDevice = TRUE;
// 		}
// 
// 		MemberIndex = MemberIndex + 1;
// 	} while ((LastDevice == FALSE) && (lpdi->MyDeviceDetected == FALSE));
// 
// 	if ( lpdi->MyDeviceDetected == FALSE )
// 		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device not detected\n" );
// 	else
// 		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device detected\n" );
// 
// 	return 0;
// }

//*************************************************************************
// 函数: Dialog_OnCommand
// 功能: 处理WM_COMMAND消息
//*************************************************************************
LRESULT Dialog_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	WORD		wNotifyCode, wID;
	HWND		hwndCtrl;

	if ( !lpUserData )
		return TRUE;

	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier
	hwndCtrl	= (HWND)lParam;		// handle of control
	
	switch ( wNotifyCode )
	{
		DWORD	dwLen;
		
	case EN_SETFOCUS:
		{
			DWORD	dwStart, dwEnd, dwRet;
			
 			dwRet = SendMessage ( lpUserData->hwndRichEditScript, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd );
 			dwRet = SendMessage ( lpUserData->hwndRichEditScript, EM_LINEFROMCHAR, dwStart, 0 );
		}
		if ( hwndCtrl != lpUserData->hwndRichEditScript )
			PostMessage ( hwndCtrl, EM_SETSEL, 0, -1 );
		break;
		
	case EN_CHANGE:
		{
			dwLen = SendMessage ( hwndCtrl, WM_GETTEXTLENGTH, 0, 0 );
			
			if ( hwndCtrl == lpUserData->hwndEditVendorID )
			{
				if ( dwLen == 4 )
					SetFocus ( lpUserData->hwndEditProductID );
			}
			else if ( hwndCtrl == lpUserData->hwndEditProductID )
			{
				if ( dwLen == 4 )
					SetFocus ( lpUserData->hwndEditCLA );
			}
			else if ( hwndCtrl == lpUserData->hwndEditCLA )
			{
				if ( dwLen == 2 )
					SetFocus ( lpUserData->hwndEditINS );
			}
			else if ( hwndCtrl == lpUserData->hwndEditINS )
			{
				if ( dwLen == 2 )
					SetFocus ( lpUserData->hwndEditP1 );
			}
			else if ( hwndCtrl == lpUserData->hwndEditP1 )
			{
				if ( dwLen == 2 )
					SetFocus ( lpUserData->hwndEditP2 );
			}
			else if ( hwndCtrl == lpUserData->hwndEditP2 )
			{
				if ( dwLen == 2 )
					SetFocus ( lpUserData->hwndEditP3 );
			}
			else if ( hwndCtrl == lpUserData->hwndEditP3 )
			{
				if ( dwLen == 2 )
					SetFocus ( lpUserData->hwndEditSendData );
			}
		}
	}
				
	switch ( wID )
	{
	case IDC_BUTTON_RESET:
		lpUserData->DialogInfo.nVendorID = VENDOR_ID;
		lpUserData->DialogInfo.nProductID = PRODUCT_ID;
//		Hid_UKey_Connect ( lpUserData );
		break;
		
	case IDC_BUTTON_SEND:
		// Hid_UKey_Send ( lpUserData );
		HED_EINK_MakeSendData ( lpUserData );
		break;
	}
	
	return TRUE;
}

//*************************************************************************
// 函数: Dialog_Script_OnPaint
// 功能: 截获Script窗口的WM_PAINT消息
//*************************************************************************
static LRESULT ShowLineNum ( HWND hEdit, LPUSERDATA lpUserData )
{
	RECT		ClientRect;		// RichEdit的客户区大小
	HDC			hdcEdit;		// RichEdit的Dc（设备环境）
	HDC			hdcCpb;			// 与RichEdit兼容的Dc
	HBITMAP		hdcBmp;			// RichEdit兼容的位图dc
	int			CharHeight;		// 字符的高度
	int			chHeight;		// 字符的高度，常量 
	int			FirstLine;		// 文本框中的第一个可见行的行号。
	int			ClientHeight;	// RichEdit的客户区高度 
	int			LineCount;		// 文本的总行数 
	char		countBuf[10];	// 显示行号的缓冲区
	CHARFORMAT	CharFmt;		// RichEdit中的一个结构，用于获取字符的一系列信息，这里只用它来获取字符高度

	// 获取RichEdit的Dc
	hdcEdit = GetDC ( hEdit );

	// 获取RichEdit的客户区高度

	GetClientRect ( hEdit, &ClientRect);
	ClientHeight = ClientRect.bottom - ClientRect.top;

	// 创建与RichEdit兼容的Dc
	hdcCpb = CreateCompatibleDC ( hdcEdit );

	// 创建与RichEdit兼容的位图Dc，我们用来显示行号用的。
	hdcBmp = CreateCompatibleBitmap ( hdcEdit, 40, ClientHeight );

	// 将位图dc选入RichEdit环境中
	SelectObject ( hdcCpb, hdcBmp );
	SelectObject ( hdcCpb, lpUserData->hFont );

	// 填充显示行号dc的背景颜色。大家可以试试其它颜色
	FillRect ( hdcCpb, &ClientRect, CreateSolidBrush ( 0xC0C0C0 ) );
	SetBkMode( hdcCpb, TRANSPARENT );

	// 获取当前RichEdit文本中的总行数
	LineCount = SendMessage ( hEdit, EM_GETLINECOUNT, 0, 0 );

	RtlZeroMemory ( &CharFmt, sizeof(CharFmt) );
	CharFmt.cbSize = sizeof ( CharFmt );
	SendMessage ( hEdit, EM_GETCHARFORMAT, TRUE, (long)&CharFmt );

	// 获取字符高度，以英寸为单位，需转化为磅，只要除以20就得到磅尺寸。
	CharHeight = CharFmt.yHeight / 20; 
	chHeight = CharHeight;
	CharHeight = 3;

	// 设置显示行号的前景色
	SetTextColor ( hdcCpb, 0x000000 );

	// 获取文本框中第一个可见的行的行号，没有这个行号，显示不会跟着文本的滚动而滚动。
	FirstLine = SendMessage ( hEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
	FirstLine ++;

	// 在位图dc中循环输出行号
	while ( FirstLine <= LineCount )
	{
		TextOut ( hdcCpb, 1, CharHeight, countBuf, wsprintf ( countBuf, TEXT("%4u"), FirstLine++ ) );
		CharHeight += chHeight + 7;
		if ( CharHeight > ClientHeight )
			break;
	}

	// 将已"画好"的位图真正"贴"到RichEdit中
	BitBlt ( hdcEdit, 0, 0, 25, ClientHeight, hdcCpb, 0, 0, SRCCOPY );
	DeleteDC ( hdcCpb );
	ReleaseDC ( hEdit, hdcEdit );
	DeleteObject ( hdcBmp );

	return TRUE;
}

//*************************************************************************
// 函数: Dialog_Script_OnPaint
// 功能: 截获Script窗口的WM_PAINT消息
//*************************************************************************
static LRESULT Dialog_Script_OnPaint ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	PAINTSTRUCT	ps;
	HDC			hDC;

	CallWindowProc ( lpUserData->wndProcScript, hWnd, uMsg, wParam, lParam );
	hDC = BeginPaint ( hWnd, &ps );

	ShowLineNum ( hWnd, lpUserData );

	return EndPaint ( hWnd, &ps );
}

//*************************************************************************
// 函数: Dialog_ScriptProc
// 功能: 截获RichEdit控件类型Script窗口消息
//*************************************************************************
static LRESULT Dialog_ScriptProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
	switch ( uMsg )
	{
	case WM_PAINT:
		Dialog_Script_OnPaint ( hWnd, uMsg, wParam, lParam );
		break;

	default:
		{
			LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
			return CallWindowProc ( lpUserData->wndProcScript, hWnd, uMsg, wParam, lParam );
		}
	}

	return 0;
}

//*************************************************************************
// 函数: Dialog_Init
// 功能: 初始化对话框相关数据
//*************************************************************************
static LRESULT Dialog_Init ( HWND hWnd, LPUSERDATA lpUserData )
{
	lpUserData->hFont = GetWindowFont ( hWnd );

	// 设置APDU Script控件
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// 取已运行设备列表框句柄

	lpUserData->hwndRichEditScript = GetDlgItem ( hWnd, IDC_RICHEDIT_SCRIPT );
	SetWindowLong ( lpUserData->hwndRichEditScript, GWL_USERDATA, (LONG)lpUserData );
	SendMessage ( lpUserData->hwndRichEditScript, EM_SETMARGINS, EC_LEFTMARGIN, 26 );

	lpUserData->wndProcScript = (WNDPROC)SetWindowLong ( lpUserData->hwndRichEditScript, GWL_WNDPROC, (long)&Dialog_ScriptProc );

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnNewDevMount
// 功能: 挂接新设备
//*************************************************************************
LRESULT Dialog_OnDeviceChange ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
#if 0
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	UINT32		i, j;
	PUSBHID		pUsbHid = NULL;
	CHAR		szText[256];
	UINT32		nCount;

	if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam )
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		
		switch( pHdr->dbch_devicetype )
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;

			for ( i = 0; i < lpUserData->nRunDevTotal; i++ )
			{
				if ( lpUserData->sRunDev[i].nType == DEV_TYPE_USBHID )
				{
					pUsbHid = lpUserData->sRunDev[i].pVoid;
					if ( stricmp ( pDevInf->dbcc_name, pUsbHid->szDevPath ) == 0 )
					{
						if ( DBT_DEVICEARRIVAL != wParam )
						{

							nCount = SendMessage ( lpUserData->hComboNewDev, CB_GETCOUNT, 0, 0 );
							for ( j = 0; j < nCount; j++ )
							{
								SendMessage ( lpUserData->hComboNewDev, CB_GETLBTEXT, j, (LPARAM)szText );
								if ( strcmp ( szText, pUsbHid->sVPID.szDesc ) == 0 )
								{
									// 卸载设备
									SendMessage ( lpUserData->hComboNewDev, CB_DELETESTRING, j, 0 );
									SendMessage ( lpUserData->hComboNewDev, CB_SETCURSEL, 0, 0 ) ;

									CloseHandle ( pUsbHid->hRead );
									CloseHandle ( pUsbHid->hWrite );
									MacroFree ( lpUserData->sRunDev[i].pVoid );
									lpUserData->sRunDev[i].nType = 0;
									if ( lpUserData->nRunDevIndex == i )
										lpUserData->nRunDevIndex = 0;
									lpUserData->nRunDevTotal --;

									return 0;
								}
							}
						}
					}
				}
			}
		}
	}
#endif
	return 0;
}

//*************************************************************************
// 函数: Dialog_OnNewDevMount
// 功能: 挂接新设备
//*************************************************************************
LRESULT Dialog_OnNewDevMount ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
#if 0
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );

	if ( lpUserData->sRunDev[lpUserData->nRunDevIndex].nType == DEV_TYPE_USBHID )
	{
		PUSBHID		pUsbHid = NULL;

		pUsbHid = lpUserData->sRunDev[lpUserData->nRunDevIndex].pVoid;
		SendMessage ( lpUserData->hComboNewDev, CB_ADDSTRING, 0, (LPARAM)pUsbHid->sVPID.szDesc );
		SendMessage ( lpUserData->hComboNewDev, CB_SETCURSEL, 0, 0 ) ;
	}
#endif
	return 0;
}

//*************************************************************************
// 函数: Dialog_InitTabPage
// 功能: 初始化DIALOG控件
//*************************************************************************
LRESULT Dialog_InitTabPage ( HWND hWnd, LPUSERDATA lpUserData )
{
	static LPSTR tabnames[]= {"Tab Page 1", "Tab Page 2", "Tab Page 3", "Tab Page 4", 0};
	static LPSTR dlgnames[]= {MAKEINTRESOURCE(IDD_DIALOG_PAGE1),
							  MAKEINTRESOURCE(IDD_DIALOG_PAGE2),
							  MAKEINTRESOURCE(IDD_DIALOG_PAGE3),
							  MAKEINTRESOURCE(IDD_DIALOG_PAGE4),0};

	New_TabControl( &TabCtrl_1, // address of TabControl struct
					GetDlgItem(hWnd, IDC_TAB), // handle to tab control
					tabnames, // text for each tab
					dlgnames, // dialog id's of each tab page dialog
					&FormMain_DlgProc, // address of main windows proc
					&TabCtrl1_TabPages_OnSize, // address of size function
					TRUE); // stretch tab page to fit tab ctrl

	return TRUE;
}

//*************************************************************************
// 函数: Dialog_OnInitDialog
// 功能: 初始化DIALOG控件
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );

	if ( lpUserData == NULL )
		return TRUE;

	Dialog_InitTabPage ( hWnd, lpUserData );

	// 初始化对话框相关数据
	g_D1A = 1;
	g_D1B = 1;
	g_D10A = 1;
	g_D10B = 1;
	Dialog_Init ( hWnd, lpUserData );

//wait	DeviceManager ( lpUserData );

	// 置控件预设值
//	SetDlgItemText_Word ( hWnd, IDC_EDIT_VENDOR_ID, lpUserData->DialogInfo.nVendorID );
//	SetDlgItemText_Word ( hWnd, IDC_EDIT_PRODUCT_ID, lpUserData->DialogInfo.nProductID );

	return TRUE;
}


//***************************************************************************
// END
//***************************************************************************
