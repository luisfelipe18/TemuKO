#pragma once
#include "stdafx.h"
class CUIWarp;
#include "TEMUKOACS Engine.h"

class CUIWarp
{
public:
	CUIWarp();
	~CUIWarp();

	void ParseUIElements();

	DWORD m_dVTableAddr;

	DWORD m_btnPartyTravel;

	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};