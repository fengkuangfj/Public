
#pragma once

#include <Sddl.h>

class CTokenControl
{
public:
	static
		BOOL
		GetCurrentUserSid(
		__inout LPTSTR	lpUserSid,
		__in	ULONG	ulSizeCh
		);

private:
	CTokenControl();

	~CTokenControl();
};
