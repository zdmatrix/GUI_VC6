//*************************************************************************
// ���̣�Win32 SDK ����������ʾ��
// �ļ���WinMain.c
// ���ݣ����һ�������� Win32 SDK ������
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#include <windows.h>
#include "resource.h"

#include "Dialog.h"						// ģ̬�Ի���ͷ�ļ�


//*************************************************************************
// �궨��


//*************************************************************************
// ȫ�ֱ���


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����

//*************************************************************************
// ����: WinMain ()
// ����: Win32 SDK Ӧ�ó�����ں��������������ڣ�������Ϣѭ��
//*************************************************************************
int WINAPI WinMain ( HINSTANCE	hInstance,		// handle to current instance (��ǰʵ�����)
					 HINSTANCE	hPrevInstance,	// handle to previous instance (ǰһ��ʵ�����)
					 LPSTR		lpCmdLine,		// command line (�������ַ�)
					 int		nCmdShow )		// show state (������ʾ��ʽ)
{
	MSG		msg;				// ��Ҫ������Ϣ����Ϣ���
	HICON	hIcon;				// �ļ�������ͼ��
	HWND	hDialog = NULL;		// �Ի�����

	// ��ʼ���Ի���
	hDialog = Dialog_Init ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );

	// ���÷�ģ̬�Ի������Ͻ�ͼ��
	hIcon = LoadIcon ( hInstance, (LPCSTR)IDI_ICON );
	SendMessage ( hDialog, WM_SETICON, ICON_BIG, (LPARAM)hIcon );

	// ��ʾ�Ի���
	ShowWindow ( hDialog, nCmdShow );
	UpdateWindow ( hDialog );

	// ������Ϣѭ����
	// �Ӹ�Ӧ�ó������Ϣ�����м�ȡ��Ϣ���͵���Ϣ������̣�
	// ����ȡ��WM_QUIT��Ϣʱ���˳���Ϣѭ����
	while ( GetMessage ( &msg, NULL, 0, 0 ) )
	{
		if ( !IsDialogMessage ( hDialog, &msg ) )	// ������Ϣ��DIALOG (TAB���л��༭���)
		{
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
	}

	// �������
	_CrtDumpMemoryLeaks();		// ��������˳����������ڴ�й¶
	return (int)msg.wParam;
}

//***************************************************************************
// END
//***************************************************************************
