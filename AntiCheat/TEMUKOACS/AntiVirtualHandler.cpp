#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include <Winnetwk.h>

#pragma comment(lib,"MPR")

inline int TEMUKOACSEngine::exists_regkey(HKEY hKey, char* regkey_s)
{
	HKEY regkey;
	LONG ret;

	ret = RegOpenKeyEx(hKey, regkey_s, 0, KEY_READ, &regkey);
	if (ret == ERROR_SUCCESS)
	{
		RegCloseKey(regkey);
		return TRUE;
	}
	else
		return FALSE;
}

inline int TEMUKOACSEngine::exists_regkey_value_str(HKEY hKey, char* regkey_s, char* value_s, char* lookup)
{
	HKEY regkey;
	LONG ret;
	DWORD size;
	char value[1024], * lookup_str;
	size_t lookup_size;

	lookup_size = strlen(lookup);
	lookup_str = (char*)malloc(lookup_size + sizeof(char));
	strncpy(lookup_str, lookup, lookup_size + sizeof(char));

	size = sizeof(value);
	ret = RegOpenKeyEx(hKey, regkey_s, 0, KEY_READ, &regkey);
	if (ret == ERROR_SUCCESS)
	{
		ret = RegQueryValueEx(regkey, value_s, NULL, NULL, (BYTE*)value, &size);
		RegCloseKey(regkey);
		if (ret == ERROR_SUCCESS)
		{
			size_t i;
			for (i = 0; i < strlen(value); i++)
			{ /* case-insensitive */
				value[i] = toupper(value[i]);
			}
			for (i = 0; i < lookup_size; i++) { /* case-insensitive */
				lookup_str[i] = toupper(lookup_str[i]);
			}
			if (strstr(value, lookup_str) != NULL)
			{
				free(lookup_str);
				return TRUE;
			}
		}
	}
	free(lookup_str);
	return FALSE;
}

// IsInsideVPC's exception filter
DWORD __forceinline TEMUKOACSEngine::IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
	PCONTEXT ctx = ep->ContextRecord;

	ctx->Ebx = -1; // Not running VPC
	ctx->Eip += 4; // skip past the "call VPC" opcodes
	return EXCEPTION_CONTINUE_EXECUTION;
	// we can safely resume execution since we skipped faulty instruction
}

// High level language friendly version of IsInsideVPC()
bool TEMUKOACSEngine::IsInsideVPC()
{
	bool rc = false;

	__try
	{
		_asm push ebx
		_asm mov  ebx, 0 // It will stay ZERO if VPC is running
		_asm mov  eax, 1 // VPC function number

		// call VPC 
		_asm __emit 0Fh
		_asm __emit 3Fh
		_asm __emit 07h
		_asm __emit 0Bh

		_asm test ebx, ebx
		_asm setz[rc]
			_asm pop ebx
	}
	// The except block shouldn't get triggered if VPC is running!!
	__except (IsInsideVPC_exceptionFilter(GetExceptionInformation()))
	{
	}

	return rc;
}


bool TEMUKOACSEngine::detectSandbox()
{
	char* pch;
	HWND snd;

	if ((snd = FindWindow("SandboxieControlWndClass", NULL)))
	{
		g_bRunning = VIRTUAL_MACHINE;
		OnLog(true, "Sandbox was detected .... FAIL! (sm-6s)");
		return false;
	}
	return true;
}

char* sModule[] = { "SbieDll.dll" };

bool TEMUKOACSEngine::IsInSandbox()
{
	for (int i = 0; i < (sizeof(sModule) / sizeof(char*)); i++)
	{
		if (GetModuleHandle(sModule[i]))
		{
			g_bRunning = VIRTUAL_MACHINE;
			OnLog(true, "Sandbox was detected .... FAIL! (sb-23)");
		}
	}
	return false;
}

void TEMUKOACSEngine::sidt()
{
	unsigned char	idtr[6];
	unsigned long	idt = 0;

	_asm sidt idtr
	idt = *((unsigned long*)&idtr[2]);

	if ((idt >> 24) == 0xff)
	{
		g_bRunning = VIRTUAL_MACHINE;
		OnLog(true, "Virtual Machine was detected .... FAIL! (sm-63)");
	}
}

void TEMUKOACSEngine::IsUsingVirtualBox()
{
	//method 9
	unsigned long pnsize = 0x1000;
	char* provider = (char*)LocalAlloc(LMEM_ZEROINIT, pnsize);
	int retv = WNetGetProviderName(WNNC_NET_RDR2SAMPLE, provider, &pnsize);
	if (retv == NO_ERROR)
	{
		if (lstrcmpi(provider, "VirtualBox Shared Folders") == 0)
		{
			g_bRunning = VIRTUAL_MACHINE;
			OnLog(true, "Virtualbox was detected .... FAIL! (vb-22)");
		}
	}
}

void TEMUKOACSEngine::smsw()
{
	unsigned int reax = 0;

	__asm
	{
		mov eax, 0xCCCCCCCC;
		smsw eax;
		mov DWORD PTR[reax], eax;
	}

	if ((((reax >> 24) & 0xFF) == 0xcc) && (((reax >> 16) & 0xFF) == 0xcc))	{
		g_bRunning = VIRTUAL_MACHINE;
		OnLog(true, "Virtualmachine was detected ....FAIL! (vm-21)");
	}
}