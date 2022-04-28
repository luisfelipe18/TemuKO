#include "stdafx.h"
#include "MerchantManager.h"

CMerchantManager::CMerchantManager()
{
	ResetMerchant();
}

CMerchantManager::~CMerchantManager()
{
}

bool CMerchantManager::NeedToCheckTooltip()
{
	return (status == MERCHANT_OPEN || status == MERCHANT_ITEM_LIST)
		&& Engine->IsVisible(Engine->uiTradeInventory->m_dVTableAddr);
}

bool CMerchantManager::NeedToCheckDisplayTooltip()
{
	if (status != MERCHANT_CREATED && status != MERCHANT_SPECIAL_CREATED)
		return false;

	return Engine->IsVisible(Engine->uiTradeItemDisplay->m_dVTableAddr)
		|| Engine->IsVisible(Engine->uiTradeItemDisplaySpecial->m_dVTableAddr);
}

bool CMerchantManager::UpdateTooltipString(std::string& str)
{
	POINT cp = Engine->m_UiMgr->localInput->MouseGetPos();

	for (MerchantSlotListItor itor = m_MerchantSlots.begin(); m_MerchantSlots.end() != itor; ++itor)
	{
		CMerchantSlot* slot = (*itor);
		DWORD slotVTable = 0;

		if (status == MERCHANT_OPEN || status == MERCHANT_ITEM_LIST)
			slotVTable = Engine->uiTradeInventory->GetSlotVTable(slot->m_iPos);
		else if (status == MERCHANT_CREATED)
			slotVTable = Engine->uiTradeItemDisplay->GetSlotVTable(slot->m_iPos);
		else if (status == MERCHANT_SPECIAL_CREATED)
			slotVTable = Engine->uiTradeItemDisplaySpecial->GetSlotVTable(slot->m_iPos);

		if (slotVTable != 0)
		{
			bool isIn = false;
			POINT pt;
			Engine->GetUiPos(slotVTable, pt);

			RECT rp;
			rp.left = pt.x;
			rp.right = pt.x + (status == MERCHANT_CREATED ? 32 : 45);
			rp.top = pt.y;
			rp.bottom = pt.y + (status == MERCHANT_CREATED ? 32 : 45);

			isIn = cp.x >= rp.left && cp.x <= rp.right && cp.y >= rp.top && cp.y <= rp.bottom;
			
			if (isIn)
			{
				std::string strPrice = Engine->StringHelper->NumberFormat(slot->m_iPrice, ',');
				if (slot->m_bIsKC)
				{
					std::string kcPrice = xorstr(" Purchasing Price : %s knight cash");
					str = string_format(kcPrice, strPrice.c_str());
					return true;
				}

				std::string noahPrice = xorstr(" Purchasing Price : %s coins");
				str = string_format(noahPrice, strPrice.c_str());
				return false;
			}
		}
	}

	return false;
}

void CMerchantManager::SetRecentItemAddReq(Packet& pkt)
{
	uint32 price, itemID;
	uint16 count;
	uint8 subCode, srcPos, dstPos, mode, isKC;

	pkt >> subCode >> itemID >> count >> price >> srcPos >> dstPos >> mode >> isKC;

	m_RecentReq = new CMerchantSlot(dstPos, price, isKC ? 1 == true : false, count);
}

bool CMerchantManager::UpdateSlot(CMerchantSlot* item)
{
	for (MerchantSlotListItor itor = m_MerchantSlots.begin(); m_MerchantSlots.end() != itor; ++itor)
	{
		CMerchantSlot* slot = (*itor);
		if (slot->m_iPos == m_RecentReq->m_iPos)
		{
			*itor = item;
			return true;
		}
	}
	return false;
}

void CMerchantManager::UpdateRecentItemAddReq(Packet& pkt)
{
	uint32 price, itemID;
	uint16 count, result;
	int16 durat;
	uint8 srcPos, dstPos;

	status = MERCHANT_OPEN;

	pkt >> result >> itemID >> count >> durat >> price >> srcPos >> dstPos;

	if (result == 1 && m_RecentReq != NULL)
	{
		if (m_RecentReq->m_iPos == dstPos && m_RecentReq->m_iPrice == price)
		{
			bool isEmpty = true;
			for (MerchantSlotListItor itor = m_MerchantSlots.begin(); m_MerchantSlots.end() != itor; ++itor)
			{
				CMerchantSlot* slot = (*itor);

				if (slot->m_iPos == m_RecentReq->m_iPos)
				{
					isEmpty = false;
					break;
				}
			}
			CMerchantSlot* newSlot = new CMerchantSlot(m_RecentReq->m_iPos, m_RecentReq->m_iPrice, m_RecentReq->m_bIsKC, m_RecentReq->m_iCount);

			if (isEmpty)
				m_MerchantSlots.push_back(newSlot);
			else
				UpdateSlot(newSlot);

			m_RecentReq = NULL;

			Engine->uiTradeInventory->UpdateTotal(price * count, false, true);
		}
	}
	else if (result == 0)
		m_RecentReq = NULL;
}

void CMerchantManager::RemoveItemFromSlot(Packet& pkt)
{
	uint16 res;
	pkt >> res;

	if (res < 1 && m_RecentReq != NULL) // son eklenen item onaylanmadýysa
	{
		m_RecentReq = NULL;
		return;
	}

	uint8 pos;
	pkt >> pos;

	for (MerchantSlotListItor itor = m_MerchantSlots.begin(); m_MerchantSlots.end() != itor; ++itor)
	{
		CMerchantSlot* slot = (*itor);

		if (slot->m_iPos == pos)
		{
			Engine->uiTradeInventory->UpdateTotal(slot->m_iPrice * slot->m_iCount * -1, false, true);
			itor = m_MerchantSlots.erase(itor);
			break;
		}
	}
}

void CMerchantManager::MerchantCreated(Packet& pkt)
{
	uint16 result;
	pkt >> result;

	if (result == 1) // pazar oluþtu
	{
		std::string advertMessage;
		uint16 m_sSocketID;
		uint8 m_bPremiumMerchant;

		pkt.DByte();
		pkt >> advertMessage >> m_sSocketID >> m_bPremiumMerchant;
		if (m_sSocketID == Engine->m_PlayerBase->GetSocketID()) // pazarý kuran bensem
		{
			if (m_bPremiumMerchant == 1)
				status = MERCHANT_SPECIAL_CREATED; // special merchant
			else 
				status = MERCHANT_CREATED; // normal merchant
		}
	}
}

void CMerchantManager::UpdateItemDisplaySlots(Packet& pkt)
{
	uint8 isSpecial, itemCount;
	pkt >> isSpecial >> itemCount;

	m_MerchantSlots.clear();
	for (int i = 0; i < itemCount; i++)
	{
		uint8 pos, isKC;
		uint32 price;

		pkt >> pos >> price >> isKC;

		CMerchantSlot* newSlot = new CMerchantSlot(pos, price, true);
		m_MerchantSlots.push_back(newSlot);
	}
}

void CMerchantManager::SetTheirMerchantSlots(Packet& pkt)
{
	m_MerchantSlots.clear();
	Engine->m_MerchantMgr->status = MERCHANT_ITEM_LIST;

	Engine->uiTradeInventory->UpdateTotal(Engine->Player.KnightCash);

	uint16 result, uid;
	pkt >> result >> uid;

	uint32 items[12];
	uint32 prices[12];
	for (int i = 0; i < 12; i++)
	{
		uint16 count, durat;
		uint32 itemID, price, ring;

		pkt >> itemID >> count >> durat >> price >> ring;
		items[i] = itemID;
		prices[i] = price;

		if (ring != 0)
		{
			string charName;
			uint8 charClass, charLevel, unk1, charExp, charRace, unk2;
			pkt.DByte();
			pkt >> charName >> charClass >> charLevel >> unk1 >> charExp >> charRace >> unk2;
		}
	}

	for (int i = 0; i < 12; i++)
	{
		uint8 isKC;
		pkt >> isKC;

		if (items[i] > 0) // slot boþ deðilse
		{
			CMerchantSlot* newSlot = new CMerchantSlot(i, prices[i], isKC == 1 ? true : false);
			m_MerchantSlots.push_back(newSlot);
		}
	}
}

void CMerchantManager::ResetMerchant()
{
	m_RecentReq = NULL;
	m_MerchantSlots.clear();
	status = MERCHANT_CLOSE;
}

bool CMerchantManager::IsMerchanting()
{
	return status == MERCHANT_ITEM_LIST || status == MERCHANT_OPEN;
}