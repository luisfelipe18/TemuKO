#include "stdafx.h"

/**
* @brief	Packet handler for the Special Old Man Exchange
* 			which is used to exchange ItemExchangeCrash.
*
* @param	pkt	The packet.
*/

void CUser::ItemSmashExchange(Packet & pkt)
{
	enum ResultOpCodes
	{
		Success = 1,
		InventoryFull = 2,
		BagFull = 3,
		CannotItem = 4,
		NpcType = 5,
	};

	enum ResultMessages
	{
		RollItem1 = 1,
		RollItem2 = 2,
		RollItem3 = 3
	};

	uint32 OriginItemID = 0;
	uint8 InventoryRanking;
	uint16 nObjectID = 0;
	uint32 bRandSlot[RollItem3];
	uint32 nExchangeID[RollItem3];
	std::vector<uint32> ExchangeIndexCrashList;
	uint8 RollItem = 0;
	uint32 bRandArray[10000];
	_ITEM_EXCHANGE_CRASH * pExchangeCrashItem = nullptr;
	pkt >> OriginItemID >> InventoryRanking >> nObjectID;
	Packet result(WIZ_ITEM_UPGRADE);

	_ITEM_DATA* ItemData = GetItem(InventoryRanking + SLOT_MAX);
	_ITEM_TABLE* ItemTable = g_pMain->GetItemPtr(OriginItemID);
	CNpc * pNpc = g_pMain->GetNpcPtr(nObjectID, GetZoneID());

	if (ItemData == nullptr
		|| ItemTable == nullptr
		|| pNpc == nullptr
		|| (pNpc->GetType() != NPC_OLD_MAN_NPC)
		|| isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing())
	{
		result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(NpcType);
		Send(&result);
		return;
	}

	if (g_pMain->m_ItemExchangeCrashArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_ItemExchangeCrashArray)
		{
			if (std::find(ExchangeIndexCrashList.begin(), ExchangeIndexCrashList.end(), itr->second->nIndex) == ExchangeIndexCrashList.end())
			{
				switch (ItemTable->ItemClass)
				{
				case 2:
				case 7:
					if (ItemTable->ItemClass == 2 ||
						ItemTable->ItemClass == 7)
					{
						if (itr->second->nIndex >= 1000000 && itr->second->nIndex < 2000000)
						{
							ExchangeIndexCrashList.push_back(itr->second->nIndex);
						}
					}
					break;
				case 3:
				case 4:
				case 5:
				case 8:
					if (ItemTable->ItemClass == 3 ||
						ItemTable->ItemClass == 4 ||
						ItemTable->ItemClass == 5 ||
						ItemTable->ItemClass == 8)
					{
						if (itr->second->nIndex >= 2000000 && itr->second->nIndex < 3000000)
						{
							ExchangeIndexCrashList.push_back(itr->second->nIndex);
						}
					}
					break;
				case 32:
				case 33:
				case 34:
				case 35:
				case 37:
				case 38:
					if (ItemTable->ItemClass == 32 ||
						ItemTable->ItemClass == 33 ||
						ItemTable->ItemClass == 34 ||
						ItemTable->ItemClass == 35 ||
						ItemTable->ItemClass == 37 ||
						ItemTable->ItemClass == 38)
					{
						if (itr->second->nIndex >= 3000000 && itr->second->nIndex < 4000000)
						{
							ExchangeIndexCrashList.push_back(itr->second->nIndex);
						}
					}
					break;
				case 21:
					if (ItemTable->ItemClass == 21)
					{
						if (itr->second->nIndex >= 4000000 && itr->second->nIndex < 5000000)
						{
							ExchangeIndexCrashList.push_back(itr->second->nIndex);
						}
					}
					break;
				case 31:
				case 22:
					if (ItemTable->ItemClass == 31 ||
						ItemTable->ItemClass == 22)
					{
						if (itr->second->nIndex >= 5000000 && itr->second->nIndex < 6000000)
						{
							ExchangeIndexCrashList.push_back(itr->second->nIndex);
						}
					}
					break;
				default:
				{
					result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(NpcType);
					Send(&result);
					return;
				}
				break;
				}
			}
		}
	}
	if (ExchangeIndexCrashList.size() > 0)
	{
		switch (ItemTable->ItemClass)
		{
		case 2:
		case 7:
		case 31:
		case 21:
		case 22:
			if (ItemTable->ItemClass == 2 ||
				ItemTable->ItemClass == 7 ||
				ItemTable->ItemClass == 31 ||
				ItemTable->ItemClass == 21 ||
				ItemTable->ItemClass == 22)
				RollItem = RollItem1;
			break;
		case 3:
		case 4:
		case 5:
		case 8:
			if (ItemTable->ItemClass == 3 ||
				ItemTable->ItemClass == 4 ||
				ItemTable->ItemClass == 5 ||
				ItemTable->ItemClass == 8)
				RollItem = RollItem2;
			break;
		case 32:
		case 33:
		case 34:
		case 35:
		case 37:
		case 38:
			if (ItemTable->ItemClass == 32 ||
				ItemTable->ItemClass == 33 ||
				ItemTable->ItemClass == 34 ||
				ItemTable->ItemClass == 35 ||
				ItemTable->ItemClass == 37 ||
				ItemTable->ItemClass == 38)
				RollItem = RollItem3;
			break;
		default:
		{
			result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
			Send(&result);
			return;
		}
		break;
		}

		if (ItemTable->ItemClass != 2
			&& (ItemTable->ItemClass != 3)
			&& (ItemTable->ItemClass != 4)
			&& (ItemTable->ItemClass != 5)
			&& (ItemTable->ItemClass != 7)
			&& (ItemTable->ItemClass != 8)
			&& (ItemTable->ItemClass != 31)
			&& (ItemTable->ItemClass != 32)
			&& (ItemTable->ItemClass != 33)
			&& (ItemTable->ItemClass != 34)
			&& (ItemTable->ItemClass != 35)
			&& (ItemTable->ItemClass != 37)
			&& (ItemTable->ItemClass != 38)
			&& (ItemTable->ItemClass != 21)
			&& (ItemTable->ItemClass != 22))
		{
			result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
			Send(&result);
			return;
		}

		if (ItemTable->m_ItemType == 4
			|| ItemTable->m_ItemType == 12)
		{
			if (!hasCoins(100000))
			{
				result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
				Send(&result);
				return;
			}
		}
		else
		{
			if (!hasCoins(10000))
			{
				result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
				Send(&result);
				return;
			}
		}

		bool CheckSlot = CheckGiveSlot(RollItem);
		if (!CheckSlot)
		{
			result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(InventoryFull);
			Send(&result);
			return;
		}

		if (RollItem < RollItem1
			|| RollItem > RollItem3)
		{
			result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
			Send(&result);
			return;
		}

		for (int i = 0; i < RollItem; i++)
		{
			int offset[3] = { 0, 0 ,0 };
			memset(&bRandArray, 0, sizeof(bRandArray));

			foreach(itr, ExchangeIndexCrashList)
			{
				pExchangeCrashItem = g_pMain->m_ItemExchangeCrashArray.GetData(*itr);
				if (pExchangeCrashItem == nullptr)
					continue;

				if (offset[i] >= 10000)
					break;

				for (int z = 0; z < int(pExchangeCrashItem->sRate / 5); z++)
				{
					if (z + offset[i] >= 10000)
						break;

					bRandArray[offset[i] + z] = pExchangeCrashItem->nIndex;
				}

				offset[i] += int(pExchangeCrashItem->sRate / 5);
			}

			bRandSlot[i] = myrand(0, offset[i]);
			nExchangeID[i] = bRandArray[bRandSlot[i]];
		}

		if (!ItemSmashSystemRobItemCheck(SLOT_MAX + InventoryRanking, OriginItemID, 1))
		{
			result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(CannotItem);
			Send(&result);
			return;
		}

		if (ItemTable->m_ItemType == 4
			|| ItemTable->m_ItemType == 12)
			GoldLose(100000, true);
		else
			GoldLose(10000, true);

		result << uint8(ITEM_OLDMAN_EXCHANGE) << uint16(Success) << OriginItemID << InventoryRanking << uint16(RollItem);
		for (int i = 0; i < RollItem; i++)
		{
			_ITEM_EXCHANGE_CRASH * pExchangeCrashItem = g_pMain->m_ItemExchangeCrashArray.GetData(nExchangeID[i]);
			if (pExchangeCrashItem == nullptr)
				continue;

			int8 pos;
			pos = FindSlotForItem(pExchangeCrashItem->nItemID, pExchangeCrashItem->nCount);
			if (pos < 0)
				continue;

			_ITEM_TABLE* pSendItem = g_pMain->GetItemPtr(pExchangeCrashItem->nItemID);
			if (pSendItem == nullptr)
				continue;

			GiveItem(pExchangeCrashItem->nItemID, uint16(pExchangeCrashItem->nCount), true);
			result << pExchangeCrashItem->nItemID << uint8(pos - SLOT_MAX) << uint16(pExchangeCrashItem->nCount);
		}
		Send(&result);
	}
}

bool CUser::ItemSmashSystemRobItemCheck(uint8 bPos, uint32 nItemID, uint16 sCount)
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