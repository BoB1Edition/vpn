#include "stdafx.h"
#include "ATHClientIfc.h"


ATHClientIfc::ATHClientIfc(CStatic *s_status)
{
	Attach = false;
	this->s_status = s_status;
}


ATHClientIfc::~ATHClientIfc()
{
	ClientIfc::disconnect();
	ClientIfc::detach();
}

int ATHClientIfc::GetStatus()
{
	if(!Attach)
		Attach = ClientIfc::attach(false, true, true, true);
	if (!Attach)
		return -1;
	if(ClientIfc::isConnected())
		return 1;
	if (ClientIfc::isAvailable())
		return 2;
	return 0;
}

bool ATHClientIfc::connect(std::wstring host, wchar_t * username, wchar_t * password)
{
	this->username = username;
	this->password = password;
	if (ClientIfc::isConnected())
		return true;
	//setBanner(L"Banner");
	setBannerResponse(true);
	
	if (ClientIfc::connect(host)) {
		return true;
	}
	return false;
}

bool ATHClientIfc::ConnectRDP()
{
	CREDENTIAL *cred = (CREDENTIAL *) calloc(2, sizeof(CREDENTIAL));
	cred->Flags = 0;
	cred->Type = 2;
	wchar_t *target = L"TERMSRV/srvrdsnlb.ath.ru";
	cred->TargetName = target;
	cred->Comment = NULL;
	std::wstring upassword(password);
	cred->CredentialBlob = (LPBYTE)upassword.c_str();
	int s = upassword.size() * sizeof(wchar_t);
	cred->CredentialBlobSize = s;
	cred->TargetAlias = NULL;
	cred->Persist = CRED_PERSIST_ENTERPRISE;
	std::wstring uname(L"ath\\");
	uname.append(username);
	//cred->CredentialBlob = (LPBYTE)uname.c_str();
	cred->UserName = (LPWSTR)uname.c_str();
	if (CredWrite(cred, 0)) {
		wchar_t cmdmstsc[] = L"c:\\windows\\system32\\mstsc.exe /v:srvrdsnlb.ath.ru\0";
		SECURITY_ATTRIBUTES sa;
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		sa.nLength = sizeof(sa);
		STARTUPINFO sp = {};
		PROCESS_INFORMATION pi = {};

		BOOL bCP = CreateProcess(NULL,
			cmdmstsc, &sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sp, &pi);
		if (bCP == 0) {
			int err = GetLastError();
			return false;
		}
	}
	return true;
}

void ATHClientIfc::deleteCred()
{
	CredDelete(L"TERMSRV/srvrdsnlb.ath.ru",	2, 0);
}

void ATHClientIfc::StatsCB(VPNStats & stats)
{
}

void ATHClientIfc::StateCB(const VPNState state, const VPNSubState subState, const tstring stateString)
{
	s_status->SetWindowTextW(stateString.c_str());
}

void ATHClientIfc::BannerCB(const tstring & banner)
{
	setBannerResponse(true);
	s_status->SetWindowTextW(banner.c_str());
}

void ATHClientIfc::NoticeCB(const tstring notice, const MessageType type)
{
	s_status->SetWindowTextW(notice.c_str());
	//ClientIfc::setBannerResponse(true);
}

void ATHClientIfc::ServiceReadyCB()
{
	s_status->SetWindowTextW(L"ServiceReadyCB");
	setBanner(L"ServiceReadyCB");
}

void ATHClientIfc::UserPromptCB(ConnectPromptInfo & ConnectPrompt)
{
	std::wstring message(L"User Message: ");
	message.append(ConnectPrompt.getMessage());
	std::list<tstring> promptNames;
	ConnectPrompt.getListPromptNames(promptNames);
	std::list<tstring> ::iterator name_iter;
	for (name_iter = promptNames.begin();
		name_iter != promptNames.end(); ++name_iter)
	{
		tstring promptName = *name_iter;
		PromptEntry *entry = ConnectPrompt.getPromptEntry(promptName);
		tstring entryName = entry->getPromptName();
		PromptType pt = entry->getPromptType();
		if (entryName == PromptEntry::Username)
		{
			std::wstring uname(username);
			entry->setValue(uname);
		}
		else if (entryName == PromptEntry::Password)
		{
			std::wstring upassword(password);
			entry->setValue(upassword);
		}
	}

	UserSubmit();
}

void ATHClientIfc::CertBlockedCB(const tstring & rtstrUntrustedServer)
{
	setCertBlockedResponse(true);
	s_status->SetWindowTextW(L"CertBlockedCB");
}

void ATHClientIfc::CertWarningCB(const tstring & rtstrUntrustedServer, const std::list<tstring>& rltstrCertErrors, bool bAllowImport)
{
	setCertWarningResponse(true, true);
}
