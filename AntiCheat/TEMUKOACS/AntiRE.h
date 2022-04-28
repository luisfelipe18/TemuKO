#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <winternl.h> 

#if defined( _MSC_VER ) && (_MSC_VER > 1200)
#pragma once
#endif

#pragma pack(1)

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfHandles;
	ULONG TotalNumberOfObjects;
}OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;


typedef struct _OBJECT_ALL_INFORMATION {
	ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
}OBJECT_ALL_INFORMATION, * POBJECT_ALL_INFORMATION;

#pragma pack()

void DebugSelf();
void ErasePEHeaderFromMemory();

bool CheckDbgPresentCloseHandle();
bool CanOpenCsrss();
bool IsParentExplorerExe();
bool HideThread(HANDLE hThread);
bool DebugObjectCheck();
bool CheckProcessDebugFlags();
bool CheckOutputDebugString(LPCTSTR String);
bool Int2DCheck();
bool ObjectListCheck();
bool IsDbgPresentPrefixCheck();

DWORD GetProcessIdFromName(LPCTSTR ProcessName);
DWORD GetCsrssProcessId();
DWORD GetExplorerPIDbyShellWindow();
DWORD GetParentProcessId();