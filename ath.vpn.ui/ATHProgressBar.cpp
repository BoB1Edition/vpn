#include "stdafx.h"
#include "ATHProgressBar.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(ATHProgressBar, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void ATHProgressBar::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, cProcBar);
}

BOOL ATHProgressBar::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

int ATHProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetTimer(503, 100, NULL);
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


void ATHProgressBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

}

void ATHProgressBar::OnPaint()
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
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void ATHProgressBar::OnTimer(UINT_PTR nIdEvent)
{
	if (nIdEvent == 503) {
		int position = cProcBar.GetPos()+1;
		int lower, upper;
		cProcBar.GetRange(lower, upper);
		if (position >= upper)
		{
			cProcBar.SetRange(lower, upper + 1);
			cProcBar.SetPos(upper + 1);
			cProcBar.SetPos(upper);
			cProcBar.SetRange(lower, upper);
		}
		else
		{
			cProcBar.SetPos(position + 1);
			cProcBar.SetPos(position);
		}
	}
}