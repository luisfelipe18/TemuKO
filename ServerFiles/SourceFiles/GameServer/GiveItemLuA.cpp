#include "stdafx.h"

bool CUser::GiveItemLua(
	uint32 nGiveItemID1,
	uint32 nGiveItemCount1,
	uint32 nGiveItemTime1,
	uint32 nGiveItemID2,
	uint32 nGiveItemCount2,
	uint32 nGiveItemTime2,
	uint32 nGiveItemID3,
	uint32 nGiveItemCount3,
	uint32 nGiveItemTime3,
	uint32 nGiveItemID4,
	uint32 nGiveItemCount4,
	uint32 nGiveItemTime4,
	uint32 nGiveItemID5,
	uint32 nGiveItemCount5,
	uint32 nGiveItemTime5,
	uint32 nGiveItemID6,
	uint32 nGiveItemCount6,
	uint32 nGiveItemTime6,
	uint32 nGiveItemID7,
	uint32 nGiveItemCount7,
	uint32 nGiveItemTime7,
	uint32 nGiveItemID8,
	uint32 nGiveItemCount8,
	uint32 nGiveItemTime8,
	uint32 nRobItemID1,
	uint32 nRobItemCount1,
	uint32 nRobItemID2,
	uint32 nRobItemCount2,
	uint32 nRobItemID3,
	uint32 nRobItemCount3,
	uint32 nRobItemID4,
	uint32 nRobItemCount4,
	uint32 nRobItemID5,
	uint32 nRobItemCount5)
{
	//GiveItemLua(UID,0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0, 0,0, 0,0, 0,0, 0,0);
	bool isResults1 = (nGiveItemID1 == 0 && nGiveItemID2 == 0 && nGiveItemID3 == 0 && nGiveItemID4 == 0);
	bool isResults2 = (nGiveItemID5 == 0 && nGiveItemID6 == 0 && nGiveItemID7 == 0 && nGiveItemID8 == 0);
	bool isReuslts3 = (nRobItemID1 == 0 && nRobItemID2 == 0 && nRobItemID3 == 0 && nRobItemID4 == 0 && nRobItemID5 == 0);
	bool isItemAdd = (nGiveItemID1 != 0 || nGiveItemID2 != 0 || nGiveItemID3 != 0 || nGiveItemID4 != 0);
	bool isItemAdd2 = (nGiveItemID5 != 0 || nGiveItemID6 != 0 || nGiveItemID7 != 0 || nGiveItemID8 != 0);

	if (isResults1 && isResults2 && isReuslts3)
		return false;

	if (!CheckExistItemAndLua(nRobItemID1,
		nRobItemCount1, nRobItemID2,
		nRobItemCount2, nRobItemID3,
		nRobItemCount3, nRobItemID4,
		nRobItemCount4, nRobItemID5,
		nRobItemCount5))
		return false;

	struct ItemList
	{
		uint32 nGiveItemID, nGiveItemCount, nGiveItemTime;
	};

	std::vector<ItemList> nGiveItemList;

	_ITEM_TABLE* pnGiveItemID1 = nullptr;
	_ITEM_TABLE* pnGiveItemID2 = nullptr;
	_ITEM_TABLE* pnGiveItemID3 = nullptr;
	_ITEM_TABLE* pnGiveItemID4 = nullptr;
	_ITEM_TABLE* pnGiveItemID5 = nullptr;
	_ITEM_TABLE* pnGiveItemID6 = nullptr;
	_ITEM_TABLE* pnGiveItemID7 = nullptr;
	_ITEM_TABLE* pnGiveItemID8 = nullptr;

	uint32 nReqWeightTotal = 0;
	uint8 nReq›nventorySlot = 0;
	Packet result;
	if (isItemAdd || isItemAdd2)
	{
		if (nGiveItemID1 != 0)
		{
			if (nGiveItemID1 == ITEM_GOLD)
			{
				if (nGiveItemCount1 <= 0)
					nGiveItemCount1 = 0;
			}
			else if (nGiveItemID1 == ITEM_COUNT)
			{
				if (nGiveItemCount1 <= 0)
					nGiveItemCount1 = 0;
			}
			else
			{
				pnGiveItemID1 = g_pMain->GetItemPtr(nGiveItemID1);
				if (pnGiveItemID1 == nullptr)
					return false;

				if (nGiveItemCount1 <= 0)
					nGiveItemCount1 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID1, nGiveItemCount1);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID1->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID1, nGiveItemCount1 });
		}
		if (nGiveItemID2 != 0)
		{
			if (nGiveItemID2 == ITEM_GOLD)
			{
				if (nGiveItemCount2 <= 0)
					nGiveItemCount2 = 0;
			}
			else if (nGiveItemID2 == ITEM_COUNT)
			{
				if (nGiveItemCount2 <= 0)
					nGiveItemCount2 = 0;
			}
			else
			{
				pnGiveItemID2 = g_pMain->GetItemPtr(nGiveItemID2);
				if (pnGiveItemID2 == nullptr)
					return false;

				if (nGiveItemCount2 <= 0)
					nGiveItemCount2 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID2, nGiveItemCount2);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID2->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;

			}
			nGiveItemList.push_back({ nGiveItemID2, nGiveItemCount2 });
		}
		if (nGiveItemID3 != 0)
		{
			if (nGiveItemID3 == ITEM_GOLD)
			{
				if (nGiveItemCount3 <= 0)
					nGiveItemCount3 = 0;
			}
			else if (nGiveItemID3 == ITEM_COUNT)
			{
				if (nGiveItemCount3 <= 0)
					nGiveItemCount3 = 0;
			}
			else
			{
				pnGiveItemID3 = g_pMain->GetItemPtr(nGiveItemID3);
				if (pnGiveItemID3 == nullptr)
					return false;

				if (nGiveItemCount3 <= 0)
					nGiveItemCount3 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID3, nGiveItemCount3);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID3->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID3, nGiveItemCount3 });
		}
		if (nGiveItemID4 != 0)
		{
			if (nGiveItemID4 == ITEM_GOLD)
			{
				if (nGiveItemCount4 <= 0)
					nGiveItemCount4 = 0;
			}
			else if (nGiveItemID4 == ITEM_COUNT)
			{
				if (nGiveItemCount4 <= 0)
					nGiveItemCount4 = 0;
			}
			else
			{
				pnGiveItemID4 = g_pMain->GetItemPtr(nGiveItemID4);
				if (pnGiveItemID4 == nullptr)
					return false;

				if (nGiveItemCount4 <= 0)
					nGiveItemCount4 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID4, nGiveItemCount4);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID4->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID4, nGiveItemCount4 });
		}
		if (nGiveItemID5 != 0)
		{
			if (nGiveItemID5 == ITEM_GOLD)
			{
				if (nGiveItemCount5 <= 0)
					nGiveItemCount5 = 0;
			}
			else if (nGiveItemID5 == ITEM_COUNT)
			{
				if (nGiveItemCount5 <= 0)
					nGiveItemCount5 = 0;
			}
			else
			{
				pnGiveItemID5 = g_pMain->GetItemPtr(nGiveItemID5);
				if (pnGiveItemID5 == nullptr)
					return false;

				if (nGiveItemCount5 <= 0)
					nGiveItemCount5 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID5, nGiveItemCount5);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID5->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID5, nGiveItemCount5 });
		}
		if (nGiveItemID6 != 0)
		{
			if (nGiveItemID6 == ITEM_GOLD)
			{
				if (nGiveItemCount6 <= 0)
					nGiveItemCount6 = 0;
			}
			else if (nGiveItemID6 == ITEM_COUNT)
			{
				if (nGiveItemCount6 <= 0)
					nGiveItemCount6 = 0;
			}
			else
			{
				pnGiveItemID6 = g_pMain->GetItemPtr(nGiveItemID6);
				if (pnGiveItemID6 == nullptr)
					return false;

				if (nGiveItemCount6 <= 0)
					nGiveItemCount6 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID6, nGiveItemCount6);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID6->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID6, nGiveItemCount6 });
		}
		if (nGiveItemID7 != 0)
		{
			if (nGiveItemID7 == ITEM_GOLD)
			{
				if (nGiveItemCount7 <= 0)
					nGiveItemCount7 = 0;
			}
			else if (nGiveItemID7 == ITEM_COUNT)
			{
				if (nGiveItemCount7 <= 0)
					nGiveItemCount7 = 0;
			}
			else
			{
				pnGiveItemID7 = g_pMain->GetItemPtr(nGiveItemID7);
				if (pnGiveItemID7 == nullptr)
					return false;

				if (nGiveItemCount7 <= 0)
					nGiveItemCount7 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID7, nGiveItemCount7);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID7->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID7, nGiveItemCount7 });
		}
		if (nGiveItemID8 != 0)
		{
			if (nGiveItemID8 == ITEM_GOLD)
			{
				if (nGiveItemCount8 <= 0)
					nGiveItemCount8 = 0;
			}
			else if (nGiveItemID8 == ITEM_COUNT)
			{
				if (nGiveItemCount8 <= 0)
					nGiveItemCount8 = 0;
			}
			else
			{
				pnGiveItemID8 = g_pMain->GetItemPtr(nGiveItemID8);
				if (pnGiveItemID8 == nullptr)
					return false;

				if (nGiveItemCount8 <= 0)
					nGiveItemCount8 = 1;

				int8 nGivePos;
				nGivePos = FindSlotForItem(nGiveItemID8, nGiveItemCount8);
				if (nGivePos < 0)
					return false;

				nReqWeightTotal += pnGiveItemID8->m_sWeight;
				if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
					return false;

				nReq›nventorySlot++;
			}
			nGiveItemList.push_back({ nGiveItemID8, nGiveItemCount8 });
		}
	}

	if (nGiveItemID1 != ITEM_GOLD && nGiveItemID1 != ITEM_COUNT)
	{
		if (pnGiveItemID1 == nullptr && nGiveItemID1 != 0)
			return false;
	}
	if (nGiveItemID2 != ITEM_GOLD && nGiveItemID2 != ITEM_COUNT)
	{
		if (pnGiveItemID2 == nullptr && nGiveItemID2 != 0)
			return false;
	}
	if (nGiveItemID3 != ITEM_GOLD && nGiveItemID3 != ITEM_COUNT)
	{
		if (pnGiveItemID3 == nullptr && nGiveItemID3 != 0)
			return false;
	}
	if (nGiveItemID4 != ITEM_GOLD && nGiveItemID4 != ITEM_COUNT)
	{
		if (pnGiveItemID4 == nullptr && nGiveItemID4 != 0)
			return false;
	}
	if (nGiveItemID5 != ITEM_GOLD && nGiveItemID5 != ITEM_COUNT)
	{
		if (pnGiveItemID5 == nullptr && nGiveItemID5 != 0)
			return false;
	}
	if (nGiveItemID6 != ITEM_GOLD && nGiveItemID6 != ITEM_COUNT)
	{
		if (pnGiveItemID6 == nullptr && nGiveItemID6 != 0)
			return false;
	}
	if (nGiveItemID7 != ITEM_GOLD && nGiveItemID7 != ITEM_COUNT)
	{
		if (pnGiveItemID7 == nullptr && nGiveItemID7 != 0)
			return false;
	}
	if (nGiveItemID8 != ITEM_GOLD && nGiveItemID8 != ITEM_COUNT)
	{
		if (pnGiveItemID8 == nullptr && nGiveItemID8 != 0)
			return false;
	}

	if (m_sItemWeight + nReqWeightTotal > m_sMaxWeight)
		return false;

	if (!CheckGiveSlot(nReq›nventorySlot))
		return false;

	if (!RobItemLua(nRobItemID1, nRobItemCount1,
		nRobItemID2, nRobItemCount2, nRobItemID3,
		nRobItemCount3, nRobItemID4, nRobItemCount4,
		nRobItemID5, nRobItemCount5))
		return false;


	if (nGiveItemList.size() > 0)
	{
		foreach(itr, nGiveItemList)
		{
			if (itr->nGiveItemID == ITEM_GOLD)
			{
				if (m_iGold + itr->nGiveItemCount > COIN_MAX)
					m_iGold = COIN_MAX;
				else
					m_iGold += itr->nGiveItemCount;

				result.clear();
				result.Initialize(WIZ_GOLD_CHANGE);
				result << uint8(CoinGain) << itr->nGiveItemCount << GetCoins();
				Send(&result);
			}
			else if (itr->nGiveItemID == ITEM_COUNT)
			{
				if (m_iLoyalty + itr->nGiveItemCount > LOYALTY_MAX)
					m_iLoyalty = LOYALTY_MAX;
				else
					m_iLoyalty += itr->nGiveItemCount;

				result.clear();
				result.Initialize(WIZ_LOYALTY_CHANGE);
				result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
					<< uint32(0)
					<< uint32(0);
				Send(&result);
			}
			else
			{
				int8 nGivePos;
				nGivePos = FindSlotForItem(itr->nGiveItemID, itr->nGiveItemCount);
				if (nGivePos < 0)
					return false;

				bool bNewItem = true;
				_ITEM_DATA *pnGiveItemData1 = GetItem(nGivePos);
				if (pnGiveItemData1 == nullptr || pnGiveItemData1->nNum != 0)
					bNewItem = false;

				if (bNewItem)
					pnGiveItemData1->nSerialNum = g_pMain->GenerateItemSerial();

				pnGiveItemData1->nNum = itr->nGiveItemID;
				pnGiveItemData1->sCount += itr->nGiveItemCount;
				pnGiveItemData1->sDuration = pnGiveItemID1->m_sDuration;

				if (pnGiveItemData1->sCount > MAX_ITEM_COUNT)
					pnGiveItemData1->sCount = MAX_ITEM_COUNT;

				if (itr->nGiveItemTime != 0)
					pnGiveItemData1->nExpirationTime = int32(UNIXTIME) + ((60 * 60 * 24) * itr->nGiveItemTime);
				else
					pnGiveItemData1->nExpirationTime = 0;

				// This is really silly, but match the count up with the duration
				// for this special items that behave this way.
				if (pnGiveItemID1->m_bKind == 255)
					pnGiveItemData1->sCount = pnGiveItemData1->sDuration;

				SendStackChange(itr->nGiveItemID, m_sItemArray[nGivePos].sCount, m_sItemArray[nGivePos].sDuration, nGivePos - SLOT_MAX, true, pnGiveItemData1->nExpirationTime);
			}
		}
	}
	return true;
}

bool CUser::CheckExistItemLua(uint32 itemid, uint32 count)
{
	if (itemid == ITEM_GOLD || itemid == ITEM_COUNT)
		return true;

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (m_sItemArray[i].nNum == itemid
			&& m_sItemArray[i].sCount >= count)
			return true;
	}

	return false;
}

bool CUser::CheckExistItemAndLua(uint32 nItemID1, uint32 sCount1, uint32 nItemID2, uint32 sCount2, uint32 nItemID3, uint32 sCount3, uint32 nItemID4, uint32 sCount4, uint32 nItemID5, uint32 sCount5)
{
	if (nItemID1 != 0
		&& !CheckExistItemLua(nItemID1, sCount1))
		return false;

	if (nItemID2 != 0
		&& !CheckExistItemLua(nItemID2, sCount2))
		return false;

	if (nItemID3 != 0
		&& !CheckExistItemLua(nItemID3, sCount3))
		return false;

	if (nItemID4 != 0
		&& !CheckExistItemLua(nItemID4, sCount4))
		return false;

	if (nItemID5 != 0
		&& !CheckExistItemLua(nItemID5, sCount5))
		return false;

	return true;
}

#pragma region CUser::RobItem(uint32 nItemID, uint16 sCount /*= 1*/)
bool CUser::RobItemLua(uint32 nItemID1, uint32 sCount1, uint32 nItemID2, uint32 sCount2, uint32 nItemID3, uint32 sCount3, uint32 nItemID4, uint32 sCount4, uint32 nItemID5, uint32 sCount5)
{
	bool RobItemTrue1 = false, RobItemTrue2 = false, RobItemTrue3 = false, RobItemTrue4 = false, RobItemTrue5 = false;
	int Pos1 = -1, Pos2 = -1, Pos3 = -1, Pos4 = -1, Pos5 = -1;
	bool xPos1 = false, xPos2 = false, xPos3 = false, xPos4 = false, xPos5 = false;
	uint32 ReqTotalLoyalty = 0, ReqTotalCoins = 0;

	_ITEM_TABLE * pTable1 = nullptr;
	_ITEM_TABLE * pTable2 = nullptr;
	_ITEM_TABLE * pTable3 = nullptr;
	_ITEM_TABLE * pTable4 = nullptr;
	_ITEM_TABLE * pTable5 = nullptr;
	Packet result;


	bool isResults = (nItemID1 != 0 || nItemID2 != 0 || nItemID3 != 0 || nItemID4 != 0 || nItemID5 != 0);
	if (isResults)
	{
		if (nItemID1 != 0)
		{
			if (nItemID1 == ITEM_GOLD || nItemID1 == ITEM_COUNT)
			{
				if (nItemID1 == ITEM_GOLD)
				{
					if (!hasCoins(sCount1))
						return false;

					ReqTotalCoins += sCount1;
					if (!hasCoins(ReqTotalCoins))
						return false;
				}
				else if (nItemID1 == ITEM_COUNT)
				{
					if (!hasLoyalty(sCount1))
						return false;

					ReqTotalLoyalty += sCount1;
					if (!hasLoyalty(ReqTotalLoyalty))
						return false;
				}
				RobItemTrue1 = true;
			}
			else
			{
				// Allow unused exchanges.
				if (sCount1 == 0)
					return false;

				pTable1 = g_pMain->GetItemPtr(nItemID1);
				if (pTable1 == nullptr)
					return false;

				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (RobItemLua(i, pTable1, sCount1))
					{
						RobItemTrue1 = true;
						Pos1 = i;
						break;
					}
				}
			}
		}
		if (nItemID2 != 0)
		{
			if (nItemID2 == ITEM_GOLD || nItemID2 == ITEM_COUNT)
			{
				if (nItemID2 == ITEM_GOLD)
				{
					if (!hasCoins(sCount2))
						return false;

					ReqTotalCoins += sCount2;
					if (!hasCoins(ReqTotalCoins))
						return false;
				}
				else if (nItemID2 == ITEM_COUNT)
				{
					if (!hasLoyalty(sCount2))
						return false;

					ReqTotalLoyalty += sCount2;
					if (!hasLoyalty(ReqTotalLoyalty))
						return false;
				}
				RobItemTrue2 = true;
			}
			else
			{
				// Allow unused exchanges.
				if (sCount2 == 0)
					return false;

				pTable2 = g_pMain->GetItemPtr(nItemID2);
				if (pTable2 == nullptr)
					return false;

				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (RobItemLua(i, pTable2, sCount2))
					{
						RobItemTrue2 = true;
						Pos2 = i;
						break;
					}
				}
			}
		}
		if (nItemID3 != 0)
		{
			if (nItemID3 == ITEM_GOLD || nItemID3 == ITEM_COUNT)
			{
				if (nItemID3 == ITEM_GOLD)
				{
					if (!hasCoins(sCount3))
						return false;

					ReqTotalCoins += sCount3;
					if (!hasCoins(ReqTotalCoins))
						return false;
				}
				else if (nItemID3 == ITEM_COUNT)
				{
					if (!hasLoyalty(sCount3))
						return false;

					ReqTotalLoyalty += sCount3;
					if (!hasLoyalty(ReqTotalLoyalty))
						return false;
				}
				RobItemTrue3 = true;
			}
			else
			{
				// Allow unused exchanges.
				if (sCount3 == 0)
					return false;

				pTable3 = g_pMain->GetItemPtr(nItemID3);
				if (pTable3 == nullptr)
					return false;

				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (RobItemLua(i, pTable3, sCount3))
					{
						RobItemTrue3 = true;
						Pos3 = i;
						break;
					}
				}
			}
		}

		if (nItemID4 != 0)
		{
			if (nItemID4 == ITEM_GOLD || nItemID4 == ITEM_COUNT)
			{
				if (nItemID4 == ITEM_GOLD)
				{
					if (!hasCoins(sCount4))
						return false;

					ReqTotalCoins += sCount4;
					if (!hasCoins(ReqTotalCoins))
						return false;
				}
				else if (nItemID4 == ITEM_COUNT)
				{
					if (!hasLoyalty(sCount4))
						return false;

					ReqTotalLoyalty += sCount4;
					if (!hasLoyalty(ReqTotalLoyalty))
						return false;
				}
				RobItemTrue4 = true;
			}
			else
			{
				// Allow unused exchanges.
				if (sCount4 == 0)
					return false;

				pTable4 = g_pMain->GetItemPtr(nItemID4);
				if (pTable4 == nullptr)
					return false;

				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (RobItemLua(i, pTable4, sCount4))
					{
						RobItemTrue4 = true;
						Pos4 = i;
						break;
					}
				}
			}
		}

		if (nItemID5 != 0)
		{
			if (nItemID5 == ITEM_GOLD || nItemID5 == ITEM_COUNT)
			{
				if (nItemID5 == ITEM_GOLD)
				{
					if (!hasCoins(sCount5))
						return false;

					ReqTotalCoins += sCount5;
					if (!hasCoins(ReqTotalCoins))
						return false;
				}
				else if (nItemID5 == ITEM_COUNT)
				{
					if (!hasLoyalty(sCount5))
						return false;

					ReqTotalLoyalty += sCount5;
					if (!hasLoyalty(ReqTotalLoyalty))
						return false;
				}
				RobItemTrue5 = true;
			}
			else
			{
				// Allow unused exchanges.
				if (sCount5 == 0)
					return false;

				pTable5 = g_pMain->GetItemPtr(nItemID5);
				if (pTable5 == nullptr)
					return false;

				for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
				{
					if (RobItemLua(i, pTable5, sCount5))
					{
						RobItemTrue5 = true;
						Pos5 = i;
						break;
					}
				}
			}
		}
	}
	if (RobItemTrue1 && nItemID1 == 0)
		return false;

	if (RobItemTrue2 && nItemID2 == 0)
		return false;

	if (RobItemTrue3 && nItemID3 == 0)
		return false;

	if (RobItemTrue4 && nItemID4 == 0)
		return false;

	if (RobItemTrue5 && nItemID5 == 0)
		return false;


	if (RobItemTrue1)
	{
		if (nItemID1 != ITEM_GOLD && nItemID1 != ITEM_COUNT)
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos1);
			if (pItem == nullptr)
				return false;
		}
	}

	if (RobItemTrue2)
	{
		if (nItemID2 != ITEM_GOLD && nItemID2 != ITEM_COUNT)
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos2);
			if (pItem == nullptr)
				return false;
		}
	}

	if (RobItemTrue3)
	{
		if (nItemID3 != ITEM_GOLD && nItemID3 != ITEM_COUNT)
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos3);
			if (pItem == nullptr)
				return false;
		}
	}

	if (RobItemTrue4)
	{
		if (nItemID4 != ITEM_GOLD && nItemID4 != ITEM_COUNT)
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos4);
			if (pItem == nullptr)
				return false;
		}
	}

	if (RobItemTrue5)
	{
		if (nItemID5 != ITEM_GOLD && nItemID5 != ITEM_COUNT)
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos5);
			if (pItem == nullptr)
				return false;
		}
	}

	if (RobItemTrue1)
	{
		if (nItemID1 == ITEM_GOLD)
		{
			m_iGold -= sCount1;

			if (m_iGold <= 0)
				m_iGold = 0;

			result.clear();
			result.Initialize(WIZ_GOLD_CHANGE);
			result << uint8(CoinLoss) << sCount1 << GetCoins();
			Send(&result);
		}
		else if (nItemID1 == ITEM_COUNT)
		{
			m_iLoyalty -= sCount1;
			if (m_iLoyalty <= 0)
				m_iLoyalty = 0;

			result.clear();
			result.Initialize(WIZ_LOYALTY_CHANGE);
			result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
				<< uint32(0)
				<< uint32(0);
			Send(&result);
		}
		else
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos1);
			if (pItem == nullptr)
				return false;

			bool bIsConsumableScroll = (pTable1->m_bKind == 255);

			if (bIsConsumableScroll)
			{
				if ((int16)sCount1 > pItem->sDuration)
					pItem->sDuration = 0;
				else
					pItem->sDuration -= (int16)sCount1;
			}
			else
			{
				if (sCount1 > pItem->sCount)
					pItem->sCount = 0;
				else
					pItem->sCount -= sCount1;
			}

			if (pItem->sCount <= 0 || (bIsConsumableScroll && pItem->sDuration <= 0))
				memset(pItem, 0, sizeof(_ITEM_DATA));

			SendStackChange(pTable1->m_iNum, pItem->sCount, pItem->sDuration, Pos1 - SLOT_MAX, false, pItem->nExpirationTime);
		}
	}
	if (RobItemTrue2)
	{
		if (nItemID2 == ITEM_GOLD)
		{
			m_iGold -= sCount2;
			if (m_iGold <= 0)
				m_iGold = 0;

			result.clear();
			result.Initialize(WIZ_GOLD_CHANGE);
			result << uint8(CoinLoss) << sCount2 << GetCoins();
			Send(&result);
		}
		else if (nItemID2 == ITEM_COUNT)
		{
			m_iLoyalty -= sCount2;
			if (m_iLoyalty <= 0)
				m_iLoyalty = 0;

			result.clear();
			result.Initialize(WIZ_LOYALTY_CHANGE);
			result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
				<< uint32(0)
				<< uint32(0);
			Send(&result);
		}
		else
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos2);
			if (pItem == nullptr)
				return false;

			bool bIsConsumableScroll = (pTable2->m_bKind == 255);

			if (bIsConsumableScroll)
			{
				if ((int16)sCount2 > pItem->sDuration)
					pItem->sDuration = 0;
				else
					pItem->sDuration -= (int16)sCount2;
			}
			else
			{
				if (sCount2 > pItem->sCount)
					pItem->sCount = 0;
				else
					pItem->sCount -= sCount2;
			}

			if (pItem->sCount <= 0 || (bIsConsumableScroll && pItem->sDuration <= 0))
				memset(pItem, 0, sizeof(_ITEM_DATA));

			SendStackChange(pTable2->m_iNum, pItem->sCount, pItem->sDuration, Pos2 - SLOT_MAX, false, pItem->nExpirationTime);
		}
	}
	if (RobItemTrue3)
	{
		if (nItemID3 == ITEM_GOLD)
		{
			m_iGold -= sCount3;
			if (m_iGold <= 0)
				m_iGold = 0;

			result.clear();
			result.Initialize(WIZ_GOLD_CHANGE);
			result << uint8(CoinLoss) << sCount3 << GetCoins();
			Send(&result);
		}
		else if (nItemID3 == ITEM_COUNT)
		{
			m_iLoyalty -= sCount3;
			if (m_iLoyalty <= 0)
				m_iLoyalty = 0;

			result.clear();
			result.Initialize(WIZ_LOYALTY_CHANGE);
			result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
				<< uint32(0)
				<< uint32(0);
			Send(&result);
		}
		else
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos3);
			if (pItem == nullptr)
				return false;

			bool bIsConsumableScroll = (pTable3->m_bKind == 255);

			if (bIsConsumableScroll)
			{
				if ((int16)sCount3 > pItem->sDuration)
					pItem->sDuration = 0;
				else
					pItem->sDuration -= (int16)sCount3;
			}
			else
			{
				if (sCount3 > pItem->sCount)
					pItem->sCount = 0;
				else
					pItem->sCount -= sCount3;
			}

			if (pItem->sCount <= 0 || (bIsConsumableScroll && pItem->sDuration <= 0))
				memset(pItem, 0, sizeof(_ITEM_DATA));

			SendStackChange(pTable3->m_iNum, pItem->sCount, pItem->sDuration, Pos3 - SLOT_MAX, false, pItem->nExpirationTime);
		}
	}

	if (RobItemTrue4)
	{
		if (nItemID4 == ITEM_GOLD)
		{
			m_iGold -= sCount4;
			if (m_iGold <= 0)
				m_iGold = 0;

			result.clear();
			result.Initialize(WIZ_GOLD_CHANGE);
			result << uint8(CoinLoss) << sCount4 << GetCoins();
			Send(&result);
		}
		else if (nItemID4 == ITEM_COUNT)
		{
			m_iLoyalty -= sCount4;
			if (m_iLoyalty <= 0)
				m_iLoyalty = 0;

			result.clear();
			result.Initialize(WIZ_LOYALTY_CHANGE);
			result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
				<< uint32(0)
				<< uint32(0);
			Send(&result);
		}
		else
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos4);
			if (pItem == nullptr)
				return false;

			bool bIsConsumableScroll = (pTable4->m_bKind == 255);

			if (bIsConsumableScroll)
			{
				if ((int16)sCount4 > pItem->sDuration)
					pItem->sDuration = 0;
				else
					pItem->sDuration -= (int16)sCount4;
			}
			else
			{
				if (sCount4 > pItem->sCount)
					pItem->sCount = 0;
				else
					pItem->sCount -= sCount4;
			}

			if (pItem->sCount <= 0 || (bIsConsumableScroll && pItem->sDuration <= 0))
				memset(pItem, 0, sizeof(_ITEM_DATA));

			SendStackChange(pTable4->m_iNum, pItem->sCount, pItem->sDuration, Pos4 - SLOT_MAX, false, pItem->nExpirationTime);
		}
	}

	if (RobItemTrue5)
	{
		if (nItemID5 == ITEM_GOLD)
		{
			m_iGold -= sCount5;
			if (m_iGold <= 0)
				m_iGold = 0;

			result.clear();
			result.Initialize(WIZ_GOLD_CHANGE);
			result << uint8(CoinLoss) << sCount5 << GetCoins();
			Send(&result);
		}
		else if (nItemID5 == ITEM_COUNT)
		{
			m_iLoyalty -= sCount5;
			if (m_iLoyalty <= 0)
				m_iLoyalty = 0;

			result.clear();
			result.Initialize(WIZ_LOYALTY_CHANGE);
			result << uint8(LOYALTY_NATIONAL_POINTS) << m_iLoyalty << m_iLoyaltyMonthly
				<< uint32(0)
				<< uint32(0);
			Send(&result);
		}
		else
		{
			_ITEM_DATA *pItem = nullptr;
			pItem = GetItem(Pos5);
			if (pItem == nullptr)
				return false;

			bool bIsConsumableScroll = (pTable5->m_bKind == 255);

			if (bIsConsumableScroll)
			{
				if ((int16)sCount5 > pItem->sDuration)
					pItem->sDuration = 0;
				else
					pItem->sDuration -= (int16)sCount5;
			}
			else
			{
				if (sCount5 > pItem->sCount)
					pItem->sCount = 0;
				else
					pItem->sCount -= sCount5;
			}

			SendStackChange(pTable5->m_iNum, pItem->sCount, pItem->sDuration, Pos5 - SLOT_MAX, false, pItem->nExpirationTime);
		}
	}

	return true;
}
#pragma endregion

#pragma region CUser::RobItemLua(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)
bool CUser::RobItemLua(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/)
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

	return true;
}
#pragma endregion