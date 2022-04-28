#include "stdafx.h"
#include "NpcThread.h"
#include "MagicInstance.h"
#include "Npc.h"

#pragma region CNpcThread::AddRequest(Signal * s)

void CNpcThread::AddRequest(Signal * s)
{
	signalqueue_lock.lock();
	signal_queue.push(s);
	signalqueue_lock.unlock();
	s_hEvent.Signal();
}

#pragma endregion

#pragma region CNpcThread::Engine()
/**
*
*/
void CNpcThread::Engine()
{
	while (!g_pMain->g_bNpcExit)
	{
		Signal *s = nullptr;

		// Pull the next packet from the shared queue
		signalqueue_lock.lock();
		if (signal_queue.size())
		{
			//if(signal_queue.size() > 20)
			//	printf("NPC Queue Size = %d, Zone = %d\n", signal_queue.size(), m_sZoneID);
			s = signal_queue.front();
			signal_queue.pop();
		}
		signalqueue_lock.unlock();

		// If there's no more packets to handle, wait until there are.
		if (s == nullptr)
		{
			// If we're shutting down, don't bother waiting for more (there are no more).
			if (!_running)
				break;

			s_hEvent.Wait();
			continue;
		}

		State_Null(*s);

		// Free the packet.
		delete s;
	}
}
#pragma endregion

#pragma region CNpcThread::State_Null(Signal &input)

void CNpcThread::State_Null(Signal &input)
{
	switch (input.signal_type)
	{
	case NpcThreadSignalType::NpcHandle:
		HandleNPC();
		break;
	case NpcThreadSignalType::NpcAdd:
		if (input.addNPCSignal)
			AddNPC(input.addNPCSignal);
		break;
	case NpcThreadSignalType::PetAdd:
		if (input.addPETSignal)
			AddPET(input.addPETSignal);
		break;
	case NpcThreadSignalType::NpcRemove:
		if (input.removeNPCSignal)
			RemoveNPC(input.removeNPCSignal->m_sNpcID);
		break;
	case NpcThreadSignalType::NpcAllLoad:
		LoadAllObjects();
		LoadNPCs();
		break;
	case NpcThreadSignalType::NpcAllRemove:
		RemoveAllNPCs();
		break;
	case NpcThreadSignalType::NpcAllReset:
		ResetAllNPCs();
		break;
	case NpcThreadSignalType::NpcAllChangeAbility:
		if (input.changeNPCAbilitySignal)
			ChangeAbilityAllNPCs(input.changeNPCAbilitySignal->m_byType);
		break;
	case NpcThreadSignalType::HandleJuraidMountainNpcKill:
		if (input.juraidKillSignal)
			HandleJuraidKill(input.juraidKillSignal->m_sNpcID, input.juraidKillSignal->m_sKillerID, input.juraidKillSignal->m_bNation);
		break;
	case NpcThreadSignalType::HandleGateOpenFlag:
		if (input.GateOpenSignal)
			HandleGateFlagOpen(input.GateOpenSignal->m_sNpcID, input.GateOpenSignal->m_stType, input.GateOpenSignal->m_sByGateOpen);
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region CNpcThread::HandleNPC()
/**
* @brief	Handles All NPCs.
*
*/
void CNpcThread::HandleNPC()
{
	CNpc *pNpc = nullptr;
	int	i = 0;
	time_t dwDiffTime = 0, dwTickTime = 0, fTime2 = 0;
	int duration_damage = 0;

	fTime2 = getMSTime(); // the current time
	foreach_stlmap(itr, m_arNpcArray)
	{
		// get the NPC's pointer
		pNpc = itr->second;

		// if it's nullptr, then we should continue;
		if (pNpc == nullptr)
			continue;

		if (pNpc->m_bDelete == true
			&& pNpc->m_bDeleteRequested == false
			&& (pNpc->m_tDeleteTime > 0 && uint32(UNIXTIME) - pNpc->m_tDeleteTime > 60))
		{
			pNpc->m_bDeleteRequested = true;
			AddRequest(new Signal(new RemoveNPCSignals(pNpc->GetID())));
			continue;
		}

		dwTickTime = fTime2 - pNpc->m_fDelayTime;

		if (pNpc->m_bForceReset == true)
		{
			if (pNpc->GetNpcState() != NPC_DEAD)
				pNpc->m_bForceReset = false;
		}
		else
		{
			if (pNpc->m_Delay > dwTickTime && !pNpc->m_bFirstLive && pNpc->m_Delay != 0)
			{
				if (pNpc->m_Delay < 0)
					pNpc->m_Delay = 0;

				if (pNpc->GetNpcState() == NPC_STANDING
					&& pNpc->CheckFindEnemy()
					&& pNpc->FindEnemy())
				{
					pNpc->StateChange(NPC_ATTACKING);
					pNpc->m_Delay = 0;
				}
				continue;
			}
		}

		dwTickTime = fTime2 - pNpc->m_fHPChangeTime;
		if (10 * SECOND < dwTickTime)
			pNpc->HpChange();

		if (1 * SECOND < dwTickTime)
		{
			switch (pNpc->GetType())
			{
			case NPC_SOCCER_BAAL:
			{
				if (!g_pMain->pSoccerEvent.isSoccerAktive())
					continue;

				if (pNpc->isInSoccerEvent() >= TeamColourBlue
					&& pNpc->isInSoccerEvent() <= TeamColourOutside)
				{
					float x, z;
					pNpc->SendInOut(INOUT_OUT, pNpc->GetX(), pNpc->GetZ(), 0.0f);
					x = 672.0f; z = 160.0f;
					pNpc->SendInOut(INOUT_IN, x, z, 0.0f);
					break;
				}
			}
			break;
			default:
				//pNpc->Update();
				break;
			}
		}

		uint8 bState = pNpc->GetNpcState();
		time_t tDelay = -1;
		switch (bState)
		{
		case NPC_LIVE:
			tDelay = pNpc->NpcLive();
			break;

		case NPC_HPCHANGE:
			tDelay = pNpc->HpChangeReq();
			break;

		case NPC_STANDING:
			tDelay = pNpc->NpcStanding();
			break;

		case NPC_MOVING:
			tDelay = pNpc->NpcMoving();
			break;

		case NPC_ATTACKING:
			tDelay = pNpc->NpcAttacking();
			break;

		case NPC_TRACING:
			tDelay = pNpc->NpcTracing();
			break;

		case NPC_FIGHTING:
			tDelay = pNpc->Attack();
			break;

		case NPC_BACK:
			tDelay = pNpc->NpcBack();
			break;

		case NPC_STRATEGY:
			break;

		case NPC_DEAD:
			pNpc->Dead(pNpc->pKiller, pNpc->m_bSendDeathPacket);
			pNpc->StateChange(NPC_LIVE);
			break;

		case NPC_SLEEPING:
			tDelay = pNpc->NpcSleeping();
			break;

		case NPC_FAINTING:
			tDelay = pNpc->NpcFainting();
			break;

		case NPC_HEALING:
			tDelay = pNpc->NpcHealing();
			break;

		case NPC_CASTING:
			tDelay = pNpc->NpcCasting();
			break;
		}

		// This may not be necessary, but it keeps behaviour identical.
		if (bState != NPC_LIVE && bState != NPC_DEAD
			&& pNpc->GetNpcState() != NPC_DEAD)
			pNpc->m_fDelayTime = getMSTime();

		if (tDelay >= 0)
			pNpc->m_Delay = tDelay;

		time_t dwTickTimeType4 = fTime2 - pNpc->m_fHPType4CheckTime;
		if (500 < dwTickTimeType4 && pNpc->isAlive())	 // 10 seconds
		{
			pNpc->HPTimeChangeType3();
			pNpc->Type4Duration();
			pNpc->m_fHPType4CheckTime = getMSTime();
		}


		if (pNpc->isAlive()
			&& pNpc->m_sDuration > 0
			&& (int32(UNIXTIME) - pNpc->m_iSpawnedTime >  pNpc->m_sDuration))
			pNpc->DeadReq();
	}
}
#pragma endregion

#pragma region CNpcThread::AddNPC(AddNPCSignals* addNPCSignal)
/**
* @brief	Adds NPC to the handling list.
*/
void CNpcThread::AddNPC(AddNPCSignals* addNPCSignal)
{
	if (addNPCSignal == nullptr)
		return;

	for (int i = 0; i < addNPCSignal->m_sCount; i++)
	{
		int16 minRange = -(addNPCSignal->m_sRadius / 2);

		float fX_temp = addNPCSignal->m_fX, fZ_temp = addNPCSignal->m_fZ;

		addNPCSignal->m_fX = fX_temp + (float)(myrand(minRange, addNPCSignal->m_sRadius));
		addNPCSignal->m_fZ = fZ_temp + (float)(myrand(minRange, addNPCSignal->m_sRadius));
		CNpcTable * proto = nullptr;
		KOMap * pZone = g_pMain->GetZoneByID(addNPCSignal->m_byZone);

		if (pZone == nullptr)
			return;

		if (pZone->m_Status == 0)
			return;

		if (addNPCSignal->m_bIsMonster)
			proto = g_pMain->m_arMonTable.GetData(addNPCSignal->m_sSid);
		else
			proto = g_pMain->m_arNpcTable.GetData(addNPCSignal->m_sSid);

		if (proto == nullptr)
			return;

		CNpc * pNpc = new CNpc();

		pNpc->m_bIsEventNpc = addNPCSignal->m_bIsEvent;
		pNpc->m_byMoveType = (addNPCSignal->m_bMoveType > 0 ? addNPCSignal->m_bMoveType : (addNPCSignal->m_bIsMonster ? 1 : 0));

		pNpc->m_byInitMoveType = (addNPCSignal->m_bMoveType > 0 ? addNPCSignal->m_bMoveType : (addNPCSignal->m_bIsMonster ? 1 : 0));
		pNpc->m_byBattlePos = 0;

		pNpc->m_bZone = addNPCSignal->m_byZone;

		pNpc->SetPosition(addNPCSignal->m_fX, addNPCSignal->m_fY, addNPCSignal->m_fZ);
		pNpc->m_nInitMinX = pNpc->m_nLimitMinX = int(addNPCSignal->m_fX);
		pNpc->m_nInitMinY = pNpc->m_nLimitMinZ = int(addNPCSignal->m_fZ);
		pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX = int(addNPCSignal->m_fX);
		pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ = int(addNPCSignal->m_fZ);
		pNpc->m_pMap = pZone;

		pNpc->m_bEventRoom = addNPCSignal->m_nEventRoom;
		pNpc->m_sDuration = addNPCSignal->m_sDuration;

		if (addNPCSignal->m_byDirection > 0) pNpc->m_byDirection = uint8(addNPCSignal->m_byDirection);

		uint32 sNpcID = g_pMain->GetFreeID();
		if (sNpcID == uint32(-1))
		{
			printf("[AddNPC Fail] No Available ID | ID = %d Zone = %d\n", pNpc->GetID(), addNPCSignal->m_byZone);
			delete pNpc;
			return;
		}

		pNpc->Load(sNpcID, proto, addNPCSignal->m_bIsMonster, addNPCSignal->m_nation);
		pNpc->InitPos();

		if (pNpc->m_bIsEventNpc)
			pNpc->m_sRegenTime = 0;

		pNpc->m_oSocketID = addNPCSignal->m_socketID;
		pNpc->m_byGateOpen = addNPCSignal->m_bGateOpen;

		if (pNpc->GetZoneID() == ZONE_DELOS)
		{
			if (pNpc->GetProtoID() == 511)
			{
				CUser * pUser = g_pMain->GetUserPtr(pNpc->m_oSocketID);
				if (pUser == nullptr)
				{
					delete pNpc;
					return;
				}
				else
				{
					if (!pUser->isInClan())
					{
						delete pNpc;
						return;
					}
					pNpc->m_oBarrackID = pUser->GetClanID();
				}
			}
			pNpc->m_oSocketID = -1;
		}

		switch (addNPCSignal->m_nSummonSpecialType)
		{
		case SpawnEventType::UnderTheCastleSummon:
		{
			if (pNpc->GetZoneID() == ZONE_UNDER_CASTLE)
			{
				_UNDER_THE_CASTLE_INFO* pRoomInfo = g_pMain->m_MonsterUnderTheCastleList.GetData(1);
				if (pRoomInfo != nullptr)
				{
					if (addNPCSignal->m_nSummonSpecialID == 1)
						pRoomInfo->m_sUtcGateID[0] = pNpc->GetID();
					else if (addNPCSignal->m_nSummonSpecialID == 2)
						pRoomInfo->m_sUtcGateID[1] = pNpc->GetID();
					else if (addNPCSignal->m_nSummonSpecialID == 3)
						pRoomInfo->m_sUtcGateID[2] = pNpc->GetID();
				}
			}
		}
		break;
		case SpawnEventType::ChaosStoneSummon:
		{
			if (pNpc->GetZoneID() == ZONE_RONARK_LAND
				|| pNpc->GetZoneID() == ZONE_RONARK_LAND_BASE
				|| pNpc->GetZoneID() == ZONE_ARDREAM)
			{
				if (addNPCSignal->m_nSummonSpecialID > 0)
				{
					_CHAOS_STONE_INFO* pChaosInfo = g_pMain->m_ChaosStoneInfoArray.GetData(addNPCSignal->m_nSummonSpecialID);
					if (pChaosInfo != nullptr)
					{
						if (pNpc->GetZoneID() == pChaosInfo->sZoneID && pChaosInfo->isChaosStoneKilled == true && pNpc->isMonster())
						{
							if (pChaosInfo->sBoosID[0] == 0)
								pChaosInfo->sBoosID[0] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[1] == 0)
								pChaosInfo->sBoosID[1] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[2] == 0)
								pChaosInfo->sBoosID[2] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[3] == 0)
								pChaosInfo->sBoosID[3] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[4] == 0)
								pChaosInfo->sBoosID[4] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[5] == 0)
								pChaosInfo->sBoosID[5] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[6] == 0)
								pChaosInfo->sBoosID[6] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[7] == 0)
								pChaosInfo->sBoosID[7] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[8] == 0)
								pChaosInfo->sBoosID[8] = pNpc->GetID();
							else if (pChaosInfo->sBoosID[9] == 0)
								pChaosInfo->sBoosID[9] = pNpc->GetID();

							pChaosInfo->sBoosKilledCount++;
						}
					}
				}
			}
		}
		break;
		case SpawnEventType::StableSummonSummon:
		{
			if (g_pMain->m_MonsterStableSignRespawnListArray.GetSize() != 0)
			{
				_MONSTER_RESPAWN_STABLE_SIGN_LIST* pStableSignList = g_pMain->m_MonsterStableSignRespawnListArray.GetData(addNPCSignal->m_nSummonSpecialID);
				if (pStableSignList != nullptr)
				{
					if (pStableSignList->GetZoneID == pNpc->GetZoneID())
					{
						pNpc->m_byTrapNumber = pStableSignList->GetTrapNumber;
						pStableSignList->CurrentGetID = pNpc->GetID();
						pStableSignList->CurrentSsid = pNpc->GetProtoID();
					}
				}
			}
		}
		break;
		case SpawnEventType::DungeonDefencSummon:
		{
			if (pNpc->GetZoneID() == ZONE_DUNGEON_DEFENCE)
			{
				_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(addNPCSignal->m_nSummonSpecialID);
				if (pRoomInfo != nullptr)
				{
					if (pRoomInfo->m_DefenceisStarted == true)
					{
						if (pNpc->isMonster())
							pRoomInfo->m_DefenceKillCount++;
					}
				}
			}
		}
		break;
		case SpawnEventType::DrakiTowerSummon:
		{
			if (pNpc->GetZoneID() == ZONE_DRAKI_TOWER)
			{
				_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(addNPCSignal->m_nSummonSpecialID);

				if (pRoomInfo != nullptr)
				{
					if (pRoomInfo->m_tDrakiTowerStart == true)
					{
						if (pNpc->isMonster())
							pRoomInfo->m_bDrakiMonsterKill++;
					}
				}
			}
		}
		break;
		}

		if (!m_arNpcArray.PutData(pNpc->GetID(), pNpc))
		{
			printf("[AddNPC Fail] Cannot PutData | ID = %d Zone = %d\n", pNpc->GetID(), addNPCSignal->m_byZone);
			delete pNpc;
			return;
		}
		++g_pMain->m_TotalNPC;
	}
}
#pragma endregion

#pragma region CNpcThread::AddPET(uint16 sProtoID, CNpc* pNpc)
/**
* @brief	Adds PET to the handling list.
*/
void CNpcThread::AddPET(AddPETSignals* addPETSignal)
{
	if (addPETSignal == nullptr)
		return;

	for (int i = 0; i < addPETSignal->m_sCount; i++)
	{
		int16 minRange = -(addPETSignal->m_sRadius / 2);

		float fX_temp = addPETSignal->m_fX, fZ_temp = addPETSignal->m_fZ;

		addPETSignal->m_fX = fX_temp + (float)(myrand(minRange, addPETSignal->m_sRadius));
		addPETSignal->m_fZ = fZ_temp + (float)(myrand(minRange, addPETSignal->m_sRadius));
		CNpcTable * proto = nullptr;
		KOMap * pZone = g_pMain->GetZoneByID(addPETSignal->m_byZone);

		if (pZone == nullptr)
			return;

		if (pZone->m_Status == 0)
			return;

		if (addPETSignal->m_bIsMonster)
			proto = g_pMain->m_arMonTable.GetData(addPETSignal->m_sSid);
		else
			proto = g_pMain->m_arNpcTable.GetData(addPETSignal->m_sSid);

		if (proto == nullptr)
			return;

		CNpc * pNpc = new CNpc();

		pNpc->m_bIsEventNpc = addPETSignal->m_bIsEvent;
		pNpc->m_byMoveType = (addPETSignal->m_bIsMonster ? 1 : 0);

		pNpc->m_byInitMoveType = 1;
		pNpc->m_byBattlePos = 0;

		pNpc->m_bZone = addPETSignal->m_byZone;

		pNpc->SetPosition(addPETSignal->m_fX, addPETSignal->m_fY, addPETSignal->m_fZ);
		pNpc->m_nInitMinX = pNpc->m_nLimitMinX = int(addPETSignal->m_fX);
		pNpc->m_nInitMinY = pNpc->m_nLimitMinZ = int(addPETSignal->m_fZ);
		pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX = int(addPETSignal->m_fX);
		pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ = int(addPETSignal->m_fZ);
		pNpc->m_pMap = pZone;

		pNpc->m_bEventRoom = addPETSignal->m_nEventRoom;
		pNpc->m_sDuration = addPETSignal->m_sDuration;

		pNpc->m_byDirection = 0;

		uint32 sNpcID = g_pMain->GetFreeID();
		if (sNpcID == uint32(-1))
		{
			printf("[AddPET Fail] No Available ID | ID = %d Zone = %d\n", pNpc->GetID(), addPETSignal->m_byZone);
			delete pNpc;
			return;
		}

		pNpc->Load(sNpcID, proto, addPETSignal->m_bIsMonster, addPETSignal->m_nation);
		pNpc->InitPos();

		if (pNpc->m_bIsEventNpc)
			pNpc->m_sRegenTime = 0;

		pNpc->m_oSocketID = addPETSignal->m_socketID;
		pNpc->m_byGateOpen = 0;

		if (addPETSignal->m_Type > 0)
		{
			enum OpCodes
			{
				PetHandler = 1,
				GuardHandler = 2
			};

			if (addPETSignal->m_Type == PetHandler)
			{
				CUser * pUser = g_pMain->GetUserPtr(addPETSignal->m_socketID);
				if (pUser == nullptr)
				{
					delete pNpc;
					return;
				}

				if (pUser->m_PettingOn == nullptr)
				{
					delete pNpc;
					return;
				}
				pNpc->m_sPetId = addPETSignal->m_socketID;
				pNpc->m_strPetName = pUser->m_PettingOn->strPetName;
				pNpc->m_strUserName = pUser->m_strUserID;
				pNpc->m_iHP = pUser->m_PettingOn->sHP;
				pNpc->m_MaxHP = pUser->m_PettingOn->info->PetMaxHP;
				pNpc->m_sMP = pUser->m_PettingOn->sMP;
				pNpc->m_MaxMP = pUser->m_PettingOn->info->PetMaxMP;
				pNpc->m_bLevel = pUser->m_PettingOn->bLevel;
				pNpc->m_sAttack = pUser->m_PettingOn->info->PetAttack;
				pNpc->m_sTotalAc = pUser->m_PettingOn->info->PetDefense;
				pNpc->m_sTotalAcTemp = pUser->m_PettingOn->info->PetDefense;
				pNpc->m_fTotalHitrate = pUser->m_PettingOn->info->PetRes;
				pNpc->m_fTotalEvasionrate = pUser->m_PettingOn->info->PetRes;
				pNpc->m_sTotalHit = pUser->m_PettingOn->info->PetAttack;
				pNpc->m_sPid = pUser->m_PettingOn->sPid;
				pNpc->m_sSize = pUser->m_PettingOn->sSize;
				pNpc->m_bNation = addPETSignal->m_nation;
				pUser->m_PettingOn->sNid = pNpc->GetID();

				pUser->m_PettingOn->sAttackStart = false;
				pUser->m_PettingOn->sAttackTargetID = -1;

				if (pUser->m_PettingOn->sSatisfaction <= 0)
					pUser->m_PettingOn->sSatisfaction = 500;

				if (addPETSignal->m_SkillID > 0)
				{
					Packet result;
					pUser->PetSpawnProcess();
					pUser->m_lastPetSatisfaction = UNIXTIME;
					MagicInstance instance;
					instance.sData[0] = 0;
					instance.sData[1] = 1;
					instance.sData[2] = 0;
					instance.sData[3] = -1;
					instance.sData[4] = pUser->m_PettingOn->sNid;
					instance.sData[5] = 0;
					instance.sData[6] = 0;

					instance.BuildSkillPacket(result, pUser->GetSocketID(), pUser->GetSocketID(), MAGIC_EFFECTING, addPETSignal->m_SkillID, instance.sData);
					pUser->Send(&result);
				}
			}
			if (addPETSignal->m_Type == GuardHandler)
				pNpc->m_sGuardID = addPETSignal->m_socketID;

			pNpc->m_oSocketID = -1;
		}
		
		if (!m_arNpcArray.PutData(pNpc->GetID(), pNpc))
		{
			printf("[AddPET Fail] Cannot PutData | ID = %d Zone = %d\n", pNpc->GetID(), addPETSignal->m_byZone);
			delete pNpc;
			return;
		}
		++g_pMain->m_TotalNPC;
	}
}
#pragma endregion

#pragma region CNpcThread::RemoveNPC(uint32 sNpcID)
/**
* @brief	Removes NPC from the handling list.
*/
void CNpcThread::RemoveNPC(uint32 sNpcID)
{
	g_pMain->GiveIDBack(sNpcID);
	m_arNpcArray.DeleteData(sNpcID);
}
#pragma endregion

void CNpcThread::LoadAllObjects()
{
	foreach_stlmap(itr, g_pMain->m_ObjectEventArray)
	{
		if (itr->second->sZoneID == m_sZoneID)
		{
			_OBJECT_EVENT * pEvent = itr->second;
			if (pEvent->sZoneID != m_sZoneID)
				continue;

			if (pEvent->sType == OBJECT_GATE
				|| pEvent->sType == OBJECT_GATE2
				|| pEvent->sType == OBJECT_GATE_LEVER
				|| pEvent->sType == OBJECT_ANVIL
				|| pEvent->sType == OBJECT_ARTIFACT
				|| pEvent->sType == OBJECT_WALL
				|| pEvent->sType == OBJECT_WOOD
				|| pEvent->sType == OBJECT_WOOD_LEVER
				|| pEvent->sType == OBJECT_EFECKT
				|| pEvent->sType == OBJECT_BIND
				|| pEvent->sType == OBJECT_POISONGAS
				|| pEvent->sType == OBJECT_KROWASGATE)
				AddObjectEventNpc(pEvent);
		}
	}

}

bool CNpcThread::AddObjectEventNpc(_OBJECT_EVENT* pEvent, uint16 nEventRoom /* = 0 */, bool isEventObject /* false */)
{
	C3DMap * pMap = g_pMain->GetZoneByID(m_sZoneID);
	if (pMap == nullptr)
		return false;

	int sSid = 0;

	if (pEvent->sType == OBJECT_GATE
		|| pEvent->sType == OBJECT_GATE2
		|| pEvent->sType == OBJECT_GATE_LEVER
		|| pEvent->sType == OBJECT_ANVIL
		|| pEvent->sType == OBJECT_ARTIFACT
		|| pEvent->sType == OBJECT_WALL
		|| pEvent->sType == OBJECT_WOOD
		|| pEvent->sType == OBJECT_WOOD_LEVER
		|| pEvent->sType == OBJECT_EFECKT
		|| pEvent->sType == OBJECT_BIND)
		sSid = pEvent->sIndex;
	else
		sSid = pEvent->sControlNpcID;

	if (sSid <= 0)
		return false;

	CNpcTable * pNpcTable = g_pMain->m_arNpcTable.GetData(sSid);
	if (pNpcTable == nullptr)
	{
		return false;
	}

	CNpc *pNpc = new CNpc();

	pNpc->m_bIsEventNpc = isEventObject;
	if (pEvent->sType != 5)
	{
		pNpc->m_byMoveType = 104;
		pNpc->m_byInitMoveType = 104;
	}
	else
	{
		pNpc->m_byMoveType = 0;
		pNpc->m_byInitMoveType = 0;
	}

	pNpc->m_byBattlePos = 0;

	pNpc->m_byObjectType = SPECIAL_OBJECT;
	pNpc->m_byGateOpen = uint8(pEvent->sStatus);
	pNpc->m_bZone = pMap->m_nZoneNumber;
	pNpc->m_bEventRoom = nEventRoom;
	pNpc->SetPosition(pEvent->fPosX, pEvent->fPosY, pEvent->fPosZ);

	pNpc->m_nInitMinX = (int)pEvent->fPosX - 1;
	pNpc->m_nInitMinY = (int)pEvent->fPosZ - 1;
	pNpc->m_nInitMaxX = (int)pEvent->fPosX + 1;
	pNpc->m_nInitMaxY = (int)pEvent->fPosZ + 1;

	uint32 sNpcID = g_pMain->GetFreeID();
	if (sNpcID == uint32(-1))
	{
		delete pNpc;
		return nullptr;
	}

	pNpc->Load(sNpcID, pNpcTable, false);

	pNpc->m_pMap = pMap;
	if (pNpc->GetMap() == nullptr
		|| !m_arNpcArray.PutData(pNpc->GetID(), pNpc))
	{
		delete pNpc;
		return false;
	}

	g_pMain->m_sMapEventNpc++;
	g_pMain->m_TotalNPC = g_pMain->m_sMapEventNpc;

	return true;
}


/*
* @brief	Loads the NPC information by gathering data from the K_NPCPOS
*			table.
*/
bool CNpcThread::LoadNPCs()
{
	static CNpcTable * pNpcTable = nullptr;
	static int nRandom = 0;
	static float fRandom_X = 0.0f, fRandom_Z = 0.0f;
	uint32 counter = g_pMain->m_TotalNPC;

	foreach_stlmap(itr, g_pMain->m_NpcPosArray)
	{
		_K_NPC_POS *pNpcPos = itr->second;
		if (pNpcPos == nullptr)
			return false;

		if (pNpcPos->bZoneID != m_sZoneID)
			continue;

		KOMap * pZone = g_pMain->GetZoneByID(pNpcPos->bZoneID);

		if (pZone == nullptr)
			continue;

		if (pZone->m_Status == 0)
			continue;

		uint8 bPathSerial = 1;
		for (uint8 j = 0; j < pNpcPos->bNumNpc; j++)
		{
			CNpc * pNpc = new CNpc();

			pNpc->m_byMoveType = pNpcPos->bActType;
			pNpc->m_byInitMoveType = pNpcPos->bActType;

			bool bMonster = (pNpcPos->bActType < 100);
			if (bMonster)
			{
				pNpcTable = g_pMain->m_arMonTable.GetData(pNpcPos->sSid);
			}
			else
			{
				pNpc->m_byMoveType = pNpcPos->bActType - 100;
				pNpcTable = g_pMain->m_arNpcTable.GetData(pNpcPos->sSid);
			}

			if (pNpcTable == nullptr)
			{
				printf("NPC %d not found in %s table.\n", pNpcPos->sSid, bMonster ? "K_MONSTER" : "K_NPC");
				delete pNpc;
				continue;
			}

			uint32 sNpcID = g_pMain->GetFreeID();
			if (sNpcID == uint32(-1))
			{
				printf("Npc PutData Fail - ID = %d Zone = %d\n", pNpc->GetID(), pNpc->GetZoneID());
				delete pNpc;
				continue;
			}

			pNpc->Load(sNpcID, pNpcTable, bMonster);
			pNpc->m_byBattlePos = 0;

			if (pNpc->m_byMoveType >= 2)
			{
				pNpc->m_byBattlePos = myrand(1, 3);
				pNpc->m_byPathCount = bPathSerial++;
			}

			pNpc->InitPos();

			pNpc->m_bZone = pNpcPos->bZoneID;
			nRandom = abs(pNpcPos->iLeftX - pNpcPos->iRightX);
			if (nRandom <= 1)
				fRandom_X = (float)pNpcPos->iLeftX;
			else
			{
				if (pNpcPos->iLeftX < pNpcPos->iRightX)
					fRandom_X = (float)myrand(pNpcPos->iLeftX, pNpcPos->iRightX);
				else
					fRandom_X = (float)myrand(pNpcPos->iRightX, pNpcPos->iLeftX);
			}

			nRandom = abs(pNpcPos->iTopZ - pNpcPos->iBottomZ);
			if (nRandom <= 1)
				fRandom_Z = (float)pNpcPos->iTopZ;
			else
			{
				if (pNpcPos->iTopZ < pNpcPos->iBottomZ)
					fRandom_Z = (float)myrand(pNpcPos->iTopZ, pNpcPos->iBottomZ);
				else
					fRandom_Z = (float)myrand(pNpcPos->iBottomZ, pNpcPos->iTopZ);
			}

			pNpc->m_bEventRoom = pNpcPos->sRoom;
			pNpc->SetPosition(fRandom_X, 0.0f, fRandom_Z);

			pNpc->m_sRegenTime = pNpcPos->sRegTime * SECOND;
			pNpc->m_byDirection = pNpcPos->bDirection;
			pNpc->m_sMaxPathCount = pNpcPos->bDotCnt;

			if ((pNpc->m_byMoveType == 2 
				|| pNpc->m_byMoveType == 3 
				|| pNpc->m_byMoveType == 5) 
				&& pNpcPos->bDotCnt == 0)
			{
				pNpc->m_byMoveType = 1;
				TRACE("##### ServerDlg:CreateNpcThread - Path type Error :  nid=%d, sid=%d, name=%s, acttype=%d, path=%d #####\n",
					pNpc->GetID(), pNpc->GetProtoID(), pNpc->GetName().c_str(), pNpc->m_byMoveType, pNpc->m_sMaxPathCount);
			}

			if (pNpcPos->bDotCnt > 0)
			{
				int index = 0;
				for (int l = 0; l < pNpcPos->bDotCnt; l++)
				{
					static char szX[5], szZ[5];

					memset(szX, 0, sizeof(szX));
					memset(szZ, 0, sizeof(szZ));

					memcpy(szX, pNpcPos->szPath + index, 4);
					index += 4;

					memcpy(szZ, pNpcPos->szPath + index, 4);
					index += 4;

					pNpc->m_PathList.pPatternPos[l].x = atoi(szX);
					pNpc->m_PathList.pPatternPos[l].z = atoi(szZ);
				}
			}

			pNpc->m_nInitMinX = pNpc->m_nLimitMinX = pNpcPos->iLeftX;
			pNpc->m_nInitMinY = pNpc->m_nLimitMinZ = pNpcPos->iTopZ;
			pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX = pNpcPos->iRightX;
			pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ = pNpcPos->iBottomZ;

			// dungeon work
			pNpc->m_byDungeonFamily = pNpcPos->bDungeonFamily;
			pNpc->m_bySpecialType = (NpcSpecialType)pNpcPos->bSpecialType;
			pNpc->m_byRegenType = pNpcPos->bRegenType;
			pNpc->m_byTrapNumber = pNpcPos->bTrapNumber;

			if (pNpc->m_byDungeonFamily > 0)
			{
				pNpc->m_nLimitMinX = pNpcPos->iLimitMinX;
				pNpc->m_nLimitMinZ = pNpcPos->iLimitMinZ;
				pNpc->m_nLimitMaxX = pNpcPos->iLimitMaxX;
				pNpc->m_nLimitMaxZ = pNpcPos->iLimitMaxZ;
			}

			pNpc->m_pMap = g_pMain->GetZoneByID(pNpc->GetZoneID());
			if (pNpc->GetMap() == nullptr)
			{
				TRACE(_T("ERROR: NPC %d in zone %d that does not exist."), pNpcPos->sSid, pNpcPos->bZoneID);
				delete pNpc;
				continue;
			}

			if (pNpc->GetZoneID() == ZONE_JURAID_MOUNTAIN)
			{
				_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(pNpc->GetEventRoom());
				if (pRoomInfo != nullptr)
				{
					if (pNpc->m_byTrapNumber == 1)
						pRoomInfo->m_sKarusBridges[0] = pNpc->GetID();
					else if (pNpc->m_byTrapNumber == 2)
						pRoomInfo->m_sKarusBridges[1] = pNpc->GetID();
					else if (pNpc->m_byTrapNumber == 3)
						pRoomInfo->m_sKarusBridges[2] = pNpc->GetID();
					else if (pNpc->m_byTrapNumber == 4)
						pRoomInfo->m_sElmoBridges[0] = pNpc->GetID();
					else if (pNpc->m_byTrapNumber == 5)
						pRoomInfo->m_sElmoBridges[1] = pNpc->GetID();
					else if (pNpc->m_byTrapNumber == 6)
						pRoomInfo->m_sElmoBridges[2] = pNpc->GetID();
				}
			}

			if (g_pMain->m_MonsterStableRespawnListArray.GetSize() != 0)
			{
				_MONSTER_RESPAWN_STABLE_LIST* pMonsterRespawn = g_pMain->m_MonsterStableRespawnListArray.GetData(pNpc->GetProtoID());
				if (pMonsterRespawn != nullptr)
				{
					if (pNpc->m_byTrapNumber != 0)
					{
						_MONSTER_RESPAWN_STABLE_SIGN_LIST* pMonsterSýgn = g_pMain->m_MonsterStableSignRespawnListArray.GetData(pNpc->GetID());
						if (pMonsterSýgn == nullptr)
						{
							_MONSTER_RESPAWN_STABLE_SIGN_LIST* pMonsterSýgnNew = new _MONSTER_RESPAWN_STABLE_SIGN_LIST();
							pMonsterSýgnNew->GetID = pNpc->GetID();
							pMonsterSýgnNew->GetSsid = pNpc->GetProtoID();
							pMonsterSýgnNew->GetZoneID = pNpc->GetZoneID();
							pMonsterSýgnNew->CurrentSsid = pNpc->GetProtoID();
							pMonsterSýgnNew->GetX = pNpc->GetX();
							pMonsterSýgnNew->GetY = pNpc->GetY();
							pMonsterSýgnNew->GetZ = pNpc->GetZ();
							pMonsterSýgnNew->GetTrapNumber = pNpc->m_byTrapNumber;
							if (!g_pMain->m_MonsterStableSignRespawnListArray.PutData(pMonsterSýgnNew->GetID, pMonsterSýgnNew))
							{
								delete pMonsterSýgnNew;
								printf("Monster Respawn Stable Error \n");
							}
						}
					}
					else
						printf("Stable Load TrapNumber Error Name: %s GetID: %d Ssid: %d \n", pNpc->GetName().c_str(), pNpc->GetID(), pNpc->GetProtoID());
				}
			}

			if (!m_arNpcArray.PutData(pNpc->GetID(), pNpc))
			{
				TRACE("Npc PutData Fail - ID = %d Zone = %d\n", pNpc->GetID(), pNpc->GetZoneID());
				delete pNpc;
				continue;
			}

			++g_pMain->m_TotalNPC;
		}
	}

	counter = g_pMain->m_TotalNPC - counter;
	return true;
}


void CNpcThread::RemoveAllNPCs()
{
	foreach_stlmap(itr, m_arNpcArray)
	{
		CNpc *pNpc = itr->second;
		if (pNpc == nullptr)
			continue;

		if (!pNpc->m_bIsEventNpc)
			continue;

		pNpc->DeadReq();
	}
}

void CNpcThread::ResetAllNPCs()
{
	foreach_stlmap(itr, m_arNpcArray)
	{
		CNpc *pNpc = itr->second;
		if (pNpc == nullptr)
			continue;

		pNpc->m_bForceReset = true;

		if (pNpc->GetNpcState() == NPC_LIVE) // if its not dead then just full its hp.
			pNpc->HpChange(pNpc->GetMaxHealth(), nullptr);
	}
}

void CNpcThread::ChangeAbilityAllNPCs(uint8 bType)
{
	foreach_stlmap(itr, m_arNpcArray)
	{
		CNpc *pNpc = itr->second;
		if (pNpc == nullptr)
			continue;

		if (pNpc->GetType() > 10 && (pNpc->GetNation() == KARUS || pNpc->GetNation() == ELMORAD))
			pNpc->ChangeAbility(bType);
	}
}

#pragma region Construction & Desctruion methods

CNpcThread::CNpcThread(uint16 sZoneID) : m_sZoneID(sZoneID)
{
	_running = true;
	m_thread = std::thread(&CNpcThread::Engine, this);
}

CNpcThread::~CNpcThread()
{
	_running = false;
	//m_arNpcArray.DeleteAllData();
}


void CNpcThread::Shutdown()
{
	_running = false;

	// Wake them up in case they're sleeping.
	s_hEvent.Broadcast();
	Sleep(1000);
	m_arNpcArray.DeleteAllData();

	_running = false;

	// Wake them up in case they're sleeping.
	s_hEvent.Broadcast();
	signalqueue_lock.lock();
	while (signal_queue.size())
	{
		Signal *p = nullptr;
		p = signal_queue.front();
		signal_queue.pop();
		if (p)
			delete p;
	}
	signalqueue_lock.unlock();

	try
	{
		// Ensure it's started & hasn't exited yet.
		if (m_thread.joinable())
			m_thread.join();
	}
	catch (std::exception & ex)
	{
		printf("Caught thread exception: %s\n", ex.what());
	}
}
#pragma endregion

#pragma region CNpcThread::HandleJuraidKill(uint16 sNpcID, uint16 sKillerID, uint8 bNation)
/**
* @brief	Handles the Juraid Mountain event monster counts and opens gate if
*			the party succeeds.
* @param	pUser the User who killed the nNpc
* @param	the killed Npc
*/
void CNpcThread::HandleJuraidKill(uint16 sNpcID, uint16 sKillerID, uint8 bNation)
{
	CNpc* nNpc = m_arNpcArray.GetData(sNpcID);
	if (nNpc == nullptr
		|| (nNpc->GetProtoID() != LEECH_KING_SSID
			&& nNpc->GetProtoID() != KOCATRIS_SSID
			&& nNpc->GetProtoID() != LILIME_SSID
			&& nNpc->GetProtoID() != MINOTAUR_SSID
			&& nNpc->GetProtoID() != BONE_DRAGON_SSID
			&& nNpc->GetProtoID() != RED_DRAGON_HATCHLING_SSID
			&& nNpc->GetProtoID() != DEVA_BIRD_SSID
			&& nNpc->GetProtoID() != MONSTER_APOSTLE_SSID
			&& nNpc->GetProtoID() != MONSTER_DOOM_SOLDIER_SSID
			&& nNpc->GetProtoID() != MONSTER_TROLL_CAPTAIN_SSID))
		return;

	CUser* pUser = g_pMain->GetUserPtr(sKillerID);
	if (pUser == nullptr
		|| !pUser->isInGame()
		|| pUser->GetEventRoom() == 0
		|| !pUser->isInTempleEventZone())
		return;

	_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(pUser->GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	// we dont really need to handle other type of monsters for juraid
	if (nNpc->GetProtoID() == MONSTER_APOSTLE_SSID
		|| nNpc->GetProtoID() == MONSTER_DOOM_SOLDIER_SSID
		|| nNpc->GetProtoID() == MONSTER_TROLL_CAPTAIN_SSID)
	{
		if (bNation == Nation::KARUS)
		{
			pRoomInfo->m_iKarusSubMonsterKill++;
			uint32 mainMobKill = pRoomInfo->m_iKarusMainMonsterKill;
			uint8 subMobKill = pRoomInfo->m_iKarusSubMonsterKill;
			if (mainMobKill == 4 && subMobKill == 20)
				HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[0], Nation::KARUS);
			else if (mainMobKill == 8 && subMobKill == 40)
				HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[1], Nation::KARUS);
			else if (mainMobKill == 12 && subMobKill == 60)
				HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[2], Nation::KARUS);
		}
		else if (bNation == Nation::ELMORAD)
		{
			pRoomInfo->m_iElmoSubMonsterKill++;
			uint32 mainMobKill = pRoomInfo->m_iElmoMainMonsterKill;
			uint8 subMobKill = pRoomInfo->m_iElmoSubMonsterKill;
			if (mainMobKill == 4 && subMobKill == 20)
				HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[0], Nation::ELMORAD);
			else if (mainMobKill == 8 && subMobKill == 40)
				HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[1], Nation::ELMORAD);
			else if (mainMobKill == 12 && subMobKill == 60)
				HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[2], Nation::ELMORAD);
		}
		return;
	}

	bNation == Nation::KARUS ? pRoomInfo->m_iKarusMainMonsterKill++ : pRoomInfo->m_iElmoMainMonsterKill++;
	uint32 mainMobKill = bNation == Nation::KARUS ? pRoomInfo->m_iKarusMainMonsterKill : pRoomInfo->m_iElmoMainMonsterKill;
	uint8 subMobKill = bNation == Nation::KARUS ? pRoomInfo->m_iKarusSubMonsterKill : pRoomInfo->m_iElmoSubMonsterKill;

	if ((mainMobKill == 13 && subMobKill == 60) || (nNpc->GetProtoID() == DEVA_BIRD_SSID))
	{
		if (!pRoomInfo->m_FinishPacketControl)
		{
			// we killed da deva bird
			g_pMain->pTempleEvent.EventCloseMainControl = true; pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = uint32(UNIXTIME) + 20;
			pRoomInfo->m_bWinnerNation = bNation;

			/*Packet pkt1(WIZ_EVENT);
			pkt1 << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_JURAD_MOUNTAIN) << uint8(2) << uint32(20);*/
			Packet pkt1(WIZ_SELECT_MSG);
			pkt1 << uint16(0) << uint8(7) << uint64(0) << uint32(7)
				<< ((pRoomInfo->m_bWinnerNation == Nation::KARUS) ? uint32(1) : uint32(0))
				<< ((pRoomInfo->m_bWinnerNation == Nation::ELMORAD) ? uint32(1) : uint32(0))
				<< uint32(20) << uint32(20) << uint32(0);
			g_pMain->Send_All(&pkt1, nullptr, Nation::ALL, ZONE_JURAID_MOUNTAIN, true, pUser->GetEventRoom());
		}
		return;
	}

	uint16 sSid = 0, sCount = 0, sRadius = 0, sDuration = 0, sEventRoom = pUser->GetEventRoom();
	int16 sSocketID = -1;
	uint8 ZoneID = nNpc->GetZoneID(), nation = nNpc->GetNation();
	float curX = nNpc->GetX(), curY = nNpc->GetY(), curZ = nNpc->GetZ();

	int random_variable = std::rand() % 3;

	if (random_variable == 0)
		sSid = MONSTER_APOSTLE_SSID;		// apostle 
	else if (random_variable == 1)
		sSid = MONSTER_DOOM_SOLDIER_SSID;	// doom soldier
	else if (random_variable == 2)
		sSid = MONSTER_TROLL_CAPTAIN_SSID;	// troll warrior
	sCount = 5;
	sRadius = 10;

	AddNPC(new AddNPCSignals(sSid, true, ZoneID, curX, curY, curZ, sCount, sRadius, 0, Nation::NONE, -1, pUser->GetEventRoom()));

	if (bNation == Nation::KARUS)
	{
		if (mainMobKill == 4 && subMobKill == 20)
			HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[0], Nation::KARUS);
		else if (mainMobKill == 8 && subMobKill == 40)
			HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[1], Nation::KARUS);
		else if (mainMobKill == 12 && subMobKill == 60)
			HandleJuraidGateOpen(pRoomInfo->m_sKarusBridges[2], Nation::KARUS);
	}
	else if (bNation == Nation::ELMORAD)
	{
		if (mainMobKill == 4 && subMobKill == 20)
			HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[0], Nation::ELMORAD);
		else if (mainMobKill == 8 && subMobKill == 40)
			HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[1], Nation::ELMORAD);
		else if (mainMobKill == 12 && subMobKill == 60)
			HandleJuraidGateOpen(pRoomInfo->m_sElmoBridges[2], Nation::ELMORAD);
	}
}
#pragma endregion

#pragma region CNpcThread::HandleJuraidGateOpen(uint16 sNpcID, Nation bNation)

/**
* @brief	Handles the Juraid Mountain event monster counts and opens gate if
*			the party succeeds.
*/
void CNpcThread::HandleJuraidGateOpen(uint16 sNpcID, Nation bNation)
{
	CNpc* pNpc = m_arNpcArray.GetData(sNpcID);
	if (pNpc == nullptr)
		return;

	pNpc->m_byGateOpen = 2;

	Packet result(WIZ_NPC_INOUT, uint8(INOUT_OUT));
	result << pNpc->GetID();
	g_pMain->Send_All(&result, nullptr, bNation, ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

	result.clear();
	result.Initialize(WIZ_NPC_INOUT);
	result << uint8(INOUT_IN) << pNpc->GetID();
	pNpc->GetNpcInfo(result);
	g_pMain->Send_All(&result, nullptr, bNation, ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());
}

#pragma endregion

#pragma region CNpcThread::HandleGateFlagOpen(uint16 sNpcID, uint8 m_stType, uint32 m_sByGateOpen)
/**
* @brief	Handles the Juraid Mountain event monster counts and opens gate if
*			the party succeeds.
* @param	pUser the User who killed the nNpc
* @param	the killed Npc
*/
void CNpcThread::HandleGateFlagOpen(uint16 sNpcID, uint8 m_stType, uint32 m_sByGateOpen)
{
	CNpc* pNpc = m_arNpcArray.GetData(sNpcID);
	if (pNpc == nullptr)
		return;

	switch (m_stType)
	{
	case NPC_GATE:
	{
		if (pNpc->GetType() == NPC_GATE)
		{
			if (m_sByGateOpen == 1)
				pNpc->m_bNation = Nation::NONE;
			else if (m_sByGateOpen == 0)
			{
				CNpcTable * proto = g_pMain->m_arNpcTable.GetData(pNpc->GetProtoID());

				if (proto != nullptr)
					pNpc->m_bNation = proto->m_byGroup;
			}
			pNpc->m_byGateOpen = m_sByGateOpen;
		}
	}
	default:
		break;
	}
	Packet result(WIZ_NPC_INOUT, uint8(INOUT_OUT));
	result << pNpc->GetID();
	g_pMain->Send_All(&result);

	result.clear();
	result.Initialize(WIZ_NPC_INOUT);
	result << uint8(INOUT_IN) << pNpc->GetID();
	pNpc->GetNpcInfo(result);
	g_pMain->Send_All(&result);
}
#pragma endregion