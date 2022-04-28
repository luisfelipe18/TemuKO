#include "stdafx.h"
#include "UISkillTree.h"

CUISkillTreePlug::CUISkillTreePlug()
{
	vector<int>offsets;
	offsets.push_back(0x1EC);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
	vTable = (uintptr_t**)m_dVTableAddr;

	m_btnResetSkill = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUISkillTreePlug::~CUISkillTreePlug()
{
}

void CUISkillTreePlug::ParseUIElements()
{
	std::string find = xorstr("btn_EXbuf");
	DWORD exBuf = 0;
	Engine->GetChildByID(m_dVTableAddr, find, exBuf);
	Engine->SetState(exBuf, UI_STATE_BUTTON_DISABLE);

	find = xorstr("btn_preset");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnResetSkill);
}

DWORD uiSkillTreeVTable;
void __stdcall UiSkillTreeIconUpdate()
{
	__asm
	{
		MOV ECX, uiSkillTreeVTable
		MOV EAX, KO_SKILL_TREE_ICON_UPDATE_FUNC
		CALL EAX
	}
}

bool CUISkillTreePlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiSkillTreeVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnResetSkill)
	{
		if (Engine->EngineSettings->State_SkillReset != 0)
			Engine->m_UiMgr->OpenSkillPreset();
		else
			Engine->m_UiMgr->ShowMessageBox("Failed", "Skill Preset feature is currently disabled.");
	}

	return true;
}

void __stdcall UiSkillTreeReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiSkillTreePlug->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiSkillTreeVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_SKILL_TREE_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUISkillTreePlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_SKILL_TREE_RECEIVE_MESSAGE_PTR = (DWORD)UiSkillTreeReceiveMessage_Hook;
}