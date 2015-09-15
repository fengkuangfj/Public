#include "VolumeDetector.h"

VOLUME_DETECTOR_INTERNAL CVolumeDetector::ms_VolumeDetectorInternal = {0};

BOOL
	CVolumeDetector::Init(
	__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
	)
{
	BOOL	bRet = FALSE;

	CWmi	Wmi;

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

	__try
	{
		if (!lpVolumeDetectorInitArguments)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		if (!Wmi.Init())
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Wmi.Init failed");
			__leave;
		}

		if (!ms_VolumeDetectorInternal.hWindow)
		{
			ms_VolumeDetectorInternal.hWindow = lpVolumeDetectorInitArguments->hWindow;
			ms_VolumeDetectorInternal.lpfnWndProc = lpVolumeDetectorInitArguments->lpfnWndProc;
			ms_VolumeDetectorInternal.bCreateMassageLoop = lpVolumeDetectorInitArguments->bCreateMassageLoop;

			if (!ms_VolumeDetectorInternal.hWindow)
			{
				ms_VolumeDetectorInternal.hWindow = CreateWnd(
					_tcslen(lpVolumeDetectorInitArguments->tchModuleName) ? lpVolumeDetectorInitArguments->tchModuleName : NULL,
					NULL,
					ms_VolumeDetectorInternal.lpfnWndProc
					);
				if (!ms_VolumeDetectorInternal.hWindow)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "CreateWnd failed");
					__leave;
				}
			}

			if (ms_VolumeDetectorInternal.bCreateMassageLoop)
			{
				if (!MessageLoop())
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "MessageLoop failed");
					__leave;
				}
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Wmi.Unload())
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Wmi.Unload failed");
		}
	}

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");

	return bRet;
}

BOOL
	CVolumeDetector::Unload()
{
	BOOL	bRet = FALSE;

	CWmi	Wmi;

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

	__try
	{
		if (ms_VolumeDetectorInternal.hWindow)
			SendMessage(ms_VolumeDetectorInternal.hWindow, WM_CLOSE, 0, 0);

		if (!Wmi.Unload())
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Wmi.Unload failed");

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");

	return bRet;
}

BOOL
	CVolumeDetector::MessageLoop()
{
	BOOL	bRet	= FALSE;

	MSG		msg		= {0};

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

	__try
	{
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
		;
	}

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");

	return bRet;
}

HWND
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
		{
			if (hRet)
			{
				SendMessage(hRet, WM_CLOSE, 0, 0);
				hRet = NULL;
			}
		}
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
	LRESULT									lRet								= 1;

	HANDLE									hThread								= NULL;
	LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS	lpWmDevicechangeWorkthreadArguments = NULL;
	BOOL									bResult								= FALSE;


	__try
	{
		switch (message)
		{
		case WM_CREATE:
			break;
		case WM_DEVICECHANGE:
			{
				lpWmDevicechangeWorkthreadArguments = (LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS)calloc(1, sizeof(WM_DEVICECHANGE_WORKTHREAD_ARGUMENTS));
				if (!lpWmDevicechangeWorkthreadArguments)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "calloc failed. (%d)", GetLastError());
					__leave;
				}

				lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_size = ((PDEV_BROADCAST_VOLUME)lParam)->dbcv_size;
				lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_devicetype = ((PDEV_BROADCAST_VOLUME)lParam)->dbcv_devicetype;
				lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_reserved = ((PDEV_BROADCAST_VOLUME)lParam)->dbcv_reserved;
				lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_unitmask = ((PDEV_BROADCAST_VOLUME)lParam)->dbcv_unitmask;
				lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_flags = ((PDEV_BROADCAST_VOLUME)lParam)->dbcv_flags;
				lpWmDevicechangeWorkthreadArguments->wParam = wParam;

				hThread = (HANDLE)_beginthreadex(NULL, 0, CVolumeDetector::WmDeviceChangeWorkThread, lpWmDevicechangeWorkthreadArguments, 0, NULL);
				if (!hThread)
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "_beginthreadex failed. (%d)", GetLastError());
					__leave;
				}

				bResult = TRUE;
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
		if (hThread)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}

		if (!bResult)
		{
			if (lpWmDevicechangeWorkthreadArguments)
			{
				free(lpWmDevicechangeWorkthreadArguments);
				lpWmDevicechangeWorkthreadArguments = NULL;
			}
		}
	}

	return lRet;
}

BOOL
	CVolumeDetector::BinaryToVolumeInternal(
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
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08x 0x%08p %d", dwBinary, lpInBuf, ulInBufSizeCh);
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
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08x %d", dwPower, ulBase);
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
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "dwPower error. 0x%08x %d", dwPower, ulBase);
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

BOOL
	CVolumeDetector::BinaryToVolume(
	__in	DWORD	dwBinary,
	__out	LPTSTR	lpInBuf,
	__inout	PULONG	ulCount,
	__in	ULONG	ulPerSizeCh
	)
{
	BOOL	bRet	= FALSE;

	ULONG	i		= 25;
	ULONG	j		= 0;


	__try
	{
		if (!dwBinary || !lpInBuf || !ulCount || !ulPerSizeCh)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08x 0x%08p 0x%08p %d", dwBinary, lpInBuf, ulCount, ulPerSizeCh);
			__leave;
		}

		for (; i > 0; i--)
		{
			if (1 == dwBinary >> i)
			{
				if (!BinaryToVolumeInternal(1 << i, lpInBuf + (j++) * ulPerSizeCh, ulPerSizeCh))
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "BinaryToVolumeInternal failed. 0x%08x - 0x%08x", dwBinary, 1 << i);
					break;
				}

				dwBinary &= ~(1 << i);
			}
		}

		*ulCount = j;

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

unsigned int
	__stdcall
	CVolumeDetector::WmDeviceChangeWorkThread(
	__in void * lpParameter
	)
{
	LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS	lpWmDevicechangeWorkthreadArguments	= NULL;
	TCHAR									tchName[26][MAX_PATH]				= {0};
	TCHAR									tchCaption[MAX_PATH]				= {0};
	ULONG									ulCount								= 0;
	ULONG									i									= 0;


	__try
	{
		if (!lpParameter)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		lpWmDevicechangeWorkthreadArguments = (LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS)lpParameter;

		ulCount = 26;
		if (!CVolumeDetector::BinaryToVolume(lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_unitmask, (LPTSTR)tchName, &ulCount, MAX_PATH))
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "BinaryToVolume failed. 0x%08x", lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_unitmask);
			__leave;
		}

		for (; i < ulCount; i++)
		{
			switch (LOWORD(lpWmDevicechangeWorkthreadArguments->wParam) | 0x8000)
			{
			case DBT_DEVICEARRIVAL:
				{
					if (!CStorageDevice::QueryCaption(tchName[i], tchCaption, _countof(tchCaption)))
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVINSTENUMERATED] CStorageDevice::QueryCaption failed");
						__leave;
					}

					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVICEARRIVAL] system detected a new device. %S - %S", tchName[i], tchCaption);

					break;
				}
			case DBT_DEVICEQUERYREMOVE:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "wants to remove, may fail. %S", tchName[i]);
					break;
				}
			case DBT_DEVICEQUERYREMOVEFAILED:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "removal aborted. %S", tchName[i]);
					break;
				}
			case DBT_DEVICEREMOVEPENDING:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "about to remove, still avail. %S", tchName[i]);
					break;
				}
			case DBT_DEVICEREMOVECOMPLETE:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device is gone. %S", tchName[i]);
					break;
				}
			case DBT_DEVICETYPESPECIFIC:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "type specific event. %S", tchName[i]);
					break;
				}
			case DBT_CUSTOMEVENT:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "user-defined event. %S", tchName[i]);
					break;
				}
			case DBT_DEVINSTENUMERATED:
				{
					if (!CStorageDevice::QueryCaption(tchName[i], tchCaption, _countof(tchCaption)))
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVINSTENUMERATED] CStorageDevice::QueryCaption failed");
						__leave;
					}

					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVINSTENUMERATED] system detected a new device. %S- %S", tchName[i], tchCaption);

					break;
				}
			case DBT_DEVINSTSTARTED:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device installed and started. %S", tchName[i]);
					break;
				}
			case DBT_DEVINSTREMOVED:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device removed from system. %S", tchName[i]);
					break;
				}
			case DBT_DEVINSTPROPERTYCHANGED:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "a property on the device changed. %S", tchName[i]);
					break;
				}
			default:
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "wParam error. 0x%08x", lpWmDevicechangeWorkthreadArguments->wParam);
					__leave;
				}
			}

			ZeroMemory(tchCaption, sizeof(tchCaption));
		}
	}
	__finally
	{
		if (lpWmDevicechangeWorkthreadArguments)
		{
			free(lpWmDevicechangeWorkthreadArguments);
			lpWmDevicechangeWorkthreadArguments = NULL;
		}
	}

	return 0;
}
