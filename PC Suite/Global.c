//*************************************************************************
// 文件：Global.c
// 内容：公用函数
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include "Global.h"


//*************************************************************************
// 宏定义


//*************************************************************************
// 此代码模块中包含的函数的向前声明
LRESULT DuLNode_Create ( LPDULNODE *lppDuLNode );
LRESULT DuLNode_Destroy ( LPDULNODE *lppDuLNode, VOID (*CallBackFn)(LPVOID, UINT32) );
LRESULT DuLNode_Append ( LPDULNODE lpDuLNode, LPVOID lpVoid, UINT32 nCustom );
LRESULT DuLNode_Delete ( LPDULNODE *lppDuLNode, UINT32 nIndex, VOID (*CallBackFn)(LPVOID, UINT32) );
LPDULNODE DuLNode_FindCustom ( LPDULNODE lpDuLNode, UINT32 nCustom );
LPDULNODE DuLNode_GetNode ( LPDULNODE lpDuLNode, UINT32 nIndex );


//*************************************************************************
// 全局变量
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
// 函数: DuLNode_GetNode
// 功能: 取链表中指定序号的节点指针
//*************************************************************************
LPDULNODE DuLNode_GetNode ( LPDULNODE lpDuLNode, UINT32 nIndex )
{
	UINT32	i;

	for ( i = 0; i < nIndex; i )
		if ( (lpDuLNode = lpDuLNode->next) == NULL )
			return NULL;	// 如果至链表尾部时还未找到，则返回NULL

	return lpDuLNode;
}

//*************************************************************************
// 函数: DuLNode_FindCustom
// 功能: 在链表中查找自定义相同的节点
//*************************************************************************
LPDULNODE DuLNode_FindCustom ( LPDULNODE lpDuLNode, UINT32 nCustom )
{
	if ( !lpDuLNode )
		return NULL;

	do
	{
		// 如果自定义字节相同则认为相同
		if ( (lpDuLNode->nCustom == nCustom) && lpDuLNode->lpData )
			return lpDuLNode;

		// 继续查找下一个节点
		lpDuLNode = lpDuLNode->next;
	} while ( lpDuLNode );

	return NULL;
}

//*************************************************************************
// 函数: DuLNode_Delete
// 功能: 删除链表中的一个指定节点
//*************************************************************************
LRESULT DuLNode_Delete ( LPDULNODE *lppDuLNode, UINT32 nIndex, VOID (*CallBackFn)(LPVOID, UINT32) )
{
	UINT32		i;
	LPDULNODE	lpCurrent = NULL;
	LPDULNODE	lpPrior = NULL;

	if ( lppDuLNode == NULL ) return -1;	// 空指针防护
	if ( *lppDuLNode == NULL ) return -1;	// 已使用的情况

	lpCurrent = *lppDuLNode;

	// 定位到指定节点
	for ( i = 0; i < nIndex; i )
	{
		if ( lpCurrent->next == NULL )
			return -1;

		lpPrior = lpCurrent;
		lpCurrent = lpCurrent->next;
	}

	// 释放用户空间
	if ( lpCurrent->lpData && CallBackFn )
		CallBackFn ( lpCurrent->lpData, lpCurrent->nCustom );
	lpCurrent->lpData = NULL;
	lpCurrent->nCustom = 0;

	// 释放节点，如果是唯一一个节点则保留为空
	if ( lpCurrent->prior && lpCurrent->next )
	{
		// 中间节点
		lpCurrent->prior->next = lpCurrent->next;
		lpCurrent->next->prior = lpCurrent->prior;
	}
	else if ( lpCurrent->prior && !lpCurrent->next )
	{
		// 最后节点
		lpCurrent->prior->next = NULL;
	}
	else if ( !lpCurrent->prior && lpCurrent->next )
	{
		// 头节点
		lpCurrent->next->prior = NULL;
		*lppDuLNode = lpCurrent->next;
	}
	else
	{
		// 唯一一个节点
		return TRUE;
	}
	MacroFree ( lpCurrent );

	return TRUE;
}

//*************************************************************************
// 函数: DuLNode_Append
// 功能: 在链表尾部添加节点。
//*************************************************************************
LRESULT DuLNode_Append ( LPDULNODE lpDuLNode, LPVOID lpVoid, UINT32 nCustom )
{
	if ( lpDuLNode == NULL ) return -1;		// 空指针防护

	// 查找最后一个节点
	while ( lpDuLNode->next != NULL )
		lpDuLNode = lpDuLNode->next;

	// 判断是否为空链表
	if ( (lpDuLNode->prior != NULL) || (lpDuLNode->lpData != NULL) )
	{
		// 非空节点
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
// 函数: DuLNode_Destroy
// 功能: 销毁链表
//*************************************************************************
LRESULT DuLNode_Destroy ( LPDULNODE *lppDuLNode, VOID (*CallBackFn)(LPVOID, UINT32) )
{
	LPDULNODE	lpCurrent = NULL;
	LPDULNODE	lpNext = NULL;

	if ( lppDuLNode == NULL ) return -1;	// 空指针防护
	if ( *lppDuLNode == NULL ) return -1;	// 已使用的情况

	lpCurrent = *lppDuLNode;

	// 从链表头部开始向后释放链表
	do
	{
		lpNext = lpCurrent->next;

		// 释放用户空间
		if ( lpCurrent->lpData && CallBackFn )
			CallBackFn ( (*lppDuLNode)->lpData, (*lppDuLNode)->nCustom );

		// 释放节点
		MacroFree ( lpCurrent );
	} while ( lpNext );	// 是否存在下一个节点
	*lppDuLNode = NULL;

	return TRUE;
}

//*************************************************************************
// 函数: DuLNode_Create
// 功能: 创建一个双向链表，如链表已存在内容则返回 -1。
//*************************************************************************
LRESULT DuLNode_Create ( LPDULNODE *lppDuLNode )
{
	if ( lppDuLNode == NULL ) return -1;	// 空指针防护
	if ( *lppDuLNode != NULL ) return -1;	// 已使用的情况

	MacroAlloc ( *lppDuLNode, sizeof(DULNODE) );	// 分配空间

	return TRUE;
}

//*************************************************************************
// 函数: Global_Hex2String
// 功能: 将16进制数据转换为字符串
//*************************************************************************
PCHAR Global_Hex2String ( PBYTE pByte, UINT32 nCount )
{
	PCHAR	pString = NULL;
	UINT32	i, j = 0;
	BYTE	temp = 0;

	MacroAlloc ( pString, nCount * 3 );

	// 转换16进制到字符串
	for ( i = 0; i < nCount; i++ )
	{
		// 高4位
		temp = pByte[i] >> 4;
		if ( temp >= 0x00 && temp <= 0x09 )
			pString[j++] = '0' + temp;
		else
			pString[j++] = 'A' + (temp - 0x0A);

		// 低4位
		temp = pByte[i] & 0x0F;
		if ( temp >= 0x00 && temp <= 0x09 )
			pString[j++] = '0' + temp;
		else
			pString[j++] = 'A' +(temp - 0x0A);

		// 字节间隔
		if ( i < (nCount - 1) )
			pString[j++] = ' ';
	}

	return pString;
}


//***************************************************************************
// END
//***************************************************************************
