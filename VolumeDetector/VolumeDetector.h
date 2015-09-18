#pragma once

#define MOD_VOLUME_DETECTOR _T("¾íÌ½²â")

#include <Windows.h>
#include <Dbt.h>
#include <process.h>
#include <math.h>

#include "..\\PrintfEx\\PrintfEx.h"
#include "..\\StorageDevice\\StorageDevice.h"

#define WND_CLASS_NAME _T("DeviceDetectorWindowClass")

LRESULT
	CALLBACK
	WndProc(
	HWND	hWnd,
	UINT	message,
	WPARAM	wParam,
	LPARAM	lParam
	);

typedef struct _VOLUME_DETECTOR_INIT_ARGUMENTS
{
	TCHAR	tchModuleName[MAX_PATH];
	HWND	hWindow;
	WNDPROC	lpfnWndProc;
	BOOL	bCreateMassageLoop;
} VOLUME_DETECTOR_INIT_ARGUMENTS, *PVOLUME_DETECTOR_INIT_ARGUMENTS, *LPVOLUME_DETECTOR_INIT_ARGUMENTS;

typedef struct _VOLUME_DETECTOR_INTERNAL
{
	HWND	hWindow;
	WNDPROC	lpfnWndProc;
	BOOL	bCreateMassageLoop;
} VOLUME_DETECTOR_INTERNAL, *PVOLUME_DETECTOR_INTERNAL, *LPVOLUME_DETECTOR_INTERNAL;

typedef struct _WM_DEVICECHANGE_WORKTHREAD_ARGUMENTS
{
	DEV_BROADCAST_VOLUME	DevBroadcastVolume;
	WPARAM					wParam;
} WM_DEVICECHANGE_WORKTHREAD_ARGUMENTS, *PWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS, *LPWM_DEVICECHANGE_WORKTHREAD_ARGUMENTS;

class CVolumeDetector
{
public:
	BOOL
		Init(
		__in LPVOLUME_DETECTOR_INIT_ARGUMENTS lpVolumeDetectorInitArguments
		);

	BOOL
		Unload();

	BOOL
		MessageLoop();

	static
		unsigned int
		__stdcall
		WmDeviceChangeWorkThread(
		__in void * lpParameter
		);

private:
	static VOLUME_DETECTOR_INTERNAL ms_VolumeDetectorInternal;

	HWND
		CreateWnd(
		__in_opt LPTSTR		lpModuleName,
		__in_opt HINSTANCE	hPrevInstance,
		__in_opt WNDPROC	lpfnWndProc
		);

	static
		BOOL
		BinaryToVolume(
		__in	DWORD	dwBinary,
		__out	LPTSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh
		);
};
