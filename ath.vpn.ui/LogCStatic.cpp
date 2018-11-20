#include "stdafx.h"
#include "LogCStatic.h"


void LogCStatic::SetWindowTextW(LPCTSTR str)
{
	LogWrite(str);
	CStatic::SetWindowTextW(str);
}

void LogCStatic::LogWrite(LPCTSTR str)
{
	DWORD wbByte(0);
	WriteFile(hLog, str, wcslen(str), &wbByte, NULL);
}

LogCStatic::LogCStatic():CStatic()
{
	hLog = CreateFile(L"C:\\Program Files (x86)\\ATH\\ATHlog.txt", GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLog == INVALID_HANDLE_VALUE) {
		int err = GetLastError();
		hLog = CreateFile(L"C:\\Program Files (x86)\\ATH\\ATHlog.txt", GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		err = GetLastError();
		wprintf(L"err %i\n", err);
	}
	//CStatic();
}


LogCStatic::~LogCStatic()
{
	CloseHandle(hLog);
}
