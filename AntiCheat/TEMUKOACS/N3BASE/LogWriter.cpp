

#include <stdio.h>
#include "N3Base.h"
#include "LogWriter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

std::string CLogWriter::s_szFileName = "";

CLogWriter::CLogWriter()
{
}

CLogWriter::~CLogWriter()
{
}

void CLogWriter::Open(const std::string& szFN)
{

	if(szFN.empty()) return;

	s_szFileName = szFN;

	HANDLE hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hFile) return;
	}

	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if(dwSizeLow > 256000)  
	{
		CloseHandle(hFile);
		::DeleteFileA(s_szFileName.c_str());
		hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hFile) return;
	}

	::SetFilePointer(hFile, 0, NULL, FILE_END); 

	char szBuff[1024];
	SYSTEMTIME time;
	GetLocalTime(&time);
	DWORD dwRWC = 0;

	sprintf(szBuff, "---------------------------------------------------------------------------\r\n");
	int iLength = lstrlenA(szBuff);
	WriteFile(hFile, szBuff, iLength, &dwRWC, NULL);

	sprintf(szBuff, "// Begin writing log... [%.2d/%.2d %.2d:%.2d]\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
	iLength = lstrlenA(szBuff);
	WriteFile(hFile, szBuff, iLength, &dwRWC, NULL);

	CloseHandle(hFile);
}

void CLogWriter::Close()
{
	HANDLE hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hFile) hFile = NULL;
	}

	if(hFile)
	{
		::SetFilePointer(hFile, 0, NULL, FILE_END); 

		char szBuff[1024];
		SYSTEMTIME time;
		GetLocalTime(&time);
		DWORD dwRWC = 0;

		sprintf(szBuff, "// End writing log... [%.2d/%.2d %.2d:%.2d]\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
		int iLength = lstrlenA(szBuff);
		WriteFile(hFile, szBuff, iLength, &dwRWC, NULL);

		sprintf(szBuff, "---------------------------------------------------------------------------\r\n");
		iLength = lstrlenA(szBuff);
		WriteFile(hFile, szBuff, iLength, &dwRWC, NULL);

		CloseHandle(hFile);
		hFile = NULL;
	}
}

void CLogWriter::Write(const char *lpszFormat, ...)
{
	if(s_szFileName.empty() || NULL == lpszFormat) return;

	static char szFinal[1024];
	static SYSTEMTIME time;
	GetLocalTime(&time);
	szFinal[0] = NULL;

	DWORD dwRWC = 0;
	sprintf(szFinal, "    [%.2d:%.2d:%.2d] ", time.wHour, time.wMinute, time.wSecond);

	static char szBuff[1024];
	szBuff[0] = NULL;
	va_list argList;
	va_start(argList, lpszFormat);
	vsprintf(szBuff, lpszFormat, argList);
	va_end(argList);

	lstrcatA(szFinal, szBuff);
	lstrcatA(szFinal, "\r\n");
	int iLength = lstrlenA(szFinal);

	HANDLE hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFileA(s_szFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hFile) hFile = NULL;
	}

	if(hFile)
	{
		::SetFilePointer(hFile, 0, NULL, FILE_END); 

		WriteFile(hFile, szFinal, iLength, &dwRWC, NULL);
		CloseHandle(hFile);
	}
}
