#include "Wmi.h"

BOOL
	CWmi::Query()
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
		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------
		hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hResult))
		{
			printf("[QueryWMI] : CoInitializeEx failed. (%d) \n", hResult);
			break;
		}

		bNeedCoUnInit = TRUE;

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------
		hResult = S_FALSE;
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
			printf("[QueryWMI] : CoInitializeSecurity failed. (%d) \n", hResult);
			break;
		}

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------
		hResult = S_FALSE;
		hResult = CoCreateInstance(
			CLSID_WbemLocator,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID *)&pIWbemLocator
			);
		if (FAILED(hResult))
		{
			printf("[QueryWMI] : CoCreateInstance failed. (%d) \n", hResult);
			break;
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method
		hResult = S_FALSE;
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
			printf("[QueryWMI] : ConnectServer failed. (%d) \n", hResult);
			break;
		}

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------
		hResult = S_FALSE;
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
			printf("[QueryWMI] : CoSetProxyBlanket failed. (%d) \n", hResult);
			break;
		}

		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----
		hResult = S_FALSE;
		hResult = pIWbemServices->ExecQuery(
			bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_DiskDrive"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pIEnumWbemClassObject
			);
		if (FAILED(hResult))
		{
			printf("[QueryWMI] : ExecQuery failed. (%d) \n", hResult);
			break;
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		if (pIEnumWbemClassObject)
		{
			do 
			{
				hResult = S_FALSE;
				hResult = pIEnumWbemClassObject->Next(
					WBEM_INFINITE,
					1,
					&pIWbemClassObject,
					&ulReturned			
					);
				if (FAILED(hResult))
				{
					printf("[QueryWMI] : Next failed. (%d) \n", hResult);
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

				hResult = S_FALSE;
				hResult = pIWbemClassObject->Get(
					L"Caption",
					0,
					&Variant,
					NULL,
					NULL
					);
				if (FAILED(hResult))
				{
					printf("[QueryWMI] : Get failed. (%d) \n", hResult);
					break;
				}

				if (Variant.bstrVal)
					printf("[QueryWMI] : Win32_DiskDrive - %S \n", Variant.bstrVal);
				else
					printf("[QueryWMI] : Win32_DiskDrive - \n");

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
