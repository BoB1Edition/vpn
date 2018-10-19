#include "ATHService.h"

std::vector<ATHService*> ATHService::Services;

ATHService::ATHService()
{
	Services.push_back(this);
	init();
	serviceName = new WCHAR[256];
	hCurr = GetCurrentProcess();
	GetModuleBaseName(hCurr, NULL, serviceName, 250);
	serviceName[lstrlenW(serviceName) - 4] = L'\0';
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
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
	
	if (!hSCManager) {
		return 2;
	}
	
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
	SC_HANDLE hService = OpenService(hSCManager, curServiceName, SERVICE_START);

	if (!StartService(hService, 0, NULL)) {
		CloseServiceHandle(hSCManager);
		return 4;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return 0;
}

void ATHService::init()
{
	SERVICE_TABLE_ENTRY ServiceTable[1];
	ServiceTable[0].lpServiceName = serviceName;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

	if (!StartServiceCtrlDispatcher(ServiceTable)) {
		
		printf("pizdecema: %i\n", GetLastError());
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

VOID ServiceMain(DWORD dwArgc, LPTSTR * lpszArgv)
{
	
}
