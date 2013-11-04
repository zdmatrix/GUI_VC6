//*************************************************************************
// APDU处理

#ifndef __APDU_H__
#define	__APDU_H__


//*************************************************************************
// 包含相关头文件


//*************************************************************************
// 宏定义

// APDU指令
#define APDU_GSM_READBINARY			0xA0B0		// Read BInary
#define APDU_GSM_UPDATABINARY		0xA0D6		// Update Binary

#define	APDU_UKEY_EINKDRIVE			0xBFAB		// 显示字符至EInk
#define APDU_UKEY_STARTKEY			0XBFAC		// 开始等待按键响应
#define APDU_UKEY_GETKEYDOWN		0XBFAD		// 取按键状态
#define	APDU_UKEY_FLASHWRITE		0xBFAE		// Flash写
#define	APDU_UKEY_FLASHREAD			0xBFAF		// Flahs读

// UKEY 参数定义
#define UKEY_CASH					0x0018		// 现金余额
#define UKEY_UKEY2					0x0019		// 账户余额


//*************************************************************************
// 数据结构定义


//***************************************************************************
// 函数声明

extern WORD APDU_Command ( LPUSERDATA lpUserData, WORD nCmd, WORD wParam, UINT32 nLen, LPBYTE lpByte );

#endif	__APDU_H__

//***************************************************************************
// END
//***************************************************************************
