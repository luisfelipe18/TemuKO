#include "stdafx.h"
#include "Map.h"
#include "../shared/DateTime.h"

using namespace std;

void CUser::ExchangeProcess(Packet & pkt)
{
	if (!isInGame())
		return;

	if (isGM())
		return;

	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
	case EXCHANGE_REQ:
		ExchangeReq(pkt);
		break;
	case EXCHANGE_AGREE:
		ExchangeAgree(pkt);
		break;
	case EXCHANGE_ADD:
		ExchangeAdd(pkt);
		break;
	case EXCHANGE_DECIDE:
		ExchangeDecide();
		break;
	case EXCHANGE_CANCEL:
		ExchangeCancel();
		break;
	default:
		printf("Trade unhanled packets %d \n",opcode);
		TRACE("Trade unhanled packets %d \n", opcode);
		break;
	}
}

void CUser::ExchangeReq(Packet & pkt)
{
	Packet result(WIZ_EXCHANGE);
	CUser * pUser;
	uint16 destid;
	DateTime time;
	if (isDead() 
		|| isMerchanting() 
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| !isInGame())
		goto fail_return;
	else if (isTrading())
	{
		ExchangeCancel();
		return;
	}

	if (GetLevel() < 30)
	{
		result.clear();
		result.Initialize(WIZ_EXCHANGE);
		result << uint8(8) << uint8(2);
		Send(&result);
		return;
	}

	destid = pkt.read<uint16>();
	pUser = g_pMain->GetUserPtr(destid);
	if (pUser == nullptr
		|| pUser->isDead()
		|| !pUser->isInGame()
		|| pUser->isTrading()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| pUser->GetZoneID() != GetZoneID()
		|| (pUser->GetNation() != GetNation() 
		&& !GetMap()->canTradeWithOtherNation()))
		goto fail_return;

	if(pUser->GetAccountName() == GetAccountName())
	{
		pUser->Disconnect();
		Disconnect();
		return;
	}

	m_sExchangeUser = destid;
	pUser->m_sExchangeUser = GetSocketID();
	m_isRequestSender = true;

	result << uint8(EXCHANGE_REQ) 
		<< GetSocketID();
	pUser->Send(&result);

	g_pMain->WriteTradeLogFile(string_format("TRADE REQUEST - %d:%d:%d || Source=%s,Target=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),pUser->GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
	return;

fail_return:
	result << uint8(EXCHANGE_CANCEL);
	Send(&result);
}

void CUser::ExchangeAgree(Packet & pkt)
{
	Packet result(WIZ_EXCHANGE);
	DateTime time;

	if (!isTrading()
		|| isDead()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
	{
		m_sExchangeUser = -1;
		goto fail_return;
	}

	uint8 bResult = pkt.read<uint8>();
	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
	{
		m_sExchangeUser = -1;

		if(pUser)
			pUser->m_sExchangeUser = -1;

		goto fail_return;
	}

	if(m_isRequestSender == true)
	{
		pUser->ExchangeFinish();
		DateTime time;
		g_pMain->WriteTradeLogFile(string_format("TRADE HACK - %d:%d:%d || Source=%s,Target=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),pUser->GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		Disconnect();
		return;
	}

	if (!bResult || pUser->isDead()) // declined
	{
		m_sExchangeUser = -1;
		pUser->m_sExchangeUser = -1;
		pUser->m_isRequestSender = false;
		bResult = 0;
	}

	if(pUser->GetAccountName() == GetAccountName())
	{
		pUser->Disconnect();
		Disconnect();
		return;
	}

	result << uint8(EXCHANGE_AGREE) 
		<< uint16(bResult);
	pUser->Send(&result);

	g_pMain->WriteTradeLogFile(string_format("TRADE AGREE - %d:%d:%d || Source=%s,Target=%s,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),pUser->GetName().c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
	return;

fail_return:
	result << uint8(EXCHANGE_CANCEL);
	Send(&result);
}

/**
* @brief	Exchange item adding.
*/
void CUser::ExchangeAdd(Packet & pkt)
{
	if (!isTrading() 
		|| isDead()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return;

	Packet result(WIZ_EXCHANGE, uint8(EXCHANGE_ADD));
	uint64 nSerialNum;
	DateTime time;
	uint32 nItemID, count = 0;
	uint16 duration = 0;
	_ITEM_DATA * pSrcItem = nullptr;
	list<_EXCHANGE_ITEM*>::iterator	Iter;
	uint8 pos;
	bool bAdd = true, bGold = false;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->isDead()
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
		return;

	if(pUser->GetAccountName() == GetAccountName())
	{
		pUser->Disconnect();
		Disconnect();
		return;
	}

	pkt >> pos >> nItemID >> count;
	if(count == 0)
	{
		pUser->Disconnect();
		Disconnect();
		return;
	}

	_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| (nItemID != ITEM_GOLD 
		&& (pos >= HAVE_MAX // Invalid position
		|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX // Cannot be traded, stored or sold.
		|| pTable->m_bRace == RACE_UNTRADEABLE)) // Cannot be traded or sold.
		|| m_bExchangeOK)
		goto add_fail;

	if (nItemID == ITEM_GOLD)
	{
		if (count <= 0 || count > m_iGold) 
			goto add_fail;

		// If we have coins in the list already
		// add to the amount of coins listed.
		foreach (itr, m_ExchangeItemList)
		{
			if ((*itr)->nItemID == ITEM_GOLD)
			{
				(*itr)->nCount += count;
				bAdd = false; /* don't need to add a new entry */
				break;
			}
		}

		m_iGold -= count;
	}
	else if ((pSrcItem = GetItem(SLOT_MAX + pos)) != nullptr
		&& pSrcItem->nNum == nItemID)
	{
		if (pSrcItem->sCount < count
			|| pSrcItem->sCount == 0
			|| pSrcItem->isRented()
			|| pSrcItem->isSealed()
			|| pSrcItem->isBound()
			|| pSrcItem->isDuplicate()
			|| pSrcItem->isExpirationTime())
			goto add_fail;

		if (pTable->m_bCountable)
		{
			foreach (itr, m_ExchangeItemList)
			{
				if ((*itr)->nItemID == nItemID)
				{
					(*itr)->nCount += count;
					bAdd = false;
					break;
				}
			}
		}

		pSrcItem->sCount -= count;

		duration = pSrcItem->sDuration;
		nSerialNum = pSrcItem->nSerialNum;
	}
	else
		goto add_fail;

	foreach (itr, m_ExchangeItemList)
	{
		if ((*itr)->nItemID == ITEM_GOLD)
		{
			bGold = true;
			break;
		}
	}

	if ((int)m_ExchangeItemList.size() > (bGold ? 13 : 12))
		goto add_fail;

	if (bAdd)
	{
		_EXCHANGE_ITEM * pItem = new _EXCHANGE_ITEM;
		pItem->nItemID = nItemID;
		pItem->sDurability = duration;
		pItem->nCount = count;
		pItem->nSerialNum = nSerialNum;
		pItem->bSrcPos = SLOT_MAX + pos;
		m_ExchangeItemList.push_back(pItem);
	}
	g_pMain->WriteTradeLogFile(string_format("TRADE ADD - %d:%d:%d || Source=%s,Target=%s,ItemID=%d,Count=%d,Zone=%d,X=%d,Z=%d\n",time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),pUser->GetName().c_str(),nItemID,count,GetZoneID(),uint16(GetX()),uint16(GetZ())));

	result << uint8(1);
	Send(&result);

	result.clear();

	result << uint8(EXCHANGE_OTHERADD)
		<< nItemID 
		<< count 
		<< duration;

	_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(nItemID);
	if (pItemTable != nullptr)
	{
		if (pItemTable->isPetItem() && bAdd)
			ShowPetItemInfo(result, nSerialNum);
		else if (pItemTable->GetNum() == ITEM_CYPHER_RING && bAdd)
			ShowCyperRingItemInfo(result, nSerialNum);
		else
			result << uint32(0); // Item Unique ID
	}
	else
		result << uint32(0); // Item Unique ID*/

	/*if (nItemID == ITEM_CYPHER_RING && bAdd)
		ShowCyperRingItemInfo(result, nSerialNum);
	else
		result << uint32(0); // Item Unique ID*/

	pUser->Send(&result);
	return;

add_fail:
	result << uint8(0);
	Send(&result);
}

void CUser::ExchangeDecide()
{
	if (!isTrading() 
		|| isDead()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);

	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->isDead()
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
		return;

	Packet result(WIZ_EXCHANGE);
	if (!pUser->m_bExchangeOK)
	{
		m_bExchangeOK = 1;
		result << uint8(EXCHANGE_OTHERDECIDE);
		pUser->Send(&result);
		return;
	}

	// Did the exchange requirements fail?
	if (!CheckExchange() || !pUser->CheckExchange())
	{
		// At this stage, neither user has their items exchanged.
		// However, their coins were removed -- these will be removed by ExchangeFinish().
		result << uint8(EXCHANGE_DONE) << uint8(0);
		Send(&result);

		if (!pUser->CheckExchange())
			pUser->Send(&result);

		ExchangeCancel();

		if (!pUser->CheckExchange())
			pUser->ExchangeCancel();
	}
	else
	{
		if(!CheckExecuteExchange() || !pUser->CheckExecuteExchange())
		{
			// At this stage, neither user has their items exchanged.
			// However, their coins were removed -- these will be removed by ExchangeFinish().
			result << uint8(EXCHANGE_DONE) << uint8(0);
			Send(&result);

			if (!pUser->CheckExecuteExchange())
				pUser->Send(&result);

			ExchangeCancel();

			if (!pUser->CheckExecuteExchange())
				pUser->ExchangeCancel();
		}
		else
		{
			ExecuteExchange();
			pUser->ExecuteExchange();

			Packet result(WIZ_EXCHANGE);
			result << uint8(EXCHANGE_DONE) << uint8(1)
				<< GetCoins()
				<< uint16(pUser->m_ExchangeItemList.size());
			DateTime time;
			foreach(itr, pUser->m_ExchangeItemList)
			{
				result << (*itr)->bDstPos << (*itr)->nItemID
					<< uint16((*itr)->nCount) << (*itr)->sDurability;

				_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr((*itr)->nItemID);
				if (pItemTable != nullptr)
				{
					if (pItemTable->isPetItem())
						ShowPetItemInfo(result, (*itr)->nSerialNum);
					else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
						ShowCyperRingItemInfo(result, (*itr)->nSerialNum);
					else
						result << uint32(0); // Item Unique ID
				}
				else
					result << uint32(0); // Item Unique ID*/

				/*if ((*itr)->nItemID == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, (*itr)->nSerialNum);
				else
					result << uint32(0); // Item Unique ID*/

				g_pMain->WriteTradeLogFile(string_format("TRADED ITEMS - %d:%d:%d || Source=%s,Target=%s,ItemID=%d,Count=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pUser->GetName().c_str(), (*itr)->nItemID, (*itr)->nCount, GetZoneID(), uint16(GetX()), uint16(GetZ())));
			}
			Send(&result);

			result.clear();

			result << uint8(EXCHANGE_DONE) << uint8(1)
				<< pUser->GetCoins()
				<< uint16(m_ExchangeItemList.size());

			foreach(itr, m_ExchangeItemList)
			{
				result << (*itr)->bDstPos
					<< (*itr)->nItemID
					<< uint16((*itr)->nCount)
					<< (*itr)->sDurability;

				_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr((*itr)->nItemID);
				if (pItemTable != nullptr)
				{
					if (pItemTable->isPetItem())
						ShowPetItemInfo(result, (*itr)->nSerialNum);
					else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
						ShowCyperRingItemInfo(result, (*itr)->nSerialNum);
					else
						result << uint32(0); // Item Unique ID
				}
				else
					result << uint32(0); // Item Unique ID*/

				/*if ((*itr)->nItemID == ITEM_CYPHER_RING)
					ShowCyperRingItemInfo(result, (*itr)->nSerialNum);
				else
					result << uint32(0); // Item Unique ID*/

				g_pMain->WriteTradeLogFile(string_format("TRADED ITEMS - %d:%d:%d || Source=%s,Target=%s,ItemID=%d,Count=%d,Zone=%d,X=%d,Z=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pUser->GetName().c_str(), (*itr)->nItemID, (*itr)->nCount, GetZoneID(), uint16(GetX()), uint16(GetZ())));
			}
			pUser->Send(&result);

			SetUserAbility(false);
			SendItemWeight();
			ExchangeFinish();

			pUser->SetUserAbility(false);
			pUser->SendItemWeight();
			pUser->ExchangeFinish();
		}
	}
}

void CUser::ExchangeCancel(bool bIsOnDeath)
{
	if (!isTrading() 
		|| (!bIsOnDeath && isDead())
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
	{
		ExchangeFinish();
		return;
	}

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
	{
		if(pUser)
			pUser->ExchangeFinish();
		return;
	}

	Packet result(WIZ_EXCHANGE, uint8(EXCHANGE_CANCEL));

	ExchangeGiveItemsBack();
	ExchangeFinish();
	Send(&result);

	pUser->ExchangeGiveItemsBack();
	pUser->ExchangeFinish();
	pUser->Send(&result);
}

void CUser::ExchangeFinish()
{
	m_sExchangeUser = -1;
	m_bExchangeOK = 0;
	m_ExchangeItemList.clear();
	m_isRequestSender = false;
}

void CUser::ExchangeGiveItemsBack()
{
	// Restore coins and items...
	while (m_ExchangeItemList.size())
	{
		_EXCHANGE_ITEM *pItem = m_ExchangeItemList.front();
		if (pItem != nullptr)
		{
			// Restore coins to owner
			if (pItem->nItemID == ITEM_GOLD)
				m_iGold += pItem->nCount;
			// Restore items to owner
			else
			{
				_ITEM_DATA * pItems = GetItem(pItem->bSrcPos);
				if (pItems != nullptr 
					&& pItems->nNum == pItem->nItemID)
					GetItem(pItem->bSrcPos)->sCount += pItem->nCount;
			}
			delete pItem;
		}

		m_ExchangeItemList.pop_front();
	}
}

/**
* @brief	Determines if a trade will be successful.
* 			If it's successful, we can exchange the items.
*
* @return	true if it succeeds, false if it fails.
*/
bool CUser::CheckExchange()
{
	if (!isTrading() 
		|| isDead()
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return false;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
		return false;

	uint32 money = 0;
	uint16 weight = 0;
	// Get the total number of free slots in the player's inventory
	uint8 bFreeSlots = 0, bItemCount = 0;
	for (uint8 i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA * pItem = GetItem(i);
		if (pItem->nNum == 0)
			bFreeSlots++;
	}

	// Loop through the other user's list of items up for trade.
	foreach (Iter, pUser->m_ExchangeItemList)
	{
		// If we're trading coins, ensure we don't exceed our limit.
		if ((*Iter)->nItemID == ITEM_GOLD)
		{
			money += (*Iter)->nCount;
			if ((m_iGold + money) > COIN_MAX)
				return false;

			continue;
		}

		// Does this item exist?
		_ITEM_TABLE *pTable = g_pMain->GetItemPtr((*Iter)->nItemID);
		if (pTable == nullptr)
			return false;

		// Is there enough room for this item?
		// NOTE: Also ensures we have enough space in our inventory (with one exchange in mind anyway)
		if (!CheckWeight(pTable, (*Iter)->nItemID, (*Iter)->nCount))
			return false;

		// Total up the weight.
		weight += pTable->m_sWeight;
		bItemCount++;
	}

	// Do we have enough free slots for all these items?
	if (bItemCount > bFreeSlots)
		return false; /* note: ignores item stacks for now */

	// Ensure the total combined item weight does not exceed our weight limit
	return ((weight + m_sItemWeight) <= m_sMaxWeight);
}

void CUser::ExecuteExchange()
{
	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID())
		return;

	ItemList::iterator coinItr = pUser->m_ExchangeItemList.end();
	foreach (Iter, pUser->m_ExchangeItemList)
	{
		if ((*Iter)->nItemID == ITEM_GOLD)
		{
			m_iGold += (*Iter)->nCount;
			coinItr = Iter;
			continue;
		}

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr((*Iter)->nItemID);
		if (pTable == nullptr)
			continue;

		int nSlot = FindSlotForItem((*Iter)->nItemID, (*Iter)->nCount);

		if(nSlot < 0 || nSlot >= SLOT_MAX+HAVE_MAX)
			return;

		_ITEM_DATA * pDstItem = GetItem(nSlot);
		_ITEM_DATA * pSrcItem = pUser->GetItem((*Iter)->bSrcPos);

		if(pDstItem == nullptr
			|| pSrcItem == nullptr 
			|| pSrcItem->nNum != pTable->m_iNum)
			return;

		if (pTable->m_bKind == 255 && pDstItem->nNum != 0)
			return;

		if(pDstItem->nNum == pSrcItem->nNum || pDstItem->nNum == 0)
		{
			DateTime time;
			g_pMain->WriteTradeLogFile(string_format("TRADE EXCHANGE - %d:%d:%d || Source=%s,Target=%s, pDstItem->nNum=%d,pDstItem->sCount=%d,pSrcItem->nNum=%d,pSrcItem->sCount=%d,pSrcItem->nSerialNum=%016llX,(*Iter)->nItemID=%d,(*Iter)->nCount=%d, Zone=%d,X=%d,Z=%d\n",
				time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),pUser->GetName().c_str(),pDstItem->nNum,pDstItem->sCount,pSrcItem->nNum,pSrcItem->sCount,pSrcItem->nSerialNum,(*Iter)->nItemID,(*Iter)->nCount,GetZoneID(),uint16(GetX()),uint16(GetZ())));

			pDstItem->nNum = pTable->m_iNum;
			pDstItem->sCount += (*Iter)->nCount;

			if (pDstItem->sCount > MAX_ITEM_COUNT)
				pDstItem->sCount = MAX_ITEM_COUNT;

			pDstItem->sDuration = pSrcItem->sDuration;
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

			// Set destination position for use in packet to client
			// to let them know where the item is.
			(*Iter)->bDstPos = (uint8) (nSlot - SLOT_MAX);

			// Remove the item from the other player.
			if (pSrcItem->sCount <= 0)
				memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		}
		else
			return;
	}

	// Remove coins from the list so it doesn't get sent
	// with the rest of the packet.
	if (coinItr != pUser->m_ExchangeItemList.end())
	{
		delete *coinItr;
		pUser->m_ExchangeItemList.erase(coinItr);
	}
}

bool CUser::CheckExecuteExchange()
{
	if (!isTrading() 
		|| isMining()
		|| isFishing()
		|| isSellingMerchantingPreparing()
		|| isBuyingMerchantingPreparing()
		|| isMerchanting()
		|| !isInGame())
		return false;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->m_sExchangeUser != GetSocketID()
		|| pUser->GetZoneID() != GetZoneID()
		|| pUser->isMining()
		|| pUser->isFishing()
		|| pUser->isSellingMerchantingPreparing()
		|| pUser->isBuyingMerchantingPreparing()
		|| pUser->isMerchanting()
		|| (pUser->GetNation() != GetNation() && !GetMap()->canTradeWithOtherNation()))
		return false;

	foreach (Iter, pUser->m_ExchangeItemList)
	{
		if ((*Iter)->nItemID == ITEM_GOLD)
			continue;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr((*Iter)->nItemID);
		if (pTable == nullptr)
			return false;

		int nSlot = FindSlotForItem((*Iter)->nItemID, (*Iter)->nCount);

		if(nSlot < 0 || nSlot >= SLOT_MAX+HAVE_MAX)
			return false;

		_ITEM_DATA * pDstItem = GetItem(nSlot);
		_ITEM_DATA * pSrcItem = pUser->GetItem((*Iter)->bSrcPos);

		if(pDstItem == nullptr
			|| pSrcItem == nullptr 
			|| pSrcItem->nNum != pTable->m_iNum)
			return false;

		if (pTable->m_bKind == 255 && pDstItem->nNum != 0)
			return false;

		if(pDstItem->nNum != pSrcItem->nNum && pDstItem->nNum != 0)
			return false;
	}

	return true;
}