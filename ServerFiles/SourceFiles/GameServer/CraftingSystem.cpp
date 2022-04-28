#include "stdafx.h"


/**
* @brief	Packet handler for the Special exchange system
* 			which is used to exchange Krowaz meterials.
*
* @param	pkt	The packet.
*/
#pragma region
void CUser::SpecialItemExchange(Packet & pkt)
{
	enum ResultOpCodes
	{
		WrongMaterial = 0,
		Success = 1,
		Failed = 2
	};

	uint16 sNpcID;
	uint32 nShadowPiece;
	uint8 nShadowPieceSlot;
	uint8 nMaterialCount;
	uint8 nDownFlag;
	uint8 nItemSlot[ITEMS_SPECIAL_EXCHANGE_GROUP];
	uint32 nItemID[ITEMS_SPECIAL_EXCHANGE_GROUP];
	uint16 nItemCount[ITEMS_SPECIAL_EXCHANGE_GROUP];
	std::vector<uint32> ExchangeIndexList;
	uint32 nItemNumber = 0;
	uint8 sItemSlot = 0;
	bool isShadowPiece = false;
	std::vector <uint32> ExchangingItems;
	ResultOpCodes resultOpCode = WrongMaterial;
	Packet result(WIZ_ITEM_UPGRADE);
	pkt >> sNpcID >> nShadowPiece >> nShadowPieceSlot >> nMaterialCount;

	if (nMaterialCount > 10)
		return;

	CNpc * pNpc = g_pMain->GetNpcPtr(sNpcID, GetZoneID());
	if (pNpc == nullptr
		|| (pNpc->GetType() != NPC_CRAFTSMAN && pNpc->GetType() != NPC_JEWELY)
		|| isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| (UNIXTIME2 - m_iLastExchangeTime < 400)
		|| nShadowPieceSlot >= HAVE_MAX)
	{
		result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
		Send(&result);
		return;
	}

	if (nShadowPiece == ITEM_SHADOW_PIECE)
	{
		_ITEM_DATA  * pItemShadowPiece = GetItem(SLOT_MAX + nShadowPieceSlot);
		_ITEM_TABLE * pTableShadowPiece = g_pMain->GetItemPtr(nShadowPiece);
		if (pItemShadowPiece == nullptr
			|| pTableShadowPiece == nullptr
			|| pItemShadowPiece->nNum != ITEM_SHADOW_PIECE
			|| pItemShadowPiece->sCount < 1)
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		if (pItemShadowPiece->isBound()
			|| pItemShadowPiece->isDuplicate()
			|| pItemShadowPiece->isRented()
			|| pItemShadowPiece->isSealed())
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		if (pNpc->GetType() != NPC_CRAFTSMAN)
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		isShadowPiece = true;
	}

	m_iLastExchangeTime = UNIXTIME2;
	memset(nItemSlot, -1, sizeof(nItemSlot));
	memset(nItemID, 0, sizeof(nItemID));
	memset(nItemCount, 0, sizeof(nItemCount));

	for (int i = 0; i < nMaterialCount; i++)
	{
		pkt >> nItemSlot[i];
		if (nItemSlot[i] > HAVE_MAX)
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}
	}

	pkt >> nDownFlag;

	for (int i = 0; i < nMaterialCount; i++)
	{
		_ITEM_DATA  * pItem = GetItem(SLOT_MAX + nItemSlot[i]);
		if (pItem == nullptr)
			continue;

		int nDigit = 100000000;
		int ItemNumLenght = 9;

		if (pItem->nNum > 999999999)
		{
			nDigit = 1000000000;
			ItemNumLenght = 10;
		}
		else
		{
			nDigit = 100000000;
			ItemNumLenght = 9;
		}

		uint8 nReadByte;
		nItemID[i] = 0;

		for (int x = 0; x < ItemNumLenght; x++)
		{
			pkt >> nReadByte;
			uint8 decimal = nReadByte - 48;
			if (decimal > 9)
			{
				result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
				Send(&result);
				return;
			}

			nItemID[i] += decimal * nDigit;
			nDigit = nDigit / 10;
		}

		uint8 nCount[3] = { 0, 0, 0 };
		pkt >> nCount[0] >> nCount[1] >> nCount[2];
		uint8 decimal1 = nCount[0] - 48;
		uint8 decimal2 = nCount[1] - 48;
		uint8 decimal3 = nCount[2] - 48;
		if (decimal1 > 9 || decimal2 > 9 || decimal3 > 9)
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		uint16 nCountFinish = 0;
		nCountFinish += decimal1 * 100;
		nCountFinish += decimal2 * 10;
		nCountFinish += decimal3 * 1;
		nItemCount[i] = nCountFinish;

		ExchangingItems.push_back(nItemID[i]);
	}

	for (int i = 0; i < nMaterialCount; i++)
	{
		_ITEM_DATA  * pItem = GetItem(SLOT_MAX + nItemSlot[i]);
		_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID[i]);

		if (pItem == nullptr
			|| pTable == nullptr
			|| pTable->m_bRace == 20
			|| pItem->nNum != nItemID[i]
			|| pItem->sCount < nItemCount[i])
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		else if (pItem->isDuplicate()
			|| pItem->isRented())
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}
	}
	if (g_pMain->m_ItemSpecialExchangeArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_ItemSpecialExchangeArray)
		{
			SPECIAL_PART_SEWING_EXCHANGE* pSpecialExchange = itr->second;
			if (pSpecialExchange == nullptr)
				continue;

			uint8 nMatchCount = 0;
			bool bAddArray = false;
			for (int i = 0; i < nMaterialCount; i++)
			{
				if (nItemID[i] != 0)
				{
					for (int x = 0; x < ITEMS_SPECIAL_EXCHANGE_GROUP; x++)
					{
						if (pSpecialExchange->nReqItemID[x] != 0
							&& nItemID[i] == pSpecialExchange->nReqItemID[x])
						{
							nMatchCount++;
							break;
						}
					}
				}
			}
			if (nMaterialCount == nMatchCount)
				bAddArray = true;

			if (bAddArray &&
				std::find(ExchangeIndexList.begin(), ExchangeIndexList.end(), itr->second->nIndex) == ExchangeIndexList.end())
			{
				switch (pNpc->GetType())
				{
				case NPC_JEWELY:
					if (itr->second->sNpcNum == 31402)
						ExchangeIndexList.push_back(itr->second->nIndex);
					break;
				case NPC_CRAFTSMAN:
					if (itr->second->sNpcNum == 19073)
						ExchangeIndexList.push_back(itr->second->nIndex);
					break;
				case 500:
					if (itr->second->sNpcNum == 31510)
						ExchangeIndexList.push_back(itr->second->nIndex);
					break;
				default:
				{
					result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
					Send(&result);
					return;
				}
				break;
				}
			}
		}
	}
	if (ExchangeIndexList.size() > 0)
	{
		uint32 randIndex = myrand(0, (uint32)(ExchangeIndexList.size() - 1));
		uint32 nExchangeID = ExchangeIndexList[randIndex];

		SPECIAL_PART_SEWING_EXCHANGE * pSpecialExchange = g_pMain->m_ItemSpecialExchangeArray.GetData(nExchangeID);
		if (pSpecialExchange == nullptr
			|| !CheckExistSpacialItemAnd(
				pSpecialExchange->nReqItemID[0], pSpecialExchange->nReqItemCount[0],
				pSpecialExchange->nReqItemID[1], pSpecialExchange->nReqItemCount[1],
				pSpecialExchange->nReqItemID[2], pSpecialExchange->nReqItemCount[2],
				pSpecialExchange->nReqItemID[3], pSpecialExchange->nReqItemCount[3],
				pSpecialExchange->nReqItemID[4], pSpecialExchange->nReqItemCount[4],
				pSpecialExchange->nReqItemID[5], pSpecialExchange->nReqItemCount[5],
				pSpecialExchange->nReqItemID[6], pSpecialExchange->nReqItemCount[6],
				pSpecialExchange->nReqItemID[7], pSpecialExchange->nReqItemCount[7],
				pSpecialExchange->nReqItemID[8], pSpecialExchange->nReqItemCount[8],
				pSpecialExchange->nReqItemID[9], pSpecialExchange->nReqItemCount[9]))
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}

		bool bContinueExchange = true;
		for (int i = 0; i < nMaterialCount; i++)
		{
			if (!bContinueExchange)
				break;

			if (nItemID[i] != 0)
			{
				for (int x = 0; x < ITEMS_SPECIAL_EXCHANGE_GROUP; x++)
				{
					if (pSpecialExchange->nReqItemID[x] != 0 &&
						nItemID[i] == pSpecialExchange->nReqItemID[x] &&
						nItemCount[i] != pSpecialExchange->nReqItemCount[x])
					{
						bContinueExchange = false;
						break;
					}
				}
			}
		}

		if (isTrading() 
			|| isMerchanting() 
			|| isSellingMerchant() 
			|| isBuyingMerchant() 
			|| isStoreOpen() 
			|| isMining() 
			|| !isInGame() 
			|| isFishing())
			bContinueExchange = false;

		uint8 MatchingCase = 0;
		uint8 NeedCount = 0;
		for (int x = 0; x < ITEMS_SPECIAL_EXCHANGE_GROUP; x++)
		{
			std::vector<uint32> Used;

			if (pSpecialExchange->nReqItemID[x] > 0)
				NeedCount++;
			else
				continue;

			foreach(itr, ExchangingItems)
			{
				if ((*itr) == pSpecialExchange->nReqItemID[x])
				{
					MatchingCase++;
					*itr = 0;
				}
			}
		}
		if (NeedCount != MatchingCase)
			bContinueExchange = false;

		if (!bContinueExchange)
		{
			result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
			Send(&result);
			return;
		}
		else
		{
			uint32 rand = myrand(0, 10000);
			uint32 upgradeRate = pSpecialExchange->iSuccessRate;

			if (isShadowPiece)
			{
				if (pSpecialExchange->isShadowSucces == 1)
					upgradeRate = 10000;
				else
					upgradeRate += (upgradeRate * 40) / 100;
			}

			if (upgradeRate > 10000)
				upgradeRate = 10000;

			if (upgradeRate < rand)
			{
				resultOpCode = Failed;
				for (int i = 0; i < nMaterialCount; i++)
				{
					_ITEM_DATA  * pDatItemCountable = GetItem(SLOT_MAX + nItemSlot[i]);
					if (pDatItemCountable == nullptr)
					{
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}

					_ITEM_TABLE * pTableItemCountable = g_pMain->GetItemPtr(pDatItemCountable->nNum);
					if (pTableItemCountable == nullptr)
					{
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}

					if (!CraftingRobItem(SLOT_MAX + nItemSlot[i], pTableItemCountable->m_iNum, nItemCount[i]))
					{
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}
				}
				if (isShadowPiece)
					CraftingShadowPieceRobItem(SLOT_MAX + nShadowPieceSlot, ITEM_SHADOW_PIECE, 1);
			}
			else
			{
				uint8 bFreeSlots = 0;
				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (GetItem(i)->nNum == 0 && ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
						break;
				}
				if (bFreeSlots < 2)
				{
					result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
					Send(&result);
					return;
				}

				int SlotForItem = FindSlotForItem(pSpecialExchange->nGiveItemID, 1);
				if (SlotForItem <= 0)
				{
					result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
					Send(&result);
					return;
				}

				sItemSlot = GetEmptySlot() - SLOT_MAX;
				nItemNumber = pSpecialExchange->nGiveItemID;
				uint16 sCount = pSpecialExchange->nGiveItemCount;

				resultOpCode = Success;
				for (int i = 0; i < nMaterialCount; i++)
				{
					_ITEM_DATA  * pDatItemCountable = GetItem(SLOT_MAX + nItemSlot[i]);
					if (pDatItemCountable == nullptr)
					{
						resultOpCode = Failed;
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}

					_ITEM_TABLE * pTableItemCountable = g_pMain->GetItemPtr(pDatItemCountable->nNum);
					if (pTableItemCountable == nullptr)
					{
						resultOpCode = Failed;
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}

					if (!CraftingRobItem(SLOT_MAX + nItemSlot[i], pTableItemCountable->m_iNum, nItemCount[i]))
					{
						resultOpCode = Failed;
						result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode;
						Send(&result);
						return;
					}
				}

				if (isShadowPiece)
					CraftingShadowPieceRobItem(SLOT_MAX + nShadowPieceSlot, ITEM_SHADOW_PIECE, 1);

				GiveItem(nItemNumber, sCount);

				if (pSpecialExchange->isNotice == 1)
				{
					_SERVER_SETTINGS *pSettings = g_pMain->m_ServerSettingsArray.GetData(g_pMain->m_nServerNo);
					if (pSettings != nullptr)
					{
						if (pSettings->DropNotice == Aktive)
						{
							result.clear();
							result.Initialize(WIZ_LOGOSSHOUT);
							result.SByte();
							result << uint8(0x02) << uint8(0x04) << GetName() << pSpecialExchange->nGiveItemID;
							g_pMain->Send_All(&result);
						}
					}
				}
			}
		}
	}

	result.clear();
	result.Initialize(WIZ_ITEM_UPGRADE);
	result << (uint8)SPECIAL_PART_SEWING << (uint8)resultOpCode << sNpcID;

	if (resultOpCode == Success)
		result << nItemNumber << sItemSlot;
	Send(&result);

	if (resultOpCode == Success)
		ShowNpcEffect(31033, true);
	else if (resultOpCode == Failed)
		ShowNpcEffect(31034, true);
}
#pragma endregion

#pragma region CUser::CraftingRobItem(uint8 bPos, uint32 nItemID, uint16 sCount /*= 1*/)
bool CUser::CraftingRobItem(uint8 ItemPos, uint32 nItemID, uint16 sCount /*= 1*/)
{
	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return false;

	if (pTable->m_bCountable == 0)
	{
		if (CraftingRobItem(ItemPos, pTable, sCount))
			return true;
	}
	else
	{
		// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
		for (int i = SLOT_MAX; i < INVENTORY_TOTAL; i++)
		{
			if (CraftingRobItem(i, pTable, sCount))
				return true;
		}
	}
	return false;
}
#pragma endregion

#pragma region CUser::CraftingRobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)
bool CUser::CraftingRobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)
{
	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	if (pTable == nullptr)
		return false;

	_ITEM_DATA *pItem = nullptr;
	pItem = GetItem(bPos);

	if (pItem == nullptr)
		return false;

	if (pItem->nNum != pTable->m_iNum
		|| pItem->sCount < sCount)
		return false;

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

	DateTime time;
	g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=RobItem,ItemName=%s,ItemID=%d,Pos=%d,sCount=%d\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetZoneID(), uint16(GetX()), uint16(GetZ()), GetName().c_str(), pTable->m_sName.c_str(), pTable->m_iNum, bPos, sCount));

	return true;
}
#pragma endregion


bool CUser::CraftingShadowPieceRobItem(uint8 bPos, uint32 nItemID, uint16 sCount)
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

	return true;
}