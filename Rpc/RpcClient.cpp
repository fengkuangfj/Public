#include "RpcPublicClient.h"


LPTSTR CPublicClient::ms_lpStringBinding = NULL;


CPublicClient::CPublicClient()
{
	;
}

CPublicClient::~CPublicClient()
{
	;
}

BOOL
	CPublicClient::Init(
	__in_opt	LPTSTR					lpObjUuid,
	__in		LPTSTR					lpProtSeq,
	__in_opt	LPTSTR					lpNetworkAddr,
	__in		LPTSTR					lpEndPoint,
	__in_opt	LPTSTR					lpOptions,
	__in		RPC_BINDING_HANDLE	*	pRpcBindingHandle
	)
{
	BOOL		bRet		= FALSE;

	RPC_STATUS	RpcStatus	= RPC_S_OK;


	__try
	{
		if (!lpProtSeq ||
			!lpEndPoint ||
			!pRpcBindingHandle)
			__leave;

		RpcStatus = RpcStringBindingCompose(
			(RPC_WSTR)lpObjUuid,
			(RPC_WSTR)lpProtSeq,
			(RPC_WSTR)lpNetworkAddr,
			(RPC_WSTR)lpEndPoint,
			(RPC_WSTR)lpOptions,
			(RPC_WSTR *)&ms_lpStringBinding
			);
		if (RPC_S_OK != RpcStatus)
			__leave;

		RpcStatus = RpcBindingFromStringBinding((RPC_WSTR)ms_lpStringBinding, pRpcBindingHandle);
		if (RPC_S_OK != RpcStatus)
			__leave;

		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			if (ms_lpStringBinding)
			{
				RpcStatus = RpcStringFree((RPC_WSTR *)&ms_lpStringBinding);
				if (RPC_S_OK == RpcStatus)
					ms_lpStringBinding = NULL;
			}

			if (pRpcBindingHandle && *pRpcBindingHandle)
			{
				RpcStatus = RpcBindingFree(pRpcBindingHandle);
				if (RPC_S_OK == RpcStatus)
					*pRpcBindingHandle = NULL;
			}
		}
	}

	return bRet;
}

BOOL
	CPublicClient::Unload(
	__in RPC_BINDING_HANDLE * pRpcBindingHandle
	)
{
	BOOL		bRet		= TRUE;

	RPC_STATUS	RpcStatus	= RPC_S_OK;


	__try
	{
		if (!pRpcBindingHandle)
		{
			bRet = FALSE;
			__leave;
		}

		if (ms_lpStringBinding)
		{
			RpcStatus = RpcStringFree((RPC_WSTR *)&ms_lpStringBinding);
			if (RPC_S_OK == RpcStatus)
				ms_lpStringBinding = NULL;
			else
				bRet = FALSE;
		}

		if (*pRpcBindingHandle)
		{
			RpcStatus = RpcBindingFree(pRpcBindingHandle);
			if (RPC_S_OK == RpcStatus)
				*pRpcBindingHandle = NULL;
			else
				bRet = FALSE;
		}
	}
	__finally
	{
		;
	}

	return bRet;
}

void
	__RPC_FAR* __RPC_USER
	midl_user_allocate(
	size_t len
	)
{
	return(malloc(len));
}

void
	__RPC_USER
	midl_user_free(
	void __RPC_FAR *ptr
	)
{
	free(ptr);
}
