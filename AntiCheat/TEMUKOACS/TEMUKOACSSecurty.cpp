#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "MD6.h"
#include "SecurtyHandler.h"
#include <assert.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <sddl.h>
#include <stdlib.h>

extern bool HideThread(HANDLE hThread);
extern bool CheckProcessDebugFlags();
extern void AntiDebugWithTLS();
extern DWORD WINAPI GetThreadStartAddress(HANDLE hThread);
#define MAKE_JUMP_OFFSET(_BaseOffset, _TargetOffset) ((LONG)(_TargetOffset) - (LONG)(_BaseOffset) - 5)
typedef int (WINAPI* WINSTATERMINATEPROC)(HANDLE hServer, DWORD dwProcessId, UINT uExitCode);

// HMODULE For System DLL
HMODULE	hNtdll;
HMODULE hWinsta;

void TEMUKOACSEngine::OnLog(bool s_sSending, std::string logmsg, ...)
{
	time_t raw;
	time(&raw);

	std::string result;
	va_list ap;

	va_start(ap, logmsg);
	tstring_format(logmsg, &result, ap);
	va_end(ap);

	ofstream out("TEMUKOACS\\engine_log.txt", ios::app);
	std::string t(ctime(&raw));
	out << t.substr(0, t.length() - 1);
	out << "  | " << result << "\n", out;
	out.close();

	if (s_sSending)
		Engine->Shutdown(logmsg);
}

void TEMUKOACSEngine::ShowDbgMsg(DEBUG_LEVEL type, std::string msg, ...)
{
#ifdef __DEBUG

	//m_console_lock.lock();
	std::string result;
	va_list ap;

	va_start(ap, msg);
	tstring_format(msg, &result, ap);
	va_end(ap);
	if (type == DEBUG_LEVEL::DBG_NORMAL)
	{
		//std::cout << "[NORMAL] "<< result << endl;
		printf("[NORMAL] ");
	}
	else if (type == DEBUG_LEVEL::DBG_WARNING)
	{
		//std::cout << "[WARNING] "<< result << endl;
		printf("[WARNING] ");
	}
	else if (type == DEBUG_LEVEL::DBG_ERROR)
	{
		//std::cout << "[ERROR] " << result << endl;
		printf("[ERROR] ");
	}
	else if (type == DEBUG_LEVEL::DBG_SYS)
	{
		//std::cout << "[NOTICE] " << result << endl;
		printf("[SYSTEM] ");
	}
	printf(result.c_str());
	printf("\n");
	//m_console_lock.unlock();
#endif
}

void WINAPI NewDbgUiRemoteBreakin(void)
{	
	hWinsta = LoadLibrary(TEXT("winsta.dll"));

	WINSTATERMINATEPROC WinStationTerminateProcess = (WINSTATERMINATEPROC)GetProcAddress(hWinsta, "WinStationTerminateProcess");

	WinStationTerminateProcess(NULL, GetCurrentProcessId(), DBG_TERMINATE_PROCESS);

	g_bRunning = DEBUGGER;
	Engine->OnLog(true, "Debugging tespit edildi!! (ht-42)");
}

BOOL TEMUKOACSEngine::HardwareBreakpoints_GetThreadContext()
{
	/* Hardware breakpoints are a technology implemented by Intel in their processor architecture,
	and are controlled by the use of special registers known as Dr0-Dr7.
	Dr0 through Dr3 are 32 bit registers that hold the address of the breakpoint */

	unsigned int NumBps = 0;

	// This structure is key to the function and is the 
	// medium for detection and removal
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT));

	// The CONTEXT structure is an in/out parameter therefore we have
	// to set the flags so Get/SetThreadContext knows what to set or get.
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

	// Get a handle to our thread
	HANDLE hThread = GetCurrentThread();

	// Get the registers
	if (GetThreadContext(hThread, &ctx) == 0)
		return -1;

	// Now we can check for hardware breakpoints, its not 
	// necessary to check Dr6 and Dr7, however feel free to
	if (ctx.Dr0 != 0)
		++NumBps;
	if (ctx.Dr1 != 0)
		++NumBps;
	if (ctx.Dr2 != 0)
		++NumBps;
	if (ctx.Dr3 != 0)
		++NumBps;

	if (NumBps > 0)
		return TRUE;
	else
		return FALSE;
}

int TEMUKOACSEngine::Anti_Debugging()
{
	DWORD	oldProtect;
	PVOID	pDbgUiRemoteBreakin = NULL;

	hNtdll = GetModuleHandle("ntdll.DLL");
	if (hNtdll == NULL)	return -11;

	PVOID	pNewDbgUiRemoteBreakin = NewDbgUiRemoteBreakin;
	BYTE	Buffer[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

	pDbgUiRemoteBreakin = GetProcAddress(hNtdll, "DbgUiRemoteBreakin");

	if (!pDbgUiRemoteBreakin) return -12;
	else
	{
		//4byte 
		*(LONG*)(Buffer + 1) = MAKE_JUMP_OFFSET(pDbgUiRemoteBreakin, pNewDbgUiRemoteBreakin);   //Offset °è»ê 


		// page 
		VirtualProtect(pDbgUiRemoteBreakin, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

		//DbgUiRemoteBreakin overwrite
		RtlMoveMemory(pDbgUiRemoteBreakin, Buffer, 5);

		//
		VirtualProtect(pDbgUiRemoteBreakin, 5, oldProtect, &oldProtect);
	}

	return 0;

}

void TEMUKOACSEngine::olly_crash()
{
	char MOVX = 0;
	__asm
	{
		mov eax, fs: [30h]
		mov al, [eax + 2h]
		mov MOVX, al
	}

	if (MOVX)
	{

		OutputDebugString(TEXT("%s%s%s%s%s%s%s%s%s%s%s")
			TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
			TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
			TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s"));
	}
}

bool TEMUKOACSEngine::InstallAntiAttach()
{
	HMODULE ntdll;                   // ntdll handle
	void* pDbgUiRemoteBreakin;       // function handle
	DWORD dwOldProtect;              // just for fun
	DWORD dwCodeSize;                // Size of code to copy

	// Get ntdll.dll handle
	ntdll = GetModuleHandle("ntdll.dll");
	if (ntdll)
	{
		// Get target function addr
		pDbgUiRemoteBreakin = GetProcAddress(ntdll, "DbgUiRemoteBreakin");
		if (pDbgUiRemoteBreakin)
		{
			__asm
			{
				// Get code size
				lea eax, __CodeToCopyStart
				lea ecx, __CodeToCopyEnd
				sub ecx, eax
				mov dwCodeSize, ecx
			}
			// Make sure that we have write rights ...
			if (VirtualProtect(pDbgUiRemoteBreakin, dwCodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			{
				__asm
				{
					// Copy code between __CodeToCopyStart and __CodeToCopyEnd
					mov edi, pDbgUiRemoteBreakin
					lea esi, __CodeToCopyStart
					mov ecx, dwCodeSize
					rep movsb
					// Skip code
					jmp __CodeEnd

					__CodeToCopyStart :
					lea eax, __CodeToCopyEnd
						jmp eax
						__CodeToCopyEnd :
				}

				// ***CODE*HERE***
				__asm
				{
					// Clear registers
					xor eax, eax
					pushfd
					mov[esp], eax
					popfd
					xor ebx, ebx
					xor ecx, ecx
					xor edx, edx
					xor esi, esi
					xor edi, edi
					xor esp, esp
					xor ebp, ebp
					// Jump to address 0
					jmp eax
				}
				// ***************

			__CodeEnd:;
				return true;
			}
		}
	}
	return false;
}

bool TEMUKOACSEngine::CreateThreadSelf(LPTHREAD_START_ROUTINE callbackFunction, LPVOID lpParameter, uint32 id)
{
	Handle_Thread t;

	t.Id = id;
	t.handle = CreateThread(NULL, NULL, callbackFunction, lpParameter, 0, 0);
	t.startAddr = GetThreadStartAddress(t.handle);
	t.updateTime = time(nullptr);

	threadArray.insert(std::make_pair(threadArray.size(), t));

	return true;
}

void TEMUKOACSEngine::Hardware_breakpointx()
{
	CONTEXT cx;
	RtlZeroMemory(&cx, sizeof(cx)); // zero clear
	cx.ContextFlags = CONTEXT_FULL;
	GetThreadContext((HANDLE)-2, &cx);
	if (cx.Dr0 || cx.Dr1 || cx.Dr2 || cx.Dr3 || cx.Dr6 || cx.Dr7)
	{
		g_bRunning = BYPASS;
		OnLog(true, "Hardware breakpoint tespit edildi(x-4)");
	}
}

DWORD TEMUKOACSEngine::ProtectProcess()
{
	// Returned to caller
	DWORD dwResult = (DWORD)-1;

	// Released on exit
	HANDLE hToken = NULL;
	PVOID pTokenInfo = NULL;

	PSID psidEveryone = NULL;
	PSID psidSystem = NULL;
	PSID psidAdmins = NULL;

	PACL pDacl = NULL;
	PSECURITY_DESCRIPTOR pSecDesc = NULL;

	__try
	{
		// Scratch
		DWORD dwSize = 0;
		BOOL bResult = FALSE;

		// If this fails, you can try to fallback to OpenThreadToken
		if (!OpenProcessToken(n_TEMUKOACS, TOKEN_READ, &hToken))
		{
			dwResult = GetLastError();
			//assert(FALSE);
			__leave; /*failed*/
		}

		bResult = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
		dwResult = GetLastError();
		//assert(bResult == FALSE && ERROR_INSUFFICIENT_BUFFER == dwResult);
		if (!(bResult == FALSE && ERROR_INSUFFICIENT_BUFFER == dwResult)) { __leave; /*failed*/ }

		if (dwSize)
		{
			pTokenInfo = HeapAlloc(GetProcessHeap(), 0, dwSize);
			dwResult = GetLastError();
			//assert(NULL != pTokenInfo);
			if (NULL == pTokenInfo) { __leave; /*failed*/ }
		}

		bResult = GetTokenInformation(hToken, TokenUser, pTokenInfo, dwSize, &dwSize);
		dwResult = GetLastError();
		//assert(bResult && pTokenInfo);
		if (!(bResult && pTokenInfo)) { __leave; /*failed*/ }

		PSID psidCurUser = ((TOKEN_USER*)pTokenInfo)->User.Sid;

		SID_IDENTIFIER_AUTHORITY sidEveryone = SECURITY_WORLD_SID_AUTHORITY;
		bResult = AllocateAndInitializeSid(&sidEveryone, 1,
			SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone);
		dwResult = GetLastError();
		//assert(bResult && psidEveryone);
		if (!(bResult && psidEveryone)) { __leave; /*failed*/ }

		SID_IDENTIFIER_AUTHORITY sidSystem = SECURITY_NT_AUTHORITY;
		bResult = AllocateAndInitializeSid(&sidSystem, 1,
			SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0, &psidSystem);
		dwResult = GetLastError();
		//assert(bResult && psidSystem);
		if (!(bResult && psidSystem)) { __leave; /*failed*/ }

		SID_IDENTIFIER_AUTHORITY sidAdministrators = SECURITY_NT_AUTHORITY;
		bResult = AllocateAndInitializeSid(&sidAdministrators, 2,
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, &psidAdmins);
		dwResult = GetLastError();
		//assert(bResult && psidAdmins);
		if (!(bResult && psidAdmins)) { __leave; /*failed*/ }

		const PSID psidArray[] = {
			psidEveryone,    /* Deny most rights to everyone */
			psidCurUser,    /* Allow what was not denied */
			psidSystem,        /* Full control */
			psidAdmins,        /* Full control */
		};

		// Determine required size of the ACL
		dwSize = sizeof(ACL);

		// First the DENY, then the ALLOW
		dwSize += GetLengthSid(psidArray[0]);
		dwSize += sizeof(ACCESS_DENIED_ACE) - sizeof(DWORD);

		for (UINT i = 1; i < _countof(psidArray); i++) {
			// DWORD is the SidStart field, which is not used for absolute format
			dwSize += GetLengthSid(psidArray[i]);
			dwSize += sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);
		}

		pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwSize);
		dwResult = GetLastError();
		//assert(NULL != pDacl);
		if (NULL == pDacl) { __leave; /*failed*/ }

		bResult = InitializeAcl(pDacl, dwSize, ACL_REVISION);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		// Mimic Protected Process
		// http://www.microsoft.com/whdc/system/vista/process_vista.mspx
		// Protected processes allow PROCESS_TERMINATE, which is
		// probably not appropriate for high integrity software.
		static const DWORD dwPoison =
			/*READ_CONTROL |*/ WRITE_DAC | WRITE_OWNER |
			PROCESS_CREATE_PROCESS | PROCESS_CREATE_THREAD |
			PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION |
			PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION |
			PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE |
			// In addition to protected process
			PROCESS_SUSPEND_RESUME | PROCESS_TERMINATE;
		bResult = AddAccessDeniedAce(pDacl, ACL_REVISION, dwPoison, psidArray[0]);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		// Standard and specific rights not explicitly denied
		static const DWORD dwAllowed = ~dwPoison & 0x1FFF;
		bResult = AddAccessAllowedAce(pDacl, ACL_REVISION, dwAllowed, psidArray[1]);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		// Because of ACE ordering, System will effectively have dwAllowed even
		// though the ACE specifies PROCESS_ALL_ACCESS (unless software uses
		// SeDebugPrivilege or SeTcbName and increases access).
		// As an exercise, check behavior of tools such as Process Explorer under XP,
		// Vista, and above. Vista and above should exhibit slightly different behavior
		// due to Restricted tokens.
		bResult = AddAccessAllowedAce(pDacl, ACL_REVISION, PROCESS_ALL_ACCESS, psidArray[2]);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		// Because of ACE ordering, Administrators will effectively have dwAllowed
		// even though the ACE specifies PROCESS_ALL_ACCESS (unless the Administrator
		// invokes 'discretionary security' by taking ownership and increasing access).
		// As an exercise, check behavior of tools such as Process Explorer under XP,
		// Vista, and above. Vista and above should exhibit slightly different behavior
		// due to Restricted tokens.
		bResult = AddAccessAllowedAce(pDacl, ACL_REVISION, PROCESS_ALL_ACCESS, psidArray[3]);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		pSecDesc = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), 0, SECURITY_DESCRIPTOR_MIN_LENGTH);
		dwResult = GetLastError();
		//assert(NULL != pSecDesc);
		if (NULL == pSecDesc) { __leave; /*failed*/ }

		// InitializeSecurityDescriptor initializes a security descriptor in
		// absolute format, rather than self-relative format. See
		// http://msdn.microsoft.com/en-us/library/aa378863(VS.85).aspx
		bResult = InitializeSecurityDescriptor(pSecDesc, SECURITY_DESCRIPTOR_REVISION);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		bResult = SetSecurityDescriptorDacl(pSecDesc, TRUE, pDacl, FALSE);
		dwResult = GetLastError();
		//assert(TRUE == bResult);
		if (FALSE == bResult) { __leave; /*failed*/ }

		dwResult = SetSecurityInfo(
			n_TEMUKOACS,
			SE_KERNEL_OBJECT, // process object
			OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
			psidCurUser, // NULL, // Owner SID
			NULL, // Group SID
			pDacl,
			NULL // SACL
		);
		dwResult = GetLastError();
		//assert(ERROR_SUCCESS == dwResult);
		if (ERROR_SUCCESS != dwResult) { __leave; /*failed*/ }

		dwResult = ERROR_SUCCESS;
	}
	__finally
	{
		if (NULL != pSecDesc) {
			HeapFree(GetProcessHeap(), 0, pSecDesc);
			pSecDesc = NULL;
		}
		if (NULL != pDacl) {
			HeapFree(GetProcessHeap(), 0, pDacl);
			pDacl = NULL;
		}
		if (psidAdmins) {
			FreeSid(psidAdmins);
			psidAdmins = NULL;
		}
		if (psidSystem) {
			FreeSid(psidSystem);
			psidSystem = NULL;
		}
		if (psidEveryone) {
			FreeSid(psidEveryone);
			psidEveryone = NULL;
		}
		if (NULL != pTokenInfo) {
			HeapFree(GetProcessHeap(), 0, pTokenInfo);
			pTokenInfo = NULL;
		}
		if (NULL != hToken) {
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return dwResult;
}

void TEMUKOACSEngine::Protection_Starter()
{
	DWORD ret = ProtectProcess();

	if (ret != ERROR_SUCCESS)
		ShowDbgMsg(DEBUG_LEVEL::DBG_ERROR, "Unable to protect process .... FAIL!");
	else if (ret == ERROR_SUCCESS)
		ShowDbgMsg(DEBUG_LEVEL::DBG_SYS, "Process protection enabled .... OK!");
	else
		ShowDbgMsg(DEBUG_LEVEL::DBG_WARNING, "ERROR. Unable to define the state .... WARNING!");
}

void TEMUKOACSEngine::SizeOfImage()
{
	// Any unreasonably large value will work say for example 0x100000 or 100,000h
	__asm
	{
		mov eax, fs: [0x30]				// PEB
		mov eax, [eax + 0x0c]			 // PEB_LDR_DATA
		mov eax, [eax + 0x0c]			// InOrderModuleList
		mov dword ptr[eax + 20h], 20000h // SizeOfImage    
	}
}

bool TEMUKOACSEngine::AdjustSingleTokenPrivilege(HANDLE TokenHandle, LPCTSTR lpName, DWORD dwAttributes)
{
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = dwAttributes;
	if (!LookupPrivilegeValue(NULL, lpName, &(tp.Privileges[0].Luid)))
		return false;
	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &tp, 0, NULL, NULL))
		return false;
	return true;
}

bool TEMUKOACSEngine::DisableDebugPrivileges(DWORD dwPID)
{
	if (!dwPID)
		dwPID = GetCurrentProcessId();
	HANDLE hProcess = NULL;
	if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwPID)) == INVALID_HANDLE_VALUE)
		return false;
	HANDLE hToken = NULL;
	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false;
	if (!AdjustSingleTokenPrivilege(hToken, SE_SECURITY_NAME, SE_PRIVILEGE_REMOVED) ||
		!AdjustSingleTokenPrivilege(hToken, SE_DEBUG_NAME, SE_PRIVILEGE_REMOVED))
		return false;
	CloseHandle(hToken);
	CloseHandle(hProcess);
	return true;
}

int TEMUKOACSEngine::SetDebugPriv()
{
	HANDLE TokenHandle;
	LUID lpLuid;
	TOKEN_PRIVILEGES NewState;
	if (!OpenProcessToken(n_TEMUKOACS, TOKEN_ALL_ACCESS, &TokenHandle))
		return 0;

	if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &lpLuid))
	{
		OnLog(true, "New SetDebugPriv1");
		CloseHandle(TokenHandle);
		return 0;
	}
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = lpLuid;
	NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &NewState, sizeof(NewState), NULL, NULL))
	{
		OnLog(true, "New SetDebugPriv2");
		CloseHandle(TokenHandle);
		return 0;
	}

	CloseHandle(TokenHandle);
	return 1;
}

void TEMUKOACSEngine::anti_dumper()
{
	__asm // Fake Main size (anti dump) our main will be like 1 gb in memory and dumper should crash :D
	{
		MOV EAX, DWORD PTR FS : [0x30]
		MOV EAX, [EAX + 0Ch]
		MOV EAX, [EAX + 0Ch]
		ADD DWORD PTR[EAX + 20h], 3000h
	}
}

void TEMUKOACSEngine::obfuscation()
{
	__asm
	{
		push ss
		pop ss
		mov eax, 9
		xor edx, edx
	}
}

void TEMUKOACSEngine::SecurtyStard()
{
	CheckProcessDebugFlags();
	olly_crash();
	InstallAntiAttach();
	Hardware_breakpointx();
	Anti_Debugging();

	Protection_Starter();
	SizeOfImage();
	DisableDebugPrivileges(GetCurrentProcessId());
	SetDebugPriv();

	//diðer
	obfuscation();
	anti_dumper();

	AntiDebugWithTLS();
	sidt();
	IsUsingVirtualBox();
	smsw();

   CreateScanThreads();
}

void TEMUKOACSEngine::CreateScanThreads()
{
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wInjectScanProc), LPVOID(this), 50454);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wTileScanProc), LPVOID(this), 843913);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wQueryCounterCountProc), LPVOID(this), 932053);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wcheck_driver), LPVOID(this), 17182480);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wchild_check), LPVOID(this), 18842378);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wstyle_check), LPVOID(this), 8505035);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wTerminate_Scan), LPVOID(this), 483854);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wTbl_Scan), LPVOID(this), 594965);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wDump_Scan), LPVOID(this), 605076);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wCheckRunTimeTBLs), LPVOID(this), 54342);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wProcessIntegrityLevelCheck), LPVOID(this), 833245);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wHookChecker), LPVOID(this), 231245);
	CreateThreadSelf(LPTHREAD_START_ROUTINE(wUserCheckThreadProc), LPVOID(this), 53999004);
}

bool TEMUKOACSEngine::CheckTBLs()
{
	bool s_CheckTBLs = false;
	MD6 md6;
	if (GetFileSizeOwn("DATA\\Skill_Magic_1.tbl") != 0x6AAC
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_1.tbl"), "e0d735c79fcb36808d1f096010423f52"))
	{
		OnLog(false, "Skill_Magic_1 %X", GetFileSizeOwn("DATA\\Skill_Magic_1.tbl"));
		OnLog(false, "Skill_Magic_1 %s", md6.digestFile("DATA\\Skill_Magic_1.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_2.tbl") != 0x8C4
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_2.tbl"), "66a3dc7ef27f9dc8a8840a09d6a21935"))
	{
		OnLog(false, "Skill_Magic_2 %X", GetFileSizeOwn("DATA\\Skill_Magic_2.tbl"));
		OnLog(false, "Skill_Magic_2 %s", md6.digestFile("DATA\\Skill_Magic_2.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_3.tbl") != 0x7584
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_3.tbl"), "cfb78bb8065d94d05102c35f3627573b"))
	{
		OnLog(false, "Skill_Magic_3 %X", GetFileSizeOwn("DATA\\Skill_Magic_3.tbl"));
		OnLog(false, "Skill_Magic_3 %s", md6.digestFile("DATA\\Skill_Magic_3.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_4.tbl") != 0x23794
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_4.tbl"), "17a579b8a95a095d0c9b4d8be71d0f2e"))
	{
		OnLog(false, "Skill_Magic_4 %X", GetFileSizeOwn("DATA\\Skill_Magic_4.tbl"));
		OnLog(false, "Skill_Magic_4 %s", md6.digestFile("DATA\\Skill_Magic_4.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_5.tbl") != 0x384
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_5.tbl"), "65ed59a13a7d8e377de695b9d6f03a86"))
	{
		OnLog(false, "Skill_Magic_5 %X", GetFileSizeOwn("DATA\\Skill_Magic_5.tbl"));
		OnLog(false, "Skill_Magic_5 %s", md6.digestFile("DATA\\Skill_Magic_5.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_6.tbl") != 0x79E4
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_6.tbl"), "22ca23a99f76206ac5a891f86fea5689"))
	{
		OnLog(false, "Skill_Magic_6 %X", GetFileSizeOwn("DATA\\Skill_Magic_6.tbl"));
		OnLog(false, "Skill_Magic_6 %s", md6.digestFile("DATA\\Skill_Magic_6.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_7.tbl") != 0x10C
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_7.tbl"), "043bef1ddb4fa472ebaa5866d192908c"))
	{
		OnLog(false, "Skill_Magic_7 %X", GetFileSizeOwn("DATA\\Skill_Magic_7.tbl"));
		OnLog(false, "Skill_Magic_7 %s", md6.digestFile("DATA\\Skill_Magic_7.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_8.tbl") != 0x484
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_8.tbl"), "4ffc26f0c8fb947f58541f335fc86a65"))
	{
		OnLog(false, "Skill_Magic_8 %X", GetFileSizeOwn("DATA\\Skill_Magic_8.tbl"));
		OnLog(false, "Skill_Magic_8 %s", md6.digestFile("DATA\\Skill_Magic_8.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_9.tbl") != 0xEC4
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_9.tbl"), "aa27446eb7f409162492e57416aac033"))
	{
		OnLog(false, "Skill_Magic_9 %X", GetFileSizeOwn("DATA\\Skill_Magic_9.tbl"));
		OnLog(false, "Skill_Magic_9 %s", md6.digestFile("DATA\\Skill_Magic_9.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\Skill_Magic_Main_us.tbl") != 0x9A7EC
		|| strcmp(md6.digestFile("DATA\\Skill_Magic_Main_us.tbl"), "aedc7679589f846a7444b735688e0ada"))
	{
		OnLog(false, "Skill_Magic_Main_us %X", GetFileSizeOwn("DATA\\Skill_Magic_Main_us.tbl"));
		OnLog(false, "Skill_Magic_Main_us %s", md6.digestFile("DATA\\Skill_Magic_Main_us.tbl"));
		s_CheckTBLs = true;
	}

	if (GetFileSizeOwn("DATA\\SkillMagicTable_us.tbl") != 0x822BC
		|| strcmp(md6.digestFile("DATA\\SkillMagicTable_us.tbl"), "b56c3469ba01239ffc765b48ef522883"))
	{
		OnLog(false, "SkillMagicTable_us %X", GetFileSizeOwn("DATA\\SkillMagicTable_us.tbl"));
		OnLog(false, "SkillMagicTable_us %s", md6.digestFile("DATA\\SkillMagicTable_us.tbl"));
		s_CheckTBLs = true;
	}

	if (s_CheckTBLs == true)
		return false;

	return true;
}


bool TEMUKOACSEngine::Securty()
{
	int mMyrand = myrand(1, 3);
	switch (mMyrand)
	{
	case 1:
		Splash = new CSplash(TEXT("TEMUKOACS\\1.bmp"), RGB(128, 128, 128));
		break;
	case 2:
		Splash = new CSplash(TEXT("TEMUKOACS\\2.bmp"), RGB(128, 128, 128));
		break;
	case 3:
		Splash = new CSplash(TEXT("TEMUKOACS\\3.bmp"), RGB(128, 128, 128));
		break;
	default:
		Splash = new CSplash(TEXT("TEMUKOACS\\1.bmp"), RGB(128, 128, 128));
		break;
	}

	Splash->ShowSplash();
	OnLog(false, DBG_SETTINGS_ADMINISTRATOR_OK);

	SetRegister("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "AppInit_DLLs", "", "string");
	SetRegister("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "LoadAppInit_DLLs", 0, "dword");
	SetRegister("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCertDlls", "AppSecDll", "", "string");

	OnLog(false, "TEMUKOACS was started successfully.");

	SecurtyStard();

	foreach(itr, threadArray)
		HideThread(itr->second.handle);
	MD6 md6;
	if (!CheckTBLs()){
		OnLog(true, "TEMUKOACS was started Failed.");
		return false;
	}

	Sleep(5 * SECOND);
	Splash->CloseSplash();
	g_bRunning = INITIATE;
	return true;
}

void TEMUKOACSEngine::SetRegister(LPCSTR yol, LPCTSTR value, LPCSTR deger, char* tip)
{
	HKEY hKey = NULL;
	DWORD dwRet = 0;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, (yol), 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, 0) != ERROR_SUCCESS)
		return;

	if (tip == "dword")
		dwRet = RegSetValueEx(hKey, (value), 0, REG_DWORD, (const BYTE*)&deger, sizeof(deger));
	if (tip == "string")
		dwRet = RegSetValueEx(hKey, (value), 0, REG_SZ, (const BYTE*)deger, lstrlen(deger));

	RegCloseKey(hKey);
}
#define STATUS_SUCCESS              ((NTSTATUS) 0x00000000)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS) 0xC0000004)
#define ThreadQuerySetWin32StartAddress 9
DWORD WINAPI GetThreadStartAddress(HANDLE hThread)
{
	NTSTATUS ntStatus;
	HANDLE hDupHandle;
	DWORD dwStartAddress;

	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread");

	if (NtQueryInformationThread == NULL)
		return 0;

	HANDLE hCurrentProcess = GetCurrentProcess();
	if (!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0))
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return 0;
	}

	ntStatus = NtQueryInformationThread(hDupHandle, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);

	CloseHandle(hDupHandle);

	if (ntStatus != STATUS_SUCCESS)
		return 0;

	return dwStartAddress;
}