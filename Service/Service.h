#pragma once

#include <Windows.h>

#include "../PrintfEx/PrintfEx.h"

#define MOD_SERVICE _T("·þÎñ")

class CService
{
public:
	BOOL
		Install(
		__in LPWSTR lpServiceName,
		__in LPWSTR lpPath
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
