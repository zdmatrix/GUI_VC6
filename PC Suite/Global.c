//*************************************************************************
// �ļ���Global.c
// ���ݣ����ú���
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


//*************************************************************************
// �궨��


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����
LRESULT DuLNode_Create ( LPDULNODE *lppDuLNode );
LRESULT DuLNode_Destroy ( LPDULNODE *lppDuLNode, VOID (*CallBackFn)(LPVOID, UINT32) );
LRESULT DuLNode_Append ( LPDULNODE lpDuLNode, LPVOID lpVoid, UINT32 nCustom );
LRESULT DuLNode_Delete ( LPDULNODE *lppDuLNode, UINT32 nIndex, VOID (*CallBackFn)(LPVOID, UINT32) );
LPDULNODE DuLNode_FindCustom ( LPDULNODE lpDuLNode, UINT32 nCustom );
LPDULNODE DuLNode_GetNode ( LPDULNODE lpDuLNode, UINT32 nIndex );


//*************************************************************************
// ȫ�ֱ���
const NODEOP GNODEOP =
{
	DuLNode_Create,
	DuLNode_Destroy,
	DuLNode_Append,
	DuLNode_Delete,
	DuLNode_FindCustom,
	DuLNode_GetNode
};


//*************************************************************************
//*************************************************************************

//*************************************************************************
// ����: DuLNode_GetNode
// ����: ȡ������ָ����ŵĽڵ�ָ��
//*************************************************************************
LPDULNODE DuLNode_GetNode ( LPDULNODE lpDuLNode, UINT32 nIndex )
{
	UINT32	i;

	for ( i = 0; i < nIndex; i )
		if ( (lpDuLNode = lpDuLNode->next) == NULL )
			return NULL;	// ���������β��ʱ��δ�ҵ����򷵻�NULL

	return lpDuLNode;
}

//*************************************************************************
// ����: DuLNode_FindCustom
// ����: �������в����Զ�����ͬ�Ľڵ�
//*************************************************************************
LPDULNODE DuLNode_FindCustom ( LPDULNODE lpDuLNode, UINT32 nCustom )
{
	if ( !lpDuLNode )
		return NULL;

	do
	{
		// ����Զ����ֽ���ͬ����Ϊ��ͬ
		if ( (lpDuLNode->nCustom == nCustom) && lpDuLNode->lpData )
			return lpDuLNode;

		// ����������һ���ڵ�
		lpDuLNode = lpDuLNode->next;
	} while ( lpDuLNode );

	return NULL;
}

//*************************************************************************
// ����: DuLNode_Delete
// ����: ɾ�������е�һ��ָ���ڵ�
//*************************************************************************
LRESULT DuLNode_Delete ( LPDULNODE *lppDuLNode, UINT32 nIndex, VOID (*CallBackFn)(LPVOID, UINT32) )
{
	UINT32		i;
	LPDULNODE	lpCurrent = NULL;
	LPDULNODE	lpPrior = NULL;

	if ( lppDuLNode == NULL ) return -1;	// ��ָ�����
	if ( *lppDuLNode == NULL ) return -1;	// ��ʹ�õ����

	lpCurrent = *lppDuLNode;

	// ��λ��ָ���ڵ�
	for ( i = 0; i < nIndex; i )
	{
		if ( lpCurrent->next == NULL )
			return -1;

		lpPrior = lpCurrent;
		lpCurrent = lpCurrent->next;
	}

	// �ͷ��û��ռ�
	if ( lpCurrent->lpData && CallBackFn )
		CallBackFn ( lpCurrent->lpData, lpCurrent->nCustom );
	lpCurrent->lpData = NULL;
	lpCurrent->nCustom = 0;

	// �ͷŽڵ㣬�����Ψһһ���ڵ�����Ϊ��
	if ( lpCurrent->prior && lpCurrent->next )
	{
		// �м�ڵ�
		lpCurrent->prior->next = lpCurrent->next;
		lpCurrent->next->prior = lpCurrent->prior;
	}
	else if ( lpCurrent->prior && !lpCurrent->next )
	{
		// ���ڵ�
		lpCurrent->prior->next = NULL;
	}
	else if ( !lpCurrent->prior && lpCurrent->next )
	{
		// ͷ�ڵ�
		lpCurrent->next->prior = NULL;
		*lppDuLNode = lpCurrent->next;
	}
	else
	{
		// Ψһһ���ڵ�
		return TRUE;
	}
	MacroFree ( lpCurrent );

	return TRUE;
}

//*************************************************************************
// ����: DuLNode_Append
// ����: ������β����ӽڵ㡣
//*************************************************************************
LRESULT DuLNode_Append ( LPDULNODE lpDuLNode, LPVOID lpVoid, UINT32 nCustom )
{
	if ( lpDuLNode == NULL ) return -1;		// ��ָ�����

	// �������һ���ڵ�
	while ( lpDuLNode->next != NULL )
		lpDuLNode = lpDuLNode->next;

	// �ж��Ƿ�Ϊ������
	if ( (lpDuLNode->prior != NULL) || (lpDuLNode->lpData != NULL) )
	{
		// �ǿսڵ�
		LPDULNODE	lpNewNode = NULL;

		MacroAlloc ( lpNewNode, sizeof(DULNODE) );
		lpDuLNode->next = lpNewNode;
		lpNewNode->prior = lpDuLNode;
		lpDuLNode = lpNewNode;
	}
	lpDuLNode->lpData = lpVoid;
	lpDuLNode->nCustom = nCustom;

	return TRUE;
}

//*************************************************************************
// ����: DuLNode_Destroy
// ����: ��������
//*************************************************************************
LRESULT DuLNode_Destroy ( LPDULNODE *lppDuLNode, VOID (*CallBackFn)(LPVOID, UINT32) )
{
	LPDULNODE	lpCurrent = NULL;
	LPDULNODE	lpNext = NULL;

	if ( lppDuLNode == NULL ) return -1;	// ��ָ�����
	if ( *lppDuLNode == NULL ) return -1;	// ��ʹ�õ����

	lpCurrent = *lppDuLNode;

	// ������ͷ����ʼ����ͷ�����
	do
	{
		lpNext = lpCurrent->next;

		// �ͷ��û��ռ�
		if ( lpCurrent->lpData && CallBackFn )
			CallBackFn ( (*lppDuLNode)->lpData, (*lppDuLNode)->nCustom );

		// �ͷŽڵ�
		MacroFree ( lpCurrent );
	} while ( lpNext );	// �Ƿ������һ���ڵ�
	*lppDuLNode = NULL;

	return TRUE;
}

//*************************************************************************
// ����: DuLNode_Create
// ����: ����һ��˫�������������Ѵ��������򷵻� -1��
//*************************************************************************
LRESULT DuLNode_Create ( LPDULNODE *lppDuLNode )
{
	if ( lppDuLNode == NULL ) return -1;	// ��ָ�����
	if ( *lppDuLNode != NULL ) return -1;	// ��ʹ�õ����

	MacroAlloc ( *lppDuLNode, sizeof(DULNODE) );	// ����ռ�

	return TRUE;
}

//*************************************************************************
// ����: Global_Hex2String
// ����: ��16��������ת��Ϊ�ַ���
//*************************************************************************
PCHAR Global_Hex2String ( PBYTE pByte, UINT32 nCount )
{
	PCHAR	pString = NULL;
	UINT32	i, j = 0;
	BYTE	temp = 0;

	MacroAlloc ( pString, nCount * 3 );

	// ת��16���Ƶ��ַ���
	for ( i = 0; i < nCount; i++ )
	{
		// ��4λ
		temp = pByte[i] >> 4;
		if ( temp >= 0x00 && temp <= 0x09 )
			pString[j++] = '0' + temp;
		else
			pString[j++] = 'A' + (temp - 0x0A);

		// ��4λ
		temp = pByte[i] & 0x0F;
		if ( temp >= 0x00 && temp <= 0x09 )
			pString[j++] = '0' + temp;
		else
			pString[j++] = 'A' +(temp - 0x0A);

		// �ֽڼ��
		if ( i < (nCount - 1) )
			pString[j++] = ' ';
	}

	return pString;
}


//***************************************************************************
// END
//***************************************************************************
