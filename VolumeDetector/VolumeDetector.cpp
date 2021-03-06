﻿#include "VolumeDetector.h"

CVolumeDetector	* CVolumeDetector::ms_pInstance = NULL;

BOOL
	CVolumeDetector::Init(
	__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
	)
{
	BOOL bRet = FALSE;

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

	__try
	{
		if (!lpVolumeDetectorInitArguments)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		m_VolumeDetectorInternal.hWindow = lpVolumeDetectorInitArguments->hWindow;
		m_VolumeDetectorInternal.lpfnWndProc = lpVolumeDetectorInitArguments->lpfnWndProc;
		m_VolumeDetectorInternal.bCreateMassageLoop = lpVolumeDetectorInitArguments->bCreateMassageLoop;

		_tcscat_s(m_VolumeDetectorInternal.tchModuleName, _countof(m_VolumeDetectorInternal.tchModuleName), lpVolumeDetectorInitArguments->tchModuleName);

		if (!m_VolumeDetectorInternal.hWindow)
		{
			m_VolumeDetectorInternal.hWindow = CreateWnd(
				_tcslen(m_VolumeDetectorInternal.tchModuleName) ? m_VolumeDetectorInternal.tchModuleName : NULL,
				NULL,
				m_VolumeDetectorInternal.lpfnWndProc
				);
			if (!m_VolumeDetectorInternal.hWindow)
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "CreateWnd failed");
				__leave;
			}
		}

		if (m_VolumeDetectorInternal.bCreateMassageLoop)
		{
			if (!MessageLoop())
			{
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "MessageLoop failed");
				__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (!Unload())
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Unload failed");
		}
	}

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");

	return bRet;
}

BOOL
	CVolumeDetector::Unload()
{
	BOOL bRet = TRUE;

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "begin");

	__try
	{
		if (m_VolumeDetectorInternal.hWindow)
			SendMessage(m_VolumeDetectorInternal.hWindow, WM_CLOSE, 0, 0);
	}
	__finally
	{
		;
	}

	printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "end");

	CPrintfEx::ReleaseInstance();

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

			if (!TranslateMessage(&msg))
				printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "TranslateMessage failed. (%d)", GetLastError());

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
				if (!lParam)
					break;

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
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input arguments error. 0x%x 0x%p %d", dwBinary, lpInBuf, ulInBufSizeCh);
			__leave;
		}

		ulIndex = (ULONG)(log10((double)dwBinary) / log10(2.0) + 0.5);

		ZeroMemory(lpInBuf, ulInBufSizeCh * sizeof(TCHAR));
		_tcscat_s(lpInBuf, ulInBufSizeCh, _T("A:"));

		*lpInBuf += (WCHAR)ulIndex;

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
	TCHAR									tchName[MAX_PATH]					= {0};
	TCHAR									tchCaption[MAX_PATH]				= {0};
	ULONG									i									= 0;
	DWORD									dwVolume							= 0;


	__try
	{
		if (!lpParameter)
		{
			printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		lpWmDevicechangeWorkthreadArguments = (LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS)lpParameter;

		for (; i < 26; i++)
		{
			if (BitTest((const LONG *)&(lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_unitmask), i))
			{
				BitTestAndSet((LONG *)&dwVolume, i);

				if (!CVolumeDetector::GetInstance()->BinaryToVolume(dwVolume, tchName, _countof(tchName)))
				{
					printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "BinaryToVolume failed. 0x%x - 0x%x", lpWmDevicechangeWorkthreadArguments->DevBroadcastVolume.dbcv_unitmask, dwVolume);
					__leave;
				}

				switch (LOWORD(lpWmDevicechangeWorkthreadArguments->wParam) | 0x8000)
				{
				case DBT_DEVICEARRIVAL:
					{
						if (!CStorageDevice::QueryCaption(tchName, tchCaption, _countof(tchCaption)))
						{
							printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "[DBT_DEVINSTENUMERATED] CStorageDevice::QueryCaption failed. %S", tchName);
							__leave;
						}

						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVICEARRIVAL] system detected a new device. %S - %S", tchName, tchCaption);

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
				case DBT_CUSTOMEVENT:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "user-defined event. %S", tchName);
						break;
					}
				case DBT_DEVINSTENUMERATED:
					{
						if (!CStorageDevice::QueryCaption(tchName, tchCaption, _countof(tchCaption)))
						{
							printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "[DBT_DEVINSTENUMERATED] CStorageDevice::QueryCaption failed. %S", tchName);
							__leave;
						}

						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "[DBT_DEVINSTENUMERATED] system detected a new device. %S- %S", tchName, tchCaption);

						break;
					}
				case DBT_DEVINSTSTARTED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device installed and started. %S", tchName);
						break;
					}
				case DBT_DEVINSTREMOVED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "device removed from system. %S", tchName);
						break;
					}
				case DBT_DEVINSTPROPERTYCHANGED:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_INFORMATION, "a property on the device changed. %S", tchName);
						break;
					}
				default:
					{
						printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "wParam error. 0x%x", lpWmDevicechangeWorkthreadArguments->wParam);
						__leave;
					}
				}

				ZeroMemory(tchName, sizeof(tchName));
				ZeroMemory(tchCaption, sizeof(tchCaption));
				dwVolume = 0;
			}
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

CVolumeDetector *
	CVolumeDetector::GetInstance(
	__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
	)
{
	typedef enum _INSTANCE_STATUS
	{
		INSTANCE_STATUS_UNINITED	= 0,
		INSTANCE_STATUS_INITING		= 1,
		INSTANCE_STATUS_INITED		= 2
	} INSTANCE_STATUS, *PINSTANCE_STATUS, *LPINSTANCE_STATUS;

	static LONG ms_lInstanceStatus = INSTANCE_STATUS_UNINITED;



	if (INSTANCE_STATUS_UNINITED == InterlockedCompareExchange(&ms_lInstanceStatus, INSTANCE_STATUS_INITING, INSTANCE_STATUS_UNINITED))
	{
		do 
		{
			new CVolumeDetector(lpVolumeDetectorInitArguments);
			if (!ms_pInstance)
				Sleep(1000);
			else
			{
				InterlockedCompareExchange(&ms_lInstanceStatus, INSTANCE_STATUS_INITED, INSTANCE_STATUS_INITING);
				break;
			}
		} while (TRUE);
	}
	else
	{
		do
		{
			if (INSTANCE_STATUS_INITED != ms_lInstanceStatus)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}

VOID
	CVolumeDetector::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}

CVolumeDetector::CVolumeDetector(
	__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
	)
{
	ms_pInstance = this;

	ZeroMemory(&m_VolumeDetectorInternal, sizeof(m_VolumeDetectorInternal));

	if (!Init(lpVolumeDetectorInitArguments))
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Init failed");
}

CVolumeDetector::~CVolumeDetector()
{
	if (!Unload())
		printfEx(MOD_VOLUME_DETECTOR, PRINTF_LEVEL_ERROR, "Unload failed");

	ZeroMemory(&m_VolumeDetectorInternal, sizeof(m_VolumeDetectorInternal));
}
