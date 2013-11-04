//*************************************************************************
// ģ̬�Ի���ͷ�ļ�

#ifndef __DIALOG_H__
#define	__DIALOG_H__


//*************************************************************************
// �������ͷ�ļ�
#include <setupapi.h>
#include "hidsdi.h"
#include "TabCtrl.h"


//*************************************************************************
// �궨��
const static WORD VENDOR_ID		= 0x0925;
const static WORD PRODUCT_ID	= 0x1234;

#define		WM_READ_HID_REPORT		(WM_USER + 1)
#define		WM_NEWDEV_MOUNT			(WM_USER + 2)

#define		DEV_TYPE_USBHID			1


//*************************************************************************
// ���ݽṹ����

// �Ѽ����豸
typedef struct _tagDEVSET
{
	UINT32		nType;			// �豸����
	PVOID		pVoid;			// �豸����ָ��
} DEVSET, *PDEVSET;

// �Ի����������
typedef struct _tagDIALOGINFO
{
	WORD		nVendorID;		// ���ұ�ʶ
	WORD		nProductID;		// ��Ʒ��ʶ

//	HANDLE		hDevInfo;
//	ULONG		Length;
//	ULONG		Required;
//	HANDLE		DeviceHandle;
//	LPSTR		MyDevicePathName;
	HIDP_CAPS	Capabilities;
	HANDLE		WriteHandle;
	HANDLE		ReadHandle;
	HANDLE		hEventObject;
	OVERLAPPED	HIDOverlapped;

	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;
} DIALOGINFO, *LPDIALOGINFO;

// �û����ݴ�ŵ�
typedef struct _tagUSERDATA
{
	HINSTANCE	hInstance;			// Ӧ��ʵ�����
	HWND		hMainDlg;			// DIALOG���ھ��
	DIALOGINFO	DialogInfo;			// DIALOG��������
	HFONT		hFont;				// ������ʹ�õ�Ĭ������

	// Tab
	TABCTRL		hDlgTab;

	// �豸��ر���
	GUID		sHidGuid;			// USBHID��ʽ������ID
	UINT32		nRunDevTotal;		// �ѹҽ��豸����
	UINT32		nRunDevIndex;		// ��ǰ����ʹ�õ��豸���
	DEVSET		sRunDev[16];		// �ѹҽ��豸������Ϣ

	HWND		hComboNewDev;		// �ѹҽ��豸�б�

	HWND		hwndRichEditScript;	// 
	WNDPROC		wndProcScript;		// 

	HWND		hwndEditVendorID;
	HWND		hwndEditProductID;
	HWND		hwndEditCLA;
	HWND		hwndEditINS;
	HWND		hwndEditP1;
	HWND		hwndEditP2;
	HWND		hwndEditP3;
	HWND		hwndEditSendData;
	HWND		hwndEditReceivedData;
	HWND		hwndEditStatus;

	// �߳���ر���
	HANDLE		hDevManagerThread;
	UINT32		nDevManagerThreadID;
} USERDATA, *LPUSERDATA;


//***************************************************************************
// ��������

// ��ʼ��DIALOG�ؼ�
extern	LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// ���� WM_COMMAND ��Ϣ
extern	LRESULT Dialog_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// ���� WM_READ_HID_REPORT ��Ϣ
extern	LRESULT Dialog_OnHID_ReadReport ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

extern	LRESULT Dialog_OnNewDevMount ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// �ҽ����豸
extern	LRESULT Dialog_OnDeviceChange ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

#endif	__DIALOG_H__

//***************************************************************************
// END
//***************************************************************************
