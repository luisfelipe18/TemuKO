#pragma once
class CUIStatePlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUIStatePlug
{
public:
	DWORD m_dVTableAddr;
	//Buttons
	DWORD m_btnResetStat;
	DWORD m_btnResetReb;
	//Texts
	DWORD m_txtCash;
	DWORD m_txtCashBonus;
	DWORD m_txtDagger;
	DWORD m_txtSword;
	DWORD m_txtClub;
	DWORD m_txtAxe;
	DWORD m_txtSpear;
	DWORD m_txtArrow;
	DWORD m_txtJamadar;

	DWORD m_btnStr10;
	DWORD m_btnHp10;
	DWORD m_btnDex10;
	DWORD m_btnMp10;
	DWORD m_btnInt10;
	DWORD m_strFreeStatPoint;

	int32 m_iCash;
	int32 m_iCashBonus;
	uint16 m_iDagger;
	uint16 m_iSword;
	uint16 m_iClub;
	uint16 m_iAxe;
	uint16 m_iSpear;
	uint16 m_iArrow;
	uint16 m_iJamadar;

public:
	CUIStatePlug();
	~CUIStatePlug();
	void ParseUIElements();
	void SetAntiDefInfo(Packet& pkt);
	void SetAntiDefInfoItemMove(Packet& pkt);
	void UpdateKC(uint32 cash, uint32 bonuscash);
	bool IsHaveFreePoints();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();
	void UpdateUI();
};