#pragma once
#include <afxwin.h>
class LogCStatic :
	public CStatic
{
public:
	template <typename T>
	void SetWindowTextW(T str);
	void LogWrite(LPCTSTR str);
	LogCStatic();
	~LogCStatic();
private:
	HANDLE hLog;
};

