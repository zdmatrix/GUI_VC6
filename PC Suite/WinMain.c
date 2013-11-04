//*************************************************************************
// 工程：Win32 SDK 开发主程序示例
// 文件：WinMain.c
// 内容：完成一个基本的 Win32 SDK 程序框架
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include "resource.h"

#include "Dialog.h"						// 模态对话框头文件


//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量


//*************************************************************************
// 此代码模块中包含的函数的向前声明

//*************************************************************************
// 函数: WinMain ()
// 功能: Win32 SDK 应用程序入口函数。创建主窗口，处理消息循环
//*************************************************************************
int WINAPI WinMain ( HINSTANCE	hInstance,		// handle to current instance (当前实例句柄)
					 HINSTANCE	hPrevInstance,	// handle to previous instance (前一个实例句柄)
					 LPSTR		lpCmdLine,		// command line (命令行字符)
					 int		nCmdShow )		// show state (窗口显示方式)
{
	MSG		msg;				// 将要接收消息的消息句柄
	HICON	hIcon;				// 文件及窗口图标
	HWND	hDialog = NULL;		// 对话框句柄

	// 初始化对话框
	hDialog = Dialog_Init ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );

	// 设置非模态对话框左上角图标
	hIcon = LoadIcon ( hInstance, (LPCSTR)IDI_ICON );
	SendMessage ( hDialog, WM_SETICON, ICON_BIG, (LPARAM)hIcon );

	// 显示对话框
	ShowWindow ( hDialog, nCmdShow );
	UpdateWindow ( hDialog );

	// 进入消息循环：
	// 从该应用程序的消息队列中检取消息，送到消息处理过程，
	// 当检取到WM_QUIT消息时，退出消息循环。
	while ( GetMessage ( &msg, NULL, 0, 0 ) )
	{
		if ( !IsDialogMessage ( hDialog, &msg ) )	// 传递消息至DIALOG (TAB键切换编辑框等)
		{
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
	}

	// 程序结束
	_CrtDumpMemoryLeaks();		// 程序结束退出，并报告内存泄露
	return (int)msg.wParam;
}

//***************************************************************************
// END
//***************************************************************************
