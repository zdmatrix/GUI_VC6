//*************************************************************************
// 文件：AppCash.c
// 内容：演示现金余额
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

//*************************************************************************
// 函数: AppCash_InitBalance
// 功能: 初始化现金余额
//*************************************************************************
WORD AppCash_InitBalance ( LPUSERDATA lpUserData, WORD wBalance )
{
	WORD	wZero = 0x00;
	WORD	wResult;

	if ( wBalance == 0xFFFF )
	{
		wBalance = 0;
		wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_CASH, 0x02, (LPBYTE)&wBalance );
		if ( wResult != 0x9000 )
			return 0;
	}

	return wBalance;
}

//*************************************************************************
// 函数: AppCash_SubCash ()
// 功能: 消费现金
//*************************************************************************
LRESULT AppCash_SubCash ( LPUSERDATA lpUserData, UINT32 nCash )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	MacroAlloc ( pString, 256 );

	// ***************   取余额   *****************************************
	// 加载将要传送的数据
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;
	
	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	if ( wBalance < nCash )
	{
		MessageBox ( lpUserData->hMainDlg, "卡内现金不足，消费失败！", "错误", MB_OK );
		return -1;
	}
	wBalance -= nCash;
	
	// ***************   回写余额   *****************************************
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	IOBuff[0] = HIBYTE(wBalance);
	IOBuff[1] = LOBYTE(wBalance);
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   显示添加余额   *****************************************
	sprintf ( pString, "-%d", nCash );
	
	// EInk显示
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   可视卡按键等待显示余额   *****************************************
	// 加载将要传送的数据
	wResult = APDU_Command ( lpUserData, APDU_UKEY_STARTKEY, UKEY_CASH, 0x00, NULL );
	if ( wResult != 0x9000 )
		return -1;

	return TRUE;
}

//*************************************************************************
// 函数: AppCash_AddCash ()
// 功能: 添加现金
//*************************************************************************
LRESULT AppCash_AddCash ( LPUSERDATA lpUserData, UINT32 nCash )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	MacroAlloc ( pString, 256 );

	// ***************   取余额   *****************************************
	// 加载将要传送的数据
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	if ( (wBalance + nCash) >= 0xFFFE )
	{
		MessageBox ( lpUserData->hMainDlg, "金额超过最高限额，金额添加失败！", "错误", MB_OK );
		return -1;
	}
	wBalance += nCash;

	// ***************   回写余额   *****************************************
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	IOBuff[0] = HIBYTE(wBalance);
	IOBuff[1] = LOBYTE(wBalance);
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   显示添加余额   *****************************************
	sprintf ( pString, "+%d", nCash );
	
	// EInk显示
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   可视卡按键等待显示余额   *****************************************
	// 加载将要传送的数据
	wResult = APDU_Command ( lpUserData, APDU_UKEY_STARTKEY, UKEY_CASH, 0x00, NULL );
	if ( wResult != 0x9000 )
		return -1;

	return TRUE;
}

//*************************************************************************
// 函数: AppCash_OnCommand ()
// 功能: 按键处理
//*************************************************************************
LRESULT AppCash_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
	if ( !lpDuLNode )
		return -1;
	if ( !GDEVOP.GetNumber ( lpDuLNode->lpData ) ) return -1;
	
	// OnCommand消息的细节
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier

	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// 按键点击消息
		if ( wID == IDC_BUTTON_ADDHUNDRED )
			AppCash_AddCash ( lpUserData, 100 );
		else if ( wID == IDC_BUTTON_ADDTHOUSAND )
			AppCash_AddCash ( lpUserData, 1000 );
		else if ( wID == IDC_BUTTON_SUB2 )
			AppCash_SubCash ( lpUserData, 2 );
		else if ( wID == IDC_BUTTON_SUB200 )
			AppCash_SubCash ( lpUserData, 200 );
		else if ( wID == IDC_BUTTON_SUB868 )
			AppCash_SubCash ( lpUserData, 868 );
		else
			return -1;
		break;

	default:
		return -1;
	}

	return TRUE;
}

//*************************************************************************
// 函数: AppCash_GetBalance ()
// 功能: 从可视卡内取出现金余额
//*************************************************************************
LRESULT AppCash_GetBalance ( LPUSERDATA lpUserData )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	// 取余额
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// 控件显示
	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	MacroAlloc ( pString, 256 );
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	
	// EInk显示
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );

	return 0;
}

//***************************************************************************
// END
//***************************************************************************
