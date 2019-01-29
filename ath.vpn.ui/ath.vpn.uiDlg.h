
// ath.vpn.uiDlg.h : файл заголовка
//

#pragma once

#include "ATHClientIfc.h"
#include "LogCStatic.h"
#include "FirstStart.h"

// диалоговое окно CathvpnuiDlg
class CathvpnuiDlg : public CDialogEx
{
// Создание
public:
	CathvpnuiDlg(CWnd* pParent = NULL);	// стандартный конструктор
	~CathvpnuiDlg();
// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATHVPNUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	HANDLE hPipe;
	ATHClientIfc *ath;
	FirstStart *fs;

public:
	CEdit ec_login;
	CEdit ec_password;
	LogCStatic s_status;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedOk();
	CButton ButtonOk;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
};
