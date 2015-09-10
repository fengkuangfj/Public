﻿#include "VolumeDetector.h"

VOLUME_DETECTOR_INTERNAL CVolumeDetector::ms_VolumeDetectorInternal = {0};

BOOL
	CVolumeDetector::Init(
	__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
	)
{
	BOOL							bRet				= FALSE;

	DEV_BROADCAST_DEVICEINTERFACE	NotificationFilter	= {0};

	CVolumeDetector					VolumeDetector;


	__try
	{
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

		if (!ms_VolumeDetectorInternal.bService && !ms_VolumeDetectorInternal.Window.hWindow)
		{
			ms_VolumeDetectorInternal.bService = lpVolumeDetectorInitArguments->bService;
			if (ms_VolumeDetectorInternal.bService)
			{
				ms_VolumeDetectorInternal.Service.hService = lpVolumeDetectorInitArguments->Service.hService;
				if (!ms_VolumeDetectorInternal.Service.hService)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
					__leave;
				}

				NotificationFilter.dbcc_size = sizeof(NotificationFilter);
				NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
				NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_VOLUME;

				ms_VolumeDetectorInternal.Service.hDevNotify = RegisterDeviceNotification(ms_VolumeDetectorInternal.Service.hService, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);
				if (!ms_VolumeDetectorInternal.Service.hDevNotify)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "RegisterDeviceNotification failed. (%d)", GetLastError());
					__leave;
				}
			}
			else
			{
				ms_VolumeDetectorInternal.Window.hWindow = lpVolumeDetectorInitArguments->Window.hWindow;
				ms_VolumeDetectorInternal.Window.lpfnWndProc = lpVolumeDetectorInitArguments->Window.lpfnWndProc;
				ms_VolumeDetectorInternal.Window.bCreateMassageLoop = lpVolumeDetectorInitArguments->Window.bCreateMassageLoop;

				if (!ms_VolumeDetectorInternal.Window.hWindow)
				{
					ms_VolumeDetectorInternal.Window.hWindow = CreateWnd(
						_tcslen(lpVolumeDetectorInitArguments->tchModuleName) ? lpVolumeDetectorInitArguments->tchModuleName : NULL,
						NULL,
						ms_VolumeDetectorInternal.Window.lpfnWndProc
						);
					if (!ms_VolumeDetectorInternal.Window.hWindow)
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "CreateWnd failed");
						__leave;
					}
				}

				if (ms_VolumeDetectorInternal.Window.bCreateMassageLoop)
				{
					if (!VolumeDetector.MessageLoop())
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

		if (ms_VolumeDetectorInternal.bService)
		{
			if (ms_VolumeDetectorInternal.Service.hDevNotify)
			{
				if (!UnregisterDeviceNotification(ms_VolumeDetectorInternal.Service.hDevNotify))
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "UnregisterDeviceNotification failed. (%d)", GetLastError());

				ms_VolumeDetectorInternal.Service.hDevNotify = NULL;
			}
		}
		else
		{
			if (ms_VolumeDetectorInternal.Window.hWindow)
				SendMessage((HWND)ms_VolumeDetectorInternal.Window.hWindow, WM_CLOSE, 0, 0);
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
	CVolumeDetector::MessageLoop()
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
	__in_opt HINSTANCE	hPrevInstance,
	__in_opt WNDPROC	lpfnWndProc
	)
{
	HWND		hRet		= NULL;

	HINSTANCE	hInstance	= NULL;
	WNDCLASS	WndClass	= {0};
	BOOL		bResult		= FALSE;


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
			WndClass.lpfnWndProc = lpfnWndProc ? lpfnWndProc : WndProc;
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
		
		if (!UpdateWindow(hRet))
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "UpdateWindow failed. (%d)", GetLastError());
			__leave;
		}

		bResult = TRUE;
	}
	__finally
	{
		if (!bResult)
			hRet = NULL;
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
