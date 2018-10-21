#include "ATHService.h"

std::vector<ATHService*> ATHService::Services;

ATHService *service;

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
		LPWSTR errMessage = new WCHAR[32000];
		wsprintf(errMessage, L"ATHService: %i\n", err);
		ev.addLog(errMessage);
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
	wsprintf(servicePath, L"\"%s\" run\0", currName);
	wprintf(L"servicePath: %s\n", servicePath);
	LPWSTR curServiceName = new WCHAR[256];
	wsprintf(curServiceName, L"%s%i\0", serviceName, Index);
	hService = CreateService(hSCManager, curServiceName, curServiceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_CRITICAL, servicePath, NULL, NULL, NULL, NULL, NULL);

	if (!hService) {
		int err = GetLastError();
		LPWSTR errMessage = new WCHAR[32000];
		wsprintf(errMessage, L"hService: %i\n", err);
		ev.addLog(errMessage);
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
	//init(curServiceName);
	SC_HANDLE hService = OpenService(hSCManager, curServiceName, SERVICE_START);

	if (!StartService(hService, 0, NULL)) {
		err = GetLastError();
		LPWSTR errMessage = new WCHAR[32000];
		wsprintf(errMessage, L"RunAll: %i\n", err);
		ev.addLog(errMessage);
		CloseServiceHandle(hSCManager);
		return err;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return 0;
}

void ATHService::init(LPWSTR curServiceName)
{
	SERVICE_TABLE_ENTRY ServiceTable;
	ServiceTable.lpServiceName = curServiceName;
	ServiceTable.lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	Sleep(1000 * 10);
	if (!StartServiceCtrlDispatcher(&ServiceTable)) {
		err = GetLastError();
		/*if (err = 1056) {
			LPWSTR errMessage = new WCHAR[32000];
			wsprintf(errMessage, L"init Success: %s\n", curServiceName);
			delete errMessage;
			return;
		}*/
		LPWSTR errMessage = new WCHAR[32000];
		wsprintf(errMessage, L"curServiceName: %s init: %i\n", curServiceName, err);
		ev.addLog(errMessage);
		delete errMessage;
		return;
	}
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"init Success: %s\n", curServiceName);
	delete errMessage;
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
	LPWSTR curServiceName = new WCHAR[256];
	wsprintf(curServiceName, L"%s%i\0", serviceName, Index);
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"MainAll: %s\n", curServiceName);
	ev.addLog(errMessage);
	serviceStatusHandle = RegisterServiceCtrlHandler(curServiceName, (LPHANDLER_FUNCTION)ControlHandler);
	if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
		err = GetLastError();
		//errMessage = new WCHAR[32000];
		wsprintf(errMessage, L"serviceStatusHandle MainAll: %i\n", err);
		ev.addLog(errMessage);
	}

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	if (serviceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		DWORD dwAvScan, dwPipe;
	
		hThAVScan = CreateThread(NULL, 0, ThreadAVStart, this, 0, &dwAvScan);
		if (hThAVScan == NULL) {
			err = GetLastError();
			//errMessage = new WCHAR[32000];
			wsprintf(errMessage, L"hThAVScan: %i\n", err);
			ev.addLog(errMessage);
			return err;
		}
		wsprintf(errMessage, L"hThAVScan: %s start ok\n", curServiceName);
		ev.addLog(errMessage);
		hThPipe = CreateThread(NULL, 0, ThreadPipeStart, this, 0, &dwPipe);
		if (hThPipe == NULL) {
			err = GetLastError();
			//LPWSTR errMessage = new WCHAR[32000];
			wsprintf(errMessage, L"hThPipe: %i\n", err);
			ev.addLog(errMessage);
			return err;
		}
		wsprintf(errMessage, L"hThPipe: %s start ok\n", curServiceName);
		ev.addLog(errMessage);
		
	}
	paused = false;
	return 0;
}

int ATHService::ThreadAVScan()
{
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadAVScan: Start");
	ev.addLog(errMessage);
	Sleep(9 * 1000);
	errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadAVScan: Stop");
	ev.addLog(errMessage);
	return 0;
}

int ATHService::ThreadPipe()
{
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadPipe: Start");
	ev.addLog(errMessage);
	Sleep(1000 * 5);
	errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadPipe: Stop");
	ev.addLog(errMessage);
	return 0;
}

int ATHService::stop() {
	for (int i = 0; i < Services.size(); i++) {
		int res = Services[i]->StopAll(i);
		if (res != 0) return res;
	}
	//ExitProcess(0);
	return 0;
}

int ATHService::StopAll(int Index)
{
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"StopAll: Start%i\0", Index);
	ev.addLog(errMessage);
	if (hThAVScan != NULL) {
		TerminateThread(hThAVScan, err);
	}
	if (hThPipe != NULL) {
		TerminateThread(hThPipe, err);
	}
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
	Sleep(10 * 1000);
	wsprintf(errMessage, L"StopAll%i: err: %i\0", Index, GetLastError());
	ev.addLog(errMessage);
	return 0;
}

int ATHService::pause()
{
	if (paused) {
		ResumeThread(hThAVScan);
		ResumeThread(hThPipe);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
	}
	else {
		SuspendThread(hThAVScan);
		SuspendThread(hThPipe);
		serviceStatus.dwCurrentState = SERVICE_PAUSED;
	}
	
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
	paused = !paused;
	return 0;
}

int ATHService::main(DWORD dwArgc, LPTSTR * lpszArgv)
{
	for (int i = 0; i < Services.size(); i++) {
		int res = Services[i]->MainAll(dwArgc, lpszArgv, i);
		if (res != 0) return res;
	}
	return 0;
}

int ATHService::init()
{
	err = 0;
	for (int i = 0; i < Services.size(); i++) {
		LPWSTR curServiceName = new WCHAR[256];
		wsprintf(curServiceName, L"%s%i\0", serviceName, i);
		Services[i]->init(curServiceName);
		if (err != 0) return err;
	}
	return 0;
}

VOID ServiceMain(DWORD dwArgc, LPTSTR * lpszArgv)
{
	LPWSTR message = new WCHAR[32000];
	//GetCurrentProcessId();
	
	//Sleep(1000 * 20);
	service = new ATHService;
	wsprintf(message, L"ServiceMain: pid %i\0", GetCurrentProcessId());
	printf("start ATHServiceMain");
	service->ev.addLog(message);
	service->main(dwArgc, lpszArgv);
	
}

void ControlHandler(DWORD request)
{
	LPWSTR errMessage = new WCHAR[32000];
	switch (request) {
	case SERVICE_RUNNING:
		service->main(0, NULL);
		break;
	case SERVICE_CONTROL_STOP:
		wsprintf(errMessage, L"SERVICE_CONTROL_STOP: Start\0");
		service->ev.addLog(errMessage);
		service->stop();
		wsprintf(errMessage, L"SERVICE_CONTROL_STOP: Stop\0");
		service->ev.addLog(errMessage);
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		wsprintf(errMessage, L"SERVICE_CONTROL_SHUTDOWN: Start\0");
		service->ev.addLog(errMessage);
		service->stop();
		wsprintf(errMessage, L"SERVICE_CONTROL_SHUTDOWN: Stop\0");
		service->ev.addLog(errMessage);
		break;
	case SERVICE_CONTROL_PAUSE:
	case SERVICE_CONTROL_CONTINUE:
		service->pause();
		break;
	default:
		wsprintf(errMessage, L"ControlHandler: request %i\0", request);
		service->ev.addLog(errMessage);
		break;
	}
}

DWORD __stdcall ThreadAVStart(LPVOID lpParameter)
{
	ATHService* lp = (ATHService*)lpParameter; 
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadAVStart: Start");
	lp->ev.addLog(errMessage);
	lp->ThreadAVScan();
	wsprintf(errMessage, L"ThreadAVStart: Stop");
	lp->ev.addLog(errMessage);
	return 0;
}

DWORD __stdcall ThreadPipeStart(LPVOID lpParameter)
{
	ATHService* lp = (ATHService*)lpParameter;
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadPipeStart: Start");
	lp->ev.addLog(errMessage);
	lp->ThreadPipe();
	Sleep(1000 + 5);
	wsprintf(errMessage, L"ThreadPipeStart: Stop");
	lp->ev.addLog(errMessage);
	return 0;
}
