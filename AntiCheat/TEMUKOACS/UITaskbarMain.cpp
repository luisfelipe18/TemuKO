#include "stdafx.h"
#include "UITaskbarMain.h"

CUITaskbarMainPlug::CUITaskbarMainPlug()
{
	vector<int>offsets;
	offsets.push_back(0x388);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
	m_btnPowerUP = 0;
	m_btnPPCard = 0;
	m_btnLifeSkill = 0;
	m_btnVoice = 0;
	m_btnMonstersch = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUITaskbarMainPlug::~CUITaskbarMainPlug()
{
	m_btnPowerUP = 0;
	m_btnPPCard = 0;
	m_btnLifeSkill = 0;
	m_btnVoice = 0;
	m_btnMonstersch = 0;
}

void CUITaskbarMainPlug::ParseUIElements()
{
	DWORD parent = 0;
	std::string find = xorstr("base_menu");
	Engine->GetChildByID(m_dVTableAddr, find, parent);
	find = xorstr("btn_power");
	Engine->GetChildByID(parent, find, m_btnPowerUP);
	find = xorstr("btn_ppcard");
	Engine->GetChildByID(parent, find, m_btnPPCard);
	find = xorstr("btn_lifeskill");
	Engine->GetChildByID(parent, find, m_btnLifeSkill);
	find = xorstr("btn_voice");
	Engine->GetChildByID(parent, find, m_btnVoice);
	find = xorstr("btn_monstersch");
	Engine->GetChildByID(parent, find, m_btnMonstersch);
}

DWORD uiTaskbarMainVTable;
bool CUITaskbarMainPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiTaskbarMainVTable = m_dVTableAddr;
	if (!pSender 
		|| pSender == 0 
		|| dwMsg != UIMSG_BUTTON_CLICK)
		return false;

	if (pSender == (DWORD*)m_btnPowerUP)
	{
		if (Engine->EngineSettings->State_PUS != 0)
		{
			if (Engine->m_UiMgr->uiPowerUpStore == NULL)
			{
				Engine->m_UiMgr->uiPowerUpStore = new CPowerUpStore();
				Engine->m_UiMgr->uiPowerUpStore->Init(Engine->m_UiMgr);
				Engine->m_UiMgr->uiPowerUpStore->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\pus_main.uif"), N3FORMAT_VER_1068);
				Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiPowerUpStore);
			}
			Engine->m_UiMgr->uiPowerUpStore->UpdateItemList(Engine->m_UiMgr->item_list);
			Engine->m_UiMgr->uiPowerUpStore->Open();
		} else Engine->m_UiMgr->ShowMessageBox("Failed", "Power-up-Store is currently disabled.");
	}
	else if (pSender == (DWORD*)m_btnLifeSkill)
	{
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_LIFESKILL));
		Engine->Send(&result);

		if (Engine->m_UiMgr->uiLifeSkill == NULL)
		{
			Engine->m_UiMgr->uiLifeSkill = new CLifeSkill();
			Engine->m_UiMgr->uiLifeSkill->Init(Engine->m_UiMgr);
			Engine->m_UiMgr->uiLifeSkill->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\lifeskill.uif"), N3FORMAT_VER_1068);
			Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiLifeSkill);
		}
		Engine->m_UiMgr->uiLifeSkill->Open();
	}
	else if (pSender == (DWORD*)m_btnVoice)
	{
		Engine->m_UiMgr->OpenVoiceSettings();
	}
	else if (pSender == (DWORD*)m_btnMonstersch)
	{
		Engine->m_UiMgr->OpenSearchMonster();
	}
	
	return true;
}

void __stdcall UITaskbarMainReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiTaskbarMain->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, uiTaskbarMainVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_TASKBAR_MAIN_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUITaskbarMainPlug::SendOpenPPCardMessage(){
	UITaskbarMainReceiveMessage_Hook((DWORD*)m_btnPowerUP, UIMSG_BUTTON_CLICK);
}

void CUITaskbarMainPlug::InitReceiveMessage(){
	*(DWORD*)KO_UI_TASKBAR_MAIN_RECEIVE_MESSAGE_PTR = (DWORD)UITaskbarMainReceiveMessage_Hook;
}

void CUITaskbarMainPlug::OpenPPCard(){
	UITaskbarMainReceiveMessage_Hook((DWORD*)m_btnPPCard, UIMSG_BUTTON_CLICK);
}