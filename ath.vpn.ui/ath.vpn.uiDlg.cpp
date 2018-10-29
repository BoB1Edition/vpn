
// ath.vpn.uiDlg.cpp : ���� ����������
//

#include "stdafx.h"
#include "ath.vpn.ui.h"
#include "ath.vpn.uiDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���������� ���� CathvpnuiDlg



CathvpnuiDlg::CathvpnuiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ATHVPNUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CathvpnuiDlg::~CathvpnuiDlg()
{
	CloseHandle(hPipe);
}

void CathvpnuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIN, ec_login);
	DDX_Control(pDX, IDC_PASSWORD, ec_password);
	DDX_Control(pDX, IDC_STATUS, s_status);
}

BEGIN_MESSAGE_MAP(CathvpnuiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CathvpnuiDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ����������� ��������� CathvpnuiDlg

BOOL CathvpnuiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ������ ������ ��� ����� ����������� ����.  ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	ShowWindow(SW_MINIMIZE);

	// TODO: �������� �������������� �������������

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
}

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������.  ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void CathvpnuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �������� ���������� ��� ���������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ������������ ������ �� ������ ����������� ��������������
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ��������� ������
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR CathvpnuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CathvpnuiDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void CathvpnuiDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

}


void CathvpnuiDlg::OnBnClickedOk()
{
	if (ath == NULL) {
		ath = new ATHClientIfc(&s_status);
	}
	hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
		return;
	}
	if (ath->GetStatus() <= 0) {
		CloseHandle(hPipe);
		ExitProcess(-1);
	}

	VPNCOMMAND command;
	DWORD pid = GetCurrentProcessId();;
	command.messsage = pid;
	command.command = tagVPNCOMMAND::CHANGEFW;
	DWORD dwByte;
	if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
	}
	ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	CloseHandle(hPipe);
	/*if (command.messsage == pid || command.command == AVOK) {
		command.command = tagVPNCOMMAND::CHECKAV;
		if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
			int err = GetLastError();
			LPTSTR  mess = new WCHAR[50];
			wsprintf(mess, L"err: %i", err);
			MessageBox(mess);
		}
	}*/

	//CDialogEx::OnOK();
}
