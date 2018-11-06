// ath.vpn.service.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "ATHService.h"
#include "EventMessage.h"

int main(int argc, char *argv[]) {
	EventMessage ev;
	LPWSTR message = new WCHAR[32000];
	//GetCurrentProcessId();
	wsprintf(message, L"main start: pid %i\0", GetCurrentProcessId());
	//wprintf(L"servicePath: %s\n", servicePath);
	ev.addLog(message);
	ATHService service;

	if (argc == 2) {
		if (_strcmpi("run", argv[1]) == 0) {
			wsprintf(message, L"run start: pid %i\0", GetCurrentProcessId());
			ev.addLog(message);
			return service.init();
		}
		if (_strcmpi("install", argv[1]) == 0) {
			return service.install();
		}
		if (_strcmpi("start", argv[1]) == 0) {
			return service.run();
		}
		if (_strcmpi("stop", argv[1]) == 0) {
			return service.stop();
		}
		if (_strcmpi("pause", argv[1]) == 0) {
			return service.pause();
		}
		if (_strcmpi("usage", argv[1]) == 0) {
			printf("first please run %s install\n", argv[0]);
			printf("second run %s start or service start in service.msc\n", argv[0]);
		}

		return 0;
	}
	return 0;

}

