#include "stdafx.h"
#include "UITradePrice.h"

CUITradePricePlug::CUITradePricePlug()
{
	vector<int>offsets;
	offsets.push_back(0x270);
	offsets.push_back(0x318);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_bIsKC = false;
	m_btnKC = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUITradePricePlug::~CUITradePricePlug()
{
}

void CUITradePricePlug::Reset()
{
	m_bIsKC = false;
}

void CUITradePricePlug::ParseUIElements()
{
	std::string find = xorstr("btn_iskc");
	Engine->GetChildByID(m_dVTableAddr, find, m_btnKC);
	
	
	DWORD tmp;
	find = xorstr("edit_money");
	Engine->GetChildByID(m_dVTableAddr, find, tmp);
	find = xorstr("text_money");
	Engine->GetChildByID(tmp, find, m_txtMoney);
}

DWORD merchantTradePriceVTable;
bool CUITradePricePlug::ReceiveMessage(DWORD* pSender, uint32_t dwMsg)
{
	merchantTradePriceVTable = m_dVTableAddr;
	if (!pSender || pSender == 0 || dwMsg != UIMSG_BUTTON_CLICK)
		return false;
	
	if (pSender == (DWORD*)m_btnKC)
	{
		m_bIsKC = !m_bIsKC;
	}

	return true;
}

bool updateRequired = false;

void CUITradePricePlug::Tick()
{
	if (Engine->IsVisible(m_dVTableAddr))
	{
		if (updateRequired) 
		{
			updateRequired = false;
			Engine->SetState(m_btnKC, m_bIsKC ? UI_STATE_BUTTON_DOWN : UI_STATE_BUTTON_NORMAL);
		}
	} 
	else 
		updateRequired = true;
}

void __stdcall MerchantTradePriceReceiveMessage_Hook(DWORD* pSender, uint32_t dwMsg)
{
	Engine->uiTradePrice->ReceiveMessage(pSender, dwMsg);

	__asm
	{
		MOV ECX, merchantTradePriceVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_MERCHANT_PRICE_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUITradePricePlug::InitReceiveMessage()
{
	*(DWORD*)KO_UI_MERCHANT_PRICE_RECEIVE_MESSAGE_PTR = (DWORD)MerchantTradePriceReceiveMessage_Hook;
}