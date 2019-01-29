#include "stdafx.h"
#include "LogCStatic.h"


template <typename T>
void LogCStatic::SetWindowTextW(T str)
{
	LogWrite(str);
	CStatic::SetWindowTextW(str);
}

void LogCStatic::LogWrite(LPCTSTR str)
{
	DWORD wbByte(0);
	WriteFile(hLog, str, wcslen(str), &wbByte, NULL);
	FlushFileBuffers(hLog);
}

LogCStatic::LogCStatic():CStatic()
{
	hLog = CreateFile(L"C:\\Program Files (x86)\\ATH\\ATHlog.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLog == INVALID_HANDLE_VALUE) {
		int err = GetLastError();
		hLog = CreateFile(L"C:\\Program Files (x86)\\ATH\\ATHlog.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		err = GetLastError();
		wprintf(L"err %i\n", err);
	}
}


LogCStatic::~LogCStatic()
{
	CloseHandle(hLog);
}
