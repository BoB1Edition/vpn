
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
}

BEGIN_MESSAGE_MAP(CathvpnuiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CathvpnuiDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// обработчики сообщений CathvpnuiDlg

BOOL CathvpnuiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	ShowWindow(SW_MINIMIZE);

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

	return 0;
}


void CathvpnuiDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

}


void CathvpnuiDlg::OnBnClickedOk()
{

	ath = new ATHClientIfc(&s_status);
	hPipe = CreateFile(L"\\\\.\\pipe\\ath.vpn", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
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
	command.messsage = GetCurrentProcessId();
	command.command = tagVPNCOMMAND::CHECKAV;
	DWORD dwByte;
	if (!WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &dwByte, NULL)) {
		int err = GetLastError();
		LPTSTR  mess = new WCHAR[50];
		wsprintf(mess, L"err: %i", err);
		MessageBox(mess);
	}
	ReadFile(hPipe, (LPVOID)&command, sizeof(command), &dwByte, NULL);

	//CDialogEx::OnOK();
}
