//*************************************************************************
// 文件: Devices.h
//***************************************************************************

#ifndef __DEVICES_H__
#define	__DEVICES_H__


//*************************************************************************
// 数据结构定义

// 链表操作接口
typedef struct _tagDEVOP
{
	LPVOID		(*Init		) (HWND);						// 初始化硬件检测
	VOID		(*Destroy	) (LPVOID * );					// 卸载硬件相关空间
	LRESULT		(*OnChange	) (LPVOID, WPARAM, LPARAM);		// 设备变化通知
	WORD		(*Send		) (LPVOID, BYTE *, BYTE *);		// 传送数据
	PCHAR		(*GetName	) (LPVOID, UINT32);				// 取设备显示用名称
	UINT32		(*GetNumber	) (LPVOID);						// 取可使用设备数量
	UINT32		(*GetChange	) (LPVOID);						// 取变化设备索引
	UINT32		(*GetIndex	) (LPVOID);						// 取当前设备索引
	VOID		(*SetIndex	) (LPVOID, UINT32);				// 设置使用设备索引
} DEVOP, *LPDEVOP;


//***************************************************************************
// 函数声明

extern const DEVOP GDEVOP;



#endif	__DEVICES_H__

//***************************************************************************
// END
//***************************************************************************
