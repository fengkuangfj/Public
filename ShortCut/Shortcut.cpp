#include "stdafx.h"
#include "Shortcut.h"

BOOL
	CShortCut::Create(__in LPTSTR lpPath, __in LPTSTR lpName)
{
	BOOL				bRet					= FALSE;

	HRESULT				hResult					= S_FALSE;
	LPITEMIDLIST		lpItemIdList			= NULL;
	TCHAR				tchLnkPath[MAX_PATH]	= {0};
	IShellLink		*	pIShellLink				= NULL;
	IPersistFile	*	pIPersistFile			= NULL;
	

	__try
	{
		if (!lpPath || !lpName)
		{
			printf("input parameter error. 0x%08p 0x%08p \n",
				lpPath, lpName);

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
			printf("SHGetFolderLocation failed. (%d) \n",
				hResult);

			__leave;
		}

		if (!SHGetPathFromIDList(lpItemIdList, tchLnkPath))
		{
			printf("SHGetPathFromIDList failed. (%d) \n",
				GetLastError());

			__leave;
		}

		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T("\\"));
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), lpName);
		_tcscat_s(tchLnkPath, _countof(tchLnkPath), _T(".lnk"));

		// CoInitialize(NULL);

		hResult = CoCreateInstance(
			CLSID_ShellLink,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IShellLink,
			(LPVOID *)&pIShellLink
			);
		if (FAILED(hResult))
		{
			printf("CoCreateInstance failed. (%d) \n",
				hResult);

			__leave;
		}

		hResult = pIShellLink->QueryInterface(
			IID_IPersistFile,
			(void **)&pIPersistFile
			);
		if (FAILED(hResult))
		{
			printf("QueryInterface failed. (%d) \n",
				hResult);

			__leave;
		}

		hResult = pIShellLink->SetPath(lpPath);
		if (FAILED(hResult))
		{
			printf("SetPath failed. %S (%d) \n",
				lpPath, hResult);

			__leave;
		}

		hResult = pIShellLink->SetShowCmd(SW_SHOWNORMAL);
		if (FAILED(hResult))
		{
			printf("SetShowCmd failed. (%d) \n",
				hResult);

			__leave;
		}

		hResult = pIShellLink->Resolve(
			NULL,
			SLR_UPDATE
			);
		if (FAILED(hResult))
		{
			printf("Resolve failed. (%d) \n",
				hResult);

			__leave;
		}

		hResult = pIPersistFile->Save(tchLnkPath, TRUE);
		if (FAILED(hResult))
		{
			printf("Save failed. %S (%d) \n",
				tchLnkPath, hResult);

			__leave;
		}

		// CoUninitialize();

		bRet = TRUE;
	}
	__finally
	{
		if (pIShellLink)
		{
			pIShellLink->Release();
			pIShellLink = NULL;
		}
	}

	return bRet;
}