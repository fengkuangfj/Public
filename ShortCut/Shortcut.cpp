#include "Shortcut.h"

BOOL
	CShortCut::Create(
	__in LPTSTR lpPath,
	__in LPTSTR lpName
	)
{
	BOOL				bRet					= FALSE;

	HRESULT				hResult					= E_UNEXPECTED;
	LPITEMIDLIST		lpItemIdList			= NULL;
	TCHAR				tchLnkPath[MAX_PATH]	= {0};
	IShellLink		*	pIShellLink				= NULL;
	IPersistFile	*	pIPersistFile			= NULL;
	BOOL				bNeedCoUninitialize		= FALSE;


	__try
	{
		if (!lpPath || !lpName)
		{
			printfPublic("input parameter error. 0x%p 0x%p", lpPath, lpName);
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
			printfPublic("SHGetFolderLocation failed. (0x%x)", hResult);
			__leave;
		}

		if (!SHGetPathFromIDList(lpItemIdList, tchLnkPath))
		{
			printfPublic("SHGetPathFromIDList failed. (%d)", GetLastError());
			__leave;
		}

		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T("\\"));
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), lpName);
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T(".lnk"));

		hResult = CoInitialize(NULL);
		if (FAILED(hResult))
		{
			printfPublic("CoInitialize failed. (0x%x)", hResult);
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
			printfPublic("CoCreateInstance failed. (0x%x)", hResult);
			__leave;
		}

		hResult = pIShellLink->QueryInterface(
			IID_IPersistFile,
			(void **)&pIPersistFile
			);
		if (FAILED(hResult))
		{
			printfPublic("QueryInterface failed. (0x%x)", hResult);
			__leave;
		}

		hResult = pIShellLink->SetPath(lpPath);
		if (FAILED(hResult))
		{
			printfPublic("SetPath failed. %S (0x%x)", lpPath, hResult);
			__leave;
		}

		hResult = pIShellLink->SetShowCmd(SW_SHOWNORMAL);
		if (FAILED(hResult))
		{
			printfPublic("SetShowCmd failed. (0x%x)", hResult);
			__leave;
		}

		hResult = pIShellLink->Resolve(
			NULL,
			SLR_UPDATE
			);
		if (FAILED(hResult))
		{
			printfPublic("Resolve failed. (0x%x)", hResult);
			__leave;
		}

		hResult = pIPersistFile->Save(tchLnkPath, TRUE);
		if (FAILED(hResult))
		{
			printfPublic("Save failed. %S (0x%x)", tchLnkPath, hResult);
			__leave;
		}

		printfPublic("%S -> %S", lpPath, tchLnkPath);

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

CShortCut::CShortCut()
{
	;
}

CShortCut::~CShortCut()
{
	;
}
