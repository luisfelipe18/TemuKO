#include "stdafx.h"

/**
* @brief	Checks whether the specified ItemID exists in the map passed by.
*			And returns the number of instance on success.
*
* @return	Returns the number of instance found in the map.
*/
static uint16 IsExistInMap(UpgradeItemsMap &map, int32 ItemID)
{
	uint16 temp = 0;
	foreach(itr, map)
	{
		if (itr->second.nItemID == ItemID)
			temp++;
	}
	return temp;
}

static UpgradeScrollType GetScrollType(uint32 ScrollNum)
{
	if (ScrollNum == 379221000
		|| ScrollNum == 379222000
		|| ScrollNum == 379223000
		|| ScrollNum == 379224000
		|| ScrollNum == 379225000
		|| ScrollNum == 379226000
		|| ScrollNum == 379227000
		|| ScrollNum == 379228000
		|| ScrollNum == 379229000
		|| ScrollNum == 379230000
		|| ScrollNum == 379231000
		|| ScrollNum == 379232000
		|| ScrollNum == 379233000
		|| ScrollNum == 379234000
		|| ScrollNum == 379235000
		|| ScrollNum == 379255000)
		return LowClassScroll;

	else if (ScrollNum == 379205000
		|| ScrollNum == 379206000
		|| ScrollNum == 379208000
		|| ScrollNum == 379209000
		|| ScrollNum == 379210000
		|| ScrollNum == 379211000
		|| ScrollNum == 379212000
		|| ScrollNum == 379213000
		|| ScrollNum == 379214000
		|| ScrollNum == 379215000
		|| ScrollNum == 379216000
		|| ScrollNum == 379217000
		|| ScrollNum == 379218000
		|| ScrollNum == 379219000
		|| ScrollNum == 379220000)
		return MiddleClassScroll;

	else if (ScrollNum == 379021000
		|| ScrollNum == 379022000
		|| ScrollNum == 379023000
		|| ScrollNum == 379024000
		|| ScrollNum == 379025000
		|| ScrollNum == 379030000
		|| ScrollNum == 379031000
		|| ScrollNum == 379032000
		|| ScrollNum == 379033000
		|| ScrollNum == 379034000
		|| ScrollNum == 379035000
		|| ScrollNum == 379138000
		|| ScrollNum == 379139000
		|| ScrollNum == 379140000
		|| ScrollNum == 379141000
		|| ScrollNum == 379016000
		|| ScrollNum == 379020000
		|| ScrollNum == 379018000
		|| ScrollNum == 379019000)
		return HighClassScroll;

	else if (ScrollNum == 379256000)
		return HighToRebirthScroll;

	else if (ScrollNum == 379257000)
		return RebirthClassScroll;

	else if (ScrollNum == 379159000
		|| ScrollNum == 379160000
		|| ScrollNum == 379161000
		|| ScrollNum == 379162000
		|| ScrollNum == 379163000
		|| ScrollNum == 379164000)
		return AccessoriesClassScroll;

	else if (ScrollNum == 379152000)
		return ClassScroll;

	return InvalidScroll;
}

/**
* @brief	Packet handler for the standard item upgrade system.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgrade(Packet & pkt, uint8 nUpgradeType)
{
	Packet result(WIZ_ITEM_UPGRADE, nUpgradeType);
	_ITEM_DATA  * pOriginItem;
	_ITEM_TABLE * proto;
	CNpc* pNpc;
	DateTime time;
	UpgradeScrollType m_ScrollType; // the Scroll type that shall be used for this upgrade.
	UpgradeItemsMap nItems; // the items sent by the client 
	UpgradeIterator OriginItem;
	uint16 sNpcID;
	int32 nItemID; int8 bPos;
	UpgradeScrollType pUserScroll = InvalidScroll;

	int8 bType = UpgradeType::UpgradeTypeNormal, bResult = UpgradeErrorCodes::UpgradeNoMatch, ItemClass = 0;
	bool trina = false, karivdis = false, Accessories = false;

	bool isTakiBasar = false;
	bool isitembasar = false;
	bool isrebirthbasar = false;
	bool islemitembas = false;

	if (isTrading() || isMerchanting() || isMining() || isFishing())
	{
		bResult = UpgradeTrading;
		goto fail_return;
	}

	pkt >> bType; // either preview or upgrade, need to allow for these types
	pkt >> sNpcID;

	pNpc = g_pMain->GetNpcPtr(sNpcID, GetZoneID());
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_ANVIL)
		goto fail_return;

	_SERVER_SETTINGS *pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);
	if (pSettings == nullptr)
		goto fail_return;

	if (m_sUserUpgradeCount >= pSettings->UserMaxUpgradeCount)
		goto fail_return;

	int counter = 0;
	for (int i = 0; i < 10; i++) // client sends 10 items in a row, starting with the item to be upgraded and latter the required items
	{
		pkt >> nItemID >> bPos;

		// Invalid slot ID
		if (bPos != -1 && bPos >= HAVE_MAX)
			goto fail_return;

		// We do not care where the user put the scroll in his screen. We do only care
		// is that whether the received Item IDs and POSs are valid.
		if (nItemID > 0 && bPos >= 0 && bPos < HAVE_MAX)
		{
			ITEM_UPGRADE_ITEM pUpgradeItem;
			pUpgradeItem.nItemID = nItemID;
			pUpgradeItem.bPos = bPos;
			nItems.insert(std::make_pair(counter++, pUpgradeItem));
		}
	}

	if (nItems.empty()) // if no items are available, then it should fail.
		goto fail_return;

	foreach(itr, nItems) // we shall check if the items sent by the client do really exist on the user.
	{
		_ITEM_DATA  * pItem = GetItem(SLOT_MAX + itr->second.bPos);
		_ITEM_TABLE * pTable = g_pMain->GetItemPtr(itr->second.nItemID);
		// Here we check and iterate through the users inventory and check
		// whether the item ids and positions are valid in the users inventory.
		// The reason why we do that is that we never trust a player.
		if (pItem == nullptr
			|| pTable == nullptr
			|| pTable->m_bKind == 250
			|| pTable->m_bKind == 252
			|| pTable->m_bKind == 253
			|| pTable->m_bKind == 254
			//|| pTable->m_bKind == 255
			|| pItem->nNum != itr->second.nItemID)
			goto fail_return;
		// We shall allow no user to upgrade items with protected types like bounded, sealed etc. 
		// Even worse trying to upgrade a duplicate item cant be accepted.
		// The official behavious is to check the origin item only but c'mon
		// a user shouldnt upgrade an item by using a sealed scroll too.
		else if (pItem->isDuplicate()
			|| pItem->isRented()
			|| pItem->isSealed()
			|| pItem->isExpirationTime())
		{
			bResult = UpgradeErrorCodes::UpgradeRental;
			goto fail_return;
		}
	}


	// Now we are about to put the data on our pointers, iterators that
	// we are going to use for the rest of upgrade process.
	OriginItem = nItems.begin(); // this is our first item in the map which is going to be upgraded.
	if (OriginItem == nItems.end())
		goto fail_return;

	proto = g_pMain->GetItemPtr(OriginItem->second.nItemID);
	pOriginItem = GetItem(SLOT_MAX + OriginItem->second.bPos);
	if (proto == nullptr || pOriginItem == nullptr)
		goto fail_return;

	foreach(itr, nItems)
	{
		pUserScroll = GetScrollType(itr->second.nItemID);

		if (pUserScroll != InvalidScroll)
			break;
	}

	if (pUserScroll == InvalidScroll)
		goto fail_return;

	int nReqOriginItem = OriginItem->second.nItemID;
	{
		_ITEM_UPGRADE * pUpgrade = nullptr;
		foreach_stlmap(itr, g_pMain->m_ItemUpgradeArray)
		{
			pUpgrade = itr->second;

			if (pUpgrade == nullptr)
				continue;

			if (pUpgrade->sOriginItem != nReqOriginItem)
				continue;

			if ((OriginItem->second.nItemID / MIN_ITEM_ID) != pUpgrade->nIndex / 100000
				&& pUpgrade->nIndex < 300000)
				continue;

			if (IsExistInMap(nItems, ITEM_TRINA)) // Trina Piece 
				trina = true;

			if (IsExistInMap(nItems, ITEM_KARIVDIS)) // Karvis Rebirth Trina
				karivdis = true;

			if ((trina && karivdis)
				|| IsExistInMap(nItems, ITEM_TRINA) > 1
				|| IsExistInMap(nItems, ITEM_KARIVDIS) > 1)
				goto fail_return;

			// Up to this point, we have checked basic requirements and whether
			// the user is trying to fool us with dummy packets. Yet, it still
			// needs to be checked whether the items are valid for this kind
			// of upgrade.

			if (pUpgrade->bOriginType != -1)
			{
				switch (pUpgrade->bOriginType)
				{
				case 0:
					if (proto->m_bKind != 11
						&& proto->m_bKind != 21
						&& proto->m_bKind != 52)
						continue;
					break;
				case 1:
					if (proto->m_bKind != 21
						&& proto->m_bKind != 22)
						continue;
					break;
				case 2:
				case 26:
					if (proto->m_bKind != 22)
						continue;
					break;
				case 3:
					if (proto->m_bKind != 31)
						continue;
					break;
				case 4:
				case 28:
					if (proto->m_bKind != 32)
						continue;
					break;
				case 5:
					if (proto->m_bKind != 41
						&& proto->m_bKind != 62)
						continue;
					break;
				case 6:
				case 30:
					if (proto->m_bKind != 42)
						continue;
					break;
				case 7:
				case 31:
					if (proto->m_bKind != 51)
						continue;
					break;
				case 8:
				case 32:
					if (proto->m_bKind != 52)
						continue;
					break;
				case 9:
					if (proto->m_bKind != 70
						&& proto->m_bKind != 71)
						continue;
					break;
				case 10:
					if (proto->m_bKind != 21
						&& proto->m_bKind != 110)
						continue;
					break;
				case 11:
					if (proto->m_bKind != 41
						&& proto->m_bKind != 181)
						continue;
					break;
				case 12:
				case 36:
					if (proto->m_bKind != 60)
						continue;
					break;
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
					if (proto->m_bKind != 210
						&& proto->m_bKind != 220
						&& proto->m_bKind != 230
						&& proto->m_bKind != 240)
						continue;
					break;
				case 18:
					if (proto->m_bKind != 91)
						continue;
					break;
				case 19:
					if (proto->m_bKind != 92)
						continue;
					break;
				case 20:
					if (proto->m_bKind != 93)
						continue;
					break;
				case 21:
					if (proto->m_bKind != 94)
						continue;
					break;
				case 23:
					if (proto->m_bKind != 91
						&& proto->m_bKind != 92
						&& proto->m_bKind != 93
						&& proto->m_bKind != 94
						&& proto->m_bKind != 11
						&& proto->m_bKind != 21
						&& proto->m_bKind != 22
						&& proto->m_bKind != 32
						&& proto->m_bKind != 51
						&& proto->m_bKind != 42
						&& proto->m_bKind != 70
						&& proto->m_bKind != 71
						&& proto->m_bKind != 60
						&& proto->m_bKind != 110
						&& proto->m_bKind != 181)
						continue;
					break;
				case 24:
					if (proto->m_bKind != 11)
						continue;
					break;
				case 25:
					if (proto->m_bKind != 21)
						continue;
					break;
				case 27:
					if (proto->m_bKind != 110
						&& proto->m_bKind != 31)
						continue;
					break;
				case 29:
					if (proto->m_bKind != 41)
						continue;
					break;
				case 33:
					if (proto->m_bKind != 70
						&& proto->m_bKind != 71
						&& proto->m_bKind != 110)
						continue;
					break;
				case 34:
					if (proto->m_bKind != 110)
						continue;
					break;
				case 35:
					if (proto->m_bKind != 181
						&& proto->m_bKind != 110)
						continue;
					break;
				case 43:
				case 44:
					if (proto->m_bKind != 140)
						continue;
					break;
				}
			}

			if ((OriginItem->second.nItemID / MIN_ITEM_ID) != (pUpgrade->nIndex / 100000)
				&& ((pUpgrade->nIndex / 100000) == 1
					|| (pUpgrade->nIndex / 100000) == 2))
				continue;

			bool isValidMatch = false;
			// Does our upgrade attempt match the requirements for this upgrade entry?
			for (int x = 1; x < MAX_ITEMS_REQ_FOR_UPGRADE; x++)
			{
				if (pUpgrade->nReqItem[x - 1] == 0)
					continue;

				if (IsExistInMap(nItems, pUpgrade->nReqItem[x - 1]))
				{
					isValidMatch = true;
					break;
				}
			}

			// Not a valid match, try another row.
			if (!isValidMatch)
				continue;

			switch (proto->ItemClass)
			{
			case 0:
				if (proto->ItemClass == 0)
				{
					if (proto->m_bKind == 91
						|| proto->m_bKind == 92
						|| proto->m_bKind == 93
						|| proto->m_bKind == 94)
						m_ScrollType = UpgradeScrollType::AccessoriesClassScroll;
					else
						m_ScrollType = UpgradeScrollType::LowClassScroll;
					break;
				}
			case 1:
				if (proto->ItemClass == 1)
					m_ScrollType = UpgradeScrollType::LowClassScroll;
				break;
			case 2:
				if (proto->ItemClass == 2)
					m_ScrollType = UpgradeScrollType::MiddleClassScroll;
				break;
			case 3:
				if (proto->ItemClass == 3)
					m_ScrollType = UpgradeScrollType::HighClassScroll;
				break;
			case 4:
				if (proto->ItemClass == 4)
					m_ScrollType = UpgradeScrollType::HighToRebirthScroll;
				break;
			case 5:
				if (proto->ItemClass == 5)
					m_ScrollType = UpgradeScrollType::RebirthClassScroll;
				break;
			case 7:
				if (proto->ItemClass == 7)
					m_ScrollType = UpgradeScrollType::MiddleClassScroll;
				break;
			case 8:
				if (proto->ItemClass == 8)
					m_ScrollType = UpgradeScrollType::HighToRebirthScroll;
				break;
			case 22:
				if (proto->ItemClass == 22)
					m_ScrollType = UpgradeScrollType::AccessoriesClassScroll;
				break;
			case 33:
				if (proto->ItemClass == 33)
					m_ScrollType = UpgradeScrollType::HighClassScroll;
				break;
			case 34:
				if (proto->ItemClass == 34)
					m_ScrollType = UpgradeScrollType::HighToRebirthScroll;
				break;
			case 35:
				if (proto->ItemClass == 35)
					m_ScrollType = UpgradeScrollType::RebirthClassScroll;
				break;
			default:
				m_ScrollType = UpgradeScrollType::InvalidScroll;
				break;
			}


			if (m_ScrollType == LowClassScroll
				&& (pUserScroll != LowClassScroll
					&& pUserScroll != MiddleClassScroll
					&& pUserScroll != HighClassScroll
					&& pUserScroll != ClassScroll))
				goto fail_return;
			else if (m_ScrollType == MiddleClassScroll
				&& (pUserScroll != MiddleClassScroll
					&& pUserScroll != HighClassScroll
					&& pUserScroll != ClassScroll))
				goto fail_return;
			else if (m_ScrollType == HighClassScroll
				&& (pUserScroll != HighClassScroll
					&& pUserScroll != HighToRebirthScroll
					&& pUserScroll != ClassScroll))
				goto fail_return;
			else if (m_ScrollType == HighToRebirthScroll
				&& (pUserScroll != HighToRebirthScroll
					&& pUserScroll != HighClassScroll))
				goto fail_return;
			else if (m_ScrollType == RebirthClassScroll
				&& pUserScroll != RebirthClassScroll)
				goto fail_return;
			else if (m_ScrollType == AccessoriesClassScroll
				&& pUserScroll != AccessoriesClassScroll)
				goto fail_return;

			if (!hasCoins(pUpgrade->nReqNoah))
			{
				bResult = UpgradeNeedCoins;
				goto fail_return;
			}

			if (karivdis
				|| trina)
			{
				if (karivdis)
				{
					if (pUserScroll != RebirthClassScroll)
					{
						bResult = UpgradeNoMatch;
						goto fail_return;
					}
				}

				if (trina)
				{
					if (pUserScroll == RebirthClassScroll)
					{
						bResult = UpgradeNoMatch;
						goto fail_return;
					}
				}
			}


			bResult = UpgradeSucceeded;
			break;
		}

		// If we ran out of upgrades to search through, it failed.
		if (bResult != UpgradeSucceeded
			|| pUpgrade == nullptr)
			goto fail_return;

		int rand = myrand(0, myrand(9000, 10000)), GenRate;
		
		GenRate = pUpgrade->sGenRate;

		if (GenRate == UpgradeFailed)
		{
			bResult = UpgradeNoMatch;
			goto fail_return;
		}

		//Trina & Kravdis Upgrade Rate
		switch (pUserScroll)
		{
		case RebirthClassScroll:
			if (GenRate > 2000)
				GenRate = 2000;

			if (karivdis)
				GenRate += 3000;
			break;

		case AccessoriesClassScroll:
			break;

		default:
			if (trina)
				GenRate += (GenRate * 20) / 100;
			break;
		}

		if (pUserScroll == AccessoriesClassScroll && CheckExistItem(379266000, 1))//&& pUpgrade->sGenRate > 0){ 
		{
			isTakiBasar = true;
			GenRate += 10000;
		}


		//if(isitembasar) //&& pUpgrade->sGenRate > 0 ) //..
		if (pUserScroll == RebirthClassScroll && CheckExistItem(379267000, 1))//&& pUpgrade->sGenRate > 0){ 
		{
			GenRate += 10000;
		}
		else
			isrebirthbasar = false;


		//if(isitembasar) //&& pUpgrade->sGenRate > 0 ) //..
		if (pUserScroll == HighClassScroll && CheckExistItem(379265000, 1))//&& pUpgrade->sGenRate > 0){ 
		{
			GenRate += 10000;
		}
		else
			isitembasar = false;

		if (GenRate > 10000)
			GenRate = 10000;

		if (bType == UpgradeTypeNormal
			&& GenRate < rand)
		{
			bResult = UpgradeFailed;
			memset(pOriginItem, 0, sizeof(_ITEM_DATA));

			// Send upgrade notice.
			ItemUpgradeNotice(proto, UpgradeFailed);

			// Rob gold upgrade noah
			GoldLose(pUpgrade->nReqNoah, true);
			m_sUserUpgradeCount++;

			// Log System
			switch (pUserScroll)
			{
			case RebirthClassScroll:
			case HighToRebirthScroll:
				if (bResult == UpgradeFailed)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemRebirthUpgrade - %d:%d:%d ] %s : Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				break;
			case AccessoriesClassScroll:
				if (bResult == UpgradeFailed)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemAccessories - %d:%d:%d ] %s : Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				break;
			default:
				if (bResult == UpgradeFailed)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemUpgrade - %d:%d:%d ] %s : Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				break;
			}
		}
		else
		{
			// Generate the new item ID
			int nNewItemID = pOriginItem->nNum + pUpgrade->nGiveItem;

			int nNewItemLevel = nNewItemID % 10;

			// Does this new item exist?
			_ITEM_TABLE * newProto = g_pMain->GetItemPtr(nNewItemID);
			if (newProto == nullptr)
			{ // if not, just say it doesn't match. No point removing the item anyway (like official :/).
				bResult = UpgradeNoMatch;
				goto fail_return;
			}

			if (bType != UpgradeTypePreview)
			{
				// Update the user's item in their inventory with the new item
				pOriginItem->nNum = nNewItemID;

				// Reset the durability also, to the new cap.
				pOriginItem->sDuration = newProto->m_sDuration;

				// Send upgrade notice.
				ItemUpgradeNotice(newProto, UpgradeSucceeded);

				// Rob gold upgrade noah
				GoldLose(pUpgrade->nReqNoah, true);
				m_sUserUpgradeCount++;

				// Log System
				switch (pUserScroll)
				{
				case RebirthClassScroll:
				case HighToRebirthScroll:
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemRebirthUpgrade - %d:%d:%d ] %s : Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
					break;
				case AccessoriesClassScroll:
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemAccessories - %d:%d:%d ] %s : Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
					break;
				default:
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemUpgrade - %d:%d:%d ] %s : Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
					break;
				}
			}

			// Replace the item ID in the list for the packet
			OriginItem->second.nItemID = nNewItemID;
		}

		// Remove the source item 
		if (bType != UpgradeTypePreview)
		{
			// Remove all required items, if applicable.
			for (int i = 1; i < MAX_ITEMS_REQ_FOR_UPGRADE; i++)
			{
				auto xitr = nItems.find(i);
				if (xitr == nItems.end())
					continue;

				_ITEM_DATA * pItem = GetItem(SLOT_MAX + xitr->second.bPos);
				if (pItem == nullptr
					|| pItem->nNum == 0
					|| pItem->sCount == 0)
					continue;

				pItem->sCount--;
				if (pItem->sCount == 0 && pItem->nNum == xitr->second.nItemID)
					memset(pItem, 0, sizeof(pItem));
			}
		}
	}
	result << bType << bResult;
	foreach(i, nItems)
		result << i->second.nItemID << i->second.bPos;
	Send(&result);

	// Send the result to everyone in the area
	// (i.e. make the anvil do its burned/upgraded indicator thing)
	result.Initialize(WIZ_OBJECT_EVENT);
	result << uint8(OBJECT_ANVIL) << bResult << sNpcID;

	if (bType != UpgradeTypePreview)
	{
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	else
		Send(&result);

	if (isitembasar)
		RobItem(379265000, 1);
	if (isTakiBasar)
		RobItem(379266000, 1);
	if (isrebirthbasar)
		RobItem(379267000, 1);
	if (islemitembas) {
		Packet result_beginner(WIZ_SHOPPING_MALL, uint8(STORE_CLOSE));
		g_pMain->AddDatabaseRequest(result_beginner, this);
	}

	return;
fail_return:
	result << bType << bResult;
	foreach(i, nItems)
		result << i->second.nItemID << i->second.bPos;
	Send(&result);
}


/**
* @brief	Upgrade notice.
*
* @param	pItem	The item.
*/
void CUser::ItemUpgradeNotice(_ITEM_TABLE * pItem, uint8 UpgradeResult)
{
	bool bSendUpgradeNotice = false;

	if (pItem->ItemClass == 4
		|| pItem->ItemClass == 5
		|| pItem->ItemClass == 8
		|| pItem->ItemClass == 21
		|| pItem->ItemClass == 22
		|| pItem->ItemClass == 33
		|| pItem->ItemClass == 34
		|| pItem->ItemClass == 35)
		bSendUpgradeNotice = true;

	if (!bSendUpgradeNotice)
		return;

	_SERVER_SETTINGS *pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);
	if (pSettings == nullptr)
		return;

	if (pSettings->UpgradeNotice == Aktive) 
	{
		Packet result(WIZ_LOGOSSHOUT, uint8(0x02));
		result.SByte();
		result << uint8(0x05) << uint8(UpgradeResult) << GetName() << pItem->m_iNum;
		g_pMain->Send_All(&result);
	}
}
/**
* @brief	Packet handler for the accessory upgrade system.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgradeAccessories(Packet & pkt)
{
	ItemUpgrade(pkt, ITEM_ACCESSORIES);
}

/**
* @brief	Packet handler for the upgrading of 'rebirthed' items.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgradeRebirth(Packet & pkt)
{
	ItemUpgrade(pkt, ITEM_UPGRADE_REBIRTH);
}