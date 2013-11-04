//*************************************************************************
// 文件：APDU.c
// 内容：APDU处理
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include "Global.h"
#include "Devices.h"

#include "APDU.h"


//*************************************************************************
// 宏定义


//*************************************************************************
//*************************************************************************

//*************************************************************************
// 函数: APDU_Command
// 功能: APDU指令通用函数
//*************************************************************************
WORD APDU_Command ( LPUSERDATA lpUserData, WORD nCmd, WORD wParam, UINT32 nLen, LPBYTE lpByte )
{
	static BYTE	OutputReport[288];
	static BYTE	InputReport[288];
	LPDULNODE	lpDuLNode = NULL;
	WORD		wResult;

	// 防止空指针传入
	if ( !lpUserData )
		return -1;

	// 清空缓冲区
	memset ( OutputReport, 0x00, 288 );
	OutputReport[0] = 0x00;				// 报告ID 必须为 0
	OutputReport[2] = HIBYTE(nCmd);		// CLA
	OutputReport[3] = LOBYTE(nCmd);		// INS
	OutputReport[4] = HIBYTE(wParam);	// P1
	OutputReport[5] = LOBYTE(wParam);	// P2
	OutputReport[6] = nLen;				// P3

	switch ( nCmd )
	{
	// 类型一
	case APDU_GSM_UPDATABINARY:
	case APDU_UKEY_FLASHWRITE:
	case APDU_UKEY_EINKDRIVE:
	case APDU_UKEY_STARTKEY:
	case APDU_UKEY_GETKEYDOWN:
		OutputReport[1] = 0x05 + nLen;		// 数据长度(包括命令头)
		if ( nLen && lpByte )
			memcpy ( &OutputReport[7], lpByte, nLen );
		break;

	// 类型二
	case APDU_UKEY_FLASHREAD:
	case APDU_GSM_READBINARY:
		OutputReport[1] = 0x05;				// 数据长度(包括命令头)
		break;
	}

	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );

	// 将数据写入设备
	wResult = GDEVOP.Send ( lpDuLNode->lpData, OutputReport, InputReport );

	switch ( nCmd )
	{
	// 类型二
	case APDU_UKEY_FLASHREAD:
	case APDU_GSM_READBINARY:
		if ( nLen && lpByte )
			memcpy ( lpByte, &InputReport[3], nLen );
		break;
	}

	return wResult;
}


//***************************************************************************
// END
//***************************************************************************
