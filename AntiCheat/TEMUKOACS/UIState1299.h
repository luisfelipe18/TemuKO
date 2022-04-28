#pragma once
class CUIStatePlug1299;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIStatePlug1299
{
public:
	DWORD m_dVTableAddr;
	//Buttons
	DWORD m_btnResetStat;
	//Texts
	DWORD m_txtCash;
	DWORD m_txtDagger;
	DWORD m_txtSword;
	DWORD m_txtClub;
	DWORD m_txtAxe;
	DWORD m_txtSpear;
	DWORD m_txtArrow;

	int32 m_iCash;
	int8 m_iDagger;
	int8 m_iSword;
	int8 m_iClub;
	int8 m_iAxe;
	int8 m_iSpear;
	int8 m_iArrow;

public:
	CUIStatePlug1299();
	~CUIStatePlug1299();
	void ParseUIElements();
	void SetAntiDefInfo(Packet& pkt);
	void UpdateKC(uint32 cash);
	bool IsHaveFreePoints();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
	void UpdateUI();
};