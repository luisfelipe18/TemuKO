#include "stdafx.h"

#pragma region CUser::HandleMiningSystem(Packet & pkt)
/**
* @brief	Handles packets related to the mining system.
* 			Also handles soccer-related packets (yuck).
*
* @param	pkt	The packet.
*/
void CUser::HandleMiningSystem(Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
	case MiningStart:
		HandleMiningStart(pkt);
		break;

	case MiningAttempt:
		HandleMiningAttempt(pkt);
		break;

	case MiningStop:
		HandleMiningStop(pkt);
		break;

	case BettingGame:
		HandleBettingGame(pkt);
		break;

	case FishingStart:
		HandleFishingStart(pkt);
		break;

	case FishingAttempt:
		HandleFishingAttempt(pkt);
		break;

	case FishingStop:
		HandleFishingStop(pkt);
		break;

	case MiningSoccer:
		HandleSoccer(pkt);
		break;

	default:
		TRACE("[SID=%d] Unknown packet %X\n", GetSocketID(), opcode);
		printf("[SID=%d] Unknown packet %X\n", GetSocketID(), opcode);
		return;
	}
}
#pragma endregion

#pragma region CUser::HandleBettingGame(Packet & pkt)
/**
* @brief	Handles users Betting Game for 10 k coins.
* @param	pkt	The packet.
*/
void CUser::HandleBettingGame(Packet & pkt)
{
	if (isDead() || !isInGame())
		return;

	enum ResultMessages
	{
		Won = 1, // %s won 10000
		EndinginaTie = 2, // Received 5000 Noah for ending in a tie
		Lose = 3, // %s lost Noah
		EnougCoins = 4, // %s You don't Enough coins.
	};

	Packet result(WIZ_MINING, uint8(BettingGame));
	ResultMessages resultsopcode = EnougCoins;

	if (!hasCoins(5000))
	{
		resultsopcode = EnougCoins;
		goto fail_return;
	}

	GoldLose(5000);

	uint8 PlayerRand = myrand(1, 5);
	uint8 NpcRand = myrand(1, 5);

	if (PlayerRand > NpcRand)
		resultsopcode = Won;
	else if (PlayerRand < NpcRand)
		resultsopcode = Lose;
	else if (PlayerRand == NpcRand)
		resultsopcode = EndinginaTie;

	if (resultsopcode == Won)
		GoldGain(10000);

fail_return:
	result << uint16(resultsopcode) << uint16(0) << uint8(0) << uint8(0) << uint8(PlayerRand) << uint8(NpcRand) << uint16(0);
	SendToRegion(&result);
}
#pragma endregion

#pragma region CUser::HandleMiningStart(Packet & pkt)

/**
* @brief	Handles users requesting to start mining.
* 			NOTE: This is a mock-up, so be warned that it does not
* 			handle checks such as identifying if the user is allowed
* 			to mine in this area.
*
* @param	pkt	The packet.
*/
void CUser::HandleMiningStart(Packet & pkt)
{
	if (isBlinking()
		|| isTrading()
		|| isMerchanting()
		|| isFishing()
		|| isDead()
		|| !isInGame()
		|| isBuyingMerchantingPreparing()
		|| isSellingMerchantingPreparing())
		return;

	Packet result(WIZ_MINING, uint8(MiningStart));
	uint16 resultCode = MiningResultSuccess;

	// Are we mining already?
	if (isMining())
		resultCode = MiningResultPreparing;

	// Do we have a pickaxe? Is it worn?
	_ITEM_DATA* pItem;
	_ITEM_TABLE* pTable = GetItemPrototype(RIGHTHAND, pItem);
	if (pItem == nullptr || pTable == nullptr
		|| pItem->sDuration <= 0
		|| !pTable->isPickaxe())
		resultCode = MiningResultNotPickaxe;

	result << resultCode;

	// If nothing went wrong, allow the user to start mining.
	// Be sure to let everyone know we're mining.
	if (resultCode == MiningResultSuccess)
	{
		_ITEM_TABLE* pItems = GetItemPrototype(CFAIRY);
		if (pItems != nullptr)
		{
			if (pItems->GetNum() == OFFLINE_MINING_ITEM)
			{
				if (GetNation() == KARUS)
					StateChangeServerDirect(11, uint32(1));
				else if (GetNation() == ELMORAD)
					StateChangeServerDirect(11, uint32(2));

				StateChangeServerDirect(7, 2);
				m_bOfflineSystemSocketType = true;
				m_bOfflineSystemType = true;
				DateTime time;
				g_pMain->WriteOfflineSystemLog(string_format("[Mining System Start - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
			}
		}

		m_bMining = true;
		m_bResHpType = USER_MINING;
		result << GetID();

		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	else
	{
		Send(&result);
	}
}
#pragma endregion 

#pragma region CUser::HandleMiningAttempt(Packet & pkt)
/**
* @brief	Handles a user's mining attempt by finding a random reward (or none at all).
* 			This is sent automatically by the client every MINING_DELAY (5) seconds.
*
* @param	pkt	The packet.
*/
void CUser::HandleMiningAttempt(Packet & pkt)
{
	if (!isMining())
		return;

	// Check to make sure we're not spamming the packet...
	if ((UNIXTIME - m_tLastMiningAttempt) < MINING_DELAY)
		return;


	Packet result(WIZ_MINING, uint8(MiningAttempt));
	uint16 resultCode = MiningResultSuccess;

	// Do we have a pickaxe? Is it worn?
	_ITEM_DATA * pItem;
	_MINING_FISHING_ITEM* pMining = nullptr;
	std::vector<uint32> pMiningList;
	_ITEM_TABLE * pTable = GetItemPrototype(RIGHTHAND, pItem);

	if (pItem == nullptr 
		|| pTable == nullptr
		|| pItem->sDuration <= 0 // are we supposed to wear the pickaxe on use? Need to verify.
		|| !pTable->isPickaxe())
		resultCode = MiningResultNotPickaxe;

	// Effect to show to clients
	uint16 sEffect = 0;

	// This is just a mock-up based on another codebase's implementation.
	// Need to log official data to get a proper idea of how it behaves, rate-wise,
	// so that we can then implement it more dynamically.
	if (resultCode == MiningResultSuccess && g_pMain->m_MiningFishingItemArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_MiningFishingItemArray)
		{
			pMining = itr->second;

			if (pMining == nullptr)
				continue;

			if (pMining->nTableType != 0)
				continue;

			if (pTable->m_iNum == GOLDEN_MATTOCK)
			{
				if (pMining->UseItemType == 1)
				{
					if (!g_pMain->isWarOpen())
					{
						if (pMining->nWarStatus == MiningResultError)
							pMiningList.push_back(pMining->nIndex);
						else
							continue;
					}
					else
					{
						if (GetZoneID() == ZONE_KARUS)
						{
							if (GetWarVictory() == ELMORAD && GetNation() == ELMORAD)
							{
								if (pMining->nWarStatus == 2)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == ELMORAD && GetNation() == KARUS)
							{
								if (pMining->nWarStatus == 1)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else
							{
								if (pMining->nWarStatus == 0)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() == ZONE_ELMORAD)
						{
							if (GetWarVictory() == KARUS && GetNation() == KARUS)
							{
								if (pMining->nWarStatus == 2)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == KARUS && GetNation() == ELMORAD)
							{
								if (pMining->nWarStatus == 1)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else
							{
								if (pMining->nWarStatus == 0)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5)
						{
							if (pMining->nWarStatus == 0)
								pMiningList.push_back(pMining->nIndex);
							else
								continue;
						}
					}
				}
			}
			else
			{
				if (pMining->UseItemType == 0)
				{
					if (!g_pMain->isWarOpen())
					{
						if (pMining->nWarStatus == MiningResultError)
							pMiningList.push_back(pMining->nIndex);
						else
							continue;
					}
					else
					{
						if (GetZoneID() == ZONE_KARUS)
						{
							if (GetWarVictory() == ELMORAD && GetNation() == ELMORAD)
							{
								if (pMining->nWarStatus == 2)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == ELMORAD && GetNation() == KARUS)
							{
								if (pMining->nWarStatus == 1)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else
							{
								if (pMining->nWarStatus == 0)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() == ZONE_ELMORAD)
						{
							if (GetWarVictory() == KARUS && GetNation() == KARUS)
							{
								if (pMining->nWarStatus == 2)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == KARUS && GetNation() == ELMORAD)
							{
								if (pMining->nWarStatus == 1)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
							else
							{
								if (pMining->nWarStatus == 0)
									pMiningList.push_back(pMining->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5)
						{
							if (pMining->nWarStatus == 0)
								pMiningList.push_back(pMining->nIndex);
							else
								continue;
						}
					}
				}
			}
		}

		if (pMiningList.size() > 0)
		{
			int offset = 0;
			uint32 bRandArray[10000];
			memset(bRandArray, 0, sizeof(bRandArray));
			uint8 sItemSlot = 0;
			foreach(itr, pMiningList)
			{
				pMining = g_pMain->m_MiningFishingItemArray.GetData(*itr);

				if (pMining == nullptr)
					return;

				if (offset >= 10000)
					break;

				for (int i = 0; i < int(pMining->SuccessRate / 5); i++)
				{
					if (i + offset >= 10000)
						break;

					bRandArray[offset + i] = pMining->nGiveItemID;
				}

				offset += int(pMining->SuccessRate / 5);
			}

			uint32 bRandSlot = myrand(0, offset);
			uint32 nItemID = bRandArray[bRandSlot];

			if (nItemID == ITEM_EXP)
			{
				sEffect = 13082; // "XP" effect
				ExpChange(GetLevel() * myrand(0, 40) * 2);
			}
			else if (nItemID > 0)
			{
				sEffect = 13081; // "Item" effect
				GiveItem(nItemID);
			}
		}
	}

	result << resultCode;

	ItemWoreOut(ATTACK, 100);

	if (resultCode != MiningResultSuccess
		&& resultCode != MiningResultNothingFound)
	{
		// Tell us the error first
		Send(&result);

		// and then tell the client to stop mining
		HandleMiningStop(pkt);
		return;
	}

	result << GetID() << sEffect;

	if (resultCode != MiningResultNothingFound)
	{
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	else if(resultCode == MiningResultNothingFound)
		Send(&result);

	m_tLastMiningAttempt = UNIXTIME;
}
#pragma endregion 

#pragma region CUser::HandleMiningStop(Packet & pkt)
/**
* @brief	Handles when a user stops mining.
*
* @param	pkt	The packet.
*/
void CUser::HandleMiningStop(Packet& pkt)
{
	if (!isMining())
		return;

	if (isOfflineSystem())
	{
		if (GetNation() == KARUS)
			StateChangeServerDirect(11, uint32(0));
		else if (GetNation() == ELMORAD)
			StateChangeServerDirect(11, uint32(0));

		StateChangeServerDirect(7, 0);
		m_bOfflineSystemSocketType = false;
		m_bOfflineSystemType = false;
		DateTime time;
		g_pMain->WriteOfflineSystemLog(string_format("[Mining System Closed - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
	}

	m_bResHpType = USER_STANDING;
	Packet result(WIZ_MINING, uint8(MiningStop));
	result << uint16(1) << GetID();
	m_bMining = false;
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, nullptr, GetEventRoom());
	else
		SendToRegion(&result);

	result.clear();
	result.SetOpcode(WIZ_MINING);
	result << uint8(MiningStop) << uint16(2);
	Send(&result);
}
#pragma endregion 

#pragma region CUser::HandleFishingStart(Packet & pkt)
/**
* @brief	Handles users requesting to start fishing.
* 			NOTE: This is a mock-up, so be warned that it does not
* 			handle checks such as identifying if the user is allowed
* 			to fish in this area.
*
* @param	pkt	The packet.
*/
void CUser::HandleFishingStart(Packet & pkt)
{
	if (isBlinking()
		|| isTrading()
		|| isMerchanting()
		|| isMining()
		|| isDead()
		|| !isInGame()
		|| isBuyingMerchantingPreparing()
		|| isSellingMerchantingPreparing())
		return;

	Packet result(WIZ_MINING, uint8(FishingStart));
	uint16 resultCode = FishingResultSuccess;

	// Are we fishing already?
	if (isFishing())
		resultCode = FishingResultPreparing;

	// Do we have a rod? Is it worn?
	_ITEM_DATA* pItem;
	_ITEM_TABLE* pTable = GetItemPrototype(RIGHTHAND, pItem);
	if (pItem == nullptr || pTable == nullptr
		|| pItem->sDuration <= 0
		|| !pTable->isFishing())
		resultCode = FishingResultNotRod;
	else if (!CheckExistItem(RAINWORM, 1))
		resultCode = FishingResultNoRainworm;

	result << resultCode;

	// If nothing went wrong, allow the user to start fishing.
	// Be sure to let everyone know we're mining.
	if (resultCode == FishingResultSuccess)
	{
		_ITEM_TABLE* pItems = GetItemPrototype(CFAIRY);
		if (pItems != nullptr)
		{
			if (pItems->GetNum() == OFFLINE_FISHING_ITEM)
			{
				if (GetNation() == KARUS)
					StateChangeServerDirect(11, uint32(1));
				else if (GetNation() == ELMORAD)
					StateChangeServerDirect(11, uint32(2));

				StateChangeServerDirect(7, 2);
				m_bOfflineSystemSocketType = true;
				m_bOfflineSystemType = true;
				DateTime time;
				g_pMain->WriteOfflineSystemLog(string_format("[Fishing System Start - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
			}
		}

		m_bFishing = true;
		m_bResHpType = USER_FLASHING;
		result << GetID();
		
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	else
	{
		Send(&result);
	}
}
#pragma endregion

#pragma region CUser::HandleFishingAttempt(Packet & pkt)
/**
* @brief	Handles a user's mining attempt by finding a random reward (or none at all).
* 			This is sent automatically by the client every MINING_DELAY (5) seconds.
*
* @param	pkt	The packet.
*/
void CUser::HandleFishingAttempt(Packet & pkt)
{
	if (!isFishing())
		return;

	Packet result(WIZ_MINING, uint8(FishingAttempt));
	uint16 resultCode = FishingResultSuccess;

	// Do we have a pickaxe? Is it worn?
	_ITEM_DATA* pItem;
	_MINING_FISHING_ITEM* pFishing = nullptr;
	std::vector<uint32> pFishingList;
	_ITEM_TABLE* pTable = GetItemPrototype(RIGHTHAND, pItem);

	if (pItem == nullptr || pTable == nullptr
		|| pItem->sDuration <= 0 // are we supposed to wear the pickaxe on use? Need to verify.
		|| !pTable->isFishing())
		resultCode = FishingResultNotRod;
	else if (!CheckExistItem(RAINWORM, 1))
		resultCode = FishingResultNoRainworm;

	// Check to make sure we're not spamming the packet...
	if ((UNIXTIME - m_tLastFishingAttempt) < FISHING_DELAY)
		resultCode = FishingResultFishingAlready; // as close an error as we're going to get...
	else if ((UNIXTIME - m_tLastFishingAttempt) > 1 && (UNIXTIME - m_tLastFishingAttempt) < 10)
	{
		result << uint16(FishingResultSuccess)
			<< GetID() << uint16(0);
		
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
		return;
	}
	// Effect to show to clients
	uint16 sEffect = 0;

	// This is just a mock-up based on another codebase's implementation.
	// Need to log official data to get a proper idea of how it behaves, rate-wise,
	// so that we can then implement it more dynamically.
	if (resultCode == FishingResultSuccess && g_pMain->m_MiningFishingItemArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_MiningFishingItemArray)
		{
			pFishing = itr->second;

			if (pFishing == nullptr)
				continue;

			if (pFishing->nTableType != 1)
				continue;

			if (pTable->m_iNum == GOLDEN_FISHING)
			{
				if (pFishing->UseItemType == 1)
				{
					if (!g_pMain->isWarOpen())
					{
						if (pFishing->nWarStatus == MiningResultError)
							pFishingList.push_back(pFishing->nIndex);
						else
							continue;
					}
					else
					{
						if (GetZoneID() == ZONE_KARUS)
						{
							if (GetWarVictory() == ELMORAD && GetNation() == ELMORAD)
							{
								if (pFishing->nWarStatus == 2)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == ELMORAD && GetNation() == KARUS)
							{
								if (pFishing->nWarStatus == 1)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else
							{
								if (pFishing->nWarStatus == 0)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() == ZONE_ELMORAD)
						{
							if (GetWarVictory() == KARUS && GetNation() == KARUS)
							{
								if (pFishing->nWarStatus == 2)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == KARUS && GetNation() == ELMORAD)
							{
								if (pFishing->nWarStatus == 1)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else
							{
								if (pFishing->nWarStatus == 0)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5)
						{
							if (pFishing->nWarStatus == 0)
								pFishingList.push_back(pFishing->nIndex);
							else
								continue;
						}
					}
				}
			}
			else
			{
				if (pFishing->UseItemType == 0)
				{
					if (!g_pMain->isWarOpen())
					{
						if (pFishing->nWarStatus == MiningResultError)
							pFishingList.push_back(pFishing->nIndex);
						else
							continue;
					}
					else
					{
						if (GetZoneID() == ZONE_KARUS)
						{
							if (GetWarVictory() == ELMORAD && GetNation() == ELMORAD)
							{
								if (pFishing->nWarStatus == 2)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == ELMORAD && GetNation() == KARUS)
							{
								if (pFishing->nWarStatus == 1)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else
							{
								if (pFishing->nWarStatus == 0)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() == ZONE_ELMORAD)
						{
							if (GetWarVictory() == KARUS && GetNation() == KARUS)
							{
								if (pFishing->nWarStatus == 2)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else if (GetWarVictory() == KARUS && GetNation() == ELMORAD)
							{
								if (pFishing->nWarStatus == 1)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
							else
							{
								if (pFishing->nWarStatus == 0)
									pFishingList.push_back(pFishing->nIndex);
								else
									continue;
							}
						}
						else if (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5)
						{
							if (pFishing->nWarStatus == 0)
								pFishingList.push_back(pFishing->nIndex);
							else
								continue;
						}
					}
				}
			}
		}

		if (pFishingList.size() > 0)
		{
			int offset = 0;
			uint32 bRandArray[10000];
			uint8 sItemSlot = 0;
			foreach(itr, pFishingList)
			{
				pFishing = g_pMain->m_MiningFishingItemArray.GetData(*itr);

				if (pFishing == nullptr)
					return;

				if (offset >= 10000)
					break;

				for (int i = 0; i < int(pFishing->SuccessRate / 5); i++)
				{
					if (i + offset >= 10000)
						break;

					bRandArray[offset + i] = pFishing->nGiveItemID;
				}

				offset += int(pFishing->SuccessRate / 5);
			}

			uint32 bRandSlot = myrand(0, offset);
			uint32 nItemID = bRandArray[bRandSlot];
			if (nItemID == ITEM_EXP)
			{
				sEffect = 13082; // "XP" effect
				ExpChange(GetLevel() * myrand(0, 40) * 2);
			}
			else if (nItemID > 0)
			{
				sEffect = 13081; // "Item" effect
				GiveItem(nItemID);
			}
		}
	}

	result << resultCode;

	ItemWoreOut(ATTACK, 100);

	if (resultCode != FishingResultSuccess
		&& resultCode != FishingResultNothingFound)
	{
		// Tell us the error first
		Send(&result);

		// and then tell the client to stop mining
		HandleFishingStop(pkt);
		return;
	}

	if (GetItem(RIGHTHAND)->nNum != GOLDEN_FISHING)
		RobItem(RAINWORM, 1);

	result << GetID() << sEffect;

	if (resultCode != FishingResultNothingFound)
	{
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}
	else if (resultCode == FishingResultNothingFound)
		Send(&result);

	m_tLastFishingAttempt = UNIXTIME;
}
#pragma endregion

#pragma region CUser::HandleFishingStop(Packet & pkt)
/**
* @brief	Handles when a user stops fishing.
*
* @param	pkt	The packet.
*/
void CUser::HandleFishingStop(Packet & pkt)
{
	if (!isFishing())
		return;

	if (isOfflineSystem())
	{
		if (GetNation() == KARUS)
			StateChangeServerDirect(11, uint32(0));
		else if (GetNation() == ELMORAD)
			StateChangeServerDirect(11, uint32(0));

		StateChangeServerDirect(7, 0);
		m_bOfflineSystemSocketType = false;
		m_bOfflineSystemType = false;
		DateTime time;
		g_pMain->WriteOfflineSystemLog(string_format("[Fishing System Closed - %d:%d:%d ] = %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str()));
	}

	m_bResHpType = USER_STANDING;
	Packet result(WIZ_MINING, uint8(FishingStop));
	result << uint16(1) << GetID();
	m_bFishing = false;
	SendToRegion(&result);

	result.clear();
	result.SetOpcode(WIZ_MINING);
	result << uint8(FishingStop) << uint8(2);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleFishingDropTester(uint8 sType, uint8 sHours)
void CUser::HandleFishingDropTester(uint8 sType, uint8 sHours)
{
	uint32 TotalExp = 0;
	std::vector<uint32> pFishingList;
	foreach_stlmap(itr, g_pMain->m_MiningFishingItemArray)
	{
		_MINING_FISHING_ITEM* pFishing = itr->second;
		if (pFishing == nullptr
			|| pFishing->nWarStatus != 0
			|| pFishing->nTableType != 1)
			continue;

		if (pFishing->UseItemType == sType)
			pFishingList.push_back(pFishing->nIndex);
	}

	struct GIVE_DROP_LIST { uint32 ItemID = 0; uint16 ItemCount = 0; std::string ItemName = ""; };
	std::vector<GIVE_DROP_LIST> pFishingDropList;

	int t1 = sHours * 60;
	int t2 = t1 * 60;
	int t3 = t2 / 5;
	for (int i = 0; i < t3; i++)
	{
		int offset = 0;
		uint32 bRandArray[10000];
		memset(&bRandArray, 0, sizeof(bRandArray));
		uint8 sItemSlot = 0;
		foreach(itr, pFishingList)
		{
			_MINING_FISHING_ITEM* pFarFishing = g_pMain->m_MiningFishingItemArray.GetData(*itr);
			if (pFarFishing == nullptr)
				continue;

			if (offset >= 10000)
				break;

			for (int i = 0; i < int(pFarFishing->SuccessRate / 5); i++)
			{
				if (i + offset >= 10000)
					break;

				bRandArray[offset + i] = pFarFishing->nGiveItemID;
			}

			offset += int(pFarFishing->SuccessRate / 5);
		}

		uint32 bRandSlot = myrand(0, offset);
		uint32 nItemID = bRandArray[bRandSlot];

		_ITEM_TABLE* pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
		if (pItem == nullptr)
			continue;

		if (nItemID == ITEM_EXP)
		{
			int sExperience = 0;
			sExperience = (GetLevel() * myrand(0, 40)) * 2;
			uint32 expAmount = sExperience * (100 + g_pMain->m_byExpEventAmount) / 100;
			TotalExp += expAmount;
		}
		else
		{
			bool sAvailable = false;
			foreach(itr, pFishingDropList)
			{
				if (itr->ItemID == nItemID)
				{
					itr->ItemCount++;
					sAvailable = true;
					break;
				}
			}
			if (!sAvailable)
			{
				GIVE_DROP_LIST pList;
				pList.ItemID = nItemID;
				pList.ItemCount = 1;
				pList.ItemName = pItem->m_sName;
				pFishingDropList.push_back(pList);
			}
		}
	}

	Packet result;

	std::string teeext = string_format("--------------------Fishing Drop System--------------------");
	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << teeext;
	Send(&result);

	std::string ExpCommand = string_format("[Fishing Drop] Total Exp: %d", TotalExp);

	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << ExpCommand;
	Send(&result);

	foreach(itr, pFishingDropList)
	{
		std::string Command = string_format("[Fishing Drop] ItemName: %s, ItemCount: %d", itr->ItemName.c_str(), itr->ItemCount);

		result.clear();
		result.Initialize(WIZ_CHAT);
		result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << Command;
		Send(&result);
	}

	std::string teeext2 = string_format("------------------------------------------");
	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << teeext2;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleFishingDropTester(uint8 sType, uint8 sHours)
void CUser::HandleMiningDropTester(uint8 sType, uint8 sHours)
{
	uint32 TotalExp = 0;
	std::vector<uint32> pMiningList;
	foreach_stlmap(itr, g_pMain->m_MiningFishingItemArray)
	{
		_MINING_FISHING_ITEM* pMining = itr->second;
		if (pMining == nullptr
			|| pMining->nWarStatus != 0
			|| pMining->nTableType != 0)
			continue;

		if (pMining->UseItemType == sType)
			pMiningList.push_back(pMining->nIndex);
	}
	struct GIVE_DROP_LIST { uint32 ItemID = 0; uint16 ItemCount = 0; std::string ItemName = ""; };
	std::vector<GIVE_DROP_LIST> pMiningDropList;

	int t1 = sHours * 60;
	int t2 = t1 * 60;
	int t3 = t2 / 5;

	if (t3 <= 0) return;
	for (int i = 0; i < t3; i++)
	{
		int offset = 0;
		uint32 bRandArray[10000];
		memset(&bRandArray, 0, sizeof(bRandArray));
		uint8 sItemSlot = 0;
		foreach(itr, pMiningList)
		{
			_MINING_FISHING_ITEM* pFarMining = g_pMain->m_MiningFishingItemArray.GetData(*itr);
			if (pFarMining == nullptr)
				continue;

			if (offset >= 10000)
				break;

			for (int i = 0; i < int(pFarMining->SuccessRate / 5); i++)
			{
				if (i + offset >= 10000)
					break;

				bRandArray[offset + i] = pFarMining->nGiveItemID;
			}

			offset += int(pFarMining->SuccessRate / 5);
		}

		uint32 bRandSlot = myrand(0, offset);
		uint32 nItemID = bRandArray[bRandSlot];

		_ITEM_TABLE* pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
		if (pItem == nullptr)
			continue;

		if (nItemID == ITEM_EXP)
		{
			int sExperience = 0;
			sExperience = (GetLevel() * myrand(0, 40)) * 2;
			uint32 expAmount = sExperience * (100 + g_pMain->m_byExpEventAmount) / 100;
			TotalExp += expAmount;
		}
		else
		{
			bool sAvailable = false;
			foreach(itr, pMiningDropList)
			{
				if (itr->ItemID == nItemID)
				{
					itr->ItemCount++;
					sAvailable = true;
					break;
				}
			}
			if (!sAvailable)
			{
				GIVE_DROP_LIST pList;
				pList.ItemID = nItemID;
				pList.ItemCount = 1;
				pList.ItemName = pItem->m_sName;
				pMiningDropList.push_back(pList);
			}
		}
	}

	Packet result;

	std::string teeext = string_format("--------------------Mining Drop System--------------------");
	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << teeext;
	Send(&result);

	std::string ExpCommand = string_format("[Mining Drop] Total Exp: %d", TotalExp);

	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << ExpCommand;
	Send(&result);

	foreach(itr, pMiningDropList)
	{
		std::string Command = string_format("[Mining Drop] ItemName: %s, ItemCount: %d", itr->ItemName.c_str(), itr->ItemCount);

		result.clear();
		result.Initialize(WIZ_CHAT);
		result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << Command;
		Send(&result);
	}

	std::string teeext2 = string_format("------------------------------------------");
	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << teeext2;
	Send(&result);
}
#pragma endregion