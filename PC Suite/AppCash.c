//*************************************************************************
// �ļ���AppCash.c
// ���ݣ���ʾ�ֽ����
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"
#include "APDU.h"

#include "Devices.h"


//*************************************************************************
// �궨��


//*************************************************************************
// ȫ�ֱ���


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����


//*************************************************************************
//*************************************************************************

//*************************************************************************
// ����: AppCash_InitBalance
// ����: ��ʼ���ֽ����
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
// ����: AppCash_SubCash ()
// ����: �����ֽ�
//*************************************************************************
LRESULT AppCash_SubCash ( LPUSERDATA lpUserData, UINT32 nCash )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	MacroAlloc ( pString, 256 );

	// ***************   ȡ���   *****************************************
	// ���ؽ�Ҫ���͵�����
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;
	
	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	if ( wBalance < nCash )
	{
		MessageBox ( lpUserData->hMainDlg, "�����ֽ��㣬����ʧ�ܣ�", "����", MB_OK );
		return -1;
	}
	wBalance -= nCash;
	
	// ***************   ��д���   *****************************************
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	IOBuff[0] = HIBYTE(wBalance);
	IOBuff[1] = LOBYTE(wBalance);
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   ��ʾ������   *****************************************
	sprintf ( pString, "-%d", nCash );
	
	// EInk��ʾ
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   ���ӿ������ȴ���ʾ���   *****************************************
	// ���ؽ�Ҫ���͵�����
	wResult = APDU_Command ( lpUserData, APDU_UKEY_STARTKEY, UKEY_CASH, 0x00, NULL );
	if ( wResult != 0x9000 )
		return -1;

	return TRUE;
}

//*************************************************************************
// ����: AppCash_AddCash ()
// ����: ����ֽ�
//*************************************************************************
LRESULT AppCash_AddCash ( LPUSERDATA lpUserData, UINT32 nCash )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	MacroAlloc ( pString, 256 );

	// ***************   ȡ���   *****************************************
	// ���ؽ�Ҫ���͵�����
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	if ( (wBalance + nCash) >= 0xFFFE )
	{
		MessageBox ( lpUserData->hMainDlg, "��������޶������ʧ�ܣ�", "����", MB_OK );
		return -1;
	}
	wBalance += nCash;

	// ***************   ��д���   *****************************************
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	IOBuff[0] = HIBYTE(wBalance);
	IOBuff[1] = LOBYTE(wBalance);
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   ��ʾ������   *****************************************
	sprintf ( pString, "+%d", nCash );
	
	// EInk��ʾ
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   ���ӿ������ȴ���ʾ���   *****************************************
	// ���ؽ�Ҫ���͵�����
	wResult = APDU_Command ( lpUserData, APDU_UKEY_STARTKEY, UKEY_CASH, 0x00, NULL );
	if ( wResult != 0x9000 )
		return -1;

	return TRUE;
}

//*************************************************************************
// ����: AppCash_OnCommand ()
// ����: ��������
//*************************************************************************
LRESULT AppCash_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

	// ȷ��˽�����ݵ���ȷ��
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	// ȷ�����豸����
	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( !lpDuLNode )
		return -1;
	if ( !GDEVOP.GetNumber ( lpDuLNode->lpData ) ) return -1;
	
	// OnCommand��Ϣ��ϸ��
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier

	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// ���������Ϣ
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
// ����: AppCash_GetBalance ()
// ����: �ӿ��ӿ���ȡ���ֽ����
//*************************************************************************
LRESULT AppCash_GetBalance ( LPUSERDATA lpUserData )
{
	BYTE	IOBuff[256] = { 0 };
	WORD	wResult;
	WORD	wBalance;
	PCHAR	pString = NULL;
	UINT32	nLen;

	// ȡ���
	wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHREAD, UKEY_CASH, 0x02, IOBuff );
	if ( wResult != 0x9000 )
		return -1;

	// �ؼ���ʾ
	wBalance = MAKEWORD ( IOBuff[1], IOBuff[0] );
	wBalance = AppCash_InitBalance ( lpUserData, wBalance );
	MacroAlloc ( pString, 256 );
	sprintf ( pString, "%d", wBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[1], IDC_EDIT_BALANCE, pString );
	
	// EInk��ʾ
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );

	return 0;
}

//***************************************************************************
// END
//***************************************************************************
