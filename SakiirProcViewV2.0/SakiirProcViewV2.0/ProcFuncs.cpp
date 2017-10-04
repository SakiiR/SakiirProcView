#include "ProcFuncs.h"

DWORD ListProcess()
{
	PROCESSENTRY32 p;
	HANDLE hProcessSnap;
	HANDLE hProcess;
	DWORD dwPriorityClass;
	p.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	if (!Process32First(hProcessSnap, &p))
		return (-1);
	do{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, p.th32ProcessID);
		dwPriorityClass = GetPriorityClass(hProcess);

		_tprintf(TEXT("#### PROCESS %s ####\n"), p.szExeFile);
		printf("\tProcess ID             : %d\n",p.th32ProcessID);
		printf("\tProcess THREAD COUNT   : %d\n", p.cntThreads);
		printf("\tProcess USAGE          : %d\n", p.cntUsage);
		printf("\tProcess PRIORITY BASE  : %d\n", p.pcPriClassBase);
		if (dwPriorityClass)
			printf("\tProcess PRIORITY CLASS : %d\n", dwPriorityClass);
		printf("\n\n");
	} while (Process32Next(hProcessSnap, &p));
	return (1);
}

DWORD GetPidByname(LPWSTR NAME)
{
	PROCESSENTRY32 p;
	HANDLE hProcessSnap;
	p.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!Process32First(hProcessSnap, &p))
		return (-1);
	do{
		if (!lstrcmpW(NAME, p.szExeFile)) return (p.th32ProcessID);
	} while (Process32Next(hProcessSnap, &p));
	return (-1);
}
	
DWORD GetModuleAddrByName(LPWSTR NAME, DWORD PID)
{
	MODULEENTRY32 m;
	HANDLE hModuleSnap;
	m.dwSize = sizeof(MODULEENTRY32);
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (!Module32First(hModuleSnap, &m))
		return (-1);
	do{
		if (!lstrcmpW(NAME,m.szModule)) return ((DWORD)m.modBaseAddr);
	} while (Module32Next(hModuleSnap, &m));
	return (-1);
}

DWORD ListModulesProcess(DWORD PID)
{
	MODULEENTRY32 m;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return(-1);
	m.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &m))
		return (-1);
	do{
		_tprintf(TEXT("#### MODULE %s ####\n"), m.szModule);
		printf("\tModule ID			: %d\n",m.th32ModuleID);
		printf("\tModule BASE ADDRESS : 0x%x\n", m.modBaseAddr);
		_tprintf(TEXT("\tModule PATH         : %s\n"), m.szExePath);
		_tprintf(TEXT("\tModule NAME         : %s\n\n"), m.szModule);

	} while (Module32Next(hModuleSnap, &m));
	return (1);

}

DWORD ListThreadProcess(DWORD PID)
{
	HANDLE hThreadSnap;
	THREADENTRY32 t;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	t.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &t))
		return (-1);
	do
	{
		if (t.th32OwnerProcessID == PID)
		{
			printf("#### THREAD %d ####\n",t.th32ThreadID);
			printf("\tThread FLAG           : %d\n", t.dwFlags);
			printf("\tThread USAGE          : %d\n", t.cntUsage);
			printf("\tThread BASE PRIORITY  : %d\n", t.tpBasePri);
			printf("\tThread DELTA PRIORITY : %d\n\n", t.tpDeltaPri);
		}
	} while (Thread32Next(hThreadSnap, &t));
	return(1);
}

DWORD KillProcessByName(LPWSTR NAME)
{
	HANDLE hProcess = NULL;
	DWORD PID;
	if ((PID = GetPidByname(NAME)) == -1)
		hProcess = OpenProcess(PROCESS_TERMINATE, false, PID);
	if (hProcess == NULL)
		return (-1);
	if (TerminateProcess(hProcess, 0) == 0)
		return(-1);
	return (1);
}

DWORD KillProcessByPID(DWORD PID)
{
	HANDLE hProcess = NULL;
	hProcess = OpenProcess(PROCESS_TERMINATE, false, PID);
	if (hProcess == NULL)
		return (-1);
	if (TerminateProcess(hProcess, 0) == 0)
		return(-1);
	return (1);
}

BOOL SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return FALSE;

	// 
	// first pass.  get current privilege setting
	// 
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(hToken,FALSE,&tp,	sizeof(TOKEN_PRIVILEGES),&tpPrevious,&cbPrevious);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL
		);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	return TRUE;
};

DWORD EnableDebugPrivilege(DWORD fEnable)
{
	DWORD fOk = 0;
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(0, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, 0, &tp, sizeof(tp), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}