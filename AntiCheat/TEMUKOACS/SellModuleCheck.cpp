#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include <TlHelp32.h>

#define ReadByteSize_Module 450

#define MAXLEN 1024
#define BUFSIZE 1024
#define UNLINK(x) (x).Blink->Flink = (x).Flink; (x).Flink->Blink = (x).Blink;

WORD rbuf[MAXLEN];    //cs
int rbuf_seq = 0;
int Size_rbuf = 0;

/*typedef struct _LDR_MODULE_DUMMY_
{
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
} LDR_MODULE, * PLDR_MODULE;

typedef struct _PEB_LDR_DATA_DUMMY_
{
	ULONG Length;
	UCHAR Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct PEB_DUMMY_
{
	PVOID Dummy1;
	PVOID Dummy2;
	PVOID Dummy3;
	PPEB_LDR_DATA LDR_Data;
} PEB_DUMMY, * PPEB_DUMMY;
*/
PPEB_DUMMY __declspec(naked) GetPEBx(void)
{
	_asm
	{
		mov eax, fs: [0x30] ;
		retn;
	}
}

void TEMUKOACSEngine::AntiHeaders(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;
	_asm
	{
		pushad;
		pushfd;
		mov eax, fs: [30h]
			mov eax, [eax + 0Ch]
			mov dwPEB_LDR_DATA, eax

			InLoadOrderModuleList :
		mov esi, [eax + 0Ch]
			mov edx, [eax + 10h]

			LoopInLoadOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 18h]
			cmp ecx, hModule
			jne SkipA
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InMemoryOrderModuleList
			SkipA :
		cmp edx, esi
			jne LoopInLoadOrderModuleList

			InMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 14h]
			mov edx, [eax + 18h]

			LoopInMemoryOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 10h]
			cmp ecx, hModule
			jne SkipB
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp InInitializationOrderModuleList
			SkipB :
		cmp edx, esi
			jne LoopInMemoryOrderModuleList

			InInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 1Ch]
			mov edx, [eax + 20h]

			LoopInInitializationOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 08h]
			cmp ecx, hModule
			jne SkipC
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp Finished
			SkipC :
		cmp edx, esi
			jne LoopInInitializationOrderModuleList

			Finished :
		popfd;
		popad;
	}
}

void TEMUKOACSEngine::HideModule(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;
	_asm
	{
		pushad;
		pushfd;
		mov eax, fs: [30h] ;
		mov eax, [eax + 0Ch];
		mov dwPEB_LDR_DATA, eax;
		mov esi, [eax + 0Ch];
		mov edx, [eax + 10h];
	LoopInLoadOrderModuleList:
		lodsd;
		mov esi, eax;
		mov ecx, [eax + 18h];
		cmp ecx, hModule;
		jne SkipA
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp jInMemoryOrderModuleList
			SkipA :
		cmp edx, esi
			jne LoopInLoadOrderModuleList
			jInMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 14h]
			mov edx, [eax + 18h]
			LoopInMemoryOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 10h]
			cmp ecx, hModule
			jne SkipB
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp jInInitializationOrderModuleList
			SkipB :
		cmp edx, esi
			jne LoopInMemoryOrderModuleList
			jInInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA
			mov esi, [eax + 1Ch]
			mov edx, [eax + 20h]
			LoopInInitializationOrderModuleList :
			lodsd
			mov esi, eax
			mov ecx, [eax + 08h]
			cmp ecx, hModule
			jne SkipC
			mov ebx, [eax]
			mov ecx, [eax + 4]
			mov[ecx], ebx
			mov[ebx + 4], ecx
			jmp Finished
			SkipC :
		cmp edx, esi
			jne LoopInInitializationOrderModuleList
			Finished :
		popfd;
		popad;
	}
}


void TEMUKOACSEngine::EraseHeaders(HINSTANCE hModule)
{
	/*
	* just a func to erase headers by Croner.
	* keep in mind you wont be able to load
	* any resources after you erase headers.
	*/
	PIMAGE_DOS_HEADER pDoH;
	PIMAGE_NT_HEADERS pNtH;
	DWORD i, ersize, protect;
	if (!hModule) return;

	// well just to make clear what we doing 
	pDoH = (PIMAGE_DOS_HEADER)(hModule);
	pNtH = (PIMAGE_NT_HEADERS)((LONG)hModule + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);
	ersize = sizeof(IMAGE_DOS_HEADER);
	if (VirtualProtect(pDoH, ersize, PAGE_READWRITE, &protect))
	{
		for (i = 0; i < ersize; i++)
			*(BYTE*)((BYTE*)pDoH + i) = 0;
	}
	ersize = sizeof(IMAGE_NT_HEADERS);
	if (pNtH && VirtualProtect(pNtH, ersize, PAGE_READWRITE, &protect))
	{
		for (i = 0; i < ersize; i++)
			*(BYTE*)((BYTE*)pNtH + i) = 0;
	}
	return;
}


PVOID TEMUKOACSEngine::AntiRevers(HMODULE dwModule)
{
	PVOID pEntry = NULL;
	PIMAGE_DOS_HEADER pId = (PIMAGE_DOS_HEADER)dwModule;
	PIMAGE_NT_HEADERS pInt = (PIMAGE_NT_HEADERS)(dwModule + pId->e_lfanew);
	pEntry = dwModule + pInt->OptionalHeader.BaseOfCode;
	return pEntry;
}

void TEMUKOACSEngine::RemovePeHeader(DWORD ModuleBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ModuleBase;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return;

	if (pNTHeader->FileHeader.SizeOfOptionalHeader)
	{
		DWORD Protect;
		WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
		VirtualProtect((void*)ModuleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
		RtlZeroMemory((void*)ModuleBase, Size);
		VirtualProtect((void*)ModuleBase, Size, Protect, &Protect);
	}
}


bool TEMUKOACSEngine::CloakDll(HMODULE hMod)
{
	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, * PUNICODE_STRING;


	typedef struct _ModuleInfoNode
	{
		LIST_ENTRY LoadOrder;
		LIST_ENTRY InitOrder;
		LIST_ENTRY MemoryOrder;
		HMODULE baseAddress;
		unsigned long entryPoint;
		unsigned int size;
		UNICODE_STRING fullPath;
		UNICODE_STRING name;
		unsigned long flags;
		unsigned short LoadCount;
		unsigned short TlsIndex;
		LIST_ENTRY HashTable;
		unsigned long timestamp;
	} ModuleInfoNode, * pModuleInfoNode;


	typedef struct _ProcessModuleInfo
	{
		unsigned int size;
		unsigned int initialized;
		HANDLE SsHandle;
		LIST_ENTRY LoadOrder;
		LIST_ENTRY InitOrder;
		LIST_ENTRY MemoryOrder;
	} ProcessModuleInfo, * pProcessModuleInfo;

	ProcessModuleInfo* pmInfo;
	ModuleInfoNode* module;

	_asm
	{
		mov eax, fs: [18h]    // TEB
		mov eax, [eax + 30h]  // PEB
		mov eax, [eax + 0Ch]  // PROCESS_MODULE_INFO
		mov pmInfo, eax
	}

	module = (ModuleInfoNode*)(pmInfo->LoadOrder.Flink);

	while (module->baseAddress && module->baseAddress != hMod)
		module = (ModuleInfoNode*)(module->LoadOrder.Flink);

	if (!module->baseAddress)
		return false;

	//  Remove the module entry from the list here
	///////////////////////////////////////////////////  
	//  Unlink from the load order list
	UNLINK(module->LoadOrder);
	//  Unlink from the init order list
	UNLINK(module->InitOrder);
	//  Unlink from the memory order list
	UNLINK(module->MemoryOrder);
	//  Unlink from the hash table
	UNLINK(module->HashTable);

	//  Erase all traces that it was ever there
	///////////////////////////////////////////////////

	//  This code will pretty much always be optimized into a rep stosb/stosd pair
	//  so it shouldn't cause problems for relocation.
	//  Zero out the module name
	memset(module->fullPath.Buffer, 0, module->fullPath.Length);
	//  Zero out the memory of this module's node
	memset(module, 0, sizeof(ModuleInfoNode));

	return true;
}


void TEMUKOACSEngine::HideModuleFromPEB(HINSTANCE hInstance)
{
	PPEB_DUMMY PEB = GetPEBx();

	PLIST_ENTRY FirstEntryInLoadOrder = (PLIST_ENTRY)&PEB->LDR_Data->InLoadOrderModuleList;
	PLIST_ENTRY FirstEntryInMemoryOrder = (PLIST_ENTRY)&PEB->LDR_Data->InMemoryOrderModuleList;
	PLIST_ENTRY FirstEntryInInitOrder = (PLIST_ENTRY)&PEB->LDR_Data->InInitializationOrderModuleList;

	for (PLIST_ENTRY TempEntry = FirstEntryInLoadOrder->Flink; TempEntry != FirstEntryInLoadOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 0 * sizeof(LIST_ENTRY));
		if (TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}

	for (PLIST_ENTRY TempEntry = FirstEntryInMemoryOrder->Flink; TempEntry != FirstEntryInMemoryOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 1 * sizeof(LIST_ENTRY));
		if (TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}

	for (PLIST_ENTRY TempEntry = FirstEntryInInitOrder->Flink; TempEntry != FirstEntryInInitOrder; TempEntry = TempEntry->Flink)
	{
		PLDR_MODULE TempModule = (PLDR_MODULE)((DWORD)TempEntry - 2 * sizeof(LIST_ENTRY));
		if (TempModule->BaseAddress == hInstance)
		{
			TempEntry->Blink->Flink = TempEntry->Flink;
			TempEntry->Flink->Blink = TempEntry->Blink;
			break;
		}
	}
}
