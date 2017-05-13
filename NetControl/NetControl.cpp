
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
