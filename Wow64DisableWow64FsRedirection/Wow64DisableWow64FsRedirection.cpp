
#include "Wow64DisableWow64FsRedirection.h"

CWow64DisableWow64FsRedirection::CWow64DisableWow64FsRedirection()
{
	__try
	{
		m_hModule = NULL;
		m_pfWow64DisableWow64FsRedirection = NULL;
		m_pfWow64RevertWow64FsRedirection = NULL;
		m_pOldValue = NULL;

#ifdef _X86_
		if (OS_PROCESSOR_TYPE_X64 != COperationSystemVersion::GetInstance()->GetOSProcessorType())
			__leave;

		m_hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!m_hModule)
			__leave;

		m_pfWow64DisableWow64FsRedirection = (WOW64_DISABLE_WOW64_FS_REDIRECTION)GetProcAddress(m_hModule, "Wow64DisableWow64FsRedirection");
		m_pfWow64RevertWow64FsRedirection = (WOW64_REVERT_WOW64_FS_REDIRECTION)GetProcAddress(m_hModule, "Wow64RevertWow64FsRedirection");
		if (!m_pfWow64DisableWow64FsRedirection || !m_pfWow64RevertWow64FsRedirection)
			__leave;

		m_pfWow64DisableWow64FsRedirection(&m_pOldValue);
#endif
	}
	__finally
	{
		;
	}
}

CWow64DisableWow64FsRedirection::~CWow64DisableWow64FsRedirection()
{
	if (m_hModule)
	{
		if (m_pfWow64RevertWow64FsRedirection && m_pOldValue)
			m_pfWow64RevertWow64FsRedirection(m_pOldValue);

		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}
