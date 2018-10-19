#pragma once

#include <Windows.h>
#include <vector>
#include <psapi.h>

class ATHService;
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);

class ATHService
{
public:
	ATHService();
	~ATHService();
	int run();
	int install();
	int stop();
	int pause();
private:
	static std::vector<ATHService*> Services;
	SC_HANDLE hService;
	LPWSTR serviceName;
	HANDLE hCurr;
	SC_HANDLE hSCManager;

	int InstallAll(int Index);
	int RunAll(int Index);
	void init();

};



