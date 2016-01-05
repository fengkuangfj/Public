#pragma once

#ifndef MOD_WMI
#define MOD_WMI _T("WMI")
#endif

#include <Windows.h>
#include <comdef.h>
#include <WbemIdl.h>

#include "..\\PrintfEx\\PrintfEx.h"

#pragma comment(lib, "Wbemuuid.lib")

class CWmi
{
public:
	static
		CWmi *
		GetInstance();

	static
		VOID
		ReleaseInstance();

	BOOL
		Init();

	BOOL
		Unload();

	static
		BOOL
		Query(
		__in LPSTR	lpClass,
		__in LPTSTR lpContent
		);

	BOOL
		QueryCaption(
		__in	ULONG	ulDiskNumber,
		__out	LPTSTR	lpOutBuf,
		__in	ULONG	ulOutBufSizeCh
		);

private:
	static CWmi		*	ms_pInstance;

	IWbemLocator	*	m_pIWbemLocator;
	IWbemServices	*	m_pIWbemServices;
	BOOL				m_bNeedCoUnInit;

	CWmi();

	~CWmi();
};
