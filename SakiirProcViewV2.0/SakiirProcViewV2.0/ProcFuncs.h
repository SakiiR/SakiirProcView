#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <CommCtrl.h>

DWORD GetPidByname(LPWSTR NAME);
DWORD GetModuleAddrByName(LPWSTR NAME, DWORD PID);
DWORD ListProcess();
DWORD ListModulesProcess(DWORD PID);
DWORD ListThreadProcess(DWORD PID);
DWORD KillProcessByName(LPWSTR NAME);
DWORD KillProcessByPID(DWORD PID);
DWORD EnableDebugPrivilege(DWORD fEnable);