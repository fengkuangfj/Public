#pragma once

#include <Windows.h>
#include <Shlobj.h>
#include <tchar.h>

#include "..\\..\\Public\\PrintfEx\\PrintfEx.h"

#define MOD_SHORTCUT _T("¿ì½Ý·½Ê½")

class CShortCut
{
public:
	static
		BOOL
		Create(
		__in LPTSTR lpPath,
		__in LPTSTR lpName
		);
};
