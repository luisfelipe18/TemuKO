#include "UITargetBar.h"
#include <math.h>

CUITargetBarPlug::CUITargetBarPlug()
{
	vector<int>offsets;
	offsets.push_back(0x1D4);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_dTextTargetHp = NULL;
	m_btnDrop = NULL;

	ParseUIElements();
	InitReceiveMessage();
}

CUITargetBarPlug::~CUITargetBarPlug()
{
}

void CUITargetBarPlug::ParseUIElements()
{
	std::string find = xorstr("Btn_Drops");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnDrop);
	find = xorstr("Text_Health");
	Engine->GetChildByID(m_dVTableAddr, find, m_dTextTargetHp);
}

DWORD uiTargetBarVTable;

bool CUITargetBarPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiTargetBarVTable = m_dVTableAddr;
	if (!pSender 
		|| pSender == 0 
		|| dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnDrop)
	{
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_DROP_REQUEST));
		result << uint8(1) << uint32(Engine->m_PlayerBase->GetTargetID());
		Engine->Send(&result);
	}
	return true;
}

void __stdcall UiTargetBarReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiTargetBar->ReceiveMessage(pSender, dwMsg);
}

void CUITargetBarPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_TARGETBAR_RECEIVE_MESSAGE_PTR = (DWORD)UiTargetBarReceiveMessage_Hook;
}

void CUITargetBarPlug::SetTargetHp(Packet& pkt)
{
	uint16 tid, damage, ssid;
	uint8 echo;
	int maxhp, hp;
	pkt >> tid >> echo >> maxhp >> hp >> damage >> ssid;

	int percent = (int)ceil((hp * 100) / maxhp);
	std::string max = Engine->StringHelper->NumberFormat(maxhp);
	std::string cur = Engine->StringHelper->NumberFormat(hp);

	std::string str = xorstr("%s/%s (%d%%)");
	Engine->SetString(m_dTextTargetHp, string_format(str, cur.c_str(), max.c_str(), percent));
	Engine->m_zMob = tid;
}