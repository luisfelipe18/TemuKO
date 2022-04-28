#pragma once
#include "stdafx.h"
class CUILogin;
#include "TEMUKOACS Engine.h"

class CUILogin
{
public:
	CUILogin();
	~CUILogin();

	void ParseUIElements();

	DWORD m_dVTableAddr;

	DWORD m_bGroupLogin;

	DWORD m_btnRememberID;
	DWORD m_btnLogin;

	DWORD m_EditUID;
	DWORD m_TextUID;

	DWORD m_EditPW;
	DWORD m_TextPW;

	void Login(string account, string password);
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};