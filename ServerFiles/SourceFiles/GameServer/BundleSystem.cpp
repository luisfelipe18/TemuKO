#include "stdafx.h"

using namespace std;

#pragma region CUser::BundleOpenReq(Packet & pkt)
/**
* @brief	Handler for opening a loot box.
*
* @param	pkt	The packet.
*/
void CUser::BundleOpenReq(Packet & pkt)
{
	Packet result(WIZ_BUNDLE_OPEN_REQ);
	uint32 bundle_index = pkt.read<uint32>();
	C3DMap* pMap = GetMap();
	CRegion * pRegion = GetRegion();

	float LootRange = m_PettingOn != nullptr  && m_PettingOn->sStateChange == MODE_LOOTING ? MAX_LOOT_RANGE * 300 : MAX_LOOT_RANGE;
	if (pMap == nullptr
		|| bundle_index < 1
		|| pRegion == nullptr
		|| isDead()) // yeah, we know people abuse this. We do not care!
	{
		if (bundle_index < 1)
		{
			result << bundle_index << uint8(0);
			Send(&result);
		}
		return;
	}

	_LOOT_BUNDLE *pBundle = pRegion->m_RegionItemArray.GetData(bundle_index);
	if (pBundle == nullptr
		|| !isInRange(pBundle->x, pBundle->z, LootRange))
	{
		if (pBundle == nullptr)
		{
			result << bundle_index << uint8(0);
			Send(&result);
		}
		return;
	}

	result << bundle_index << uint8(1);
	// Send all items in the bundle to the player
	foreach(itr, pBundle->Items)
		result << itr->nItemID << itr->sCount;

	// The client expects all n items, so if there's any excess...
	// send placeholder data for them.
	for (uint32 i = (uint32)pBundle->Items.size(); i < LOOT_ITEMS; i++)
		result << uint32(0) << uint16(0);
	Send(&result);
}
#pragma endregion

#pragma region CUser::ItemGet(Packet & pkt)
/**
* @brief	Handler for looting an item from a loot box.
*
* @param	pkt	The packet.
*/
void CUser::ItemGet(Packet & pkt)
{
	enum LootErrorCodes
	{
		LootError = 0,
		LootSolo = 1,
		LootPartyCoinDistribution = 2,
		LootPartyNotification = 3,
		LootPartyItemGivenAway = 4,
		LootPartyItemGivenToUs = 5,
		LootNoRoom = 6
	};

	Packet result(WIZ_ITEM_GET);
	uint32 nBundleID = pkt.read<uint32>(), nItemID = pkt.read<uint32>();
	_LOOT_BUNDLE * pBundle = nullptr;
	_LOOT_ITEM * pItem = nullptr;
	CRegion* pRegion = GetRegion();
	CUser * pReceiver = nullptr;
	int JackpotNoah = rand() % 100 + 1;
	int JackpotNoahParty = rand() % 100 + 1;
	// Lock the array while we process this request
	// to prevent any race conditions between getting/removing the items...
	Guard lock(pRegion->m_RegionItemArray.m_lock);

	float LootRange = m_PettingOn != nullptr  && m_PettingOn->sStateChange == MODE_LOOTING ? MAX_LOOT_RANGE * 300 : MAX_LOOT_RANGE;
	float GiveLooting = m_PettingOn != nullptr  && m_PettingOn->sStateChange == MODE_LOOTING ? RANGE_50M + RANGE_50M : RANGE_50M;
	// Are we in any region?
	if (pRegion == nullptr
		|| isTrading()
		|| isDead()
		// Does the bundle exist in this region's list?
		|| (pBundle = pRegion->m_RegionItemArray.GetData(nBundleID)) == nullptr
		// Are we close enough to the bundle?
		|| !isInRange(pBundle->x, pBundle->z, LootRange))
		goto fail_return;

	// Does the specified item we're looting exist in the bundle?
	foreach(itr, pBundle->Items)
	{
		if (itr->nItemID == nItemID)
		{
			pItem = &(*itr);
			break;
		}
	}

	// Attempt to loot the specified item.
	// If looting is possible, we can then give the receiver the item.
	if (pItem == nullptr
		|| pItem->sCount == 0
		|| (pReceiver = GetLootUser(pBundle, pItem)) == nullptr)
		goto fail_return;

	// If we're dealing with coins, either:
	//  - we're not in a party, in which case the coins go to us. 
	//  - we're in a party, in which case we need to distribute the coins (proportionately, by their level). 
	// Error handling should already have occurred in GetLootUser().
	if (nItemID == ITEM_GOLD)
	{
		_PARTY_GROUP * pParty;
		uint32 pGold = 0;
		// Not in a party, so all the coins go to us.
		if (!isInParty()
			|| (pParty = g_pMain->GetPartyPtr(GetPartyID())) == nullptr)
		{
			// NOTE: Coins have been checked already.
			if (GetPremiumProperty(PremiumNoahPercent) > 0)
				pGold = pItem->sCount * (100 + GetPremiumProperty(PremiumNoahPercent)) / 100;
			else
				pGold = pItem->sCount;

			if (GetClanPremiumProperty(PremiumNoahPercent) > 0)
				pGold = pGold * (100 + GetClanPremiumProperty(PremiumNoahPercent)) / 100;

			if (JackpotNoah > 85)
			{
				if (g_pMain->m_sJackGoldPots)
					JackPotNoah(pGold);
				else
					GoldGain(pGold, false, true);
			}
			else
				GoldGain(pGold, false, true);

			result << uint8(LootSolo) << nBundleID << int8(-1) << nItemID << pItem->sCount << GetCoins();
			pReceiver->Send(&result);
		}
		// In a party, so distribute the coins relative to their level.
		else
		{
			uint16 sumOfLevels = 0;
			vector<CUser *> partyUsers;
			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				CUser * pUser = g_pMain->GetUserPtr(pParty->uid[i]);
				if (pUser == nullptr)
					continue;

				sumOfLevels += pUser->GetLevel();
				partyUsers.push_back(pUser);
			}

			if (partyUsers.empty())
				goto fail_return;

			foreach(itr, partyUsers)
			{
				if ((*itr)->isDead() ||
					!(*itr)->isInRange(pBundle->x, pBundle->z, GiveLooting))
					continue;

				// Calculate the number of coins to give the player
				// Give each party member coins relative to their level.
				int coins = (int)(pItem->sCount * (float)((*itr)->GetLevel() / (float)sumOfLevels));

				if ((*itr)->GetPremiumProperty(PremiumNoahPercent) > 0)
					pGold = coins * (100 + (*itr)->GetPremiumProperty(PremiumNoahPercent)) / 100;
				else
					pGold = coins;

				if ((*itr)->GetClanPremiumProperty(PremiumNoahPercent) > 0)
					pGold = pGold * (100 + (*itr)->GetClanPremiumProperty(PremiumNoahPercent)) / 100;

				if (JackpotNoahParty > 85)
				{
					if (g_pMain->m_sJackGoldPots)
						(*itr)->JackPotNoah(pGold);
					else
						(*itr)->GoldGain(pGold, false, true);
				}
				else
					(*itr)->GoldGain(pGold, false, true);

				// Let each player know they received coins.
				result.clear();
				result << uint8(LootPartyCoinDistribution) << nBundleID << uint8(-1) << nItemID << (*itr)->GetCoins();
				(*itr)->Send(&result);
			}
		}
	} // end of coin distribution
	  // If the item selected is actually an item...
	else
	{
		if (pReceiver->isDead() ||
			!pReceiver->isInRange(pBundle->x, pBundle->z, GiveLooting))
			(pReceiver = GetLootUser(pBundle, pItem));

		// Retrieve the position for this item.
		int8 bDstPos = pReceiver->FindSlotForItem(pItem->nItemID, pItem->sCount);

		// This should NOT happen unless their inventory changed after the check.
		// The item won't be removed until after processing's complete, so it's OK to error out here.
		if (bDstPos < 0)
			goto fail_return;

		// Ensure there's enough room in this user's inventory.
		if (!pReceiver->CheckWeight(pItem->nItemID, pItem->sCount))
		{
			result << uint8(LootNoRoom);
			pReceiver->Send(&result);
			return; // don't need to remove the item, so stop here.
		}

		// Add item to receiver's inventory
		_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID); // note: already looked up in GetLootUser() so it definitely exists
		_ITEM_DATA * pDstItem = &pReceiver->m_sItemArray[bDstPos];

		pDstItem->nNum = pItem->nItemID;
		pDstItem->sCount += pItem->sCount;

		if (pDstItem->sCount == pItem->sCount)
		{
			pDstItem->nSerialNum = g_pMain->GenerateItemSerial();

			// NOTE: Obscure special items that act as if their durations are their stack sizes
			// will be broken here, but usual cases are typically only given in the PUS.
			// Will need to revise this logic (rather, shift it out into its own method).
			pDstItem->sDuration = pTable->m_sDuration;
		}

		if (pDstItem->sCount > MAX_ITEM_COUNT)
			pDstItem->sCount = MAX_ITEM_COUNT;

		result << uint8(pReceiver == this ? LootSolo : LootPartyItemGivenToUs)
			<< nBundleID
			<< uint8(bDstPos - SLOT_MAX)
			<< pDstItem->nNum << pDstItem->sCount
			<< pReceiver->GetCoins();

		pReceiver->Send(&result);

		pReceiver->SetUserAbility(false);
		pReceiver->SendItemWeight();

		bool HasObtained = false;
		if (pTable->ItemClass == 4
			|| pTable->ItemClass == 5
			|| pTable->ItemClass == 8
			|| pTable->ItemClass == 21
			|| pTable->ItemClass == 22
			|| pTable->ItemClass == 33
			|| pTable->ItemClass == 34
			|| pTable->ItemClass == 35
			|| pTable->GetNum() == 379068000)
			HasObtained = true;

		// Now notify the party that we've looted, if applicable.
		if (isInParty())
		{
			result.clear();
			result << uint8(LootPartyNotification) << nBundleID << nItemID << pReceiver->GetName();
			g_pMain->Send_PartyMember(GetPartyID(), &result);

			// If we're not the receiver, i.e. round-robin gave it to someone else
			// we should let us know that this was done (otherwise we'll be like, "GM!!? WHERE'S MY ITEM?!?")
			if (pReceiver != this)
			{
				result.clear();
				result << uint8(LootPartyItemGivenAway);
				Send(&result);
			}
		}
		if (HasObtained == true)
		{
			_SERVER_SETTINGS *pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);

			if (pSettings != nullptr)
			{
				if (pSettings->DropNotice == Aktive)
				{
					result.clear();
					result.Initialize(WIZ_LOGOSSHOUT);
					result.SByte();
					result << uint8(0x02) << uint8(0x04) << pReceiver->GetName() << pTable->m_iNum;
					g_pMain->Send_All(&result);
				}
			}
		}
		DateTime time;
		g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d), UserID=%s, ItemName=%s, ItemID=%d,Count=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pReceiver->GetZoneID(), uint16(pReceiver->GetX()), uint16(pReceiver->GetZ()), pReceiver->GetName().c_str(), pTable->m_sName.c_str(), pTable->m_iNum, pItem->sCount));
	}

	// Everything is OK, we have a target. Now remove the item from the bundle.
	// If there's nothing else in the bundle, remove the bundle from the region.
	if (GetMap())
		GetMap()->RegionItemRemove(pRegion, pBundle, pItem);

	return;

fail_return:
	// Generic error
	result << uint8(LootError);
	Send(&result);
}
#pragma endregion

#pragma region CUser::GetLootUser(_LOOT_BUNDLE * pBundle, _LOOT_ITEM * pItem)
/**
* @brief	Gets the user to give the loot to.
*
* @param	pBundle	The loot bundle.
* @param	pItem  	The item being looted.
*
* @return	null if it fails, else the loot user.
*/
CUser * CUser::GetLootUser(_LOOT_BUNDLE * pBundle, _LOOT_ITEM * pItem)
{
	CUser * pReceiver = nullptr;

	if (pBundle == nullptr
		|| pItem == nullptr)
		return nullptr;

	//	If we're dealing with coins, either:
	//  - we're in a party, in which case we need to distribute the coins (proportionately, by their level). 
	//	  No checks are necessary here (the coins will be miniscule, so if there's no room we can safely ignore them)
	//  - we're not in a party, in which case the coins go to us. 
	//	  In this case, we MUST check to be sure we have room for the coins.
	if (pItem->nItemID == ITEM_GOLD)
	{
		// NOTE: No checks are necessary if we're in a party.
		if (!isInParty())
		{
			// We're not in a party, so we must check to be 
			// sure we have enough room for the coins.
			if ((GetCoins() + pItem->sCount) > COIN_MAX)
				return nullptr;
		}

		// The caller will perform the distribution.
		return this;
	}

	//	If we're dealing with items:
	//	- if we're in a party: 
	//		distribute the item to the next player in the party in round-robin fashion, 
	//		whilst ensuring that user can actually hold the item.
	//  - if not in a party: 
	//		simply ensure that we can hold the item.
	if (isInParty())
	{
		CUser * pMember = GetItemRoutingUser(pItem->nItemID, pItem->sCount);

		if (pMember == nullptr 
			|| !pMember->isInRange(pBundle->x, pBundle->z, RANGE_50M))
			return nullptr;

		return pMember;
	}
	else
	{
		// NOTE: We check to see if they can hold this item in the caller.
		pReceiver = this;
	}

	return pReceiver;
}
#pragma endregion

#pragma region CUser::GetLootUserLooting(_LOOT_BUNDLE * pBundle, _LOOT_ITEM * pItem)
/**
* @brief	Gets the user to give the loot to.
*
* @param	pBundle	The loot bundle.
* @param	pItem  	The item being looted.
*
* @return	null if it fails, else the loot user.
*/
CUser* CUser::GetLootUserLooting(_LOOT_BUNDLE* pBundle, _LOOT_ITEM* pItem)
{
	CUser* pReceiver = nullptr;

	if (pBundle == nullptr
		|| pItem == nullptr)
		return nullptr;

	float PartyLootRange = RANGE_50M + RANGE_50M;

	//	If we're dealing with coins, either:
	//  - we're in a party, in which case we need to distribute the coins (proportionately, by their level). 
	//	  No checks are necessary here (the coins will be miniscule, so if there's no room we can safely ignore them)
	//  - we're not in a party, in which case the coins go to us. 
	//	  In this case, we MUST check to be sure we have room for the coins.
	if (pItem->nItemID == ITEM_GOLD)
	{
		// NOTE: No checks are necessary if we're in a party.
		if (!isInParty())
		{
			// We're not in a party, so we must check to be 
			// sure we have enough room for the coins.
			if ((GetCoins() + pItem->sCount) > COIN_MAX)
				return nullptr;
		}

		// The caller will perform the distribution.
		return this;
	}

	//	If we're dealing with items:
	//	- if we're in a party: 
	//		distribute the item to the next player in the party in round-robin fashion, 
	//		whilst ensuring that user can actually hold the item.
	//  - if not in a party: 
	//		simply ensure that we can hold the item.
	if (isInParty())
	{
		CUser* pMember = GetItemRoutingUserLooting(pItem->nItemID, pItem->sCount);

		if (pMember == nullptr
			|| !pMember->isInRange(pBundle->x, pBundle->z, PartyLootRange))
			return nullptr;

		return pMember;
	}
	else
	{
		// NOTE: We check to see if they can hold this item in the caller.
		pReceiver = this;
	}

	return pReceiver;
}
#pragma endregion

#pragma region CUser::ItemGetLooting(bundle_index, itr.itemid, itr.slot)
void CUser::ItemGetLooting(uint32 nBundleID, uint32 nItemID, uint16 bBoxSlot)
{
	enum LootErrorCodes
	{
		LootError = 0,
		LootSolo = 1,
		LootPartyCoinDistribution = 2,
		LootPartyNotification = 3,
		LootPartyItemGivenAway = 4,
		LootPartyItemGivenToUs = 5,
		LootNoRoom = 6
	};

	Packet result(WIZ_ITEM_GET);
	/*_LOOT_BUNDLE* pBundle = nullptr;
	_LOOT_ITEM* pItem = nullptr;
	CRegion* pRegion = GetRegion();
	CUser* pReceiver = nullptr;*/
	_LOOT_BUNDLE* pBundle = nullptr;
	_LOOT_ITEM* pItem = nullptr;
	CRegion* pRegion = nullptr;
	CUser* pReceiver = nullptr;
	int JackpotNoah = rand() % 100 + 1;
	int JackpotNoahParty = rand() % 100 + 1;
	// Lock the array while we process this request
	// to prevent any race conditions between getting/removing the items...

	int16 rx = GetRegionX(), rz = GetRegionZ();
	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		pRegion = GetMap()->GetRegion(region_x, region_z);

		if (pRegion == nullptr)
			continue;

		Guard lock(pRegion->m_RegionItemArray.m_lock);
		auto ix = pRegion->m_RegionItemArray.GetData(nBundleID);
		if (ix != nullptr)
		{
			pBundle = ix;
			goto out;
		}
	}
out:

	if (pRegion == nullptr)
		return;

	Guard lock(pRegion->m_RegionItemArray.m_lock);

	if(isTrading()
		|| isDead()
		|| GetMap() == nullptr)
		goto fail_return;

	float LootRange = MAX_LOOT_RANGE * 300;
	float PartyLootRange = RANGE_50M + RANGE_50M;

	if (pBundle == nullptr
		|| !isInRange(pBundle->x, pBundle->z, LootRange))
		goto fail_return;

	// Does the specified item we're looting exist in the bundle?
	foreach(itr, pBundle->Items)
	{
		if (itr->nItemID == nItemID)
		{
			pItem = &(*itr);
			break;
		}
	}

	// Attempt to loot the specified item.
	// If looting is possible, we can then give the receiver the item.
	if (pItem == nullptr
		|| pItem->sCount == 0
		|| (pReceiver = GetLootUserLooting(pBundle, pItem)) == nullptr)
		goto fail_return;

	// If we're dealing with coins, either:
	//  - we're not in a party, in which case the coins go to us. 
	//  - we're in a party, in which case we need to distribute the coins (proportionately, by their level). 
	// Error handling should already have occurred in GetLootUserLooting().
	if (nItemID == ITEM_GOLD)
	{
		_PARTY_GROUP* pParty;
		uint32 pGold = 0;
		// Not in a party, so all the coins go to us.
		if (!isInParty()
			|| (pParty = g_pMain->GetPartyPtr(GetPartyID())) == nullptr)
		{
			// NOTE: Coins have been checked already.
			if (GetPremiumProperty(PremiumNoahPercent) > 0)
				pGold = pItem->sCount * (100 + GetPremiumProperty(PremiumNoahPercent)) / 100;
			else
				pGold = pItem->sCount;

			if (GetClanPremiumProperty(PremiumNoahPercent) > 0)
				pGold = pGold * (100 + GetClanPremiumProperty(PremiumNoahPercent)) / 100;

			if (JackpotNoah > 85)
			{
				if (g_pMain->m_sJackGoldPots)
					JackPotNoah(pGold);
				else
					GoldGain(pGold, false, true);
			}
			else
				GoldGain(pGold, false, true);

			result << uint8(LootSolo) << nBundleID << int8(-1) << nItemID << pItem->sCount << GetCoins();
			pReceiver->Send(&result);
		}
		// In a party, so distribute the coins relative to their level.
		else
		{
			uint16 sumOfLevels = 0;
			vector<CUser*> partyUsers;
			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				CUser* pUser = g_pMain->GetUserPtr(pParty->uid[i]);
				if (pUser == nullptr)
					continue;

				sumOfLevels += pUser->GetLevel();
				partyUsers.push_back(pUser);
			}

			if (partyUsers.empty())
				goto fail_return;

			foreach(itr, partyUsers)
			{
				if ((*itr)->isDead() ||
					!(*itr)->isInRange(pBundle->x, pBundle->z, PartyLootRange))
					continue;

				// Calculate the number of coins to give the player
				// Give each party member coins relative to their level.
				int coins = (int)(pItem->sCount * (float)((*itr)->GetLevel() / (float)sumOfLevels));

				if ((*itr)->GetPremiumProperty(PremiumNoahPercent) > 0)
					pGold = coins * (100 + (*itr)->GetPremiumProperty(PremiumNoahPercent)) / 100;
				else
					pGold = coins;

				if ((*itr)->GetClanPremiumProperty(PremiumNoahPercent) > 0)
					pGold = pGold * (100 + (*itr)->GetClanPremiumProperty(PremiumNoahPercent)) / 100;

				if (JackpotNoahParty > 85)
				{
					if (g_pMain->m_sJackGoldPots)
						(*itr)->JackPotNoah(pGold);
					else
						(*itr)->GoldGain(pGold, false, true);
				}
				else
					(*itr)->GoldGain(pGold, false, true);

				// Let each player know they received coins.
				result.clear();
				result << uint8(LootPartyCoinDistribution) << nBundleID << uint8(-1) << nItemID << (*itr)->GetCoins();
				(*itr)->Send(&result);
			}
		}
	} // end of coin distribution
	  // If the item selected is actually an item...
	else
	{
		if (pReceiver->isDead() ||
			!pReceiver->isInRange(pBundle->x, pBundle->z, PartyLootRange))
			(pReceiver = GetLootUserLooting(pBundle, pItem));

		// Retrieve the position for this item.
		int8 bDstPos = pReceiver->FindSlotForItem(pItem->nItemID, pItem->sCount);

		// This should NOT happen unless their inventory changed after the check.
		// The item won't be removed until after processing's complete, so it's OK to error out here.
		if (bDstPos < 0)
			goto fail_return;

		// Ensure there's enough room in this user's inventory.
		if (!pReceiver->CheckWeight(pItem->nItemID, pItem->sCount))
		{
			result << uint8(LootNoRoom);
			pReceiver->Send(&result);
			return; // don't need to remove the item, so stop here.
		}

		// Add item to receiver's inventory
		_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID); // note: already looked up in GetLootUserLooting() so it definitely exists
		_ITEM_DATA* pDstItem = &pReceiver->m_sItemArray[bDstPos];

		pDstItem->nNum = pItem->nItemID;
		pDstItem->sCount += pItem->sCount;

		if (pDstItem->sCount == pItem->sCount)
		{
			pDstItem->nSerialNum = g_pMain->GenerateItemSerial();

			// NOTE: Obscure special items that act as if their durations are their stack sizes
			// will be broken here, but usual cases are typically only given in the PUS.
			// Will need to revise this logic (rather, shift it out into its own method).
			pDstItem->sDuration = pTable->m_sDuration;
		}

		if (pDstItem->sCount > MAX_ITEM_COUNT)
			pDstItem->sCount = MAX_ITEM_COUNT;

		result << uint8(pReceiver == this ? LootSolo : LootPartyItemGivenToUs)
			<< nBundleID
			<< uint8(bDstPos - SLOT_MAX)
			<< pDstItem->nNum << pDstItem->sCount
			<< pReceiver->GetCoins();

		pReceiver->Send(&result);

		pReceiver->SetUserAbility(false);
		pReceiver->SendItemWeight();

#if __VERSION > 1960
		bool HasObtained = false;
		if (pTable->ItemClass == 4
			|| pTable->ItemClass == 5
			|| pTable->ItemClass == 8
			|| pTable->ItemClass == 21
			|| pTable->ItemClass == 22
			|| pTable->ItemClass == 33
			|| pTable->ItemClass == 34
			|| pTable->ItemClass == 35
			|| pTable->GetNum() == 379068000)
			HasObtained = true;
#endif

		// Now notify the party that we've looted, if applicable.
		if (isInParty())
		{
			result.clear();
			result << uint8(LootPartyNotification) << nBundleID << nItemID << pReceiver->GetName();
			g_pMain->Send_PartyMember(GetPartyID(), &result);

			// If we're not the receiver, i.e. round-robin gave it to someone else
			// we should let us know that this was done (otherwise we'll be like, "GM!!? WHERE'S MY ITEM?!?")
			if (pReceiver != this)
			{
				result.clear();
				result << uint8(LootPartyItemGivenAway);
				Send(&result);
			}
		}
#if __VERSION > 1960
		if (HasObtained == true)
		{
			_SERVER_SETTINGS* pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);

			if (pSettings != nullptr)
			{
				if (pSettings->DropNotice == Aktive)
				{
					result.clear();
					result.Initialize(WIZ_LOGOSSHOUT);
					result.SByte();
					result << uint8(0x02) 
						<< uint8(0x04) 
						<< pReceiver->GetName() 
						<< pTable->m_iNum;
					g_pMain->Send_All(&result);
				}
			}
		}
#endif

		DateTime time;
		g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d), UserID=%s, ItemName=%s, ItemID=%d,Count=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pReceiver->GetZoneID(), uint16(pReceiver->GetX()), uint16(pReceiver->GetZ()), pReceiver->GetName().c_str(), pTable->m_sName.c_str(), pTable->m_iNum, pItem->sCount));
	}

	// Everything is OK, we have a target. Now remove the item from the bundle.
	// If there's nothing else in the bundle, remove the bundle from the region.
	if (GetMap())
		GetMap()->RegionItemRemove(pRegion, pBundle, pItem);

	return;

fail_return:
	// Generic error
	result << uint8(LootError);
	Send(&result);
}
#pragma endregion