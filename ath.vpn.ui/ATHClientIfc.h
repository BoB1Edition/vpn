#pragma once


#include "ClientIfc.h"
#include "VPNCOMMAND.h"

class ATHClientIfc : protected ClientIfc
{
public:
	ATHClientIfc(CStatic *s_status);
	~ATHClientIfc();

	int GetStatus();
	void inline SetCallbackStatus(CStatic *s_status) {
		this->s_status = s_status;
	}
protected:

	CStatic *s_status;

	virtual void StatsCB(VPNStats & stats) override;
	virtual void StateCB(const VPNState state, const VPNSubState subState, const tstring stateString) override;
	virtual void BannerCB(const tstring & banner) override;
	virtual void NoticeCB(const tstring notice, const MessageType type) override;
	virtual void ServiceReadyCB() override;
	virtual void UserPromptCB(ConnectPromptInfo & ConnectPrompt) override;
	virtual void CertBlockedCB(const tstring & rtstrUntrustedServer) override;
	virtual void CertWarningCB(const tstring & rtstrUntrustedServer, const std::list<tstring>& rltstrCertErrors, bool bAllowImport) override;
private:
	bool Attach;
};

