//*************************************************************************
// ���̣�ģ̬�Ի�����ģ��
// �ļ���Dialog.c
// ���ݣ�???
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
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
// �궨��

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

static PINBUF D2[] =	// ��0λ
{
	{ 11, 7 },	// ��0λ
	{ 11, 4 },	// ��1λ
	{ 11, 2 },	// ��2λ
	{ 11, 1 },	// ��3λ
	{ 11, 3 },	// ��4λ
	{ 11, 5 },	// ��5λ
	{ 11, 6 },	// ��6λ
	{ 11, 0 },	// ��7λ
};

static PINBUF D3[] =	// ��1λ
{
	{ 10, 5 },	// ��0λ
	{ 10, 2 },	// ��1λ
	{ 10, 0 },	// ��2λ
	{  9, 7 },	// ��3λ
	{ 10, 1 },	// ��4λ
	{ 10, 3 },	// ��5λ
	{ 10, 4 },	// ��6λ
	{  9, 6 },	// ��7λ
};

static PINBUF D4[] =	// ��2λ
{
	{  9, 3 },	// ��0λ
	{  9, 0 },	// ��1λ
	{  8, 6 },	// ��2λ
	{  8, 5 },	// ��3λ
	{  8, 7 },	// ��4λ
	{  9, 1 },	// ��5λ
	{  9, 2 },	// ��6λ
	{  8, 4 },	// ��7λ
};

static PINBUF D5[] =	// ��3λ
{
	{  8, 1 },	// ��0λ
	{  7, 6 },	// ��1λ
	{  7, 4 },	// ��2λ
	{  7, 3 },	// ��3λ
	{  7, 5 },	// ��4λ
	{  7, 7 },	// ��5λ
	{  8, 0 },	// ��6λ
	{  7, 2 },	// ��7λ
};

static PINBUF D6[] =	// ��4λ
{
	{  6, 7 },	// ��0λ
	{  6, 4 },	// ��1λ
	{  6, 2 },	// ��2λ
	{  6, 1 },	// ��3λ
	{  6, 3 },	// ��4λ
	{  6, 5 },	// ��5λ
	{  6, 6 },	// ��6λ
	{  6, 0 },	// ��7λ
};

static PINBUF D7[] =	// ��5λ
{
	{  5, 5 },	// ��0λ
	{  5, 2 },	// ��1λ
	{  5, 0 },	// ��2λ
	{  4, 7 },	// ��3λ
	{  5, 1 },	// ��4λ
	{  5, 3 },	// ��5λ
	{  5, 4 },	// ��6λ
	{  4, 6 },	// ��7λ
};

static PINBUF D8[] =	// ��6λ
{
	{  4, 3 },	// ��0λ
	{  4, 0 },	// ��1λ
	{  3, 6 },	// ��2λ
	{  3, 5 },	// ��3λ
	{  3, 7 },	// ��4λ
	{  4, 1 },	// ��5λ
	{  4, 2 },	// ��6λ
	{  3, 4 },	// ��7λ
};

static PINBUF D9[] =	// ��7λ
{
	{  3, 1 },	// ��0λ
	{  2, 6 },	// ��1λ
	{  2, 4 },	// ��2λ
	{  2, 3 },	// ��3λ
	{  2, 5 },	// ��4λ
	{  2, 7 },	// ��5λ
	{  3, 0 },	// ��6λ
	{  2, 2 },	// ��7λ
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
// ȫ�ֱ���

//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����




//*************************************************************************
//*************************************************************************
LRESULT HED_EINK_Num2Buff ( UINT8 *pBuff, PPINBUF pPinBuf, UINT8 num )
{
	UINT8	i;
	UINT8	nIndex, nOffset;

	for ( i = 0; g_NUM[num][i] != 0xFF; i++ )
	{	// ȡ����7����ĵ�

		// ȡ����
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

	// ��д CheckSum
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

	// ����豸δ���أ�����ʾ��ʾ��Ϣ
	if ( lpUserData->nRunDevTotal == 0 )
	{
		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device not detected\n" );
		return;
	}

	// ���ؽ�Ҫ���͵�����
	memset ( OutputReport, 0x00, 256 );
	OutputReport[0] = 0x00;		// ����ID ����Ϊ 0
	OutputReport[1] = 0x57;		// ���ݳ���(��������ͷ)
	OutputReport[2] = 0xBF;
	OutputReport[3] = 0xAA;
	OutputReport[4] = 0x00;
	OutputReport[5] = 0x00;
	OutputReport[6] = 0x52;
	memcpy ( &OutputReport[7], pSendData, 82 );

	// ������д���豸
	bResult = WriteFile ( pUsbHid->hWrite, OutputReport, pUsbHid->sCapabilities.OutputReportByteLength, &BytesWritten, NULL );

	// ��д��ɹ������Ի�ȡReport
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
// ����: Dialog_OnHID_ReadReport
// ����: ???
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

	// �������ȡ����Report
	//	Sleep ( 100 );
	// ���ȡ�������ٴη���WM_READ_HID_REPORT��Ϣ����
	//		PostMessage ( lpUserData->hMainDlg, WM_READ_HID_REPORT, 0, 0 );
	

	// ��ȡReport
	bResult = ReadFile ( pUsbHid->hRead, InputReport, pUsbHid->sCapabilities.InputReportByteLength, &NumberOfBytesRead, (LPOVERLAPPED) &pUsbHid->HIDOverlapped ); 

	// ��ȡ���ݳ��ִ���ʱ�������⴦��
	if ( bResult == 0 )
	{
		// ��ȡ������Ϣ
		dwResult = GetLastError ();

		// I/O�˿ڷ�æ
		if ( dwResult == ERROR_IO_PENDING )
		{
			// �����ȴ�
			dwResult = WaitForSingleObject ( pUsbHid->hEventObject, 1000 );

			// I/O�˿�����
			if ( dwResult == WAIT_OBJECT_0 )
			{
				// ȥ��ȡ���ݵ���������ֵ
				GetOverlappedResult ( pUsbHid->hRead, &pUsbHid->HIDOverlapped, &NumberOfBytesRead, FALSE );
			}
			else
			{
				return FALSE;	// �ȴ���ʱ
			}
		}
		else
		{
			return FALSE;	// ��������
		}
	}

	// ��ʾ��ȡ����Report
//wait	SetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_STATUS, MAKEWORD(InputReport[4], InputReport[3]) );
#endif
	return TRUE;
}

//*************************************************************************
// ����: Hid_UKey_Send
// ����: ???
//*************************************************************************
static void Hid_UKey_Send ( LPUSERDATA lpUserData )
{
#if 0
	BOOL			bResult;
	BYTE			OutputReport[256];
	DWORD			BytesWritten = 0;
	PUSBHID			pUsbHid;
	
	pUsbHid = lpUserData->sRunDev[lpUserData->nRunDevIndex].pVoid;

	// ����豸δ���أ�����ʾ��ʾ��Ϣ
	if ( lpUserData->nRunDevTotal == 0 )
	{
		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"Device not detected\n" );
		return;
	}

	// ���ؽ�Ҫ���͵�����
	memset ( OutputReport, 0x00, 256 );
	OutputReport[0] = 0x00;		// ����ID ����Ϊ 0
	OutputReport[1] = 0x01;		// ??? APDU�յ����ֽ��ƺ���������
	OutputReport[2] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_CLA, 2 );
	OutputReport[3] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_INS, 2 );
	OutputReport[4] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P1,  2 );
	OutputReport[5] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P2,  2 );
	OutputReport[6] = (BYTE)GetDlgItemText_Word ( lpUserData->hMainDlg, IDC_EDIT_P3,  2 );

	// ������д���豸
	bResult = WriteFile ( pUsbHid->hWrite, OutputReport, pUsbHid->sCapabilities.OutputReportByteLength, &BytesWritten, NULL );

	// ��д��ɹ������Ի�ȡReport
	if ( bResult )
	{
		PostMessage ( lpUserData->hMainDlg, WM_READ_HID_REPORT, 0, 0 );
	}
#endif
	return;
}
// 
// //*************************************************************************
// // ����: PrepareForOverlappedTransfer
// // ����: ???
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
// // ����: RegisterForDeviceNotifications
// // ����: ???
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
// // ����: GetDeviceCapabilities
// // ����: ???
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
// // ����: Display_GUID
// // ����: ��ʾHID�豸��GUID��Ϣ
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
// 			  "HID���GUID��ʶΪ: %08X-%04X-%04X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
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
// // ����: Hid_UKey_Connect
// // ����: HID UKEY �豸����
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
// 	// ȡ��HID�豸��GUID
// 	HidD_GetHidGuid ( &lpdi->sHidGuid );
// 	Display_GUID ( lpUserData );	// ��ʾHID�豸��GUID��Ϣ
// 
// 	// ��ȡһ��ָ������ȫ�����������Ѱ�װ�豸����Ϣ
// 	lpdi->hDevInfo = SetupDiGetClassDevs ( &lpdi->sHidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
// 	if ( INVALID_HANDLE_VALUE == lpdi->hDevInfo )
// 	{
// 		SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"\n���ð�װ�豸���ʧ��!!!\n" );
// 		return FALSE;
// 	}
// 	SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"���ð�װ�豸����ɹ���\n" );
// 
// 	// ����USB HID���͵�UKEY�豸
// 	SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"���ڲ���USB HID���͵�UKEY�豸...\n" );
// 	devInfoData.cbSize = sizeof(devInfoData);
// 
// 	do
// 	{
// 		// ö���豸���ɹ��󷵻��豸��SP_DEVICE_INTERFACE_DATA�ͽṹָ�룬����ö�ٽ����˳�ѭ��
// 		Result = SetupDiEnumDeviceInterfaces ( lpdi->hDevInfo, 0, &lpdi->sHidGuid, MemberIndex, &devInfoData );
// 
// 		if ( Result != 0 )
// 		{
// 			// ��ȡ�豸�ӿڵ���ϸ��Ϣ��SetupDiGetDeviceInterfaceDetail������Ҫ����2��
// 			// 1. �������Ļ����С������һ�����ʵĻ���
// 			Result = SetupDiGetDeviceInterfaceDetail ( lpdi->hDevInfo, &devInfoData, NULL, 0, &lpdi->Length, NULL );
// 			lpdi->detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(lpdi->Length);
// 			lpdi->detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
// 
// 			// 2. ���ٴε��ú�������ýӿ�ϸ��
// 			Result = SetupDiGetDeviceInterfaceDetail ( lpdi->hDevInfo, &devInfoData, lpdi->detailData, lpdi->Length, &lpdi->Required, NULL );
// 			if ( Result == FALSE )
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"��ȡ�豸�ӿڵ���ϸ��Ϣʱ����!!!\n" );
// 			}
// 			else
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"��ȡ�豸�ӿڵ���ϸ��Ϣ�ɹ�������ͨ������·�������豸��\n" );
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)lpdi->detailData->DevicePath );
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"\n" );
// 			}
// 
// 			// �������豸��ͨ��
// 			lpdi->DeviceHandle = CreateFile ( lpdi->detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
// 			if ( lpdi->DeviceHandle == INVALID_HANDLE_VALUE )
// 			{
// 				SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"�޷���ͨ�Ŷ˿�!!!\n" );
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
// 			SendMessage ( lpUserData->hwndRichEditScript, EM_REPLACESEL, (WPARAM)0, (LPARAM)"δ�ҵ����õ�UKEY�豸!!!\n" );
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
// ����: Dialog_OnCommand
// ����: ����WM_COMMAND��Ϣ
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
// ����: Dialog_Script_OnPaint
// ����: �ػ�Script���ڵ�WM_PAINT��Ϣ
//*************************************************************************
static LRESULT ShowLineNum ( HWND hEdit, LPUSERDATA lpUserData )
{
	RECT		ClientRect;		// RichEdit�Ŀͻ�����С
	HDC			hdcEdit;		// RichEdit��Dc���豸������
	HDC			hdcCpb;			// ��RichEdit���ݵ�Dc
	HBITMAP		hdcBmp;			// RichEdit���ݵ�λͼdc
	int			CharHeight;		// �ַ��ĸ߶�
	int			chHeight;		// �ַ��ĸ߶ȣ����� 
	int			FirstLine;		// �ı����еĵ�һ���ɼ��е��кš�
	int			ClientHeight;	// RichEdit�Ŀͻ����߶� 
	int			LineCount;		// �ı��������� 
	char		countBuf[10];	// ��ʾ�кŵĻ�����
	CHARFORMAT	CharFmt;		// RichEdit�е�һ���ṹ�����ڻ�ȡ�ַ���һϵ����Ϣ������ֻ��������ȡ�ַ��߶�

	// ��ȡRichEdit��Dc
	hdcEdit = GetDC ( hEdit );

	// ��ȡRichEdit�Ŀͻ����߶�

	GetClientRect ( hEdit, &ClientRect);
	ClientHeight = ClientRect.bottom - ClientRect.top;

	// ������RichEdit���ݵ�Dc
	hdcCpb = CreateCompatibleDC ( hdcEdit );

	// ������RichEdit���ݵ�λͼDc������������ʾ�к��õġ�
	hdcBmp = CreateCompatibleBitmap ( hdcEdit, 40, ClientHeight );

	// ��λͼdcѡ��RichEdit������
	SelectObject ( hdcCpb, hdcBmp );
	SelectObject ( hdcCpb, lpUserData->hFont );

	// �����ʾ�к�dc�ı�����ɫ����ҿ�������������ɫ
	FillRect ( hdcCpb, &ClientRect, CreateSolidBrush ( 0xC0C0C0 ) );
	SetBkMode( hdcCpb, TRANSPARENT );

	// ��ȡ��ǰRichEdit�ı��е�������
	LineCount = SendMessage ( hEdit, EM_GETLINECOUNT, 0, 0 );

	RtlZeroMemory ( &CharFmt, sizeof(CharFmt) );
	CharFmt.cbSize = sizeof ( CharFmt );
	SendMessage ( hEdit, EM_GETCHARFORMAT, TRUE, (long)&CharFmt );

	// ��ȡ�ַ��߶ȣ���Ӣ��Ϊ��λ����ת��Ϊ����ֻҪ����20�͵õ����ߴ硣
	CharHeight = CharFmt.yHeight / 20; 
	chHeight = CharHeight;
	CharHeight = 3;

	// ������ʾ�кŵ�ǰ��ɫ
	SetTextColor ( hdcCpb, 0x000000 );

	// ��ȡ�ı����е�һ���ɼ����е��кţ�û������кţ���ʾ��������ı��Ĺ�����������
	FirstLine = SendMessage ( hEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
	FirstLine ++;

	// ��λͼdc��ѭ������к�
	while ( FirstLine <= LineCount )
	{
		TextOut ( hdcCpb, 1, CharHeight, countBuf, wsprintf ( countBuf, TEXT("%4u"), FirstLine++ ) );
		CharHeight += chHeight + 7;
		if ( CharHeight > ClientHeight )
			break;
	}

	// ����"����"��λͼ����"��"��RichEdit��
	BitBlt ( hdcEdit, 0, 0, 25, ClientHeight, hdcCpb, 0, 0, SRCCOPY );
	DeleteDC ( hdcCpb );
	ReleaseDC ( hEdit, hdcEdit );
	DeleteObject ( hdcBmp );

	return TRUE;
}

//*************************************************************************
// ����: Dialog_Script_OnPaint
// ����: �ػ�Script���ڵ�WM_PAINT��Ϣ
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
// ����: Dialog_ScriptProc
// ����: �ػ�RichEdit�ؼ�����Script������Ϣ
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
// ����: Dialog_Init
// ����: ��ʼ���Ի����������
//*************************************************************************
static LRESULT Dialog_Init ( HWND hWnd, LPUSERDATA lpUserData )
{
	lpUserData->hFont = GetWindowFont ( hWnd );

	// ����APDU Script�ؼ�
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// ȡ�������豸�б����

	lpUserData->hwndRichEditScript = GetDlgItem ( hWnd, IDC_RICHEDIT_SCRIPT );
	SetWindowLong ( lpUserData->hwndRichEditScript, GWL_USERDATA, (LONG)lpUserData );
	SendMessage ( lpUserData->hwndRichEditScript, EM_SETMARGINS, EC_LEFTMARGIN, 26 );

	lpUserData->wndProcScript = (WNDPROC)SetWindowLong ( lpUserData->hwndRichEditScript, GWL_WNDPROC, (long)&Dialog_ScriptProc );

	return 0;
}

//*************************************************************************
// ����: Dialog_OnNewDevMount
// ����: �ҽ����豸
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
									// ж���豸
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
// ����: Dialog_OnNewDevMount
// ����: �ҽ����豸
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
// ����: Dialog_InitTabPage
// ����: ��ʼ��DIALOG�ؼ�
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
// ����: Dialog_OnInitDialog
// ����: ��ʼ��DIALOG�ؼ�
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );

	if ( lpUserData == NULL )
		return TRUE;

	Dialog_InitTabPage ( hWnd, lpUserData );

	// ��ʼ���Ի����������
	g_D1A = 1;
	g_D1B = 1;
	g_D10A = 1;
	g_D10B = 1;
	Dialog_Init ( hWnd, lpUserData );

//wait	DeviceManager ( lpUserData );

	// �ÿؼ�Ԥ��ֵ
//	SetDlgItemText_Word ( hWnd, IDC_EDIT_VENDOR_ID, lpUserData->DialogInfo.nVendorID );
//	SetDlgItemText_Word ( hWnd, IDC_EDIT_PRODUCT_ID, lpUserData->DialogInfo.nProductID );

	return TRUE;
}


//***************************************************************************
// END
//***************************************************************************
