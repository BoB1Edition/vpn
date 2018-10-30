#include "stdafx.h"
#include "ATHFWSetup.h"


ATHFWSetup::ATHFWSetup()
{
	HRESULT hr = S_OK;
	HRESULT comInit = E_FAIL;
	INetFwProfile* fwProfile = NULL;
	comInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	hr = CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), (void**)&fwMgr);
	CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&fwPolicy2);
	fwMgr->get_LocalPolicy(&fwPolicy);
	fwPolicy->get_CurrentProfile(&fwProfile);
	fwPolicy2->get_Rules(&RulesObject);
}


ATHFWSetup::~ATHFWSetup()
{
}

int ATHFWSetup::addPolicy(FWStruct &fw)
{
	INetFwRule *rule;
	CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&rule);
	rule->put_Action(fw.Action);
	rule->put_ApplicationName(fw.ApplicationName);
	rule->put_Description(fw.Description);
	rule->put_Direction(fw.Direction);
	rule->put_EdgeTraversal(fw.EdgeTraversal);
	rule->put_Enabled(fw.Enabled);
	rule->put_Grouping(fw.Grouping);
	rule->put_IcmpTypesAndCodes(fw.IcmpTypesAndCodes);
	rule->put_Interfaces(fw.Interfaces);
	rule->put_InterfaceTypes(fw.InterfaceTypes);
	rule->put_LocalAddresses(fw.LocalAddresses);
	rule->put_LocalPorts(fw.LocalPorts);
	rule->put_Name(fw.Name);
	rule->put_Profiles(fw.Profiles);
	rule->put_Protocol(fw.Protocol);
	rule->put_RemoteAddresses(fw.RemoteAddresses);
	rule->put_RemotePorts(fw.RemotePorts);
	rule->put_ServiceName(fw.ServiceName);
	HRESULT hr;
	hr = RulesObject->Add(rule);
	return hr;
}

int ATHFWSetup::DeleteAllRules()
{
	IUnknown *pUnk = NULL;
	RulesObject->get__NewEnum(&pUnk);
	IEnumVARIANT *pEnum;
	HRESULT hr = pUnk->QueryInterface(IID_IEnumVARIANT, (void**)&pEnum);
	BSTR bstr = NULL;
	VARIANT var;
	INetFwRule *pADs = NULL;
	ULONG lFetch;
	IDispatch *pDisp = NULL;
	VariantInit(&var);
	hr = pEnum->Next(1, &var, &lFetch);
	while (hr == S_OK)
	{
		if (lFetch == 1)
		{
			pDisp = V_DISPATCH(&var);
			pDisp->QueryInterface(IID_INetFwRule, (void**)&pADs);
			pADs->get_Name(&bstr);
			hr = RulesObject->Remove(bstr);
			//printf("Session name: %S\n", bstr);
			SysFreeString(bstr);
			pADs->Release();
		}
		VariantClear(&var);
		hr = pEnum->Next(1, &var, &lFetch);
	}
	hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, VARIANT_TRUE);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_DOMAIN, VARIANT_TRUE);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_DOMAIN, NET_FW_ACTION_BLOCK);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_DOMAIN, NET_FW_ACTION_BLOCK);

	hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, VARIANT_TRUE);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_PUBLIC, VARIANT_TRUE);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_PUBLIC, NET_FW_ACTION_BLOCK);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, NET_FW_ACTION_BLOCK);

	hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, VARIANT_TRUE);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_PRIVATE, VARIANT_TRUE);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_PRIVATE, NET_FW_ACTION_BLOCK);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PRIVATE, NET_FW_ACTION_BLOCK);
	return 0;
}

int ATHFWSetup::SaveRulesToFile(LPCWSTR fName)
{
	FWSettings fwsettings = {};
	HRESULT hr = fwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, &fwsettings.domainProfileEnabled);
	hr = fwPolicy2->get_BlockAllInboundTraffic(NET_FW_PROFILE2_DOMAIN, &fwsettings.domainBlockAllInboundTraffic);
	hr = fwPolicy2->get_DefaultInboundAction(NET_FW_PROFILE2_DOMAIN, &fwsettings.domainDefaultInboundAction);
	hr = fwPolicy2->get_DefaultOutboundAction(NET_FW_PROFILE2_DOMAIN, &fwsettings.domainDefaultOutboundAction);

	hr = fwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, &fwsettings.publicProfileEnabled);
	hr = fwPolicy2->get_BlockAllInboundTraffic(NET_FW_PROFILE2_PUBLIC, &fwsettings.publicBlockAllInboundTraffic);
	hr = fwPolicy2->get_DefaultInboundAction(NET_FW_PROFILE2_PUBLIC, &fwsettings.publicDefaultInboundAction);
	hr = fwPolicy2->get_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, &fwsettings.publicDefaultOutboundAction);

	hr = fwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, &fwsettings.privateProfileEnabled);
	hr = fwPolicy2->get_BlockAllInboundTraffic(NET_FW_PROFILE2_PRIVATE, &fwsettings.privateBlockAllInboundTraffic);
	hr = fwPolicy2->get_DefaultInboundAction(NET_FW_PROFILE2_PRIVATE, &fwsettings.privateDefaultInboundAction);
	hr = fwPolicy2->get_DefaultOutboundAction(NET_FW_PROFILE2_PRIVATE, &fwsettings.privateDefaultOutboundAction);

	Json::Value root;

	root["domainProfileEnabled"] = VariantBoolToBool(fwsettings.domainProfileEnabled);
	root["privateProfileEnabled"] = VariantBoolToBool(fwsettings.privateProfileEnabled);
	root["publicProfileEnabled"] = VariantBoolToBool(fwsettings.publicProfileEnabled);

	root["domainBlockAllInboundTraffic"] = VariantBoolToBool(fwsettings.domainBlockAllInboundTraffic);
	root["privateBlockAllInboundTraffic"] = VariantBoolToBool(fwsettings.privateBlockAllInboundTraffic);
	root["publicBlockAllInboundTraffic"] = VariantBoolToBool(fwsettings.publicBlockAllInboundTraffic);

	root["domainDefaultInboundAction"] = (int)fwsettings.domainDefaultInboundAction;
	root["publicDefaultInboundAction"] = (int)fwsettings.publicDefaultInboundAction;
	root["privateDefaultInboundAction"] = (int)fwsettings.privateDefaultInboundAction;

	root["domainDefaultOutboundAction"] = (int)fwsettings.domainDefaultOutboundAction;
	root["publicDefaultOutboundAction"] = (int)fwsettings.publicDefaultOutboundAction;
	root["privateDefaultOutboundAction"] = (int)fwsettings.privateDefaultOutboundAction;


	IUnknown *pUnk = NULL;
	RulesObject->get__NewEnum(&pUnk);
	IEnumVARIANT *pEnum;
	hr = pUnk->QueryInterface(IID_IEnumVARIANT, (void**)&pEnum);
	VARIANT var;
	INetFwRule *pADs = NULL;
	ULONG lFetch;
	IDispatch *pDisp = NULL;
	VariantInit(&var);
	hr = pEnum->Next(1, &var, &lFetch);

	Json::Value rules;

	while (hr == S_OK)
	{
		if (lFetch == 1)
		{

			pDisp = V_DISPATCH(&var);
			pDisp->QueryInterface(IID_INetFwRule, (void**)&pADs);
			FWStruct * fw = new FWStruct;
			pADs->get_Action(&fw->Action);
			pADs->get_ApplicationName(&fw->ApplicationName);
			pADs->get_Description(&fw->Description);
			pADs->get_Direction(&fw->Direction);
			pADs->get_EdgeTraversal(&fw->EdgeTraversal);
			pADs->get_Enabled(&fw->Enabled);
			pADs->get_Grouping(&fw->Grouping);
			pADs->get_IcmpTypesAndCodes(&fw->IcmpTypesAndCodes);
			pADs->get_Interfaces(&fw->Interfaces);
			pADs->get_InterfaceTypes(&fw->InterfaceTypes);
			pADs->get_LocalAddresses(&fw->LocalAddresses);
			pADs->get_LocalPorts(&fw->LocalPorts);
			pADs->get_Name(&fw->Name);
			pADs->get_Profiles(&fw->Profiles);
			pADs->get_Protocol(&fw->Protocol);
			pADs->get_RemoteAddresses(&fw->RemoteAddresses);
			pADs->get_RemotePorts(&fw->RemotePorts);
			pADs->get_ServiceName(&fw->ServiceName);


			Json::Value rule;

			rule["Action"] = fw->Action;
			rule["ApplicationName"] = BstrToChar(fw->ApplicationName);
			rule["Description"] = BstrToChar(fw->Description);
			rule["Direction"] = fw->Direction;

			rule["EdgeTraversal"] = VariantBoolToBool(fw->EdgeTraversal);
			rule["Enabled"] = VariantBoolToBool(fw->Enabled);
			rule["Grouping"] = BstrToChar(fw->Grouping);
			rule["IcmpTypesAndCodes"] = BstrToChar(fw->IcmpTypesAndCodes);
			rule["Interfaces"] = BstrToChar(fw->Interfaces.bstrVal);
			rule["InterfaceTypes"] = BstrToChar(fw->InterfaceTypes);
			rule["LocalAddresses"] = BstrToChar(fw->LocalAddresses);
			rule["LocalPorts"] = BstrToChar(fw->LocalPorts);
			rule["Name"] = BstrToChar(fw->Name);
			rule["Profiles"] = fw->Profiles;
			rule["Protocol"] = fw->Protocol;
			rule["RemoteAddresses"] = BstrToChar(fw->RemoteAddresses);
			rule["RemotePorts"] = BstrToChar(fw->RemotePorts);
			rule["ServiceName"] = BstrToChar(fw->ServiceName);
			root["Rules"].append(rule);
		}
		VariantClear(&var);
		hr = pEnum->Next(1, &var, &lFetch);
	}
	Json::StyledStreamWriter writer;
	std::ofstream fwsetting(fName);
	writer.write(fwsetting, root);
	fwsetting.flush();
	fwsetting.close();
	return 0;
}

int ATHFWSetup::LoadRulesFromFile(LPCWSTR fName)
{
	FWSettings fwsettings = { 0 };
	std::ifstream fwsetting(fName);

	Json::Value root;
	fwsetting >> root;
	fwsettings.domainProfileEnabled = BoolToVariantBool(root["domainProfileEnabled"].asBool());
	fwsettings.privateProfileEnabled = BoolToVariantBool(root["privateProfileEnabled"].asBool());
	fwsettings.publicProfileEnabled = BoolToVariantBool(root["publicProfileEnabled"].asBool());

	fwsettings.domainBlockAllInboundTraffic = BoolToVariantBool(root["domainBlockAllInboundTraffic"].asBool());
	fwsettings.privateBlockAllInboundTraffic = BoolToVariantBool(root["privateBlockAllInboundTraffic"].asBool());
	fwsettings.publicBlockAllInboundTraffic = BoolToVariantBool(root["publicBlockAllInboundTraffic"].asBool());

	fwsettings.domainDefaultInboundAction = (NET_FW_ACTION)root["domainDefaultInboundAction"].asInt();
	fwsettings.publicDefaultInboundAction = (NET_FW_ACTION)root["publicDefaultInboundAction"].asInt();
	fwsettings.privateDefaultInboundAction = (NET_FW_ACTION)root["privateDefaultInboundAction"].asInt();

	fwsettings.domainDefaultOutboundAction = (NET_FW_ACTION)root["domainDefaultOutboundAction"].asInt();
	fwsettings.domainDefaultOutboundAction = (NET_FW_ACTION)root["publicDefaultOutboundAction"].asInt();
	fwsettings.domainDefaultOutboundAction = (NET_FW_ACTION)root["privateDefaultOutboundAction"].asInt();

	fwsettings.fwstructs.clear();

	for (int i = 0; i < root["Rules"].size(); i++) {
		Json::Value rule = root["Rules"][i];
		FWStruct fw;
		fw.Action = (NET_FW_ACTION)rule["Action"].asInt();
		fw.ApplicationName = CharToBstr(rule["ApplicationName"].asCString());
		fw.Description = CharToBstr(rule["Description"].asCString());
		fw.Direction = (NET_FW_RULE_DIRECTION)rule["Direction"].asInt();

		fw.EdgeTraversal = BoolToVariantBool(rule["EdgeTraversal"].asBool());
		fw.Enabled = BoolToVariantBool(rule["Enabled"].asBool());
		fw.Grouping = CharToBstr(rule["Grouping"].asCString());
		fw.IcmpTypesAndCodes = CharToBstr(rule["IcmpTypesAndCodes"].asCString());
		fw.Interfaces.bstrVal = CharToBstr(rule["Interfaces"].asCString());
		fw.InterfaceTypes = CharToBstr(rule["InterfaceTypes"].asCString());
		fw.LocalAddresses = CharToBstr(rule["LocalAddresses"].asCString());
		fw.LocalPorts = CharToBstr(rule["LocalPorts"].asCString());
		fw.Name = CharToBstr(rule["Name"].asCString());
		fw.Profiles = rule["Profiles"].asInt();
		fw.Protocol = rule["Protocol"].asInt();
		fw.RemoteAddresses = CharToBstr(rule["RemoteAddresses"].asCString());
		fw.RemotePorts = CharToBstr(rule["RemotePorts"].asCString());
		fw.ServiceName = CharToBstr(rule["ServiceName"].asCString());
		addPolicy(fw);
	}

	return 0;
}

char * ATHFWSetup::BstrToChar(BSTR str) {
	LPWSTR lpwstr = new WCHAR[SysStringLen(str) + 10];
	wsprintf(lpwstr, L"%s\0", str);
	char *cOut = new char[SysStringLen(str) + 11];
	int conv = WideCharToMultiByte(1251, WC_COMPOSITECHECK, lpwstr, -1, cOut, 0, NULL, NULL);
	conv = WideCharToMultiByte(1251, WC_COMPOSITECHECK, lpwstr, -1, cOut, conv, NULL, NULL);
	if (conv == 0) {
		wsprintf(lpwstr, L"err: conv %i, err: %i", conv, GetLastError());
		return (char*)lpwstr;
	}
	return cOut;
}

BSTR ATHFWSetup::CharToBstr(const char * str)
{
	return BSTR();
}

bool ATHFWSetup::VariantBoolToBool(VARIANT_BOOL vbool) {
	bool ret;
	if (vbool) {
		ret = true;
	}
	else {
		ret = false;
	}

	return ret;
}

VARIANT_BOOL ATHFWSetup::BoolToVariantBool(bool b)
{
	return VARIANT_BOOL();
}
