#pragma once
#include "stdafx.h"
class CUITargetBarPlug;
#include "TEMUKOACS Engine.h"

class CUITargetBarPlug
{
public:
	CUITargetBarPlug();
	~CUITargetBarPlug();

	void ParseUIElements();

	DWORD m_dVTableAddr;

	DWORD m_btnDrop;

	DWORD m_dTextTargetHp;

	void SetTargetHp(Packet& pkt);

	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};