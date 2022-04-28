#include "stdafx.h"

using namespace std;

void CGameServerDlg::color_c(char* buff, WORD color)
{
	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);


	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, color);
	WriteConsole(hstdout, buff, strlen(buff), new DWORD, 0);
}

void CGameServerDlg::SendConsoleFAIL()
{

	color_c("[ ", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	color_c("FAIL", FOREGROUND_RED + FOREGROUND_INTENSITY);
	color_c(" ]\n", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

}

void CGameServerDlg::SendConsoleOK()
{

	color_c("[ ", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	color_c("OK", FOREGROUND_GREEN + FOREGROUND_INTENSITY);
	color_c(" ]\n", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

}

void CGameServerDlg::Yaz(char* yazi, WORD color)
{
	char data[105];
	memset(data, 0x00, 105);

	strcpy(data, yazi);

	int i2 = 0;
	i2 = 100 - strlen(yazi);

	for (int i = 0; i < i2; i++)
		sprintf(data, "%s ", data);

	if (color == 0)
		color_c((char*)(LPCTSTR)data, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	else
		color_c((char*)(LPCTSTR)data, color);

}