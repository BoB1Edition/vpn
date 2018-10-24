
// ath.vpn.ui.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CathvpnuiApp:
// О реализации данного класса см. ath.vpn.ui.cpp
//

class CathvpnuiApp : public CWinApp
{
public:
	CathvpnuiApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CathvpnuiApp theApp;