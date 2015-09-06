#pragma once

#include <Windows.h>

#include "..\\PrintfEx\\PrintfEx.h"

#define MOD_SERVICE _T("·þÎñ")

typedef
	BOOL
	(* WOW64_DISABLE_WOW64_FS_REDIRECTION)(
	__out PVOID * OldValue
	);

typedef
	BOOL
	(* WOW64_REVERT_WOW64_FS_REDIRECTION)(
	__in PVOID OlValue
	);

class CService
{
public:
	BOOL
		Install(
		__in		LPWSTR	lpServiceName,
		__in		DWORD	dwServiceType,
		__in		DWORD	dwStartType,
		__in_opt	DWORD	dwErrorControl,
		__in		LPWSTR	lpPath,
		__in_opt	LPWSTR	lpLoadOrderGroup,
		__in_opt	LPWSTR	lpDependencies
		);

	BOOL
		Start(
		__in LPWSTR lpServiceName
		);

	BOOL
		Stop(
		__in LPWSTR lpServiceName
		);

	BOOL
		Delete(
		__in LPWSTR lpServiceName
		);

	BOOL
		Disable(
		__in LPWSTR lpServiceName
		);
};
