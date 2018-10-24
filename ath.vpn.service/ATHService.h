#pragma once

#include <Windows.h>
#include <vector>
#include <psapi.h>
#include "EventMessage.h"
#include "wscapi.h"

enum tagVPNCOMMAND{
	CHECKAV,
	CHECKFW,
	CHECKUP,
	CHANGEFW,
	RESTOREFW,
	FWOK
};

struct VPNCOMMAND{
	int messsage;
	tagVPNCOMMAND command;
};

class ATHService;
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
void ControlHandler(DWORD request);
DWORD WINAPI ThreadAVStart(LPVOID lpParameter);
DWORD WINAPI ThreadPipeStart(LPVOID lpParameter);


class ATHService
{
public:
	ATHService();
	~ATHService();
	int run();
	int install();
	int stop();
	int pause();

	int main(DWORD dwArgc, LPTSTR* lpszArgv);
	int init();

	EventMessage ev;

private:
//	EventMessage ev;
	static std::vector<ATHService*> Services;
	SC_HANDLE hService;
	LPWSTR serviceName;
	HANDLE hCurr;
	SC_HANDLE hSCManager;
	int err;
	HANDLE hThAVScan, hThPipe;
	HANDLE hPipe;


	SERVICE_STATUS serviceStatus;
	SERVICE_STATUS_HANDLE hStatus;
	SERVICE_STATUS_HANDLE serviceStatusHandle;
	bool paused;


	int InstallAll(int Index);
	int RunAll(int Index);
	void init(LPWSTR curServiceName);
	int MainAll(DWORD dwArgc, LPTSTR* lpszArgv, int Index);
	int ATHService::StopAll(int Index);

	int LastScan;

	friend DWORD WINAPI ThreadAVStart(LPVOID lpParameter);
	friend DWORD WINAPI ThreadPipeStart(LPVOID lpParameter);

	int ThreadAVScan();
	int ThreadPipe();

	int CheckFirewall();

};



