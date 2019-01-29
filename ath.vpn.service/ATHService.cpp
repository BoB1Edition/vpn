#include "ATHService.h"
//#include <LM.h>

#define URL "https://srvlk.ath.ru/vpn/"
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
	//for (int i = 0; i < Services.size(); i++) {
		int res = Services[0]->RunAll(0);
		if (res != 0) return res;
	//}
	return 0;
}

int ATHService::install()
{
	USER_INFO_1 ATHAdmin, ATHUser;

	DWORD dwLevel = 1;
	DWORD dwError = 0;
	memset(&ATHAdmin, 0, sizeof(ATHAdmin));
	memset(&ATHUser, 0, sizeof(ATHUser));
	ATHAdmin.usri1_name = L"ATHAdmin";
	ATHAdmin.usri1_password = L"%6Nop!2dgPPv";
	ATHAdmin.usri1_priv = USER_PRIV_USER;
	ATHAdmin.usri1_home_dir = NULL;
	ATHAdmin.usri1_comment = NULL;
	ATHAdmin.usri1_flags = UF_SCRIPT | UF_NORMAL_ACCOUNT | UF_DONT_EXPIRE_PASSWD | UF_TRUSTED_FOR_DELEGATION;
	ATHAdmin.usri1_script_path = NULL;

	NET_API_STATUS nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&ATHAdmin, NULL);

	LOCALGROUP_MEMBERS_INFO_3 account;
	memset(&account, 0, sizeof(account));

	account.lgrmi3_domainandname = L"ATHAdmin";
	NetLocalGroupAddMembers(NULL, L"Administrators", 3, (LPBYTE)&account, 1);
	NetLocalGroupAddMembers(NULL, L"Администраторы", 3, (LPBYTE)&account, 1);


	ATHUser.usri1_name = L"ATHUser";
	ATHUser.usri1_password = L"Ath@2018";
	ATHUser.usri1_priv = USER_PRIV_USER;
	ATHUser.usri1_home_dir = NULL;
	ATHUser.usri1_comment = NULL;
	ATHUser.usri1_flags = UF_SCRIPT | UF_NORMAL_ACCOUNT | UF_PASSWORD_EXPIRED;
	ATHUser.usri1_script_path = NULL;

	nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&ATHUser, NULL);

	memset(&account, 0, sizeof(account));

	account.lgrmi3_domainandname = L"ATHUser";
	NetLocalGroupAddMembers(NULL, L"Users", 3, (LPBYTE)&account, 1);
	NetLocalGroupAddMembers(NULL, L"Пользователи", 3, (LPBYTE)&account, 1);

	Json::Value root;
	root["admin"] = "%6Nop!2dgPPv";
	DWORD nResult(0);
	GetDWORDRegKey(&nResult);
	root["Teamviewer"] = (unsigned int)nResult;

	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));
	LSA_HANDLE lsaH;
	LSA_UNICODE_STRING* userRights = new LSA_UNICODE_STRING[1];
	LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES, &lsaH);
	PSID Sid = (PSID)LocalAlloc(LPTR, SECURITY_MAX_SID_SIZE);
	DWORD cbSid = SECURITY_MAX_SID_SIZE;
	LPWSTR DomainName = (LPWSTR)LocalAlloc(LPTR, sizeof(WCHAR) * 2048);
	DWORD size = 1024;
	SID_NAME_USE peUse;
	LookupAccountName(NULL, L"ATHAdmin", Sid, &cbSid, DomainName, &size, &peUse);
	InitLsaString(userRights, L"SeServiceLogonRight");
	LsaAddAccountRights(lsaH, Sid, userRights, 1);
	Json::StyledStreamWriter writer;
	std::ofstream fwsetting(L"AthFile.err");
	writer.write(fwsetting, root);
	fwsetting.flush();
	fwsetting.close();

	//for (int i = 0; i < Services.size(); i++) {
		int res = Services[0]->InstallAll(0);
		if (res != 0) return res;
	//}
	return 0;
}


bool ATHService::InitLsaString(PLSA_UNICODE_STRING pLsaString,LPCWSTR pwszString)
{
	DWORD dwLen = 0;

	if (NULL == pLsaString)
		return FALSE;

	if (NULL != pwszString)
	{
		dwLen = wcslen(pwszString);
		if (dwLen > 0x7ffe)   // String is too large
			return FALSE;
	}

	// Store the string.
	pLsaString->Buffer = (WCHAR *)pwszString;
	pLsaString->Length = (USHORT)dwLen * sizeof(WCHAR);
	pLsaString->MaximumLength = (USHORT)(dwLen + 1) * sizeof(WCHAR);

	return TRUE;
}


static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp) {
	DWORD readBytes(0);
	ReadFile(userp, dest, size*nmemb, &readBytes, NULL);
	return readBytes;
}

/*void ATHService::SendJson()
{
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();

	HANDLE fd = CreateFile(L"c:\\temp\\AthFile.err", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	curl_easy_setopt(curl, CURLOPT_URL, L"http://vpnapi.ath.ru:9898/");
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READDATA, fd);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
}*/

LONG ATHService::GetDWORDRegKey(DWORD *nValue)
{
	HKEY hKey;
	LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TeamViewer", 0, KEY_READ, &hKey);
	//nValue = nDefaultValue;
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	DWORD Type(REG_DWORD);
	LONG nError = RegQueryValueEx(hKey, L"ClientID", 0, &Type, (LPBYTE)&nResult, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		*nValue = nResult;
	}
	else {
		*nValue = 0;
	}
	return nError;
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
		SERVICE_AUTO_START, SERVICE_ERROR_CRITICAL, servicePath, NULL, NULL, NULL, L".\\ATHAdmin\0", L"%6Nop!2dgPPv");

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
	if (!StartServiceCtrlDispatcher(&ServiceTable)) {
		err = GetLastError();
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
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, L"global_KAV_SCAN");
	LARGE_INTEGER liDueTime;
	while (true) {
		liDueTime.QuadPart = -100000000LL * 6;
		if(LastScan % 60 == 0)
		try {
			Update();
		}
		catch (int a) {
			ev.addLog(L"Жопа в огне");
		}
		SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			ev.addLog(L"WaitForSingleObject failed");
		if (LastScan == 0) {
			ev.addLog(L"Start avira scan");
			SECURITY_ATTRIBUTES sa;
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			sa.nLength = sizeof(sa);
			STARTUPINFO sp = {};
			PROCESS_INFORMATION pi = {};
			wchar_t cmdUpd[] = L"\"c:\\Program Files (x86)\\Avira\\Antivirus\\update.exe\"\0";
			BOOL bCP = CreateProcess(NULL,
				cmdUpd,
				&sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sp, &pi);
			if (bCP == 0) {
				int err = GetLastError();
				return err;
			}
			if (WaitForSingleObject(pi.hProcess, INFINITE) != 0) {
				return GetLastError();
			}

			wchar_t cmdUpdate[] = L"\"c:\\Program Files (x86)\\Avira\\Antivirus\\avscan.exe\" /CFG=\"c:\\Program Files (x86)\\Avira\\Antivirus\\alldiscs.avp\"\0";
			bCP = CreateProcess(NULL,
				cmdUpdate,
				&sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sp, &pi);
			if (bCP == 0) {
				int err = GetLastError();
				return err;
			}
			if (WaitForSingleObject(pi.hProcess, INFINITE) != 0) {
				return GetLastError();
			}
			if (bCP == 0) {
				int err = GetLastError();
				return err;
			}
			if (WaitForSingleObject(pi.hProcess, INFINITE) != 0) {
				return GetLastError();
			}
			ev.addLog(L"END avira scan");
			LastScan = 1440;
		}
		LastScan -= 1;
	}

	wsprintf(errMessage, L"ThreadAVScan: Stop");
	ev.addLog(errMessage);
	return 0;
}

int ATHService::ThreadPipe()
{
	ATHFWSetup ath;
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"ThreadPipe: Start");
	ev.addLog(errMessage);

	SECURITY_ATTRIBUTES sa = { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };

	InitializeSecurityDescriptor(
		&sd,
		SECURITY_DESCRIPTOR_REVISION);

	SetSecurityDescriptorDacl(
		&sd,
		TRUE,
		NULL,
		FALSE);

	sa.bInheritHandle = false;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(sa);
	hPipe = CreateNamedPipe(L"\\\\.\\pipe\\ath.vpn", PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
		sizeof(VPNCOMMAND), sizeof(VPNCOMMAND), 5000, &sa);

	if (hPipe == INVALID_HANDLE_VALUE) {
		err = GetLastError();
		wsprintf(errMessage, L"hPipe: err %i", err);
		ev.addLog(errMessage);
		return err;
	}
	else {
		wsprintf(errMessage, L"hPipe: Created");
		ev.addLog(errMessage);
	}
	if (!ConnectNamedPipe(hPipe, NULL)) {
		err = GetLastError();
		wsprintf(errMessage, L"ConnectNamedPipe: err %i", err);
		ev.addLog(errMessage);
		return err;
	}
	while (true) {
		DWORD  cbRead;
		VPNCOMMAND command = {};
		if (!ReadFile(hPipe, (LPVOID)&command, sizeof(VPNCOMMAND), &cbRead, NULL)) {
			err = GetLastError();
			wsprintf(errMessage, L"Command %i, %i, err: %i", command.command, command.messsage, err);
			ev.addLog(errMessage);
			DisconnectNamedPipe(hPipe);
			if (!ConnectNamedPipe(hPipe, NULL)) {
				err = GetLastError();
				wsprintf(errMessage, L"ConnectNamedPipe: err %i", err);
				ev.addLog(errMessage);
				return err;
			}
			continue;
		}
		wsprintf(errMessage, L"Command %i ,%i", command.command, command.messsage);
		ev.addLog(errMessage);
		switch (command.command) {
		case CHECKFW:
			if (CheckFirewall() == 0) {
				wsprintf(errMessage, L"Firewall ok");
				ev.addLog(errMessage);
				command.command = FWOK;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			else {
				wsprintf(errMessage, L"Firewall fail");
				ev.addLog(errMessage);
				command.command = FWFAIL;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			break;
		case CHECKAV:
			if (CheckAntivirus() == 5) {
				wsprintf(errMessage, L"Antivirus ok");
				ev.addLog(errMessage);
				command.command = AVOK;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			else {
				wsprintf(errMessage, L"Antivirus fail");
				ev.addLog(errMessage);
				command.command = AVFAIL;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			break;
		case CHECKUP:
			if (CheckUpdate() == 0) {
				wsprintf(errMessage, L"Update ok");
				ev.addLog(errMessage);
				command.command = UPOK;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			else {
				wsprintf(errMessage, L"Update fail");
				ev.addLog(errMessage);
				command.command = UPFAIL;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			}
			break;
		case CHANGEFW:
			if (ath.SaveRulesToFile(L"C:\\Program Files (x86)\\ATH\\defaultFWconfig.config") < 0) {
				wsprintf(errMessage, L"CHANGEFW fail");
				ev.addLog(errMessage);
				command.command = FWFAIL;
				WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
				return -1;
			}
			ath.DeleteAllRules();
			ath.LoadRulesFromFile(L"C:\\Program Files (x86)\\ATH\\ATHfwconfig.config");
			WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
			break;
		case RESTOREFW:
			ath.DeleteAllRules();
			ath.LoadRulesFromFile(L"C:\\Program Files (x86)\\ATH\\defaultFWconfig.config");
			DeleteFile(L"C:\\Program Files (x86)\\ATH\\defaultFWconfig.config");
			break;
		default:
			wsprintf(errMessage, L"command.command: %i", command.command);
			ev.addLog(errMessage);
			WriteFile(hPipe, (LPCVOID)&command, sizeof(command), &cbRead, NULL);
		}
	}
	wsprintf(errMessage, L"ThreadPipe: Stop");
	ev.addLog(errMessage);
	return 0;
}

int ATHService::CheckFirewall()
{

	WSC_SECURITY_PROVIDER_HEALTH secHealth;
	HRESULT result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_SERVICE, &secHealth);
	if (result != S_OK) {
		return 255;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}
	result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_FIREWALL, &secHealth);
	if (result != S_OK) {
		return 255;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}
	return 0;
}

int ATHService::CheckAntivirus()
{
	WSC_SECURITY_PROVIDER_HEALTH secHealth;
	HRESULT result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_SERVICE, &secHealth);
	if (result != S_OK) {
		return 200;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}
	result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &secHealth);
	if (result != S_OK) {
		return 200;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}

	CoInitializeEx(0, 0);
	CoInitializeSecurity(0, -1, 0, 0, 0, 3, 0, 0, 0);
	IWbemLocator *locator = 0;
	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void **)&locator);
	IWbemServices * services = 0;
	wchar_t *name = L"root\\SecurityCenter2";
	bool noWindowsDefender = false;
	if (SUCCEEDED(locator->ConnectServer(name, 0, 0, 0, 0, 0, 0, &services))) {
		CoSetProxyBlanket(services, 10, 0, 0, 3, 3, 0, 0);
		wchar_t *query = L"Select * From AntiVirusProduct";
		IEnumWbemClassObject *e = 0;
		
		if (SUCCEEDED(services->ExecQuery(L"WQL", query, WBEM_FLAG_FORWARD_ONLY, 0, &e))) {
			IWbemClassObject *object = 0;
			ULONG u = 0;
			std::string antiVirus;

			while (e) {
				e->Next(WBEM_INFINITE, 1, &object, &u);
				if (!u) break;
				VARIANT cvtVersion;
				object->Get(L"displayName", 0, &cvtVersion, 0, 0);
				LPWSTR errMessage = new WCHAR[500];
				wsprintf(errMessage, L"Antivirus: %s enabled", cvtVersion.bstrVal);
				if (lstrcmpiW(L"Windows Defender", errMessage) != 0) noWindowsDefender = true;
				ev.addLog(errMessage);
			}
		}
	}
	services->Release();
	locator->Release();
	CoUninitialize();

	if(noWindowsDefender) return 5;
	return 4;
}

int ATHService::CheckUpdate() {

	WSC_SECURITY_PROVIDER_HEALTH secHealth;
	HRESULT result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_SERVICE, &secHealth);
	if (result != S_OK) {
		return 255;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}
	result = WscGetSecurityProviderHealth(WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS, &secHealth);
	if (result != S_OK) {
		return 255;
	}
	if (secHealth != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
		return 1;
	}
	return 0;
}

BOOL ATHService::BuildSecurityAttributes(SECURITY_ATTRIBUTES * psa)
{
	LPWSTR errMessage = new WCHAR[3200];
	DWORD dwAclSize;
	PSID  pSidAnonymous = NULL; // Well-known AnonymousLogin SID
	PSID  pSidOwner = NULL;

	SID_IDENTIFIER_AUTHORITY siaAnonymous = SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY siaOwner = SECURITY_NT_AUTHORITY;

	do
	{
		PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (psd == NULL)
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
		}

		if (!InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION))
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
			break;
		}

		// Build anonymous SID
		AllocateAndInitializeSid(&siaAnonymous, 1,
			SECURITY_ANONYMOUS_LOGON_RID,
			0, 0, 0, 0, 0, 0, 0,
			&pSidAnonymous
		);

		if (!GetUserSid(&pSidOwner))
		{
			return FALSE;
		}

		// Compute size of ACL
		dwAclSize = sizeof(ACL) +
			2 * (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)) +
			GetLengthSid(pSidAnonymous) +
			GetLengthSid(pSidOwner);

		PACL pACL = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclSize);
		if (pACL == NULL)
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
			break;
		}

		InitializeAcl(pACL, dwAclSize, ACL_REVISION);


		if (!AddAccessAllowedAce(pACL,
			ACL_REVISION,
			GENERIC_ALL,
			pSidOwner
		))
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
			break;
		}


		if (!AddAccessAllowedAce(pACL,
			ACL_REVISION,
			FILE_GENERIC_READ, //GENERIC_READ | GENERIC_WRITE,
			pSidAnonymous
		))
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
			break;
		}

		if (!SetSecurityDescriptorDacl(psd, TRUE, pACL, FALSE))
		{
			err = GetLastError();
			wsprintf(errMessage, L"HeapAlloc: err %i", err);
			ev.addLog(errMessage);
			break;
		}

		psa->nLength = sizeof(SECURITY_ATTRIBUTES);
		psa->bInheritHandle = TRUE;
		psa->lpSecurityDescriptor = psd;

	} while (0);

	if (pSidAnonymous)   FreeSid(pSidAnonymous);
	if (pSidOwner)       FreeSid(pSidOwner);

	return TRUE;
}

BOOL ATHService::GetUserSid(PSID * ppSidUser)
{
	HANDLE      hToken;
	DWORD       dwLength = sizeof(TOKEN_USER);
	DWORD       cbName = 250;
	DWORD       cbDomainName = 250;
	PTOKEN_USER pTokenUser = NULL;

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	pTokenUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
	if (!GetTokenInformation(hToken,       // handle of the access token
		TokenUser,    // type of information to retrieve
		pTokenUser,   // address of retrieved information 
		0,            // size of the information buffer
		&dwLength     // address of required buffer size
	))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			pTokenUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
			if (pTokenUser == NULL)
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	if (!GetTokenInformation(hToken,     // handle of the access token
		TokenUser,  // type of information to retrieve
		pTokenUser, // address of retrieved information 
		dwLength,   // size of the information buffer
		&dwLength   // address of required buffer size
	))
	{
		HeapFree(GetProcessHeap(), 0, pTokenUser);
		pTokenUser = NULL;

		return FALSE;
	}

	*ppSidUser = pTokenUser->User.Sid;
	return TRUE;
}

LPWSTR ATHService::GeneratePassword()
{
	srand((unsigned)time(NULL));
	int r = rand() % 20;
	for (int i = 0; i < r; i++) {
		srand((unsigned)time(NULL));
		Sleep(rand() % 100);
	}
	srand((unsigned)time(NULL));
	int lPass = (double)rand() / (RAND_MAX + 1) * (15 - 10) + 10;
	std::wstring s = L"";
	for (int i = 0; i < lPass; i++) {
		s += chars[rand() % chars.length()];
	}


	LPWSTR PassWord = new WCHAR[lPass];
	PassWord = LPWSTR(s.c_str());
	return PassWord;
}

bool ATHService::Update()
{
	DeleteFile(L"C:\\WINDOWS\\TEMP\\vpn.service.json");
	URLDownloadToFile(0, L"https://srvlk.ath.ru/vpn/version.json", L"C:\\WINDOWS\\TEMP\\vpn.service.json", 0, 0);
	//Sleep(1000 * 10);
	std::ifstream fwsetting(L"C:\\WINDOWS\\TEMP\\vpn.service.json");
	if (!fwsetting.is_open())
		return false;
	Json::Value root;
	fwsetting >> root;
	if (strcmp(root["version"].asCString(), "1.0.0.1") != 0) {
		const char* urls = root["url"].asCString();
		
		int urlsize = strnlen_s(urls, 1024);
		size_t len = strnlen_s(URL, 1024) + urlsize+1;
		char *url = new char[len];
		
		strcpy_s(url, len, URL);
		strcat_s(url, len, urls);
		wchar_t* w = new wchar_t[2048];
		size_t outSize;
		mbstowcs_s(&outSize, w, len, url, len-1);
		ev.addLog(w);
		len = strlen("c:\\windows\\temp\\") + urlsize+1;
		char *local = new char[len];
		strcpy_s(local, len, "c:\\windows\\temp\\");
		strcat_s(local, len, urls);
		URLDownloadToFileA(0, url, local, 0, 0);
		mbstowcs_s(&outSize, w, len, local, len - 1);
		SECURITY_ATTRIBUTES sa;
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		sa.nLength = sizeof(sa);
		STARTUPINFO sp = {};
		PROCESS_INFORMATION pi = {};
		//wchar_t cmdUpd[] = L"\"c:\\Program Files (x86)\\Avira\\Antivirus\\update.exe\"\0";
		BOOL bCP = CreateProcess(NULL,
			w,
			&sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sp, &pi);
		if (bCP == 0) {
			int err = GetLastError();
			return err;
		}
		if (WaitForSingleObject(pi.hProcess, INFINITE) != 0) {
			return GetLastError();
		}
		//system(local);
	}

	return false;
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
	CloseHandle(hCurr);
	CloseHandle(hPipe);
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
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
	LPWSTR errMessage = new WCHAR[32000];
	wsprintf(errMessage, L"main run: Start\0");
	ev.addLog(errMessage);
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

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR * lpszArgv)
{
	LPWSTR message = new WCHAR[32000];
	//ATHService::
	service = new ATHService;
	wsprintf(message, L"ServiceMain: pid %i\0", GetCurrentProcessId());
	printf("start ATHServiceMain");
	service->ev.addLog(message);
	service->main(dwArgc, lpszArgv);
	wsprintf(message, L"ServiceMain: pid %i\0", GetCurrentProcessId());
	printf("stop ATHServiceMain");
}

void ControlHandler(DWORD request)
{
	LPWSTR errMessage = new WCHAR[32000];
	switch (request) {
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
	case SERVICE_CONTROL_INTERROGATE:
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
	wsprintf(errMessage, L"ThreadPipeStart: Stop");
	lp->ev.addLog(errMessage);
	return 0;
}
