
#pragma once

#include "..\\OperationSystemVersion\\OperationSystemVersion.h"

typedef
BOOL
(WINAPI * WOW64_DISABLE_WOW64_FS_REDIRECTION)(
	__out PVOID * OldValue
	);

typedef
BOOL
(WINAPI * WOW64_REVERT_WOW64_FS_REDIRECTION)(
	__in PVOID OlValue
	);

class CWow64DisableWow64FsRedirection
{
public:
	CWow64DisableWow64FsRedirection();

	~CWow64DisableWow64FsRedirection();

private:
	HMODULE								m_hModule;

	WOW64_DISABLE_WOW64_FS_REDIRECTION	m_pfWow64DisableWow64FsRedirection;
	WOW64_REVERT_WOW64_FS_REDIRECTION	m_pfWow64RevertWow64FsRedirection;
	PVOID								m_pOldValue;
	BOOL								m_bWow64DisableWow64FsRedirection;
};
