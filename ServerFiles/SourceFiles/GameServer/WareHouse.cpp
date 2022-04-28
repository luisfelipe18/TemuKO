#include "stdafx.h"
#include "DBAgent.h"

#pragma region CUser::WarehouseSystem(Packet & pkt)
/**
* @brief	Handles the Storage, i.e. the Inn Hostess, feature packets.
*
* @param	pkt	The packet.
*/
void CUser::WarehouseSystem(Packet & pkt)
{
	if (isDead()
		|| !isInGame())
		return;

	if (isTrading()
		|| isStoreOpen()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		return;

	CNpc* pNpc = nullptr;
	pNpc = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());

	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	switch (pNpc->GetType())
	{
	case NPC_WAREHOUSE:
		WarehouseProcess(pkt);
		break;
	case NPC_CLAN_BANK:
		ClanWarehouseProcess(pkt);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::WarehouseProcess(Packet & pkt)
/**
* @brief	Handles the Storage, i.e. the Inn Hostess, feature packets.
*
* @param	pkt	The packet.
*/
void CUser::WarehouseProcess(Packet& pkt)
{
	enum ResultOpCodes
	{
		NotAccess = 0,
		OpenWarehouse = 1,
		RequiredMoney = 2,
		InvalidPassword = 3
	};

	Packet result(WIZ_WAREHOUSE);
	uint32 nItemID, nCount;
	uint16 sNpcId, reference_pos;
	uint8 page, bSrcPos, bDstPos;
	CNpc* pNpc = nullptr;
	_ITEM_TABLE* pTable = nullptr;
	_ITEM_DATA* pSrcItem = nullptr, * pDstItem = nullptr;
	uint8 opcode;
	ResultOpCodes resultOpCode = NotAccess;

	if (isDead()
		|| !isInGame())
		return;

	if (isTrading()
		|| isStoreOpen()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		goto fail_return;

	pkt >> opcode;
	if (opcode == WAREHOUSE_OPEN)
	{
		if (isInPKZone())
		{
			if (hasCoins(10000))
				GoldLose(10000);
			else
			{
				resultOpCode = RequiredMoney;
				goto fail_return;
			}
		}
		result << opcode << uint8(1) << GetInnCoins();
		for (int i = 0; i < WAREHOUSE_MAX; i++)
		{
			_ITEM_DATA* pItem = &m_sWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;

			_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
			if (pItemTable != nullptr)
			{
				if (pItemTable->isPetItem())
					ShowPetItemInfo(result, pItem->nSerialNum);
				else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, pItem->nSerialNum);
				else
					result << uint32(0); // Item Unique ID
			}
			else
				result << uint32(0); // Item Unique ID*/

			/*if (pItem->nNum == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID*/

			result << pItem->nExpirationTime;
		}
		Send(&result);
		return;
	}
	pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;

	pNpc = g_pMain->GetNpcPtr(sNpcId, GetZoneID());
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_WAREHOUSE
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		goto fail_return;

	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		goto fail_return;

	reference_pos = 24 * page;

	switch (opcode)
	{
		// Inventory -> inn
	case WAREHOUSE_INPUT:
		pkt >> nCount;

		// Handle coin input.
		if (nItemID == ITEM_GOLD)
		{
			if (!hasCoins(nCount)
				|| GetInnCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iBank += nCount;
			m_iGold -= nCount;
			break;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = GetItem(SLOT_MAX + bSrcPos))->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate())
			goto fail_return;

		pDstItem = &m_sWarehouseArray[reference_pos + bDstPos];
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.													 
			|| pSrcItem->sCount < nCount) // Ensure users have enough of the specified item to move.
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inventory
	case WAREHOUSE_OUTPUT:
		pkt >> nCount;

		if (nItemID == ITEM_GOLD)
		{
			if (!hasInnCoins(nCount)
				|| GetCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iGold += nCount;
			m_iBank -= nCount;
			break;
		}

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| bDstPos > HAVE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = &m_sWarehouseArray[reference_pos + bSrcPos])->nNum != nItemID
			// Does the player have enough room in their inventory?
			|| !CheckWeight(pTable, nItemID, (uint16)nCount))
			goto fail_return;

		pDstItem = GetItem(SLOT_MAX + bDstPos);
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
													 // Ensure users have enough of the specified item to move.
			|| pSrcItem->sCount < nCount)
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0
			|| pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inn
	case WAREHOUSE_MOVE:
		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX)
			goto fail_return;

		pSrcItem = &m_sWarehouseArray[reference_pos + bSrcPos];
		pDstItem = &m_sWarehouseArray[reference_pos + bDstPos];

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inn (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;

		// Inventory -> inventory (using the inn dialog)
	case WAREHOUSE_INVENMOVE:
		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > HAVE_MAX
			|| bDstPos > HAVE_MAX)
			goto fail_return;

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		pDstItem = GetItem(SLOT_MAX + bDstPos);

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inventory (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;
	}

	resultOpCode = OpenWarehouse;

fail_return:
	result << opcode << resultOpCode;
	Send(&result);
}
#pragma endregion

#pragma region CUser::ClanWarehouseProcess(Packet & pkt)
/**
* @brief	Handles the Storage, i.e. the Inn Hostess, feature packets.
*
* @param	pkt	The packet.
*/
void CUser::ClanWarehouseProcess(Packet& pkt)
{
	enum ResultOpCodes
	{
		NotAccess = 0,
		OpenWarehouse = 1,
		RequiredMoney = 2,
		InvalidPassword = 3
	};

	Packet result(WIZ_WAREHOUSE);
	uint32 nItemID, nCount;
	uint16 sNpcId, reference_pos;
	uint8 page, bSrcPos, bDstPos;
	CNpc* pNpc = nullptr;
	_ITEM_TABLE* pTable = nullptr;
	_ITEM_DATA* pSrcItem = nullptr, * pDstItem = nullptr;
	uint8 opcode;
	ResultOpCodes resultOpCode = NotAccess;

	if (!isInClan())
		goto fail_return;

	if (isInClan() 
		&& GetClanPremium() != 13)
		goto fail_return;

	CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());

	if (pKnights == nullptr)
		goto fail_return;

	if (isDead()
		|| !isInGame())
		return;

	if (isTrading()
		|| isStoreOpen()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		goto fail_return;

	pkt >> opcode;
	if (opcode == WAREHOUSE_OPEN)
	{
		if (isInPKZone())
		{
			if (hasCoins(10000))
				GoldLose(10000);
			else
			{
				resultOpCode = RequiredMoney;
				goto fail_return;
			}
		}
		result << opcode << uint8(1) << pKnights->GetInnCoins();
		for (int i = 0; i < WAREHOUSE_MAX; i++)
		{
			_ITEM_DATA* pItem = &pKnights->m_sWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;

			_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
			if (pItemTable != nullptr)
			{
				if (pItemTable->isPetItem())
					ShowPetItemInfo(result, pItem->nSerialNum);
				else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, pItem->nSerialNum);
				else
					result << uint32(0); // Item Unique ID
			}
			else
				result << uint32(0); // Item Unique ID*/

			result << pItem->nExpirationTime;
		}
		Send(&result);
		return;
	}
	pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;

	pNpc = g_pMain->GetNpcPtr(sNpcId, GetZoneID());
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_CLAN_BANK
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		goto fail_return;

	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		goto fail_return;

	reference_pos = 24 * page;

	switch (opcode)
	{
		// Inventory -> inn
	case WAREHOUSE_INPUT:
		pkt >> nCount;

		// Handle coin input.
		if (nItemID == ITEM_GOLD)
		{
			if (!hasCoins(nCount)
				|| pKnights->GetInnCoins() + nCount > COIN_MAX)
				goto fail_return;

			pKnights->m_iBank += nCount;
			m_iGold -= nCount;
			break;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = GetItem(SLOT_MAX + bSrcPos))->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate())
			goto fail_return;

		pDstItem = &pKnights->m_sWarehouseArray[reference_pos + bDstPos];
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.													 
			|| pSrcItem->sCount < nCount) // Ensure users have enough of the specified item to move.
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inventory
	case WAREHOUSE_OUTPUT:
		pkt >> nCount;

		if (!isClanLeader()
			&& !isClanAssistant())
			goto fail_return;

		if (nItemID == ITEM_GOLD)
		{
			if (!pKnights->hasInnCoins(nCount)
				|| GetCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iGold += nCount;
			pKnights->m_iBank -= nCount;
			break;
		}

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| bDstPos > HAVE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = &pKnights->m_sWarehouseArray[reference_pos + bSrcPos])->nNum != nItemID
			// Does the player have enough room in their inventory?
			|| !CheckWeight(pTable, nItemID, (uint16)nCount))
			goto fail_return;

		pDstItem = GetItem(SLOT_MAX + bDstPos);
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
													 // Ensure users have enough of the specified item to move.
			|| pSrcItem->sCount < nCount)
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0
			|| pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inn
	case WAREHOUSE_MOVE:
		if (isClanLeader()
			&& isClanAssistant())
			goto fail_return;

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX)
			goto fail_return;

		pSrcItem = &pKnights->m_sWarehouseArray[reference_pos + bSrcPos];
		pDstItem = &pKnights->m_sWarehouseArray[reference_pos + bDstPos];

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inn (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;

		// Inventory -> inventory (using the inn dialog)
	case WAREHOUSE_INVENMOVE:
		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > HAVE_MAX
			|| bDstPos > HAVE_MAX)
			goto fail_return;

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		pDstItem = GetItem(SLOT_MAX + bDstPos);

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inventory (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;
	}
	g_DBAgent.UpdateClanWarehouseData(GetClanID(), UPDATE_PACKET_SAVE);
	g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);
	resultOpCode = OpenWarehouse;
fail_return:
	result << opcode << resultOpCode;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HShieldClanWarehouseProcess(Packet & pkt)
/**
* @brief	Handles the Storage, i.e. the Inn Hostess, feature packets.
*
* @param	pkt	The packet.
*/
void CUser::HShieldClanWarehouseProcess(Packet& pkt)
{
	enum ResultOpCodes
	{
		NotAccess = 0,
		OpenWarehouse = 1,
		RequiredMoney = 2,
		InvalidPassword = 3
	};

	Packet result(WIZ_CLANWAREHOUSE);
	uint32 nItemID, nCount;
	uint16 sNpcId, reference_pos;
	uint8 page, bSrcPos, bDstPos;
	CNpc* pNpc = nullptr;
	_ITEM_TABLE* pTable = nullptr;
	_ITEM_DATA* pSrcItem = nullptr, * pDstItem = nullptr;
	uint8 opcode;
	ResultOpCodes resultOpCode = NotAccess;

	if (!isInClan())
		goto fail_return;

	if (isInClan()
		&& GetClanPremium() != 13)
		goto fail_return;

	CKnights* pKnights = g_pMain->GetClanPtr(GetClanID());

	if (pKnights == nullptr)
		goto fail_return;

	if (isDead()
		|| !isInGame())
		return;

	if (isTrading()
		|| isStoreOpen()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		goto fail_return;

	pkt >> opcode;
	if (opcode == WAREHOUSE_OPEN)
	{
		if (isInPKZone())
		{
			if (hasCoins(10000))
				GoldLose(10000);
			else
			{
				resultOpCode = RequiredMoney;
				goto fail_return;
			}
		}
		result << opcode << uint8(1) << pKnights->GetInnCoins();
		for (int i = 0; i < WAREHOUSE_MAX; i++)
		{
			_ITEM_DATA* pItem = &pKnights->m_sWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;

			_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
			if (pItemTable != nullptr)
			{
				if (pItemTable->isPetItem())
					ShowPetItemInfo(result, pItem->nSerialNum);
				else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, pItem->nSerialNum);
				else
					result << uint32(0); // Item Unique ID
			}
			else
				result << uint32(0); // Item Unique ID*/

			result << pItem->nExpirationTime;
		}
		Send(&result);
		return;
	}
	pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;

	/*pNpc = g_pMain->GetNpcPtr(sNpcId, GetZoneID());
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_CLAN_BANK
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		goto fail_return;*/

	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		goto fail_return;

	reference_pos = 24 * page;

	switch (opcode)
	{
		// Inventory -> inn
	case WAREHOUSE_INPUT:
		pkt >> nCount;

		// Handle coin input.
		if (nItemID == ITEM_GOLD)
		{
			if (!hasCoins(nCount)
				|| pKnights->GetInnCoins() + nCount > COIN_MAX)
				goto fail_return;

			pKnights->m_iBank += nCount;
			m_iGold -= nCount;
			break;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = GetItem(SLOT_MAX + bSrcPos))->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate())
			goto fail_return;

		pDstItem = &pKnights->m_sWarehouseArray[reference_pos + bDstPos];
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.													 
			|| pSrcItem->sCount < nCount) // Ensure users have enough of the specified item to move.
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inventory
	case WAREHOUSE_OUTPUT:
		pkt >> nCount;

		if (!isClanLeader()
			&& !isClanAssistant())
			goto fail_return;

		if (nItemID == ITEM_GOLD)
		{
			if (!pKnights->hasInnCoins(nCount)
				|| GetCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iGold += nCount;
			pKnights->m_iBank -= nCount;
			break;
		}

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| bDstPos > HAVE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = &pKnights->m_sWarehouseArray[reference_pos + bSrcPos])->nNum != nItemID
			// Does the player have enough room in their inventory?
			|| !CheckWeight(pTable, nItemID, (uint16)nCount))
			goto fail_return;

		pDstItem = GetItem(SLOT_MAX + bDstPos);
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
													 // Ensure users have enough of the specified item to move.
			|| pSrcItem->sCount < nCount)
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		pDstItem->sCount += (uint16)nCount;
		pSrcItem->sCount -= (uint16)nCount;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && (pDstItem->nSerialNum == 0))
		{
			if (pDstItem->nSerialNum == 0)
				pDstItem->nSerialNum = g_pMain->GenerateItemSerial();
		}

		if (pSrcItem->sCount <= 0
			|| pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inn
	case WAREHOUSE_MOVE:
		if (isClanLeader()
			&& isClanAssistant())
			goto fail_return;

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX)
			goto fail_return;

		pSrcItem = &pKnights->m_sWarehouseArray[reference_pos + bSrcPos];
		pDstItem = &pKnights->m_sWarehouseArray[reference_pos + bDstPos];

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inn (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;

		// Inventory -> inventory (using the inn dialog)
	case WAREHOUSE_INVENMOVE:
		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > HAVE_MAX
			|| bDstPos > HAVE_MAX)
			goto fail_return;

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		pDstItem = GetItem(SLOT_MAX + bDstPos);

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inventory (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;
	}
	g_DBAgent.UpdateClanWarehouseData(GetClanID(), UPDATE_PACKET_SAVE);
	g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);
	resultOpCode = OpenWarehouse;
fail_return:
	result << opcode << resultOpCode;
	Send(&result);
}
#pragma endregion