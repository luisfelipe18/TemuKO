#include "stdafx.h"
#include "UIClanWindow.h"

CUIClanWindowPlug::CUIClanWindowPlug()
{
	vector<int>offsets;
	offsets.push_back(0x548);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
	m_btnClanBank = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUIClanWindowPlug::~CUIClanWindowPlug()
{
}

void CUIClanWindowPlug::ParseUIElements()
{
	std::string find = xorstr("group");
	DWORD parent;
	Engine->GetChildByID(m_dVTableAddr, find, parent);
	find = xorstr("btn_bank");
	Engine->GetChildByID(parent, find, m_btnClanBank);
}

DWORD uiClanWindowVTable;
bool CUIClanWindowPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiClanWindowVTable = m_dVTableAddr;
	if (!pSender 
		|| pSender == 0 
		|| dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	std::string msg = "";
	if (pSender == (DWORD*)m_btnClanBank)
	{
		Packet result(WIZ_CLANWAREHOUSE, uint8(0x01));
		Engine->Send(&result);
	}

	return true;
}

void __stdcall UiClanWindowReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiClanWindowPlug->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiClanWindowVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_CLAN_WINDOW_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIClanWindowPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_CLAN_WINDOW_RECEIVE_MESSAGE_PTR = (DWORD)UiClanWindowReceiveMessage_Hook;
}