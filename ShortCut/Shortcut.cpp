#include "Shortcut.h"

BOOL
	CShortCut::Create(
	__in LPTSTR lpPath,
	__in LPTSTR lpName
	)
{
	BOOL				bRet					= FALSE;

	HRESULT				hResult					= S_FALSE;
	LPITEMIDLIST		lpItemIdList			= NULL;
	TCHAR				tchLnkPath[MAX_PATH]	= {0};
	IShellLink		*	pIShellLink				= NULL;
	IPersistFile	*	pIPersistFile			= NULL;
	BOOL				bNeedCoUninitialize		= FALSE;


	__try
	{
		if (!lpPath || !lpName)
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "input parameter error. 0x%08p 0x%08p", lpPath, lpName);
			__leave;
		}

		hResult = SHGetFolderLocation(
			NULL,
			CSIDL_DESKTOP, // CSIDL_COMMON_DESKTOPDIRECTORY CSIDL_DESKTOPDIRECTORY
			NULL,
			0,
			&lpItemIdList
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "SHGetFolderLocation failed. (0x%08x)", hResult);
			__leave;
		}

		if (!SHGetPathFromIDList(lpItemIdList, tchLnkPath))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "SHGetPathFromIDList failed. (%d)", GetLastError());
			__leave;
		}

		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T("\\"));
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), lpName);
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T(".lnk"));

		hResult = CoInitialize(NULL);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "CoInitialize failed. (0x%08x)", hResult);
			__leave;
		}

		bNeedCoUninitialize = TRUE;

		hResult = CoCreateInstance(
			CLSID_ShellLink,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IShellLink,
			(LPVOID *)&pIShellLink
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "CoCreateInstance failed. (0x%08x)", hResult);
			__leave;
		}

		hResult = pIShellLink->QueryInterface(
			IID_IPersistFile,
			(void **)&pIPersistFile
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "QueryInterface failed. (0x%08x)", hResult);
			__leave;
		}

		hResult = pIShellLink->SetPath(lpPath);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "SetPath failed. %S (0x%08x)", lpPath, hResult);
			__leave;
		}

		hResult = pIShellLink->SetShowCmd(SW_SHOWNORMAL);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "SetShowCmd failed. (0x%08x)", hResult);
			__leave;
		}

		hResult = pIShellLink->Resolve(
			NULL,
			SLR_UPDATE
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "Resolve failed. (0x%08x)", hResult);
			__leave;
		}

		hResult = pIPersistFile->Save(tchLnkPath, TRUE);
		if (FAILED(hResult))
		{
			printfEx(MOD_SHORTCUT, PRINTF_LEVEL_ERROR, "Save failed. %S (0x%08x)", tchLnkPath, hResult);
			__leave;
		}

		printfEx(MOD_SHORTCUT, PRINTF_LEVEL_INFORMATION, "%S -> %S", lpPath, tchLnkPath);

		bRet = TRUE;
	}
	__finally
	{
		if (pIShellLink)
		{
			pIShellLink->Release();
			pIShellLink = NULL;
		}

		if (bNeedCoUninitialize)
			CoUninitialize();
	}

	return bRet;
}
