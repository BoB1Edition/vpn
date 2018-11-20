#include "stdafx.h"
#include "ATHFWSetup.h"
#include "base64.h"



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

int ATHFWSetup::addPolicy(FWStruct &fw) {
	INetFwRule *rule = NULL;
	
	CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&rule);
	HRESULT hr;
	hr = rule->put_Direction(fw.Direction);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_ApplicationName(fw.ApplicationName);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_Action(fw.Action);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_Protocol(fw.Protocol);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_LocalAddresses(fw.LocalAddresses);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	if(fw.LocalPorts != NULL)
	hr = rule->put_LocalPorts(fw.LocalPorts);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_RemoteAddresses(fw.RemoteAddresses);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	if (fw.LocalPorts != NULL)
	hr = rule->put_RemotePorts(fw.RemotePorts);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	if (fw.IcmpTypesAndCodes != NULL) {
		hr = rule->put_IcmpTypesAndCodes(fw.IcmpTypesAndCodes);
		if (FAILED(hr)) {
			err = hr;
			return err;
		}
	}
	hr = rule->put_Action(fw.Action);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_Description(fw.Description);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_EdgeTraversal(fw.EdgeTraversal);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_Enabled(fw.Enabled);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_Grouping(fw.Grouping);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_InterfaceTypes(fw.InterfaceTypes);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	/*hr = rule->put_Profiles(fw.Profiles);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}*/
	hr = rule->put_Name(fw.Name);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	hr = rule->put_ServiceName(fw.ServiceName);
	if (FAILED(hr)) {
		err = hr;
		return err;
	}
	
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
			HRESULT hrror= RulesObject->Remove(bstr);
			if (hrror != S_OK) {
				int err = GetLastError();
				int e = hrror;
				LPWSTR errMessage = new WCHAR[32000];
				wsprintf(errMessage, L"Rules: %s, not deleted: %i, %i\n", bstr, err, hrror);
				ev.addLog(errMessage);
				hr = pEnum->Next(1, &var, &lFetch);

				continue;
			}
			SysFreeString(bstr);
			pADs->Release();
		}
		else {
			//printf("lFetch: %i", lFetch);
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
	std::ofstream fwsetting(fName);
	fwsetting.flush();
	fwsetting.close();
	fwsetting.open(fName);
	if(!fwsetting.is_open())
		return -1;
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
			/*if (fw->Profiles != 0) {
				char * ctestProfiles = (char *)fw->Profiles;
				wchar_t * wtestProfiles = (wchar_t *)fw->Profiles;

				wprintf(L"wtestProfiles: %s", wtestProfiles);
			}*/
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
	
	writer.write(fwsetting, root);
	fwsetting.flush();
	fwsetting.close();
	return 0;
}

int ATHFWSetup::LoadRulesFromFile(LPCWSTR fName)
{
	FWSettings fwsettings = { 0 };
	std::ofstream ferr(L"File.err");
	std::ofstream fok(L"File.ok");
	std::ifstream fwsetting(fName);
	if (!fwsetting.is_open()) {
		return -2;
	}
	Json::Value root;
	fwsetting >> root;
	fwsettings.domainProfileEnabled = BoolToVariantBool(root["domainProfileEnabled"].asBool());
	fwsettings.privateProfileEnabled = BoolToVariantBool(root["privateProfileEnabled"].asBool());
	fwsettings.publicProfileEnabled = BoolToVariantBool(root["publicProfileEnabled"].asBool());

	fwsettings.domainBlockAllInboundTraffic = BoolToVariantBool(root["domainBlockAllInboundTraffic"].asBool());
	fwsettings.privateBlockAllInboundTraffic = BoolToVariantBool(root["privateBlockAllInboundTraffic"].asBool());
	fwsettings.publicBlockAllInboundTraffic = BoolToVariantBool(root["publicBlockAllInboundTraffic"].asBool());

	fwsettings.domainDefaultInboundAction = (NET_FW_ACTION)root["domainDefaultInboundAction"].asInt64();
	fwsettings.publicDefaultInboundAction = (NET_FW_ACTION)root["publicDefaultInboundAction"].asInt64();
	fwsettings.privateDefaultInboundAction = (NET_FW_ACTION)root["privateDefaultInboundAction"].asInt64();

	fwsettings.domainDefaultOutboundAction = (NET_FW_ACTION)root["domainDefaultOutboundAction"].asInt64();
	fwsettings.publicDefaultOutboundAction = (NET_FW_ACTION)root["publicDefaultOutboundAction"].asInt64();
	fwsettings.privateDefaultOutboundAction = (NET_FW_ACTION)root["privateDefaultOutboundAction"].asInt64();

	HRESULT hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, fwsettings.domainProfileEnabled);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_DOMAIN, fwsettings.domainBlockAllInboundTraffic);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_DOMAIN, fwsettings.domainDefaultInboundAction);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_DOMAIN, fwsettings.domainDefaultOutboundAction);

	hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, fwsettings.publicProfileEnabled);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_PUBLIC, fwsettings.publicBlockAllInboundTraffic);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_PUBLIC, fwsettings.publicDefaultInboundAction);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, fwsettings.publicDefaultOutboundAction);

	hr = fwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, fwsettings.privateProfileEnabled);
	hr = fwPolicy2->put_BlockAllInboundTraffic(NET_FW_PROFILE2_PRIVATE, fwsettings.privateBlockAllInboundTraffic);
	hr = fwPolicy2->put_DefaultInboundAction(NET_FW_PROFILE2_PRIVATE, fwsettings.privateDefaultInboundAction);
	hr = fwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PRIVATE, fwsettings.privateDefaultOutboundAction);


	fwsettings.fwstructs.clear();

	for (int unsigned i = 0; i < root["Rules"].size(); i++) {
		Json::Value rule = root["Rules"][i];
		FWStruct fw = { };
		fw.Action = (NET_FW_ACTION)rule["Action"].asInt64();
		fw.ApplicationName = CharToBstr(rule["ApplicationName"].asCString());
		fw.Description = CharToBstr(rule["Description"].asCString());
		fw.Direction = (NET_FW_RULE_DIRECTION)rule["Direction"].asInt64();

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
		if (FAILED(addPolicy(fw))) {
			ferr << rule["ApplicationName"].asCString() << ":" << rule["Name"].asCString() << ":" << fw.Profiles << "\r\n";
			ferr.flush();
			
		}
		else {
			fok << rule["ApplicationName"].asCString() << ":" << rule["Name"].asCString() << ":" << fw.Profiles << "\r\n";
			fok.flush();
		}
	}
	ferr.close();
	fok.close();
	fwsetting.close();
	return 0;
}

char * ATHFWSetup::BstrToChar(BSTR str) {
	_bstr_t origin = str;
	_bstr_t end = L"\0";
	_bstr_t finalstr = origin + end;
	int lenght = SysStringByteLen(finalstr.GetBSTR());
	std::string *name = new std::string(base64_encode((const unsigned char*)finalstr.GetBSTR(), lenght + 2));
	char *ret = (char*)name->c_str();
	return ret;
	/*
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
	*/
}

BSTR ATHFWSetup::CharToBstr(const char * str)
{
	/*if (strcmp("", str) != 0) {
		bstr_t * ret = new bstr_t(str);
		return ret->GetBSTR();
	}
	return NULL;*/
	std::string *tmp = new std::string(str);
	*tmp = base64_decode(*tmp);
	BSTR bstr = (BSTR)tmp->c_str();
	if (wcscmp(bstr, L"") == 0) return NULL;
	return bstr;
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
	if (b) {
		return VARIANT_TRUE;
	}
	else {
		return VARIANT_FALSE;
	}
}
