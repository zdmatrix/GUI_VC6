//*************************************************************************
// 文件：AppUKey2.h
// 内容：演示安全交易
//*************************************************************************


#ifndef __APP_UKEY2_H__
#define	__APP_UKEY2_H__


//*************************************************************************
// 包含相关头文件


//*************************************************************************
// 宏定义


//*************************************************************************
// 数据结构定义


//***************************************************************************
// 函数声明
extern LRESULT AppUKey2_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
extern LRESULT AppUKey2_OnPaint ( LPUSERDATA, HWND );		// 显示图形
extern LRESULT AppUKey2_SendCard ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
extern LRESULT AppUKey2_GetBalance ( LPUSERDATA );
extern void AppUKey2_WaitKey ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



#endif	__APP_UKEY2_H__


//***************************************************************************
// END
//***************************************************************************
