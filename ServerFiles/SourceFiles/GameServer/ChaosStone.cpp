#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;

#pragma region CGameServerDlg::ChaosStoneLoad()
bool CGameServerDlg::ChaosStoneLoad()
{
	uint8 ChaosStoneResCount = 0;
	if (g_pMain->ChaosStoneRespawnOkey && g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize() > 0)
	{
		for (int i = 1; i <= g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize(); i++)
		{
			_CHAOS_STONE_RESPAWN *pChaosRespawn = g_pMain->m_ChaosStoneRespawnCoordinateArray.GetData(i);
			if (pChaosRespawn == nullptr
				|| pChaosRespawn->sRank != 1)
				continue;

			ChaosStoneResCount++;
			if (ChaosStoneResCount > 3)
				return false;

			_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(ChaosStoneResCount);
			if (pChaosInfo == nullptr)
				return false;

			pChaosInfo->sChaosID = pChaosRespawn->sChaosID;
			pChaosInfo->sRank = pChaosRespawn->sRank;
			pChaosInfo->sZoneID = pChaosRespawn->sZoneID;
			pChaosInfo->sSpawnTime = pChaosRespawn->sSpawnTime * MINUTE;
			pChaosInfo->sMonsterFamily = 1;
			pChaosInfo->isChaosStoneKilled = false;
			pChaosInfo->isOnResTimer = false;
			pChaosInfo->isTotalKilledMonster = false;
			pChaosRespawn->isOpen == 1 ? pChaosInfo->ChaosStoneON = true : pChaosInfo->ChaosStoneON = false;

			if (!pChaosInfo->ChaosStoneON)
				continue;

			g_pMain->SpawnEventNpc(pChaosRespawn->sChaosID, true, (uint8)pChaosRespawn->sZoneID, pChaosRespawn->sSpawnX, 0, pChaosRespawn->sSpawnZ, pChaosRespawn->sCount, pChaosRespawn->sRadiusRange, 0, 0, -1, 0, (uint8)pChaosRespawn->sDirection, 0, 0, 0, 0);
		}
		g_pMain->ChaosStoneRespawnOkey = false;
	}
	return true;
}
#pragma endregion

#pragma region CGameServerDlg::ChaosStoneRespawnTimer()
void CGameServerDlg::ChaosStoneRespawnTimer()
{
	if (g_pMain->m_ChaosStoneInfoArray.GetSize() > 0)
	{
		for (int i = 1; i <= g_pMain->m_ChaosStoneInfoArray.GetSize(); i++)
		{
			_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(i);
			if (pChaosInfo == nullptr
				|| !pChaosInfo->ChaosStoneON
				|| !pChaosInfo->isOnResTimer
				|| !pChaosInfo->isChaosStoneKilled
				|| !pChaosInfo->isTotalKilledMonster)
				continue;

			if (pChaosInfo->sSpawnTime > 0)
				pChaosInfo->sSpawnTime--;

			if (pChaosInfo->sSpawnTime <= 0)
			{
				ChaosStoneSummon(pChaosInfo->sChaosID, pChaosInfo->sRank, pChaosInfo->sZoneID);
				pChaosInfo->isChaosStoneKilled = false;
				pChaosInfo->isOnResTimer = false;
				pChaosInfo->isTotalKilledMonster = false;
				pChaosInfo->sBoosKilledCount = 0;

				std::string m_ChaosStoneZoneName;

				if (pChaosInfo->sZoneID == 71)
					m_ChaosStoneZoneName = "Ronark Land";
				else if (pChaosInfo->sZoneID == 72)
					m_ChaosStoneZoneName = "Ardream";
				else if (pChaosInfo->sZoneID == 73)
					m_ChaosStoneZoneName = "Ronark Land Base";

				std::string ChaosStoneRespawnNotice = string_format("Chaos Stone was respawned at %s and it will summon bosses upon the knights once it is destroyed!", m_ChaosStoneZoneName.c_str());
				g_pMain->SendNotice(ChaosStoneRespawnNotice.c_str(), Nation::ALL);
				g_pMain->SendAnnouncement(ChaosStoneRespawnNotice.c_str(), Nation::ALL);
			}
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::ChaosStoneSummonSelectStage(uint16 ChaosGetID,uint8 RankID, uint16 ZoneID)
uint8 CGameServerDlg::ChaosStoneSummonSelectStage(uint16 ChaosGetID, uint8 RankID, uint16 ZoneID)
{
	for (int i = 1; i <= g_pMain->m_ChaosStoneInfoArray.GetSize(); i++)
	{
		_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(i);
		if (pChaosInfo == nullptr)
			continue;

		if (pChaosInfo->sChaosID == ChaosGetID && pChaosInfo->sRank == RankID && pChaosInfo->sZoneID == ZoneID)
			return (uint8)pChaosInfo->sChaosIndex;
	}

	return 0;
}
#pragma endregion

#pragma region CGameServerDlg::ChaosStoneSummon(uint16 ChaosGetID,uint8 RankID, uint16 ZoneID)
void CGameServerDlg::ChaosStoneSummon(uint16 ChaosGetID, uint8 RankID, uint16 ZoneID)
{
	if (g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize() > 0)
	{
		for (int i = 1; i <= g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize(); i++)
		{
			_CHAOS_STONE_RESPAWN *pChaosRespawn = g_pMain->m_ChaosStoneRespawnCoordinateArray.GetData(i);
			if (pChaosRespawn == nullptr
				|| pChaosRespawn->sChaosID != ChaosGetID
				|| pChaosRespawn->sRank != RankID
				|| pChaosRespawn->sZoneID != ZoneID
				|| pChaosRespawn->isOpen == 0)
				continue;

			uint8 Info = ChaosStoneSummonSelectStage(pChaosRespawn->sChaosID, pChaosRespawn->sRank, pChaosRespawn->sZoneID);
			_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(Info);
			if (pChaosInfo != nullptr)
			{
				g_pMain->SpawnEventNpc(pChaosRespawn->sChaosID, true, (uint8)pChaosRespawn->sZoneID, pChaosRespawn->sSpawnX, 0, pChaosRespawn->sSpawnZ, pChaosRespawn->sCount, pChaosRespawn->sRadiusRange, 0, 0, -1, 0, (uint8)pChaosRespawn->sDirection, 0, 0, 0, 0);
				pChaosInfo->sSpawnTime = pChaosRespawn->sSpawnTime  * MINUTE;
			}
		}
	}
}
#pragma endregion

#pragma region CNpc::ChaosStoneSelectStage(uint8 sRank)
uint8 CNpc::ChaosStoneSelectStage(uint8 sRank)
{
	if (g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize() > 0)
	{
		for (int i = 1; i <= g_pMain->m_ChaosStoneRespawnCoordinateArray.GetSize(); i++)
		{
			_CHAOS_STONE_RESPAWN *pChaosRespawn = g_pMain->m_ChaosStoneRespawnCoordinateArray.GetData(i);
			if (pChaosRespawn == nullptr
				|| pChaosRespawn->isOpen == 0)
				continue;

			if (pChaosRespawn->sChaosID == GetProtoID() && pChaosRespawn->sRank == sRank && pChaosRespawn->sZoneID && GetZoneID())
				return (uint8)pChaosRespawn->sIndex;
		}
	}

	return 0;
}
#pragma endregion

#pragma region CNpc::ChaosStoneDeath(CUser *pUser)
void CNpc::ChaosStoneDeath(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	g_pMain->SendNotice<CHAOS_STONE_ENEMY_NOTICE>("", GetZoneID(), Nation::ALL);
	uint8 ChaosIndex = 0;

	if (g_pMain->m_ChaosStoneInfoArray.GetSize() > 0)
	{
		for (int i = 1; i <= g_pMain->m_ChaosStoneInfoArray.GetSize(); i++)
		{
			_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(i);
			if (pChaosInfo == nullptr
				|| !pChaosInfo->ChaosStoneON
				|| pChaosInfo->sZoneID != GetZoneID()
				|| GetProtoID() != pChaosInfo->sChaosID)
				continue;

			pChaosInfo->sRank++;

			if (pChaosInfo->sRank > 4)
				pChaosInfo->sRank = 1;

			pChaosInfo->isChaosStoneKilled = true;
			uint8 Respawn = ChaosStoneSelectStage(pChaosInfo->sRank);

			_CHAOS_STONE_RESPAWN* pChaosRespawn = g_pMain->m_ChaosStoneRespawnCoordinateArray.GetData(Respawn);
			if (pChaosRespawn != nullptr)
			{
				pChaosInfo->sSpawnTime = pChaosRespawn->sSpawnTime * MINUTE;
				pChaosInfo->sRank = pChaosRespawn->sRank;
			}
			else
			{
				uint8 Respawnisnull = ChaosStoneSelectStage(1);
				_CHAOS_STONE_RESPAWN* pChaosRespawn2 = g_pMain->m_ChaosStoneRespawnCoordinateArray.GetData(Respawnisnull);
				if (pChaosRespawn2 != nullptr)
				{
					pChaosInfo->sSpawnTime = pChaosRespawn2->sSpawnTime * MINUTE;
					pChaosInfo->sRank = pChaosRespawn2->sRank;
				}
				else
				{
					pChaosInfo->sSpawnTime = 30 * MINUTE;
					pChaosInfo->sRank = 1;
				}
			}
			pChaosInfo->isTotalKilledMonster = false;
			pChaosInfo->sBoosKilledCount = 0;
			pChaosInfo->isOnResTimer = true;
			ChaosIndex = pChaosInfo->sChaosIndex;
		}
		ChaosStoneDeathRespawnMonster(ChaosIndex);
	}
}
#pragma endregion

#pragma region CNpc::ChaosStoneDeathRespawnMonster(uint16 ChaosGetIndex)
void CNpc::ChaosStoneDeathRespawnMonster(uint8 ChaosGetIndex)
{
	_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(ChaosGetIndex);
	if (pChaosInfo == nullptr)
		return;

	if (g_pMain->m_ChaosStoneSummonListArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_ChaosStoneSummonListArray)
		{
			_CHAOS_STONE_SUMMON_LIST* pSummonList = itr->second;
			if (pSummonList == nullptr)
				continue;

			if (pSummonList->ZoneID == pChaosInfo->sZoneID 
				&& pSummonList->MonsterSpawnFamily == pChaosInfo->sMonsterFamily)
				g_pMain->SpawnEventNpc(pSummonList->sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), 1, CHAOS_STONE_MONSTER_RESPAWN_RADIUS, 0, GetNation(), GetID(), GetEventRoom(), 0, 1, 0,SpawnEventType::ChaosStoneSummon, uint8(ChaosGetIndex));
		}
		pChaosInfo->sMonsterFamily++;
		uint8 Family = g_pMain->ChaosStoneSummonSelectFamilyStage(pChaosInfo->sChaosIndex, pChaosInfo->sMonsterFamily, pChaosInfo->sZoneID);

		if (Family)
			pChaosInfo->sMonsterFamily = Family;
	}
}
#pragma endregion

#pragma region CGameServerDlg::ChaosStoneSummonSelectFamilyStage(uint8 ChaosGetID, uint8 FamilyID, uint16 ZoneID)
uint8 CGameServerDlg::ChaosStoneSummonSelectFamilyStage(uint8 ChaosGetID, uint8 FamilyID, uint16 ZoneID)
{
	_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(ChaosGetID);
	if (pChaosInfo != nullptr)
	{
		foreach_stlmap(itr, m_ChaosStoneStageArray)
		{
			_CHAOS_STONE_STAGE* pStageList = itr->second;
			if (pStageList == nullptr)
				continue;

			if (pStageList->ZoneID == ZoneID && pStageList->nIndexFamily == FamilyID)
				return pStageList->nIndexFamily;
		}
	}

	return 1;
}
#pragma endregion

#pragma region CNpc::ChaosStoneBossKilledBy()
void CNpc::ChaosStoneBossKilledBy()
{
	for (int i = 1; i <= g_pMain->m_ChaosStoneInfoArray.GetSize(); i++)
	{
		_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(i);
		if (pChaosInfo == nullptr
			|| pChaosInfo->sZoneID != GetZoneID()
			|| !pChaosInfo->isChaosStoneKilled
			|| !pChaosInfo->ChaosStoneON
			|| pChaosInfo->isTotalKilledMonster)
			continue;

		for (int b = 0; b <= 9; b++)
		{
			if (pChaosInfo->sBoosID[b] == GetID())
			{
				pChaosInfo->sBoosKilledCount--;
				pChaosInfo->sBoosID[b] = 0;

				if (pChaosInfo->sBoosKilledCount <= 0)
					pChaosInfo->isTotalKilledMonster = true;
				break;
			}
		}
	}
}
#pragma endregion

#pragma region CDBAgent::LoadChaosStoneFamilyStage()
bool CDBAgent::LoadChaosStoneFamilyStage()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());

	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_CHAOS_STONE_STAGE}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		_CHAOS_STONE_STAGE *pData = new _CHAOS_STONE_STAGE;

		dbCommand->FetchByte(1, pData->nIndex);
		dbCommand->FetchUInt16(2, pData->ZoneID);
		dbCommand->FetchByte(3, pData->nIndexFamily);

		if (!g_pMain->m_ChaosStoneStageArray.PutData(pData->nIndex, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion