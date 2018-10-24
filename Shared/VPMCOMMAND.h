#pragma once


enum tagVPNCOMMAND {
	CHECKAV,
	CHECKFW,
	CHECKUP,
	CHANGEFW,
	RESTOREFW,
	FWOK,
	AVOK,
	UPOK,
	FWFAIL,
	AVFAIL,
	UPFAIL
};

struct VPNCOMMAND {
	int messsage;
	tagVPNCOMMAND command;
};
