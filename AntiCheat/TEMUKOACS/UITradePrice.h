#pragma once
class CUITradePricePlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUITradePricePlug
{
public:
	DWORD m_dVTableAddr;
	bool m_bIsKC;
	DWORD m_btnKC;
	DWORD m_txtMoney;

public:
	CUITradePricePlug();
	~CUITradePricePlug();
	void Reset();
	void ParseUIElements();
	void Tick();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};