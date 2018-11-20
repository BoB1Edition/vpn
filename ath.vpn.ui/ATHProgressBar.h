#pragma once
#include "stdafx.h"
#include <afxdialogex.h>


class ATHProgressBar :
	public CDialog
{
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATHDIALOGBAR };
#endif
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnSend();
	//afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIdEvent);
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl cProcBar;
	afx_msg void OnBnClickedButton1();
};

