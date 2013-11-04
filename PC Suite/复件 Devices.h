//*************************************************************************
// �ļ�: Devices.h
//***************************************************************************

#ifndef __DEVICES_H__
#define	__DEVICES_H__

#include "Dialog.h"


//*************************************************************************
// ���ݽṹ����

// USB-HID �豸ID
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
// ��������

extern void DeviceManager ( LPUSERDATA lpUD );

#endif	__DEVICES_H__

//***************************************************************************
// END
//***************************************************************************
