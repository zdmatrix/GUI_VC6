//*************************************************************************
// ģ̬�Ի���ͷ�ļ�

#ifndef __GLOBAL_H__
#define	__GLOBAL_H__

#include "TabCtrl.h"


//*************************************************************************
// �궨��

#define		WM_READ_HID_REPORT		(WM_USER + 1)
#define		WM_DEVICE_MOUNT			(WM_USER + 2)
#define		WM_DEVICE_UNMOUNT		(WM_USER + 3)
#define		WM_UKEY2_SENDCARD		(WM_USER + 4)
#define		WM_UKEY2_WAITKEY		(WM_USER + 5)

//////////////////////////////////////////////////////////////////
// MacroFree �궨��												//
#define	MacroFree(x)											\
	if ( (x) != NULL )											\
	{															\
		free ( x );												\
		(x) = NULL;												\
	}

//////////////////////////////////////////////////////////////////
// MacroAlloc �궨��											//
#define	MacroAlloc(m,l)											\
	if ( (m) == NULL )											\
	{															\
		(m) = malloc (l);										\
		if ( (m) != NULL )										\
		{														\
			memset ( (m), 0x00, (l) );							\
		}														\
	}

//////////////////////////////////////////////////////////////////
// MacroTest �����												//
#define MacroTest(x)											\
	if ( x == NULL )											\
	{															\
		while ( TRUE );											\
	}


//*************************************************************************
// ���ݽṹ����

// ˫���������ݽṹ
typedef struct _tagDULNODE
{
	struct _tagDULNODE	*prior;		// ǰһ���ڵ㣬NULLΪͷ�ڵ�
	struct _tagDULNODE	*next;		// ��һ���ڵ㣬NULLΪβ�ڵ�

	LPVOID				lpData;		// �ڵ�����ָ��
	UINT32				nCustom;	// �ڵ��Զ���
} DULNODE, *LPDULNODE;

// �û����ݴ�ŵ�
typedef struct _tagUSERDATA
{
	HINSTANCE	hInstance;			// Ӧ��ʵ�����
	HWND		hMainDlg;			// DIALOG���ھ��
	HFONT		hFont;				// ������ʹ�õ�Ĭ������

	// ����ؼ����
	HWND		hComboNewDev;		// �ѹҽ��豸�б�
	TABCTRL		sTabCtrl;			// Tab�ؼ�
	UINT32		nTabSelect;			// ��ǰTAB��ѡ��ǩ����
	
	LPDULNODE	lpModNode;			// ģ��ר������ָ��

	// UKEY���
	UINT32		nPaint;				// �Ƿ���ʾ
} USERDATA, *LPUSERDATA;

// ģ�鶨��
typedef enum _tagAPPMOD
{
	APPMOD_DEVICE = 0,			// Ӳ���豸
	APPMOD_END					// �������
} APPMOD, *LPAPPMOD;

// ��������ӿ�
typedef struct _tagNODEOP
{
	LRESULT		(*Create	) (LPDULNODE *);												// ����һ��˫�������������Ѵ��������򷵻� -1��
	LRESULT		(*Destroy	) (LPDULNODE *, VOID (*CallBackFn)(LPVOID, UINT32));			// ��������
	LRESULT		(*Append	) (LPDULNODE, LPVOID, UINT32);									// ������β����ӽڵ㡣
	LRESULT		(*Delete	) (LPDULNODE *, UINT32, VOID (*CallBackFn)(LPVOID, UINT32));	// ɾ�������е�һ��ָ���ڵ�
	LPDULNODE	(*Find		) (LPDULNODE, UINT32);											// �������в����Զ�����ͬ�Ľڵ�
	LPDULNODE	(*GetNode	) (LPDULNODE, UINT32);											// ȡ������ָ����ŵĽڵ�ָ��
}NODEOP, *LPNODEOP;

extern const NODEOP GNODEOP;


//***************************************************************************
// ��������

extern PCHAR		Global_Hex2String	( PBYTE, UINT32 );				// ��16��������ת��Ϊ�ַ���


#endif	__GLOBAL_H__

//***************************************************************************
// END
//***************************************************************************
