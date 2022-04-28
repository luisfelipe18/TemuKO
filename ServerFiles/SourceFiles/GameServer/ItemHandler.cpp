#include "stdafx.h"
#include "DBAgent.h"

using std::vector;

#pragma region CUser::CheckWeight(uint32 nItemID, uint16 sCount)

bool CUser::CheckWeight(uint32 nItemID, uint16 sCount)
{
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);

	if(pTable == nullptr)
		return false; 

	return CheckWeight(pTable, nItemID, sCount);
}

#pragma endregion

#pragma region CUser::CheckWeight(_ITEM_TABLE * pTable, uint32 nItemID, uint16 sCount)

bool CUser::CheckWeight(_ITEM_TABLE * pTable, uint32 nItemID, uint16 sCount)
{
	return (pTable != nullptr // Make sure the item exists
		// and that the weight doesn't exceed our limit
		&& (m_sItemWeight + (pTable->m_sWeight * sCount)) <= m_sMaxWeight
		// and we have room for the item.
		&& FindSlotForItem(nItemID, sCount) >= 0);
}

#pragma endregion

#pragma region  CUser::CheckExistItem(int itemid, short count /*= 1*/)
bool CUser::CheckExistItem(int itemid, short count /*= 1*/)
{
	if (itemid == ITEM_HUNT) // this is a hunt quest kinda item     17.04.2015 -> DEFINED ITEM_HUNT 
	{
		if (QuestV2CheckQuestFinished(count))
			return true;

		return false;
	}

	if (itemid == ITEM_GOLD) // if its gold just skip this check over, will check it with hasCoin method.
		return true;

	if (itemid == ITEM_CHAT)
		return true;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE) {
		// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
		for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		{
			// This implementation fixes a bug where it ignored the possibility for multiple stacks.
			if (m_sItemKnightRoyalArray[i].nNum == itemid
				&& m_sItemKnightRoyalArray[i].sCount >= count)
				return true;
		}
	}
	else {
		// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
		for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		{
			// This implementation fixes a bug where it ignored the possibility for multiple stacks.
			if (m_sItemArray[i].nNum == itemid
				&& m_sItemArray[i].sCount >= count)
				return true;
		}
	}
	return false;
}
#pragma endregion

#pragma region CUser::GetItemCount(uint32 nItemID)
uint16 CUser::GetItemCount(uint32 nItemID)
{
	uint16 result = 0;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE) {
		// Search for the existance of all items in the player's inventory storage and onwards (No magic bags)
		for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		{
			// This implementation fixes a bug where it ignored the possibility for multiple stacks.
			if (m_sItemKnightRoyalArray[i].nNum == nItemID)
				result += m_sItemKnightRoyalArray[i].sCount;
		}
	}
	else {
		// Search for the existance of all items in the player's inventory storage and onwards (No magic bags)
		for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		{
			// This implementation fixes a bug where it ignored the possibility for multiple stacks.
			if (m_sItemArray[i].nNum == nItemID)
				result += m_sItemArray[i].sCount;
		}
	}
	return result;
}
#pragma endregion

#pragma region CUser::CheckExistItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5)

// Pretend you didn't see me. This really needs to go (just copying official)
bool CUser::CheckExistItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5)
{
	if (nItemID1
		&& !CheckExistItem(nItemID1, sCount1))
		return false;

	if (nItemID2
		&& !CheckExistItem(nItemID2, sCount2))
		return false;

	if (nItemID3
		&& !CheckExistItem(nItemID3, sCount3))
		return false;

	if (nItemID4
		&& !CheckExistItem(nItemID4, sCount4))
		return false;

	if (nItemID5
		&& !CheckExistItem(nItemID5, sCount5))
		return false;

	return true;
}

// Pretend you didn't see me. This really needs to go (just copying official)
bool CUser::CheckExistSpacialItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2, int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5, int32 nItemID6, int16 sCount6, int32 nItemID7, int16 sCount7, int32 nItemID8, int16 sCount8, int32 nItemID9, int16 sCount9, int32 nItemID10, int16 sCount10)
{
	if (nItemID1
		&& !CheckExistItem(nItemID1, sCount1))
		return false;

	if (nItemID2
		&& !CheckExistItem(nItemID2, sCount2))
		return false;

	if (nItemID3
		&& !CheckExistItem(nItemID3, sCount3))
		return false;

	if (nItemID4
		&& !CheckExistItem(nItemID4, sCount4))
		return false;

	if (nItemID5
		&& !CheckExistItem(nItemID5, sCount5))
		return false;

	if (nItemID6
		&& !CheckExistItem(nItemID6, sCount6))
		return false;

	if (nItemID7
		&& !CheckExistItem(nItemID7, sCount7))
		return false;

	if (nItemID8
		&& !CheckExistItem(nItemID8, sCount8))
		return false;

	if (nItemID9
		&& !CheckExistItem(nItemID9, sCount9))
		return false;

	if (nItemID10
		&& !CheckExistItem(nItemID10, sCount10))
		return false;

	return true;
}
#pragma endregion

#pragma region CUser::RobItem(uint32 nItemID, uint16 sCount /*= 1*/)

bool CUser::RobItem(uint32 nItemID, uint16 sCount /*= 1*/)
{
	// Allow unused exchanges.
	if (sCount == 0)
		return true;

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return false;

	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < INVENTORY_TOTAL; i++)
	{
		if (RobItem(i, pTable, sCount))
			return true;
	}
	return false;
}
#pragma endregion

#pragma region CUser::RobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)

bool CUser::RobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)
{
	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	if (pTable == nullptr)
		return false;

	_ITEM_DATA *pItem = nullptr;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE)
		pItem = GetKnightRoyaleItem(bPos);
	else
		pItem = GetItem(bPos);

	if (pItem == nullptr)
		return false;

	if (pItem->nNum != pTable->m_iNum
		|| pItem->sCount < sCount)
		return false;

	// Consumable "scrolls" (with some exceptions) use the duration/durability as a usage count
	// instead of the stack size. Interestingly, the client shows this instead of the stack size in this case.
	bool bIsConsumableScroll = (pTable->m_bKind == 255);

	if (bIsConsumableScroll)
	{
		if (sCount > pItem->sDuration)
			pItem->sDuration = 0;
		else
			pItem->sDuration -= sCount;
	}
	else
	{
		if (sCount > pItem->sCount)
			pItem->sCount = 0;
		else
			pItem->sCount -= sCount;
	}

	if (pItem->sCount <= 0 
		|| (bIsConsumableScroll && pItem->sDuration <= 0))
		memset(pItem, 0, sizeof(_ITEM_DATA));

	SendStackChange(pTable->m_iNum, pItem->sCount, pItem->sDuration, bPos - SLOT_MAX, false, pItem->nExpirationTime);

	DateTime time;

	g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=RobItem,ItemName=%s,ItemID=%d,Pos=%d,sCount=%d\n",
		time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,bPos,sCount));

	return true;
}
#pragma endregion

#pragma region CUser::RobAllItemParty(uint32 nItemID, uint16 sCount /*= 1*/)

/**
* @brief	Checks if all players in the party have sCount of item nItemID
* 			and if so, removes it.
*
* @param	nItemID	Identifier for the item.
* @param	sCount 	Stack size.
*
* @return	true if the required items were taken, false if not.
*/
bool CUser::RobAllItemParty(uint32 nItemID, uint16 sCount /*= 1*/)
{
	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return RobItem(nItemID, sCount);

	// First check to see if all users in the party have enough of the specified item.
	vector<CUser *> partyUsers;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if(pUser == nullptr
			|| !pUser->isInGame()
			|| !pUser->isInParty())
			continue;

		if (pUser != nullptr
			&& !pUser->CheckExistItem(nItemID, sCount))
			return false;

		partyUsers.push_back(pUser);
	}

	// Since all users have the required item, we can now remove them. 
	foreach (itr, partyUsers)
		(*itr)->RobItem(nItemID, sCount);

	return true;
}
#pragma endregion

#pragma region CUser::GiveItem(uint32 itemid, uint16 count, bool send_packet /*= true*/, int isRentalHour, bool isQuest)

bool CUser::GiveItem(uint32 itemid, uint16 count, bool send_packet /*= true*/, uint32 Time)
{
	int8 pos;
	bool bNewItem = true;
	_ITEM_TABLE* pTable = g_pMain->GetItemPtr( itemid );
	_ITEM_DATA *pItem = nullptr;

	if (pTable == nullptr)
		return false;	

	pos = FindSlotForItem(itemid, count);
	if (pos < 0)
		return false;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE)
		pItem = GetKnightRoyaleItem(pos);
	else
		pItem = GetItem(pos);

	if (pItem->nNum != 0 || pItem == nullptr) 
		bNewItem = false;

	if (bNewItem)
		pItem->nSerialNum = g_pMain->GenerateItemSerial();

	pItem->nNum = itemid;
	pItem->sCount += count;
	pItem->sDuration = pTable->m_sDuration;

	if (pItem->sCount > MAX_ITEM_COUNT)
		pItem->sCount = MAX_ITEM_COUNT;

	uint32 Expiration = 0;
	_ITEM_EXPIRATION_TABLE *sItem = g_pMain->m_UserExpirationItemArray.GetData(pItem->nNum);
	if (sItem != nullptr)
		Expiration = uint32(UNIXTIME + sItem->nExpiration * 86400);

	pItem->nExpirationTime = Expiration;

	uint32 renttime = uint32(UNIXTIME + 24 * 60 * 60 * Time);
	pItem->sDuration = pTable->m_sDuration;

	/*if (Time != 0)
		pItem->nExpirationTime = int32(UNIXTIME) + ((60 * 60 * 24) * Time);
	else
		pItem->nExpirationTime = 0;*/

	// This is really silly, but match the count up with the duration
	// for this special items that behave this way.
	// bakılcak
	/*if (pTable->m_bKind == 255)
		pItem->sCount = pItem->sDuration;*/

	if (send_packet)
	{
		if (GetZoneID() == ZONE_KNIGHT_ROYALE)
			SendStackChange(itemid, m_sItemKnightRoyalArray[pos].sCount, m_sItemKnightRoyalArray[pos].sDuration, pos - SLOT_MAX, true, pItem->nExpirationTime);
		else
			SendStackChange(itemid, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true, pItem->nExpirationTime);
	}

	if (!send_packet)
		SetUserAbility(false);

	if (!send_packet)
		SendItemWeight();

	DateTime time;
	g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=GiveItem,ItemName=%s,ItemID=%d,Count=%d\n",
	time.GetHour(),time.GetMinute(),time.GetSecond(),GetZoneID(),uint16(GetX()),uint16(GetZ()),GetName().c_str(),pTable->m_sName.c_str(),pTable->m_iNum,count));

	return true;
}

#pragma endregion

#pragma region CUser::SendItemWeight()

void CUser::SendItemWeight()
{
	Packet result(WIZ_WEIGHT_CHANGE);
	result << m_sItemWeight;
	Send(&result);
}

#pragma endregion

#pragma region CUser::ItemClassAvailable(_ITEM_TABLE *pTable)
/**
* @brief	Checks whether the item is designated for this class.
*
* @return	Returns true if the item is available for user's class, false elsewhere.
*/
bool CUser::ItemClassAvailable(_ITEM_TABLE *pTable)
{
	if (pTable == nullptr)
		return false;

	uint8	m_bItemClass = pTable->m_bClass;
	if (m_bItemClass > 0)
	{
		uint16 m_sClassBase = GetClass() % 100;

		switch (m_bItemClass)
		{
		case 1: // Warrior Newbie Items
			if (isWarrior())
				return m_sClassBase == 1 || m_sClassBase == 5 || m_sClassBase == 6;
			
			if (isPortuKurian())
				return m_sClassBase == 13 || m_sClassBase == 14 || m_sClassBase == 15;
		case 2:
			return m_sClassBase == 2 || m_sClassBase == 7 || m_sClassBase == 8;
		case 3:
			return m_sClassBase == 3 || m_sClassBase == 9 || m_sClassBase == 10;
		case 4:
			return m_sClassBase == 4 || m_sClassBase == 11 || m_sClassBase == 12;
		case 5:
			if (isNoviceWarrior() || isMasteredWarrior())
				return m_sClassBase == 5 || m_sClassBase == 6;

			if (isNoviceKurianPortu() || isMasteredKurianPortu())
				return m_sClassBase == 14 || m_sClassBase == 15;
		case 6:
			if (isMasteredWarrior())
				return m_sClassBase == 6;

			if (isMasteredKurianPortu())
				return m_sClassBase == 15;
		case 7:
			return m_sClassBase == 7 || m_sClassBase == 8;
		case 8:
			return m_sClassBase == 8;
		case 9:
			return m_sClassBase == 9 || m_sClassBase == 10;
		case 10:
			return m_sClassBase == 10;
		case 11:
			return m_sClassBase == 11 || m_sClassBase == 12;
		case 12:
			return m_sClassBase == 12;
		case 13:
			return m_sClassBase == 13 || m_sClassBase == 14 || m_sClassBase == 15;
		case 14:
			return m_sClassBase == 14 || m_sClassBase == 15;
		case 15:
			return m_sClassBase == 15;
		}
	}
	return true;
}


#pragma endregion

#pragma region CUser::ItemEquipAvailable(_ITEM_TABLE *pTable)

bool CUser::ItemEquipAvailable(_ITEM_TABLE *pTable)
{
	return (pTable != nullptr
		&& GetLevel() >= pTable->m_bReqLevel 
		&& GetLevel() <= pTable->m_bReqLevelMax
		&& m_bTitle >= pTable->m_bReqTitle // this is unused
		&& GetStat(STAT_STR) >= pTable->m_bReqStr 
		&& GetStat(STAT_STA) >= pTable->m_bReqSta 
		&& GetStat(STAT_DEX) >= pTable->m_bReqDex 
		&& GetStat(STAT_INT) >= pTable->m_bReqIntel 
		&& GetStat(STAT_CHA) >= pTable->m_bReqCha);
}

#pragma endregion

#pragma region CUser::InventorySystemReflesh(Packet & pkt)
void CUser::InventorySystemReflesh(Packet & pkt)
{
	Packet result(WIZ_ITEM_MOVE, uint8(2));
	result << uint8(1);

	std::vector<_ITEM_DATA> inventoryItems;

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
		inventoryItems.push_back(m_sItemArray[i]);

	std::sort(inventoryItems.begin(), inventoryItems.end(),
		[](_ITEM_DATA const &a, _ITEM_DATA const &b) { return a.nNum > b.nNum; });

	for (int k = SLOT_MAX; k < SLOT_MAX + HAVE_MAX; k++)
	{
		m_sItemArray[k] = inventoryItems[k - SLOT_MAX];
		result << uint32(m_sItemArray[k].nNum)
			<< uint16(m_sItemArray[k].sDuration)
			<< uint16(m_sItemArray[k].sCount)
			<< uint8(m_sItemArray[k].bFlag)
			<< uint16(m_sItemArray[k].sRemainingRentalTime);

		_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(m_sItemArray[k].nNum);
		if (pItemTable != nullptr)
		{
			if (pItemTable->isPetItem())
				ShowPetItemInfo(result, m_sItemArray[k].nSerialNum);
			else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, m_sItemArray[k].nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}
		else
			result << uint32(0); // Item Unique ID*/

		/*if (m_sItemArray[k].nNum == ITEM_CYPHER_RING)
			ShowCyperRingItemInfo(result, m_sItemArray[k].nSerialNum);
		else
			result << uint32(0); // Item Unique ID*/

		result << uint32(m_sItemArray[k].nExpirationTime);
	}

	Send(&result);
}
#pragma endregion

#pragma region CUser::InventorySystemKnightRoyal(Packet & pkt)
void CUser::InventorySystemKnightRoyal(Packet & pkt)
{
	if (!isInGame()
		|| m_bIsLoggingOut)
		return;

	Packet X;
	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
	{
		X.Initialize(WIZ_ITEM_MOVE);
		X << uint8(1) << uint8(0);
		Send(&X);
		return;
	}

	uint32 nItemID;
	uint8 dir, bSrcPos, bDstPos;
	pkt >> dir >> nItemID >> bSrcPos >> bDstPos;

	TRACE("Knight Royal Slot Inventory :%u Slot :%u\n", bSrcPos, bDstPos);

	_ITEM_TABLE *pTable = nullptr, *pTableSrc = nullptr;
	_ITEM_DATA *pSrcItem, *pDstItem, tmpItem;
	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| ((dir == ITEM_INVEN_SLOT
			|| dir == ITEM_SLOT_SLOT)
			&& (bDstPos > SLOT_MAX
				|| !ItemEquipAvailable(pTable)
				|| !ItemClassAvailable(pTable)))
		|| (dir == ITEM_SLOT_INVEN
			&& bSrcPos > SLOT_MAX)
		|| ((dir == ITEM_INVEN_SLOT
			|| dir == ITEM_SLOT_SLOT)
			&& bDstPos == RESERVED))
	{
		X.Initialize(WIZ_ITEM_MOVE);
		X << uint8(1) << uint8(0);
		Send(&X);
		return;
	}

	switch (dir)
	{
	case ITEM_INVEN_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemKnightRoyalArray[INVENTORY_INVENT + bSrcPos].nNum
			// Disable duplicate item moving to slot.
			|| m_sItemKnightRoyalArray[INVENTORY_INVENT + bSrcPos].isDuplicate()
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPosKnightRoyal(pTable, bDstPos))
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}

		pSrcItem = &m_sItemKnightRoyalArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemKnightRoyalArray[bDstPos];
		break;

	case ITEM_SLOT_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemKnightRoyalArray[bSrcPos].nNum)
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}

		pSrcItem = &m_sItemKnightRoyalArray[bSrcPos];
		pDstItem = &m_sItemKnightRoyalArray[INVENTORY_INVENT + bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0
			&& (pTableSrc == nullptr
				|| pTableSrc->m_bSlot != pTable->m_bSlot
				|| !IsValidSlotPosKnightRoyal(pTable, bSrcPos)))
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}
		break;

	case ITEM_INVEN_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemKnightRoyalArray[INVENTORY_INVENT + bSrcPos].nNum)
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}

		pSrcItem = &m_sItemKnightRoyalArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemKnightRoyalArray[INVENTORY_INVENT + bDstPos];
		break;

	case ITEM_SLOT_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemKnightRoyalArray[bSrcPos].nNum
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPosKnightRoyal(pTable, bDstPos))
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}

		pSrcItem = &m_sItemKnightRoyalArray[bSrcPos];
		pDstItem = &m_sItemKnightRoyalArray[bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0
			&& (pTableSrc == nullptr
				|| pTableSrc->m_bSlot != pTable->m_bSlot))
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}
		break;
	default:
		{
			X.Initialize(WIZ_ITEM_MOVE);
			X << uint8(1) << uint8(0);
			Send(&X);
			return;
		}
	}

	// If there's an item already in the target slot already, we need to just swap the items
	if (pDstItem->nNum != 0)
	{
		if (pTableSrc != nullptr
			&& pTableSrc->m_bCountable == 1
			&& pDstItem->nNum == pSrcItem->nNum)
		{
			pSrcItem->sCount += pDstItem->sCount;
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Shift the item over
			memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data

		}
		else
		{
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
	}
	// Since there's no way to move a partial stack using this handler, just overwrite the destination.
	else
	{
		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Shift the item over
		memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data
	}

	// If equipping/de-equipping an item
	if (dir == ITEM_INVEN_SLOT
		|| dir == ITEM_SLOT_INVEN)
	{
		// Re-update item stats
		SetUserAbility();
	}

	SendItemMove(1, 1);
	SendItemWeight();

	// Update everyone else, so that they can see your shiny new items (you didn't take them off did you!? DID YOU!?)
	switch (dir)
	{
	case ITEM_INVEN_SLOT:
		UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		break;
	case ITEM_SLOT_INVEN:
		UserLookChange(bSrcPos, 0, 0);
		break;
	case ITEM_SLOT_SLOT:
		UserLookChange(bSrcPos, pSrcItem->nNum, pSrcItem->sDuration);
		UserLookChange(bDstPos, pDstItem->nNum, pDstItem->sDuration);
		break;
	}
}
#pragma endregion

#pragma region CUser::InventorySystem(Packet & pkt)
void CUser::InventorySystem(Packet & pkt)
{
	if (!isInGame() || m_bIsLoggingOut)
		return;

	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| GetZoneID() == ZONE_CHAOS_DUNGEON)
		goto fail_return;

	uint32 nItemID;
	uint8 dir, bSrcPos, bDstPos;

	pkt >> dir >> nItemID >> bSrcPos >> bDstPos;

	_ITEM_TABLE *pTable = nullptr, *pTableSrc = nullptr;
	_ITEM_DATA *pSrcItem, *pDstItem, tmpItem;
	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| ((dir == ITEM_INVEN_SLOT || dir == ITEM_SLOT_SLOT)
			&& (bDstPos > SLOT_MAX 
				|| !ItemEquipAvailable(pTable) 
				|| !ItemClassAvailable(pTable)))
		|| (dir == ITEM_SLOT_INVEN 
			&& bSrcPos > SLOT_MAX)
		|| ((dir == ITEM_INVEN_SLOT 
			|| dir == ITEM_SLOT_SLOT) 
			&& bDstPos == RESERVED))
		goto fail_return;

	if (isGM())
		g_pMain->SendFormattedNoticeToPlayer(this, "Moving : [ItemID] = %d", false, nItemID);

	switch (dir)
	{
	case ITEM_MBAG_TO_MBAG:
		if (bDstPos >= MBAG_TOTAL || bSrcPos >= MBAG_TOTAL
			// We also need to make sure that if we're setting an item in a magic bag, we need to actually
			// have a magic back to put the item in!
			|| (INVENTORY_MBAG + bDstPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bDstPos >= INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_MBAG + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_MBAG + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_MBAG + bDstPos];
		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		break;
	case ITEM_MBAG_TO_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= MBAG_TOTAL
			// We also need to make sure that if we're taking an item from a magic bag, we need to actually
			// have a magic back to take it from!
			|| (INVENTORY_MBAG + bSrcPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bSrcPos >= INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_MBAG + bSrcPos].nNum
			/*|| m_sItemArray[INVENTORY_INVENT + bDstPos].nNum > 0*/)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_MBAG + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];
		break;
	case ITEM_INVEN_TO_MBAG:
		if (bDstPos >= MBAG_TOTAL || bSrcPos >= HAVE_MAX
			// We also need to make sure that if we're adding an item to a magic bag, we need to actually
			// have a magic back to put the item in!
			|| (INVENTORY_MBAG + bDstPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bDstPos >= INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum
			|| (m_sItemArray[INVENTORY_MBAG + bDstPos].nNum > 0
				&& (pTable->m_bCountable == 1 || pTable->m_bKind == 255)
				&& nItemID != m_sItemArray[INVENTORY_MBAG + bDstPos].nNum))
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_MBAG + bDstPos];
		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		break;
	case ITEM_COSP_TO_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= COSP_MAX
			// Make sure that the item actually exists there.
			|| (bSrcPos == COSP_TATTO && nItemID != m_sItemArray[CTATTOO].nNum)
			|| (bSrcPos == COSP_FAIRY && nItemID != m_sItemArray[CFAIRY].nNum)
			|| bSrcPos == COSP_BAG1
			|| bSrcPos == COSP_BAG2)
			goto fail_return;

		if (bSrcPos == COSP_FAIRY)
			pSrcItem = &m_sItemArray[CFAIRY];
		else if (bSrcPos == COSP_TATTO)
			pSrcItem = &m_sItemArray[CTATTOO];
		else
			pSrcItem = &m_sItemArray[INVENTORY_COSP + bSrcPos];

		pDstItem = &m_sItemArray[SLOT_MAX + bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0
			&& (pTableSrc == nullptr 
			|| pTableSrc->m_bSlot != pTable->m_bSlot 
			|| !IsValidSlotPos(pTableSrc, bSrcPos)))
			goto fail_return;

		if (nItemID == OFFLINE_FISHING_ITEM
			|| nItemID == OFFLINE_MERCHANT_ITEM
			|| nItemID == OFFLINE_MINING_ITEM)
		{
			Packet result;
			std::string m_sAutosystem;

			if (nItemID == OFFLINE_FISHING_ITEM)
				m_sAutosystem = string_format("[Offline Fishing] Not active.");
			if (nItemID == OFFLINE_MERCHANT_ITEM)
				m_sAutosystem = string_format("[Offline Merchant] Not active.");
			if (nItemID == OFFLINE_MINING_ITEM)
				m_sAutosystem = string_format("[Offline Mining] Not active.");

			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_INFOMESSAGE);
			result << m_sAutosystem;
			Send(&result);
		}break;
	case ITEM_INVEN_TO_COSP:
		if (bDstPos >= COSP_MAX + MBAG_COUNT || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[SLOT_MAX + bSrcPos].nNum
			|| !IsValidSlotPos(pTable, bDstPos))
			goto fail_return;

		pSrcItem = &m_sItemArray[SLOT_MAX + bSrcPos];

		// Slot 47 is reserved for the fairy. 
		// INVENTORY_COSP = 14 + 28 = 42 + despos (5 -or 6)= 48 or 49 for magicbag
		// If we're setting a magic bag...
		if (bDstPos == COSP_BAG1 || bDstPos == COSP_BAG2)
		{
			pDstItem = &m_sItemArray[INVENTORY_COSP + bDstPos + 2];
			// Can't replace existing magic bag.
			if (pDstItem->nNum != 0
				// Can't set any old item in the bag slot, it must be a bag.
				|| pTable->m_bSlot != ItemSlotBag)
				goto fail_return;
		}
		else if (bDstPos == COSP_FAIRY)
			pDstItem = &m_sItemArray[CFAIRY];
		else if (bDstPos == COSP_TATTO)
			pDstItem = &m_sItemArray[CTATTOO];
		else
			pDstItem = &m_sItemArray[INVENTORY_COSP + bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0 
			&& !IsValidSlotPos(pTableSrc, bDstPos))
			goto fail_return;

		if (nItemID == OFFLINE_FISHING_ITEM
			|| nItemID == OFFLINE_MERCHANT_ITEM
			|| nItemID == OFFLINE_MINING_ITEM)
		{
			Packet result;
			std::string m_sAutosystem;

			if (nItemID == OFFLINE_FISHING_ITEM)
				m_sAutosystem = string_format("[Offline Fishing] Activated.");
			if (nItemID == OFFLINE_MERCHANT_ITEM)
				m_sAutosystem = string_format("[Offline Merchant] Activated.");
			if (nItemID == OFFLINE_MINING_ITEM)
				m_sAutosystem = string_format("[Offline Mining] Activated.");

			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_INFOMESSAGE);
			result << m_sAutosystem;
			Send(&result);
		}break;
	case ITEM_INVEN_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum
			// Disable duplicate item moving to slot.
			|| m_sItemArray[INVENTORY_INVENT + bSrcPos].isDuplicate()
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPos(pTable, bDstPos))
			goto fail_return;

		if (bDstPos == SHOULDER)
		{
			if (pTable->isPetItem())
			{
				if (m_PettingOn != nullptr)
					goto fail_return;
			}
		}

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[bDstPos];

		if (nItemID == AUTOMATIC_MINING
			|| nItemID == AUTOMATIC_FISHING
			|| nItemID == ROBIN_LOOT_ITEM
			|| nItemID == AUTOMATIC_DROP_FISHING
			|| nItemID == AUTOMATIC_DROP_MINING)
		{
			Packet result;
			std::string m_sAutosystem;

			if (nItemID == AUTOMATIC_FISHING)
				m_sAutosystem = string_format("[Auto Fishing] Activated.");
			if (nItemID == AUTOMATIC_MINING)
				m_sAutosystem = string_format("[Auto Mining] Activated.");
			if (nItemID == ROBIN_LOOT_ITEM)
				m_sAutosystem = string_format("[Robin Loot Item] Activated.");

			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_INFOMESSAGE);
			result << m_sAutosystem;
			Send(&result);
		}break;
	case ITEM_SLOT_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[bSrcPos].nNum)
			goto fail_return;

		if (bSrcPos == SHOULDER)
		{
			if (pTable->isPetItem())
			{
				if (m_PettingOn != nullptr)
					goto fail_return;
			}
		}

		pSrcItem = &m_sItemArray[bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0
			&& (pTableSrc == nullptr 
				|| pTableSrc->m_bSlot != pTable->m_bSlot 
				|| !IsValidSlotPos(pTable, bSrcPos)))
			goto fail_return;

		if (nItemID == AUTOMATIC_MINING
			|| nItemID == AUTOMATIC_FISHING
			|| nItemID == ROBIN_LOOT_ITEM
			|| nItemID == AUTOMATIC_DROP_MINING
			|| nItemID == AUTOMATIC_DROP_FISHING)
		{
			Packet result;
			std::string m_sAutosystem;

			if (nItemID == AUTOMATIC_FISHING)
				m_sAutosystem = string_format("[Auto Fishing] Not active.");
			if (nItemID == AUTOMATIC_MINING)
				m_sAutosystem = string_format("[Auto Mining] Not active.");
			if (nItemID == ROBIN_LOOT_ITEM)
				m_sAutosystem = string_format("[Robin Loot Item] Not active.");

			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_INFOMESSAGE);
			result << m_sAutosystem;
			Send(&result);
		}break;
	case ITEM_INVEN_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];
		break;
	case ITEM_SLOT_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[bSrcPos].nNum
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPos(pTable, bDstPos))
			goto fail_return;

		pSrcItem = &m_sItemArray[bSrcPos];
		pDstItem = &m_sItemArray[bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0 
			&& (pTableSrc == nullptr 
				|| pTableSrc->m_bSlot != pTable->m_bSlot))
			goto fail_return;
		break;
	case ITEM_INVEN_TO_PET:
		if (m_PettingOn == nullptr
			|| bSrcPos >= HAVE_MAX
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[SLOT_MAX + bSrcPos];
		pDstItem = &m_PettingOn->sItem[bDstPos - 3];
		break;
	default:
		goto fail_return;
		break;
	}

	// If there's an item already in the target slot already, we need to just swap the items
	if (pDstItem->nNum != 0)
	{
		if (pTableSrc != nullptr
			&& pTableSrc->m_bCountable == 1
			&& pDstItem->nNum == pSrcItem->nNum)
		{
			pSrcItem->sCount += pDstItem->sCount;
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Shift the item over
			memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data

		}
		else
		{
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
	}
	// Since there's no way to move a partial stack using this handler, just overwrite the destination.
	else
	{
		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Shift the item over
		memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data
	}

	// If equipping/de-equipping an item
	if (dir == ITEM_INVEN_SLOT || dir == ITEM_SLOT_INVEN
		// or moving an item to/from our cospre item slots
		|| dir == ITEM_INVEN_TO_COSP || dir == ITEM_COSP_TO_INVEN
		|| dir == ITEM_SLOT_SLOT)
	{
		// Re-update item stats
		SetUserAbility(false);
	}

	SendItemMove(1, 1);
	SendItemWeight();

	// Update everyone else, so that they can see your shiny new items (you didn't take them off did you!? DID YOU!?)
	switch (dir)
	{
	case ITEM_INVEN_SLOT:
		if (bDstPos == 1)
		{
			_ITEM_DATA *pItem = GetItem(CHELMET);
			if (pItem != nullptr && (pItem->nNum == 0 || m_bIsHidingCospre))
				UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		}
		else if (bDstPos == 4 || bDstPos == 10 || bDstPos == 12 || bDstPos == 13)
		{
			_ITEM_DATA *pItem = GetItem(CTOP);
			if (pItem != nullptr && (pItem->nNum == 0 || m_bIsHidingCospre))
				UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		}
		else
			UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		break;
	case ITEM_INVEN_TO_COSP:
		if (bDstPos != 5 && bDstPos != 6)
			UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		break;

	case ITEM_SLOT_INVEN:
		if (bSrcPos == 1)
		{
			_ITEM_DATA *pItem = GetItem(CHELMET);
			if (pItem != nullptr && (pItem->nNum == 0 || m_bIsHidingCospre))
				UserLookChange(bSrcPos, 0, 0);
		}
		else if (bSrcPos == 4 || bSrcPos == 10 || bSrcPos == 12 || bSrcPos == 13)
		{
			_ITEM_DATA *pItem = GetItem(CTOP);
			if (pItem != nullptr && (pItem->nNum == 0 || m_bIsHidingCospre))
				UserLookChange(bSrcPos, 0, 0);
		}
		else
			UserLookChange(bSrcPos, 0, 0);
		break;
	case ITEM_COSP_TO_INVEN:
		if (bSrcPos == COSP_BREAST)
		{
			uint8 equippedItems[] = { BREAST, LEG, GLOVE, FOOT };
			foreach_array(i, equippedItems)
			{
				_ITEM_DATA * pItem = GetItem(equippedItems[i]);

				if (pItem == nullptr)
				{
					UserLookChange(equippedItems[i], 0, 0);
					continue;
				}
				UserLookChange(equippedItems[i], pItem->nNum, pItem->sDuration);
			}
		}
		else if (bSrcPos == COSP_HELMET)
		{
			uint8 equippedItems[] = { HEAD };
			foreach_array(i, equippedItems)
			{
				_ITEM_DATA * pItem = GetItem(equippedItems[i]);

				if (pItem == nullptr)
				{
					UserLookChange(equippedItems[i], 0, 0);
					continue;
				}
				UserLookChange(equippedItems[i], pItem->nNum, pItem->sDuration);
			}
		}
		else
			UserLookChange(bSrcPos, 0, 0);
		break;

	case ITEM_SLOT_SLOT:
		UserLookChange(bSrcPos, pSrcItem->nNum, pSrcItem->sDuration);
		UserLookChange(bDstPos, pDstItem->nNum, pDstItem->sDuration);
		break;
	}

	return;

fail_return:
	SendItemMove(1, 0);
}
#pragma endregion

#pragma region CUser::ItemMove(Packet & pkt)

/**
* @brief	Handles the item move related packets.
*/
void CUser::ItemMove(Packet & pkt)
{
	if (!isInGame() 
		|| m_bIsLoggingOut)
		return;

	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| GetZoneID() == ZONE_CHAOS_DUNGEON)
		goto fail_return;

	uint8 type;

	pkt >> type;

	switch (type)
	{
	case 2:
		InventorySystemReflesh(pkt);
		break;
	case 3:
		InventorySystemKnightRoyal(pkt);
		break;
	default:
		InventorySystem(pkt);
		break;
	}
fail_return:
	SendItemMove(1, 0);
}

#pragma endregion

#pragma region CUser::CheckExchange(int nExchangeID)
bool CUser::CheckExchange(int nExchangeID)
{
	Packet result;
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
#pragma endregion

#pragma region CUser::CheckExchangeExp(int nExchangeID, int32 selectedAward /* -1 */, uint8 giveAllExp /*= 0*/)
bool CUser::CheckExchangeExp(int nExchangeID, int32 selectedAward /* -1 */, uint8 giveAllExp /*= 0*/)
{
	Packet result;
	// Does the exchange exist?
	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);
	_ITEM_EXCHANGE_EXP * pExchangeExp = g_pMain->m_ItemExchangeExpArray.GetData(nExchangeID);
	if (pExchange == nullptr 
		|| selectedAward > 4)
		return false;

	if (m_bSelectMsgFlag == 5 
		&& selectedAward < 0)
		return false;

	if (!giveAllExp 
		&& pExchangeExp 
		&& selectedAward < 0)
		return false;

	// Find free slots in the inventory, so that we can check against this later.
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA *pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		if (pItem->nNum == 0)
			bFreeSlots++;
	}

	// Add up the rates for this exchange to obtain a total percentage
	int nTotalPercent = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		uint32 nItem = pExchange->nExchangeItemNum[i];
		if (nItem == ITEM_EXP 
			|| nItem == ITEM_COUNT 
			|| nItem == ITEM_GOLD)
			continue;

		nTotalPercent += pExchange->sExchangeItemCount[i];
	}
	int nTotalGold = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		if (pExchange->nExchangeItemNum[i] == ITEM_GOLD)
			nTotalGold += pExchange->sExchangeItemCount[i];
	}
	if (pExchangeExp != nullptr 
		&& selectedAward >= 0)
	{
		uint32 nItem = pExchangeExp->nExchangeItemNum[selectedAward];
		if (!(nItem == ITEM_EXP) 
			&& !(nItem == ITEM_COUNT) 
			&& !(nItem == ITEM_GOLD))
			nTotalPercent += pExchangeExp->sExchangeItemCount[selectedAward];
	}
	else if (pExchangeExp != nullptr && selectedAward == -1)
	{
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			uint32 nItem = pExchangeExp->nExchangeItemNum[i];
			if (nItem == ITEM_EXP 
				|| nItem == ITEM_COUNT
				|| nItem == ITEM_GOLD)
				continue;

			nTotalPercent += pExchangeExp->sExchangeItemCount[i];
		}
	}

	if (nTotalPercent > 9000)
		return (bFreeSlots > 0);

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	uint32 nReqWeight = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		uint32 nItemID = pExchange->nExchangeItemNum[i];

		_ITEM_TABLE * pTable = nullptr;
		if (nItemID == 0 
			|| nItemID == ITEM_EXP 
			|| nItemID == ITEM_COUNT 
			|| nItemID == ITEM_GOLD
			|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
			continue;

		int pos;
		if ((pos = FindSlotForItem(nItemID, 1)) < 0)
		{
			result.Initialize(WIZ_QUEST);
			result << uint8(13) << uint8(3);
			Send(&result);
			return false;
		}

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

	if (pExchangeExp != nullptr 
		&& selectedAward >= 0)
	{
		uint32 nItemID = pExchangeExp->nExchangeItemNum[selectedAward];
		if ((nItemID == ITEM_EXP) 
			|| (nItemID == ITEM_COUNT) 
			|| (nItemID == ITEM_GOLD))
			return true;

		_ITEM_TABLE * pTable = nullptr;
		if (nItemID == 0 
			|| nItemID == ITEM_EXP 
			|| nItemID == ITEM_COUNT
			|| nItemID == ITEM_GOLD
			|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
			goto Check;

		int pos;
		if ((pos = FindSlotForItem(nItemID, 1)) < 0)
		{
			result.Initialize(WIZ_QUEST);
			result << uint8(13) << uint8(3);
			Send(&result);
			return false;
		}

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
	else if (pExchangeExp != nullptr 
		&& selectedAward == -1)
	{
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			uint32 nItemID = pExchangeExp->nExchangeItemNum[i];

			_ITEM_TABLE * pTable = nullptr;
			if (nItemID == 0 
				|| nItemID == ITEM_EXP 
				|| nItemID == ITEM_COUNT 
				|| nItemID == ITEM_GOLD
				|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
				continue;

			int pos;
			if ((pos = FindSlotForItem(nItemID, 1)) < 0)
			{
				result.Initialize(WIZ_QUEST);
				result << uint8(13) << uint8(3);
				Send(&result);
				return false;
			}

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
	}
Check:
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
#pragma endregion

#pragma region CUser::RunQuestExchange(int nExchangeID, int32 selectedAward /* = -1 */, uint8 giveAllExp /* = 0 */)
/**
* @brief	Runs the quest item exchange process.
*
* @param	nExchangeID	the nIndex in the ITEM_EXCHANGE table.
* @param	count		the item count.
*/
bool CUser::RunQuestExchange(int nExchangeID, int32 selectedAward /* = -1 */, uint8 giveAllExp /* = 0 */)
{
	Packet result;

	if (!CheckExchangeExp(nExchangeID, selectedAward, giveAllExp))
		return false;

	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID); // get the exchange data
	_ITEM_EXCHANGE_EXP * pExchangeExp = g_pMain->m_ItemExchangeExpArray.GetData(nExchangeID); // get the exchange data

	if (pExchange == nullptr)
		return false;

	// logical holder for item count in the ITEM_EXCHANGE table
	uint32 temp_sOriginItemCount[ITEMS_IN_ORIGIN_GROUP];
	int holder = 0;
	for (uint32 &x : temp_sOriginItemCount)
	{
		x = pExchange->nOriginItemNum[holder] == 0 ? 0 : pExchange->sOriginItemCount[holder];
		holder++;
	}

	if (!CheckExchange(nExchangeID)			// Is it a valid exchange (do we have room?)
		|| pExchange->bRandomFlag > 101)	// We handle flags from 0-101 only. Anything else is broken.
		return false;

	if (!CheckExistItemAnd( // Do we have all of the required items?
		pExchange->nOriginItemNum[0], temp_sOriginItemCount[0],
		pExchange->nOriginItemNum[1], temp_sOriginItemCount[1],
		pExchange->nOriginItemNum[2], temp_sOriginItemCount[2],
		pExchange->nOriginItemNum[3], temp_sOriginItemCount[3],
		pExchange->nOriginItemNum[4], temp_sOriginItemCount[4]))
		return false;

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		if (pExchange->nOriginItemNum[i] == uint32(ITEM_GOLD))
		{
			if (!hasCoins(temp_sOriginItemCount[i]))
				return false;
		}
	}

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		if (pExchange->nOriginItemNum[i] == uint32(ITEM_HUNT))
			continue; // if quest hunt then continue nothing to take
		else if (pExchange->nOriginItemNum[i] == uint32(ITEM_GOLD) 
			&& temp_sOriginItemCount[i] > 0)
		{ // gold
			if (!GoldLose(temp_sOriginItemCount[i], true))
				return false;
			continue;
		}
		// These checks are a little pointless, but remove the required items as well.
		if (!RobItem(pExchange->nOriginItemNum[i], temp_sOriginItemCount[i]))
			return false;
	}

	std::map<uint32, uint32> sExchangeMap;
	if (pExchangeExp != nullptr && selectedAward > -1)
	{
		if (pExchangeExp->sExchangeItemCount[selectedAward] == 0)
			return false;

		uint32 nItemID = pExchangeExp->nExchangeItemNum[selectedAward];
		uint32 nCount = pExchangeExp->sExchangeItemCount[selectedAward];

		auto itr = sExchangeMap.find(nItemID);
		if (itr == sExchangeMap.end())
			sExchangeMap.insert(std::make_pair(nItemID, nCount));
		else if (itr->second < nCount && GetPremium() > 0)
			itr->second = nCount;
	}
	else if (pExchangeExp != nullptr && selectedAward == -1)
	{
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			if (pExchangeExp->sExchangeItemCount[i] == 0)
				continue;

			uint32 nItemID = pExchangeExp->nExchangeItemNum[i];
			uint32 nCount = pExchangeExp->sExchangeItemCount[i];

			auto itr = sExchangeMap.find(nItemID);
			if (itr == sExchangeMap.end())
				sExchangeMap.insert(std::make_pair(nItemID, nCount));
			else if (itr->second < nCount && GetPremium() > 0)
				itr->second = nCount;
		}
	}

	// No random element? We're just exchanging x items for y items.
	if (pExchange->bRandomFlag != 101)
	{
		if (pExchangeExp == nullptr 
			&& selectedAward != -1)
		{
			if (pExchange->sExchangeItemCount[selectedAward] == 0)
				return false;

			uint32 nItemID = pExchange->nExchangeItemNum[selectedAward];
			uint32 nCount = pExchange->sExchangeItemCount[selectedAward];

			auto itr = sExchangeMap.find(nItemID);
			if (itr == sExchangeMap.end())
				sExchangeMap.insert(std::make_pair(nItemID, nCount));
			else if (itr->second < nCount && GetPremium() > 0)
				itr->second = nCount;
		}
		else
		{
			for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
			{
				if (pExchange->sExchangeItemCount[i] == 0)
					continue;

				uint32 nItemID = pExchange->nExchangeItemNum[i];
				uint32 nCount = pExchange->sExchangeItemCount[i];

				auto itr = sExchangeMap.find(nItemID);
				if (itr == sExchangeMap.end())
					sExchangeMap.insert(std::make_pair(nItemID, nCount));
				else if (itr->second < nCount && GetPremium() > 0)
					itr->second = nCount;
			}
		}
	}
	// For 101, the rates are determined by sExchangeItemCount.
	else if (pExchange->bRandomFlag == 101)
	{
		uint32 nTotalPercent = 0;
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
			nTotalPercent += pExchange->sExchangeItemCount[i];

		// If they add up to more than 100%, 
		if (nTotalPercent > 10000)
			return false;

		uint8 bRandArray[10000];
		memset(&bRandArray, 0, sizeof(bRandArray));

		// Copy the counts, as we're going to adjust them locally.
		uint32 sExchangeCount[ITEMS_IN_EXCHANGE_GROUP];
		memcpy(&sExchangeCount, &pExchange->sExchangeItemCount, sizeof(pExchange->sExchangeItemCount));

		// Build array of exchange item slots (0-4)
		int offset = 0;
		for (int n = 0, i = 0; n < ITEMS_IN_EXCHANGE_GROUP; n++)
		{
			if (sExchangeCount[n] > 0)
			{
				memset(&bRandArray[offset], n, sExchangeCount[n]);
				offset += sExchangeCount[n];
			}
		}

		// Pull our exchange item slot out of our hat (the array we generated).
		uint8 bRandSlot = bRandArray[myrand(0, 9999)];
		uint32 nItemID = pExchange->nExchangeItemNum[bRandSlot];

		// Finally, give our item.
		if (nItemID == uint32(ITEM_EXP) // exp
			|| nItemID == uint32(ITEM_GOLD)) // gold
			sExchangeMap.insert(std::make_pair(nItemID, 10000));
		else if (nItemID != ITEM_SKILL)
			sExchangeMap.insert(std::make_pair(nItemID, 1));
	}

	int counter = 0;
	uint32 sExchangeItemNum[ITEMS_IN_EXCHANGE_GROUP];
	uint32 sExchangeItemCount[ITEMS_IN_EXCHANGE_GROUP];
	memset(&sExchangeItemNum, 0, sizeof(sExchangeItemNum));
	memset(&sExchangeItemCount, 0, sizeof(sExchangeItemCount));

	foreach(itr, sExchangeMap)
	{
		uint32 nItemID = itr->first;
		uint32 nCount = itr->second;

		if (counter < ITEMS_IN_EXCHANGE_GROUP)
		{
			sExchangeItemNum[counter] = nItemID;
			sExchangeItemCount[counter] = nCount;
			counter++;
		}

		if (nItemID == uint32(ITEM_EXP) && nCount > 0)
		{ // exp
			ExpChange(nCount, true);
			continue;
		}
		else if (nItemID == uint32(ITEM_GOLD) && nCount > 0)
		{ // gold
			GoldGain(nCount, true, false);
			continue;
		}
		else if (nItemID == uint32(ITEM_SKILL) && nCount > 0)
		{ //Skill
			continue;
		}
		else if (nItemID == uint32(ITEM_COUNT) && nCount > 0)
		{ // count
			SendLoyaltyChange(nCount, false, false, false);
			continue;
		}
		else if (nItemID == uint32(ITEM_LADDERPOINT) && nCount > 0)
		{ // Ladder point
			SendLoyaltyChange(nCount);
			continue;
		}
		else
			GiveItem(nItemID, nCount);
	}
		QuestV2ShowGiveItem(sExchangeItemNum[0], sExchangeItemCount[0],
		sExchangeItemNum[1], sExchangeItemCount[1],
		sExchangeItemNum[2], sExchangeItemCount[2],
		sExchangeItemNum[3], sExchangeItemCount[3],
		sExchangeItemNum[4], sExchangeItemCount[4]);

	return true;
}
#pragma endregion

#pragma region CUser::RunExchange(int nExchangeID, uint16 count /* = 0 */, bool isQuest /* = false */, int32 SelectedAward /* = -1 */,  uint16 giveCount /* = 1 */)
/**
* @brief	Runs the item exchange process.
*
* @param	nExchangeID	the nIndex in the ITEM_EXCHANGE table.
* @param	count		the item count.
*/
bool CUser::RunExchange(int nExchangeID, uint16 count /* = 0 */, bool isQuest /* = false */, int32 SelectedAward /* = -1 */, uint16 giveCount /* = 1 */)
{
	if (isQuest)
		return false;

	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

	uint16 temp_sOriginItemCount0 = 0;
	uint16 temp_sOriginItemCount1 = 0;
	uint16 temp_sOriginItemCount2 = 0;
	uint16 temp_sOriginItemCount3 = 0;
	uint16 temp_sOriginItemCount4 = 0;

	uint16 temp_sCount = 0;

	if (pExchange != nullptr)
	{
		uint16 sItemCount[5];
		sItemCount[0] = GetItemCount(pExchange->nOriginItemNum[0]);
		sItemCount[1] = GetItemCount(pExchange->nOriginItemNum[1]);
		sItemCount[2] = GetItemCount(pExchange->nOriginItemNum[2]);
		sItemCount[3] = GetItemCount(pExchange->nOriginItemNum[3]);
		sItemCount[4] = GetItemCount(pExchange->nOriginItemNum[4]);

		temp_sCount = sItemCount[0]; // burada bi sacmalik var gibi sanki

		for (int i = 1; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			if (sItemCount[i] < temp_sCount && sItemCount[i] != 0)
				temp_sCount = sItemCount[i];
		}

		if (temp_sCount >= count)
			temp_sCount = count;

		temp_sOriginItemCount0 = pExchange->nOriginItemNum[0] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[0] : temp_sCount);
		temp_sOriginItemCount1 = pExchange->nOriginItemNum[1] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[1] : temp_sCount);
		temp_sOriginItemCount2 = pExchange->nOriginItemNum[2] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[2] : temp_sCount);
		temp_sOriginItemCount3 = pExchange->nOriginItemNum[3] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[3] : temp_sCount);
		temp_sOriginItemCount4 = pExchange->nOriginItemNum[4] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[4] : temp_sCount);

	}

	if (pExchange == nullptr
		// Is it a valid exchange (do we have room?)
		|| !CheckExchange(nExchangeID)
		// We handle flags from 0-101 only. Anything else is broken.
		|| pExchange->bRandomFlag > 101
		// Do we have all of the required items?
		|| !CheckExistItemAnd(
			pExchange->nOriginItemNum[0], temp_sOriginItemCount0,
			pExchange->nOriginItemNum[1], temp_sOriginItemCount1,
			pExchange->nOriginItemNum[2], temp_sOriginItemCount2,
			pExchange->nOriginItemNum[3], temp_sOriginItemCount3,
			pExchange->nOriginItemNum[4], temp_sOriginItemCount4)
		// These checks are a little pointless, but remove the required items as well.
		|| !RobItem(pExchange->nOriginItemNum[0], temp_sOriginItemCount0)
		|| !RobItem(pExchange->nOriginItemNum[1], temp_sOriginItemCount1)
		|| !RobItem(pExchange->nOriginItemNum[2], temp_sOriginItemCount2)
		|| !RobItem(pExchange->nOriginItemNum[3], temp_sOriginItemCount3)
		|| !RobItem(pExchange->nOriginItemNum[4], temp_sOriginItemCount4))
		return false;

	// No random element? We're just exchanging x items for y items.
	if (!pExchange->bRandomFlag)
	{
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			if (count == 0)
				GiveItem(pExchange->nExchangeItemNum[i], pExchange->sExchangeItemCount[i]);
			else
				GiveItem(pExchange->nExchangeItemNum[i], temp_sCount);
		}

		if (count > 0)
			QuestV2ShowGiveItem(pExchange->nExchangeItemNum[0], temp_sCount,
				pExchange->nExchangeItemNum[1], temp_sCount,
				pExchange->nExchangeItemNum[2], temp_sCount,
				pExchange->nExchangeItemNum[3], temp_sCount,
				pExchange->nExchangeItemNum[4], temp_sCount);
	}
	// For these items the rate set by bRandomFlag.
	else if (pExchange->bRandomFlag <= 100)
	{
		int rand = myrand(0, 1000 * pExchange->bRandomFlag) / 1000;
		if (rand == 5)
			rand = 4;

		if (rand <= 4)
			GiveItem(pExchange->nExchangeItemNum[rand], pExchange->sExchangeItemCount[rand]);
	}
	// For 101, the rates are determined by sExchangeItemCount.
	else if (pExchange->bRandomFlag == 101)
	{
		uint32 nTotalPercent = 0;
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
			nTotalPercent += pExchange->sExchangeItemCount[i];

		// If they add up to more than 100%, 
		if (nTotalPercent > 10000)
		{
			TRACE("Exchange %d is invalid. Rates add up to more than 100%% (%d%%)", nExchangeID, nTotalPercent / 100);
			return false;
		}

		// Holy stack batman! We're just going ahead and copying official for now.
		// NOTE: Officially they even use 2 bytes per element. Yikes.
		uint8 bRandArray[10000];
		memset(&bRandArray, 0, sizeof(bRandArray)); // default to 0 in case it's lower than 100% (in which case, first item's rate increases)

													// Copy the counts, as we're going to adjust them locally.
		uint32 sExchangeCount[ITEMS_IN_EXCHANGE_GROUP];
		memcpy(&sExchangeCount, &pExchange->sExchangeItemCount, sizeof(pExchange->sExchangeItemCount));

		// Build array of exchange item slots (0-4)
		int offset = 0;
		for (int n = 0, i = 0; n < ITEMS_IN_EXCHANGE_GROUP; n++)
		{
			if (sExchangeCount[n] > 0)
			{
				memset(&bRandArray[offset], n, sExchangeCount[n]);
				offset += sExchangeCount[n];
			}
		}

		// Pull our exchange item slot out of our hat (the array we generated).
		uint8 bRandSlot = bRandArray[myrand(0, 9999)];
		uint32 nItemID = pExchange->nExchangeItemNum[bRandSlot];

		// Finally, give our item.
		GiveItem(nItemID, 1);
	}

	return true;
}
#pragma endregion;

#pragma region CUser::RunCountCheckExchange(int nExchangeID)
bool CUser::RunCountCheckExchange(int nExchangeID)
{
	Packet result;
	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		if (pExchange->nOriginItemNum[i] == uint32(ITEM_GOLD)
			|| pExchange->nOriginItemNum[i] == uint32(ITEM_LADDERPOINT)
			|| pExchange->nOriginItemNum[i] == uint32(ITEM_COUNT)
			|| pExchange->nOriginItemNum[i] == uint32(ITEM_HUNT)
			|| pExchange->nOriginItemNum[i] == uint32(ITEM_SKILL))
			return false;
	}

	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0
			&& ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
			break;
	}

	uint32 temp_sOriginItemCount[ITEMS_IN_ORIGIN_GROUP];
	int holder = 0;
	for (uint32 &x : temp_sOriginItemCount)
	{
		x = pExchange->nOriginItemNum[holder] == 0 ? 0 : GetItemCount(pExchange->nOriginItemNum[holder]);
		holder++;
	}

	uint32 min_count = temp_sOriginItemCount[0];
	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		if (temp_sOriginItemCount[i] < min_count && temp_sOriginItemCount[i] != 0)
			min_count = temp_sOriginItemCount[i];
	}

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
		temp_sOriginItemCount[i] = pExchange->nOriginItemNum[i] == 0 ? 0 : (temp_sOriginItemCount[i] == 0 ? pExchange->sOriginItemCount[i] : min_count);

	int nTotalGold = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		if (pExchange->nExchangeItemNum[i] == ITEM_GOLD)
			nTotalGold += (temp_sOriginItemCount[i] * pExchange->sExchangeItemCount[i]);
	}

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

		if (nItemID == ITEM_GOLD
			|| nItemID == ITEM_COUNT
			|| nItemID == ITEM_LADDERPOINT
			|| nItemID == ITEM_SKILL
			|| nItemID == ITEM_HUNT)
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

		_ITEM_DATA *pItem = GetItem(pos);
		if (pItem == nullptr)
		{
			result.Initialize(WIZ_QUEST);
			result << uint8(13) << uint8(3);
			Send(&result);
			return false;
		}

		if (pItem->nNum == 0)
			bReqSlots++;

		nReqWeight += pTable->m_sWeight;
	}

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
	return (bFreeSlots >= bReqSlots);
}
#pragma endregion

#pragma region CUser::RunCountExchange(int nExchangeID)
bool CUser::RunCountExchange(int nExchangeID)
{
	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr
		|| !RunCountCheckExchange(nExchangeID))
		return false;

	uint32 temp_sOriginItemCount[ITEMS_IN_ORIGIN_GROUP];
	int holder = 0;
	for (uint32 &x : temp_sOriginItemCount)
	{
		x = pExchange->nOriginItemNum[holder] == 0 ? 0 : GetItemCount(pExchange->nOriginItemNum[holder]);
		holder++;
	}

	if (!CheckExistItemAnd( // Do we have all of the required items?
		pExchange->nOriginItemNum[0], temp_sOriginItemCount[0],
		pExchange->nOriginItemNum[1], temp_sOriginItemCount[1],
		pExchange->nOriginItemNum[2], temp_sOriginItemCount[2],
		pExchange->nOriginItemNum[3], temp_sOriginItemCount[3],
		pExchange->nOriginItemNum[4], temp_sOriginItemCount[4]))
		return false;

	uint32 min_count = temp_sOriginItemCount[0];
	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		if (temp_sOriginItemCount[i] < min_count && temp_sOriginItemCount[i] != 0)
			min_count = temp_sOriginItemCount[i];
	}

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
		temp_sOriginItemCount[i] = pExchange->nOriginItemNum[i] == 0 ? 0 : (temp_sOriginItemCount[i] == 0 ? pExchange->sOriginItemCount[i] : min_count);

	for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
	{
		// These checks are a little pointless, but remove the required items as well.
		if (!RobItem(pExchange->nOriginItemNum[i], temp_sOriginItemCount[i]))
			return false;
	}

	std::map<uint32, uint32> sExchangeMap;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
	{
		if (pExchange->sExchangeItemCount[i] == 0)
			continue;

		uint32 nItemID = pExchange->nExchangeItemNum[i];
		uint32 nCount = temp_sOriginItemCount[i];

		if (nItemID == ITEM_GOLD
			|| nItemID == ITEM_COUNT
			|| nItemID == ITEM_EXP
			|| nItemID == ITEM_LADDERPOINT)
			nCount *= pExchange->sExchangeItemCount[i];

		auto itr = sExchangeMap.find(nItemID);
		if (itr == sExchangeMap.end())
			sExchangeMap.insert(std::make_pair(nItemID, nCount));
	}

	int counter = 0;
	uint32 sExchangeItemNum[ITEMS_IN_EXCHANGE_GROUP];
	uint32 sExchangeItemCount[ITEMS_IN_EXCHANGE_GROUP];
	memset(&sExchangeItemNum, 0, sizeof(sExchangeItemNum));
	memset(&sExchangeItemCount, 0, sizeof(sExchangeItemCount));

	foreach(itr, sExchangeMap)
	{
		uint32 nItemID = itr->first;
		uint32 nCount = itr->second;

		if (counter < ITEMS_IN_EXCHANGE_GROUP)
		{
			sExchangeItemNum[counter] = nItemID;
			sExchangeItemCount[counter] = nCount;
			counter++;
		}

		if (nItemID == uint32(ITEM_GOLD))
		{
			if (nCount > 0)
			{
				GoldGain(nCount, true, false);
			}
			continue;
		}
		else if (nItemID == uint32(ITEM_COUNT))
		{
			if (nCount > 0)
			{
				SendLoyaltyChange(nCount, false, false, false);
			}
			continue;
		}
		else if (nItemID == uint32(ITEM_LADDERPOINT))
		{
			if (nCount > 0)
			{
				SendLoyaltyChange(nCount);
			}
			continue;
		}
		else if (nItemID == uint32(ITEM_EXP))
		{
			if (nCount > 0)
			{
				ExpChange(nCount, true);
			}
			continue;
		}
		else if (nItemID == uint32(ITEM_SKILL)
			|| nItemID == uint32(ITEM_HUNT))
		{
			if (nCount > 0)
				continue;

			continue;
		}
		else
		{
			if (nCount > 0)
				GiveItem(nItemID, nCount);
		}
	}

	QuestV2ShowGiveItem(sExchangeItemNum[0], sExchangeItemCount[0],
		sExchangeItemNum[1], sExchangeItemCount[1],
		sExchangeItemNum[2], sExchangeItemCount[2],
		sExchangeItemNum[3], sExchangeItemCount[3],
		sExchangeItemNum[4], sExchangeItemCount[4]);

	return true;
}
#pragma endregion

#pragma region CUser::GetMaxExchange(int nExchangeID)
uint16 CUser::GetMaxExchange(int nExchangeID)
{
	uint16 sResult = 0;
	_ITEM_TABLE * pTable;
	uint16 temp_sWeight = 0;

	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

	if (pExchange != nullptr)
	{
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		{
			pTable = g_pMain->GetItemPtr(pExchange->nExchangeItemNum[i]);
			if (pTable != nullptr)
				temp_sWeight += pTable->m_sWeight;
		}

		if(temp_sWeight > 0) // 22.05.2015
			sResult = (m_sMaxWeight - m_sItemWeight) / temp_sWeight;
	}

	return sResult;
}
#pragma endregion

#pragma region CUser::IsValidSlotPosKnightRoyal(_ITEM_TABLE* pTable, int destpos)
bool CUser::IsValidSlotPosKnightRoyal(_ITEM_TABLE* pTable, int destpos)
{
	if (pTable == nullptr)
		return false;

	bool bOneHandedItem = false;
	switch (pTable->m_bSlot)
	{
	case ItemSlot1HEitherHand:
		if (destpos != KNIGHT_ROYAL_LEFTHAND && destpos != KNIGHT_ROYAL_RIGHTHAND)
			return false;
		bOneHandedItem = true;
		break;
	case ItemSlot1HRightHand:
		if (destpos != KNIGHT_ROYAL_RIGHTHAND)
			return false;
		bOneHandedItem = true;
		break;
	case ItemSlot2HRightHand:
		if (destpos != KNIGHT_ROYAL_RIGHTHAND || GetKnightRoyaleItem(KNIGHT_ROYAL_LEFTHAND)->nNum != 0)
		{
			_ITEM_DATA *pSrcItem = GetKnightRoyaleItem(KNIGHT_ROYAL_LEFTHAND), *pDstItem = GetKnightRoyaleItem(KNIGHT_ROYAL_RIGHTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;
	case ItemSlot1HLeftHand:
		if (destpos != KNIGHT_ROYAL_LEFTHAND)
			return false;
		bOneHandedItem = true;
		break;
	case ItemSlot2HLeftHand:
		if (destpos != KNIGHT_ROYAL_LEFTHAND || GetKnightRoyaleItem(KNIGHT_ROYAL_RIGHTHAND)->nNum != 0)
		{
			_ITEM_DATA *pSrcItem = GetKnightRoyaleItem(KNIGHT_ROYAL_RIGHTHAND), *pDstItem = GetKnightRoyaleItem(KNIGHT_ROYAL_LEFTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;
	case ItemSlotPauldron:
		if (destpos != KNIGHT_ROYAL_PAULDRON)
			return false;
		break;
	case ItemSlotPads:
		if (destpos != KNIGHT_ROYAL_PANTS)
			return false;
		break;
	case ItemSlotHelmet:
		if (destpos != KNIGHT_ROYAL_HEAD)
			return false;
		break;
	case ItemSlotGloves:
		if (destpos != KNIGHT_ROYAL_GLOVE)
			return false;
		break;
	case ItemSlotBoots:
		if (destpos != KNIGHT_ROYAL_BOOTS)
			return false;
		break;
	default:
		return false;
		break;
	}

	// 1H items can only be equipped when a 2H item isn't equipped.
	if (bOneHandedItem)
	{
		_ITEM_DATA * pItem;
		_ITEM_TABLE * pTable2 = GetItemPrototype(destpos == KNIGHT_ROYAL_LEFTHAND ? KNIGHT_ROYAL_RIGHTHAND : KNIGHT_ROYAL_LEFTHAND, pItem);
		if (pTable2 != nullptr && pTable2->is2Handed())
		{
			_ITEM_DATA *pSrcItem = GetKnightRoyaleItem(KNIGHT_ROYAL_RIGHTHAND), *pDstItem = GetKnightRoyaleItem(KNIGHT_ROYAL_LEFTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
	}

	return true;
}
#pragma endregion

#pragma region CUser::IsValidSlotPos(_ITEM_TABLE* pTable, int destpos)
bool CUser::IsValidSlotPos(_ITEM_TABLE* pTable, int destpos)
{
	if (pTable == nullptr)
		return false;

	bool bOneHandedItem = false;
	switch (pTable->m_bSlot)
	{
	case ItemSlot1HEitherHand:
		if (destpos != RIGHTHAND && destpos != LEFTHAND)
			return false;

		bOneHandedItem = true;
		break;

	case ItemSlot1HRightHand:
		if (destpos != RIGHTHAND)
			return false;

		bOneHandedItem = true;
		break;

		// If we're equipping a 2H item in our right hand, there must
		// be no item in our left hand.
	case ItemSlot2HRightHand:
		if (destpos != RIGHTHAND || GetItem(LEFTHAND)->nNum != 0)
		{
			_ITEM_DATA *pSrcItem = GetItem(LEFTHAND), *pDstItem = GetItem(RIGHTHAND), tmpItem ;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;

	case ItemSlot1HLeftHand:
		if (destpos != LEFTHAND)
			return false;

		bOneHandedItem = true;
		break;

		// If we're equipping a 2H item in our left hand, there must
		// be no item in our right hand.
	case ItemSlot2HLeftHand:
		if (destpos != LEFTHAND || GetItem(RIGHTHAND)->nNum != 0)	
		{
			_ITEM_DATA *pSrcItem = GetItem(RIGHTHAND), *pDstItem = GetItem(LEFTHAND), tmpItem ;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;

	case ItemSlotPauldron:
		if (destpos != BREAST)
			return false;
		break;

	case ItemSlotPads:
		if (destpos != LEG)
			return false;
		break;

	case ItemSlotHelmet:
		if (destpos != HEAD)
			return false;
		break;

	case ItemSlotGloves:
		if (destpos != GLOVE)
			return false;
		break;

	case ItemSlotBoots:
		if (destpos != FOOT)
			return false;
		break;

	case ItemSlotEarring:
		if (destpos != RIGHTEAR && destpos != LEFTEAR)
			return false;
		break;

	case ItemSlotNecklace:
		if (destpos != NECK)
			return false;
		break;

	case ItemSlotRing:
		if (destpos != RIGHTRING && destpos != LEFTRING)
			return false;
		break;

	case ItemSlotShoulder:
	case ItemSlotKaul:
		if (destpos != SHOULDER)
			return false;
		break;

	case ItemSlotBelt:
		if (destpos != WAIST)
			return false;
		break;

	case ItemSlotCospreGloves:
		if (destpos != COSP_GLOVE && destpos != COSP_GLOVE2)
			return false;
		break;

	case ItemSlotCosprePauldron:
		if (destpos != COSP_BREAST)
			return false;
		break;

	case ItemSlotCospreHelmet:
		if (destpos != COSP_HELMET)
			return false;
		break;

	case ItemSlotCospreWings:
		if (destpos != COSP_WINGS)
			return false;
		break;

	case ItemSlotBag:
		if (destpos != COSP_BAG1 && destpos != COSP_BAG2)
			return false;
		break;

	case ItemSlotCospreFairy:
		if (destpos != COSP_FAIRY)
			return false;
		break;

	case ItemSlotCospreTattoo:
		if (destpos != COSP_TATTO)
			return false;
		break;
	default:
		return false;
	}

	// 1H items can only be equipped when a 2H item isn't equipped.
	if (bOneHandedItem)
	{
		_ITEM_DATA * pItem;
		_ITEM_TABLE * pTable2 = GetItemPrototype(destpos == LEFTHAND ? RIGHTHAND : LEFTHAND, pItem);
		if (pTable2 != nullptr && pTable2->is2Handed())
		{
			_ITEM_DATA *pSrcItem = GetItem(RIGHTHAND), *pDstItem = GetItem(LEFTHAND), tmpItem ;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
	}

	return true;
}
#pragma endregion

#pragma region CUser::SendStackChange(uint32 nItemID, uint32 nCount /* needs to be 4 bytes, not a bug */, uint16 sDurability, uint8 bPos, bool bNewItem /* = false */, uint32 Time, uint8 bSlotSection /* = 1 */)
void CUser::SendStackChange(uint32 nItemID, uint32 nCount /* needs to be 4 bytes, not a bug */, uint16 sDurability, uint8 bPos, bool bNewItem /* = false */, uint32 Time, uint8 bSlotSection /* = 1 */)
{
	Packet result(WIZ_ITEM_COUNT_CHANGE);
	result << uint16(1);
	result << uint8(bSlotSection);
	result << uint8(bPos);
	result << uint32(nItemID) << uint32(nCount);
	result << uint8(bNewItem ? 100 : 0);
	result << sDurability;
	result << uint32(0);

	if(Time > UNIXTIME)
	{
		result << uint32(Time);
		result << uint16(0);
	}
	else
	{
		result << uint32(0) << uint16(0);
	}
	
	SetUserAbility(false);
	SendItemWeight();
	Send(&result);
}
#pragma endregion

#pragma region CUser::ItemRemove(Packet & pkt)
void CUser::ItemRemove(Packet & pkt)
{
	Packet result(WIZ_ITEM_REMOVE);
	_ITEM_DATA * pItem;
	uint8 bType, bPos;
	uint32 nItemID;

	pkt >> bType >> bPos >> nItemID;

	// Inventory
	if (bType == 0)
	{
		if (bPos >= HAVE_MAX)
			goto fail_return;

		bPos += SLOT_MAX;
	}
	// Equipped items
	else if (bType == 1)
	{
		if (bPos >= SLOT_MAX)
			goto fail_return;
	}
	else if (bType == 2)
	{
		if (bPos >= HAVE_MAX)
			goto fail_return;

		bPos += SLOT_MAX;
	}

	pItem = GetItem(bPos);

	// Make sure the item matches what the client says it is
	if (pItem == nullptr 
		|| pItem->nNum != nItemID
		|| pItem->isSealed()
		|| pItem->isRented()
		|| isMining()
		|| isFishing()
		|| GetZoneID() == ZONE_CHAOS_DUNGEON) 
		goto fail_return;

	uint32 size = m_sDeletedItemMap.GetSize();

	if(pItem->nExpirationTime == 0 && size <= 19)
	{
		_DELETED_ITEM *pItemDeleted  = nullptr;
		foreach_stlmap(itr, m_sDeletedItemMap)
		{
			if(itr->second == nullptr)
				continue;

			if (itr->second->nNum != nItemID)
				continue;

			if(UNIXTIME - itr->second->iDeleteTime > 60 * 60 * 24 * 3)
				continue;

			pItemDeleted = itr->second;
			break;
		}

		if(pItemDeleted == nullptr)
		{
			_DELETED_ITEM *pItemDeleted = new _DELETED_ITEM;
			pItemDeleted->nNum = pItem->nNum;
			pItemDeleted->sCount = pItem->sCount;
			pItemDeleted->iDeleteTime = uint32(UNIXTIME);

			if (!m_sDeletedItemMap.PutData(size, pItemDeleted))
				delete pItemDeleted;
		}
	}

	memset(pItem, 0, sizeof(_ITEM_DATA));

	SetUserAbility();
	SendItemWeight();

	result << uint8(1);
	Send(&result);

	return;
fail_return:
	result << uint8(0);
	Send(&result);
}
#pragma endregion

#pragma region CUser::CheckGiveSlot(uint8 sSlot)
bool CUser::CheckGiveSlot(uint8 sSlot)
{
	// Find free slots in the inventory, so that we can check against this later.

	if (isDead() 
		|| isTrading() 
		|| isStoreOpen() 
		|| isMerchanting() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| isFishing())
		return false;

	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0 && ++bFreeSlots >= sSlot)
			break;
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	for (int i = 0; i < sSlot; i++)
		bReqSlots++; // new item? new required slot.

	if (bFreeSlots < bReqSlots)
		return false;

	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}
#pragma endregion

#pragma region CUser::LuaCheckGiveSlot(uint8 sSlot)
bool CUser::LuaCheckGiveSlot(uint8 sSlot)
{
	Packet result;

	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0 && ++bFreeSlots >= sSlot)
			break;
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	for (int i = 0; i < sSlot; i++)
		bReqSlots++; // new item? new required slot.

	if (bFreeSlots < bReqSlots)
	{
		result.Initialize(WIZ_QUEST);
		result << uint8(13) << uint8(3);
		Send(&result);
		return false;
	}

	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}
#pragma endregion