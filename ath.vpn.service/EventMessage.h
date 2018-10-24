#pragma once
#ifndef EVENT_MESSAGE_H
#define EVENT_MESSAGE_H



#include <Windows.h>
#include <psapi.h>
#include "Event_log.h"

class EventMessage
{
public:
	EventMessage();
	~EventMessage();
	void addLog(LPWSTR message);
private:
	HANDLE hEventLog;
};

#endif // !EVENT_MESSAGE_H