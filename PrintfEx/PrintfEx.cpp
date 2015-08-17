#include "PrintfEx.h"

VOID
	CPrintfEx::PrintfInternal(
	__in PCHAR	pFuncName,
	__in LPSTR	Fmt,
	...
	)
{
	CHAR chInfo[MAX_PATH] = {0};

	va_list Args;


	setlocale(LC_ALL, "");

	va_start(Args, Fmt);

	StringCchVPrintfA(chInfo, _countof(chInfo), Fmt, Args);

	printf("[%hs] %hs \n", pFuncName, chInfo);

	va_end(Args);
}
