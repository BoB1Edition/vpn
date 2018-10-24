#include "EventMessage.h"



EventMessage::EventMessage()
{
	LPWSTR serviceName = new WCHAR[256];
	HANDLE hCurr = GetCurrentProcess();
	GetModuleBaseName(hCurr, NULL, serviceName, 250);
	serviceName[lstrlenW(serviceName) - 4] = L'\0';
	hEventLog = RegisterEventSource(NULL, serviceName);
}



EventMessage::~EventMessage()
{
	DeregisterEventSource(hEventLog);
}

void EventMessage::addLog(LPWSTR message)
{
	LPCWSTR mess = LPCWSTR(message);
	ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, MSG_INFO_1, 0, 1, 0, &mess, 0);
}
