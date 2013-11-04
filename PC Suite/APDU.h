//*************************************************************************
// APDU����

#ifndef __APDU_H__
#define	__APDU_H__


//*************************************************************************
// �������ͷ�ļ�


//*************************************************************************
// �궨��

// APDUָ��
#define APDU_GSM_READBINARY			0xA0B0		// Read BInary
#define APDU_GSM_UPDATABINARY		0xA0D6		// Update Binary

#define	APDU_UKEY_EINKDRIVE			0xBFAB		// ��ʾ�ַ���EInk
#define APDU_UKEY_STARTKEY			0XBFAC		// ��ʼ�ȴ�������Ӧ
#define APDU_UKEY_GETKEYDOWN		0XBFAD		// ȡ����״̬
#define	APDU_UKEY_FLASHWRITE		0xBFAE		// Flashд
#define	APDU_UKEY_FLASHREAD			0xBFAF		// Flahs��

// UKEY ��������
#define UKEY_CASH					0x0018		// �ֽ����
#define UKEY_UKEY2					0x0019		// �˻����


//*************************************************************************
// ���ݽṹ����


//***************************************************************************
// ��������

extern WORD APDU_Command ( LPUSERDATA lpUserData, WORD nCmd, WORD wParam, UINT32 nLen, LPBYTE lpByte );

#endif	__APDU_H__

//***************************************************************************
// END
//***************************************************************************
