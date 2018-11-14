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

LogCStatic::LogCStatic()
{
	hLog = CreateFile(L"ATHlog.txt", GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLog = INVALID_HANDLE_VALUE)
		hLog = CreateFile(L"ATHlog.txt", GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

}


LogCStatic::~LogCStatic()
{
	CloseHandle(hLog);
}
