#pragma once


#ifndef _PUBLIC_TAG_
#define _PUBLIC_TAG_	'LBUP'
#endif


BOOL
	ExecuteCmdLine(
	__in		LPTSTR	lpCmdLine,
	__in_opt	BOOL	bWaitUntilCmdExit = TRUE
	);
