#pragma once

#include <Windows.h>
#include <Shlobj.h>



#define MOD_SHORTCUT _T("¿ì½Ý·½Ê½")

class CShortCut
{
public:
	BOOL
		Create(__in LPTSTR lpPath, __in LPTSTR lpName);
};
