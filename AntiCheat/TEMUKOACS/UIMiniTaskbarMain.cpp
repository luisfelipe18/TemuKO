#include "stdafx.h"
#include "UIMiniTaskbarMain.h"

CUIMiniTaskbarMainPlug::CUIMiniTaskbarMainPlug()
{
	vector<int>offsets;
	offsets.push_back(0x38C);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
	m_btnPowerUP = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUIMiniTaskbarMainPlug::~CUIMiniTaskbarMainPlug(){
	m_btnPowerUP = 0;
}

void CUIMiniTaskbarMainPlug::ParseUIElements()
{
	std::string find = xorstr("btn_pus");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnPowerUP);
}

DWORD UIMiniTaskbarMainVTable;
bool CUIMiniTaskbarMainPlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	UIMiniTaskbarMainVTable = m_dVTableAddr;

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
		}
		else Engine->m_UiMgr->ShowMessageBox("Failed", "Power-up-Store is currently disabled.");
	}

	return true;
}

void __stdcall UIMiniTaskbarMainReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->UIMiniTaskbarMain->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, UIMiniTaskbarMainVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_MINI_MAIN_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUIMiniTaskbarMainPlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_MINI_MAIN_RECEIVE_MESSAGE_PTR = (DWORD)UIMiniTaskbarMainReceiveMessage_Hook;
}