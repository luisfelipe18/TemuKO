#include "stdafx.h"
#include "UIMiniMenu.h"

CUIMiniMenuPlug::CUIMiniMenuPlug()
{
	vector<int>offsets;
	offsets.push_back(0x398);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_btnViewInfo = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUIMiniMenuPlug::~CUIMiniMenuPlug()
{
}

void CUIMiniMenuPlug::ParseUIElements()
{
	std::string find = xorstr("base_default");
	DWORD parent;
	Engine->GetChildByID(m_dVTableAddr, find, parent);
	find = xorstr("btn_viewinfo");
	Engine->GetChildByID(parent, find, m_btnViewInfo);
}

DWORD uiMiniMenuVTable;
DWORD miniMenuSetVisibleFalse;
void __stdcall SetVisibleViaOGFuncAsm()
{
	_asm
	{
		MOV ECX, uiMiniMenuVTable
		MOV EAX, miniMenuSetVisibleFalse
		PUSH EAX
		CALL KO_SET_VISIBLE_MINIMENU_FUNC
	}
}

bool CUIMiniMenuPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	uiMiniMenuVTable = m_dVTableAddr;
	miniMenuSetVisibleFalse = 0;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;

	
	if (pSender == (DWORD*)m_btnViewInfo)
	{
		Packet pkt(WIZ_USER_INFO, uint8(5));
		pkt.SByte();
		
		uint16 target;
		DWORD ko_adr;

		ReadProcessMemory(GetCurrentProcess(), (LPVOID)KO_PTR_CHR, &ko_adr, sizeof(&ko_adr), 0);
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)(ko_adr + KO_OFF_MOB), &target, sizeof(&target), 0);

		pkt << target;
		Engine->Send(&pkt);
		SetVisibleViaOGFuncAsm();
	}

	return true;
}

void __stdcall UiMiniMenuReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiMiniMenuPlug->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, uiMiniMenuVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_MINIMENU_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIMiniMenuPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_MINIMENU_RECEIVE_MESSAGE_PTR = (DWORD)UiMiniMenuReceiveMessage_Hook;
}