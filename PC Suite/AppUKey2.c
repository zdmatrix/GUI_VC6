//*************************************************************************
// 文件：AppUKey2.c
// 内容：演示安全交易
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"
#include "APDU.h"

#include "Devices.h"


//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量
static char g_szInput1[256];
static char g_szInput2[256];
static char g_szInput3[256];
static char g_szInput4[256];


typedef unsigned    char        UINT8;
UINT8		g_c1[256] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c2[256] =
{
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c3[256] =
{
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c4[256] =
{
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c5[256] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c6[256] =
{
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c7[256] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c8[256] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

UINT8		g_c9[256] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00
};

typedef struct _tagPOINT
{
	UINT32	x;
	UINT32	y;
} PNT;

const static PNT ORG[6] =
{
	{ 0,  0 },	// 1
	{ 7,  0 },	// 2
	{ 0,  7 },	// 3
	{ 7,  7 },	// 4
	{ 0, 14 },	// 5
	{ 7, 14 }	// 6
};

const static UINT32 NUM0[8]  = { 7, 1, 2, 4, 6, 5, 3, 1 };
const static UINT32 NUM1[3]  = { 2, 2, 6 };
const static UINT32 NUM2[7]  = { 6, 1, 2, 4, 3, 5, 6 };
const static UINT32 NUM3[8]  = { 7, 1, 2, 4, 3, 4, 6, 5 };
const static UINT32 NUM4[7]  = { 6, 1, 3, 4, 2, 4, 6 };
const static UINT32 NUM5[7]  = { 6, 2, 1, 3, 4, 6, 5 };
const static UINT32 NUM6[8]  = { 7, 2, 1, 3, 4, 6, 5, 3 };
const static UINT32 NUM7[4]  = { 3, 1, 2, 6 };
const static UINT32 NUM8[10] = { 9, 1, 2, 4, 6, 5, 3, 1, 3, 4 };
const static UINT32 NUM9[8]  = { 7, 4, 2, 1, 3, 4, 6, 5 };

const static UINT32 *NUM09[10] = { NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9 };

static WORD gnBalance;
static WORD gnMoney;

//*************************************************************************
// 此代码模块中包含的函数的向前声明


//*************************************************************************
//*************************************************************************

//*************************************************************************
// 函数: AppUKey2_Finish ()
// 功能: 完成交易
//*************************************************************************
LRESULT AppUKey2_Finish ( LPUSERDATA lpUserData )
{
	MessageBox ( lpUserData->hMainDlg, "交易成功!!!", "交易完成", MB_OK );
	return TRUE;
}

//*************************************************************************
// 函数: AppUKey2_Data2Card ()
// 功能: 按键处理
//*************************************************************************
LRESULT AppUKey2_Data2Card ( LPUSERDATA lpUserData )
{
	if ( GetDlgItemText ( lpUserData->sTabCtrl.hTabPages[2], IDC_EDIT_2UKEY_1, (LPSTR)g_szInput1, 11 ) != 10 )
	{
		MessageBox ( lpUserData->hMainDlg, "请输入10位用户帐号！", "错误", MB_OK );
		return FALSE;
	}
	if ( GetDlgItemText ( lpUserData->sTabCtrl.hTabPages[2], IDC_EDIT_2UKEY_2, (LPSTR)g_szInput2, 11 ) != 10 )
	{
		MessageBox ( lpUserData->hMainDlg, "请输入10位目的帐号！", "错误", MB_OK );
		return FALSE;
	}
	GetDlgItemText ( lpUserData->sTabCtrl.hTabPages[2], IDC_EDIT_2UKEY_3, (LPSTR)g_szInput3, 7 );

	gnMoney = atoi ( g_szInput3 );
	if ( gnMoney > gnBalance )
	{
		MessageBox ( lpUserData->hMainDlg, "账户余额不足！", "错误", MB_OK );
		return FALSE;
	}

	lpUserData->nPaint = 0x55AA;
	InvalidateRect ( lpUserData->sTabCtrl.hTabPages[2], NULL, FALSE );

	return TRUE;
}

//*************************************************************************
// 函数: AppUKey2_OnCommand ()
// 功能: 按键处理
//*************************************************************************
LRESULT AppUKey2_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

	// 确保私有数据的正确性
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}
	
	// 确保有设备联接
	lpDuLNode = GNODEOP.Find ( lpUserData->lpModNode, APPMOD_DEVICE );
	if ( !GDEVOP.GetNumber ( lpDuLNode->lpData ) ) return -1;
	
	// OnCommand消息的细节
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier
	
	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// 按键点击消息
		if ( wID == IDC_BUTTON_2UKEY_2CARD )
			AppUKey2_Data2Card ( lpUserData );
		else
			return -1;
		break;

	default:
		return -1;
	}

	return TRUE;
}

//*************************************************************************
// 画线函数
//*************************************************************************
// 布雷森汉姆直线算法 bresenham
// 参考资料来源 http://zh.wikipedia.org
//	function line(x0, x1, y0, y1)
// 		boolean steep := abs(y1 - y0) > abs(x1 - x0)
// 		if steep then
// 			swap(x0, y0)
// 			swap(x1, y1)
// 		if x0 > x1 then
// 			swap(x0, x1)
// 			swap(y0, y1)
// 		int deltax := x1 - x0
// 		int deltay := abs(y1 - y0)
// 		int error := deltax / 2
// 		int ystep
// 		int y := y0
// 		if y0 < y1 then ystep := 1 else ystep := -1
// 		for x from x0 to x1
// 			if steep then plot(y,x) else plot(x,y)
// 			error := error - deltay
// 			if error < 0 then
// 				y := y + ystep
//				error := error + deltax
//*************************************************************************
#define swap(x,y){int s=x;x=y;y=s;}

UINT32 draw_line ( HDC    hDC,
				   UINT32 x0,
				   UINT32 y0,
				   UINT32 x1,
				   UINT32 y1,
				   UINT32 color )
{
	UINT32	steep = abs ( y1 - y0 ) > abs ( x1 - x0 );
	UINT32	deltax;
	UINT32	deltay;
	INT32	error;
	UINT32	ystep;
	UINT32	x, y;

	if ( steep )
	{
		swap ( x0, y0 );
		swap ( x1, y1 );
	}

	if ( x0 > x1 )
	{
		swap ( x0, x1 );
		swap ( y0, y1 );
	}

	deltax = x1 - x0;

	deltay = abs ( y1 - y0 );

	error = deltax / 2;

	y = y0;

	if ( y0 < y1 )
		ystep = 1;
	else
		ystep = -1;

	for ( x = x0; x <= x1; x ++ )
	{
		if ( steep )
			SetPixel ( hDC, y, x, color );
		else
			SetPixel ( hDC, x, y, color );

		error = error - deltay;

		if ( error < 0 )
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

	return 0;
}

//*************************************************************************
// 显示边框
void Display_Frame ( HDC hDC, UINT32 nx, UINT32 ny, UINT32 nw, UINT32 nh )
{
	// 上边框
	draw_line ( hDC, nx,          ny,          nx + nw - 1, ny,          0x00808080 );
	draw_line ( hDC, nx + 1,      ny + 1,      nx + nw - 2, ny + 1,      0x00404040 );

	// 左边框
	draw_line ( hDC, nx,          ny + 1,      nx,          ny + nh - 1, 0x00808080 );
	draw_line ( hDC, nx + 1,      ny + 2,      nx + 1,      ny + nh - 2, 0x00404040 );
	
	// 右边框
	draw_line ( hDC, nx + nw,     ny,          nx + nw,     ny + nh,     0x00FFFFFF );
	draw_line ( hDC, nx + nw - 1, ny + 1,      nx + nw - 1, ny + nh - 1, 0x00D4D0C8 );
	
	// 下边框
	draw_line ( hDC, nx,          ny + nh,     nx + nw - 1, ny + nh,     0x00FFFFFF );
	draw_line ( hDC, nx + 1,      ny + nh - 1, nx + nw - 2, ny + nh - 1, 0x00D4D0C8 );

	return;
}

//*************************************************************************
// 显示干扰底图
void Display_BG ( HDC hDC, UINT32 nx, UINT32 ny, UINT32 nw, UINT32 nh )
{
	UINT32	i;
	UINT32	w1, h1, w2, h2;
	HPEN	hOldPen = NULL;
	UINT32	color;

	for ( i = 0; i < 15; i++ )
	{
		color = rand() % 0xFFFFFF;

		w1 = rand() % (nw + 10);
		h1 = rand() % (nh + 10);
		w2 = rand() % (nw + 10);
		h2 = rand() % (nh + 10);

		draw_line ( hDC, nx + w1 - 5, ny + h1 - 5, nx + w2 - 5, ny + h2 - 5, color );
	}

	SelectObject ( hDC, hOldPen );

	return;
}

//*************************************************************************
// 显示数据
void Display_Font ( HDC hDC, UINT32 nx, UINT32 ny, UINT8 *pFont )
{
	UINT32	x, y;

	for ( y = 0; y < 16; y++ )
	{
		for ( x = 0; x < 16; x++ )
		{
			if ( pFont[x + (y * 16)] != 00 )
				SetPixel ( hDC, nx + x, ny + y, 0x000000 );
		}
	}

	return;
}

//*************************************************************************
// 显示扭曲的7段码
void Display_7Duan ( HDC hDC, UINT32 nx, UINT32 ny, const UINT32 *pFont )
{
	UINT32		z = 0;
	PNT			p[6];
	UINT32		x1, y1, x2 = 0, y2 = 0;

	for ( z = 0; z < 6; z++ )
	{
		p[z].x = ORG[z].x + rand() % 3;
		p[z].y = ORG[z].y + rand() % 3;
	}
	
	x1 = p[pFont[1] - 1].x;
	y1 = p[pFont[1] - 1].y;
	for ( z = 2; z <= pFont[0]; z++ )
	{
		x2 = p[pFont[z] - 1].x;
		y2 = p[pFont[z] - 1].y;
		draw_line ( hDC, nx + x1, ny + y1, nx + x2, ny + y2, 0 );

		// 将边线加宽
		if ( x1 < 7 && x2 < 7 )
			draw_line ( hDC, nx + x1 - 1, ny + y1, nx + x2 - 1, ny + y2, 0 );
		else if ( x1 >= 7 && x2 >= 7 )
			draw_line ( hDC, nx + x1 + 1, ny + y1, nx + x2 + 1, ny + y2, 0 );
		else if ( y1 < 7 && y2 < 7 )
			draw_line ( hDC, nx + x1, ny + y1 - 1, nx + x2, ny + y2 - 1, 0 );
		else
			draw_line ( hDC, nx + x1, ny + y1 + 1, nx + x2, ny + y2 + 1, 0 );

		// 交换坐标
		x1 = x2;
		y1 = y2;
		x2 = 0;
		y2 = 0;
	}

	return;
}

//*************************************************************************
// 函数: AppUKey2_OnPaint ()
// 功能: 显示图形
//*************************************************************************
LRESULT AppUKey2_OnPaint ( LPUSERDATA lpUserData, HWND hWnd )
{
	PAINTSTRUCT	ps;
	HDC			hDC;
	UINT32		x = 20, y = 50, w = 0, h = 0;
	UINT32		nMaxX = 0, nMaxY = 0;
	UINT32		i, num;
	SYSTEMTIME	time ;

	if ( lpUserData->nPaint != 0x55AA )
		return FALSE;

	GetLocalTime ( &time );
	srand( (unsigned)time.wSecond );
	
	hDC = BeginPaint ( hWnd, &ps );

	// 显示边框 ====================================================
	x = 21;
	y = 345;
	w = 260;
	h = 98;
	Display_Frame ( hDC, x, y, w, h );
	Display_BG ( hDC, x + 8, y + 8, w - 16, h - 16 );

	// 目的帐号 ====================================================
	x += 10;
	y += 10;
	w = 0;
	h = 0;
	Display_BG ( hDC, x, y, 16 * 4, 16 );
	Display_Font ( hDC, x + w, y + h, g_c1 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c2 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c3 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c4 );
	w += 16;
	w += 16;

	// 显示帐号数字
	Display_BG ( hDC, x + w, y + h, 16 * 10, 16 );
	for ( i = 0; i < 10; i++ )
	{
		num = g_szInput2[i] & 0x0F;
		Display_7Duan ( hDC, x + w, y + h, NUM09[num] );
		w += 16;
	}


	// 金额 ====================================================
	w = 0;
	h += (16 * 2);
	Display_BG ( hDC, x, y + h, 16 * 2, 16 );
	Display_Font ( hDC, x + w, y + h, g_c5 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c6 );
	w += (16 * 4);
	
	// 显示金额
	Display_BG ( hDC, x + w, y + h, 16 * 6, 16 );
	for ( i = 0; i < strlen(g_szInput3); i++ )
	{
		//		num = rand() % 10;
		num = g_szInput3[i] & 0x0F;
		Display_7Duan ( hDC, x + w, y + h, NUM09[num] );
		w += 16;
	}

	// 验证码 ====================================================
	w = 0;
	h += (16 * 2);
	Display_BG ( hDC, x + w, y + h, 16 * 3, 16 );
	Display_Font ( hDC, x + w, y + h, g_c7 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c8 );
	w += 16;
	Display_Font ( hDC, x + w, y + h, g_c9 );
	w += (16 * 3);
	
	// 显示验证码
	Display_BG ( hDC, x + w, y + h, 16 * 4, 16 );
	for ( i = 0; i < 4; i++ )
	{
		num = rand() % 10;
		g_szInput4[i] = '0' + num;
		Display_7Duan ( hDC, x + w, y + h, NUM09[num] );
		w += 16;
	}
	g_szInput4[i] = 0x00;
	PostMessage ( lpUserData->hMainDlg, WM_UKEY2_SENDCARD, (WPARAM)g_szInput4, 0 );

	EndPaint ( hWnd, &ps );

	return FALSE;
}

//*************************************************************************
// 函数: AppUKey2_SendCard ()
// 功能: 显示验证码
//*************************************************************************
LRESULT AppUKey2_SendCard ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	WORD		wResult;
	UINT32		nLen;

	// ***************   显示添加余额   *****************************************
	// EInk显示
	nLen = strlen ( (char *)wParam );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, (char *)wParam );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   可视卡按键等待显示余额   *****************************************
	// 加载将要传送的数据
	wResult = APDU_Command ( lpUserData, APDU_UKEY_STARTKEY, UKEY_UKEY2, 0x00, NULL );
	if ( wResult != 0x9000 )
		return -1;

	// ***************   等待按键回应   *****************************************
	Sleep ( 1000 );
	PostMessage ( lpUserData->hMainDlg, WM_UKEY2_WAITKEY, 0, 0 );

	lpUserData->nPaint = 0;
	return TRUE;
}


//*************************************************************************
// 函数: AppUKey2_InitBalance
// 功能: 初始化账户余额
//*************************************************************************
UINT32 AppUKey2_InitBalance ( LPUSERDATA lpUserData, WORD wBalance )
{
	WORD	wResult;

	if ( wBalance == 0xFFFF )
	{
		wBalance = 1000;

		wResult = APDU_Command ( lpUserData, APDU_UKEY_FLASHWRITE, UKEY_UKEY2, 0x02, (LPBYTE)&wBalance );
		if ( wResult != 0x9000 )
			return -1;
	}

	return wBalance;
}

//*************************************************************************
// 函数: AppUKey2_GetBalance ()
// 功能: 显示验证码
//*************************************************************************
LRESULT AppUKey2_GetBalance ( LPUSERDATA lpUserData )
{
	WORD	wResult;
	PCHAR	pString = NULL;
	WORD	nBalance;
	UINT32	nLen;

	// 初始化账户
	gnBalance = nBalance = 1000;
	MacroAlloc ( pString, 256 );
	sprintf ( pString, "%d", nBalance );
	SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[2], IDC_EDIT_UKEY_BALANCE, pString );
	
	// EInk显示
	nLen = strlen ( pString );
	wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
	MacroFree ( pString );
	if ( wResult != 0x9000 )
		return -1;

	return 0;
}

void AppUKey2_WaitKey ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	WORD		wResult;

	// 等待卡端按键
	wResult = APDU_Command ( lpUserData, APDU_UKEY_GETKEYDOWN, UKEY_UKEY2, 0x00, NULL );
	if ( wResult != 0x9000 )
	{
		Sleep ( 1000 );
		PostMessage ( lpUserData->hMainDlg, WM_UKEY2_WAITKEY, 0, 0 );
	}
	else
	{
		PCHAR	pString = NULL;
		UINT32	nLen;

		MacroAlloc ( pString, 256 );
		gnBalance -= gnMoney;
 		sprintf ( pString, "交易完成！\n消费金额%d元，余额%d元。", gnMoney, gnBalance );
		MessageBox ( lpUserData->hMainDlg, pString, "交易完成", MB_OK );

		sprintf ( pString, "%d", gnBalance );
		SetDlgItemText ( lpUserData->sTabCtrl.hTabPages[2], IDC_EDIT_UKEY_BALANCE, pString );

		// EInk显示
		nLen = strlen ( pString );
		wResult = APDU_Command ( lpUserData, APDU_UKEY_EINKDRIVE, 0x00, nLen, pString );
		MacroFree ( pString );
	}

	return;
}

//***************************************************************************
// END
//***************************************************************************
