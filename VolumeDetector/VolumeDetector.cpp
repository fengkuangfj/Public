#include "VolumeDetector.h"

HANDLE		CVolumeDetector::ms_hWindowOrService	= NULL;
BOOL		CVolumeDetector::ms_bService			= FALSE;
HDEVNOTIFY	CVolumeDetector::ms_hDevNotify			= NULL;

BOOL
	CVolumeDetector::Init(
	__in_opt	LPTSTR	lpModuleName,
	__in		HANDLE	hWindowOrService,
	__in		BOOL	bService,
	__in		BOOL	bCreateMassageLoop,
	__in		BOOL	bCreateMassageLoopThread
	)
{
	BOOL							bRet				= FALSE;

	DEV_BROADCAST_DEVICEINTERFACE	NotificationFilter	= {0};

	CVolumeDetector					VolumeDetector;

	// https://msdn.microsoft.com/en-us/library/aa363432(v=VS.85).aspx
	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		if (!ms_hWindowOrService)
		{
			ms_hWindowOrService = hWindowOrService;
			ms_bService = bService;

			if (ms_bService)
			{
				if (!ms_hWindowOrService)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
					__leave;
				}
				
				NotificationFilter.dbcc_size = sizeof(NotificationFilter);
				NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
				NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_VOLUME;
	
				ms_hDevNotify = RegisterDeviceNotification(ms_hWindowOrService, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);
				if (!ms_hDevNotify)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "RegisterDeviceNotification failed. (%d)", GetLastError());
					__leave;
				}
			}
			else
			{
				if (!ms_hWindowOrService)
				{
					ms_hWindowOrService = CreateWnd(lpModuleName, NULL);
					if (!ms_hWindowOrService)
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "CreateWnd failed");
						__leave;
					}
				}

				if (!bCreateMassageLoop)
				{
					if (!VolumeDetector.MessageLoop(bCreateMassageLoopThread))
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "VolumeDetector.MessageLoop failed");
						__leave;
					}
				}
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}

BOOL
	CVolumeDetector::Unload()
{
	BOOL bRet = FALSE;


	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		if (!ms_bService && ms_hWindowOrService)
			SendMessage((HWND)ms_hWindowOrService, WM_CLOSE, 0, 0);

		if (ms_hDevNotify)
		{
			if (!UnregisterDeviceNotification(ms_hDevNotify))
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "UnregisterDeviceNotification failed. (%d)", GetLastError());

			ms_hDevNotify = NULL;
		}

		bRet = TRUE;
	}
	__finally
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}

unsigned int
	__stdcall
	CVolumeDetector::MessageLoopWorkThread(
	__in void * lpParameter
	)
{
	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		if (!MessageLoopInternal())
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "MessageLoopInternal failed");
			__leave;
		}
	}
	__finally
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");
	}

	return 0;
}

BOOL
	CVolumeDetector::MessageLoop(
	__in BOOL bCreateThread
	)
{
	BOOL	bRet	= FALSE;

	HANDLE	hThread = NULL;


	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		if (bCreateThread)
		{
			hThread = (HANDLE)_beginthreadex(NULL, 0, MessageLoopWorkThread, NULL, 0, NULL);
			if (!hThread)
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "_beginthreadex failed. (%d)", GetLastError());
				__leave;
			}
		}
		else
		{
			if (!MessageLoopInternal())
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "MessageLoopInternal failed");
				__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (hThread)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}

		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}

BOOL
	CVolumeDetector::MessageLoopInternal()
{
	BOOL	bRet	= FALSE;

	MSG		msg		= {0};


	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		// Start the message loop. 
		while (0 != (bRet = GetMessage(&msg, NULL, 0, 0)))
		{
			if (-1 == bRet)
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "GetMessage failed. (%d)", GetLastError());
				__leave;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		bRet = TRUE;
	}
	__finally
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");
	}

	return bRet;
}

HANDLE
	CVolumeDetector::CreateWnd(
	__in_opt LPTSTR		lpModuleName,
	__in_opt HINSTANCE	hPrevInstance
	)
{
	HWND		hRet		= NULL;

	HINSTANCE	hInstance	= NULL;
	WNDCLASS	WndClass	= {0};


	__try
	{
		hInstance = GetModuleHandle(lpModuleName);
		if (!hInstance)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "GetModuleHandle failed. (%d)", GetLastError());
			__leave;
		}

		// Register the window class for the main window. 
		if (!hPrevInstance) 
		{
			WndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			WndClass.hInstance = hInstance;
			WndClass.lpfnWndProc = (WNDPROC)WndProc;
			WndClass.cbClsExtra = 0;
			WndClass.cbWndExtra = 0;
			WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			WndClass.hbrBackground = CreateSolidBrush(RGB(192,192,192));
			WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			WndClass.lpszClassName = WND_CLASS_NAME;
			WndClass.lpszMenuName = NULL;

			if (!RegisterClass(&WndClass)) 
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "RegisterClass failed. (%d)", GetLastError());
				__leave;
			}
		} 

		// Create the main window. 
		hRet = CreateWindow(
			WND_CLASS_NAME,
			_T("Sample"), 
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT, 
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL, 
			NULL,
			hInstance,
			NULL
			); 
		if (!hRet)
		{
			// If the main window cannot be created, terminate 
			// the application. 
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "CreateWindow failed. (%d)", GetLastError());
			__leave;
		}

		// Show the window and paint its contents. 
		ShowWindow(hRet, SW_HIDE); 
		UpdateWindow(hRet); 
	}
	__finally
	{
		;
	}

	return hRet;
}

LRESULT
	CALLBACK
	WndProc(
	HWND	hWnd,
	UINT	message,
	WPARAM	wParam,
	LPARAM	lParam
	)
{
	LRESULT					lRet				= 1;

	PDEV_BROADCAST_VOLUME	pDevBroadcastVolume	= NULL;
	TCHAR					tchName[MAX_PATH]	= {0};


	__try
	{
		switch (message)
		{
		case WM_CREATE:
			break;
		case WM_DEVICECHANGE:
			{
				pDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)lParam;
				if (!pDevBroadcastVolume)
					break;

				if (!CVolumeDetector::BinaryToVolume(pDevBroadcastVolume->dbcv_unitmask, tchName, _countof(tchName)))
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "BinaryToVolume failed. %I64d", pDevBroadcastVolume->dbcv_unitmask);
					break;
				}

				switch (LOWORD(wParam) | 0x8000)
				{
				case DBT_DEVICEARRIVAL:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "system detected a new device. %S", tchName);
						break;
					}
				case DBT_DEVICEQUERYREMOVE:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "wants to remove, may fail. %S", tchName);
						break;
					}
				case DBT_DEVICEQUERYREMOVEFAILED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "removal aborted. %S", tchName);
						break;
					}
				case DBT_DEVICEREMOVEPENDING:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "about to remove, still avail. %S", tchName);
						break;
					}
				case DBT_DEVICEREMOVECOMPLETE:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device is gone. %S", tchName);
						break;
					}
				case DBT_DEVICETYPESPECIFIC:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "type specific event. %S", tchName);
						break;
					}
#if (WINVER >= 0x040A)
				case DBT_CUSTOMEVENT:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "user-defined event. %S", tchName);
						break;
					}
#endif
#if (WINVER >= _WIN32_WINNT_WIN7)
				case DBT_DEVINSTENUMERATED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[>= _WIN32_WINNT_WIN7] system detected a new device. %S", tchName);
						break;
					}
				case DBT_DEVINSTSTARTED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[>= _WIN32_WINNT_WIN7] device installed and started. %S", tchName);
						break;
					}
				case DBT_DEVINSTREMOVED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[>= _WIN32_WINNT_WIN7] device removed from system. %S", tchName);
						break;
					}
				case DBT_DEVINSTPROPERTYCHANGED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[>= _WIN32_WINNT_WIN7] a property on the device changed. %S", tchName);
						break;
					}
#endif
				default:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "wParam error. 0x%08x", wParam);
						__leave;
					}
				}

				break;
			}
		case WM_CLOSE:
			{
				DestroyWindow(hWnd);
				break;
			}
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		default:
			{
				lRet = DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}
		}
	}
	__finally
	{
		;
	}

	return lRet;
}

BOOL
	CVolumeDetector::BinaryToVolume(
	__in	DWORD	dwBinary,
	__out	LPTSTR	lpInBuf,
	__in	ULONG	ulInBufSizeCh
	)
{
	BOOL	bRet	= FALSE;

	ULONG	ulIndex = 0;


	__try
	{
		if (!dwBinary || !lpInBuf || !ulInBufSizeCh)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. %I64d 0x%08p %d", dwBinary, lpInBuf, ulInBufSizeCh);
			__leave;
		}

		ulIndex = GetIndex(dwBinary, 2);

		ZeroMemory(lpInBuf, ulInBufSizeCh * sizeof(TCHAR));
		_tcscat_s(lpInBuf, ulInBufSizeCh, _T("A:"));

		*lpInBuf += ulIndex;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

ULONG
	CVolumeDetector::GetIndex(
	__in DWORD dwPower,
	__in ULONG ulBase
	)
{
	ULONG	ulRet		= 0;

	DWORD	dwRemainder = 0;


	__try
	{
		if (!dwPower || (1 == dwPower && 1 == ulBase))
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. %I64d %d", dwPower, ulBase);
			__leave;
		}

		do 
		{
			dwRemainder = dwPower % ulBase;
			dwPower /= ulBase;
			if (dwRemainder)
			{
				if (1 != dwRemainder)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "dwPower error. %I64D %d", dwPower, ulBase);
					__leave;
				}

				break;
			}

			ulRet++;
		} while (TRUE);
	}
	__finally
	{
		;
	}

	return ulRet;
}
