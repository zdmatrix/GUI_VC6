//*************************************************************************
// �ļ���APDU.c
// ���ݣ�APDU����
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#include <windows.h>
#include "Global.h"
#include "Devices.h"

#include "APDU.h"


//*************************************************************************
// �궨��


//*************************************************************************
//*************************************************************************

//*************************************************************************
// ����: APDU_Command
// ����: APDUָ��ͨ�ú���
//*************************************************************************
WORD APDU_Command ( LPUSERDATA lpUserData, WORD nCmd, WORD wParam, UINT32 nLen, LPBYTE lpByte )
{
	static BYTE	OutputReport[288];
	static BYTE	InputReport[288];
	LPDULNODE	lpDuLNode = NULL;
	WORD		wResult;

	// ��ֹ��ָ�봫��
	if ( !lpUserData )
		return -1;

	// ��ջ�����
	memset ( OutputReport, 0x00, 288 );
	OutputReport[0] = 0x00;				// ����ID ����Ϊ 0
	OutputReport[2] = HIBYTE(nCmd);		// CLA
	OutputReport[3] = LOBYTE(nCmd);		// INS
	OutputReport[4] = HIBYTE(wParam);	// P1
	OutputReport[5] = LOBYTE(wParam);	// P2
	OutputReport[6] = nLen;				// P3

	switch ( nCmd )
	{
	// ����һ
	case APDU_GSM_UPDATABINARY:
	case APDU_UKEY_FLASHWRITE:
	case APDU_UKEY_EINKDRIVE:
	case APDU_UKEY_STARTKEY:
	case APDU_UKEY_GETKEYDOWN:
		OutputReport[1] = 0x05 + nLen;		// ���ݳ���(��������ͷ)
		if ( nLen && lpByte )
			memcpy ( &OutputReport[7], lpByte, nLen );
		break;

	// ���Ͷ�
	case APDU_UKEY_FLASHREAD:
	case APDU_GSM_READBINARY:
		OutputReport[1] = 0x05;				// ���ݳ���(��������ͷ)
		break;
	}

	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );

	// ������д���豸
	wResult = GDEVOP.Send ( lpDuLNode->lpData, OutputReport, InputReport );

	switch ( nCmd )
	{
	// ���Ͷ�
	case APDU_UKEY_FLASHREAD:
	case APDU_GSM_READBINARY:
		if ( nLen && lpByte )
			memcpy ( lpByte, &InputReport[3], nLen );
		break;
	}

	return wResult;
}


//***************************************************************************
// END
//***************************************************************************
