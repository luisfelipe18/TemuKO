#include "stdafx.h"
#include "KingSystem.h"

#pragma region CUser::HandlePlayerRankings(Packet & pkt)
void CUser::HandlePlayerRankings(Packet& pkt)
{
	C3DMap* pMap = GetMap();
	if (pMap == nullptr
		|| g_pMain->m_IsPlayerRankingUpdateProcess)
		return;

	uint8 nRankType = pkt.read<uint8>();

	switch (nRankType)
	{
	case RANK_TYPE_PK_ZONE:
		HandleRankingPKZone(pkt);
		break;
	case RANK_TYPE_ZONE_BORDER_DEFENSE_WAR:
		HandleRankingBDW(pkt);
		break;
	case RANK_TYPE_CHAOS_DUNGEON:
		HandleRankingChaosDungeon(pkt);
		break;
	default:
		printf("Player Ranksings unhandled packets %d \n", nRankType);
		TRACE("Player Ranksings unhandled packets %d \n", nRankType);
		break;
	}
}
#pragma endregion

#pragma region CUser::HandleRankingPKZone(Packet & pkt)
void CUser::HandleRankingPKZone(Packet& pkt)
{
	Packet result(WIZ_RANK, uint8(RankTypes::RANK_TYPE_PK_ZONE));
	bool myRankFound = false;
	uint16 nMyRank = 0;
	uint16 sCount = 0;
	size_t wpos = 0;

	std::vector<_PLAYER_KILLING_ZONE_RANKING> UserRankingSorted[2];
	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		foreach_stlmap(itr, g_pMain->m_UserPlayerKillingZoneRankingArray[nation])
		{
			if (GetZoneID() == itr->second->p_Zone)
				UserRankingSorted[nation].push_back(*itr->second);
		}

		std::sort(UserRankingSorted[nation].begin(), UserRankingSorted[nation].end(),
			[](_PLAYER_KILLING_ZONE_RANKING const& a, _PLAYER_KILLING_ZONE_RANKING const& b) { return a.P_LoyaltyDaily > b.P_LoyaltyDaily; });

		sCount = 0;
		wpos = result.wpos();
		result << sCount;

		if ((uint32)UserRankingSorted[nation].size() > 0)
		{
			foreach(itr, UserRankingSorted[nation])
			{
				if ((myRankFound == true && sCount > 9)
					|| (GetNation() != (nation + 1) && sCount > 9))
					break;

				_PLAYER_KILLING_ZONE_RANKING* pRankInfo = &(*itr);
				if (pRankInfo == nullptr)
					continue;

				if (myRankFound == false && GetNation() == (nation + 1))
				{
					nMyRank++;
					if (pRankInfo->p_SocketID == GetSocketID())
						myRankFound = true;
				}

				if (sCount <= 9)
				{
					CUser* pUser = g_pMain->GetUserPtr(pRankInfo->p_SocketID);
					if (pUser == nullptr)
						continue;

					if (!pUser->isInGame())
						continue;

					result << pUser->GetName() << GetNation();

					CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

					if (pKnights == nullptr)
						result << uint16(0) << uint16(0) << (std::string)"";
					else
						result << pKnights->GetID() << pKnights->m_sMarkVersion << pKnights->GetName();

					result << pRankInfo->P_LoyaltyDaily;

					if (pRankInfo->P_LoyaltyPremiumBonus <= 999)
						result << pRankInfo->P_LoyaltyPremiumBonus;
					else
						result << uint16(999);
					sCount++;
				}
			}
		}
		result.put(wpos, sCount);
		wpos = result.wpos();
	}

	result << nMyRank << m_PlayerKillingLoyaltyDaily << m_PlayerKillingLoyaltyPremiumBonus;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleRankingBDW(Packet & pkt)
void CUser::HandleRankingBDW(Packet& pkt)
{
	_BDW_ROOM_INFO* pBdwRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pBdwRoomInfo == nullptr || !g_pMain->isBorderDefenceWarActive())
		return;

	Packet result(WIZ_RANK, uint8(RankTypes::RANK_TYPE_ZONE_BORDER_DEFENSE_WAR));
	uint16 sCount = 0;
	size_t wpos = 0;

	std::vector<_BORDER_DEFENCE_WAR_RANKING> UserRankingSorted[2];

	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		foreach_stlmap(itr, g_pMain->m_UserBorderDefenceWarRankingArray[nation])
		{
			if (m_bEventRoom == itr->second->b_EventRoom)
				UserRankingSorted[nation].push_back(*itr->second);
		}

		std::sort(UserRankingSorted[nation].begin(), UserRankingSorted[nation].end(),
			[](_BORDER_DEFENCE_WAR_RANKING const& a, _BORDER_DEFENCE_WAR_RANKING const& b) { return a.b_UserPoint > b.b_UserPoint; });

		sCount = 0;
		wpos = result.wpos();
		result << sCount;

		if ((uint32)UserRankingSorted[nation].size() > 0)
		{
			foreach(itr, UserRankingSorted[nation])
			{
				if (sCount > 7)
					break;

				_BORDER_DEFENCE_WAR_RANKING* pRankInfo = &(*itr);
				if (pRankInfo == nullptr)
					continue;

				if (sCount <= 7)
				{
					CUser* pUser = g_pMain->GetUserPtr(pRankInfo->b_SocketID);
					if (pUser == nullptr)
						continue;

					if (!pUser->isInGame())
						continue;

					result << pUser->GetName() << GetNation();

					CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

					if (pKnights == nullptr)
						result << uint16(0) << uint16(0) << (std::string)"";
					else
						result << pKnights->GetID() << pKnights->m_sMarkVersion << pKnights->GetName();

					result << pRankInfo->b_UserPoint;
					sCount++;
				}
			}
		}
		result.put(wpos, sCount);
		wpos = result.wpos();
	}

	int64 nGainedExp = int64(pow(GetLevel(), 3) * 0.15 * (5 * m_BorderDefenceWarUserPoint));
	int64 nPremiumGainedExp = nGainedExp * 2;

	if (nGainedExp < 0) nGainedExp = 0;

	if (nGainedExp > 8000000) nGainedExp = 8000000;

	if (nPremiumGainedExp < 0) nPremiumGainedExp = 0;

	if (nPremiumGainedExp > 10000000) nPremiumGainedExp = 10000000;

	result << nGainedExp << nPremiumGainedExp;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleRankingChaosDungeon(Packet & pkt)
void CUser::HandleRankingChaosDungeon(Packet& pkt)
{
	_CHAOS_ROOM_INFO* pChaosRoom = g_pMain->m_TempleEventChaosRoomList.GetData(GetEventRoom());
	if (pChaosRoom == nullptr || !g_pMain->isChaosExpansionActive())
		return;

	Packet result(WIZ_RANK, uint8(RankTypes::RANK_TYPE_CHAOS_DUNGEON));

	std::vector<_CHAOS_EXPANSION_RANKING> UserRankingSorted;
	foreach_stlmap(itr, g_pMain->m_UserChaosExpansionRankingArray)
	{
		if (m_bEventRoom == itr->second->c_EventRoom)
			UserRankingSorted.push_back(*itr->second);
	}

	std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
		[](_CHAOS_EXPANSION_RANKING const& a, _CHAOS_EXPANSION_RANKING const& b) { return a.c_KillCount > b.c_DeathCount; });

	uint8 sCount = 0;
	size_t wpos = result.wpos();
	result << sCount;
	foreach(itr, UserRankingSorted)
	{
		if (sCount > 18)
			break;

		_CHAOS_EXPANSION_RANKING* pRankInfo = &(*itr);
		if (pRankInfo == nullptr
			|| GetEventRoom() != pRankInfo->c_EventRoom)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(pRankInfo->c_SocketID);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		result << pUser->GetName() << pRankInfo->c_KillCount << pRankInfo->c_DeathCount;
		sCount++;
	}
	result.put(wpos, sCount);
	wpos = result.wpos();

	int32 nGainedExp = int32(pow(GetLevel(), 3) * 0.15 * (5 * m_ChaosExpansionKillCount - m_ChaosExpansionDeadCount));
	int32 nPremiumGainedExp = (int64)nGainedExp * 2;

	if (nGainedExp < 0) nGainedExp = 0;

	if (nGainedExp > 8000000) nGainedExp = 8000000;

	if (nPremiumGainedExp < 0) nPremiumGainedExp = 0;

	if (nPremiumGainedExp > 10000000) nPremiumGainedExp = 10000000;

	result << nGainedExp << nPremiumGainedExp << uint32(1);
	Send(&result);
}
#pragma endregion

#pragma region CUser::PlayerKillingAddPlayerRank()
void CUser::PlayerKillingAddPlayerRank()
{
	m_PlayerKillingLoyaltyDaily = 0; m_PlayerKillingLoyaltyPremiumBonus = 0;

	if (GetNation() == KARUS)
	{
		_PLAYER_KILLING_ZONE_RANKING* pKarusRanking = g_pMain->m_UserPlayerKillingZoneRankingArray[0].GetData(GetSocketID());
		if (pKarusRanking == nullptr)
		{
			_PLAYER_KILLING_ZONE_RANKING* pKillinZoneRank = new _PLAYER_KILLING_ZONE_RANKING;
			pKillinZoneRank->p_SocketID = GetSocketID();
			pKillinZoneRank->P_Nation = GetNation();
			pKillinZoneRank->p_Zone = GetZoneID();
			pKillinZoneRank->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pKillinZoneRank->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
			if (!g_pMain->m_UserPlayerKillingZoneRankingArray[0].PutData(pKillinZoneRank->p_SocketID, pKillinZoneRank))
				delete pKillinZoneRank;
		}
		else
		{
			pKarusRanking->p_SocketID = GetSocketID();
			pKarusRanking->P_Nation = GetNation();
			pKarusRanking->p_Zone = GetZoneID();
			pKarusRanking->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pKarusRanking->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
		}
		g_pMain->m_UserPlayerKillingZoneRankingArray[1].DeleteData(GetSocketID());
	}
	else
	{
		_PLAYER_KILLING_ZONE_RANKING* pHumanRanking = g_pMain->m_UserPlayerKillingZoneRankingArray[1].GetData(GetSocketID());
		if (pHumanRanking == nullptr)
		{
			_PLAYER_KILLING_ZONE_RANKING* pKillinZoneRank = new _PLAYER_KILLING_ZONE_RANKING;
			pKillinZoneRank->p_SocketID = GetSocketID();
			pKillinZoneRank->P_Nation = GetNation();
			pKillinZoneRank->p_Zone = GetZoneID();
			pKillinZoneRank->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pKillinZoneRank->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
			if (!g_pMain->m_UserPlayerKillingZoneRankingArray[1].PutData(pKillinZoneRank->p_SocketID, pKillinZoneRank))
				delete pKillinZoneRank;
		}
		else
		{
			pHumanRanking->p_SocketID = GetSocketID();
			pHumanRanking->P_Nation = GetNation();
			pHumanRanking->p_Zone = GetZoneID();
			pHumanRanking->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pHumanRanking->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
		}
		g_pMain->m_UserPlayerKillingZoneRankingArray[0].DeleteData(GetSocketID());
	}
}
#pragma endregion

#pragma region CUser::ChaosExpansionAddPlayerRank()
void CUser::ChaosExpansionAddPlayerRank()
{
	m_ChaosExpansionKillCount = 0; m_ChaosExpansionDeadCount = 0;

	_CHAOS_EXPANSION_RANKING* pReturn = g_pMain->m_UserChaosExpansionRankingArray.GetData(GetSocketID());
	if (pReturn == nullptr)
	{
		_CHAOS_EXPANSION_RANKING* pChaosRank = new _CHAOS_EXPANSION_RANKING;
		pChaosRank->c_SocketID = GetSocketID();
		pChaosRank->c_EventRoom = GetEventRoom();
		pChaosRank->c_KillCount = m_ChaosExpansionKillCount;
		pChaosRank->c_DeathCount = m_ChaosExpansionDeadCount;
		if (!g_pMain->m_UserChaosExpansionRankingArray.PutData(pChaosRank->c_SocketID, pChaosRank))
			delete pChaosRank;
	}
	else
	{
		pReturn->c_SocketID = GetSocketID();
		pReturn->c_EventRoom = GetEventRoom();
		pReturn->c_KillCount = m_ChaosExpansionKillCount;
		pReturn->c_DeathCount = m_ChaosExpansionDeadCount;
	}
}
#pragma endregion

#pragma region CUser::BorderDefenceAddPlayerRank()
void CUser::BorderDefenceAddPlayerRank()
{
	m_BorderDefenceWarUserPoint = 0;

	if (GetNation() == KARUS)
	{
		_BORDER_DEFENCE_WAR_RANKING* pKarusRanking = g_pMain->m_UserBorderDefenceWarRankingArray[0].GetData(GetSocketID());
		if (pKarusRanking == nullptr)
		{
			_BORDER_DEFENCE_WAR_RANKING* pBdwRank = new _BORDER_DEFENCE_WAR_RANKING;
			pBdwRank->b_SocketID = GetSocketID();
			pBdwRank->b_EventRoom = GetEventRoom();
			pBdwRank->b_Nation = GetNation();
			pBdwRank->b_UserPoint = m_BorderDefenceWarUserPoint;
			if (!g_pMain->m_UserBorderDefenceWarRankingArray[0].PutData(pBdwRank->b_SocketID, pBdwRank))
				delete pBdwRank;
		}
		else
		{
			pKarusRanking->b_SocketID = GetSocketID();
			pKarusRanking->b_EventRoom = GetEventRoom();
			pKarusRanking->b_Nation = GetNation();
			pKarusRanking->b_UserPoint = m_BorderDefenceWarUserPoint;
		}
		g_pMain->m_UserBorderDefenceWarRankingArray[1].DeleteData(GetSocketID());
	}
	else
	{
		_BORDER_DEFENCE_WAR_RANKING* pHumanRanking = g_pMain->m_UserBorderDefenceWarRankingArray[1].GetData(GetSocketID());
		if (pHumanRanking == nullptr)
		{
			_BORDER_DEFENCE_WAR_RANKING* pBdwRank = new _BORDER_DEFENCE_WAR_RANKING;
			pBdwRank->b_SocketID = GetSocketID();
			pBdwRank->b_EventRoom = GetEventRoom();
			pBdwRank->b_Nation = GetNation();
			pBdwRank->b_UserPoint = m_BorderDefenceWarUserPoint;
			if (!g_pMain->m_UserBorderDefenceWarRankingArray[1].PutData(pBdwRank->b_SocketID, pBdwRank))
				delete pBdwRank;
		}
		else
		{
			pHumanRanking->b_SocketID = GetSocketID();
			pHumanRanking->b_EventRoom = GetEventRoom();
			pHumanRanking->b_Nation = GetNation();
			pHumanRanking->b_UserPoint = m_BorderDefenceWarUserPoint;
		}
		g_pMain->m_UserBorderDefenceWarRankingArray[0].DeleteData(GetSocketID());
	}
}
#pragma endregion

#pragma region CUser::PlayerKillingRemovePlayerRank()
void CUser::PlayerKillingRemovePlayerRank()
{
	m_PlayerKillingLoyaltyDaily = 0; m_PlayerKillingLoyaltyPremiumBonus = 0;

	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		g_pMain->m_UserPlayerKillingZoneRankingArray[nation].DeleteData(GetSocketID());
	}
}
#pragma endregion

#pragma region CUser::BorderDefenceRemovePlayerRank()
void CUser::BorderDefenceRemovePlayerRank()
{
	m_BorderDefenceWarUserPoint = 0;
	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		g_pMain->m_UserBorderDefenceWarRankingArray[nation].DeleteData(GetSocketID());
	}
}
#pragma endregion

#pragma region CUser::ChaosExpansionRemovePlayerRank)
void CUser::ChaosExpansionRemovePlayerRank()
{
	m_ChaosExpansionKillCount = 0; m_ChaosExpansionDeadCount = 0;
	g_pMain->m_UserChaosExpansionRankingArray.DeleteData(GetSocketID());
}
#pragma endregion

#pragma region CUser::UpdatePlayerKillingRank()
void CUser::UpdatePlayerKillingRank()
{
	if (GetNation() == KARUS)
	{
		_PLAYER_KILLING_ZONE_RANKING* pKarusRanking = g_pMain->m_UserPlayerKillingZoneRankingArray[0].GetData(GetSocketID());;
		if (pKarusRanking != nullptr)
		{
			pKarusRanking->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pKarusRanking->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
		}
	}
	else
	{
		_PLAYER_KILLING_ZONE_RANKING* pKarusRanking = g_pMain->m_UserPlayerKillingZoneRankingArray[1].GetData(GetSocketID());;
		if (pKarusRanking != nullptr)
		{
			pKarusRanking->P_LoyaltyDaily = m_PlayerKillingLoyaltyDaily;
			pKarusRanking->P_LoyaltyPremiumBonus = m_PlayerKillingLoyaltyPremiumBonus;
		}
	}
}
#pragma endregion

#pragma region CUser::UpdateBorderDefenceWarRank()
void CUser::UpdateBorderDefenceWarRank()
{
	if (GetNation() == KARUS)
	{
		if (g_pMain->isBorderDefenceWarActive())
		{
			_BORDER_DEFENCE_WAR_RANKING* pKarusRanking = g_pMain->m_UserBorderDefenceWarRankingArray[0].GetData(GetSocketID());;
			if (pKarusRanking != nullptr)
				pKarusRanking->b_UserPoint = m_BorderDefenceWarUserPoint;
		}
	}
	else
	{
		_BORDER_DEFENCE_WAR_RANKING* pKarusRanking = g_pMain->m_UserBorderDefenceWarRankingArray[1].GetData(GetSocketID());;
		if (pKarusRanking != nullptr)
			pKarusRanking->b_UserPoint = m_BorderDefenceWarUserPoint;
	}
}
#pragma endregion

#pragma region CUser::UpdateChaosExpansionRank()
void CUser::UpdateChaosExpansionRank()
{
	if (g_pMain->isChaosExpansionActive())
	{
		_CHAOS_EXPANSION_RANKING* pRankInfo = g_pMain->m_UserChaosExpansionRankingArray.GetData(GetSocketID());
		if (pRankInfo != nullptr)
		{
			pRankInfo->c_KillCount = m_ChaosExpansionKillCount;
			pRankInfo->c_DeathCount = m_ChaosExpansionDeadCount;
		}
	}
}
#pragma endregion

#pragma region CUser::GetPlayerRank(uint8 nRankType)
uint16 CUser::GetPlayerRank(uint8 nRankType)
{
	if (!isInGame() 
		|| GetNation() == 0)
		return 0;

	uint16 nMyRank = 0;

	switch (nRankType)
	{
	case RANK_TYPE_CHAOS_DUNGEON:
	{
		std::vector<_CHAOS_EXPANSION_RANKING> UserRankingSorted;

		foreach_stlmap(itr, g_pMain->m_UserChaosExpansionRankingArray)
		{
			if (GetEventRoom() == itr->second->c_EventRoom)
				UserRankingSorted.push_back(*itr->second);
		}

		std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
			[](_CHAOS_EXPANSION_RANKING const& a, _CHAOS_EXPANSION_RANKING const& b) { return a.c_KillCount > b.c_KillCount; });

		foreach(itr, UserRankingSorted)
		{
			_CHAOS_EXPANSION_RANKING* pRankInfo = &(*itr);

			if (pRankInfo == nullptr)
				continue;

			nMyRank++;

			if (GetSocketID() == pRankInfo->c_SocketID)
				break;
		}
		return nMyRank;
	}
	break;
	case RANK_TYPE_PK_ZONE:
	{
		int8 nRankArrayIndex = -1;
		GetNation() == Nation::KARUS ? nRankArrayIndex = 0 : nRankArrayIndex = 1;

		if (nRankArrayIndex == -1)
			return 0;

		std::vector<_PLAYER_KILLING_ZONE_RANKING> UserRankingSorted;

		foreach_stlmap(itr, g_pMain->m_UserPlayerKillingZoneRankingArray[nRankArrayIndex])
			UserRankingSorted.push_back(*itr->second);

		std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
			[](_PLAYER_KILLING_ZONE_RANKING const& a, _PLAYER_KILLING_ZONE_RANKING const& b) { return a.P_LoyaltyDaily > b.P_LoyaltyDaily; });

		foreach(itr, UserRankingSorted)
		{
			_PLAYER_KILLING_ZONE_RANKING* pRankInfo = &(*itr);
			if (pRankInfo == nullptr)
				continue;

			nMyRank++;

			if (GetSocketID() == pRankInfo->p_SocketID)
				break;
		}
		return nMyRank;
	}
	break;
	case RANK_TYPE_ZONE_BORDER_DEFENSE_WAR:
	{
		int8 nRankArrayIndex = -1;
		GetNation() == Nation::KARUS ? nRankArrayIndex = 0 : nRankArrayIndex = 1;

		if (nRankArrayIndex == -1)
			return 0;

		std::vector<_BORDER_DEFENCE_WAR_RANKING> UserRankingSorted;

		foreach_stlmap(itr, g_pMain->m_UserBorderDefenceWarRankingArray[nRankArrayIndex])
		{
			if (GetEventRoom() == itr->second->b_EventRoom)
				UserRankingSorted.push_back(*itr->second);
		}

		std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
			[](_BORDER_DEFENCE_WAR_RANKING const& a, _BORDER_DEFENCE_WAR_RANKING const& b) { return a.b_UserPoint > b.b_UserPoint; });

		foreach(itr, UserRankingSorted)
		{
			_BORDER_DEFENCE_WAR_RANKING* pRankInfo = &(*itr);
			if (pRankInfo == nullptr)
				continue;

			nMyRank++;

			if (GetSocketID() == pRankInfo->b_SocketID)
				break;
		}
		return nMyRank;
	}
	break;
	}
	return 0;
}
#pragma endregion

void CGameServerDlg::ResetPlayerRankings()
{
	if (m_sRankResetHour == m_nPlayerRankingResetTime)
	{
		m_IsPlayerRankingUpdateProcess = true;
		m_sRankResetHour = 0;
		foreach_stlmap(itr, g_pMain->m_ZoneArray)
		{
			KOMap* pMap = itr->second;
			if (pMap == nullptr)
				continue;

			if (pMap->GetID() != ZONE_ARDREAM
				&& pMap->GetID() != ZONE_RONARK_LAND
				&& pMap->GetID() != ZONE_RONARK_LAND_BASE)
				continue;

			ResetPlayerKillingRanking();
		}

		m_IsPlayerRankingUpdateProcess = false;
		return;
	}
	m_sRankResetHour++;
}

#pragma region CGameServerDlg::ResetPlayerKillingRanking()
void CGameServerDlg::ResetPlayerKillingRanking()
{
	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		foreach_stlmap(itrRank, m_UserPlayerKillingZoneRankingArray[nation])
		{
			_PLAYER_KILLING_ZONE_RANKING* pRankInfo = itrRank->second;
			if (pRankInfo == nullptr)
				continue;

			CUser* pUser = g_pMain->GetUserPtr(pRankInfo->p_SocketID);
			if (pUser == nullptr)
				continue;

			pRankInfo->P_LoyaltyDaily = 0;
			pRankInfo->P_LoyaltyPremiumBonus = 0;
			pUser->m_PlayerKillingLoyaltyDaily = 0;
			pUser->m_PlayerKillingLoyaltyPremiumBonus = 0;
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::TrashBorderDefenceWarRanking()
void CGameServerDlg::TrashBorderDefenceWarRanking()
{
	m_UserBorderDefenceWarRankingArray[0].DeleteAllData();
	m_UserBorderDefenceWarRankingArray[1].DeleteAllData();
}
#pragma endregion

#pragma region CGameServerDlg::TrashChaosExpansionRanking()
void CGameServerDlg::TrashChaosExpansionRanking()
{
	m_UserChaosExpansionRankingArray.DeleteAllData();
}
#pragma endregion

#pragma region CUser::ShowBulletinBoard()
/**
* @brief	Shows the Bulletin Rank Board for the respective nation.
*/
void CUser::ShowBulletinBoard()
{
	if (GetNation() == KARUS)
		ShowKarusRankBoard();
	else if (GetNation() == ELMORAD)
		ShowElmoradRankBoard();
}
#pragma endregion


#pragma region CUser::ShowKarusRankBoard()
/**
* @brief	Shows the Bulletin Rank Board for karus naton.
*/
void CUser::ShowKarusRankBoard()
{
	Packet result(WIZ_BATTLE_EVENT, uint8(13));
	result.DByte();

	std::string strKingName = "";
	CKingSystem* pKingSystem = g_pMain->m_KingSystemArray.GetData(KARUS);
	CKnights* pKingKnights = nullptr;
	if (pKingSystem != nullptr)
	{
		strKingName = pKingSystem->m_strKingName;
		pKingKnights = g_pMain->GetClanPtr(pKingSystem->m_sKingClanID);
	}

	std::string emptyUserName = "<Empty>";
	uint8 sCount = 1;
	size_t wpos = result.wpos();
	result << sCount;

	if (pKingKnights == nullptr)
		result << uint16(1000) << strKingName << uint16(0) << uint16(-1) << uint16(0) << uint16(0) << uint16(0);
	else
		result << uint16(1000) << strKingName << uint16(0)
		<< pKingKnights->GetID() << uint16(pKingKnights->m_sMarkVersion) << pKingKnights->GetName()
		<< uint8(pKingKnights->m_byFlag)  // clan flag
		<< uint8(pKingKnights->m_byGrade); // clan grade

	uint16 index = 1201;
	int counter = 0;
	{
		foreach_stlmap(itr, g_pMain->m_KnightsRatingArray[KARUS_ARRAY])
		{
			if (counter++ > 9)
				break;
			CKnights* pKnights = g_pMain->GetClanPtr(itr->second->sClanID);
			if (pKnights == nullptr)
				continue;

			result << index++ << pKnights->m_strChief << uint16(0)
				<< pKnights->GetID() << uint16(pKnights->m_sMarkVersion) << pKnights->GetName()
				<< uint8(pKnights->m_byFlag)  // clan flag
				<< uint8(pKnights->m_byGrade); // clan grade
			sCount++;
		}
	}


	index = 1101;
	{
		counter = 0;
		Guard lock(g_pMain->m_userRankingsLock);
		std::vector<_USER_RANK> UserRankingSorted;
		foreach(itr, g_pMain->m_UserKarusPersonalRankMap)
		{
			UserRankingSorted.push_back(*(itr->second));
		}

		std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
			[](_USER_RANK const& a, _USER_RANK const& b) { return a.nLoyalty > b.nLoyalty; });

		foreach(itr, UserRankingSorted)
		{
			if (counter++ > 9)
				break;
			result << index++ << itr->strUserName << uint16(0);

			CKnights* pKnights = g_pMain->GetClanPtr(itr->sKnights);
			if (pKnights == nullptr)
				result << uint16(-1) << uint16(0) << uint16(0) << uint16(0);
			else
				result << uint16(pKnights->GetID()) << uint16(pKnights->m_sMarkVersion) << pKnights->GetName()
				<< uint8(pKnights->m_byFlag)  // clan flag
				<< uint8(pKnights->m_byGrade); // clan grade

			sCount++;
		}
	}

	/*result << uint16(2311) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2312) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2313) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2321) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2322) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2323) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2331) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2332) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2333) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0);

	result << uint16(2011) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2012) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2013) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2021) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2022) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2023) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2031) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2032) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2033) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0);

	sCount += 18;*/
	result.put(wpos, sCount);
	wpos = result.wpos();

	Send(&result);
}
#pragma endregion

#pragma region CUser::ShowElmoradRankBoard()
/**
* @brief	Shows the Bulletin Rank Board for elmorad naton.
*/
void CUser::ShowElmoradRankBoard()
{
	Packet result(WIZ_BATTLE_EVENT, uint8(13));
	result.DByte();

	std::string strKingName = "";
	CKingSystem* pKingSystem = g_pMain->m_KingSystemArray.GetData(ELMORAD);
	CKnights* pKingKnights = nullptr;
	if (pKingSystem != nullptr)
	{
		strKingName = pKingSystem->m_strKingName;
		pKingKnights = g_pMain->GetClanPtr(pKingSystem->m_sKingClanID);
	}

	std::string emptyUserName = "<Empty>";
	uint8 sCount = 1;
	size_t wpos = result.wpos();
	result << sCount;

	if (pKingKnights == nullptr)
		result << uint16(1000) << strKingName << uint16(0) << uint16(-1) << uint16(0) << uint16(0) << uint16(0);
	else
		result << uint16(1000) << strKingName << uint16(0)
		<< pKingKnights->GetID() << uint16(pKingKnights->m_sMarkVersion) << pKingKnights->GetName()
		<< uint8(pKingKnights->m_byFlag)  // clan flag
		<< uint8(pKingKnights->m_byGrade); // clan grade

	uint16 index = 1201;
	int counter = 0;
	{
		foreach_stlmap(itr, g_pMain->m_KnightsRatingArray[ELMORAD_ARRAY])
		{
			if (counter++ > 9)
				break;
			CKnights* pKnights = g_pMain->GetClanPtr(itr->second->sClanID);
			if (pKnights == nullptr)
				continue;

			result << index++ << pKnights->m_strChief << uint16(0)
				<< pKnights->GetID() << uint16(pKnights->m_sMarkVersion) << pKnights->GetName()
				<< uint8(pKnights->m_byFlag)  // clan flag
				<< uint8(pKnights->m_byGrade); // clan grade
			sCount++;
		}
	}


	index = 1101;
	{
		counter = 0;
		Guard lock(g_pMain->m_userRankingsLock);
		std::vector<_USER_RANK> UserRankingSorted;
		foreach(itr, g_pMain->m_UserElmoPersonalRankMap)
		{
			UserRankingSorted.push_back(*(itr->second));
		}

		std::sort(UserRankingSorted.begin(), UserRankingSorted.end(),
			[](_USER_RANK const& a, _USER_RANK const& b) { return a.nLoyalty > b.nLoyalty; });

		foreach(itr, UserRankingSorted)
		{
			if (counter++ > 9)
				break;
			result << index++ << itr->strUserName << uint16(0);

			CKnights* pKnights = g_pMain->GetClanPtr(itr->sKnights);
			if (pKnights == nullptr)
				result << uint16(-1) << uint16(0) << uint16(0) << uint16(0);
			else
				result << uint16(pKnights->GetID()) << uint16(pKnights->m_sMarkVersion) << pKnights->GetName()
				<< uint8(pKnights->m_byFlag)  // clan flag
				<< uint8(pKnights->m_byGrade); // clan grade

			sCount++;
		}

	}

	/*result << uint16(2311) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2312) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2313) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2321) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2322) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2323) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2331) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2332) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2333) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0);

	result << uint16(2011) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2012) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2013) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2021) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2022) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2023) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2031) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2032) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0)
	<< uint16(2033) << "<Empty>" << uint16(0) << uint16(-1) <<  uint16(0) <<  "<Empty>" <<  uint16(0);

	sCount += 18;*/
	result.put(wpos, sCount);
	wpos = result.wpos();

	Send(&result);
}
#pragma endregion

#pragma region CUser::GetRankReward(bool isMonthly)
/**
* @brief	Get a player loyalty reward.
*
* @param	isMonthly	Monthly reward.
*/
uint8 CUser::GetRankReward(bool isMonthly)
{
	enum RankErrorCodes
	{
		NoRank = 0,
		RewardSuccessfull = 1,
		RewardAlreadyTaken = 2
	};

	int8 nRank = -1;
	int32 nGoldAmount = 0;

	Guard lock(g_pMain->m_userRankingsLock);

	std::string strUserID = GetName();
	STRTOUPPER(strUserID);

	UserNameRankMap::iterator itr;

	if (isMonthly)
	{
		if (GetNation() == KARUS)
		{
			itr = g_pMain->m_UserKarusPersonalRankMap.find(strUserID);
			nRank = itr != g_pMain->m_UserKarusPersonalRankMap.end() ? int8(itr->second->nRank) : -1;
		}
		else
		{
			itr = g_pMain->m_UserElmoPersonalRankMap.find(strUserID);
			nRank = itr != g_pMain->m_UserElmoPersonalRankMap.end() ? int8(itr->second->nRank) : -1;
		}
	}
	else
	{
		if (GetNation() == KARUS)
		{
			itr = g_pMain->m_UserKarusKnightsRankMap.find(strUserID);
			nRank = itr != g_pMain->m_UserKarusKnightsRankMap.end() ? int8(itr->second->nRank) : -1;
		}
		else
		{
			itr = g_pMain->m_UserElmoKnightsRankMap.find(strUserID);
			nRank = itr != g_pMain->m_UserElmoKnightsRankMap.end() ? int8(itr->second->nRank) : -1;
		}

	}

	nRank = 1;

	if (nRank > 0 && nRank <= 100)
	{
		if (nRank == 1)
			nGoldAmount = 1000000;
		else if (nRank >= 2 && nRank <= 4)
			nGoldAmount = 700000;
		else if (nRank >= 5 && nRank <= 10)
			nGoldAmount = 500000;
		else if (nRank >= 11 && nRank <= 40)
			nGoldAmount = 300000;
		else if (nRank >= 41 && nRank <= 100)
			nGoldAmount = 200000;
		else
			nGoldAmount = 0;

		if (nGoldAmount > 0)
		{
			if (GetUserDailyOp(isMonthly ? DAILY_USER_PERSONAL_RANK_REWARD : DAILY_USER_RANK_REWARD) == 0)
			{
				return RewardAlreadyTaken;
			}

			GoldGain(nGoldAmount);
			return RewardSuccessfull;
		}
	}
	return NoRank;
}

#pragma endregion