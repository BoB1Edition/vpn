#pragma once
#include <afxwin.h>
class LogCStatic :
	public CStatic
{
public:
	void SetWindowTextW(LPCTSTR str);
	void LogWrite(LPCTSTR str);
	LogCStatic();
	~LogCStatic();
private:
	HANDLE hLog;
};

