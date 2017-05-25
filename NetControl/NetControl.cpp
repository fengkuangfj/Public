
#include "NetControl.h"

BOOL
CNetControl::GetLocalIP(
						__inout	LPTSTR	lpIP,
						__in	ULONG	ulBufSizeCh
						)
{
	BOOL		bRet = FALSE;

	WSADATA		WsaData = {0};
	int			nResult = 0;
	CHAR		chHostName[MAX_PATH] = {0};
	PHOSTENT	pHostent = NULL;


	__try
	{
		if (!lpIP || !ulBufSizeCh)
			__leave;

		nResult= WSAStartup(MAKEWORD(2,2), &WsaData);  
		if (nResult)  
			__leave; 

		nResult = gethostname(chHostName, sizeof(chHostName));  
		if (nResult)  
			__leave; 

		pHostent = gethostbyname(chHostName);  
		if (!pHostent)  
			__leave;

		StringCchPrintf(lpIP, ulBufSizeCh, _T("%S"), inet_ntoa(*((in_addr *)(*(pHostent->h_addr_list)))));

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}

BOOL
CNetControl::Connection(
						__in LPTSTR lpLocalName,
						__in LPTSTR lpRemoteName,
						__in LPTSTR lpUserName,
						__in LPTSTR lpPassword,
						__in BOOL	bRemembered
						)
{
	BOOL		bRet = FALSE;

	NETRESOURCE NetResource = {0};
	DWORD		dwResult = NO_ERROR;


	__try
	{
		if (!lpRemoteName || !lpUserName || !lpPassword)
		{
			printfEx(MOD_NET_CONTROL, PRINTF_LEVEL_ERROR, "input arguments error. lpRemoteName(%p) lpUserName(%p) lpPassword(%p)",
				lpRemoteName, lpUserName, lpPassword);

			__leave;
		}

		NetResource.dwType = RESOURCETYPE_ANY;
		NetResource.lpLocalName = lpLocalName;
		NetResource.lpRemoteName = lpRemoteName;

		dwResult = WNetAddConnection2(&NetResource, lpPassword, lpUserName, bRemembered ? CONNECT_UPDATE_PROFILE : CONNECT_TEMPORARY);
		if (NO_ERROR != dwResult)
		{
			printfEx(MOD_NET_CONTROL, PRINTF_LEVEL_ERROR, "WNetAddConnection2 failed. (%d)", dwResult);
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
CNetControl::DisConnection(
						   __in LPTSTR lpRemoteName
						   )
{
	BOOL	bRet = FALSE;

	DWORD	dwResult = NO_ERROR;


	__try
	{
		if (!lpRemoteName)
		{
			printfEx(MOD_NET_CONTROL, PRINTF_LEVEL_ERROR, "input argument error");
			__leave;
		}

		do 
		{
			dwResult = WNetCancelConnection2(lpRemoteName, CONNECT_UPDATE_PROFILE, FALSE);
			if (NO_ERROR == dwResult)
				break;

			if (ERROR_OPEN_FILES == dwResult)
			{
				Sleep(1);
				continue;
			}

			if (NO_ERROR != dwResult)
			{
				printfEx(MOD_NET_CONTROL, PRINTF_LEVEL_ERROR, "WNetCancelConnection2 failed. (%d)", dwResult);
				__leave;
			}
		} while (TRUE);

		bRet = TRUE;
	}
	__finally
	{
		;
	}

	return bRet;
}