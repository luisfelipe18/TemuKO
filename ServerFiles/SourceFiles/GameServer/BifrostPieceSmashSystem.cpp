#include "stdafx.h"

/**
* @brief	Packet handler for the Chaotic Generator system
* 			which is used to exchange Bifrost pieces/fragments.
*
* @param	pkt	The packet.
*/
void CUser::BifrostPieceProcess(Packet & pkt)
{
	enum ResultMessages
	{
		EffectNone = 0, // No effect
		EffectRed = 1, // There will be better days.
		EffectGreen = 2, // Don't be too disappointed. You're luck isn't that bad.
		EffectWhite = 3 // It must be your lucky day.
	};

	uint16 nObjectID = 0;
	uint32 nExchangeItemID = 0;
	ResultMessages resultMessage = EffectNone;
	uint32 nTotalPercent = 0;
	uint32 bRandArray[10000];
	memset(&bRandArray, 0, sizeof(bRandArray));
	int offset = 0;
	_ITEM_EXCHANGE * pExchange;
	std::vector<uint32> ExchangeIndexList;
	uint8 sItemSlot = -1;
	uint8 sExchangeItemSlot = 0;
	Packet result(WIZ_ITEM_UPGRADE);

	pkt >> nObjectID >> nExchangeItemID;

	CNpc * pNpc = g_pMain->GetNpcPtr(nObjectID, GetZoneID());
	if (pNpc == nullptr
		|| (pNpc->GetType() != NPC_CHAOTIC_GENERATOR && pNpc->GetType() != NPC_CHAOTIC_GENERATOR2)
		|| isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| (UNIXTIME2 - m_iLastExchangeTime < 400))
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	m_iLastExchangeTime = UNIXTIME2;
	sExchangeItemSlot = FindSlotForItem(nExchangeItemID, 1) - SLOT_MAX;
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0 && ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
			break;
	}

	if (bFreeSlots < 1)
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	if (g_pMain->m_ItemExchangeArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_ItemExchangeArray)
		{
			if (itr->second->nOriginItemNum[0] == nExchangeItemID)
				ExchangeIndexList.push_back(itr->second->nIndex);
			else
				continue;
		}
	}

	foreach(itr, ExchangeIndexList)
	{
		pExchange = g_pMain->m_ItemExchangeArray.GetData(*itr);

		if (pExchange == nullptr
			|| !BifrostCheckExchange(*itr)
			|| pExchange->bRandomFlag >= 101
			|| !CheckExistItemAnd(pExchange->nOriginItemNum[0], pExchange->sOriginItemCount[0], 0, 0, 0, 0, 0, 0, 0, 0))
			continue;

		if (offset >= 10000)
			break;

		for (int i = 0; i < int(pExchange->sExchangeItemCount[0] / 5); i++)
		{
			if (i + offset >= 10000)
				break;
			bRandArray[offset + i] = pExchange->nExchangeItemNum[0];
		}

		offset += int(pExchange->sExchangeItemCount[0] / 5);
	}

	if (offset >= 10000)
		offset = 10000;

	uint32 bRandSlot = myrand(0, offset);
	uint32 nItemID = bRandArray[bRandSlot];

	_ITEM_TABLE *pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
	if (pItem == nullptr)
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	int SlotForItem = FindSlotForItem(pItem->m_iNum, 1);
	if (SlotForItem == -1)
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	if (!BifrostPieceRobItem(SLOT_MAX + sExchangeItemSlot, nExchangeItemID, 1))
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	sItemSlot = GetBifrostPieceSmashEmptySlot(pItem->m_iNum, 1);
	if (sItemSlot == -1)
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(2);
		Send(&result);
		return;
	}

	sItemSlot = (sItemSlot - SLOT_MAX);

	if (!BifrostPieceSmashGiveItem(nItemID, 1))
	{
		result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(0);
		Send(&result);
		return;
	}

	if (pItem->m_ItemType == 4)
		resultMessage = EffectWhite;
	else if (pItem->m_ItemType == 5)
		resultMessage = EffectGreen;
	else
		resultMessage = EffectRed;

	result << (uint8)ITEM_BIFROST_EXCHANGE << uint8(1) << nItemID << sItemSlot << nExchangeItemID << sExchangeItemSlot << (uint8)resultMessage;
	Send(&result);

	result.clear();
	result.SetOpcode(WIZ_OBJECT_EVENT);
	result << (uint8)OBJECT_ARTIFACT << (uint8)resultMessage << nObjectID;
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, nullptr, GetEventRoom());
	else
		SendToRegion(&result);

	bool HasObtained = false;
	if ((pItem->m_ItemType == 4 && (pItem->ItemClass == 22 || pItem->ItemClass == 33))
		|| pItem->GetNum() == 379068000)
		HasObtained = true;

	if (HasObtained)
	{
		_SERVER_SETTINGS *pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);
		if (pSettings != nullptr)
		{
			if (pSettings->DropNotice == Aktive)
			{
				result.clear();
				result.Initialize(WIZ_LOGOSSHOUT);
				result.SByte();
				result << uint8(0x02) << uint8(0x04) << GetName() << pItem->m_iNum;
				g_pMain->Send_All(&result);
			}
		}
	}
}

#pragma region CUser::BifrostCheckExchange(int nExchangeID)
bool CUser::BifrostCheckExchange(int nExchangeID)
{
	// Does the exchange exist?
	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;

	// Find free slots in the inventory, so that we can check against this later.
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0
			&& ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
			break;
	}

	// Add up the rates for this exchange to obtain a total percentage
	int nTotalPercent = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		nTotalPercent += pExchange->sExchangeItemCount[i];

	if (nTotalPercent > 9000)
		return (bFreeSlots > 0);

	int nTotalGold = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		if (pExchange->nExchangeItemNum[i] == ITEM_GOLD)
			nTotalGold += pExchange->sExchangeItemCount[i];
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	uint32 nReqWeight = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		uint32 nItemID = pExchange->nExchangeItemNum[i];

		// Does the item exist? If not, we'll ignore it (NOTE: not official behaviour).
		_ITEM_TABLE * pTable = nullptr;
		if (nItemID == 0
			|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
			continue;

		// Try to find a slot for the item.
		// If we can't find an applicable slot with our inventory as-is,
		// there's no point even checking further.
		int pos;
		if ((pos = FindSlotForItem(nItemID, 1)) < 0)
			return false;

		// Now that we have our slot, see if it's in use (i.e. if adding a stackable item)
		// If it's in use, then we don't have to worry about requiring an extra slot for this item.
		// The only caveat here is with having multiple of the same stackable item: 
		// theoretically we could give them OK, but when it comes time to adding them, we'll find that
		// there's too many of them and they can't fit in the same slot. 
		// As this isn't an issue with real use cases, we can ignore it.
		_ITEM_DATA *pItem = GetItem(pos);
		if (pItem == nullptr)
			return false;

		if (pItem->nNum == 0)
			bReqSlots++; // new item? new required slot.

						 // Also take into account weight (not official behaviour)
		nReqWeight += pTable->m_sWeight;
	}

	// Holding too much already?
	if (m_sItemWeight + nReqWeight > m_sMaxWeight)
		return false;

	if (bFreeSlots < bReqSlots)
		return false;

	if (nTotalGold + GetCoins() > COIN_MAX)
		return false;

	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}
#pragma endregion

bool CUser::BifrostPieceSmashGiveItem(uint32 nItemID, uint16 sCount)
{
	int8 pos;
	bool bNewItem = true;
	_ITEM_DATA *pItem = nullptr;

	_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return false;

	pos = FindSlotForItem(nItemID, sCount);
	if (pos < 0)
		return false;

	pItem = GetItem(pos);
	if (pItem == nullptr || pItem->nNum != 0)
		bNewItem = false;

	if (bNewItem)
		pItem->nSerialNum = g_pMain->GenerateItemSerial();

	pItem->nNum = nItemID;
	pItem->sCount += sCount;
	pItem->sDuration = pTable->m_sDuration;
	pItem->nExpirationTime = 0;

	if (pItem->sCount > MAX_ITEM_COUNT)
		pItem->sCount = MAX_ITEM_COUNT;

	if (pTable->m_bKind == 255)
		pItem->sCount = 1;

	SendStackChange(nItemID, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true, pItem->nExpirationTime);

	DateTime time;
	g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=GiveItem,ItemName=%s,ItemID=%d,Count=%d\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetZoneID(), uint16(GetX()), uint16(GetZ()), GetName().c_str(), pTable->m_sName.c_str(), pTable->m_iNum, sCount));

	return true;
}

bool CUser::BifrostPieceRobItem(uint8 bPos, uint32 nItemID, uint16 sCount)
{
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return false;

	_ITEM_DATA *pItem = nullptr;
	pItem = GetItem(bPos);

	if (pItem == nullptr)
		return false;

	if (pItem->nNum != pTable->m_iNum
		|| pItem->sCount < sCount)
		return false;

	pItem->sCount -= sCount;

	// Delete the item if the stack's now 0
	if (pItem->sCount <= 0)
		memset(pItem, 0, sizeof(_ITEM_DATA));

	SendStackChange(pTable->m_iNum, pItem->sCount, pItem->sDuration, bPos - SLOT_MAX);
	return true;
}

int CUser::GetBifrostPieceSmashEmptySlot(uint32 nItemID, uint16 sCount)
{
	_ITEM_DATA *pItem;
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return -1;

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		if (pTable->m_bCountable)
		{
			if (pItem->nNum == nItemID && pItem->sCount + sCount <= ITEMCOUNT_MAX)
				return i;

			if (pItem->nNum == 0)
				return i;
		}
		else
		{
			if (pItem->nNum == 0)
				return i;
		}
	}

	return -1;
}