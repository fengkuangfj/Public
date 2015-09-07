#pragma once

#define MOD_DEVICE_MONITOR _T("Éè±¸¼à¿Ø")

#include <Windows.h>
#include <Dbt.h>

#include "..\\SimpleDump\\SimpleDump.h"
#include "..\\PrintfEx\\PrintfEx.h"

class CDeviceMonitorWnd
{
public:

};

class CDeviceMonitor
{
public:
	BOOL
		Init();

	BOOL
		MessageLoop();

private:

};
