//*************************************************************************
// 模态对话框头文件

#ifndef __DIALOG_H__
#define	__DIALOG_H__


//*************************************************************************
// 包含相关头文件
#include <setupapi.h>
#include "hidsdi.h"
#include "TabCtrl.h"


//*************************************************************************
// 宏定义
const static WORD VENDOR_ID		= 0x0925;
const static WORD PRODUCT_ID	= 0x1234;

#define		WM_READ_HID_REPORT		(WM_USER + 1)
#define		WM_NEWDEV_MOUNT			(WM_USER + 2)

#define		DEV_TYPE_USBHID			1


//*************************************************************************
// 数据结构定义

// 已加载设备
typedef struct _tagDEVSET
{
	UINT32		nType;			// 设备类型
	PVOID		pVoid;			// 设备数据指针
} DEVSET, *PDEVSET;

// 对话框相关数据
typedef struct _tagDIALOGINFO
{
	WORD		nVendorID;		// 厂家标识
	WORD		nProductID;		// 产品标识

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

// 用户数据存放点
typedef struct _tagUSERDATA
{
	HINSTANCE	hInstance;			// 应用实例句柄
	HWND		hMainDlg;			// DIALOG窗口句柄
	DIALOGINFO	DialogInfo;			// DIALOG所用数据
	HFONT		hFont;				// 程序所使用的默认字体

	// Tab
	TABCTRL		hDlgTab;

	// 设备相关变量
	GUID		sHidGuid;			// USBHID方式管理器ID
	UINT32		nRunDevTotal;		// 已挂接设备总数
	UINT32		nRunDevIndex;		// 当前正在使用的设备序号
	DEVSET		sRunDev[16];		// 已挂接设备描述信息

	HWND		hComboNewDev;		// 已挂接设备列表

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

	// 线程相关变量
	HANDLE		hDevManagerThread;
	UINT32		nDevManagerThreadID;
} USERDATA, *LPUSERDATA;


//***************************************************************************
// 函数声明

// 初始化DIALOG控件
extern	LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// 处理 WM_COMMAND 消息
extern	LRESULT Dialog_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// 处理 WM_READ_HID_REPORT 消息
extern	LRESULT Dialog_OnHID_ReadReport ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

extern	LRESULT Dialog_OnNewDevMount ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

// 挂接新设备
extern	LRESULT Dialog_OnDeviceChange ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );

#endif	__DIALOG_H__

//***************************************************************************
// END
//***************************************************************************
