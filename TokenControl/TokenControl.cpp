
#include "TokenControl.h"

CTokenControl::CTokenControl()
{
}

CTokenControl::~CTokenControl()
{
}

BOOL
CTokenControl::GetCurrentUserSid(
__inout LPTSTR	lpUserSid,
__in	ULONG	ulSizeCh
)
{
	BOOL			bRet = FALSE;

	DWORD			dwSesssionID = 0xFFFFFFFF;
	HANDLE			hUserToken = NULL;
	PTOKEN_USER		pTokenUser = NULL;
	DWORD			dwReturnLength = 0;
	LPTSTR			lpSid = NULL;


	__try
	{
		if (!lpUserSid || !ulSizeCh)
			__leave;

		dwSesssionID = WTSGetActiveConsoleSessionId();
		if (0xFFFFFFFF == dwSesssionID)
			__leave;

		if (!WTSQueryUserToken(dwSesssionID, &hUserToken))
			__leave;

		GetTokenInformation(hUserToken, TokenUser, pTokenUser, dwReturnLength, &dwReturnLength);
		if (!dwReturnLength)
			__leave;

		pTokenUser = (PTOKEN_USER)calloc(1, dwReturnLength);
		if (!pTokenUser)
			__leave;

		if (!GetTokenInformation(hUserToken, TokenUser, pTokenUser, dwReturnLength, &dwReturnLength))
			__leave;

		if (!ConvertSidToStringSid(pTokenUser->User.Sid, &lpSid))
			__leave;

		_tcscat_s(lpUserSid, ulSizeCh, lpSid);

		bRet = TRUE;
	}
	__finally
	{
		if (pTokenUser)
		{
			free(pTokenUser);
			pTokenUser = NULL;
		}

		if (hUserToken)
		{
			CloseHandle(hUserToken);
			hUserToken = NULL;
		}
	}

	return bRet;
}
