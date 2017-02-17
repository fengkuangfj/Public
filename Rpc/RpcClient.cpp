#include "RpcClient.h"


CRpcClient			*	CRpcClient::ms_pInstance = NULL;
LPTSTR					CRpcClient::ms_lpStringBinding = NULL;
RPC_BINDING_HANDLE		CRpcClient::ms_RpcServerInterface_Binding = NULL;


CRpcClient::CRpcClient(
	__in_opt	LPTSTR					lpObjUuid,
	__in		LPTSTR					lpProtSeq,
	__in_opt	LPTSTR					lpNetworkAddr,
	__in		LPTSTR					lpEndPoint,
	__in_opt	LPTSTR					lpOptions,
	__in		RPC_BINDING_HANDLE	*	pRpcBindingHandle
)
{
	ms_pInstance = this;

	Init(lpObjUuid, lpProtSeq, lpNetworkAddr, lpEndPoint, lpOptions, pRpcBindingHandle);
}

CRpcClient::~CRpcClient()
{
	Unload(&ms_RpcServerInterface_Binding);
}

BOOL
CRpcClient::Init(
	__in_opt	LPTSTR					lpObjUuid,
	__in		LPTSTR					lpProtSeq,
	__in_opt	LPTSTR					lpNetworkAddr,
	__in		LPTSTR					lpEndPoint,
	__in_opt	LPTSTR					lpOptions,
	__in		RPC_BINDING_HANDLE	*	pRpcBindingHandle
)
{
	BOOL		bRet = FALSE;

	RPC_STATUS	RpcStatus = RPC_S_OK;


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

		ms_RpcServerInterface_Binding = ms_lpStringBinding;

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
CRpcClient::Unload(
	__in RPC_BINDING_HANDLE * pRpcBindingHandle
)
{
	BOOL		bRet = TRUE;

	RPC_STATUS	RpcStatus = RPC_S_OK;


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

#pragma warning(push)
#pragma warning(disable : 4127)
CRpcClient *
CRpcClient::GetInstance(
	__in_opt	LPTSTR					lpObjUuid,
	__in		LPTSTR					lpProtSeq,
	__in_opt	LPTSTR					lpNetworkAddr,
	__in		LPTSTR					lpEndPoint,
	__in_opt	LPTSTR					lpOptions,
	__in		RPC_BINDING_HANDLE	*	pRpcBindingHandle
)
{
	typedef enum _INSTANCE_STATUS
	{
		INSTANCE_STATUS_UNINITED = 0,
		INSTANCE_STATUS_INITING = 1,
		INSTANCE_STATUS_INITED = 2
	} INSTANCE_STATUS, *PINSTANCE_STATUS, *LPINSTANCE_STATUS;

	static LONG ms_lInstanceStatus = INSTANCE_STATUS_UNINITED;



	if (INSTANCE_STATUS_UNINITED == InterlockedCompareExchange(&ms_lInstanceStatus, INSTANCE_STATUS_INITING, INSTANCE_STATUS_UNINITED))
	{
		do
		{
			new CRpcClient(lpObjUuid, lpProtSeq, lpNetworkAddr, lpEndPoint, lpOptions, pRpcBindingHandle);
			if (!ms_pInstance)
				Sleep(1000);
			else
			{
				InterlockedCompareExchange(&ms_lInstanceStatus, INSTANCE_STATUS_INITED, INSTANCE_STATUS_INITING);
				break;
			}
		} while (TRUE);
	}
	else
	{
		do
		{
			if (INSTANCE_STATUS_INITED != ms_lInstanceStatus)
				Sleep(1000);
			else
				break;
		} while (TRUE);
	}

	return ms_pInstance;
}
#pragma warning(pop)

VOID
CRpcClient::ReleaseInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
