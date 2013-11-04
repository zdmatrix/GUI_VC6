//*************************************************************************
// 文件: Devices.h
//***************************************************************************

#ifndef __DEVICES_H__
#define	__DEVICES_H__

#include "Dialog.h"


//*************************************************************************
// 数据结构定义

// USB-HID 设备ID
typedef struct _tagVPID
{
	WORD	nVendorID;
	WORD	nProductID;
	CHAR	szDesc[64];
} VPID, *PVPID;

//
typedef struct _tagUSBHID
{
	VPID		sVPID;
	GUID		*pHidGuid;
	CHAR		szDevPath[256];

	HIDP_CAPS	sCapabilities;
	HANDLE		hWrite;
	HANDLE		hRead;
	HANDLE		hEventObject;
	OVERLAPPED	HIDOverlapped;
} USBHID, *PUSBHID;

//***************************************************************************
// 函数声明

extern void DeviceManager ( LPUSERDATA lpUD );

#endif	__DEVICES_H__

//***************************************************************************
// END
//***************************************************************************
