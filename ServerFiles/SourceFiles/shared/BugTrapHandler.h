#pragma once

#define VERSIONSTRING "v2.223"

enum BugTrapFlags
{
	DetailedMode = 0x01,
	EditMail = 0x02,
	AttackhReport = 0x04,
	ScreenCapture = 0x020
};

class CBugTrap
{
public:
	CBugTrap();

	HMODULE g_LibraryBugTrap;

	typedef void (APIENTRY * BT_SETAPPNAME)(LPCTSTR pszAppName);
	typedef void (APIENTRY * BT_SETAPPVERSION)(LPCTSTR pszAppVersion);
	typedef void (APIENTRY * BT_SETFLAGS)(DWORD dwFlags);
	typedef void (APIENTRY * BT_SETSUPPORTURL)(LPCTSTR pszSupportURL);
	typedef void (APIENTRY * BT_SETSUPPORTEMAIL)(LPCTSTR pszSupportEMAIL);
	typedef void (APIENTRY * BT_SETSUPPORTSERVER)(LPCTSTR pszSupportHost, SHORT nSupportPort);

	bool InitBugTrap(void);
	void ShutdownBugTrap(void);
	bool IsBugTrapLoaded(void);

	~CBugTrap();
};

