#include "stdafx.h"
#include "UITradeItemDisplaySpecial.h"

CUITradeItemDisplaySpecialPlug::CUITradeItemDisplaySpecialPlug()
{
	vector<int>offsets;
	offsets.push_back(0x278);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);

	ParseUIElements();
}

CUITradeItemDisplaySpecialPlug::~CUITradeItemDisplaySpecialPlug()
{
}

void CUITradeItemDisplaySpecialPlug::ParseUIElements()
{
	for (int i = 0; i < 8; i++)
	{
		DWORD curVTable = 0;
		Engine->GetChildByID(m_dVTableAddr, std::to_string(i), curVTable);

		if (curVTable != 0)
		{
			CMerchantInvSlot* newSlot = new CMerchantInvSlot(i, curVTable);
			m_Slots.push_back(newSlot);
		}
	}
}

DWORD CUITradeItemDisplaySpecialPlug::GetSlotVTable(int id)
{
	for (MerchantInvSlotListItor itor = m_Slots.begin(); m_Slots.end() != itor; ++itor)
	{
		CMerchantInvSlot* slot = (*itor);

		if (slot->m_iID == id)
			return slot->m_dVTableAddr;
	}

	return 0;
}