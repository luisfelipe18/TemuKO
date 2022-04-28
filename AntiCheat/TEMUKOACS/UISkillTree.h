#pragma once
class CUISkillTreePlug;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"

class CUISkillTreePlug
{
public:
	uintptr_t** vTable;
	DWORD m_dVTableAddr;
	// UI
	DWORD m_btnResetSkill;
public:
	CUISkillTreePlug();
	~CUISkillTreePlug();
	void ParseUIElements();
	bool ReceiveMessage(DWORD* pSender, uint32_t dwMsg);
	void InitReceiveMessage();

private:

};