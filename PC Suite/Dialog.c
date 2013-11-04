//*************************************************************************
// ���̣�ģ̬�Ի�����ģ��
// �ļ���Dialog.c
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
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
// �궨��


//*************************************************************************
// ���ݽṹ����


//*************************************************************************
// �ڲ�ȫ�ֱ���
const static CHAR gsTitle[] = "APDU Tools";

static LPSTR gTabNames[] = { "��˽Կ/֤��", "�����ֽ�", "����Ukey", "��̬����", "��ҵ����", 0 };
static LPSTR gDlgNames[] = { MAKEINTRESOURCE(IDD_DIALOG_PAGE1),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE2),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE3),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE4),
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE5),
							 0 };


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );


//*************************************************************************
// ����: Dialog_OnInitDialog
// ����: ��ʼ��Ӧ������ģ��
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPVOID		lpVoid = NULL;

	if ( lpUserData == NULL )
		return TRUE;

	// ��ʼ���Ի����������
	lpUserData->hFont = GetWindowFont ( hWnd );
	
	// ����APDU Script�ؼ�
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// ȡ�������豸�б����

	// ����ģ������
	GNODEOP.Create ( &lpUserData->lpModNode );

	// ��ʼ��Ӳ����Ⲣ��ӵ�ģ��������
	if ( (lpVoid = GDEVOP.Init ( hWnd )) == NULL )
		return -1;
	GNODEOP.Append ( lpUserData->lpModNode, lpVoid, APPMOD_DEVICE );

	// ��ʼ��Tab�ؼ�
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
// ����: Dialog_OnClose ()
// ����: �Ի��򴰿��˳�����
//*************************************************************************
static void Dialog_OnClose ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;

	// �ͷ�Ӳ���豸�ռ�
	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( lpDuLNode )
		GDEVOP.Destroy ( &lpDuLNode->lpData );

	// �ͷ�TAB�ռ����
	TabControl_Destroy ( &lpUserData->sTabCtrl );

	// �ͷ�ģ������
	GNODEOP.Destroy ( &lpUserData->lpModNode, NULL );

	DestroyWindow ( hWnd );
	return;
}

//*************************************************************************
// ����: Dialog_OnNotify ()
// ����: Tab���ڱ�ǩ�ĵ��֪ͨ
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
			lpUserData->sTabCtrl.OnSelChanged();	// ԭʼ���
			lpUserData->nTabSelect = SendMessage ( lpUserData->sTabCtrl.hTab, TCM_GETCURSEL, 0, 0 );

			// �л�TAB��
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
// ����: Dialog_OnDevUnmount
// ����: ж���豸
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
// ����: Dialog_OnDevMount
// ����: �ҽ����豸
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
// ����: Dialog_OnDeviceChange
// ����: �ҽ����豸
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
// ����: Dialog_OnPaint
// ����: ����OnPaint��Ϣ
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
// ����: Dialog_OnCommand
// ����: ����OnCommand��Ϣ
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
// ����: Dialog_WndProc ()
// ����: �Ի��򴰿�����Ϣѭ��
//*************************************************************************
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:			// ��ʼ��Ӧ������ģ��
		Dialog_OnInitDialog ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_NOTIFY:
		Dialog_OnNotify ( hWnd, uMsg, wParam, lParam );
		return FALSE;

	case WM_DEVICECHANGE:		// HID �����豸���֪ͨ
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

	return FALSE;		// ����FALSE��ȱʡ�Ի�����DefDlgProc(),��ʾû�д�����Ϣ
}

//*************************************************************************
// ����: Dialog_Init ()
// ����: ��ʼ���Ի���
//*************************************************************************
HWND Dialog_Init ( HINSTANCE	hInstance,		// handle to current instance (��ǰʵ�����)
				   HINSTANCE	hPrevInstance,	// handle to previous instance (ǰһ��ʵ�����)
				   LPSTR		lpCmdLine,		// command line (�������ַ�)
				   int			nCmdShow )		// show state (������ʾ��ʽ)
{
	static USERDATA	UserData = { 0 };	// Ӧ������
	HMODULE			hReturn;

	// ��ʼ������
	UserData.hInstance = hInstance;		// �洢ʵ�����

	// ִ�пؼ����躯��
	InitCommonControls ();						// Tab�ؼ�����
	hReturn = LoadLibrary( "riched32.dll");		// RichEdit�ؼ�������ô���
	if ( hReturn == NULL )
		return NULL;

	// �����Ի���
	UserData.hMainDlg = CreateDialog ( hInstance, (LPCTSTR)IDD_DIALOG, NULL, (DLGPROC)Dialog_WndProc );
	if ( UserData.hMainDlg == NULL )
		return NULL;

	// ���÷�ģ̬�Ի������
	SendMessage ( UserData.hMainDlg, WM_SETTEXT, 0, (LPARAM)gsTitle );

	// ���û�����ָ�븳��Ի�����
	SetWindowLong ( UserData.hMainDlg, GWL_USERDATA, (LONG)&UserData );

	// WM_INITDIALOG��һ���ǻ�û�л���û�����ָ�룬�����ٴη���
	SendMessage ( UserData.hMainDlg, WM_INITDIALOG, 0, 0 );

	return UserData.hMainDlg;
}


//***************************************************************************
// END
//***************************************************************************
