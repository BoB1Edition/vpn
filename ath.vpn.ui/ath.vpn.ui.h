
// ath.vpn.ui.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CathvpnuiApp:
// � ���������� ������� ������ ��. ath.vpn.ui.cpp
//

class CathvpnuiApp : public CWinApp
{
public:
	CathvpnuiApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CathvpnuiApp theApp;