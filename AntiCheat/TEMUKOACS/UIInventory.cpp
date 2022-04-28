#include "stdafx.h"
#include "UIInventory.h"

CUIInventoryPlug::CUIInventoryPlug()
{
	vector<int>offsets;
	offsets.push_back(0x1B8);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
	//InitReceiveMessage();
}

CUIInventoryPlug::~CUIInventoryPlug()
{
}

void CUIInventoryPlug::ParseUIElements()
{
	DWORD parent = 0;
	std::string find = xorstr("x");
	for (int i = 0; i < 28; i++)
	{
		find = string_format(xorstr("%d"), i);
		Engine->GetChildByID(m_dVTableAddr, find, slot[i]);
	}
}

DWORD uiInventoryVTable;
bool CUIInventoryPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiInventoryVTable = m_dVTableAddr;
	if (!pSender 
		|| pSender == 0
		|| dwMsg != 67108864)
		return false;
	
	return true;
}

void __stdcall UIInventoryReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiInventoryPlug->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, uiInventoryVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_INVENTORY_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIInventoryPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_INVENTORY_RECEIVE_MESSAGE_PTR = (DWORD)UIInventoryReceiveMessage_Hook;
}