//*************************************************************************
// 模态对话框头文件

#ifndef __GLOBAL_H__
#define	__GLOBAL_H__

#include "TabCtrl.h"


//*************************************************************************
// 宏定义

#define		WM_READ_HID_REPORT		(WM_USER + 1)
#define		WM_DEVICE_MOUNT			(WM_USER + 2)
#define		WM_DEVICE_UNMOUNT		(WM_USER + 3)
#define		WM_UKEY2_SENDCARD		(WM_USER + 4)
#define		WM_UKEY2_WAITKEY		(WM_USER + 5)

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

//////////////////////////////////////////////////////////////////
// MacroTest 宏测试												//
#define MacroTest(x)											\
	if ( x == NULL )											\
	{															\
		while ( TRUE );											\
	}


//*************************************************************************
// 数据结构定义

// 双向链表数据结构
typedef struct _tagDULNODE
{
	struct _tagDULNODE	*prior;		// 前一个节点，NULL为头节点
	struct _tagDULNODE	*next;		// 后一个节点，NULL为尾节点

	LPVOID				lpData;		// 节点数据指针
	UINT32				nCustom;	// 节点自定义
} DULNODE, *LPDULNODE;

// 用户数据存放点
typedef struct _tagUSERDATA
{
	HINSTANCE	hInstance;			// 应用实例句柄
	HWND		hMainDlg;			// DIALOG窗口句柄
	HFONT		hFont;				// 程序所使用的默认字体

	// 界面控件句柄
	HWND		hComboNewDev;		// 已挂接设备列表
	TABCTRL		sTabCtrl;			// Tab控件
	UINT32		nTabSelect;			// 当前TAB所选标签索引
	
	LPDULNODE	lpModNode;			// 模块专用链表指针

	// UKEY相关
	UINT32		nPaint;				// 是否显示
} USERDATA, *LPUSERDATA;

// 模块定义
typedef enum _tagAPPMOD
{
	APPMOD_DEVICE = 0,			// 硬件设备
	APPMOD_END					// 定义结束
} APPMOD, *LPAPPMOD;

// 链表操作接口
typedef struct _tagNODEOP
{
	LRESULT		(*Create	) (LPDULNODE *);												// 创建一个双向链表，如链表已存在内容则返回 -1。
	LRESULT		(*Destroy	) (LPDULNODE *, VOID (*CallBackFn)(LPVOID, UINT32));			// 销毁链表
	LRESULT		(*Append	) (LPDULNODE, LPVOID, UINT32);									// 在链表尾部添加节点。
	LRESULT		(*Delete	) (LPDULNODE *, UINT32, VOID (*CallBackFn)(LPVOID, UINT32));	// 删除链表中的一个指定节点
	LPDULNODE	(*Find		) (LPDULNODE, UINT32);											// 在链表中查找自定义相同的节点
	LPDULNODE	(*GetNode	) (LPDULNODE, UINT32);											// 取链表中指定序号的节点指针
}NODEOP, *LPNODEOP;

extern const NODEOP GNODEOP;


//***************************************************************************
// 函数声明

extern PCHAR		Global_Hex2String	( PBYTE, UINT32 );				// 将16进制数据转换为字符串


#endif	__GLOBAL_H__

//***************************************************************************
// END
//***************************************************************************
