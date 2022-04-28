#pragma once
class CUITaskbarMainPlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUITaskbarMainPlug
{
public:
	DWORD m_dVTableAddr;
	DWORD m_btnPowerUP, m_btnVoice, m_btnMonstersch;
	DWORD m_btnPPCard;
	DWORD m_btnLifeSkill;
public:
	CUITaskbarMainPlug();
	~CUITaskbarMainPlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void SendOpenPPCardMessage();
	void InitReceiveMessage();
	void OpenPPCard();
private:

};