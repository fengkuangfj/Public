#pragma once

#include <Windows.h>
#include <Shlobj.h>



#define MOD_SHORTCUT _T("��ݷ�ʽ")

class CShortCut
{
public:
	BOOL
		Create(__in LPTSTR lpPath, __in LPTSTR lpName);
};
