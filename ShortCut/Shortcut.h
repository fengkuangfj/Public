#pragma once

#define MOD_SHORTCUT _T("��ݷ�ʽ")

#include <Windows.h>
#include <Shlobj.h>
#include <tchar.h>

#include "..\\Public.h"

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
