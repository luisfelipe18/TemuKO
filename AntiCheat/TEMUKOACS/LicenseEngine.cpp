#include "LicenseEngine.h"

static const string SERVER_INI = xorstr("Server.ini");

DWORD WINAPI LicenseEngine(TEMUKOACSEngine* e)
{
	string Path = e->m_BasePath + SERVER_INI;
	string LicensedIP = SERVER_IP;
	char INICurrentIP[16];
	while (true)
	{
		// licenseengine.cpp ve licenseengine.h Las direcciones IP deben ser las mismas en sus archivos.
		GetPrivateProfileStringA(xorstr("TEST"), xorstr("IP0"), xorstr("127.0.0.1"), INICurrentIP, 16, Path.c_str()); 
		if (LicensedIP != string(INICurrentIP) || (e->uiClanWindowPlug != NULL && LicensedIP != e->m_connectedIP))
		{
			e->power = false;
			exit(0);
			FreeLibrary(GetModuleHandle(NULL));
			TerminateProcess(GetCurrentProcess(), 0);
		}
		Sleep(5000);
	}
}