//*************************************************************************
// �ļ���AppUKey2.h
// ���ݣ���ʾ��ȫ����
//*************************************************************************


#ifndef __APP_UKEY2_H__
#define	__APP_UKEY2_H__


//*************************************************************************
// �������ͷ�ļ�


//*************************************************************************
// �궨��


//*************************************************************************
// ���ݽṹ����


//***************************************************************************
// ��������
extern LRESULT AppUKey2_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
extern LRESULT AppUKey2_OnPaint ( LPUSERDATA, HWND );		// ��ʾͼ��
extern LRESULT AppUKey2_SendCard ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
extern LRESULT AppUKey2_GetBalance ( LPUSERDATA );
extern void AppUKey2_WaitKey ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



#endif	__APP_UKEY2_H__


//***************************************************************************
// END
//***************************************************************************
