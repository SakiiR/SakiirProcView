#include "ProcFuncs.h"

int main(int argc, char **argv)
{
	EnableDebugPrivilege(1);
	if (argc < 2)
	{
		printf("## Sakiir Proc View V2.0 ##\n");
		printf("## Last Console Version  ##\n");
		printf("\tUSAGE : ./prog -h\n");
		return (1);
	}

	if (strcmp(argv[1], "-h") == 0)
	{
		printf("## Sakiir Proc View V2.0 ##\n");
		printf("## Last Console Version  ##\n");
		printf("\tHELP					  : ./prog -h\n");
		printf("\tLIST PROCESS			  : ./prog -lp\n");
		printf("\tLIST MODULES			  : ./prog -lm <PID>\n");
		printf("\tLIST THREAD			  : ./prog -lt <PID>\n");
		printf("\tGET PID BY NAME         : ./prog -gpid <NAME>\n");
		printf("\tKILL PROCESS BY NAME    : ./prog -kn <NAME>\n");
		printf("\tKILL PROCESS BY ID      : ./prog -kid <PID>\n");
		printf("\t");
		Sleep(2000);
		return (1);
	}

	if (strcmp(argv[1], "-l") == 0)
	{
		if (ListProcess() == -1)
		{
			printf("[-] Failed To List Process !");
			return (-1);
		}
		return (1);
	}

	if (strcmp(argv[1], "-lm") == 0)
	{
		if (argc != 3)
		{
			printf("[-] BAD ARGUMENTS GIVEN !\n");
			return (-1);
		}

		DWORD PID = atoi(argv[2]);
		if (ListModulesProcess(PID) == -1)
		{
			printf("[-] Failed To List Modules\n");
			return (-1);
		}
		return (1);
	}

	if (strcmp(argv[1], "-lt") == 0)
	{
		if (argc != 3)
		{
			printf("[-] BAD ARGUMENTS GIVEN !\n");
			return (-1);
		}

		DWORD PID = atoi(argv[2]);
		if (ListThreadProcess(PID) == -1)
		{
			printf("[-] Failed To List Threads\n");
			return (-1);
		}
		return (1);

	}

	if (strcmp(argv[1], "-gpid") == 0)
	{
		wchar_t NAME[1024] = L"";
		DWORD PID;
		size_t length = 0;
		int size;
		if (argc != 3)
		{
			printf("[-] BAD ARGUMENTS GIVEN !\n");
			return (-1);
		}
		
		size  = strlen(argv[2]);
		mbstowcs_s(&length, NAME, argv[2], size + 1); // Convert char *  to LPWSTR 
		PID = GetPidByname(NAME);
		if (PID == -1)
		{
			_tprintf(TEXT("[-] Failed To Find %s\n"), NAME);
			return (-1);
		}

		printf("[+] PID : %d\n", PID);
		return (1);
	}

	if (strcmp(argv[1], "-kn") == 0)
	{
		wchar_t NAME[1024] = L"";
		size_t length = 0;
		int size;

		if (argc != 3)
		{
			printf("[-] BAD ARGUMENTS GIVEN !\n");
			return (-1);
		}
		size = strlen(argv[2]);
		mbstowcs_s(&length, NAME, argv[2], size + 1);
		if (KillProcessByName(NAME) == -1)
		{
			printf("[-] Failed To Kill Process");
			return (-1);
		}
		printf("[+] Process Killed ! \n");
		return (1);
	}

	if (strcmp(argv[1], "-kid") == 0)
	{
		DWORD PID;
		if (argc != 3)
		{
			printf("[-] BAD ARGUMENTS GIVEN !\n");
			return (-1);
		}
		PID = atoi(argv[2]);
		if (KillProcessByPID(PID) == -1)
		{
			printf("[-] Failed To Kill Process");
			return (-1);
		}
		printf("[+] Process Killed ! \n");
		return (1);
	}
	EnableDebugPrivilege(0);
	


}