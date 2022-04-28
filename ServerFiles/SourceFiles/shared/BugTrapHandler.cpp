#include "stdafx.h"
#include "BugTrapHandler.h"

CBugTrap::CBugTrap() : g_LibraryBugTrap(nullptr)
{
	InitBugTrap();
}

bool CBugTrap::InitBugTrap(void)
{
	BT_SETAPPNAME lpfnBT_SetAppName;
	BT_SETAPPVERSION lpfnBT_SetAppVersion;
	BT_SETFLAGS lpfnBT_SetFlags;
	BT_SETSUPPORTURL lpfnBT_SetSupportURL;
	BT_SETSUPPORTEMAIL lpfnBT_SetSupportEMail;
	BT_SETSUPPORTSERVER lpfnBT_SetSupportServer;

#ifndef _WIN64
	g_LibraryBugTrap = LoadLibrary("BugTrap.dll");
#else
	g_LibraryBugTrap = LoadLibrary("BugTrap-x64.dll");
#endif

	lpfnBT_SetAppName = (BT_SETAPPNAME)GetProcAddress(g_LibraryBugTrap, "BT_SetAppName");
	lpfnBT_SetAppVersion = (BT_SETAPPVERSION)GetProcAddress(g_LibraryBugTrap, "BT_SetAppVersion");
	lpfnBT_SetFlags = (BT_SETFLAGS)GetProcAddress(g_LibraryBugTrap, "BT_SetFlags");
	lpfnBT_SetSupportURL = (BT_SETSUPPORTURL)GetProcAddress(g_LibraryBugTrap, "BT_SetSupportURL");
	lpfnBT_SetSupportEMail = (BT_SETSUPPORTURL)GetProcAddress(g_LibraryBugTrap, "BT_SetSupportEMail");
	lpfnBT_SetSupportServer = (BT_SETSUPPORTSERVER)GetProcAddress(g_LibraryBugTrap, "BT_SetSupportServer");

	if (g_LibraryBugTrap)
	{
		lpfnBT_SetAppName(TEXT("System BugsTrap"));
		lpfnBT_SetAppVersion(TEXT(VERSIONSTRING));
		lpfnBT_SetFlags(BugTrapFlags::DetailedMode | BugTrapFlags::EditMail | BugTrapFlags::AttackhReport | BugTrapFlags::ScreenCapture);
		lpfnBT_SetSupportURL(TEXT("http://knightonlinetr.com/"));
		lpfnBT_SetSupportEMail(TEXT("merx06@outlook.com"));
		lpfnBT_SetSupportServer(TEXT("localhost"), 9999);

		return true;
	}

	return false;
}

void CBugTrap::ShutdownBugTrap(void)
{
	if (g_LibraryBugTrap)
		FreeLibrary(g_LibraryBugTrap);
}

bool CBugTrap::IsBugTrapLoaded(void)
{
	return !!g_LibraryBugTrap;
}

CBugTrap::~CBugTrap()
{
	ShutdownBugTrap();
}