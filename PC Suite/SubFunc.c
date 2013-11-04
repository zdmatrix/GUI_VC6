//*************************************************************************
// 工程：Win32 SDK 开发主程序示例
// 文件：SubFunc.c
// 内容：公用子函数
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//*************************************************************************
// 包含必要的头文件

#include <stdio.h>
#include <windows.h>

#include "SubFunc.h"


//*************************************************************************
// 宏定义

//*************************************************************************
// 全局变量

//*************************************************************************
// 此代码模块中包含的函数的向前声明


//*************************************************************************
// 函数: SetDlgItemText_Word
// 功能: 设置控件内容(将整数显示到控件上)
//*************************************************************************
LRESULT SetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, WORD nWord )
{
	LPSTR	lpText = NULL;
	int		nLen;

	nLen = (sizeof(WORD) * 2) + 1;

	// 将整数转为字符串
	lpText = (LPSTR)malloc ( nLen );
	memset ( lpText, 0x00, nLen );
	sprintf ( lpText, "%04X", nWord );

	// 设置到控件
	SetDlgItemText ( hWnd, nIDDlgItem, lpText );

	free ( lpText );

	return 0;
}

//*************************************************************************
// 函数: GetDlgItemText_Word
// 功能: 读取控件内容(将控件上字符串转换为整数)
//*************************************************************************
WORD GetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, int nLength )
{
	LPSTR	lpText = NULL;
	WORD	nReturn = 0;
	int		i;
	int		nLen;
	
	nLen = (sizeof(WORD) * 2) + 1;

	// 取控件的字符串
	lpText = (LPSTR)malloc ( nLen );
	memset ( lpText, 0x00, nLen );
	GetDlgItemText ( hWnd, nIDDlgItem, lpText, nLen );

	// 转换10进制到16进制
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
