
// MFC_with_OpenCV.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMFC_with_OpenCVApp:
// �� Ŭ������ ������ ���ؼ��� MFC_with_OpenCV.cpp�� �����Ͻʽÿ�.
//

class CMFC_with_OpenCVApp : public CWinApp
{
public:
	CMFC_with_OpenCVApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMFC_with_OpenCVApp theApp;