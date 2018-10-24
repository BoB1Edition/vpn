#include "stdafx.h"
#include "ATHClientIfc.h"


ATHClientIfc::ATHClientIfc(CStatic *s_status)
{
	this->s_status = s_status;
}


ATHClientIfc::~ATHClientIfc()
{
}

int ATHClientIfc::GetStatus()
{
	bool Attach = ClientIfc::attach(false, false, true, true);
	if (!Attach)
		return -1;
	if(ClientIfc::isConnected())
		return 1;
	if (ClientIfc::isAvailable())
		return 2;
	return 0;
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
	s_status->SetWindowTextW(banner.c_str());
}

void ATHClientIfc::NoticeCB(const tstring notice, const MessageType type)
{
	s_status->SetWindowTextW(notice.c_str());
}

void ATHClientIfc::ServiceReadyCB()
{
}

void ATHClientIfc::UserPromptCB(ConnectPromptInfo & ConnectPrompt)
{
}

void ATHClientIfc::CertBlockedCB(const tstring & rtstrUntrustedServer)
{
}

void ATHClientIfc::CertWarningCB(const tstring & rtstrUntrustedServer, const std::list<tstring>& rltstrCertErrors, bool bAllowImport)
{
}
