#pragma once

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "wbemuuid")
#pragma comment(lib, "urlmon.lib")

#include <Windows.h>
#include <vector>
#include <psapi.h>
#include "EventMessage.h"
#include "wscapi.h"
#include "VPNCOMMAND.h"
#include "ATHFWSetup.h"
#include <aclapi.h>
#include <lmaccess.h>
#include <time.h>
#include <Winnt.h>
#include "ntsecapi.h"
#include <wbemidl.h>
#include "Urlmon.h"

class ATHService;
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
void ControlHandler(DWORD request);
DWORD WINAPI ThreadAVStart(LPVOID lpParameter);
DWORD WINAPI ThreadPipeStart(LPVOID lpParameter);
static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp);

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
	int CheckAntivirus();
	int CheckUpdate();

	BOOL BuildSecurityAttributes(SECURITY_ATTRIBUTES* psa);
	BOOL GetUserSid(PSID*  ppSidUser);

	LPWSTR GeneratePassword();
	bool Update();
	const std::wstring chars = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789%^&$+-_!@#%?";

public:
	//void SendJson();
	bool ATHService::InitLsaString(PLSA_UNICODE_STRING pLsaString, LPCWSTR pwszString);
	LONG GetDWORDRegKey(DWORD *nValue);

};



