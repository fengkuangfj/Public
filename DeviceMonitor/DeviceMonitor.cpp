#include "DeviceMonitor.h"

BOOL
	CDeviceMonitor::Init(
	__in HANDLE hWorS
	__in BOOL	bW
	)
{
	BOOL bRet = FALSE;

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {0};
	HDEVNOTIFY hDevNotify = NULL;


	__try
	{

	
			ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
			NotificationFilter.dbcc_size		= sizeof(DEV_BROADCAST_DEVICEINTERFACE);
			NotificationFilter.dbcc_devicetype	= DBT_DEVTYP_DEVICEINTERFACE;
			NotificationFilter.dbcc_classguid	= GUID_DEVINTERFACE_VOLUME;

			hDevNotify = RegisterDeviceNotification(
				hWorS,
				&NotificationFilter,
				DEVICE_NOTIFY_WINDOW_HANDLE
				);


			if(!*hDevNotify)
			{
				
			}

		


// 		switch (CSimpleDump::GetApplicationType())
// 		{
// 		case APPLICATION_TYPE_CONSOLE:
// 			{
// 				break;
// 			}
// 		case APPLICATION_TYPE_NOT_CONSOLE:
// 			{
// 				break;
// 			}
// 		default:
// 			{
// 				printfEx(MOD_DEVICE_MONITOR, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p %d", lpOutBuf, ulOutBufSizeCh);
// 				__leave;
// 			}
// 		}
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
