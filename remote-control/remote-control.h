
// remote-control.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CremoteControlApp: 
// �йش����ʵ�֣������ remote-control.cpp
//

class CremoteControlApp : public CWinApp
{
public:
	CremoteControlApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CremoteControlApp theApp;