#pragma once

#include <stdio.h>
#include <Rpc.h>

#include "../OperationSystemVersion/OperationSystemVersion.h"


#pragma comment(lib, "Rpcrt4.lib")


#ifndef _PUBLIC_SERVER_TAG_
#define _PUBLIC_SERVER_TAG_	'VSBP'
#endif


void
	__RPC_FAR* __RPC_USER
	midl_user_allocate(
	size_t len
	);

void
	__RPC_USER
	midl_user_free(
	void __RPC_FAR *ptr
	);


class CPublicServer
{
public:
	CPublicServer();

	~CPublicServer();

	BOOL
		Init(
		__in		RPC_IF_HANDLE					RpcIfHandle,
		__in_opt	UUID						*	pMgrTypeUuid,
		__in_opt	RPC_MGR_EPV					*	pMgrEpv,
		__in		unsigned int					uFlags,
		__in_opt	unsigned int					uMaxCalls,
		__in_opt	unsigned int					uMaxRpcSize,
		__in_opt	RPC_IF_CALLBACK_FN			*	pIfCallbackFn,
		__in		LPTSTR							lpProtseq,
		__in		LPTSTR							lpEndpoint,
		__in_opt	void						*	pSecurityDescriptor,
		__in		unsigned int					uMinimumCallThreads,
		__in		unsigned int					uDontWait,
		__in_opt	RPC_MGMT_AUTHORIZATION_FN		RpcMgmtAuthorizationFn
		);

	BOOL
		Unload(
		__in BOOL bFromRpcInterface
		);

	static
		int
		__RPC_API 
		RpcMgmtAuthorizationFn(
		__in	RPC_BINDING_HANDLE		ClientBinding,
		__in	unsigned long			RequestedMgmtOperation,
		__out	RPC_STATUS __RPC_FAR *	Status
		);

	static
		RPC_STATUS
		RPC_ENTRY
		RpcIfCallbackFn(
		__in RPC_IF_HANDLE		InterfaceUuid,
		__in void			*	Context
		);

private:
	typedef struct _RPC_SERVER_LISTEN_INFO
	{
		unsigned int uMinimumCallThreads;
		unsigned int uMaxCalls;
	} RPC_SERVER_LISTEN_INFO, *PRPC_SERVER_LISTEN_INFO, *LPRPC_SERVER_LISTEN_INFO;

	static unsigned int		ms_bDontWait;
	static UUID			*	ms_pMgrTypeUuid;
	static BOOL				ms_bRegistedAuthorizationFn;
	static BOOL				ms_bUnloaded;

	static
		DWORD
		WINAPI
		RpcServerListenThread(
		LPVOID lpThreadParameter
		);
};
