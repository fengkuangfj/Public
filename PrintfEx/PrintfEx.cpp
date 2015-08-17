#include "PrintfEx.h"

VOID
	PrintfInternal(
	__in PCHAR	pFuncName,
	__in LPSTR	Fmt,
	...
	)
{
	CHAR chInfo[MAX_PATH] = {0};

	va_list Args;


	va_start(Args, Fmt);

	StringCchVPrintfA(chInfo, _countof(chInfo), Fmt, Args);

	printf("[%s] %s \n", pFuncName, chInfo);

	va_end(Args);
}
