#pragma once

#define MOD_DEVICE_MONITOR _T("Éè±¸¼à¿Ø")

#include <Windows.h>
#include <Dbt.h>

#include <afxwin.h>

#include "..\\PrintfEx\\PrintfEx.h"

class CDeviceMonitorWnd : public CWnd
{
public:

};

class CDeviceMonitor
{
public:
	BOOL
		Init(
		__in HANDLE hWindowOrService,
		__in BOOL	bService
		);

	BOOL
		Unload();

	BOOL
		MessageLoop();

private:
	static CDeviceMonitorWnd *	ms_pDeviceMonitorWnd;
	static HDEVNOTIFY			ms_hDevNotify;
};
