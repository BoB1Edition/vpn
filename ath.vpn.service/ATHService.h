#pragma once

#include <Windows.h>
#include <vector>
#include <psapi.h>

class ATHService;
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
void ControlHandler(DWORD request);


class ATHService
{
public:
	ATHService();
	~ATHService();
	int run();
	int install();
	int stop();
	int pause();

	void main(DWORD dwArgc, LPTSTR* lpszArgv);

private:
	static std::vector<ATHService*> Services;
	SC_HANDLE hService;
	LPWSTR serviceName;
	HANDLE hCurr;
	SC_HANDLE hSCManager;
	int err;

	SERVICE_STATUS serviceStatus;
	SERVICE_STATUS_HANDLE hStatus;
	SERVICE_STATUS_HANDLE serviceStatusHandle;

	int InstallAll(int Index);
	int RunAll(int Index);
	void init(LPWSTR curServiceName);
	int MainAll(DWORD dwArgc, LPTSTR* lpszArgv, int Index);

};



