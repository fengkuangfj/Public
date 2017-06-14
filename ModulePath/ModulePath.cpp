#include "ModulePath.h"

BOOL
	CModulePath::Get(
	__in_opt	HMODULE	hModule,
	__out		LPTSTR	lpOutBuf,
	__in		ULONG	ulOutBufSizeCh
	)
{
	BOOL	bRet		= FALSE;

	DWORD	dwResult	= 0;


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh)
		{
			printfPublic("input arguments error. 0x%p %d", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(TCHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfPublic("GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileName(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfPublic("GetModuleFileName failed. (%d)", GetLastError());
			__leave;
		}

		dwResult = GetLongPathName(lpOutBuf, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfPublic("GetLongPathName failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CModulePath::Get(
				 __in_opt	HMODULE	hModule,
				 __out		LPSTR	lpOutBuf,
				 __in		ULONG	ulOutBufSizeCh
				 )
{
	BOOL	bRet		= FALSE;

	DWORD	dwResult	= 0;


	__try
	{
		if (!lpOutBuf || !ulOutBufSizeCh)
		{
			printfPublic("input arguments error. 0x%p %d", lpOutBuf, ulOutBufSizeCh);
			__leave;
		}

		ZeroMemory(lpOutBuf, ulOutBufSizeCh * sizeof(CHAR));

		if (!hModule)
		{
			hModule = GetModuleHandle(NULL);
			if (!hModule)
			{
				printfPublic("GetModuleHandle failed. (%d)", GetLastError());
				__leave;
			}
		}

		dwResult = GetModuleFileNameA(hModule, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfPublic("GetModuleFileName failed. (%d)", GetLastError());
			__leave;
		}

		dwResult = GetLongPathNameA(lpOutBuf, lpOutBuf, ulOutBufSizeCh);
		if (!dwResult)
		{
			printfPublic("GetLongPathName failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

CModulePath::CModulePath()
{
	;
}

CModulePath::~CModulePath()
{
	;
}

BOOL
CModulePath::Enum()
{
	BOOL		bRet = FALSE;

	HANDLE		hProcess = 0;
	HMODULE  *	phModule = NULL;
	DWORD		dwSizeB = 0;
	ULONG		ulIndex = 0;
	HMODULE  	hModule = NULL;
	TCHAR		tchPath[MAX_PATH] = { 0 };
	LPTSTR		lpName = NULL;


	__try
	{
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
		if (!hProcess)
		{
			printfPublic("OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		if (!EnumProcessModules(
			hProcess,
			phModule,
			0,
			&dwSizeB
		))
		{
			printfPublic("EnumProcessModules failed. (%d)", GetLastError());
			__leave;
		}

		if (!dwSizeB)
		{
			printfPublic("dwSizeB error");
			__leave;
		}

		phModule = (HMODULE *)calloc(1, dwSizeB);
		if (!phModule)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		if (!EnumProcessModules(
			hProcess,
			phModule,
			dwSizeB,
			&dwSizeB
		))
		{
			printfPublic("EnumProcessModules failed. (%d)", GetLastError());
			__leave;
		}

		for (; ulIndex < dwSizeB / sizeof(HMODULE); ulIndex++)
		{
			hModule = phModule[ulIndex];
			if (!CModulePath::Get(hModule, tchPath, _countof(tchPath)))
			{
				printfPublic("CModulePath::Get failed");
				__leave;
			}

			lpName = PathFindFileName(tchPath);
			if (lpName)
				printfPublic("[0x%x] %S", hModule, lpName);
			else
				printfPublic("[0x%x]", hModule);
		}

		bRet = TRUE;
	}
	__finally
	{
		if (phModule)
		{
			free(phModule);
			phModule = NULL;
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

	return bRet;
}

BOOL
CModulePath::GetName(
	__in	DWORD_PTR	dwAddr,
	__out	LPTSTR		lpModuleName,
	__in	ULONG		ulModuleNameBufSizeCh
)
{
	BOOL		bRet = FALSE;

	HANDLE		hProcess = 0;
	HMODULE  *	phModule = NULL;
	DWORD		dwSizeB = 0;
	ULONG		ulIndex = 0;
	HMODULE  	hModuleCurrent = NULL;
	TCHAR		tchPath[MAX_PATH] = { 0 };
	LPTSTR		lpName = NULL;
	HMODULE		hModulePre = NULL;
	ULONG		ulIndexRet = 0;


	__try
	{
		if (!dwAddr || !lpModuleName || !ulModuleNameBufSizeCh)
		{
			printfPublic("input arguments error. dwAddr(0x%p) lpModuleName(0x%p) ulModuleNameBufSizeCh(%d)",
				dwAddr, lpModuleName, ulModuleNameBufSizeCh);

			__leave;
		}

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
		if (!hProcess)
		{
			printfPublic("OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		if (!EnumProcessModules(
			hProcess,
			phModule,
			0,
			&dwSizeB
		))
		{
			printfPublic("EnumProcessModules failed. (%d)", GetLastError());
			__leave;
		}

		if (!dwSizeB)
		{
			printfPublic("dwSizeB error");
			__leave;
		}

		phModule = (HMODULE *)calloc(1, dwSizeB);
		if (!phModule)
		{
			printfPublic("calloc failed. (%d)", GetLastError());
			__leave;
		}

		if (!EnumProcessModules(
			hProcess,
			phModule,
			dwSizeB,
			&dwSizeB
		))
		{
			printfPublic("EnumProcessModules failed. (%d)", GetLastError());
			__leave;
		}

		for (; ulIndex < dwSizeB / sizeof(HMODULE); ulIndex++)
		{
			hModuleCurrent = phModule[ulIndex];

			if (hModuleCurrent <= (HMODULE)dwAddr)
			{
				if (!hModulePre)
				{
					hModulePre = hModuleCurrent;
					ulIndexRet = ulIndex;
				}
				else
				{
					if (hModulePre <= hModuleCurrent)
					{
						hModulePre = hModuleCurrent;
						ulIndexRet = ulIndex;
					}
				}
			}
		}

		if (!CModulePath::Get(phModule[ulIndexRet], tchPath, _countof(tchPath)))
		{
			printfPublic("CModulePath::Get failed");
			__leave;
		}

		lpName = PathFindFileName(tchPath);
		if (!lpName)
		{
			printfPublic("PathFindFileName failed. (%d)", GetLastError());
			__leave;
		}

		_tcscpy_s(lpModuleName, ulModuleNameBufSizeCh, lpName);

		bRet = TRUE;
	}
	__finally
	{
		if (phModule)
		{
			free(phModule);
			phModule = NULL;
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

	return bRet;
}
