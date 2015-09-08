#pragma once

#define MOD_DEVICE_MONITOR _T("…Ë±∏ÃΩ≤‚")

#include <Windows.h>
#include <Dbt.h>

#include "..\\PrintfEx\\PrintfEx.h"

#define WND_CLASS_NAME _T("DeviceDetectorWindowClass")

LRESULT
	CALLBACK
	WndProc(
	HWND	hWnd,
	UINT	message,
	WPARAM	wParam,
	LPARAM	lParam
	);

class CVolumeDetector
{
public:
	static HANDLE	ms_hWindowOrService;
	static BOOL		ms_bService;

	BOOL
		Init(
		__in_opt	LPTSTR	lpModuleName,
		__in		HANDLE	hWindowOrService,
		__in		BOOL	bService
		);

	BOOL
		Unload();

	BOOL
		MessageLoop();

	static
		BOOL
		BinaryToVolume(
		__in	DWORD	dwBinary,
		__out	LPTSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh
		);

private:
	HANDLE
		CreateWnd(
		__in_opt LPTSTR		lpModuleName,
		__in_opt HINSTANCE	hPrevInstance
		);

	static
		ULONG
		GetIndex(
		__in DWORD dwPower,
		__in ULONG ulBase
		);
};
