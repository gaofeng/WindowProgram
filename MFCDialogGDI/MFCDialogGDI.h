
// MFCDialogGDI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCDialogGDIApp:
// �йش����ʵ�֣������ MFCDialogGDI.cpp
//

class CMFCDialogGDIApp : public CWinApp
{
public:
	CMFCDialogGDIApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCDialogGDIApp theApp;