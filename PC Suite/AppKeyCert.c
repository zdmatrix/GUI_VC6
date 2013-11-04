//*************************************************************************
// 文件：AppKeyCert.c
// 内容：演示公私钥与证书
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"
#include "APDU.h"

#include "Devices.h"


//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量


//*************************************************************************
// 此代码模块中包含的函数的向前声明


//*************************************************************************
//*************************************************************************

typedef struct _tagPRIKEY
{
	BYTE	pri_d[128];
	BYTE	pri_p[64];
	BYTE	pri_q[64];
	BYTE	pri_dp[64];
	BYTE	pri_dq[64];
	BYTE	pri_qInv[64];
} PRIKEY, *PPRIKEY;

//*************************************************************************
// 函数: AppKeyCert_MakeEDCH
// 功能: 
//*************************************************************************
LRESULT AppKeyCert_MakeEDCH ( LPUSERDATA lpUserData )
{
	BYTE	OutputReport[256] = { 0 };
	BYTE	InputReport[256] = { 0 };
	PCHAR	pString = NULL;
	WORD	wResult;

	//*************************************************************************
	//*************************************************************************
	FILE	*fh;
	int		nLen, nLen2;
	PPRIKEY	lpBuff = NULL;
//	int		i;

	fh = fopen ( "PriKey.1", "rb" );
	fseek ( fh, 0, SEEK_END );
	nLen = ftell ( fh );
	MacroAlloc ( lpBuff, nLen );
	fseek ( fh, 0, SEEK_SET );
	nLen2 = fread ( (void *)lpBuff, 1, nLen, fh );
	fclose ( fh );

	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 1, 64, (LPBYTE)lpBuff->pri_d );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 2, 64, (LPBYTE)&lpBuff->pri_d[64] );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 3, 64, (LPBYTE)lpBuff->pri_p );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 4, 64, (LPBYTE)lpBuff->pri_q );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 5, 64, (LPBYTE)lpBuff->pri_dp );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 6, 64, (LPBYTE)lpBuff->pri_dq );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, 7, 64, (LPBYTE)lpBuff->pri_qInv );
	MacroFree ( lpBuff );

	fh = fopen ( "tocard.bin", "rb" );
	fseek ( fh, 0, SEEK_END );
	nLen = ftell ( fh );
	fseek ( fh, 0, SEEK_SET );
	nLen2 = fread ( (void *)&OutputReport[4], 1, nLen, fh );
//	nLen2 = fread ( (void *)InputReport, 1, nLen, fh );
	fclose ( fh );

//	for ( i = 0; i < 128; i++ )
//		OutputReport[4 + i] = InputReport[127 - i];

	OutputReport[0] = 0x05;
	OutputReport[1] = 0x00;
	OutputReport[2] = 0x00;
	wResult = APDU_Command ( lpUserData, APDU_GSM_UPDATABINARY, 0x00, 0x84, OutputReport );
	if ( wResult != 0x9000 )
		return -1;
	
	// 读取卡端RSA签名数据
	wResult = APDU_Command ( lpUserData, APDU_GSM_READBINARY, 0x00, 131, InputReport );
	if ( wResult != 0x9000 )
		return -1;

//	for ( i = 0; i < 128; i++ )
//		OutputReport[4 + i] = InputReport[130 - i];

	fh = fopen ( "out.bin", "wb+" );
	fwrite ( (void *)&InputReport[3], 1, 128, fh );
	fclose ( fh );

	//*************************************************************************
	//*************************************************************************

	// 卡端生成临时公私钥
	OutputReport[0] = 0x01;
	OutputReport[1] = 0x00;
	OutputReport[2] = 0x00;
	wResult = APDU_Command ( lpUserData, APDU_GSM_UPDATABINARY, 0x00, 0x03, OutputReport );
	if ( wResult != 0x9000 )
		return -1;

	// 读取卡端临时公钥
	wResult = APDU_Command ( lpUserData, APDU_GSM_READBINARY, 0x00, 0x33, InputReport );
	if ( wResult != 0x9000 )
		return -1;
	MacroAlloc ( pString, 256 );
	pString = Global_Hex2String ( &InputReport[3], InputReport[2] );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[0], IDC_EDIT_EDCH, pString );
	MacroFree ( pString );

	return 0;
}

//*************************************************************************
// 函数: AppKeyCert_MakeSign
// 功能: 
//*************************************************************************
LRESULT AppKeyCert_MakeSign ( LPUSERDATA lpUserData )
{
	BYTE	OutputReport[256] = { 0 };
	BYTE	InputReport[256] = { 0 };
	PCHAR	pString = NULL;
	WORD	wResult;

	// 取用户输入数据然后签名
	GetDlgItemText ( lpUserData->sTabCtrl.hTabPages[0], IDC_EDIT_SIGNINPUT, &OutputReport[3], 32 );
	OutputReport[0] = 0x03;
	OutputReport[1] = 0x00;
	OutputReport[2] = 0x20;
	wResult = APDU_Command ( lpUserData, APDU_GSM_UPDATABINARY, 0x00, 0x23, InputReport );
	if ( wResult != 0x9000 )
		return -1;

	// 读取签名结果
	wResult = APDU_Command ( lpUserData, APDU_GSM_READBINARY, 0x00, 0x33, InputReport );
	if ( wResult != 0x9000 )
		return -1;
	pString = Global_Hex2String ( &InputReport[3], InputReport[2] );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[0], IDC_EDIT_SIGNOUTPUT, pString );
	MacroFree ( pString );

	return 0;
}

//*************************************************************************
// 函数: AppKeyCert_OnCommand
// 功能: 
//*************************************************************************
LRESULT AppKeyCert_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

	// 确保私有数据的正确性
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	// 确保有设备联接
	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( lpDuLNode && !GDEVOP.GetNumber ( lpDuLNode->lpData ) ) return -1;

	// OnCommand消息的细节
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier

	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// 按键点击消息
		if ( wID == IDC_BUTTON_MAKEEDCH )
			AppKeyCert_MakeEDCH ( lpUserData );
		else if ( wID == IDC_BUTTON_MAKESIGN )
			AppKeyCert_MakeSign ( lpUserData );
		else
			return -1;
		break;

	case CBN_SELCHANGE:
		// ComboBox改变消息
		if ( wID == IDC_COMBO_RUNDEV )
			GDEVOP.SetIndex ( lpDuLNode->lpData, SendMessage ( (HWND)lParam, CB_GETCURSEL, 0, 0 ) );
		break;

	default:
		return -1;
	}

	return TRUE;
}


//***************************************************************************
// END
//***************************************************************************
