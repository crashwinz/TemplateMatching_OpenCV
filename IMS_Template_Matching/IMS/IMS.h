
// IMS.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CIMSApp:
// �� Ŭ������ ������ ���ؼ��� IMS.cpp�� �����Ͻʽÿ�.
//

class CIMSApp : public CWinApp
{
public:
	CIMSApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CIMSApp theApp;