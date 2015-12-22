#include "RemoteThread.h"

BOOL
	CRemoteThread::Inject(
	__in DWORD					dwPid,
	__in LPTHREAD_START_ROUTINE lpStartAddress,
	__in LPTHREAD_START_ROUTINE	lpStartAddressFollow,
	__in LPVOID					lpParameter,
	__in DWORD					dwParameterSizeB
	)
{
	BOOL		bRet						= FALSE;

	HANDLE		hProcess					= NULL;
	HANDLE		hThread						= NULL;
	LPVOID		lpFunctionAddress			= NULL;
	LPVOID		lpFunctionParameterAddress	= NULL;
	DWORD		dwFunctionSizeB				= 0;
	BYTE	*	pbyStartAddress				= NULL;
	BYTE	*	pbyStartAddressFollow		= NULL;


	__try
	{
		if (!dwPid || !lpStartAddress || !lpStartAddressFollow || !lpParameter || !dwParameterSizeB)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "input arguments error. dwPid(%d) lpStartAddress(0x%p) lpStartAddressFollow(0x%p) lpParameter(%x%p) dwParameterSizeB(%d)",
				dwPid, lpStartAddress, lpStartAddressFollow, lpParameter, dwParameterSizeB);

			__leave;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (!hProcess)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "OpenProcess failed. (%d)", GetLastError());
			__leave;
		}

		//��Ŀ������ڷ������̺߳�����ʼ��ַ���ڴ�
#ifdef _DEBUG
		pbyStartAddress = (BYTE *)lpStartAddress;		//DEBUGģʽ�±���������һ��������ת��
		if (0xe9 == *pbyStartAddress)	//jmp�Ļ�����Ϊ0xE9
		{
			pbyStartAddress++;	
			pbyStartAddress += *(int *)pbyStartAddress;				//��ǰ��ַ+ƫ�Ƶ�ַ
			pbyStartAddress += 4;				//�˴�����ΪCC CC CC CC�ټ���ʵ����������,����Ҫ������4��0xCC,
		}
		lpStartAddress = (LPTHREAD_START_ROUTINE)pbyStartAddress;

		pbyStartAddressFollow = (BYTE *)lpStartAddressFollow;		//DEBUGģʽ�±���������һ��������ת��
		if (0xe9 == *pbyStartAddressFollow)	//jmp�Ļ�����Ϊ0xE9
		{
			pbyStartAddressFollow++;	
			pbyStartAddressFollow += *(int *)pbyStartAddressFollow;				//��ǰ��ַ+ƫ�Ƶ�ַ
			pbyStartAddressFollow += 4;				//�˴�����ΪCC CC CC CC�ټ���ʵ����������,����Ҫ������4��0xCC,
		}
		lpStartAddressFollow = (LPTHREAD_START_ROUTINE)pbyStartAddressFollow;
#endif

		dwFunctionSizeB = ((BYTE *)(DWORD)lpStartAddressFollow - (BYTE *)(DWORD)lpStartAddress); 

		lpFunctionAddress = VirtualAllocEx(
			hProcess,
			NULL,
			dwFunctionSizeB,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!lpFunctionAddress)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "VirtualAllocEx failed. (%d)", GetLastError());
			__leave;
		}

		if (!WriteProcessMemory(
			hProcess,
			lpFunctionAddress,
			(LPCVOID)lpStartAddress,
			dwFunctionSizeB,
			NULL
			))
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "WriteProcessMemory failed. (%d)", GetLastError());
			__leave;
		}

		lpFunctionParameterAddress = VirtualAllocEx(
			hProcess,
			NULL,
			dwParameterSizeB,
			MEM_COMMIT,
			PAGE_READWRITE
			);
		if (!lpFunctionParameterAddress)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "VirtualAllocEx failed. (%d)", GetLastError());
			__leave;
		}

		if (!WriteProcessMemory(
			hProcess,
			lpFunctionParameterAddress,
			(LPCVOID)lpParameter,
			dwParameterSizeB,
			NULL
			))
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "WriteProcessMemory failed. (%d)", GetLastError());
			__leave;
		}

		hThread = CreateRemoteThread(
			hProcess,
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)lpFunctionAddress,
			lpFunctionParameterAddress,
			0,
			NULL
			);
		if (!hThread)
		{
			printfEx(MOD_REMOTE_THREAD, PRINTF_LEVEL_ERROR, "CreateRemoteThread failed. (%d)", GetLastError());
			__leave;
		}

		bRet = TRUE;
	}
	__finally
	{
		if (lpFunctionAddress)
		{
			VirtualFreeEx(hProcess, lpFunctionAddress, sizeof(DWORD), MEM_RELEASE);
			lpFunctionAddress = NULL;
		}

		if (lpFunctionParameterAddress)
		{
			VirtualFreeEx(hProcess, lpFunctionParameterAddress, sizeof(DWORD), MEM_RELEASE);
			lpFunctionParameterAddress = NULL;
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

	return bRet;
}
