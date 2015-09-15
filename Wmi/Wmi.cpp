#include "Wmi.h"

BOOL
	CWmi::Query(
	__in LPSTR	lpClass,
	__in LPTSTR lpContent
	)
{
	BOOL					bRet					= FALSE;

	HRESULT					hResult					= S_FALSE;
	BOOL					bNeedCoUnInit			= FALSE;
	IWbemLocator*			pIWbemLocator			= NULL;
	IWbemServices*			pIWbemServices			= NULL;
	IEnumWbemClassObject*	pIEnumWbemClassObject	= NULL;
	IWbemClassObject*		pIWbemClassObject		= NULL;
	ULONG					ulReturned				= 0;
	VARIANT					Variant					= {0};
	BOOL					bBreakByFailed			= TRUE;
	CHAR					chSql[MAX_PATH]			= {0};


	do 
	{
		if (!lpClass || !lpContent)
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p 0x%08p", lpClass, lpContent);
			break;
		}

		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------
		hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoInitializeEx failed. (%d)", hResult);
			break;
		}

		bNeedCoUnInit = TRUE;

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------
		hResult = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoInitializeSecurity failed. (%d)", hResult);
			break;
		}

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------
		hResult = CoCreateInstance(
			CLSID_WbemLocator,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID *)&pIWbemLocator
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoCreateInstance failed. (%d)", hResult);
			break;
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method
		hResult = pIWbemLocator->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"),
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			NULL,
			&pIWbemServices
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIWbemLocator->ConnectServer failed. (%d)", hResult);
			break;
		}

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------
		hResult = CoSetProxyBlanket(
			pIWbemServices,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoSetProxyBlanket failed. (%d)", hResult);
			break;
		}

		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----
		strcat_s(chSql, _countof(chSql), "SELECT * FROM ");
		strcat_s(chSql, _countof(chSql), lpClass);

		hResult = pIWbemServices->ExecQuery(
			bstr_t("WQL"),
			bstr_t(chSql),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pIEnumWbemClassObject
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIWbemServices->ExecQuery failed. (%d)", hResult);
			break;
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		if (pIEnumWbemClassObject)
		{
			do 
			{
				hResult = pIEnumWbemClassObject->Next(
					WBEM_INFINITE,
					1,
					&pIWbemClassObject,
					&ulReturned			
					);
				if (FAILED(hResult))
				{
					printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIEnumWbemClassObject->Next failed. (%d)", hResult);
					break;
				}
				else
				{
					if (!ulReturned)
					{
						bBreakByFailed = FALSE;
						break;
					}
				}

				hResult = pIWbemClassObject->Get(
					lpContent,
					0,
					&Variant,
					NULL,
					NULL
					);
				if (FAILED(hResult))
				{
					printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIWbemClassObject->Get failed. (%d)", hResult);
					break;
				}

				if (Variant.bstrVal)
					printfEx(MOD_WMI, PRINTF_LEVEL_INFORMATION, "[%s][%S] %S ", lpClass, lpContent, Variant.bstrVal);
				else
					printfEx(MOD_WMI, PRINTF_LEVEL_INFORMATION, "[%s][%S] NULL", lpClass, lpContent);

				VariantClear(&Variant);
				if (pIWbemClassObject)
				{
					pIWbemClassObject->Release();
					pIWbemClassObject = NULL;
				}
				ulReturned = 0;
			} while (TRUE);

			if (bBreakByFailed)
				break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (pIWbemClassObject)
		pIWbemClassObject->Release();

	if (pIEnumWbemClassObject)
		pIEnumWbemClassObject->Release();

	if (pIWbemServices)
		pIWbemServices->Release();

	if (pIWbemLocator)
		pIWbemLocator->Release();

	if (bNeedCoUnInit)
		CoUninitialize();

	return bRet;
}

BOOL
	CWmi::QueryCaption(
	__in	ULONG	ulDiskNumber,
	__out	LPTSTR	lpOutBuf,
	__in	ULONG	ulOutBufSizeCh
	)
{
	BOOL					bRet					= FALSE;

	HRESULT					hResult					= S_FALSE;
	BOOL					bNeedCoUnInit			= FALSE;
	IWbemLocator*			pIWbemLocator			= NULL;
	IWbemServices*			pIWbemServices			= NULL;
	IEnumWbemClassObject*	pIEnumWbemClassObject	= NULL;
	IWbemClassObject*		pIWbemClassObject		= NULL;
	ULONG					ulReturned				= 0;
	VARIANT					Variant					= {0};
	BOOL					bBreakByFailed			= TRUE;


	do 
	{
		if (!lpOutBuf || !ulOutBufSizeCh)
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "input arguments error. 0x%08p %d", lpOutBuf, ulOutBufSizeCh);
			break;
		}

		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------
		hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoInitializeEx failed. (%d)", hResult);
			break;
		}

		bNeedCoUnInit = TRUE;

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------
		hResult = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoInitializeSecurity failed. (%d)", hResult);
			break;
		}

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------
		hResult = CoCreateInstance(
			CLSID_WbemLocator,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID *)&pIWbemLocator
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoCreateInstance failed. (%d)", hResult);
			break;
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method
		hResult = pIWbemLocator->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"),
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			NULL,
			&pIWbemServices
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIWbemLocator->ConnectServer failed. (%d)", hResult);
			break;
		}

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------
		hResult = CoSetProxyBlanket(
			pIWbemServices,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "CoSetProxyBlanket failed. (%d)", hResult);
			break;
		}

		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----
		hResult = pIWbemServices->ExecQuery(
			bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_DiskDrive"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pIEnumWbemClassObject
			);
		if (FAILED(hResult))
		{
			printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIWbemServices->ExecQuery failed. (%d)", hResult);
			break;
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		if (pIEnumWbemClassObject)
		{
			do 
			{
				hResult = pIEnumWbemClassObject->Next(
					WBEM_INFINITE,
					1,
					&pIWbemClassObject,
					&ulReturned			
					);
				if (FAILED(hResult))
				{
					printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "pIEnumWbemClassObject->Next failed. (%d)", hResult);
					break;
				}
				else
				{
					if (!ulReturned)
					{
						bBreakByFailed = FALSE;
						break;
					}
				}

				hResult = pIWbemClassObject->Get(
					_T("Index"),
					0,
					&Variant,
					NULL,
					NULL
					);
				if (FAILED(hResult))
				{
					printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "[Index] pIWbemClassObject->Get failed. (%d)", hResult);
					break;
				}

				if (Variant.lVal == ulDiskNumber)
				{
					VariantClear(&Variant);
					hResult = pIWbemClassObject->Get(
						_T("Caption"),
						0,
						&Variant,
						NULL,
						NULL
						);
					if (FAILED(hResult))
					{
						printfEx(MOD_WMI, PRINTF_LEVEL_ERROR, "[Caption] pIWbemClassObject->Get failed. (%d)", hResult);
						break;
					}

					_tcscat_s(lpOutBuf, ulOutBufSizeCh, Variant.bstrVal);
					bBreakByFailed = FALSE;
					break;
				}

				VariantClear(&Variant);
				if (pIWbemClassObject)
				{
					pIWbemClassObject->Release();
					pIWbemClassObject = NULL;
				}
				ulReturned = 0;
			} while (TRUE);

			if (bBreakByFailed)
				break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (pIWbemClassObject)
		pIWbemClassObject->Release();

	if (pIEnumWbemClassObject)
		pIEnumWbemClassObject->Release();

	if (pIWbemServices)
		pIWbemServices->Release();

	if (pIWbemLocator)
		pIWbemLocator->Release();

	if (bNeedCoUnInit)
		CoUninitialize();

	return bRet;
}
