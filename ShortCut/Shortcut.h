#pragma once

#include <Windows.h>
#include <Shlobj.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <Objbase.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Ole32.lib")

#include "..\\Public.h"

#ifndef MOD_SHORTCUT
#define MOD_SHORTCUT _T("¿ì½Ý·½Ê½")
#endif

class CShortCut
{
public:
	static
		BOOL
		Create(
		__in LPTSTR	lpPath,
		__in LPTSTR	lpLnkPath
		);

	static
		BOOL
		Delete(
		__in LPTSTR lpLnkPath
		);

private:
	CShortCut();

	~CShortCut();
};
