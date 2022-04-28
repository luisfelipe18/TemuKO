#include "stdafx.h"
#include <windows.h>
#include <TlHelp32.h>
#include "TEMUKOACS Engine.h"

void AntiDebug()
{
	typedef unsigned long(__stdcall* pfnNtQueryInformationProcess)(IN  HANDLE, IN  unsigned int, OUT PVOID, IN ULONG, OUT PULONG);
	const int ProcessDbgPort = 7;
	pfnNtQueryInformationProcess NtQueryInfoProcess = NULL;
	unsigned long Ret;
	unsigned long IsRemotePresent = 0;
	HMODULE hNtDll = GetModuleHandle(TEXT("ntdll.dll"));

	if (hNtDll == NULL)
		return;

	NtQueryInfoProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");

	if (NtQueryInfoProcess == NULL)
		return;

	Ret = NtQueryInfoProcess(GetCurrentProcess(), ProcessDbgPort, &IsRemotePresent, sizeof(unsigned long), NULL);
	VIRTUALIZER_START
	if (Ret == 0x00000000 && IsRemotePresent != 0)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debug was detected.");

	}
	VIRTUALIZER_END
}

void DebugControl()
{
	unsigned long Flags = 0;
	__asm
	{
		mov eax, fs: [30h]
		mov eax, [eax + 68h]
		mov Flags, eax
	}

	if (Flags & 0x70)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debugger detected");
	}


	char p = 0;

	__asm
	{
		mov eax, fs: [30h]
		mov al, [eax + 2h]
		mov p, al
	}

	if (p)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debugger detected-x");
	}
}

void isdebugger()
{
	if (IsDebuggerPresent())
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debugger was detected(Error 74)");
	}
}

BOOL IsDebuggerPresentPEB()
{
	/* The IsDebuggerPresent function is actually a wrapper around this code.
	It directly access the PEB for the process and reads a byte value that signifies if the process is being debugged. */

	char IsDbgPresent = 0;
	__asm {
		mov eax, fs: [30h]
		mov al, [eax + 2h]
		mov IsDbgPresent, al
	}

	return IsDbgPresent;
}

void AntiAttachMsg()
{
	g_bRunning = DEBUGGER;
	Engine->OnLog(true, "AntiAttach Debugger detected.");
}

void AntiAttach()
{
	DWORD oldProtect;
	void(*dbgaddr)() = (void(*)()) GetProcAddress(GetModuleHandleA("ntdll.dll"), "DbgUiRemoteBreakin");
	void(*quitaddr)() = AntiAttachMsg;
	char hook[] = "\xb8\x90\x90\x90\x90\xff\xe0";

	memcpy(hook + 1, &quitaddr, sizeof(quitaddr));

	if (VirtualProtect(dbgaddr, sizeof(hook), PAGE_EXECUTE_READWRITE, &oldProtect)) {
		memcpy(dbgaddr, hook, sizeof(hook));
		VirtualProtect(dbgaddr, sizeof(hook), oldProtect, &oldProtect);
	}
}

void fDbgBreakPoint()
{
	DWORD dwOldProtect;
	unsigned char* DbgBreakPoint = (unsigned char*)GetProcAddress(GetModuleHandle(("ntdll.dll")), "DbgBreakPoint");
	VirtualProtect(DbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*DbgBreakPoint = 0x90;
}

void sOutputDebugString_GetLastError()
{
	OutputDebugStringA("Hi Debugger");
	if (GetLastError() == 0)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debugger(x) tespit edildi! (ht-4)");
	}
}

BOOL NtQueryInformationProcess_SystemKernelDebuggerInformation()
{
	/*	This function uses NtQuerySystemInformation using the parameter SystemKernelDebuggerInformation,
	from class SYSTEM_INFORMATION_CLASS. (value: 35)
	It will return 2 boolean values :
	BOOLEAN DebuggerEnabled;
	BOOLEAN DebuggerNotPresent;
	(here defined in _SYSTEM_KERNEL_DEBUGGER_INFORMATION).
	the first one is detecting if debugger is enabled, the second if it's present
	/!\ A debugger can be present and no enabled. */

	typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
		BOOLEAN DebuggerEnabled;
		BOOLEAN DebuggerNotPresent;
	} SYSTEM_KERNEL_DEBUGGER_INFORMATION, * PSYSTEM_KERNEL_DEBUGGER_INFORMATION;
	SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;

	enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 };

	// Function Pointer Typedef for ZwQuerySystemInformation
	typedef NTSTATUS(WINAPI* pZwQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength);

	// We have to import the function
	pZwQuerySystemInformation ZwQuerySystemInformation = NULL;

	// Other Vars
#define STATUS_SUCCESS   ((NTSTATUS)0x00000000L) 
	HANDLE hProcess = GetCurrentProcess();

	HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
	if (hNtDll == NULL)
	{
		// Handle however.. chances of this failing
		// is essentially 0 however since
		// ntdll.dll is a vital system resource
		return false;
	}

	ZwQuerySystemInformation = (pZwQuerySystemInformation)GetProcAddress(hNtDll, "ZwQuerySystemInformation");

	if (ZwQuerySystemInformation == NULL)
	{
		// Handle however it fits your needs but as before,
		// if this is missing there are some SERIOUS issues with the OS
		return false;
	}

	// Time to finally make the call
	if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL))
	{
		if (Info.DebuggerEnabled)
		{
			if (Info.DebuggerNotPresent)
				return FALSE;
			else
				return TRUE;
		}
	}
	return TRUE;
}

BOOL NtSetInformationThread_ThreadHideFromDebugger()
{
	/* Calling NtSetInformationThread will attempt with ThreadInformationClass set to  x11 (ThreadHideFromDebugger)
	to hide a thread from the debugger, Passing NULL for hThread will cause the function to hide the thread the
	function is running in. Also, the function returns false on failure and true on success. When  the  function
	is called, the thread will continue  to run but a debugger will no longer receive any events related  to  that  thread. */

	// Function Pointer Typedef for NtQueryInformationProcess
	typedef NTSTATUS(WINAPI* pNtSetInformationThread)(IN HANDLE, IN UINT, IN PVOID, IN ULONG);

	// ThreadHideFromDebugger
	const int ThreadHideFromDebugger = 0x11;

	// We have to import the function
	pNtSetInformationThread NtSetInformationThread = NULL;

	// Other Vars
	NTSTATUS Status;
	BOOL IsBeingDebug = FALSE;

	HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
	if (hNtDll == NULL)
	{
		// Handle however.. chances of this failing
		// is essentially 0 however since
		// ntdll.dll is a vital system resource
		return false;
	}

	NtSetInformationThread = (pNtSetInformationThread)GetProcAddress(hNtDll, "NtSetInformationThread");

	if (NtSetInformationThread == NULL)
	{
		// Handle however it fits your needs but as before,
		// if this is missing there are some SERIOUS issues with the OS
		return false;
	}

	// Time to finally make the call
	Status = NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, 0);

	if (Status)
		IsBeingDebug = TRUE;

	return IsBeingDebug;
}

BOOL NtGlobalFlag()
{
	/* NtGlobalFlag is a DWORD value inside the process PEB. It contains many flags set by the OS
	that affects the way the process runs When a process is being debugged, the flags
	FLG_HEAP_ENABLE_TAIL_CHECK (0x10), FLG_HEAP_ENABLE_FREE_CHECK(0x20), and FLG_HEAP_VALIDATE_PARAMETERS(0x40)
	are set for the process, and we can use this to our advantage to identify if our process is being debugged. */


	unsigned long NtGlobalFlags = 0;
	__asm
	{
		mov eax, fs: [30h]
		mov eax, [eax + 68h]
		mov NtGlobalFlags, eax
	}

	if (NtGlobalFlags & 0x70)
		return TRUE;
	else
		return FALSE;
}

BOOL HeapFlags()
{
	char IsBeingDbg = FALSE;

	__asm {
		mov eax, FS: [0x30]
		mov eax, [eax + 0x18]; PEB.ProcessHeap
		mov eax, [eax + 0x44]
		cmp eax, 0
		je DebuggerNotFound
		mov IsBeingDbg, 1
		DebuggerNotFound:
	}
	return IsBeingDbg;
}

BOOL ForceFlags()
{
	char IsBeingDbg = 0;

	__asm {
		mov eax, FS: [0x30]
		mov eax, [eax + 0x18]; PEB.ProcessHeap
		mov eax, [eax + 0x40]
		dec eax
		dec eax
		jne DebuggerFound
		jmp ExitMe
		DebuggerFound :
		mov IsBeingDbg, 1
			ExitMe :
	}

	return IsBeingDbg;
}

void AntiDebugYeni()
{
	VIRTUALIZER_START
		if (IsDebuggerPresentPEB())
		{
			g_bRunning = DEBUGGER;
			Engine->OnLog(true, "Detected from : IsDebuggerPresent Using PEB");
			
		}
	if (NtGlobalFlag())
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Detected from : NtGlobalFlags");
		
	}

	if (HeapFlags())
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Detected from : Heap Flags");
		
	}

	if (ForceFlags())
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Detected from : Force Flags");
		
	}

	if (Engine->HardwareBreakpoints_GetThreadContext())
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Hardware Breakpoint detected - GetThreadtcontext");
		
	}
	VIRTUALIZER_END
}

DWORD dwPEBBasedCheck(void)
{
	VIRTUALIZER_START
		char PEBdbgTrue = 0;
	__asm
	{
		xor eax, eax; Nullify eax just as a good programming technique
		mov eax, fs: [30h]
		mov al, [eax + 2h]
		mov PEBdbgTrue, al
	}

	if (PEBdbgTrue)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debugging attemption was detected ....FAIL! (hd-42)");
		

		return 1; /*If return is 1 there IS a debugger present*/
	}

	VIRTUALIZER_END

		return 0; /*If return is 0 there is NO debugger*/
}

DWORD dwRemoteDebugCheck(void)
{
	VIRTUALIZER_START
		BOOL b_IsRmotedbgPresent = false; // Assue Debugger is not present
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &b_IsRmotedbgPresent);
	if (b_IsRmotedbgPresent)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "Debug attempt was detected ....FAIL! (hs-422)");
		
		return 1;
	}
	return 0;
	VIRTUALIZER_END
}

BOOL DetectSoftIce()
{
	BOOL bSoftICE = (CreateFileA("////.//SICE", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0) != INVALID_HANDLE_VALUE); // \\.\SICE
	BOOL bNTICE = (CreateFileA("////.//NTICE", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0) != INVALID_HANDLE_VALUE); // \\.\NTICE
	return bSoftICE || bNTICE;
}

void Anti_softice()
{
	VIRTUALIZER_START
		if (DetectSoftIce())
		{
			g_bRunning = DEBUGGER;
			Engine->OnLog(true, "Debugger(Softice) tespit edildi! (hb-2)");
			

		}
	VIRTUALIZER_END
}

void GetFileNameFromPath(char* szSource)
{
	char* szTemp = strrchr(szSource, '//');
	if (szTemp != NULL)
	{
		szTemp++;
		DWORD l = DWORD(strlen(szTemp)) + 1;
		CopyMemory(szSource, szTemp, l);
	}
}

/*Sometimes, it needs to search for specific process or isolates to specific process.
It was demonstrated how finding parent process and check if it is EXPLORER.EXE and killing all parent process
except Explorer windows in sample source code. There are both C++ and assembly code inside CryptStuff.CPP to illuminate this task.*/
void AntiDebugWithExplorer()
{
	VIRTUALIZER_START

		char lpszSystemInfo[MAX_PATH];
	HANDLE hSnapshot = NULL;
	DWORD PID_child;
	DWORD PID_parent, PID_explorer;
	HANDLE hh_parnet = NULL;
	PROCESSENTRY32    pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);//0x128;
	PID_child = GetCurrentProcessId();//getpid();
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(hSnapshot, &pe32))
	{
		while (Process32Next(hSnapshot, &pe32) && g_bRunning == 1)
		{
			GetFileNameFromPath(pe32.szExeFile);
			CharUpperBuff(pe32.szExeFile, strlen(pe32.szExeFile));

			if (strcmp(pe32.szExeFile, "EXPLORER.EXE") == 0)
				PID_explorer = pe32.th32ProcessID;

			if (pe32.th32ProcessID == PID_child)
				PID_parent = pe32.th32ParentProcessID;
			Sleep(1);
		}
	}
	if (PID_parent != PID_explorer)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "AntiDebugWithExplorer was detected...(ads-0)");
		
	}
	else
	{
		MODULEENTRY32    me32 = { 0 };
		me32.dwSize = sizeof(MODULEENTRY32);
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
			PID_explorer);
		if (Module32First(hSnapshot, &me32))
		{
			do
			{
				if (PID_explorer == me32.th32ProcessID)
				{
					GetWindowsDirectory(lpszSystemInfo,
						MAX_PATH + 1);
					strcat_s(lpszSystemInfo, "//");
					strcat_s(lpszSystemInfo, "EXPLORER.EXE");
					CharUpperBuff(me32.szExePath,
						strlen(me32.szExePath));
					if (strncmp(me32.szExePath,
						lpszSystemInfo,
						strlen(lpszSystemInfo)))
					{
						GetFileNameFromPath(me32.szExePath);
						if (strcmp(me32.szExePath,
							"EXPLORER.EXE") == 0)
						{
							g_bRunning = DEBUGGER;
							Engine->OnLog(true, "AntiDebugWithExplorer2 was detected...(ads-0)");
							
						}
					}
				}
				Sleep(1);
			} while (Module32Next(hSnapshot, &me32) && g_bRunning == 1);
		}
	}
	VIRTUALIZER_END
}

typedef DWORD(WINAPI* ZW_SET_INFORMATION_THREAD)(HANDLE, DWORD, PVOID, ULONG);
#define ThreadHideFromDebugger 17  

void DisableDebugEvent()
{
	ZW_SET_INFORMATION_THREAD ZwSetInformationThread;
	ZwSetInformationThread = (ZW_SET_INFORMATION_THREAD)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwSetInformationThread");
	ZwSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, NULL);
	ZwSetInformationThread((HANDLE)-2L, (DWORD)0x11, NULL, 0);
}


/**
* @brief Checks if any debugging exists.
*/
unsigned __stdcall DebuggingCheck()
{
	int result = 0;
	result = Engine->Anti_Debugging();

	if (result == -11)
		Engine->ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Anti_Debug_GetModuleHandle_ntdll .... Fail!");
	else if (result == -12)
		Engine->ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Anti_Debug_GetProcAddress_DbgUiRemoteBreakin .... Fail!");
	else if (result == -13)
		Engine->ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "Anti_Debug_LoadLibrary_winsta.dll .... Fail!");
	else
		Engine->ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "ANTI-DEBUG is active .... OK!");

	return result;
}

typedef LONG(WINAPI* ZWQUERYINFORMATIONPROCESS)(HANDLE, DWORD, PVOID, ULONG, PULONG);
void DebugPortCheck()
{
	VIRTUALIZER_START

		HMODULE hNtDLL = GetModuleHandle("ntdll.dll");

	ZWQUERYINFORMATIONPROCESS QueryProc = (ZWQUERYINFORMATIONPROCESS)GetProcAddress(hNtDLL, "ZwQueryInformationProcess");

	HANDLE DebugPort = NULL;
	ULONG ReturnLength = 0L;
	if (QueryProc((HANDLE)0xFFFFFFFFL, 7, &DebugPort, 4, &ReturnLength) >= 0)
	{
		if (DebugPort)
		{
			g_bRunning = DEBUGGER;
			Engine->OnLog(true, "DebugPortDebugger was detected...(p-0)");
			
		}
	}

	VIRTUALIZER_END
}

void AntiDebugWithHeap()
{
	VIRTUALIZER_START

		unsigned int var;
	__asm
	{
		MOV EAX, FS: [0x30] ;
		MOV EAX, [EAX + 0x18];
		MOV EAX, [EAX + 0x0c];
		MOV var, EAX
	}

	if (var != 2)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, HEAP_DEBUGGER_DETECTED);
		
	}

	VIRTUALIZER_END
}


DWORD __inline GetBeingDebugged()
{
	DWORD_PTR* BeingDebuggedBit;
	DWORD returnVal;
#ifdef __INTRIN_H_
#ifdef _M_IX86
	BeingDebuggedBit = (DWORD_PTR*)__readfsdword(0x30);
#elif _M_X64
	BeingDebuggedBit = (DWORD_PTR*)__readgsqword(0x60);
#endif
	returnVal = (DWORD)*BeingDebuggedBit ^ 0x00010000;
#else
#ifdef _M_IX86
	__asm {
		MOV EAX, FS: [0x30]
		MOV BeingDebuggedBit, EAX
	};
#endif
	returnVal = (DWORD)*BeingDebuggedBit ^ 0x00010000;
#endif

	return(returnVal);
}

DWORD __inline OutputDebugCheck()
{
	DWORD returnVal;
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&osvi) == 0)
	{
#ifdef AGGRESSIVE
		return(0);
#else
		return(1);
#endif
	}

	if (osvi.dwMajorVersion < 6)
	{
		SetLastError(0x0F0F0F0F);
		OutputDebugString("%s%s");
		if (GetLastError() == 0x0F0F0F0F)
			returnVal = 0;
	}
	else
	{
		try
		{
			OutputDebugString("%s%s");
			returnVal = 1;
		}
		catch (...)
		{
			returnVal = 0;
		}
	}

	return(returnVal);
}

void AntiDebugWithSiberianTiger()
{
	VIRTUALIZER_START
		if (OutputDebugCheck() == 0)
		{
			g_bRunning = DEBUGGER;
			Engine->OnLog(true, "TigerDebugger was detected...(odg-04)");
			
		}

	if (GetBeingDebugged() == 0)
	{
		g_bRunning = DEBUGGER;
		Engine->OnLog(true, "BeingDebugger was detected...(gb-2)");
		
	}

	VIRTUALIZER_END
}

void AntiDebugWithTLS()
{
	PBOOLEAN BeingDebugged = (PBOOLEAN)__readfsdword(0x30) + 2;
	VIRTUALIZER_START
		if (*BeingDebugged)
		{
			if (GetBeingDebugged() == 0)
			{
				g_bRunning = DEBUGGER;
				Engine->OnLog(true, "Debugger was detected...(tl-a)");
			}
		}
	VIRTUALIZER_END
}