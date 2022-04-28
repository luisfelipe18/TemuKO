#include "stdafx.h"
#include "Map.h"

void CUser::ZoneChange(uint16 sNewZone, float x, float z)
{
	C3DMap * pMap = g_pMain->GetZoneByID(sNewZone);
	_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);

	if (pMap == nullptr)
		return;

	WarpListResponse errorReason;

	if (!CanChangeZone(pMap, errorReason))
	{
		Packet result(WIZ_WARP_LIST, uint8(2));

		result << uint8(errorReason);

		if (errorReason == WarpListMinLevel)
			result << pMap->GetMinLevelReq();

		Send(&result);
		return;
	}

	if (x == 0.0f && z == 0.0f)
	{
		_START_POSITION * pStartPosition = g_pMain->GetStartPosition(sNewZone);
		if (pStartPosition != nullptr)
		{
			x = (float)(GetNation() == KARUS ? pStartPosition->sKarusX : pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX));
			z = (float)(GetNation() == KARUS ? pStartPosition->sKarusZ : pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ));
		}
	}

	m_bWarp = true;
	m_bZoneChangeFlag = true;

	// Random respawn position...
	if (sNewZone == ZONE_CHAOS_DUNGEON)
	{
		short sx, sz;
		GetStartPositionRandom(sx, sz, (uint8)sNewZone);
		x = (float)sx;
		z = (float)sz;
	}

	// Random respawn position...
	if (sNewZone == ZONE_KNIGHT_ROYALE)
	{
		short sx, sz;
		GetStartKnightRoyalePositionRandom(sx, sz);
		x = (float)sx;
		z = (float)sz;
	}

	m_LastX = x;
	m_LastZ = z;

	if (sNewZone == ZONE_DELOS)
	{
		if (pKnightSiege != nullptr)
		{
			if (pKnightSiege->sMasterKnights == GetClanID() && GetClanID())
			{
				if (GetNation() == KARUS)
				{
					x = (float)(455 + myrand(0, 5));
					z = (float)(790 + myrand(0, 5));
				}
				else
				{
					x = (float)(555 + myrand(0, 5));
					z = (float)(790 + myrand(0, 5));
				}
			}
		}
	}

	if (GetZoneID() != sNewZone)
	{
		if (sNewZone != ZONE_CHAOS_DUNGEON
			&& sNewZone != ZONE_JURAID_MOUNTAIN
			&& sNewZone != ZONE_BORDER_DEFENSE_WAR
			&& sNewZone != ZONE_DUNGEON_DEFENCE
			&& sNewZone != ZONE_DRAKI_TOWER
			&& sNewZone != ZONE_KNIGHT_ROYALE)
			UserInOut(INOUT_OUT);

		if (isInTempleQuestEventZone(GetZoneID()))
			m_bEventRoom = 0;

		if (isInGame())
			HandleSurroundingUserRegionUpdate();

		SetZoneAbilityChange(sNewZone);

		// Reset the user's anger gauge when leaving the zone
		// Unknown if this is official behaviour, but it's logical.
		if (GetAngerGauge() > 0)
			UpdateAngerGauge(0);

		m_bZoneChangeSameZone = false;
		m_bZoneChangeControl = true;

		/*
		Here we also send a clan packet with subopcode 0x16 (with a byte flag of 2) if war zone/Moradon
		or subopcode 0x17 (with nWarEnemyID) for all else
		*/
#if 0
		if (isInClan())
		{
			CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->bKnightsWarStarted)
			{
				Packet clanPacket(WIZ_KNIGHTS_PROCESS);
				if (pMap->isWarZone() || sNewZone == ZONE_MORADON)
					clanPacket << uint8(0x17) << uint8(2);
				else
					clanPacket << uint16(0x16) << uint16(0 /*nWarEnemyID*/);

				Send(&clanPacket);
			}
		}
#endif
		if (isInParty() && !m_bZoneChangeSameZone)
		{
			if (isPartyLeader())
			{
				_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
				if (pParty == nullptr)
					return;

				PartyLeaderPromote(pParty->uid[1]);
			}
		}

		PartyNemberRemove(GetSocketID());

		if (hasRival())
			RemoveRival();

		PetOnDeath();
		ResetWindows();
	}
	else
	{
		m_bWarp = false;
		Warp(uint16(x * 10), uint16(z * 10));
		PetHome(uint16(x * 10), 0, uint16(z * 10));
		return;
	}

	/*Event is Disband.*/
	switch (GetZoneID())
	{
	case ZONE_SNOW_BATTLE:
	{
		if (sNewZone != ZONE_SNOW_BATTLE)
			SetMaxHp(1);
	}
	break;
	case ZONE_BATTLE6:
	{
		if (sNewZone != ZONE_BATTLE6)
		{
			if (GetTowerID() != -1)
				TowerExitsFunciton();
		}
	}
	break;
	case ZONE_CHAOS_DUNGEON:
	{
		if (sNewZone != ZONE_CHAOS_DUNGEON)
		{
			if (g_pMain->pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS)
			{
				RobChaosSkillItems();
				SetMaxHp(1);
			}
			if (isEventUser())
				m_bEventRoom = 0;
		}
	}
	break;
	case ZONE_JURAID_MOUNTAIN:
	{
		if (sNewZone != ZONE_JURAID_MOUNTAIN)
		{
			if (isEventUser())
				m_bEventRoom = 0;
		}
	}
	break;
	case ZONE_BORDER_DEFENSE_WAR:
	{
		if (sNewZone != ZONE_BORDER_DEFENSE_WAR)
		{
			if (isEventUser())
				m_bEventRoom = 0;
		}
	}
	break;
	case ZONE_DUNGEON_DEFENCE:
	{
		if (sNewZone != ZONE_DUNGEON_DEFENCE)
		{
			DungeonDefenceRobItemSkills();
			SetMaxHp(1);
			m_bEventRoom = 0;
		}
	}
	break;
	case ZONE_DRAKI_TOWER:
	{
		if (sNewZone != ZONE_DRAKI_TOWER)
		{
			m_bEventRoom = 0;
		}
	}
	break;
	case ZONE_STONE1:
	case ZONE_STONE2:
	case ZONE_STONE3:
	{
		if (sNewZone != ZONE_STONE1 
			|| sNewZone != ZONE_STONE2 
			|| sNewZone != ZONE_STONE3)
		{
			if (isQuestEventUser())
				MonsterStoneExitProcess();
		}
	}
	break;
	case ZONE_FORGOTTEN_TEMPLE:
	{
		if (sNewZone != ZONE_FORGOTTEN_TEMPLE)
		{
			g_pMain->m_nForgettenTempleUsers.erase(std::remove(
				g_pMain->m_nForgettenTempleUsers.begin(),
				g_pMain->m_nForgettenTempleUsers.end(),
				GetSocketID()),
				g_pMain->m_nForgettenTempleUsers.end());
		}
	}
	break;
	case ZONE_UNDER_CASTLE:
	{
		if (sNewZone != ZONE_UNDER_CASTLE)
		{
			g_pMain->m_nUnderTheCastleUsers.erase(std::remove(
				g_pMain->m_nUnderTheCastleUsers.begin(),
				g_pMain->m_nUnderTheCastleUsers.end(),
				GetSocketID()),
				g_pMain->m_nUnderTheCastleUsers.end());
		}
	}
	break;
	case ZONE_KNIGHT_ROYALE:
	{
		if (sNewZone != ZONE_KNIGHT_ROYALE)
		{
			if (!g_pMain->isKnightRoyaleActive())
			{
				_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
				if (pKnightRoyaleInfo != nullptr)
				{
					_KNIGHT_ROYALE_STARTED_USER* pRoyaleUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
					if (pRoyaleUser == nullptr)
						KnightRoyaleLoqOutPacket();
				}
			}
		}
	}
	break;
	}

	/* Event is sign*/
	switch (sNewZone)
	{
	case ZONE_FORGOTTEN_TEMPLE:
		g_pMain->m_nForgettenTempleUsers.push_back(GetSocketID());
		break;
	case ZONE_UNDER_CASTLE:
		g_pMain->m_nUnderTheCastleUsers.push_back(GetSocketID());
		break;
	}
	
	m_bZone = (uint8)sNewZone; // this is 2 bytes to support the warp data loaded from SMDs. It should not go above a byte, however.
	SetPosition(x, 0.0f, z);
	m_pMap = pMap;

	if (g_pMain->m_nServerNo != pMap->m_nServerNo)
	{
		_ZONE_SERVERINFO *pInfo = g_pMain->m_ServerArray.GetData(pMap->m_nServerNo);
		if (pInfo == nullptr)
			return;

		UserDataSaveToAgent();

		m_bLogout = 2;	// server change flag
		SendServerChange(pInfo->strServerIP, 2);
		return;
	}

	SetRegion(GetNewRegionX(), GetNewRegionZ());

	Packet result(WIZ_ZONE_CHANGE, uint8(ZoneChangeTeleport));
	result << uint16(GetZoneID()) << GetSPosX() << GetSPosZ() << GetSPosY() << GetNation();
	Send(&result);

	if (!m_bZoneChangeSameZone)
	{
		m_sWhoKilledMe = -1;
		m_iLostExp = 0;
		m_bRegeneType = 0;
		m_tLastRegeneTime = 0;
		m_sBind = -1;
		InitType3();
		InitType4();
		CMagicProcess::CheckExpiredType9Skills(this, true);// Check for expired type 9/visibility skills
		SetUserAbility();

		// You don't need anymore 
		if (m_bNeedParty == 2)
		{
			g_pMain->m_SeekingPartyArrayLock.lock();
			foreach(itr, g_pMain->m_SeekingPartyArray)
			{
				if ((*itr) == nullptr)
					continue;

				if ((*itr)->m_sGetID == GetID())
				{
					(*itr)->m_bZone = uint8(sNewZone);
					break;
				}
			}
			g_pMain->m_SeekingPartyArrayLock.unlock();
		}
	}

	g_pMain->TempleEventSendActiveEventTime(this);

	// Wanted Event Zone Change
	if (VanGuardStatus && sNewZone != ZONE_RONARK_LAND)
		g_pMain->VanGuardFinish(2);

	if (sNewZone == ZONE_DELOS)
		CastleSiegeWarFlag();

	m_bZoneChangeFlag = false;
}

/**
* @brief	Changes the zone of all party members within the user's zone.
* 			If the user is not in a party, they should still be teleported.
*
* @param	sNewZone	ID of the new zone.
* @param	x			The x coordinate.
* @param	z			The z coordinate.
*/
void CUser::ZoneChangeParty(uint16 sNewZone, float x, float z)
{
	_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return ZoneChange(sNewZone, x, z);

	short partyUsers[MAX_PARTY_USERS];

	for (int i = 0; i < MAX_PARTY_USERS; i++)
		partyUsers[i] = pParty->uid[i];

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(partyUsers[i]);
		if (pUser != nullptr)
			pUser->ZoneChange(sNewZone, x, z);
	}
}

/**
* @brief	Changes the zone of all clan members in home/neutral zones (including Eslant).
* 			If the user is not in a clan, they should not be teleported.
*
* @param	sNewZone	ID of the new zone.
* @param	x			The x coordinate.
* @param	z			The z coordinate.
*/
void CUser::ZoneChangeClan(uint16 sNewZone, float x, float z)
{
	CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr)
		return;

	foreach_stlmap(itr, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER * p = itr->second;
		if (p == nullptr)
			continue;

		CUser* pTUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		if (pTUser == nullptr
			|| !pTUser->isInGame()
			|| pTUser->isInTempleEventZone())
			continue;

		pTUser->ZoneChange(sNewZone, x, z);
	}
}

#pragma region CUser::Warp(uint16 sPosX, uint16 sPosZ)
void CUser::Warp(uint16 sPosX, uint16 sPosZ)
{
	if (GetMap() == nullptr)
		return;

	if (m_bWarp)
		return;

	float real_x = sPosX / 10.0f, real_z = sPosZ / 10.0f;
	if (!GetMap()->IsValidPosition(real_x, real_z, 0.0f))
	{
		TRACE("Invalid position %f,%f\n", real_x, real_z);
		return;
	}

	m_LastX = real_x;
	m_LastZ = real_z;

	Packet result(WIZ_WARP);
	result << sPosX << sPosZ;
	Send(&result);

	UserInOut(INOUT_OUT);

	m_curx = real_x;
	m_curz = real_z;
	curX1 = sPosX;
	curZ1 = sPosZ;
	m_oldwillx = sPosX;
	m_oldwillz = sPosZ;

	SetRegion(GetNewRegionX(), GetNewRegionZ());

	UserInOut(INOUT_WARP);
	g_pMain->UserInOutForMe(this);
	g_pMain->NpcInOutForMe(this);
	g_pMain->MerchantUserInOutForMe(this);

	ResetWindows();
}
#pragma endregion

#pragma region CUser::RecvWarp(Packet & pkt)
void CUser::RecvWarp(Packet & pkt)
{
	if (isDead() 
		|| !isGM())
		return;

	uint16 PosX, PosZ;
	pkt >> PosX >> PosZ;
	Warp(PosX, PosZ);
}
#pragma endregion

void CUser::RecvZoneChange(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();
	if (opcode == ZoneChangeLoading)
	{
		if (!m_bZoneChangeControl) // we also need to make sure we're actually waiting on this request...
			return;

		g_pMain->RegionUserInOutForMe(this);
		g_pMain->RegionNpcInfoForMe(this);
		g_pMain->MerchantUserInOutForMe(this);
		
		Packet result(WIZ_ZONE_CHANGE, uint8(ZoneChangeLoaded)); // finalise the zone change
		Send(&result);
	}
	else if (opcode == ZoneChangeLoaded)
	{
		if (!m_bZoneChangeControl) // we also need to make sure we're actually waiting on this request...
			return;

		UserInOut(INOUT_RESPAWN);

		// TODO: Fix all this up (it's too messy/confusing)
		if (!m_bZoneChangeSameZone)
		{
			if (GetZoneID() == ZONE_CHAOS_DUNGEON)
				BlinkStart(45);
			else if (!isNPCTransformation())
				BlinkStart();
		}

		if (GetZoneID() != ZONE_CHAOS_DUNGEON)
		{
			InitType4();
			RecastSavedMagic();
		}

		if (isDead())
			SendDeathAnimation();

		if (GetZoneID() == ZONE_DRAKI_TOWER)
			SendDrakiTempleDetail(true);

		if (GetZoneID() == ZONE_CLAN_WAR_ARDREAM
			|| GetZoneID() == ZONE_CLAN_WAR_RONARK
			|| GetZoneID() == ZONE_PARTY_VS_1
			|| GetZoneID() == ZONE_PARTY_VS_2
			|| GetZoneID() == ZONE_PARTY_VS_3
			|| GetZoneID() == ZONE_PARTY_VS_4)
			TournamentSendTimer();

		SetZoneAbilityChange((uint16)GetZoneID());

		/*if (CollectionRaceRegister)
			CollectionRaceEndProcess();*/
		
		m_bZoneChangeFlag = false;
		m_bWarp = false;
		m_bZoneChangeControl = false;
	}
	else if (opcode == ZoneMilitaryCamp)
	{
		uint16 ZoneID = pkt.read<uint8>();

		if (g_pMain->isWarOpen() 
			&& (isInLufersonCastle() 
				|| isInElmoradCastle()))
			return;

		ZoneChange(ZoneID, 0.0f, 0.0f);
	}
}

void CUser::Rotate(Packet & pkt)
{
	if (isDead())
		return;

	Packet result(WIZ_ROTATE);
	pkt >> m_sDirection;
	result << GetSocketID() << m_sDirection;
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, this, GetEventRoom());
	else
		SendToRegion(&result);
}

bool CUser::CanChangeZone(C3DMap * pTargetMap, WarpListResponse & errorReason)
{
	// While unofficial, game masters should be allowed to teleport anywhere.
	if (isGM())
		return true;

	CKnights* pClan = nullptr;

	// Generic error reason; this should only be checked when the method returns false.
	errorReason = WarpListGenericError;

	if (GetLevel() < pTargetMap->GetMinLevelReq() // Orjinal Kýsým
		|| (pTargetMap->GetID() == ZONE_ARDREAM && !g_pMain->OpenArdream)
		|| GetLevel() > pTargetMap->GetMinLevelReq()
		&& (pTargetMap->GetID() == ZONE_RONARK_LAND && !g_pMain->OpenCZ)
		|| GetLevel() > pTargetMap->GetMinLevelReq()
		&& (pTargetMap->GetID() == ZONE_RONARK_LAND_BASE && !g_pMain->OpenBaseLand))
	{
		errorReason = WarpListMinLevel;
		return false;
	}

	if (GetLevel() > pTargetMap->GetMaxLevelReq()
		&& (pTargetMap->GetID() == ZONE_ARDREAM && !g_pMain->OpenArdream)
		|| GetLevel() > pTargetMap->GetMaxLevelReq()
		&& (pTargetMap->GetID() == ZONE_RONARK_LAND && !g_pMain->OpenCZ)
		|| GetLevel() > pTargetMap->GetMaxLevelReq()
		&& (pTargetMap->GetID() == ZONE_RONARK_LAND_BASE && !g_pMain->OpenBaseLand))
	{
		errorReason = WarpListDoNotQualify;
		return false;
	}

	switch (pTargetMap->GetID())
	{
	case ZONE_KARUS:
	case ZONE_KARUS2:
	case ZONE_KARUS3:
		// Users may enter Luferson (1)/El Morad (2) if they are that nation, 
		if (GetNation() == KARUS && pTargetMap->GetID() >= ZONE_KARUS && pTargetMap->GetID() <= ZONE_KARUS3)
			return true;
		// Users may also enter if there's a war invasion happening in that zone.
		if (GetNation() == ELMORAD)
			return g_pMain->m_byKarusOpenFlag;
		else
			return g_pMain->m_byElmoradOpenFlag;
	case ZONE_ELMORAD:
	case ZONE_ELMORAD2:
	case ZONE_ELMORAD3:
		// Users may enter Luferson (1)/El Morad (2) if they are that nation, 
		if (GetNation() == ELMORAD && pTargetMap->GetID() >= ZONE_ELMORAD && pTargetMap->GetID() <= ZONE_ELMORAD3)
			return true;
		// Users may also enter if there's a war invasion happening in that zone.
		if (GetNation() == KARUS)
			return g_pMain->m_byElmoradOpenFlag;
		else
			return g_pMain->m_byKarusOpenFlag;
	case ZONE_KARUS_ESLANT:
	case ZONE_KARUS_ESLANT2:
	case ZONE_KARUS_ESLANT3:
		if (GetNation() == KARUS 
			&& pTargetMap->GetID() >= ZONE_KARUS_ESLANT 
			&& pTargetMap->GetID() <= ZONE_KARUS_ESLANT3)
			return true;
	case ZONE_ELMORAD_ESLANT:
	case ZONE_ELMORAD_ESLANT2:
	case ZONE_ELMORAD_ESLANT3:
		if (GetNation() == ELMORAD 
			&& pTargetMap->GetID() >= ZONE_ELMORAD_ESLANT 
			&& pTargetMap->GetID() <= ZONE_ELMORAD_ESLANT3)
			return true;
	case ZONE_DELOS: // TODO: implement CSW logic.
	{
		if (g_pMain->pCswEvent.Status == CswOperationStatus::BarrackCreated)
		{
			_DEATHMATCH_WAR_INFO* pKnightList = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
			if (pKnightList == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			if (pKnightSiege == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnights != nullptr)
			{
				if (GetClanID() == pKnights->GetID())
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}
			}

			CKnights *pKnights2 = g_pMain->GetClanPtr(GetClanID());
			if (pKnights2 == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}
		}
		if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
		{
			_DEATHMATCH_BARRACK_INFO* pData4 = g_pMain->m_KnightSiegeWarBarrackList.GetData(GetClanID());
			if (pData4 == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			_DEATHMATCH_WAR_INFO* pKnightList = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
			if (pKnightList == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			if (pKnightSiege == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnights != nullptr)
			{
				if (GetClanID() == pKnights->GetID())
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}
			}

			CKnights *pKnights2 = g_pMain->GetClanPtr(GetClanID());
			if (pKnights2 == nullptr)
				return false;
		}
		if (g_pMain->pCswEvent.Status == CswOperationStatus::Preparation)
		{
			_CASTELLAN_WAR_INFO* pKnightList2 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
			if (pKnightList2 == nullptr)
			{
				_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
				if (pKnightSiege == nullptr)
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}

				CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
				if (pKnights == nullptr)
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}

				if (GetClanID() != pKnights->GetID())
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}
			}
		}
		if (g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
		{
			_CASTELLAN_WAR_INFO* pKnightList2 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
			if (pKnightList2 == nullptr)
			{
				_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
				if (pKnightSiege == nullptr)
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}

				_CASTELLAN_OLD_CSW_WINNER* pOldClan = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(GetClanID());
				if (pOldClan == nullptr)
				{
					errorReason = WarpListNotDuringCSW;
					return false;
				}
			}
		}

		if (g_pMain->pCswEvent.Status == CswOperationStatus::BarrackCreated
			|| g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch
			|| g_pMain->pCswEvent.Status == CswOperationStatus::Preparation
			|| g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
		{
			CKnights * pKnight = g_pMain->GetClanPtr(GetClanID());
			if (pKnight == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			pClan = g_pMain->GetClanPtr(GetClanID());
			if (pClan == nullptr)
			{
				errorReason = WarpListNotDuringCSW;
				return false;
			}

			if (pClan->m_byGrade > 3
				|| pClan->m_byFlag < ClanTypePromoted)
			{
				if (pClan->m_byFlag >= ClanTypeAccredited5)
					goto fail_return;

				errorReason = WarpListDoNotQualify;
				return false;
			}
		}
	fail_return:
		if (GetLoyalty() <= 0)
		{
			errorReason = WarpListNeedNP;
			return false;
		}
	}return true;
	case ZONE_BIFROST:
		return true;
	case ZONE_ARDREAM:
		if (g_pMain->isWarOpen())
		{
			errorReason = WarpListNotDuringWar;
			return false;
		}

		if (GetLoyalty() <= 0)
		{
			errorReason = WarpListNeedNP;
			return false;
		}return true;
	case ZONE_RONARK_LAND_BASE:
		if (g_pMain->isWarOpen() && g_pMain->m_byBattleZoneType != ZONE_ARDREAM)
		{
			errorReason = WarpListNotDuringWar;
			return false;
		}

		if (GetLoyalty() <= 0)
		{
			errorReason = WarpListNeedNP;
			return false;
		}return true;
	case ZONE_RONARK_LAND:
		if (g_pMain->isWarOpen() && g_pMain->m_byBattleZoneType != ZONE_ARDREAM)
		{
			errorReason = WarpListNotDuringWar;
			return false;
		}

		if (GetLoyalty() <= 0)
		{
			errorReason = WarpListNeedNP;
			return false;
		}return true;
	default:
		// War zones may only be entered if that war zone is active.
		if (pTargetMap->isWarZone())
		{
			if (pTargetMap->GetID() == ZONE_SNOW_BATTLE)
			{
				if ((pTargetMap->GetID() - ZONE_SNOW_BATTLE) != g_pMain->m_byBattleZone)
					return false;
			}
			else if ((pTargetMap->GetID() - ZONE_BATTLE_BASE) != g_pMain->m_byBattleZone)
				return false;
			else if ((GetNation() == ELMORAD && g_pMain->m_byElmoradOpenFlag)
				|| (GetNation() == KARUS && g_pMain->m_byKarusOpenFlag))
				return false;
		}
	}return true;
}

bool CUser::CanLevelQualify(uint8 sLevel)
{
	int16 nStatTotal = 300 + (sLevel - 1) * 3;
	uint8 nSkillTotal = (sLevel - 9) * 2;

	if (sLevel > 60)
		nStatTotal += 2 * (sLevel - 60);

	if ((m_sPoints + GetStatTotal()) > nStatTotal || GetTotalSkillPoints() > nSkillTotal)
		return false;

	return true;
}