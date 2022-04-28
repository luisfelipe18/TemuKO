#include "stdafx.h"
#include "UIParty.h"

CUIPartyPlugin::CUIPartyPlugin()
{
	vector<int>offsets;
	offsets.push_back(0x1E8);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
	//InitReceiveMessage();
}

CUIPartyPlugin::~CUIPartyPlugin()
{
}

void CUIPartyPlugin::ParseUIElements()
{
	for (int i = 0; i < 8; i++)
		slot[i] = new PartySlot();

	DWORD parent = 0;
	std::string find = xorstr("x");
	for (int i = 0; i < 8; i++)
	{
		find = string_format(xorstr("Base_Party%d"), i);
		Engine->GetChildByID(m_dVTableAddr, find, slot[i]->baseAddr);

		find = string_format(xorstr("str_name%d"), i);
		Engine->GetChildByID(slot[i]->baseAddr, find, slot[i]->strName);
		find = string_format(xorstr("str_hp%d"), i);
		Engine->GetChildByID(slot[i]->baseAddr, find, slot[i]->strHP);

		for (int j = 0; j < 5; j++) {
			find = string_format(xorstr("icon_skill%d_%d"), i, j);
			Engine->GetChildByID(slot[i]->baseAddr, find, slot[i]->iconSkill[j]);
			Engine->SetVisible(slot[i]->iconSkill[j], false);
		}

		find = string_format(xorstr("icon_voice%d"), i);
		Engine->GetChildByID(slot[i]->baseAddr, find, slot[i]->iconVoice);
		Engine->SetVisible(slot[i]->iconVoice, false);

		find = string_format(xorstr("str_nobuff%d"), i);
		Engine->GetChildByID(slot[i]->baseAddr, find, slot[i]->strNoBuff);
	}
}

void CUIPartyPlugin::ReorderParty(Packet pkt)
{
	ResetParty();

	uint8 subcode, memberCount;
	pkt >> subcode >> memberCount;

	for (int i = 0; i < memberCount; i++)
	{
		uint8 j = 0;
		uint8 level, swift, hpbuff, acbuff, resisbuff, resbuff;
		uint16 uid;
		short maxHP, hp;
		pkt >> uid >> maxHP >> hp >> level >> swift >> hpbuff >> acbuff >> resisbuff >> resbuff;
		slot[i]->uid = uid;
		Engine->SetString(slot[i]->strHP, string_format(xorstr("%d/%d [%d]"), hp, maxHP, level));
		Engine->SetVisible(slot[i]->iconSkill[j++], hpbuff == 1);
		Engine->SetVisible(slot[i]->iconSkill[j++], acbuff == 1);
		Engine->SetVisible(slot[i]->iconSkill[j++], resisbuff == 1);
		Engine->SetVisible(slot[i]->iconSkill[j++], resbuff == 1);
		Engine->SetVisible(slot[i]->iconSkill[j++], swift == 1);

		Engine->SetVisible(slot[i]->strNoBuff, hpbuff == 0 && acbuff == 0 && resisbuff == 0 && resbuff == 0 && swift == 0);
	}
}

bool CUIPartyPlugin::isInParty()
{
	for (int i = 0; i < 8; i++) {
		if (slot[i]->uid > 0)
			return true;
	}
	return false;
}

void CUIPartyPlugin::ResetParty()
{
	for (int i = 0; i < 8; i++) {
		slot[i]->uid = 0;
		uint8 j = 0;
		Engine->SetVisible(slot[i]->iconSkill[j++], false);
		Engine->SetVisible(slot[i]->iconSkill[j++], false);
		Engine->SetVisible(slot[i]->iconSkill[j++], false);
		Engine->SetVisible(slot[i]->iconSkill[j++], false);
		Engine->SetVisible(slot[i]->iconSkill[j++], false);
		Engine->SetVisible(slot[i]->iconVoice, false);
		Engine->SetVisible(slot[i]->strNoBuff, true);
	}
}

void CUIPartyPlugin::UserSpeaking(uint16 uid, bool speaking)
{
	for (int i = 0; i < 8; i++)
		if (slot[i]->uid == uid) {
			Engine->SetVisible(slot[i]->iconVoice, speaking);
			if (speaking)
				slot[i]->voiceTimer = new CTimer();
			else
				slot[i]->voiceTimer = NULL;
		}
}

void CUIPartyPlugin::Tick()
{
	for (int i = 0; i < 8; i++) {
		if (slot[i] != NULL && slot[i]->voiceTimer != NULL) {
			slot[i]->voiceTimer->Tick();
			if (slot[i]->voiceTimer->IsElapsedSecond())
				UserSpeaking(slot[i]->uid, false);
		}
	}
}

DWORD uiPartyBBSVTable;
bool CUIPartyPlugin::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiPartyBBSVTable = m_dVTableAddr;
	if (!pSender 
		|| pSender == 0 
		|| dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	return true;
}

void __stdcall UIPatyBBSReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiInventoryPlug->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, uiPartyBBSVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_INVENTORY_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIPartyPlugin::InitReceiveMessage()
{
	*(DWORD*)KO_UI_INVENTORY_RECEIVE_MESSAGE_PTR = (DWORD)UIPatyBBSReceiveMessage_Hook;
}