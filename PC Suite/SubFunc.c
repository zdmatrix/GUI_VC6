//*************************************************************************
// ���̣�Win32 SDK ����������ʾ��
// �ļ���SubFunc.c
// ���ݣ������Ӻ���
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//*************************************************************************
// ������Ҫ��ͷ�ļ�

#include <stdio.h>
#include <windows.h>

#include "SubFunc.h"


//*************************************************************************
// �궨��

//*************************************************************************
// ȫ�ֱ���

//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����


//*************************************************************************
// ����: SetDlgItemText_Word
// ����: ���ÿؼ�����(��������ʾ���ؼ���)
//*************************************************************************
LRESULT SetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, WORD nWord )
{
	LPSTR	lpText = NULL;
	int		nLen;

	nLen = (sizeof(WORD) * 2) + 1;

	// ������תΪ�ַ���
	lpText = (LPSTR)malloc ( nLen );
	memset ( lpText, 0x00, nLen );
	sprintf ( lpText, "%04X", nWord );

	// ���õ��ؼ�
	SetDlgItemText ( hWnd, nIDDlgItem, lpText );

	free ( lpText );

	return 0;
}

//*************************************************************************
// ����: GetDlgItemText_Word
// ����: ��ȡ�ؼ�����(���ؼ����ַ���ת��Ϊ����)
//*************************************************************************
WORD GetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, int nLength )
{
	LPSTR	lpText = NULL;
	WORD	nReturn = 0;
	int		i;
	int		nLen;
	
	nLen = (sizeof(WORD) * 2) + 1;

	// ȡ�ؼ����ַ���
	lpText = (LPSTR)malloc ( nLen );
	memset ( lpText, 0x00, nLen );
	GetDlgItemText ( hWnd, nIDDlgItem, lpText, nLen );

	// ת��10���Ƶ�16����
	for ( i = 0; i < nLength; i++ )
	{
		WORD	temp = 0;

		nReturn = nReturn << 4;
		if ( lpText[i] >= '0' && lpText[i] <= '9' )
			temp = lpText[i] - '0';
		else if ( lpText[i] >= 'A' && lpText[i] <= 'F' )
			temp = 10 + lpText[i] - 'A';
		else if ( lpText[i] >= 'a' && lpText[i] <= 'f' )
			temp = 10 + lpText[i] - 'a';
		else
			temp = 0;
		nReturn += temp;
	}

	free ( lpText );

	return nReturn;
}


//***************************************************************************
// END
//***************************************************************************
