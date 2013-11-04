//*************************************************************************
// 工程：模态对话框处理模块
// 文件：Dialog.c
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include "Global.h"
#include "Devices.h"
#include "AppKeyCert.h"
#include "AppCash.h"
#include "AppUKey2.h"

#include "Dialog.h"


//*************************************************************************
// 宏定义


//*************************************************************************
// 数据结构定义


//*************************************************************************
// 内部全局变量
const static CHAR gsTitle[] = "APDU Tools";

static LPSTR gTabNames[] = { "公私钥/证书", "电子现金", "二代Ukey", "动态口令", "行业数据", 0 };
static LPSTR gDlgNames[] = { MAKEINTRESOURCE(IDD_DIALOG_PAGE1),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE2),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE3),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE4),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE5),
							 0 };


//*************************************************************************
// 此代码模块中包含的函数的向前声明
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );


//*************************************************************************
// 函数: Dialog_OnInitDialog
// 功能: 初始化应用所需模块
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPVOID		lpVoid = NULL;

	if ( lpUserData == NULL )
		return TRUE;

	// 初始化对话框相关数据
	lpUserData->hFont = GetWindowFont ( hWnd );
	
	// 设置APDU Script控件
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// 取已运行设备列表框句柄

	// 创建模块链表
	GNODEOP.Create ( &lpUserData->lpModNode );

	// 初始化硬件检测并添加到模块链表中
	if ( (lpVoid = GDEVOP.Init ( hWnd )) == NULL )
		return -1;
	GNODEOP.Append ( lpUserData->lpModNode, lpVoid, APPMOD_DEVICE );

	// 初始化Tab控件
	New_TabControl ( &lpUserData->sTabCtrl,			// address of TabControl struct
					 GetDlgItem ( hWnd, IDC_TAB ),	// handle to tab control
					 gTabNames,						// text for each tab
					 gDlgNames,						// dialog id's of each tab page dialog
					 &Dialog_WndProc,				// address of main windows proc
					 NULL,							// address of size function
					 TRUE );						// stretch tab page to fit tab ctrl

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnClose ()
// 功能: 对话框窗口退出处理
//*************************************************************************
static void Dialog_OnClose ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;

	// 释放硬件设备空间
	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( lpDuLNode )
		GDEVOP.Destroy ( &lpDuLNode->lpData );

	// 释放TAB空间相关
	TabControl_Destroy ( &lpUserData->sTabCtrl );

	// 释放模块链表
	GNODEOP.Destroy ( &lpUserData->lpModNode, NULL );

	DestroyWindow ( hWnd );
	return;
}

//*************************************************************************
// 函数: Dialog_OnNotify ()
// 功能: Tab窗口标签的点击通知
//*************************************************************************
static BOOL Dialog_OnNotify ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	int			id = (int)wParam;
	LPNMHDR		nm = (LPNMHDR)lParam;
	LPDULNODE	lpDuLNode = NULL;

	if ( id == IDC_TAB )
	{
		TabControl_Select ( &lpUserData->sTabCtrl );	// update internal "this" pointer

		if ( nm->code == TCN_SELCHANGE )
		{
			lpUserData->sTabCtrl.OnSelChanged();	// 原始语句
			lpUserData->nTabSelect = SendMessage ( lpUserData->sTabCtrl.hTab, TCM_GETCURSEL, 0, 0 );

			// 切换TAB后
			lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
			if ( (lpUserData->nTabSelect == 1) && GDEVOP.GetNumber ( lpDuLNode->lpData ) )
				AppCash_GetBalance ( lpUserData );
			else if ( (lpUserData->nTabSelect == 2) && GDEVOP.GetNumber ( lpDuLNode->lpData ) )
				AppUKey2_GetBalance ( lpUserData );
		}
	}


	return FALSE;
}

//*************************************************************************
// 函数: Dialog_OnDevUnmount
// 功能: 卸载设备
//*************************************************************************
LRESULT Dialog_OnDevUnmount ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;

	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	SendMessage ( lpUserData->hComboNewDev, CB_DELETESTRING, GDEVOP.GetChange ( lpDuLNode->lpData ), 0 );
	SendMessage ( lpUserData->hComboNewDev, CB_SETCURSEL, GDEVOP.GetIndex ( lpDuLNode->lpData ), 0 );

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnDevMount
// 功能: 挂接新设备
//*************************************************************************
LRESULT Dialog_OnDevMount ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	PCHAR		pString = NULL;
	LPDULNODE	lpDuLNode = NULL;
	LRESULT		nChange;

	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	nChange = GDEVOP.GetChange ( lpDuLNode->lpData );
	if ( nChange == -1 )
		return -1;
	pString = GDEVOP.GetName ( lpDuLNode->lpData, nChange );
	if ( !pString ) return 0;

	SendMessage ( lpUserData->hComboNewDev, CB_ADDSTRING, 0, (LPARAM)pString );
	SendMessage ( lpUserData->hComboNewDev, CB_SETCURSEL, GDEVOP.GetIndex ( lpDuLNode->lpData ), 0 ) ;
	MacroFree ( pString );

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnDeviceChange
// 功能: 挂接新设备
//*************************************************************************
LRESULT Dialog_OnDeviceChange ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	DEVOP		sDevOp = { NULL };

	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( !lpDuLNode )
		return 0;

	GDEVOP.OnChange ( lpDuLNode->lpData, wParam, lParam );

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnPaint
// 功能: 处理OnPaint消息
//*************************************************************************
LRESULT Dialog_OnPaint ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	if ( hWnd == lpUserData->sTabCtrl.hTabPages[2] )
		return AppUKey2_OnPaint ( lpUserData, hWnd );

	return FALSE;
}

//*************************************************************************
// 函数: Dialog_OnCommand
// 功能: 处理OnCommand消息
//*************************************************************************
LRESULT Dialog_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );

	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	if ( lpUserData->nTabSelect == 0 )
		AppKeyCert_OnCommand ( hWnd, uMsg, wParam, lParam );
	else if ( lpUserData->nTabSelect == 1 )
		AppCash_OnCommand ( hWnd, uMsg, wParam, lParam );
	else if ( lpUserData->nTabSelect == 2 )
		AppUKey2_OnCommand ( hWnd, uMsg, wParam, lParam );

	return 0;
}

//*************************************************************************
// 函数: Dialog_WndProc ()
// 功能: 对话框窗口主消息循环
//*************************************************************************
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:			// 初始化应用所需模块
		Dialog_OnInitDialog ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_NOTIFY:
		Dialog_OnNotify ( hWnd, uMsg, wParam, lParam );
		return FALSE;

	case WM_DEVICECHANGE:		// HID 类型设备插拔通知
		Dialog_OnDeviceChange ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_DEVICE_MOUNT:
		Dialog_OnDevMount ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_DEVICE_UNMOUNT:
		Dialog_OnDevUnmount ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_COMMAND:
		Dialog_OnCommand ( hWnd, uMsg, wParam, lParam );
		return TRUE;
		
	case WM_PAINT:
		return Dialog_OnPaint ( hWnd, uMsg, wParam, lParam );

	case WM_UKEY2_SENDCARD:
		AppUKey2_SendCard ( hWnd, uMsg, wParam, lParam );
		break;
		
	case WM_UKEY2_WAITKEY:
		AppUKey2_WaitKey ( hWnd, uMsg, wParam, lParam );
		break;

	case WM_CLOSE:
		Dialog_OnClose ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage ( 0 );
		return TRUE;

	default:
		break;
	}

	return FALSE;		// 返回FALSE给缺省对话框函数DefDlgProc(),表示没有处理本消息
}

//*************************************************************************
// 函数: Dialog_Init ()
// 功能: 初始化对话框
//*************************************************************************
HWND Dialog_Init ( HINSTANCE	hInstance,		// handle to current instance (当前实例句柄)
				   HINSTANCE	hPrevInstance,	// handle to previous instance (前一个实例句柄)
				   LPSTR		lpCmdLine,		// command line (命令行字符)
				   int			nCmdShow )		// show state (窗口显示方式)
{
	static USERDATA	UserData = { 0 };	// 应用数据
	HMODULE			hReturn;

	// 初始化数据
	UserData.hInstance = hInstance;		// 存储实例句柄

	// 执行控件所需函数
	InitCommonControls ();						// Tab控件所需
	hReturn = LoadLibrary( "riched32.dll");		// RichEdit控件必须调用此行
	if ( hReturn == NULL )
		return NULL;

	// 创建对话框
	UserData.hMainDlg = CreateDialog ( hInstance, (LPCTSTR)IDD_DIALOG, NULL, (DLGPROC)Dialog_WndProc );
	if ( UserData.hMainDlg == NULL )
		return NULL;

	// 设置非模态对话框标题
	SendMessage ( UserData.hMainDlg, WM_SETTEXT, 0, (LPARAM)gsTitle );

	// 将用户数据指针赋予对话框句柄
	SetWindowLong ( UserData.hMainDlg, GWL_USERDATA, (LONG)&UserData );

	// WM_INITDIALOG第一次是还没有获得用户数据指针，所以再次发送
	SendMessage ( UserData.hMainDlg, WM_INITDIALOG, 0, 0 );

	return UserData.hMainDlg;
}


//***************************************************************************
// END
//***************************************************************************
