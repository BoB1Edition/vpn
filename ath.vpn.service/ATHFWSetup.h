#pragma once
#include <netfw.h>
#include <json/json.h>
#include <iostream>
#include <fstream>

struct FWStruct {
	NET_FW_ACTION				Action;
	BSTR						ApplicationName;
	BSTR						Description;
	NET_FW_RULE_DIRECTION		Direction;
	VARIANT_BOOL				EdgeTraversal;
	VARIANT_BOOL				Enabled;
	BSTR						Grouping;
	BSTR						IcmpTypesAndCodes;
	VARIANT						Interfaces;
	BSTR						InterfaceTypes;
	BSTR						LocalAddresses;
	BSTR						LocalPorts;
	BSTR						Name;
	long						Profiles;
	LONG						Protocol;
	BSTR						RemoteAddresses;
	BSTR						RemotePorts;
	BSTR						ServiceName;
};

struct FWSettings {
	VARIANT_BOOL domainProfileEnabled;
	VARIANT_BOOL publicProfileEnabled;
	VARIANT_BOOL privateProfileEnabled;
	VARIANT_BOOL domainBlockAllInboundTraffic;
	VARIANT_BOOL publicBlockAllInboundTraffic;
	VARIANT_BOOL privateBlockAllInboundTraffic;
	NET_FW_ACTION domainDefaultInboundAction;
	NET_FW_ACTION publicDefaultInboundAction;
	NET_FW_ACTION privateDefaultInboundAction;
	NET_FW_ACTION domainDefaultOutboundAction;
	NET_FW_ACTION publicDefaultOutboundAction;
	NET_FW_ACTION privateDefaultOutboundAction;
	std::vector<FWStruct*> fwstructs;
};

class ATHFWSetup
{
public:
	ATHFWSetup();
	~ATHFWSetup();

	int addPolicy(FWStruct &fw);
	int DeleteAllRules();
	int SaveRulesToFile(LPCWSTR fName);
	int LoadRulesFromFile(LPCWSTR fName);
private:
	INetFwMgr* fwMgr = NULL;
	INetFwPolicy2* fwPolicy2 = NULL;
	INetFwPolicy* fwPolicy = NULL;
	INetFwRules *RulesObject = NULL;

	char*  BstrToChar(BSTR str);

	bool VariantBoolToBool(VARIANT_BOOL vbool);
};

