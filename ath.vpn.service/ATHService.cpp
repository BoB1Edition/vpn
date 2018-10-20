#include "ATHService.h"

std::vector<ATHService*> ATHService::Services;

ATHService::ATHService()
{
	Services.push_back(this);
	//init();
	serviceName = new WCHAR[256];
	hCurr = GetCurrentProcess();
	GetModuleBaseName(hCurr, NULL, serviceName, 250);
	serviceName[lstrlenW(serviceName) - 4] = L'\0';
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager) {
		err = GetLastError();
	}
}


ATHService::~ATHService()
{
	CloseServiceHandle(hService);
}

int ATHService::run()
{
	for (int i = 0; i < Services.size(); i++) {
		int res = Services[i]->RunAll(i);
		if (res != 0) return res;
	}
	return 0;
}

int ATHService::install()
{
	for (int i = 0; i < Services.size(); i++) {
		int res = Services[i]->InstallAll(i);
		if( res != 0) return res;
	}
	return 0;
}

int ATHService::InstallAll(int Index)
{
	LPWSTR currName = new WCHAR[MAX_PATH];
	
	GetModuleFileNameEx(hCurr, NULL, currName, MAX_PATH);
	int lenpath = lstrlenW(currName);
	if (lenpath >= MAX_PATH - 1 - lstrlenW(L"\0")) {
		SetLastError(ERROR_BAD_PATHNAME);
		return 1;
	}

	//LPWSTR fullpath = lstrcpynW(lstrcpynW(currdir, L"\0", lenpath + 1), currName, lenpath + 1);
	
	
	LPWSTR servicePath = new WCHAR[MAX_PATH];
	wsprintf(servicePath, L"\"%s\" start\0", currName);
	wprintf(L"servicePath: %s\n", servicePath);
	LPWSTR curServiceName = new WCHAR[256];
	wsprintf(curServiceName, L"%s%i\0", serviceName, Index);
	hService = CreateService(hSCManager, curServiceName, curServiceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_CRITICAL, servicePath, NULL, NULL, NULL, NULL, NULL);

	if (!hService) {
		int err = GetLastError();

		CloseHandle(hCurr);
		CloseServiceHandle(hSCManager);
		return err;
	}

	CloseServiceHandle(hSCManager);
	CloseHandle(hCurr);
	return 0;
}

int ATHService::RunAll(int Index)
{
	LPWSTR curServiceName = new WCHAR[256];
	wsprintf(curServiceName, L"%s%i\0", serviceName, Index);
	init(curServiceName);
	SC_HANDLE hService = OpenService(hSCManager, curServiceName, SERVICE_START);

	if (!StartService(hService, 0, NULL)) {
		CloseServiceHandle(hSCManager);
		return 4;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return 0;
}

void ATHService::init(LPWSTR curServiceName)
{
	SERVICE_TABLE_ENTRY ServiceTable[1];
	ServiceTable[0].lpServiceName = curServiceName;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

	if (!StartServiceCtrlDispatcher(ServiceTable)) {
		
		printf("pizdecema: %i\n", GetLastError());
	}
}

int ATHService::MainAll(DWORD dwArgc, LPTSTR * lpszArgv, int Index)
{
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_CONTROL_PAUSE | SERVICE_CONTROL_CONTINUE;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
	if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
		return;
	}

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	while (serviceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		SetServiceStatus
	}
}

int ATHService::stop()
{
	return 0;
}

int ATHService::pause()
{
	return 0;
}

void ATHService::main(DWORD dwArgc, LPTSTR * lpszArgv)
{
	for (int i = 0; i < Services.size(); i++) {
		int res = Services[i]->MainAll(dwArgc, lpszArgv, i);
		if (res != 0) return;
	}
	return;
}

VOID ServiceMain(DWORD dwArgc, LPTSTR * lpszArgv)
{
	
	ATHService service;
	
	service.main(dwArgc, lpszArgv);
}

void ControlHandler(DWORD request)
{
}
