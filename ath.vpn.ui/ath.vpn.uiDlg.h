
// ath.vpn.uiDlg.h : ���� ���������
//

#pragma once

#include "ATHClientIfc.h"
#include "LogCStatic.h"
#include "ATHProgressBar.h"

// ���������� ���� CathvpnuiDlg
class CathvpnuiDlg : public CDialogEx
{
// ��������
public:
	CathvpnuiDlg(CWnd* pParent = NULL);	// ����������� �����������
	~CathvpnuiDlg();
// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATHVPNUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	HANDLE hPipe;
	ATHClientIfc *ath;
	ATHProgressBar *ProgressBar;

public:
	CEdit ec_login;
	CEdit ec_password;
	LogCStatic s_status;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedOk();
	CButton ButtonOk;
	afx_msg void OnDestroy();
};
