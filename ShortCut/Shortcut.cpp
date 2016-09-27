#include "Shortcut.h"

BOOL
	CShortCut::Create(
	__in LPTSTR	lpPath,
	__in LPTSTR	lpLnkPath
	)
{
	BOOL				bRet					= FALSE;

	HRESULT				hResult					= E_UNEXPECTED;
	IShellLink		*	pIShellLink				= NULL;
	IPersistFile	*	pIPersistFile			= NULL;
	BOOL				bNeedCoUninitialize		= FALSE;


	__try
	{
		if (!lpPath || !lpLnkPath)
		{
			printfPublic("input parameter error. lpPath(0x%p) lpLnkPath(0x%p)", lpPath, lpLnkPath);
			__leave;
		}

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
			SLR_UPDATE | SLR_NO_UI
			);
		if (FAILED(hResult))
		{
			printfPublic("Resolve failed. (0x%x)", hResult);
			__leave;
		}

		hResult = pIPersistFile->Save(lpLnkPath, TRUE);
		if (FAILED(hResult))
		{
			printfPublic("Save failed. %S (0x%x)", lpLnkPath, hResult);
			__leave;
		}

		printfPublic("%S -> %S", lpPath, lpLnkPath);

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

BOOL
	CShortCut::Delete(
	__in LPTSTR lpLnkPath
	)
{
	BOOL bRet = FALSE;


	__try
	{
		if (!lpLnkPath)
		{
			printfPublic("input argument error");
			__leave;
		}

		if (PathFileExists(lpLnkPath))
		{
			if (!DeleteFile(lpLnkPath))
			{
				printfPublic("DeleteFile failed. (%d)", GetLastError());
				__leave;
			}
		}

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}
