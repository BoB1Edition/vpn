// ath.vpn.service.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "ATHService.h"

int main(int argc, char *argv[]) {

	ATHService service;

	if (argc == 2) {
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
    return service.run();
}

