#include "stdafx.h"

bool CUser::RunGiveItemCheckExchange(int nExchangeID)
{
	Packet result;

	_GIVE_LUA_ITEM_EXCHANGE * pExchange = g_pMain->m_LuaGiveItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;

	// Find free slots in the inventory, so that we can check against this later.
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0
			&& ++bFreeSlots >= ITEMS_IN_GIVE_ITEM_GROUP_LUA)
			break;
	}

	int nTotalGold = 0, nTotalLoyalty = 0;
	for (int i = 0; i < ITEMS_IN_GIVE_ITEM_GROUP_LUA; i++)
	{
		if (pExchange->nGiveItemID[i] == ITEM_GOLD)
			nTotalGold += pExchange->nGiveItemCount[i];
		else if (pExchange->nGiveItemID[i] == ITEM_COUNT)
			nTotalLoyalty += pExchange->nGiveItemCount[i];
		else if (pExchange->nGiveItemID[i] == ITEM_LADDERPOINT)
			nTotalLoyalty += pExchange->nGiveItemCount[i];
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	uint32 nReqWeight = 0;
	for (int i = 0; i < ITEMS_IN_GIVE_ITEM_GROUP_LUA; i++)
	{
		uint32 nItemID = pExchange->nGiveItemID[i];

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
		{
			result.Initialize(WIZ_QUEST);
			result << uint8(13) << uint8(3);
			Send(&result);
			return false;
		}

		// Now that we have our slot, see if it's in use (i.e. if adding a stackable item)
		// If it's in use, then we don't have to worry about requiring an extra slot for this item.
		// The only caveat here is with having multiple of the same stackable item: 
		// theoretically we could give them OK, but when it comes time to adding them, we'll find that
		// there's too many of them and they can't fit in the same slot. 
		// As this isn't an issue with real use cases, we can ignore it.
		_ITEM_DATA *pItem = GetItem(pos);
		if (pItem == nullptr)
		{
			result.Initialize(WIZ_QUEST);
			result << uint8(13) << uint8(3);
			Send(&result);
			return false;
		}

		if (pItem->nNum == 0)
			bReqSlots++; // new item? new required slot.

						 // Also take into account weight (not official behaviour)
		nReqWeight += pTable->m_sWeight;
	}

	// Holding too much already?
	if (m_sItemWeight + nReqWeight > m_sMaxWeight)
	{
		result.Initialize(WIZ_QUEST);
		result << uint8(13) << uint8(1);
		Send(&result);
		return false;
	}

	if (bFreeSlots < bReqSlots)
	{
		result.Initialize(WIZ_QUEST);
		result << uint8(13) << uint8(3);
		Send(&result);
		return false;
	}

	if (nTotalGold + GetCoins() > COIN_MAX)
	{
		result.Initialize(WIZ_QUEST);
		result << uint8(13) << uint8(2);
		Send(&result);
		return false;
	}

	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}

// Pretend you didn't see me. This really needs to go (just copying official)
bool CUser::RunGiveCheckExistItemAnd(int32 nItemID1, int16 sCount1,
	int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4,
	int32 nItemID5, int16 sCount5, int32 nItemID6, int16 sCount6, int32 nItemID7, int16 sCount7,
	int32 nItemID8, int16 sCount8, int32 nItemID9, int16 sCount9, int32 nItemID10, int16 sCount10)
{
	if (nItemID1
		&& !RunGiveCheckExistItem(nItemID1, sCount1))
		return false;

	if (nItemID2
		&& !RunGiveCheckExistItem(nItemID2, sCount2))
		return false;

	if (nItemID3
		&& !RunGiveCheckExistItem(nItemID3, sCount3))
		return false;

	if (nItemID4
		&& !RunGiveCheckExistItem(nItemID4, sCount4))
		return false;

	if (nItemID5
		&& !RunGiveCheckExistItem(nItemID5, sCount5))
		return false;

	if (nItemID6
		&& !RunGiveCheckExistItem(nItemID6, sCount6))
		return false;

	if (nItemID7
		&& !RunGiveCheckExistItem(nItemID7, sCount7))
		return false;

	if (nItemID8
		&& !RunGiveCheckExistItem(nItemID8, sCount8))
		return false;

	if (nItemID9
		&& !RunGiveCheckExistItem(nItemID9, sCount9))
		return false;

	if (nItemID10
		&& !RunGiveCheckExistItem(nItemID10, sCount10))
		return false;

	return true;
}

#pragma region  CUser::RunGiveCheckExistItem(int itemid, short count /*= 1*/)
bool CUser::RunGiveCheckExistItem(int itemid, short count /*= 1*/)
{
	if (itemid == ITEM_GOLD
		|| itemid == ITEM_COUNT
		|| itemid == ITEM_LADDERPOINT)
		return true;

	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		// This implementation fixes a bug where it ignored the possibility for multiple stacks.
		if (m_sItemArray[i].nNum == itemid
			&& m_sItemArray[i].sCount >= count)
			return true;
	}
	return false;
}
#pragma endregion

bool CUser::RunGiveItemExchange(int nExchangeID)
{
	_GIVE_LUA_ITEM_EXCHANGE * pExchange = g_pMain->m_LuaGiveItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;

	uint32 temp_nRobItemCount[10];
	int holder = 0;
	for (uint32 &x : temp_nRobItemCount)
	{
		x = pExchange->nRobItemID[holder] == 0 ? 0 : pExchange->nRobItemCount[holder];
		holder++;
	}

	if (!RunGiveItemCheckExchange(nExchangeID))
		return false;

	if (!RunGiveCheckExistItemAnd( // Do we have all of the required items?
		pExchange->nRobItemID[0], temp_nRobItemCount[0],
		pExchange->nRobItemID[1], temp_nRobItemCount[1],
		pExchange->nRobItemID[2], temp_nRobItemCount[2],
		pExchange->nRobItemID[3], temp_nRobItemCount[3],
		pExchange->nRobItemID[4], temp_nRobItemCount[4],
		pExchange->nRobItemID[5], temp_nRobItemCount[5],
		pExchange->nRobItemID[6], temp_nRobItemCount[6],
		pExchange->nRobItemID[7], temp_nRobItemCount[7],
		pExchange->nRobItemID[8], temp_nRobItemCount[8],
		pExchange->nRobItemID[9], temp_nRobItemCount[9]))
		return false;

	int nTotalGold = 0, nTotalLoyalty = 0;
	for (int i = 0; i < ITEMS_IN_GIVE_ITEM_GROUP_LUA; i++)
	{
		if (pExchange->nGiveItemID[i] <= 0
			|| pExchange->nGiveItemCount[i] <= 0)
			continue;

		if (pExchange->nGiveItemID[i] == uint32(ITEM_GOLD))
		{
			nTotalGold += pExchange->nGiveItemCount[i];
			if (!hasCoins(temp_nRobItemCount[i]))
				return false;
		}
		else if (pExchange->nGiveItemID[i] == uint32(ITEM_COUNT))
		{
			nTotalLoyalty += pExchange->nGiveItemCount[i];
			if (!hasLoyalty(temp_nRobItemCount[i]))
				return false;
		}
		else if (pExchange->nGiveItemID[i] == uint32(ITEM_LADDERPOINT))
		{
			nTotalLoyalty += pExchange->nGiveItemCount[i];
			if (!hasLoyalty(temp_nRobItemCount[i]))
				return false;
		}
	}

	if (nTotalGold > 0 || nTotalLoyalty > 0)
	{
		if (nTotalGold > 0)
		{
			if (!hasCoins(nTotalGold))
				return false;
		}

		if (nTotalLoyalty > 0)
		{
			if (!hasLoyalty(nTotalLoyalty))
				return false;
		}
	}

	for (int i = 0; i < ITEMS_IN_GIVE_ITEM_GROUP_LUA; i++)
	{
		if (pExchange->nGiveItemID[i] <= 0
			|| pExchange->nGiveItemCount[i] <= 0)
			continue;

		if (pExchange->nRobItemID[i] == uint32(ITEM_GOLD))
		{
			if (temp_nRobItemCount[i] > 0)
			{
				if (!GoldLose(temp_nRobItemCount[i], true))
					return false;
			}
			continue;
		}
		else if (pExchange->nRobItemID[i] == uint32(ITEM_COUNT)
			|| pExchange->nRobItemID[i] == uint32(ITEM_LADDERPOINT))
		{
			if (temp_nRobItemCount[i] > 0)
			{
				if (!LoyaltyLose(temp_nRobItemCount[i]))
					return false;
			}
			continue;
		}

		if (!RobItem(pExchange->nRobItemID[i], temp_nRobItemCount[i]))
			return false;
	}

	for (int i = 0; i < ITEMS_IN_GIVE_ITEM_GROUP_LUA; i++)
	{
		if (pExchange->nGiveItemID[i] <= 0
			|| pExchange->nGiveItemCount[i] <= 0)
			continue;

		if (pExchange->nGiveItemID[i] == uint32(ITEM_EXP))
		{
			ExpChange(pExchange->nGiveItemCount[i], true);
			continue;
		}
		else if (pExchange->nGiveItemID[i] == uint32(ITEM_GOLD))
		{
			GoldGain(pExchange->nGiveItemCount[i], true, false);
			continue;
		}
		else if (pExchange->nGiveItemID[i] == uint32(ITEM_COUNT))
		{
			SendLoyaltyChange(pExchange->nGiveItemCount[i], false, false, false);
			continue;
		}
		else if (pExchange->nGiveItemID[i] == uint32(ITEM_LADDERPOINT))
		{
			SendLoyaltyChange(pExchange->nGiveItemCount[i]);
			continue;
		}
		else
		{
			if (pExchange->nGiveItemTime[i] <= 0)
				pExchange->nGiveItemTime[i] = 0;

			if (pExchange->nGiveItemID[i] > 0 && pExchange->nGiveItemCount[i] > 0)
				GiveItem(pExchange->nGiveItemID[i], pExchange->nGiveItemCount[i], true, pExchange->nGiveItemTime[i]);
		}
	}

	return true;
}