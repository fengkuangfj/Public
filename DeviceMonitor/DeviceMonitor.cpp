#include "DeviceMonitor.h"

CDeviceMonitorWnd * CDeviceMonitor::ms_pDeviceMonitorWnd	= NULL;
HDEVNOTIFY			CDeviceMonitor::ms_hDevNotify			= NULL;

BOOL
	CDeviceMonitor::Init(
	__in HANDLE hWindowOrService,
	__in BOOL	bService
	)
{
	BOOL							bRet				= FALSE;

	DEV_BROADCAST_DEVICEINTERFACE	NotificationFilter	= {0};
	HDEVNOTIFY						hDevNotify			= NULL;
	DWORD							dwFlags				= 0;


	__try
	{
		NotificationFilter.dbcc_size = sizeof(NotificationFilter);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_VOLUME;

		if (bService)
		{
			dwFlags = DEVICE_NOTIFY_SERVICE_HANDLE;


			if (!hWindowOrService)
			{
				printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "input argument error");
				__leave;
			}
		}
		else
		{
			dwFlags = DEVICE_NOTIFY_WINDOW_HANDLE;

			if (!hWindowOrService)
			{
				if (!ms_pDeviceMonitorWnd)
				{
					ms_pDeviceMonitorWnd = new CDeviceMonitorWnd();
					if (!ms_pDeviceMonitorWnd)
					{
						printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "new failed. (%d)", GetLastError());
						__leave;
					}
				}

				hWindowOrService = ms_pDeviceMonitorWnd->m_hWnd;
			}
		}

		ms_hDevNotify = RegisterDeviceNotification(
			hWindowOrService,
			&NotificationFilter,
			dwFlags
			);
		if (!ms_hDevNotify)
		{
			printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "RegisterDeviceNotification failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CDeviceMonitor::Unload()
{
	BOOL bRet = FALSE;


	__try
	{
		if (ms_hDevNotify)
		{
			if (!UnregisterDeviceNotification(ms_hDevNotify))
				printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "UnregisterDeviceNotification failed. (%d)", GetLastError());
			else
				ms_hDevNotify = NULL;
		}

		if (ms_pDeviceMonitorWnd)
		{
			delete ms_pDeviceMonitorWnd;
			ms_pDeviceMonitorWnd = NULL;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
	CDeviceMonitor::MessageLoop()
{
	BOOL	bRet	= FALSE;

	MSG		msg		= {0};


	__try
	{
		while (0 != (bRet = GetMessage(&msg, NULL, 0, 0)))
		{
			if (-1 == bRet)
			{
				printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "GetMessage failed. (%d)", GetLastError());
				__leave;
			}

			switch (msg.message)
			{
			case WM_DEVICECHANGE:
				{
					switch(msg.wParam)
					{
					case DBT_DEVICEARRIVAL:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "system detected a new device");
							break;
						}
					case DBT_DEVICEQUERYREMOVE:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "wants to remove, may fail");
							break;
						}
					case DBT_DEVICEQUERYREMOVEFAILED:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "removal aborted");
							break;
						}
					case DBT_DEVICEREMOVEPENDING:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "about to remove, still avail");
							break;
						}
					case DBT_DEVICEREMOVECOMPLETE:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "device is gone");
							break;
						}
					case DBT_DEVICETYPESPECIFIC:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_INFORMATION, "type specific event");
							break;
						}
					default:
						{
							printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "wParam error. 0x%08x", msg.wParam);
							__leave;
						}
					}

					break;
				}
			default:
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}
