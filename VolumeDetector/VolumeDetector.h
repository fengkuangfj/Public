#pragma once

#define MOD_VOLUME_DETECTOR _T("¾íÌ½²â")

#include <Windows.h>
#include <Dbt.h>
#include <process.h>

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
		__in		BOOL	bService,
		__in		BOOL	bCreateMassageLoop,
		__in		BOOL	bCreateMassageLoopThread
		);

	BOOL
		Unload();

	BOOL
		MessageLoop(
		__in BOOL bCreateThread
		);

	static
		BOOL
		BinaryToVolume(
		__in	DWORD	dwBinary,
		__out	LPTSTR	lpInBuf,
		__in	ULONG	ulInBufSizeCh
		);

private:
	static HDEVNOTIFY ms_hDevNotify;

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

	static
		BOOL
		MessageLoopInternal();

	static
		unsigned int
		__stdcall
		MessageLoopWorkThread(
		__in void * lpParameter
		);
};
