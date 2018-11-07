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
	UPFAIL,
	FWCHANGED
};

struct VPNCOMMAND {
	int messsage;
	tagVPNCOMMAND command;
};
