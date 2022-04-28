#include "stdafx.h"
#include "UITradeInventory.h"

CUITradeInventoryPlug::CUITradeInventoryPlug()
{
	vector<int>offsets;
	offsets.push_back(0x270);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	m_txtTotal = 0;
	val = 0;
	lastState = 0;
	lastVisible = false;

	ParseUIElements();
}

CUITradeInventoryPlug::~CUITradeInventoryPlug()
{
}

void CUITradeInventoryPlug::ParseUIElements()
{
	std::string find = xorstr("text_money_total");
	Engine->GetChildByID(m_dVTableAddr, find, m_txtTotal);
	find = xorstr("img_kc");
	Engine->GetChildByID(m_dVTableAddr, find, m_imgKC);
	find = xorstr("img_coin");
	Engine->GetChildByID(m_dVTableAddr, find, m_imgCoin);
	find = xorstr("at%d");

	for (int i = 0; i < 12; i++)
	{
		DWORD curVTable = 0;
		Engine->GetChildByID(m_dVTableAddr, string_format(find, i), curVTable);

		if (curVTable != 0)
		{
			CMerchantInvSlot* newSlot = new CMerchantInvSlot(i, curVTable);
			m_Slots.push_back(newSlot);
		}
	}
}

void CUITradeInventoryPlug::OnOpen()
{
	if (lastState == 0) val = Engine->Player.KnightCash;
	UpdateTotal(val, lastState == 0 ? true : false, false);
}

void CUITradeInventoryPlug::OnClose()
{
	val = 0;
}

void CUITradeInventoryPlug::UpdateTotal(int32 value, bool iskc, bool add)
{
	if ((lastState == 0 && !iskc) 
		|| (lastState == 1 && iskc)) 
	{
		val = 0;
		lastState = !lastState;
	}

	if (add)
		val += value;
	else
		val = value;

	std::string strValue = Engine->StringHelper->NumberFormat(val);
	Engine->SetString(m_txtTotal, strValue);

	if (iskc) {
		Engine->SetVisible(m_imgCoin, false);
		Engine->SetVisible(m_imgKC, true);
	}
	else {
		Engine->SetVisible(m_imgCoin, true);
		Engine->SetVisible(m_imgKC, false);
	}
}

void CUITradeInventoryPlug::Tick()
{
	if (m_dVTableAddr != 0) {
		if (lastVisible != Engine->IsVisible(m_dVTableAddr))
		{
			lastVisible = !lastVisible;
			if (lastVisible)
				OnOpen();
			else
				OnClose();
		}
	}
}

DWORD CUITradeInventoryPlug::GetSlotVTable(int id)
{
	for (MerchantInvSlotListItor itor = m_Slots.begin(); m_Slots.end() != itor; ++itor)
	{
		CMerchantInvSlot* slot = (*itor);

		if (slot->m_iID == id)
			return slot->m_dVTableAddr;
	}

	return 0;
}