
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
	try {
		// Try to open the mutex.
		HANDLE hMutex = OpenMutex(
			MUTEX_ALL_ACCESS, 0, L"AHT.vpn.dlg");

		if (!hMutex)
			// Mutex doesn’t exist. This is
			// the first instance so create
			// the mutex.
			hMutex =
			CreateMutex(0, 0, L"AHT.vpn.dlg");
		else {
			MessageBeep(10);

			CWnd *cwdd = FindWindow(L"#32770", L"");
			cwdd->ShowWindow(SW_MAXIMIZE | SW_SHOW);
			ExitProcess(0);
		}

		ReleaseMutex(hMutex);
	}
	catch (int err) {
		ExitProcess(-1);
	}
	return;
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
	ON_BN_CLICKED(IDCANCEL, &CathvpnuiDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// обработчики сообщений CathvpnuiDlg

BOOL CathvpnuiDlg::OnInitDialog()
{
	//fs = new FirstStart();
	//fs->Create(IDD_ATHProgress, fs);
	//ProgressBar->DoModal();
	//ProgressBar->ShowWindow(SW_SHOW);
	//if (fs->DoModal() < 0) {
	//	int i = GetLastError();
		//IErrorInfo *ei;
		//GetErrorInfo(0, &ei);
	//}


	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	LPWSTR username = new WCHAR[300];
	DWORD dw = 300;
	GetUserName(username, &dw);
	hPipe = NULL;
	if (StrCmpIW(L"azhokhov", username) == 0 || StrCmpIW(L"athuser", username) == 0 || StrCmpIW(L"developer", username) == 0) {
		ath = new ATHClientIfc(&s_status);
		
	}
	else {
		LPWSTR message = new WCHAR[3000];
		wsprintf(message, L"Пользователь %s не может запустить данную программу, Вам необходимо перезайти в компьютер под пользователем ATHUser. Хотите это сделать сейчас?\0", username);
		int Relogin = MessageBox(message, 0, MB_YESNO);
		if (Relogin == 6) {
			ExitWindows(0, 0);
		}
		ExitProcess(500);
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
	try {
		ButtonOk.EnableWindow(false);

		hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		int ul = ec_login.GetWindowTextLengthW() + 10;
		int pl = ec_password.GetWindowTextLengthW() + 10;
		LPTSTR u = new WCHAR[ul];
		LPTSTR p = new WCHAR[pl];
		int ecl = ec_login.GetWindowTextW(u, ul);
		std::wstring wstr(u);
		int f = wstr.find('@', 0);
		if (f > 0)
			wstr = wstr.substr(0, f);
		f = wstr.find('\\', 0);
		if (f > 0)
			wstr = wstr.substr(f + 1, wstr.length());
		u = (LPTSTR)wstr.c_str();
		int ecp = ec_password.GetWindowTextW(p, pl);
		if (ecl <= 0 && ecp <= 0) {
			MessageBox(L"Не введен логин и пароль");
			ButtonOk.EnableWindow(true);
			return;
		}
		ec_login.EnableWindow(false);
		ec_password.EnableWindow(false);
		if (hPipe == INVALID_HANDLE_VALUE) {
			int err = GetLastError();
			LPTSTR  mess = new WCHAR[200];
			wsprintf(mess, L"Незапущенна служба ath.vpn.service0: %i", err);
			MessageBox(mess);
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
			ButtonOk.EnableWindow(true);
			return;
		}
		//MessageBox(L"GetStatus");
		if (ath->GetStatus() < 0) {
			CloseHandle(hPipe);
			LPTSTR  mess = new WCHAR[200];
			wsprintf(mess, L"GetStatus");
			MessageBox(mess);
			ExitProcess(-1);
		}
		//MessageBox(L"");
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
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
			ButtonOk.EnableWindow(true);
			return;
		}
		BOOL r = ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
		//MessageBox(L"");
		if (command.command != AVOK) {
			LPTSTR  mess = new WCHAR[250];
			wsprintf(mess, L"У Вас проблема с антивирусом, включите его.");
			MessageBox(mess);
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
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
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
			ButtonOk.EnableWindow(true);
			return;
		}
		//MessageBox(L"");
		r = ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
		if (command.command != FWOK) {
			command.messsage = pid;
			command.command = CHANGEFW;
			if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
				int err = GetLastError();
				LPTSTR  mess = new WCHAR[50];
				wsprintf(mess, L"err: %i", err);
				MessageBox(mess);
				ec_login.EnableWindow(true);
				ec_password.EnableWindow(true);
				ButtonOk.EnableWindow(true);
				return;
			}
			//MessageBox(L"");
			r = ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
		}
		else {
			command.messsage = pid;
			command.command = CHANGEFW;
			if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
				int err = GetLastError();
				LPTSTR  mess = new WCHAR[50];
				wsprintf(mess, L"err: %i", err);
				MessageBox(mess);
				ec_login.EnableWindow(true);
				ec_password.EnableWindow(true);
				ButtonOk.EnableWindow(true);
				return;
			}
			//MessageBox(L"");
		}
		r = ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
		if (command.command != CHANGEFW) {
			LPTSTR  mess = new WCHAR[250];
			wsprintf(mess, L"У Вас проблема с фаерволом, включите его");
			MessageBox(mess);
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
			ButtonOk.EnableWindow(true);
			return;
		}
		//MessageBox(L"");
		//ath->GetStatus();


		WCHAR message[1024];
		wsprintf(message, L"ecl: %i, ecp: %i, u: %s, p: %s", ecl, ecp, u, p);
		if (!ath->connect(L"gate2.ath.ru", u, p)) {
			//ShowWindow(SW_MINIMIZE);
			if (!ath->connect(L"gate2.ath.ru", u, p)) {
				unsigned int i = GetLastError();
				command.command = RESTOREFW;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL);
				ButtonOk.EnableWindow(true);
				MessageBox(L"Проверьте интернет соединение, а так же логин и пароль.");
				return;
			}
		}
		if (!ath->ConnectRDP()) {
			unsigned int i = GetLastError();
			ath->disconnect();
			command.command = RESTOREFW;
			WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL);
			ButtonOk.EnableWindow(true);
			MessageBox(L"Нестабильное соединение, проверьте интернет и перезагрузите компьютер");
			return;
		}
		if (ath->GetStatus() != 1) {
			command.messsage = pid;
			command.command = RESTOREFW;
			DWORD dwByte = 0;
			WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL);
			ec_login.EnableWindow(true);
			ec_password.EnableWindow(true);
			ButtonOk.EnableWindow(true);
			return;
		}
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETSCREENSAVESECURE, TRUE, NULL, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 5 * 60, NULL, SPIF_UPDATEINIFILE);
		//delete u, p;
	}
	catch (int err) {
		MessageBox(L"Что-то пошло не так как планировалось, перезагрузите компьютер и попробуйте еще раз");
	}
}


void CathvpnuiDlg::OnDestroy()
{
	
	ath->deleteCred();
	if (hPipe == NULL && ath->isConnected())
		hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	VPNCOMMAND command;
	DWORD pid = GetCurrentProcessId();
	command.messsage = pid;
	command.command = RESTOREFW;
	DWORD dwByte = 0;
	WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL);
	//ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);
	delete ath;
	CDialogEx::OnDestroy();
}


void CathvpnuiDlg::OnBnClickedCancel()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialogEx::OnCancel();
}
