//*************************************************************************
// 公用子函数的头文件

#ifndef __SUBFUNC_H__
#define	__SUBFUNC_H__


//////////////////////////////////////////////////////////////////
// MacroFree 宏定义												//
#define	MacroFree(x)											\
	if ( (x) != NULL )											\
	{															\
		free ( x );												\
		(x) = NULL;												\
	}

//////////////////////////////////////////////////////////////////
// MacroAlloc 宏定义											//
#define	MacroAlloc(m,l)											\
	if ( (m) == NULL )											\
	{															\
		(m) = malloc (l);										\
		if ( (m) != NULL )										\
		{														\
			memset ( (m), 0x00, (l) );							\
		}														\
	}


//***************************************************************************
// 函数声明

// 设置控件内容(将整数显示到控件上)
extern	LRESULT SetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, WORD nWord );

// 读取控件内容(将控件上字符串转换为整数)
extern	WORD GetDlgItemText_Word ( HWND hWnd, int nIDDlgItem, int nLength );


#endif	__SUBFUNC_H__

//***************************************************************************
// END
//***************************************************************************
