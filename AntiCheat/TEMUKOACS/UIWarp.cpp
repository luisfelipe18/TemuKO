#include "UIWarp.h"

CUIWarp::CUIWarp()
{
	vector<int>offsets;
	offsets.push_back(0x444);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
	InitReceiveMessage();
}

CUIWarp::~CUIWarp()
{
}

void CUIWarp::ParseUIElements()
{
	std::string find = xorstr("btn_PartyTravel");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnPartyTravel);
}

DWORD uiWarpVTable;
bool CUIWarp::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiWarpVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnPartyTravel)
	{
		
	}
	return true;
}

void __stdcall UiWarpReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiWarp->ReceiveMessage(pSender, dwMsg);
}

void CUIWarp::InitReceiveMessage()
{
	*(DWORD*)KO_UI_WARP_RECEIVE_MESSAGE_PTR = (DWORD)UiWarpReceiveMessage_Hook;
}