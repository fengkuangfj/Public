#pragma once

#include <Windows.h>
#include <Shlobj.h>
#include <tchar.h>

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
		__in LPTSTR lpPath,
		__in LPTSTR lpName
		);

private:
	CShortCut();

	~CShortCut();
};
