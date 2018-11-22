
// ath.vpn.uiDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "ath.vpn.ui.h"
#include "ath.vpn.uiDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// диалоговое окно CathvpnuiDlg



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
	DDX_Control(pDX, IDOK, ButtonOk);
}

BEGIN_MESSAGE_MAP(CathvpnuiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CathvpnuiDlg::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// обработчики сообщений CathvpnuiDlg

BOOL CathvpnuiDlg::OnInitDialog()
{
	fs = new FirstStart();
	//fs->Create(IDD_ATHProgress, fs);
	//ProgressBar->DoModal();
	//ProgressBar->ShowWindow(SW_SHOW);
	if (fs->DoModal() < 0) {
		int i = GetLastError();
		//IErrorInfo *ei;
		//GetErrorInfo(0, &ei);
	}


	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	LPWSTR username = new WCHAR[300];
	DWORD dw = 300;
	GetUserName(username, &dw);
	hPipe = NULL;
	if (StrCmpIW(L"azhokhov", username) == 0 || StrCmpIW(L"athuser", username) == 0) {
		ath = new ATHClientIfc(&s_status);
		
	}
	else {
		LPWSTR message = new WCHAR[3000];
		wsprintf(message, L"User: %s cannot use this program\0", username);
		MessageBox(message);
		ExitProcess(500);
		return FALSE;
	}
	//ShowWindow(SW_MINIMIZE);

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CathvpnuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CathvpnuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CathvpnuiDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	hPipe = NULL;
	return 0;
}


void CathvpnuiDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

}


void CathvpnuiDlg::OnBnClickedOk()
{
	ButtonOk.EnableWindow(false);
	if(hPipe == NULL)
		hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[200];
		wsprintf(mess, L"Незапущенна служба ath.vpn.service0: %i", err);
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	if (ath->GetStatus() <= 0) {
		CloseHandle(hPipe);
		ExitProcess(-1);
	}

	VPNCOMMAND command;
	DWORD pid = GetCurrentProcessId();
	command.messsage = pid;
	command.command = CHECKAV;
	DWORD dwByte;
	if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	if (command.command != AVOK) {
		LPTSTR  mess = new WCHAR[250];
		wsprintf(mess, L"У Вас проблема с антивирусом, включите его.");
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	command.messsage = pid;
	command.command = CHECKFW;
	if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	if (command.command != FWOK) {
		command.messsage = pid;
		command.command = CHANGEFW;
		if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
			int err = GetLastError();
			LPTSTR  mess = new WCHAR[50];
			wsprintf(mess, L"err: %i", err);
			MessageBox(mess);
			ButtonOk.EnableWindow(true);
			return;
		}
		ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	}
	command.messsage = pid;
	command.command = CHANGEFW;
	if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	if (command.command != CHANGEFW) {
		LPTSTR  mess = new WCHAR[250];
		wsprintf(mess, L"У Вас проблема с фаерволом, включите его");
		MessageBox(mess);
		ButtonOk.EnableWindow(true);
		return;
	}
	ath->GetStatus();
	int ul = ec_login.GetWindowTextLengthW() + 1;
	int pl = ec_password.GetWindowTextLengthW() + 1;
	LPTSTR u = new WCHAR[ul];
	LPTSTR p = new WCHAR[pl];
	ec_login.GetWindowTextW(u, ul);
	ec_password.GetWindowTextW(p, pl);
	if (ath->connect(L"gate1.ath.ru", u, p) && ath->ConnectRDP()) {
		ShowWindow(SW_MINIMIZE);
		//ButtonOk.SetWindowTextW(L"Disconnect");
	}
	else {
		unsigned int i = GetLastError();
		//MessageBox(L"Not connected", 0, i);
	}
	delete u, p;
}


void CathvpnuiDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	ath->deleteCred();
	if (hPipe == NULL && ath->isConnected())
		hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	VPNCOMMAND command;
	DWORD pid = GetCurrentProcessId();
	command.messsage = pid;
	command.command = RESTOREFW;
	DWORD dwByte = 0;
	WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL);
	delete ath;
}
