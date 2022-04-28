#include "stdafx.h"
#include "Map.h"
#include "MagicInstance.h"
#include "../shared/DateTime.h"

using namespace std;

CNpc::CNpc() : Unit(UnitNPC), m_bDelete(false),
m_NpcState(NPC_LIVE), m_OldNpcState(m_NpcState), m_byGateOpen(0), m_byObjectType(NORMAL_OBJECT), m_byPathCount(0),
m_byAttackPos(0), m_ItemUserLevel(0), m_Delay(0), m_nActiveSkillID(0), m_sActiveTargetID(-1), m_sActiveCastTime(0),
m_byDirection(0), m_tDeleteTime(0),
m_proto(nullptr), m_pPath(nullptr)
{
	Initialize();
}



CNpc::~CNpc()
{
}

/**
* @brief	Initializes this object.
*/
void CNpc::Initialize()
{
	Unit::Initialize();
	m_bDeleteRequested = false;
	m_sDuration = 0;
	m_bForceReset = false;

	m_sSize = 100;
	m_strName.clear();
	m_strUserName.clear();
	m_strPetName.clear();

	m_MaxHP = 0;
	m_iHP = 0;
	m_iSellingGroup = 0;

	m_byDirection = 0;
	m_iWeapon_1 = 0;
	m_iWeapon_2 = 0;
	StateChange(NPC_LIVE);
	m_byGateOpen = 0;
	m_byObjectType = NORMAL_OBJECT;

	m_byTrapNumber = 0;
	m_fDelayTime = getMSTime();

	m_tNpcAttType = ATROCITY_ATTACK_TYPE;
	m_bHasFriends = false;
	m_byMoveType = 1;
	m_byInitMoveType = 1;
	m_byRegenType = 0;
	m_byDungeonFamily = 0;
	m_bySpecialType = NpcSpecialTypeNone;
	m_byTrapNumber = 0;
	m_byChangeType = 0;
	m_byDeadType = 0;
	m_sPathCount = 0;
	m_sMaxPathCount = 0;
	m_sUtcSecond = 0;
	m_sPetId = m_sGuardID = m_WoodOwnerID = -1;

	m_bFirstLive = true;

	m_fHPChangeTime = getMSTime();
	m_fHPType4CheckTime = getMSTime();
	m_tFaintingTime = 0;

	m_nLimitMinX = m_nLimitMinZ = 0;
	m_nLimitMaxX = m_nLimitMaxZ = 0;
	m_bIsEventNpc = false;
	m_fSecForRealMoveMetor = 0.0f;
	InitUserList();

	m_bTracing = false;
	m_fTracingStartX = m_fTracingStartZ = 0.0f;

	for (int i = 0; i < NPC_MAX_PATH_LIST; i++)
	{
		m_PathList.pPatternPos[i].x = -1;
		m_PathList.pPatternPos[i].z = -1;
	}

	m_pPatternPos.x = m_pPatternPos.z = 0;
	m_bMonster = false;
	WoodCooldownClose = 0;
	isReturningSpawnSlot = false;
}

/*
* @brief	Initializes a NPC's target.
*
*/
void CNpc::InitTarget()
{
	m_byAttackPos = 0;
	m_Target.id = -1;
	m_Target.bSet = false;
	m_Target.x = m_Target.y = m_Target.z = 0.0f;
	m_bTracing = false;
	m_Target.m_tLastDamageTime = 0;
}

void CNpc::Init()
{
	m_pMap = g_pMain->GetZoneByID(GetZoneID());
	m_Delay = 0;
	m_fDelayTime = getMSTime();

	if (GetMap() == nullptr)
	{
		TRACE("#### Npc-Init Zone Fail : [name=%s], zone=%d #####\n", GetName().c_str(), GetZoneID());
		return;
	}

	Guard lock(m_damageListLock);
	m_DamagedUserList.clear();
}

/**
* @brief	Adds the NPC to the region.
*
* @param	new_region_x	The new region x coordinate.
* @param	new_region_z	The new region z coordinate.
*/
void CNpc::AddToRegion(int16 new_region_x, int16 new_region_z)
{
	if (GetRegion())
		GetRegion()->Remove(this);

	SetRegion(new_region_x, new_region_z);

	if (GetRegion())
		GetRegion()->Add(this);
}

/**
* @brief	Constructs an in/out packet for the NPC.
*
* @param	result	The packet buffer the constructed packet will be stored in.
* @param	bType 	The type (in or out).
*/
void CNpc::GetInOut(Packet & result, uint8 bType)
{
	result.Initialize(WIZ_NPC_INOUT);
	result << bType << GetID();
	if (bType != INOUT_OUT)
		GetNpcInfo(result);

	if (bType == INOUT_IN)
		OnRespawn();
}

/**
* @brief	Constructs and sends an in/out packet for the NPC.
*
* @param	bType	The type (in or out).
* @param	fX   	The x coordinate.
* @param	fZ   	The z coordinate.
* @param	fY   	The y coordinate.
*/
void CNpc::SendInOut(uint8 bType, float fX, float fZ, float fY)
{
	if (GetRegion() == nullptr)
	{
		SetRegion(GetNewRegionX(), GetNewRegionZ());
		if (GetRegion() == nullptr)
			return;
	}

	if (bType == INOUT_OUT)
	{
		GetRegion()->Remove(this);
	}
	else
	{
		GetRegion()->Add(this);
		SetPosition(fX, fY, fZ);
	}

	Packet result;
	GetInOut(result, bType);
	SendToRegion(&result);
}

/**
* @brief	Gets NPC information for use in various NPC packets.
*
* @param	pkt	The packet the information will be stored in.
*/
void CNpc::GetNpcInfo(Packet & pkt)
{
	pkt.SByte();
	switch (GetType())
	{
	case 15:
	{
		switch (GetProtoID())
		{
		case 511:
		{
			CKnights *pKnights = g_pMain->m_KnightsArray.GetData(m_oBarrackID);
			if (pKnights == nullptr)
			{
				pkt << GetProtoID()
					<< uint8(2)
					<< GetPID()
					<< m_iSellingGroup
					<< GetType()
					<< uint32(0)
					<< m_sSize
					<< uint32(m_iWeapon_1)
					<< uint32(m_iWeapon_2)
					<< "" 
					<< GetName()
					<< uint8(GetNation())
					<< GetLevel()
					<< GetSPosX()
					<< GetSPosZ() 
					<< GetSPosY()
					<< uint32(m_byGateOpen)
					<< m_byObjectType
					<< uint16(0)
					<< uint16(0)
					<< int16(m_byDirection);
				return;
			}

			_DEATHMATCH_BARRACK_INFO* pData4 = g_pMain->m_KnightSiegeWarBarrackList.GetData(pKnights->GetID());
			if (pData4 == nullptr)
			{
				pkt << GetProtoID()
					<< uint8(2)
					<< GetPID()
					<< m_iSellingGroup
					<< GetType()
					<< uint32(0)
					<< m_sSize
					<< uint32(m_iWeapon_1)
					<< uint32(m_iWeapon_2)
					<< "" 
					<< GetName()
					<< uint8(GetNation())
					<< GetLevel()
					<< GetSPosX()
					<< GetSPosZ() 
					<< GetSPosY()
					<< uint32(m_byGateOpen)
					<< m_byObjectType
					<< uint16(0)
					<< uint16(0)
					<< int16(m_byDirection);
				return;
			}
			pkt << GetProtoID()
				<< uint8(2)
				<< GetPID()
				<< m_iSellingGroup
				<< GetType()
				<< uint32(0)
				<< m_sSize
				<< uint32(m_iWeapon_1)
				<< uint32(m_iWeapon_2)
				<< pKnights->GetName() 
				<< GetName()
				<< uint8(GetNation())
				<< GetLevel()
				<< GetSPosX()
				<< GetSPosZ() 
				<< GetSPosY()
				<< uint32(m_byGateOpen)
				<< m_byObjectType
				<< pKnights->GetID()
				<< pKnights->m_sMarkVersion
				<< int16(m_byDirection);
			return;
		}
		break;
		default:
		{
			pkt << GetProtoID()
				<< uint8(2)
				<< GetPID()
				<< m_iSellingGroup
				<< GetType()
				<< uint32(0)
				<< m_sSize
				<< uint32(m_iWeapon_1)
				<< uint32(m_iWeapon_2)
				<< GetNameUser()
				<< GetNamePet()
				<< uint8(GetNation())
				<< GetLevel()
				<< GetSPosX() 
				<< GetSPosZ() 
				<< GetSPosY()
				<< uint32(m_byGateOpen)
				<< m_byObjectType
				<< uint16(0)
				<< uint16(0)
				<< int16(m_byDirection);
			return;
		}	
		break;
		}		
	}
	break;
	case 191:
	{
		pkt << GetProtoID()
			<< uint8(2)
			<< GetPID()
			<< m_iSellingGroup
			<< GetType()
			<< uint32(0)
			<< m_sSize
			<< uint32(m_iWeapon_1)
			<< uint32(m_iWeapon_2)
			<< uint8(GetNation())
			<< GetLevel()
			<< GetSPosX() 
			<< GetSPosZ() 
			<< GetSPosY()
			<< uint32(m_byGateOpen)
			<< m_byObjectType
			<< uint16(0)
			<< uint16(0)
			<< int16(m_byDirection);
		return;
	}
	break;
	default:
	{
		pkt << GetProtoID()
			<< uint8(isMonster() ? 1 : 2)
			<< GetPID()
			<< m_iSellingGroup;
		if (!isMonster())
			pkt << uint8(NPC_SCARECROW) 
			<< uint32(0);
		else
			pkt << GetType() 
			<< uint32(0);
		pkt << m_sSize
			<< uint32(m_iWeapon_1)
			<< uint32(m_iWeapon_2);
		if (isGuardSummon())
			pkt << uint8(GetNation());
		else
			pkt << uint8(isMonster() ? 0 : GetNation());
		pkt << GetLevel()
			<< GetSPosX() 
			<< GetSPosZ() 
			<< GetSPosY()
			<< uint32(m_byGateOpen)
			<< m_byObjectType
			<< uint16(0)
			<< uint16(0)
			<< int16(m_byDirection);
		return;
	}
	break;
	}
}

/**
* @brief	Sends a gate status.
*
* @param	ObjectType  object type
* @param	bFlag  	The flag (open or shut).
*/
void CNpc::SendGateFlag(uint8 bFlag /*= -1*/)
{
	uint8 objectType = OBJECT_FLAG_LEVER;

	_OBJECT_EVENT * pObjectEvent = GetMap()->GetObjectEvent(GetProtoID());

	if (pObjectEvent)
		objectType = (uint8)pObjectEvent->sType;

	Packet result(WIZ_OBJECT_EVENT, objectType);

	// If there's a flag to set, set it now.
	if (bFlag >= 0)
		m_byGateOpen = bFlag;

	if (GetType() == NPC_OBJECT_WOOD 
		|| GetType() == NPC_ROLLINGSTONE)
		return;

	// Tell everyone nearby our new status.
	result << uint8(1) << GetID() << m_byGateOpen;
	SendToRegion(&result);
}

/**
* @brief	Sends a bridge status.
*
* @param	bFlag  	The flag (open or shut).
*/
void CNpc::SendJuraidBridgeFlag(uint8 bFlag /*= -1*/)
{
	uint8 objectType = OBJECT_GATE;
	m_byObjectType = NORMAL_OBJECT;

	if (GetZoneID() != ZONE_JURAID_MOUNTAIN
		// This isn't a typo, it's actually just a hack.
		// The picture/model ID of most spawns is the same as their prototype ID.
		// When there's more than one spawn prototype (i.e. different sSid), we keep using
		// the same picture/model ID. So we check this instead of the sSid...
		|| GetPID() != 6700)
		return;

	Packet result(WIZ_OBJECT_EVENT, objectType);

	// If there's a flag to set, set it now.
	if (bFlag >= 0)
		m_byGateOpen = (bFlag == 1);

	// Tell everyone nearby our new status.
	result << uint8(1) << GetID() << m_byGateOpen;
	SendToRegion(&result);
}

/**
* @brief	Changes an NPC's hitpoints.
*
* @param	amount   	The amount to adjust the HP by.
* @param	pAttacker	The attacker.
* @param	bSendToAI	true to update the AI server.
*/
time_t CNpc::HpChangeReq()
{
	return 0;
}


/**
* @brief	Changes an NPC's hitpoints.
*
* @param	amount   	The amount to adjust the HP by.
* @param	pAttacker	The attacker.
* @param	bSendToAI	true to update the AI server.
*/
void CNpc::HpChange(int amount, Unit *pAttacker /*= nullptr*/, bool isDOT /*= false*/)
{
	if (isDead())
		return;

	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	if (amount < 0)
		RecvAttackReq(-amount, tid, (AttributeType)AttributeNone);

	//GM Her yaratığı bir vuruşta öldürmek!
	if (pAttacker && pAttacker->isPlayer())
		if (TO_USER(pAttacker)->isGM())
			amount = (-m_iHP);

	if (isPet()){
		CUser *pUser = g_pMain->GetUserPtr(GetPetID());
		if (pUser != nullptr)
		{
			if (pUser->m_PettingOn != nullptr)
				pUser->SendPetHpChange(GetID(), amount);
		}
	}
	else {
		// Glorious copypasta.
		if (amount < 0 && -amount > m_iHP)
			m_iHP = 0;
		else if (amount >= 0 && m_iHP + amount > (int32)m_MaxHP)
			m_iHP = m_MaxHP;
		else
			m_iHP += amount;
	}

	if (m_iHP == 0)
		DeadReq(pAttacker);

	if (pAttacker != nullptr && pAttacker->isPlayer())
		TO_USER(pAttacker)->SendTargetHP(0, GetID(), amount);
	if (pAttacker != nullptr
		&& pAttacker->isNPC()
		&& TO_NPC(pAttacker)->isPet())
	{
		CUser *pUser = g_pMain->GetUserPtr(TO_NPC(pAttacker)->GetPetID());
		if (pUser != nullptr)
			pUser->SendPetHP(GetID(), amount);
	}
}

/**
* @brief Handles any attack request sent from the Gameserver.
*
* @param nDamage		the amount of the damage.
* @param sAttackerID	the ID of the attacker.
* @param attributeType	the attribute type of the attack.
*
*/
void CNpc::RecvAttackReq(int nDamage, uint16 sAttackerID, AttributeType attributeType /*= AttributeNone*/)
{
	if (isDead()
		|| nDamage < 0)
		return;
	
	Unit * pAttacker = g_pMain->GetUnitPtr(sAttackerID, GetZoneID());

	if (pAttacker != nullptr
		&& pAttacker->isPlayer())
	{
		Guard lock(m_damageListLock);
		m_DamagedUserList[pAttacker->GetID()] += nDamage;
	}

	m_TotalDamage += nDamage;

	if (pAttacker == nullptr)
		return;

	if (!pAttacker->isPlayer())
	{
		if (TO_NPC(pAttacker)->isPet())
		{
			Guard lock(m_damageListLock);
			m_DamagedUserList[pAttacker->GetID()] += nDamage;

			if (m_Target.bSet == false)
			{
				m_Target.id = pAttacker->GetID();
				m_Target.bSet = true;
				m_Target.x = pAttacker->GetX();
				m_Target.y = pAttacker->GetY();
				m_Target.z = pAttacker->GetZ();
			}
		}

		if (isGuardSummon())
		{
			if (m_Target.bSet == false)
			{
				m_Target.id = pAttacker->GetID();
				m_Target.bSet = true;
				m_Target.x = pAttacker->GetX();
				m_Target.y = pAttacker->GetY();
				m_Target.z = pAttacker->GetZ();
			}
		}

		ChangeNTarget(TO_NPC(pAttacker));
		return;
	}

	if (GetNpcState() != NPC_FAINTING)
	{
		int iRandom = myrand(1, 100);
		int sDamage = 0;
		switch (attributeType)
		{
		case 1:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sFireR / 80)));
			break;
		case 2:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sColdR / 80)));
			break;
		case 3:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sLightningR / 80)));
			break;
		case 4:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sMagicR / 80)));
			break;
		case 5:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sDiseaseR / 80)));
			break;
		case 6:
			sDamage = (int)(10 + (40 - 40 * ((double)m_sPoisonR / 80)));
			break;
			sDamage = nDamage;
		default:
			break;
		}

		if (COMPARE(iRandom, 0, sDamage))
		{
			StateChange(NPC_FAINTING);
			m_Delay = 0;
			m_tFaintingTime = UNIXTIME;
		}
		else
		{
			ChangeTarget(0, TO_USER(pAttacker));
		}
	}
}

void CNpc::HpChangeMagic(int amount, Unit *pAttacker /*= nullptr*/, AttributeType attributeType /*= AttributeNone*/)
{
	if (isDead())
		return;

	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	HpChange(amount, pAttacker);
}


/**
* @brief	Changes an NPC's mana.
*
* @param	amount	The amount to adjust the mana by.
*/
void CNpc::MSpChange(int amount)
{
	if (isPet())
	{
		CUser* pUser = g_pMain->GetUserPtr(GetPetID());
		if (pUser == nullptr)
			return;

		pUser->SendPetMSpChange(GetID(), amount);
		pUser->SendPetHP(GetID(), amount);
	}
}

/**
* @brief	Initiates the skill casting on the target with the given
*			skill ID.
*
* @param	pTarget		the target that the skill to be casted on.
* @param	nSkillID	the skill id.
*/
bool CNpc::CastSkill(Unit * pTarget, uint32 nSkillID)
{
	if (pTarget == nullptr)
		return false;

	MagicInstance instance;

	instance.bSendFail = false;
	instance.nSkillID = nSkillID;
	instance.sCasterID = GetID();
	instance.sTargetID = pTarget->GetID();
	instance.sSkillCasterZoneID = GetZoneID();
	instance.Run();

	return (instance.bSkillSuccessful);
}

float CNpc::GetRewardModifier(uint8 byLevel)
{
	int iLevelDifference = GetLevel() - byLevel;

	if (iLevelDifference <= -14)
		return 0.2f;
	else if (iLevelDifference <= -8 && iLevelDifference >= -13)
		return 0.5f;
	else if (iLevelDifference <= -2 && iLevelDifference >= -7)
		return 0.8f;

	return 1.0f;
}

float CNpc::GetPartyRewardModifier(uint32 nPartyLevel, uint32 nPartyMembers)
{
	int iLevelDifference = GetLevel() - (nPartyLevel / nPartyMembers);

	if (iLevelDifference >= 8)
		return 2.0f;
	else if (iLevelDifference >= 5)
		return 1.5f;
	else if (iLevelDifference >= 2)
		return 1.2f;

	return 1.0f;
}

/**
* @brief	Executes the death action.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeath(Unit *pKiller)
{
	if (m_byObjectType == SPECIAL_OBJECT)
	{
		_OBJECT_EVENT *pEvent = GetMap()->GetObjectEvent(GetProtoID());
		if (pEvent != nullptr)
			pEvent->byLife = 0;
	}

	Unit::OnDeath(pKiller);
	OnDeathProcess(pKiller);
}

/**
* @brief	Executes the death process.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeathProcess(Unit *pKiller)
{
	if (pKiller == nullptr
		|| !pKiller->isPlayer())
		return;

	CUser * pUser = TO_USER(pKiller);

	if (pUser == nullptr)
		return;

	if (!m_bMonster)
	{
		switch (GetType())
		{
		case NPC_BIFROST_MONUMENT:
			//BifrostMonumentProcess(pUser);
			pUser->BifrostProcess(pUser);
			break;
		case NPC_PVP_MONUMENT:
			PVPMonumentProcess(pUser);
			break;
		case NPC_BATTLE_MONUMENT:
			BattleMonumentProcess(pUser);
			break;
		case NPC_HUMAN_MONUMENT:
			HumanNationMonumentProcess(pUser);
			break;
		case NPC_KARUS_MONUMENT:
			KarusNationMonumentProcess(pUser);
			break;
		case NPC_DESTROYED_ARTIFACT:
			CastleSiegeWarMonumentKilled(pUser);
			break;
		case NPC_CLAN_WAR_MONUMENT:
			TournamentMonumentKillProcess(pUser);
			break;
		case NPC_OBJECT_FLAG:
			if (GetProtoID() == 511)
				CastleSiegeWarBarrackKilled(pUser);
		break;
		}
	}
	else if (m_bMonster)
	{
		if (GetProtoID() == 700 
			|| GetProtoID() == 750 
			|| GetProtoID() == 701 
			|| GetProtoID() == 751)
		{
			if (pUser->CheckExistEvent(STARTER_SEED_QUEST, 1))
			{
				_QUEST_HELPER * pQuestHelper;

				if (pUser->GetNation() == ELMORAD)
					pQuestHelper = g_pMain->m_QuestHelperArray.GetData(5005);
				else
					pQuestHelper = g_pMain->m_QuestHelperArray.GetData(5002);

				pUser->QuestV2RunEvent(pQuestHelper, pQuestHelper->nEventTriggerIndex);
			}
		}

		if (GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
		{
			if (GetType() == NPC_BORDER_MONUMENT)
				BDWMonumentAltarSystem(pUser);
		}

		if (GetZoneID() == ZONE_DRAKI_TOWER)
		{
			_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(pUser->GetEventRoom());
			if (pRoomInfo != nullptr)
			{
				if (pRoomInfo->m_tDrakiTowerStart == true)
				{
					pRoomInfo->m_bDrakiMonsterKill--;

					if (pRoomInfo->m_bDrakiMonsterKill <= 0)
						pUser->ChangeDrakiMode();
				}
			}
		}

		if (GetZoneID() == ZONE_DUNGEON_DEFENCE)
		{
			_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(pUser->GetEventRoom());
			if (pRoomInfo != nullptr)
			{
				if (pRoomInfo->m_DefenceisStarted == true)
				{
					pRoomInfo->m_DefenceKillCount--;

					if (pRoomInfo->m_DefenceKillCount <= 0)
						pUser->ChangeDungeonDefenceStage();
				}
			}
		}

		if (GetProtoID() == CHAOS_STONE1 
			|| GetProtoID() == CHAOS_STONE2 
			|| GetProtoID() == CHAOS_STONE3)
			ChaosStoneDeath(pUser);

		if (GetZoneID() == ZONE_RONARK_LAND
			|| GetZoneID() == ZONE_RONARK_LAND_BASE
			|| GetZoneID() == ZONE_ARDREAM)
			ChaosStoneBossKilledBy();

		if (g_pMain->pTempleEvent.isActive && GetZoneID() == ZONE_JURAID_MOUNTAIN)
		{ // the room system for Juraid Mountain
			if (GetEventRoom() > 0 
				&& pUser->GetEventRoom() == GetEventRoom())
			{
				CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(GetZoneID());
				if (zoneitrThread == nullptr)
					return;

				zoneitrThread->AddRequest(new Signal(new HandleJuraidKillSignals(GetID(), pUser->GetID(), pUser->GetNation())));
			}
		}

		if (g_pMain->m_MonsterVeriableRespawnListArray.GetSize() != 0)
		{
			_MONSTER_RESPAWN_VERIABLE_LIST * pMonsterRespawn = g_pMain->m_MonsterVeriableRespawnListArray.GetData(GetProtoID());
			if (pMonsterRespawn != nullptr)
			{
				if (pMonsterRespawn->sZoneID != 0)
				{
					if (pMonsterRespawn->sZoneID == GetZoneID())
						g_pMain->SpawnEventNpc(pMonsterRespawn->sSid, pMonsterRespawn->sType == 0 ? true : false, GetZoneID(), GetX(), GetY(), GetZ(), pMonsterRespawn->sCount, pMonsterRespawn->sRadius, pMonsterRespawn->isDeadTime * MINUTE, 0, GetID(), pUser->GetEventRoom(), 0, 1, 0);
				}
				else
				{
					g_pMain->SpawnEventNpc(pMonsterRespawn->sSid, pMonsterRespawn->sType == 0 ? true : false, GetZoneID(), GetX(), GetY(), GetZ(), pMonsterRespawn->sCount, pMonsterRespawn->sRadius, pMonsterRespawn->isDeadTime* MINUTE, 0, GetID(), pUser->GetEventRoom(), 0, 1, 0);
				}
			}
		}

		if (g_pMain->m_MonsterStableRespawnListArray.GetSize() != 0)
		{
			_MONSTER_RESPAWN_STABLE_LIST* pMonsterRespawn = g_pMain->m_MonsterStableRespawnListArray.GetData(GetProtoID());
			if (pMonsterRespawn != nullptr)
			{
				foreach_stlmap(itr, g_pMain->m_MonsterStableSignRespawnListArray)
				{
					_MONSTER_RESPAWN_STABLE_SIGN_LIST* pMonsterSign = itr->second;
					if (pMonsterSign == nullptr)
						continue;

					if (pMonsterRespawn->sZoneID == pMonsterSign->GetZoneID)
					{
						if (pMonsterRespawn->sZoneID == GetZoneID()
							&& pMonsterSign->GetZoneID == GetZoneID())
						{
							if (m_byTrapNumber == pMonsterSign->GetTrapNumber 
								|| pMonsterSign->GetID == GetID())
							{
								pMonsterSign->CurrentSsid = pMonsterRespawn->sSid;
								g_pMain->SpawnEventNpc(pMonsterRespawn->sSid, pMonsterRespawn->isNpc == 0 ? true : false, GetZoneID(), pMonsterSign->GetX, pMonsterSign->GetY, pMonsterSign->GetZ, pMonsterRespawn->sCount, pMonsterRespawn->sRadius, pMonsterRespawn->isDeadTime * MINUTE, 0, GetID(), pUser->GetEventRoom(), 0, 1,0,SpawnEventType::StableSummonSummon, pMonsterSign->GetID);
								break;
							}
						}
					}
				}
			}
		}

		if (g_pMain->m_MonsterResourceArray.GetSize() > 0)
		{
			_MONSTER_RESOURCE * pMonsterResource = g_pMain->m_MonsterResourceArray.GetData(GetProtoID());
			std::string strResource;

			if (pMonsterResource)
			{
				Packet result(WIZ_CHAT, uint8(pMonsterResource->sNoticeType));
				strResource = pMonsterResource->strResource;
				result << pUser->GetNation() << pUser->GetSocketID() << uint8(0) << strResource;

				if (pMonsterResource->sNoticeZone == 0)
					g_pMain->Send_All(&result);
				else if (pMonsterResource->sNoticeZone == 1)
					g_pMain->Send_Zone(&result, pUser->GetZoneID());
			}
		}

		if (g_pMain->m_bForgettenTempleIsActive 
			&& GetZoneID() == ZONE_FORGOTTEN_TEMPLE)
			g_pMain->m_ForgettenTempleMonsterList.erase(GetProtoID());

		if (g_pMain->m_bUnderTheCastleIsActive 
			&& GetZoneID() == ZONE_UNDER_CASTLE)
			UnderTheCastleProcess(pUser);

		if (isInTempleQuestEventZone() 
			&& pUser->isQuestEventUser())
			MonsterStoneKillProcess(pUser);
		
		if (g_pMain->isKnightRoyaleActive())
		{
			if (GetZoneID() == ZONE_KNIGHT_ROYALE)
				KnightRoyaleTreasureDeathProcess();
		}

		if (GetZoneID() == ZONE_DUNGEON_DEFENCE 
			&& pUser->GetEventRoom() > 0)
			DungeonDefenceProcess(pUser);

		if (pUser->isInParty())
		{
			_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());

			if (pParty == nullptr)
				return;

			short partyUsers[MAX_PARTY_USERS];

			for (int i = 0; i < MAX_PARTY_USERS; i++)
				partyUsers[i] = pParty->uid[i];

			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				CUser * PartyUser = g_pMain->GetUserPtr(partyUsers[i]);

				if (PartyUser == nullptr)
					continue;

				if (!isInRangeSlow(PartyUser, RANGE_50M)
					|| PartyUser->isDead() || !PartyUser->isInGame())
					continue;

				if (PartyUser->isPlayer())
				{
					PartyUser->QuestV2MonsterCountAdd(GetProtoID());
					PartyUser->AchieveMonsterCountAdd(GetProtoID());
					PartyUser->CollectionRaceProcess(GetProtoID());
				}
			}
		}
		else if (!pUser->isInParty() && pUser->isPlayer())
		{
			pUser->QuestV2MonsterCountAdd(GetProtoID());
			pUser->AchieveMonsterCountAdd(GetProtoID());

			if (g_pMain->CollectionRaceGameServerSatus)
				pUser->CollectionRaceProcess(GetProtoID());
		}
		pUser->MonsterDeathCount();
		pUser->LifeSkillHuntProcess(GetID());

		/*if (g_pMain->CollectionRaceGameServerSatus) // Açarsan solo 2x sayar
			pUser->CollectionRaceProcess(GetProtoID());*/
	}

	DateTime time;
	string pKillerPartyUsers;

	if (pUser->isInParty())
	{
		CUser *pPartyUser;
		_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
		if (pParty)
		{
			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
				if (pPartyUser)
					pKillerPartyUsers += string_format("%s,", pPartyUser->GetName().c_str());
			}
		}

		if (!pKillerPartyUsers.empty())
			pKillerPartyUsers = pKillerPartyUsers.substr(0, pKillerPartyUsers.length() - 1);
	}

	if (pKillerPartyUsers.empty())
		g_pMain->WriteDeathNpcLogFile(string_format("[ %s - %d:%d:%d ] Killer=%s,SID=%d,Target=%s,Zone=%d,X=%d,Z=%d\n", m_bMonster ? "MONSTER" : "NPC", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), GetProtoID(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	else
		g_pMain->WriteDeathNpcLogFile(string_format("[ %s - %d:%d:%d ] Killer=%s,KillerParty=%s,SID=%d,Target=%s,Zone=%d,X=%d,Z=%d\n", m_bMonster ? "MONSTER" : "NPC", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), pKillerPartyUsers.c_str(), GetProtoID(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
}

/**
* @brief	Executes the Npc respawn.
*/
void CNpc::OnRespawn()
{
	if (g_pMain->m_byBattleOpen == NATION_BATTLE
		&& (GetProtoID() == ELMORAD_MONUMENT_SID
			|| GetProtoID() == ASGA_VILLAGE_MONUMENT_SID
			|| GetProtoID() == RAIBA_VILLAGE_MONUMENT_SID
			|| GetProtoID() == DODO_CAMP_MONUMENT_SID
			|| GetProtoID() == LUFERSON_MONUMENT_SID
			|| GetProtoID() == LINATE_MONUMENT_SID
			|| GetProtoID() == BELLUA_MONUMENT_SID
			|| GetProtoID() == LAON_CAMP_MONUMENT_SID))
	{
		_MONUMENT_INFORMATION* pData = new	_MONUMENT_INFORMATION();
		pData->sSid = GetProtoID();
		pData->sNid = m_sNid;
		pData->RepawnedTime = int32(UNIXTIME) + 60;
		pData->bLevel = 1;

		if (GetProtoID() == DODO_CAMP_MONUMENT_SID || GetProtoID() == LAON_CAMP_MONUMENT_SID)
			g_pMain->m_bMiddleStatueNation = m_bNation;

		if (g_pMain->m_bVictory == m_bNation)
		{
			if (!g_pMain->m_NationMonumentWinnerNationArray.PutData(pData->sSid, pData))
				delete pData;
		}
		else
		{
			if (!g_pMain->m_NationMonumentDefeatedNationArray.PutData(pData->sSid, pData))
				delete pData;
		}
	}
	else if (g_pMain->m_bForgettenTempleIsActive && GetZoneID() == ZONE_FORGOTTEN_TEMPLE)
		g_pMain->m_ForgettenTempleMonsterList.insert(std::make_pair(GetID(), GetProtoID()));
	else if (g_pMain->m_bUnderTheCastleIsActive && GetZoneID() == ZONE_UNDER_CASTLE)
		g_pMain->m_UnderTheCastleMonsterList.insert(std::make_pair(GetID(), GetProtoID()));
}

/**
* @brief	Changes a Monster's state directly from the server
* 			without any checks.
*
* @param	bType	State type.
* @param	nBuff	The buff/flag (depending on the state type).
*/
void CNpc::StateChangeServerDirect(uint8 bType, uint32 nBuff)
{
	uint8 buff = *(uint8 *)&nBuff; // don't ask
	Packet result;

	switch (bType)
	{
	case 1:
	{
		switch (buff)
		{
		case 4:
			StateChange(NPC_SLEEPING);
			break;
		case 5:
			StateChange(NPC_FIGHTING);
			break;
		case 6:
			StateChange(NPC_STANDING);
			break;
		default:
			break;
		}
	}break;
	default:
		TRACE("[SID=%d] StateChange: %s tripped (bType=%d, buff=%d, nBuff=%d) somehow, HOW!?\n",GetID(), GetName().c_str(), bType, buff, nBuff);
		printf("[SID=%d] StateChange: %s tripped (bType=%d, buff=%d, nBuff=%d) somehow, HOW!?\n", GetID(), GetName().c_str(), bType, buff, nBuff);
		break;
	}
	result.Initialize(WIZ_STATE_CHANGE);
	result << GetID() << bType << nBuff;
	SendToRegion(&result);
}
/*
* @brief	Executes the pvp monument process.
*
* @param	pUser	The User.
*/
void CNpc::PVPMonumentProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_CHAT, uint8(MONUMENT_NOTICE));
	result << uint8(FORCE_CHAT) << pUser->GetNation() << pUser->GetName().c_str();
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, Nation::ALL);

	g_pMain->m_nPVPMonumentNation[GetZoneID()] = pUser->GetNation();
	g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation(), pUser->GetNation() == KARUS ? MONUMENT_KARUS_SPID : MONUMENT_ELMORAD_SPID);
}

/*
* @brief	Executes the battle monument process.
*
* @param	pUser	The User.
*/
void CNpc::BattleMonumentProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->m_byBattleOpen == NATION_BATTLE)
	{
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation(), pUser->GetNation() == KARUS ? MONUMENT_KARUS_SPID : MONUMENT_ELMORAD_SPID);
		g_pMain->Announcement(DECLARE_BATTLE_MONUMENT_STATUS, Nation::ALL, m_byTrapNumber, pUser);

		if (pUser->GetNation() == KARUS)
		{
			g_pMain->m_sKarusMonumentPoint += 2;
			g_pMain->m_sKarusMonuments++;

			if (g_pMain->m_sKarusMonuments >= 7)
				g_pMain->m_sKarusMonumentPoint += 10;

			if (g_pMain->m_sElmoMonuments != 0)
				g_pMain->m_sElmoMonuments--;
		}
		else
		{
			g_pMain->m_sElmoMonumentPoint += 2;
			g_pMain->m_sElmoMonuments++;

			if (g_pMain->m_sElmoMonuments >= 7)
				g_pMain->m_sElmoMonumentPoint += 10;

			if (g_pMain->m_sKarusMonuments != 0)
				g_pMain->m_sKarusMonuments--;
		}

		if (pUser->GetZoneID() == ZONE_BATTLE4 && g_pMain->isWarOpen())
		{
			Packet result;

			if (g_pMain->m_sKarusMonuments >= 7
				|| g_pMain->m_sElmoMonuments >= 7)
			{
				g_pMain->m_byNereidsIslandRemainingTime = 900;

				result.Initialize(WIZ_MAP_EVENT);
				result << uint8(6) << uint16(g_pMain->m_byNereidsIslandRemainingTime) << uint16(0);
				g_pMain->Send_All(&result, nullptr, 0, ZONE_BATTLE4);
			}

			g_pMain->m_sNereidsIslandMonuArray[m_byTrapNumber - 1] = pUser->GetNation();
			result.clear();
			result.Initialize(WIZ_MAP_EVENT);
			result << uint8(0) << uint8(7);

			for (int i = 0; i < 7; i++)
				result << g_pMain->m_sNereidsIslandMonuArray[i];

			g_pMain->Send_All(&result, nullptr, 0, ZONE_BATTLE4);

			result.clear();
			result.Initialize(WIZ_MAP_EVENT);
			result << uint8(2) << uint16(g_pMain->m_sElmoMonumentPoint) << uint16(g_pMain->m_sKarusMonumentPoint);
			g_pMain->Send_All(&result, nullptr, 0, ZONE_BATTLE4);
		}
	}
}

/*
* @brief  Executes the nation monument process.
*
* @param  pUser  The User.
*/
void CNpc::KarusNationMonumentProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->m_byBattleOpen == NATION_BATTLE)
	{
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation());
		g_pMain->Announcement(DECLARE_NATION_MONUMENT_STATUS, Nation::ALL, GetProtoID(), pUser);

		if (pUser->GetNation() == ELMORAD)
			g_pMain->m_NationMonumentDefeatedNationArray.DeleteData(GetProtoID());
		else
			g_pMain->m_NationMonumentWinnerNationArray.DeleteData(GetProtoID());
	}
}

/*
* @brief  Executes the nation monument process.
*
* @param  pUser  The User.
*/
void CNpc::HumanNationMonumentProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->m_byBattleOpen == NATION_BATTLE)
	{
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation());
		g_pMain->Announcement(DECLARE_NATION_MONUMENT_STATUS, Nation::ALL, GetProtoID(), pUser);

		if (pUser->GetNation() == KARUS)
			g_pMain->m_NationMonumentDefeatedNationArray.DeleteData(GetProtoID());
		else
			g_pMain->m_NationMonumentWinnerNationArray.DeleteData(GetProtoID());
	}
}

void CNpc::DeadReq(Unit * pKillerUnit /*= nullptr*/, bool bSendDeathPacket /*= true*/)
{
	pKiller = pKillerUnit;
	m_bSendDeathPacket = bSendDeathPacket;

	if (bSendDeathPacket)
	{
		SendExpToUserList();
		
		if (isShowBox())
			GiveNpcHaveItem(pKillerUnit);
	}

	StateChange(NPC_DEAD);
}

void CNpc::Dead(Unit * pKiller /*= nullptr*/, bool bSendDeathPacket /*= true*/)
{
	KOMap* pMap = GetMap();
	if (pMap == nullptr)
		return;

	OnDeath(pKiller);

	m_iHP = 0;
	m_oSocketID = m_oBarrackID = m_WoodOwnerID = -1;

	m_sACPercent = 100;
	m_Delay = m_sRegenTime;

	m_bFirstLive = false;
	m_byDeadType = 100;
	InitType3();
	InitType4();

	if (GetRegionX() > pMap->GetXRegionMax() || GetRegionZ() > pMap->GetZRegionMax())
		return;

	if (isPet())
	{
		CUser *pUser = g_pMain->GetUserPtr(GetPetID());
		if (pUser != nullptr)
		{
			if (pUser->m_PettingOn != nullptr)
				pUser->PetOnDeath();
		}
	}

	if (isGuardSummon())
		m_sGuardID = -1;

	if (GetRegion())
		GetRegion()->Remove(TO_NPC(this));

	SetRegion();
}

void CNpc::Load(uint16 sNpcID, CNpcTable * proto, bool bMonster, uint8 nation)
{
	m_sNid = sNpcID;
	m_proto = proto;
	m_sPid = m_proto->m_sPid;
	m_bMonster = bMonster;
	m_sSize = proto->m_sSize;
	m_iWeapon_1 = proto->m_iWeapon_1;
	m_iWeapon_2 = proto->m_iWeapon_2;
	m_bNation = nation == 0 ? proto->m_byGroup : nation;
	m_bLevel = (uint8)proto->m_sLevel; // max level used that I know about is 250, no need for 2 bytes

	// Monsters cannot, by design, be friendly to everybody.
	if (isMonster() && GetNation() == Nation::ALL)
		m_bNation = Nation::NONE;

	m_strName = proto->m_strName;
	m_byActType = proto->m_byActType;
	m_byRank = proto->m_byRank;
	m_byTitle = proto->m_byTitle;
	m_iSellingGroup = proto->m_iSellingGroup;
	m_iHP = proto->m_MaxHP;
	m_MaxHP = proto->m_MaxHP;
	m_MaxHPTemp = proto->m_MaxHP;
	m_sMP = proto->m_MaxMP;
	m_MaxMP = proto->m_MaxMP;
	m_sAttack = proto->m_sAttack;
	m_sTotalAc = proto->m_sDefense;
	m_sTotalAcTemp = proto->m_sDefense;
	m_fTotalHitrate = proto->m_sHitRate;
	m_fTotalEvasionrate = proto->m_sEvadeRate;
	m_sTotalHit = proto->m_sDamage;
	m_sAttackDelay = proto->m_sAttackDelay;
	m_sSpeed = proto->m_sSpeed;

	// Object NPCs should have an effective speed of 1x (not that it should matter, mind)
	if (m_byObjectType == SPECIAL_OBJECT)
		m_sSpeed = 1000;

	m_fSpeed_1 = ((float)proto->m_bySpeed_1 * ((float)m_sSpeed / 1000));
	m_fSpeed_2 = ((float)proto->m_bySpeed_2 * ((float)m_sSpeed / 1000));
	m_fOldSpeed_1 = ((float)proto->m_bySpeed_1 * ((float)m_sSpeed / 1000));
	m_fOldSpeed_2 = ((float)proto->m_bySpeed_2 * ((float)m_sSpeed / 1000));

	m_fSecForMetor = 4.0f;
	m_sStandTime = proto->m_sStandTime;
	m_sFireR = proto->m_byFireR;
	m_sColdR = proto->m_byColdR;
	m_sLightningR = proto->m_byLightningR;
	m_sMagicR = proto->m_byMagicR;
	m_sDiseaseR = proto->m_byDiseaseR;
	m_sPoisonR = proto->m_byPoisonR;
	m_bySearchRange = proto->m_bySearchRange;
	m_byAttackRange = proto->m_byAttackRange;
	m_byTracingRange = proto->m_byTracingRange;
	m_iMoney = proto->m_iMoney;
	m_iItem = proto->m_iItem;

	if (GetType() == NPC_DESTROYED_ARTIFACT)
		m_sRegenTime = 60 * SECOND;
	else
		m_sRegenTime = 7200 * SECOND;

	m_sMaxPathCount = 0;

	m_pMap = g_pMain->GetZoneByID(GetZoneID());
	m_bFirstLive = true;
}

void CNpc::InitPos()
{
	static const float fDD = 1.5f;
	static const float fx[4][5] =
	{														// battle pos 
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },					// 0
	{ 0.0f, -(fDD * 2),  -(fDD * 2), -(fDD * 4),  -(fDD * 4) },	// 1
	{ 0.0f,  0.0f, -(fDD * 2), -(fDD * 2), -(fDD * 2) },		// 2
	{ 0.0f, -(fDD * 2),  -(fDD * 2), -(fDD * 2), -(fDD * 4) }	// 3
	};

	static const float fz[4][5] =
	{														// battle pos 
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },					// 0
	{ 0.0f,  (fDD * 1),  -(fDD * 1),  (fDD * 1),  -(fDD * 1) },	// 1
	{ 0.0f, -(fDD * 2), (fDD * 1), (fDD * 1), (fDD * 3) },		// 2
	{ 0.0f,  (fDD * 2),   0.0f,    -(fDD * 2),  0.0f }		// 3
	};

	if (m_byBattlePos > 3)
		return;

	m_fBattlePos_x = fx[m_byBattlePos][m_byPathCount - 1];
	m_fBattlePos_z = fz[m_byBattlePos][m_byPathCount - 1];
}

/**
* @brief	Clears NPC's path data
*
*/
void CNpc::ClearPathFindData()
{
	m_bPathFlag = false;
	m_sStepCount = 0;
	m_iAniFrameCount = 0;
	m_iAniFrameIndex = 0;
	m_fAdd_x = m_fAdd_z = 0.0f;

	for (int i = 0; i < MAX_PATH_LINE; i++)
	{
		m_pPoint[i].byType = 0;
		m_pPoint[i].bySpeed = 0;
		m_pPoint[i].fXPos = -1.0f;
		m_pPoint[i].fZPos = -1.0f;
	}
}

/**
* @brief	The state of NPC's strategy thinking or shouting out
*			to seek help for other NPC's.
*
* @param	type	The type of the monster to be searched.
*/
#pragma region void CNpc::NpcStrategy(uint8 type)
void CNpc::NpcStrategy(uint8 type)
{
	switch (type)
	{
	case NPC_ATTACK_SHOUT:
		StateChange(NPC_TRACING);
		m_Delay = m_sSpeed;//STEP_DELAY;
		m_fDelayTime = getMSTime();
		break;
	}
}
#pragma endregion

/**
* @brief	Finds friend if it is atro aggressive type like Brahman, Paramun, Apostle
*			monsters.
*
* @param	type	The type of the monster to be searched.
* @return	Returns the monster id if we are looking for a healer, 0 else.
*/
#pragma region int CNpc::FindFriend(MonSearchType type)
int CNpc::FindFriend(MonSearchType type)
{
	CNpc* pNpc = nullptr;
	KOMap* pMap = GetMap();
	if (pMap == nullptr
		|| m_bySearchRange == 0
		|| (type != MonSearchNeedsHealing && hasTarget()))
		return 0;


	_TargetHealer arHealer[9];
	for (int i = 0; i < 9; i++)
	{
		arHealer[i].sNID = -1;
		arHealer[i].sValue = 0;
	}

	foreach_region(rx, rz)
		FindFriendRegion(rx + GetRegionX(), rz + GetRegionZ(), pMap, &arHealer[0], type);

	int iValue = 0, iMonsterNid = 0;
	for (int i = 0; i < 9; i++)
	{
		if (iValue < arHealer[i].sValue)
		{
			iValue = arHealer[i].sValue;
			iMonsterNid = arHealer[i].sNID;
		}
	}

	if (iMonsterNid != 0)
	{
		m_Target.id = iMonsterNid;
		m_Target.bSet = true;
		m_Target.m_tLastDamageTime = UNIXTIME2;
		return iMonsterNid;
	}

	return 0;
}
#pragma endregion

/**
* @brief	Finds friend if it is atro aggressive type like Brahman, Paramun, Apostle
*			monsters.
*
* @param	x	The region X to be search of.
* @param	z	The region Z to be search of.
* @param	pMap	the Map
* @param	pHealer	healer array if the type matches.
* @param	type	The type of search for the monster
* @return	Returns the monster id if we are looking for a healer, 0 else.
*/
#pragma region void CNpc::FindFriendRegion(int x, int z, KOMap* pMap, _TargetHealer* pHealer, MonSearchType type)
void CNpc::FindFriendRegion(int x, int z, KOMap* pMap, _TargetHealer* pHealer, MonSearchType type)
{
	if (x < 0 || z < 0 || x > pMap->GetXRegionMax() || z > pMap->GetZRegionMax())
	{
		TRACE("#### Npc-FindFriendRegion() Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", GetID(), GetProtoID(), x, z);
		return;
	}

	float fDis = 0.0f, fSearchRange = (type == MonSearchNeedsHealing ? (float)m_byAttackRange : (float)m_byTracingRange);
	int iValue = 0;

	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockNpcArray.lock();
	if (pRegion->m_RegionNpcArray.size() <= 0)
	{
		pRegion->m_lockNpcArray.unlock();
		return;
	}

	ZoneNpcArray cm_RegionNpcArray = pRegion->m_RegionNpcArray;
	pRegion->m_lockNpcArray.unlock();
	foreach(itr, cm_RegionNpcArray)
	{
		CNpc *pNpc = g_pMain->GetNpcPtr(*itr, GetZoneID());
		if (pNpc == nullptr
			|| pNpc->isDead()
			|| pNpc->GetID() == GetID()
			|| GetDistanceSqrt(pNpc) > fSearchRange)
			continue;

		if (type == MonSearchAny)
		{
			if (pNpc->hasTarget() && pNpc->GetNpcState() == NPC_FIGHTING)
				continue;

			pNpc->m_Target.id = m_Target.id;
			pNpc->m_Target.bSet = true;
			pNpc->m_Target.x = m_Target.x;
			pNpc->m_Target.y = m_Target.y;
			pNpc->m_Target.z = m_Target.z;
			pNpc->m_Target.m_tLastDamageTime = UNIXTIME2;
			pNpc->NpcStrategy(NPC_ATTACK_SHOUT);

		}
		else if (type == MonSearchSameFamily)
		{
			if (pNpc->m_bHasFriends && pNpc->m_proto->m_byFamilyType == m_proto->m_byFamilyType)
			{
				if (pNpc->hasTarget() && pNpc->GetNpcState() == NPC_FIGHTING)
					continue;

				pNpc->m_Target.id = m_Target.id;
				pNpc->m_Target.bSet = true;
				pNpc->m_Target.x = m_Target.x;
				pNpc->m_Target.y = m_Target.y;
				pNpc->m_Target.z = m_Target.z;
				pNpc->m_Target.m_tLastDamageTime = UNIXTIME2;
				pNpc->NpcStrategy(NPC_ATTACK_SHOUT);
			}
		}
		else if (type == MonSearchNeedsHealing)
		{
			if (pHealer == nullptr)
				return;

			int iHP = (int)(pNpc->m_MaxHP * 0.9);
			if (pNpc->m_iHP <= iHP)
			{
				int iCompValue = (int)((pNpc->m_MaxHP - pNpc->m_iHP) / (pNpc->m_MaxHP * 0.01));
				if (iValue < iCompValue)
				{
					iValue = iCompValue;
					pHealer->sNID = pNpc->GetID();
					pHealer->sValue = iValue;
				}
			}
		}
	}
}
#pragma endregion


/**
* @brief	Checks whether the NPC is close target to a user.
*
* @param	pUser	the User.
* @param	nRange	the range to be compared between.
*/
#pragma region bool CNpc::IsCloseTarget(CUser *pUser, int nRange)
bool CNpc::IsCloseTarget(CUser *pUser, int nRange)
{
	if (pUser == nullptr
		|| pUser->isDead()
		|| !isInRangeSlow(pUser, nRange * 2.0f))
		return false;

	m_Target.id = pUser->GetID();
	m_Target.bSet = true;
	m_Target.x = pUser->GetX();
	m_Target.y = pUser->GetY();
	m_Target.z = pUser->GetZ();

	return true;
}
#pragma endregion

/**
* @brief	Changes a NPC's target to a user.
*
* @param	nAttackType	the attack type.
* @param	pNpc		the pointer to the target user.
*/
#pragma region void CNpc::ChangeTarget(int nAttackType, CUser *pUser)
void CNpc::ChangeTarget(int nAttackType, CUser *pUser)
{
	int preDamage, lastDamage;
	__Vector3 vUser, vNpc;
	float fDistance1 = 0.0f, fDistance2 = 0.0f;
	int iRandom = myrand(0, 100);

	if (pUser == nullptr
		|| pUser->isDead()
		|| !isHostileTo(pUser)
		|| pUser->m_bInvisibilityType
		|| pUser->isGM()
		|| GetNpcState() == NPC_FAINTING
		|| isNonAttackingObject())
		return;

	CUser *preUser = nullptr;
	if (hasTarget() && m_Target.id < NPC_BAND)
		preUser = g_pMain->GetUserPtr(m_Target.id);

	if (pUser == preUser)
	{
		if (m_bHasFriends || GetType() == NPC_BOSS)
			FindFriend(GetType() == NPC_BOSS ? MonSearchAny : MonSearchSameFamily);
		return;
	}

	if (preUser != nullptr)
	{
		preDamage = 0; lastDamage = 0;

		if (iRandom >= 0 && iRandom < 50)
		{
			preDamage = preUser->GetDamage(this, nullptr, true);
			lastDamage = pUser->GetDamage(this, nullptr, true);

			if (preDamage > lastDamage)
				return;
		}
		else if (iRandom >= 50 && iRandom < 80)
		{
			vNpc.Set(GetX(), GetY(), GetZ());
			vUser.Set(preUser->GetX(), 0, preUser->GetZ());
			fDistance1 = GetDistance(vNpc, vUser);
			vUser.Set(pUser->GetX(), 0, pUser->GetZ());
			fDistance2 = GetDistance(vNpc, vUser);

			if (fDistance2 > fDistance1)
				return;
		}
		else if (iRandom >= 80 && iRandom < 95)
		{
			/* preview the amount of damage that might be dealt for comparison */
			preDamage = GetDamage(preUser, nullptr, true);
			lastDamage = GetDamage(pUser, nullptr, true);
			if (preDamage > lastDamage)
				return;
		}
	}
	else if (preUser == nullptr && nAttackType == 1004)	// Heal magic
		return;

	m_Target.id = pUser->GetID();
	m_Target.bSet = true;
	m_Target.x = pUser->GetX();
	m_Target.y = pUser->GetY();
	m_Target.z = pUser->GetZ();
	m_Target.m_tLastDamageTime = UNIXTIME2;


	int nValue = 0;

	if (GetNpcState() == NPC_STANDING || GetNpcState() == NPC_MOVING || GetNpcState() == NPC_SLEEPING)
	{
		if (IsCloseTarget(pUser, m_byAttackRange) == true)
		{
			StateChange(NPC_FIGHTING);
			m_Delay = 0;
		}
		else
		{
			nValue = GetTargetPath(1);
			if (nValue == 1)
			{
				StateChange(NPC_TRACING);
				m_Delay = 0;
			}
			else if (nValue == -1)
			{
				StateChange(NPC_STANDING);
				InitTarget();
				m_Delay = 0;
			}
			else if (nValue == 0)
			{
				m_fSecForMetor = m_fSpeed_2;	// 
				IsNoPathFind(m_fSecForMetor);
				StateChange(NPC_TRACING);
				m_Delay = 0;
			}
		}
	}

	if (m_bHasFriends || GetType() == NPC_BOSS)
		FindFriend(GetType() == NPC_BOSS ? MonSearchAny : MonSearchSameFamily);
}
#pragma endregion

/**
* @brief	Changes a NPC's target to another NPC.
*
* @param	pNpc	the pointer to the target NPC.
*/
#pragma region void CNpc::ChangeNTarget(CNpc *pNpc)
void CNpc::ChangeNTarget(CNpc *pNpc)
{
	int preDamage, lastDamage;
	__Vector3 vMonster, vNpc;
	float fDist = 0.0f;

	if (pNpc == nullptr
		|| pNpc->GetNpcState() == NPC_DEAD
		|| !hasTarget()
		|| m_Target.id < NPC_BAND)
		return;

	CNpc *preNpc = g_pMain->GetNpcPtr(m_Target.id, GetZoneID());
	if (preNpc == nullptr || pNpc == preNpc)
		return;

	preDamage = GetDamage(preNpc, nullptr, true); /* preview the damage that might be dealt for comparison */
	lastDamage = GetDamage(pNpc, nullptr, true);

	vNpc.Set(GetX(), GetY(), GetZ());
	vMonster.Set(preNpc->GetX(), 0, preNpc->GetZ());
	fDist = GetDistance(vNpc, vMonster);
	preDamage = (int)((double)preDamage / fDist + 0.5);
	vMonster.Set(pNpc->GetX(), 0, pNpc->GetZ());
	fDist = GetDistance(vNpc, vMonster);
	lastDamage = (int)((double)lastDamage / fDist + 0.5);

	if (preDamage > lastDamage) return;

	m_Target.id = pNpc->GetID();
	m_Target.bSet = true;
	m_Target.x = pNpc->GetX();
	m_Target.y = pNpc->GetZ();
	m_Target.z = pNpc->GetZ();

	int nValue = 0;
	if (GetNpcState() == NPC_STANDING || GetNpcState() == NPC_MOVING || GetNpcState() == NPC_SLEEPING)
	{
		if (IsCloseTarget(m_byAttackRange, AttackTypeNone) == 1)
		{
			StateChange(NPC_FIGHTING);
			m_Delay = 0;
		}
		else
		{
			nValue = GetTargetPath();
			if (nValue == 1)
			{
				StateChange(NPC_TRACING);
				m_Delay = 0;
			}
			else if (nValue == -1)
			{
				StateChange(NPC_STANDING);
				InitTarget();
				m_Delay = 0;
			}
			else if (nValue == 0)
			{
				m_fSecForMetor = m_fSpeed_2;
				IsNoPathFind(m_fSecForMetor);
				StateChange(NPC_TRACING);
				m_Delay = 0;
			}
		}
	}

	if (m_bHasFriends)
		FindFriend();
}
#pragma endregion

/**
* @brief	Resets the NPC's path.
*
*/
#pragma region bool CNpc::ResetPath()
bool CNpc::ResetPath()
{
	float cur_x, cur_z;
	if (!GetTargetPos(cur_x, cur_z))
		return false;

	m_Target.x = cur_x;
	m_Target.z = cur_z;

	int nValue = GetTargetPath();
	if (nValue == -1)
	{
		TRACE("Npc-ResetPath Fail - target_x = %.2f, z=%.2f, value=%d\n", m_Target.x, m_Target.z, nValue);
		return false;
	}
	else if (nValue == 0)
	{
		m_fSecForMetor = m_fSpeed_2;
		IsNoPathFind(m_fSecForMetor);
	}

	return true;
}
#pragma endregion

bool CNpc::GetTargetPos(float& x, float& z)
{
	if (!hasTarget())
		return false;

	float fDis = 0.0f;

	Unit * pUnit = g_pMain->GetUnitPtr(m_Target.id, GetZoneID());
	if (pUnit == nullptr
		|| pUnit->GetZoneID() != GetZoneID())
		return false;


	x = pUnit->GetX();
	z = pUnit->GetZ();

	return true;
}

bool CNpc::IsChangePath()
{
	float fCurX = 0.0f, fCurZ = 0.0f;
	GetTargetPos(fCurX, fCurZ);

	__Vector3 vStart, vEnd;
	vStart.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vEnd.Set(fCurX, 0, fCurZ);

	float fDis = GetDistance(vStart, vEnd);
	float fCompDis = 3.0f;

	if (fDis < fCompDis)
		return false;

	return true;
}

void CNpc::TracingAttack()
{
	uint16 nID = m_Target.id;
	if (nID < NPC_BAND)	// Target is a player
	{
		CUser * pUser = g_pMain->GetUserPtr(nID);
		if (pUser == nullptr
			|| pUser->m_bInvisibilityType
			|| pUser->isGM()
			//|| !GetMap()->canAttackOtherNation() 
			|| pUser->GetID() == m_oSocketID)
			return;
	}
	else // Target is an NPC/monster
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(nID, GetZoneID());
		if (pNpc == nullptr)
			return;
	}
	SendAttackRequest(nID);
}

int CNpc::GetTargetPath(int option)
{
	int nInitType = m_byInitMoveType;
	if (m_byInitMoveType >= 100)
		nInitType -= 100;

	if (GetType() != NPC_MONSTER
		&& m_byMoveType != nInitType)
		m_byMoveType = nInitType;

	m_fSecForMetor = m_fSpeed_2;
	CUser* pUser = nullptr;
	CNpc* pNpc = nullptr;
	float iTempRange = 0.0f;
	__Vector3 vUser, vNpc, vDistance, vEnd22;
	float fDis = 0.0f;
	float fDegree = 0.0f, fTargetDistance = 0.0f;
	float fSurX = 0.0f, fSurZ = 0.0f;

	// Player
	if (m_Target.id < NPC_BAND)
	{
		pUser = g_pMain->GetUserPtr(m_Target.id);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->isDead()
			|| pUser->GetZoneID() != GetZoneID())
			return -1;


		if (option == 1)
		{	// magic
			vNpc.Set(GetX(), 0, GetZ());
			vUser.Set(pUser->GetX(), 0, pUser->GetZ());
			fDis = GetDistance(vNpc, vUser);

			if (fDis >= NPC_MAX_MOVE_RANGE)
				return -1;

			iTempRange = fDis + 10;
		}
		else
		{
			iTempRange = (float)m_bySearchRange;
			if (IsDamagedUserList(pUser))
				iTempRange = (float)m_byTracingRange;
			else
				iTempRange += 5;
		}

		if (m_bTracing && !isInRangeSlow(m_fTracingStartX, m_fTracingStartZ, iTempRange))
		{
			InitTarget();
			return -1;
		}
	}
	// NPC
	else if (m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)
	{	// Target
		pNpc = g_pMain->GetNpcPtr(m_Target.id, GetZoneID());

		if (pNpc == nullptr)
		{
			InitTarget();
			return false;
		}

		if (pNpc->m_iHP <= 0 || pNpc->GetNpcState() == NPC_DEAD)
		{
			InitTarget();
			return -1;
		}

		iTempRange = (float)m_byTracingRange;
	}

	KOMap* pMap = GetMap();
	if (pMap == nullptr)
		return -1;

	int max_xx = pMap->GetMapSize();
	int max_zz = pMap->GetMapSize();

	int min_x = (int)(GetX() - iTempRange) / TILE_SIZE;	if (min_x < 0) min_x = 0;
	int min_z = (int)(GetZ() - iTempRange) / TILE_SIZE;	if (min_z < 0) min_z = 0;
	int max_x = (int)(GetX() + iTempRange) / TILE_SIZE;	if (max_x > max_xx) max_x = max_xx;
	int max_z = (int)(GetZ() + iTempRange) / TILE_SIZE;	if (min_z > max_zz) min_z = max_zz;

	// Targeting player
	if (m_Target.id < NPC_BAND)
	{
		if (pUser == nullptr || !pUser->isInGame())
			return -1;

		CRect r = CRect(min_x, min_z, max_x + 1, max_z + 1);
		if (!r.PtInRect((int)pUser->GetX() / TILE_SIZE, (int)pUser->GetZ() / TILE_SIZE))
		{
			TRACE("### Npc-GetTargetPath() User Fail return -1: [nid=%d] t_Name=%s, AttackPos=%d ###\n", GetID(), pUser->GetName().c_str(), m_byAttackPos);
			return -1;
		}

		m_fStartPoint_X = GetX();		m_fStartPoint_Y = GetZ();

		vNpc.Set(GetX(), 0, GetZ());
		vUser.Set(pUser->GetX(), 0, pUser->GetZ());

		IsSurround(pUser);

		if (hasTarget() && GetZoneID() != ZONE_BIFROST)
		{
			vUser.Set(pUser->GetX(), pUser->GetY(), pUser->GetZ());

			CalcAdaptivePosition(vNpc, vUser, 2.0f + m_proto->m_fBulk, &vEnd22);
			float fX, fZ;
			if (m_byAttackPos > 0 && m_byAttackPos < 9)
			{
				fX = vUser.x + surround_fx[m_byAttackPos - 1];	fZ = vUser.z + surround_fz[m_byAttackPos - 1];
				vEnd22.Set(fX, 0, fZ);
				m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
			}
			else
			{
				fX = vEnd22.x;	fZ = vEnd22.z;
				m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
			}
		}
		else
		{
			if (m_byAttackPos > 0 && m_byAttackPos < 9)
			{
				fDegree = (float)((m_byAttackPos - 1) * 45);
				fTargetDistance = 2.0f + m_proto->m_fBulk;
				ComputeDestPos(vUser, fDegree, fTargetDistance, &vEnd22);
				fSurX = vEnd22.x - vUser.x;			fSurZ = vEnd22.z - vUser.z;
				m_fEndPoint_X = vUser.x + fSurX;	m_fEndPoint_Y = vUser.z + fSurZ;
			}
			else
			{
				CalcAdaptivePosition(vNpc, vUser, 2.0f + m_proto->m_fBulk, &vEnd22);
				m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
			}
		}
	}
	else
	{
		if (pNpc == nullptr)
			return -1;

		CRect r = CRect(min_x, min_z, max_x + 1, max_z + 1);
		if (!r.PtInRect((int)pNpc->GetX() / TILE_SIZE, (int)pNpc->GetZ() / TILE_SIZE))
		{
			TRACE("### Npc-GetTargetPath() Npc Fail return -1: [nid=%d] t_Name=%s, AttackPos=%d ###\n", GetID(), pNpc->GetName().c_str(), m_byAttackPos);
			return -1;
		}

		m_fStartPoint_X = GetX();		m_fStartPoint_Y = GetZ();

		vNpc.Set(GetX(), 0, GetZ());
		vUser.Set(pNpc->GetX(), pNpc->GetY(), pNpc->GetZ());

		CalcAdaptivePosition(vNpc, vUser, 2.0f + m_proto->m_fBulk, &vEnd22);
		m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
	}

	vDistance = vEnd22 - vNpc;
	fDis = vDistance.Magnitude();

	if (fDis <= m_fSecForMetor)
	{
		ClearPathFindData();
		m_bPathFlag = true;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		return true;
	}

	if ((int)fDis > iTempRange)
	{
		TRACE("Npc-GetTargetPath() searchrange over Fail return -1: [nid=%d,%s]\n", GetID(), GetName().c_str());
		return -1;
	}


	if (GetType() != NPC_DUNGEON_MONSTER && hasTarget())
		return 0;

	CPoint start, end;
	start.x = (int)(GetX() / TILE_SIZE) - min_x;
	start.y = (int)(GetZ() / TILE_SIZE) - min_z;
	end.x = (int)(vEnd22.x / TILE_SIZE) - min_x;
	end.y = (int)(vEnd22.z / TILE_SIZE) - min_z;

	if (GetType() == NPC_DUNGEON_MONSTER && !isInSpawnRange((int)vEnd22.x, (int)vEnd22.z))
		return -1;

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	return PathFind(start, end, m_fSecForMetor);
}

CloseTargetResult CNpc::IsCloseTarget(int nRange, AttackType attackType)
{
	if (!hasTarget())
		return CloseTargetInvalid;

	CUser * pUser = nullptr;
	CNpc * pNpc = nullptr;
	__Vector3 vUser, vWillUser, vNpc, vDistance;
	float fDis = 0.0f, fWillDis = 0.0f, fX = 0.0f, fZ = 0.0f;
	bool  bUserType = false;
	vNpc.Set(GetX(), 0, GetZ());

	if (m_Target.id < NPC_BAND)
	{
		pUser = g_pMain->GetUserPtr(m_Target.id);
		if (pUser == nullptr)
		{
			InitTarget();
			return CloseTargetInvalid;
		}

		vUser.Set(pUser->GetX(), 0, pUser->GetZ());
		vWillUser.Set(pUser->m_curx, 0, pUser->m_curz);
		fX = pUser->GetX();
		fZ = pUser->GetZ();

		vDistance = vWillUser - vNpc;
		fWillDis = vDistance.Magnitude();
		fWillDis = fWillDis - m_proto->m_fBulk;
		bUserType = true;
	}
	else
	{
		pNpc = g_pMain->GetNpcPtr(m_Target.id, GetZoneID());
		if (pNpc == nullptr)
		{
			InitTarget();
			return CloseTargetInvalid;
		}
		vUser.Set(pNpc->GetX(), 0, pNpc->GetZ());
		fX = pNpc->GetX();
		fZ = pNpc->GetZ();
	}

	vDistance = vUser - vNpc;
	fDis = vDistance.Magnitude();

	fDis = fDis - m_proto->m_fBulk;

	if (GetType() == NPC_DUNGEON_MONSTER
		&& !isInSpawnRange((int)vUser.x, (int)vUser.z))
		return CloseTargetInvalid;

	if (attackType == AttackTypePhysical)
	{
		if (pUser != nullptr)
		{
			if ((int)fDis < nRange)
				m_bStopFollowingTarget = true;
		}
	}

	if ((int)fDis > nRange)
	{
		if (attackType == AttackTypeMagic)
		{
			//m_bStopFollowingTarget = true;
			m_Target.x = fX;
			m_Target.z = fZ;
		}

		return CloseTargetNotInRange;
	}

	m_fEndPoint_X = GetX();
	m_fEndPoint_Y = GetZ();
	m_Target.x = fX;
	m_Target.z = fZ;

	if (GetProto()->m_byDirectAttack == 1)
	{
		if (fDis <= LONG_ATTACK_RANGE)
			return CloseTargetInGeneralRange;
		else if (fDis <= nRange)
			return CloseTargetInAttackRange;
	}
	else
	{
		if (attackType == AttackTypeMagic)
		{
			if (fDis <= (SHORT_ATTACK_RANGE + m_proto->m_fBulk))
				return CloseTargetInGeneralRange;
			else if (fDis <= nRange)
				return CloseTargetInAttackRange;

			if (bUserType && fWillDis > (SHORT_ATTACK_RANGE + m_proto->m_fBulk) && fWillDis <= nRange)
				return CloseTargetInAttackRange;
		}
		else
		{
			if (fDis >= (SHORT_ATTACK_RANGE + m_proto->m_fBulk))
				return CloseTargetInGeneralRange;
			else if (fDis <= nRange)
				return CloseTargetInAttackRange;
		}
	}

	return CloseTargetNotInRange;
}

bool CNpc::StepMove()
{
	if (GetNpcState() != NPC_MOVING && GetNpcState() != NPC_TRACING && GetNpcState() != NPC_BACK)
		return false;

	__Vector3 vStart, vEnd, vDis;
	float fDis;

	float fOldCurX = 0.0f, fOldCurZ = 0.0f;

	if (m_sStepCount == 0)
	{
		fOldCurX = GetX();
		fOldCurZ = GetZ();
	}
	else
	{
		fOldCurX = m_fPrevX;
		fOldCurZ = m_fPrevZ;
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);


	if (m_pPoint[m_iAniFrameCount].fXPos < 0 || m_pPoint[m_iAniFrameCount].fZPos < 0)
	{
		m_fPrevX = m_fEndPoint_X;
		m_fPrevZ = m_fEndPoint_Y;

		RegisterRegionNpc(m_fPrevX, m_fPrevZ);
		return false;
	}

	fDis = GetDistance(vStart, vEnd);
	if (fDis >= m_fSecForMetor)
	{
		GetVectorPosition(vStart, vEnd, m_fSecForMetor, &vDis);
		m_fPrevX = vDis.x;
		m_fPrevZ = vDis.z;
	}
	else
	{
		m_iAniFrameCount++;
		if (m_iAniFrameCount == m_iAniFrameIndex)
		{
			vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);
			fDis = GetDistance(vStart, vEnd);

			if (fDis > m_fSecForMetor)
			{
				GetVectorPosition(vStart, vEnd, m_fSecForMetor, &vDis);
				m_fPrevX = vDis.x;
				m_fPrevZ = vDis.z;
			}
			else
			{
				m_fPrevX = m_fEndPoint_X;
				m_fPrevZ = m_fEndPoint_Y;
			}
		}
		else
		{
			vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);
			fDis = GetDistance(vStart, vEnd);
			if (fDis >= m_fSecForMetor)
			{
				GetVectorPosition(vStart, vEnd, m_fSecForMetor, &vDis);
				m_fPrevX = vDis.x;
				m_fPrevZ = vDis.z;
			}
			else
			{
				m_fPrevX = m_fEndPoint_X;
				m_fPrevZ = m_fEndPoint_Y;
			}
		}
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	vEnd.Set(m_fPrevX, 0, m_fPrevZ);

	m_fSecForRealMoveMetor = GetDistance(vStart, vEnd);

	if (m_fSecForRealMoveMetor > m_fSecForMetor + 1)
	{
		TRACE("#### move fail : [nid = %d], m_fSecForMetor = %.2f\n", GetID(), m_fSecForRealMoveMetor);
	}

	if (m_sStepCount++ > 0)
	{
		m_curx = fOldCurX;		 m_curz = fOldCurZ;
		if (GetX() < 0 || GetZ() < 0)
		{
			TRACE("Npc-StepMove : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());
		}
		return RegisterRegionNpc(GetX(), GetZ());
	}

	return true;
}

bool CNpc::StepNoPathMove()
{
	if (GetNpcState() != NPC_MOVING && GetNpcState() != NPC_TRACING && GetNpcState() != NPC_BACK)
		return false;

	__Vector3 vStart, vEnd;
	float fOldCurX = 0.0f, fOldCurZ = 0.0f;

	if (m_sStepCount == 0)
	{
		fOldCurX = GetX();
		fOldCurZ = GetZ();
	}
	else
	{
		fOldCurX = m_fPrevX;
		fOldCurZ = m_fPrevZ;
	}


	if (m_sStepCount < 0 || m_sStepCount > m_iAniFrameIndex)
	{
		TRACE("#### IsNoPtahfind Fail : nid=%d,%s, count=%d/%d ####\n", GetID(), GetName().c_str(), m_sStepCount, m_iAniFrameIndex);
		return false;
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	m_fPrevX = m_pPoint[m_sStepCount].fXPos;
	m_fPrevZ = m_pPoint[m_sStepCount].fZPos;
	vEnd.Set(m_fPrevX, 0, m_fPrevZ);

	if (m_fPrevX == -1 || m_fPrevZ == -1)
	{
		TRACE("##### StepNoPath Fail : nid=%d,%s, x=%.2f, z=%.2f #####\n", GetID(), GetName().c_str(), m_fPrevX, m_fPrevZ);
		return false;
	}

	m_fSecForRealMoveMetor = GetDistance(vStart, vEnd);

	if (m_sStepCount++ > 0)
	{
		if (fOldCurX < 0 || fOldCurZ < 0)
		{
			TRACE("#### Npc-StepNoPathMove Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), fOldCurX, fOldCurZ);
			return false;
		}
		else
		{
			m_curx = fOldCurX;	 m_curz = fOldCurZ;
		}
		return RegisterRegionNpc(GetX(), GetZ());

	}

	return true;
}

bool CNpc::IsMovingEnd()
{
	if (m_fPrevX == m_fEndPoint_X && m_fPrevZ == m_fEndPoint_Y)
	{
		m_iAniFrameCount = 0;
		return true;
	}

	return false;
}

int CNpc::IsSurround(CUser* pUser)
{
	if (GetProto()->m_byDirectAttack)
		return 0;

	if (pUser == nullptr)
		return -2;

	int nDir = GetDir(GetX(), GetZ(), pUser->GetX(), pUser->GetZ());
	if (nDir != 0)
	{
		m_byAttackPos = nDir;
		return nDir;
	}
	return -1;
}

bool CNpc::IsDamagedUserList(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	Guard lock(m_damageListLock);
	return (m_DamagedUserList.find(pUser->GetID()) != m_DamagedUserList.end());
}

bool CNpc::IsPathFindCheck(float fDistance)
{
	int nX = 0, nZ = 0;
	__Vector3 vStart, vEnd, vDis, vOldDis;
	float fDis = 0.0f;
	vStart.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	vEnd.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vDis.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	int count = 0;
	int nError = 0;

	KOMap* pMap = GetMap();

	nX = (int)(vStart.x / TILE_SIZE);
	nZ = (int)(vStart.z / TILE_SIZE);
	if (pMap->IsMovable(nX, nZ))
		return false;

	nX = (int)(vEnd.x / TILE_SIZE);
	nZ = (int)(vEnd.z / TILE_SIZE);
	if (pMap->IsMovable(nX, nZ))
		return false;

	do
	{
		vOldDis.Set(vDis.x, 0, vDis.z);
		GetVectorPosition(vDis, vEnd, fDistance, &vDis);
		fDis = GetDistance(vOldDis, vEnd);

		if (fDis > NPC_MAX_MOVE_RANGE)
		{
			nError = -1;
			break;
		}

		nX = (int)(vDis.x / TILE_SIZE);
		nZ = (int)(vDis.z / TILE_SIZE);

		if (pMap->IsMovable(nX, nZ)
			|| count >= MAX_PATH_LINE)
		{
			nError = -1;
			break;
		}

		m_pPoint[count].fXPos = vEnd.x;
		m_pPoint[count++].fZPos = vEnd.z;

	} while (fDis <= fDistance);

	m_iAniFrameIndex = count;

	if (nError == -1)
		return false;

	return true;
}

/**
* @brief
*/
void CNpc::IsNoPathFind(float fDistance)
{
	{
		Guard lock(m_buffLock);
		if (!m_buffMap.empty())
		{
			auto itr = m_buffMap.find(BUFF_TYPE_SPEED2);
			if (itr != m_buffMap.end())
			{
				fDistance /= 3;
			}
		}
	}

	ClearPathFindData();
	m_bPathFlag = true;

	__Vector3 vStart, vEnd, vDis, vOldDis;
	float fDis = 0.0f;
	vStart.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	vEnd.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vDis.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	int count = 0;

	fDis = GetDistance(vStart, vEnd);
	if (fDis > NPC_MAX_MOVE_RANGE)
	{
		ClearPathFindData();
		return;
	}

	if (GetMap() == nullptr)
	{
		ClearPathFindData();
		TRACE("#### Npc-IsNoPathFind No map : [nid=%d, name=%s], zone=%d #####\n", GetID(), GetName().c_str(), GetZoneID());
		return;
	}

	KOMap* pMap = GetMap();

	do
	{
		vOldDis.Set(vDis.x, 0, vDis.z);
		if (fDis > fDistance)
			GetVectorPosition(vOldDis, vEnd, fDistance, &vDis);
		else
		{
			vDis.Set(vEnd.x, 0, vEnd.z);
			m_pPoint[count].fXPos = vDis.x;
			m_pPoint[count].fZPos = vDis.z;
			count++;
			break;
		}

		fDis = GetDistance(vDis, vEnd);

		if (count < 0 || count >= MAX_PATH_LINE)
		{
			ClearPathFindData();
			TRACE("#### Npc-IsNoPathFind index overflow Fail 1 :  count=%d ####\n", count);
			return;
		}

		m_pPoint[count].fXPos = vDis.x;
		m_pPoint[count++].fZPos = vDis.z;

	} while (fDis != 0.0f);

	if (count <= 0 || count >= MAX_PATH_LINE)
	{
		ClearPathFindData();
		TRACE("#### IsNoPtahfind Fail : nid=%d,%s, count=%d ####\n", GetID(), GetName().c_str(), count);
		return;
	}
	m_iAniFrameIndex = count;

}

void CNpc::SendAttackRequest(int16 tid)
{
	Unit * pTarget;
	int16 sDamage;
	uint8 bResult = ATTACK_FAIL;

	pTarget = g_pMain->GetUnitPtr(tid, GetZoneID());

	if (isPlayer()
		|| pTarget == nullptr
		|| isDead()
		|| pTarget->isDead()
		|| pTarget->GetEventRoom() != GetEventRoom())
		return;

	// TODO: Wrap this up into its own virtual method
	sDamage = GetDamage(pTarget);
	if (sDamage > 0)
	{
		pTarget->HpChange(-(sDamage), this);
		if (pTarget->isDead())
			bResult = ATTACK_TARGET_DEAD;
		else
			bResult = ATTACK_SUCCESS;

		// Every hit takes a little of the defender's armour durability.
		if (pTarget->isPlayer())
			TO_USER(pTarget)->ItemWoreOut(DEFENCE, sDamage);
	}

	Packet result(WIZ_ATTACK, uint8(LONG_ATTACK));
	result << bResult << GetID() << tid;
	SendToRegion(&result);
}

#pragma region NpcThread Methods. Finite State Machine Structure Implementation.

/**
* @brief	Handles the NPC state when attacking.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::Attack()
time_t CNpc::Attack()
{
	if (isDead())
		return -1;

	int nRandom = 0, nPercent = 1000;
	bool bTeleport = false;

	if (isNonAttackingObject())
	{
		StateChange(NPC_STANDING);
		InitTarget();
		return 0;
	}

	uint16 nID = m_Target.id;
	int nStandingTime = m_sStandTime;

	if (GetProto()->m_byDirectAttack == 1)
		return LongAndMagicAttack();
	
	auto result = IsCloseTarget(m_byAttackRange, AttackTypeMagic);
	if (result == CloseTargetNotInRange)
	{
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInAttackRange)
	{
		if (GetProto()->m_byDirectAttack == 2)
			return LongAndMagicAttack();

		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInvalid)
	{
		StateChange(NPC_STANDING);
		InitTarget();
		return 0;
	}

	int	nDamage = 0;

	// Targeting player
	if (nID < NPC_BAND)
	{
		CUser * pUser = g_pMain->GetUserPtr(nID);
		if (pUser == nullptr
			|| pUser->isDead()
			|| pUser->m_bInvisibilityType)
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return nStandingTime;
		}

		// Don't attack GMs.
		if (pUser->isGM())
		{
			InitTarget();
			StateChange(NPC_MOVING);
			return nStandingTime;
		}

		if (GetProto()->m_byMagicAttack == 4
			|| GetProto()->m_byMagicAttack == 5)
		{
			nRandom = myrand(1, 5000);
			if (nRandom < nPercent)
			{
				int32 sRandSkill = myrand(0, 400);

				if (sRandSkill < 30 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (sRandSkill < 60 && m_proto->m_iMagic3 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				return m_sAttackDelay;
			}
		}
		else if (GetProto()->m_byMagicAttack == 2)
		{
			nRandom = myrand(1, 5000);
			if (nRandom < nPercent)
			{
				int32 sRandSkill = myrand(0, 400);

				if (sRandSkill < 30 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (sRandSkill < 60 && m_proto->m_iMagic3 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				return m_sAttackDelay;
			}
		}
		else if (GetProto()->m_byMagicAttack == 6)
		{
			nRandom = myrand(1, 3500);
			if (nRandom < nPercent)
			{
				int32 sRandSkill = myrand(0, 3000);

				if (sRandSkill < 1000 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (sRandSkill < 2000 && m_proto->m_iMagic3 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));

				return m_sAttackDelay;
			}
		}
		else if (GetProto()->m_byMagicAttack == 3)
		{
			switch (GetProtoID())
			{
#pragma region Under The Castle 1. Room
			case MONSTER_ElITE_TIMARLI:
			case MONSTER_ElITE_TIMARLI_II:
			case MONSTER_ElITE_TIMARLI_III:
			{
				if (m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_FLYING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			}
			return 1 * SECOND;

			case MONSTER_EMPEROR_MAMMOTH_I:
			case MONSTER_EMPEROR_MAMMOTH_II:
			case MONSTER_EMPEROR_MAMMOTH_III:
			{
				if (m_sUtcSecond == 15 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 20 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 30 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 40)
				{
					m_sUtcSecond = 0;

					int32 sRand = myrand(0, 100);
					if (sRand < 30)
						MagicPacket(MAGIC_CASTING, (uint32)502013, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 60)
						MagicPacket(MAGIC_CASTING, (uint32)502014, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else
						MagicPacket(MAGIC_CASTING, (uint32)502024, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_CRESHERGIMMIC_I:
			case MONSTER_CRESHERGIMMIC_II:
			case MONSTER_CRESHERGIMMIC_III:
			case MONSTER_CRESHERGIMMIC_VI:
			{
				if (m_sUtcSecond == 15 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 20 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 30 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 50 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_CASTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;
#pragma endregion

#pragma region Under The Castle 2. Room
			case MONSTER_MOEBIUS_EVIL: // Moebius Evil
			case MONSTER_MOEBIUS_RAGE:
			case MONSTER_MOEBIUS_RAGE_II:
			case MONSTER_MOEBIUS_EVIL_II:
			{
				if (m_sUtcSecond == 10 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 20 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 30 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_PURIOUS_I: // Purious
			case MONSTER_PURIOUS_II:
			case MONSTER_PURIOUS_III:
			case MONSTER_PURIOUS_VI:
			{
				if (m_sUtcSecond == 15 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 25 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 40 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 60)
				{
					m_sUtcSecond = 0;

					int32 sRand = myrand(0, 100);
					if (sRand < 30)
						MagicPacket(MAGIC_CASTING, (uint32)502025, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 60)
						MagicPacket(MAGIC_CASTING, (uint32)502015, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 85)
						MagicPacket(MAGIC_CASTING, (uint32)502017, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else
						MagicPacket(MAGIC_CASTING, (uint32)502016, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_GARIONEUS: // UTC - Garioneus
			case MONSTER_ELITE_GARIONEUS: // UTC - [Elite] Garioneus
			case MONSTER_ELITE_GARIONEUS_II:
			{
				if (m_sUtcSecond == 10 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 20 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 30 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;
#pragma endregion

#pragma region Under The Castle 3. Room
			case MONSTER_SORCERER_GEDEN_I: //UTC - [Sorcerer] Geden
			case MONSTER_SORCERER_GEDEN_II: //UTC - [Sorcerer] Geden
			case MONSTER_SORCERER_GEDEN_III: //UTC - [Sorcerer] Geden
			{
				if (m_sUtcSecond == 40 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 80 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 120 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_ATAL: //UTC - Atal
			{
				if (m_sUtcSecond == 20 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 60 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 90 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_MOSPELL: //UTC - Moospell
			{
				if (m_sUtcSecond == 30 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 70 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 100 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_AHMI: //UTC - Ahmi
			{
				if (m_sUtcSecond == 10 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 35 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 65 && m_proto->m_iMagic3 != 0)
				{
					m_sUtcSecond = 0;
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;

			case MONSTER_FLUWITON_ROOM_3_I: //Pluwitonn 2.od
			case MONSTER_FLUWITON_ROOM_3_II: //Pluwitonn 2.od
			case MONSTER_FLUWITON_ROOM_3_III: //Pluwitonn 2.od
			{
				if (m_sUtcSecond == 10 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 35 && m_proto->m_iMagic2 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 65)
				{
					m_sUtcSecond = 0;

					int32 sRand = myrand(0, 200);
					if (sRand < 30)
						MagicPacket(MAGIC_CASTING, (uint32)502020, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 60)
						MagicPacket(MAGIC_CASTING, (uint32)502021, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 85)
						MagicPacket(MAGIC_CASTING, (uint32)502031, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 120)
						MagicPacket(MAGIC_CASTING, (uint32)502019, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 150)
						MagicPacket(MAGIC_CASTING, (uint32)502028, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 170)
						MagicPacket(MAGIC_CASTING, (uint32)502029, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else if (sRand < 180)
						MagicPacket(MAGIC_CASTING, (uint32)502030, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else
						MagicPacket(MAGIC_CASTING, (uint32)502019, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;
#pragma endregion

#pragma region Under The Castle 4. Room
			case MONSTER_FLUWITON_ROOM_4_I: //Pluwitonn Son Oda
			case MONSTER_FLUWITON_ROOM_4_II: //Pluwitonn Son Oda
			case MONSTER_FLUWITON_ROOM_4_III: //Pluwitonn Son Oda
			case MONSTER_FLUWITON_ROOM_4_VI: //Pluwitonn Son Oda
			{
				if (m_sUtcSecond == 5 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 10 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 15 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 20 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 25 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 30 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 35 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 40 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 45 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 50 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 55 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (m_sUtcSecond == 60 && m_proto->m_iMagic1 != 0)
					MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				if (m_sUtcSecond == 70)
				{
					m_sUtcSecond = 0;

					int32 sRand = myrand(0, 100);
					if (sRand < 70)
						MagicPacket(MAGIC_CASTING, (uint32)502022, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else
						MagicPacket(MAGIC_CASTING, (uint32)502023, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
				m_sUtcSecond++;
				SendAttackRequest(pUser->GetID());
			}
			return 1 * SECOND;
#pragma endregion

			default:
				nRandom = myrand(1, 5000);
				if (nRandom < nPercent)
				{
					int32 sRandSkill = myrand(0, 400);

					if (sRandSkill < 30 && m_proto->m_iMagic2 != 0)
						MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					if (sRandSkill < 60 && m_proto->m_iMagic3 != 0)
						MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					else
						MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
					return m_sAttackDelay;
				}
				break;
			}
		}
		SendAttackRequest(pUser->GetID());
	}
	else // Targeting NPC
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(nID, GetZoneID());
		if (pNpc == nullptr
			|| pNpc->isDead())
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return nStandingTime;
		}

		if (isHealer()
			&& !isHostileTo(pNpc))
		{
			StateChange(NPC_HEALING);
			return 0;
		}
		SendAttackRequest(pNpc->GetID());
	}

	return m_sAttackDelay;
}
#pragma endregion

/**
* @brief	Handles the NPC state when making a long distanced magic attack.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::LongAndMagicAttack()
time_t CNpc::LongAndMagicAttack()
{
	int nStandingTime = m_sStandTime;
	auto result = IsCloseTarget(m_byAttackRange, AttackTypeMagic);
	if (result == CloseTargetNotInRange)
	{
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInAttackRange && GetProto()->m_byDirectAttack == 1)
	{
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInvalid)
	{
		StateChange(NPC_STANDING);
		InitTarget();
		return 0;
	}

	CNpc*	pNpc = nullptr;
	CUser*	pUser = nullptr;
	int		nDamage = 0;
	uint16 nID = m_Target.id;

	if (nID < NPC_BAND)
	{
		pUser = g_pMain->GetUserPtr(nID);
		if (pUser == nullptr
			|| pUser->isDead()
			|| pUser->m_bInvisibilityType
			// Don't cast skills on GMs.
			|| pUser->isGM())
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return nStandingTime;
		}

		if (GetProtoID() == MONSTER_FLUWITON_ROOM_4_I
			|| GetProtoID() == MONSTER_FLUWITON_ROOM_4_II
			|| GetProtoID() == MONSTER_FLUWITON_ROOM_4_III
			|| GetProtoID() == MONSTER_FLUWITON_ROOM_4_VI)
		{
			int32 sRandSkill = myrand(0, 400);

			if (sRandSkill < 30)
				return MagicPacket(MAGIC_CASTING, (uint32)502022, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			if (sRandSkill < 60)
				return MagicPacket(MAGIC_CASTING, (uint32)502023, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			else
				return MagicPacket(MAGIC_CASTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
		}
		else if (GetProtoID() == MONSTER_ElITE_TIMARLI
			|| GetProtoID() == MONSTER_ElITE_TIMARLI
			|| GetProtoID() == MONSTER_ElITE_TIMARLI)
		{
			if (m_proto->m_iMagic1 != 0)
				return MagicPacket(MAGIC_FLYING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
		}
		else if (GetProtoID() == MONSTER_FLUWITON_ROOM_3_I
			|| GetProtoID() == MONSTER_FLUWITON_ROOM_3_II
			|| GetProtoID() == MONSTER_FLUWITON_ROOM_3_III)
		{
			int32 sRand = myrand(0, 800);

			if (sRand < 200)
			{
				if (sRand < 10)
					return MagicPacket(MAGIC_CASTING, (uint32)502020, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 20)
					return MagicPacket(MAGIC_CASTING, (uint32)502021, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 40)
					return MagicPacket(MAGIC_CASTING, (uint32)502031, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 60)
					return MagicPacket(MAGIC_CASTING, (uint32)502019, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 80)
					return MagicPacket(MAGIC_CASTING, (uint32)502028, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 100)
					return MagicPacket(MAGIC_CASTING, (uint32)502029, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 120)
					return MagicPacket(MAGIC_CASTING, (uint32)502030, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else if (sRand < 140)
					return MagicPacket(MAGIC_CASTING, (uint32)502019, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				else
				{
					if (m_proto->m_iMagic2 != 0)
						return MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
				}
			}
			else
			{
				if (m_proto->m_iMagic1 != 0)
					return MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			}
		}
		else
		{
			int32 sRandSkill = myrand(0, 400);

			if (sRandSkill < 30 && m_proto->m_iMagic2 != 0)
				return MagicPacket(MAGIC_CASTING, m_proto->m_iMagic2, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			if (sRandSkill < 60 && m_proto->m_iMagic3 != 0)
				return MagicPacket(MAGIC_CASTING, m_proto->m_iMagic3, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
			else
				return MagicPacket(MAGIC_CASTING, m_proto->m_iMagic1, GetID(), pUser->GetID(), int16(pUser->GetX()), int16(pUser->GetY()), int16(pUser->GetZ()));
		}
	}
	else // Target monster/NPC 
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(nID, GetZoneID());
		if (pNpc == nullptr
			|| pNpc->isDead())
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return nStandingTime;
		}

		if (isGuardSummon())
		{
			if (m_proto->m_iMagic1 != 0)
				return MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic1, GetID(), pNpc->GetID(), int16(pNpc->GetX()), int16(pNpc->GetY()), int16(pNpc->GetZ()));
			if (m_proto->m_iMagic2 != 0)
				return MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic2, GetID(), pNpc->GetID(), int16(pNpc->GetX()), int16(pNpc->GetY()), int16(pNpc->GetZ()));
			if (m_proto->m_iMagic3 != 0)
				return MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), pNpc->GetID(), int16(pNpc->GetX()), int16(pNpc->GetY()), int16(pNpc->GetZ()));
		}
	}

	return m_sAttackDelay;
}
#pragma endregion

/**
* @brief	Sets the NPC alive if dead.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcLive()
time_t CNpc::NpcLive()
{
	// Dungeon Work 
	if (m_byRegenType == 2 || (m_byRegenType == 1 && m_byChangeType == 100))
	{
		StateChange(NPC_LIVE);
		return m_sRegenTime;
	}

	StateChange(SetLive() ? NPC_STANDING : NPC_LIVE);
	return m_sStandTime;
}
#pragma endregion

/**
* @brief	The state of the NPC when tracing an enemy or a predefined path.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcTracing()
time_t CNpc::NpcTracing()
{
	if (m_sStepCount != 0)
	{
		if (m_fPrevX < 0 || m_fPrevZ < 0)
		{
			TRACE("### Npc-NpcTracing  Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), m_fPrevX, m_fPrevZ);
		}
		else
		{
			m_curx = m_fPrevX;
			m_curz = m_fPrevZ;
		}
	}

	if (isNonAttackingObject())
	{
		InitTarget();
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	// Prevent spawns like Guard Towers from following
	// targets while attacking.
	if (m_byMoveType == 4
		|| m_byMoveType == 5)
	{
		if (m_byMoveType == 4)
			InitTarget();

		StateChange(NPC_FIGHTING);
		return m_sStandTime;
	}

	auto result = IsCloseTarget(m_byAttackRange, AttackTypePhysical);
	if (result == CloseTargetInGeneralRange)
	{
		NpcMoveEnd();
		StateChange(NPC_FIGHTING);
		return 0;
	}
	else if (result == CloseTargetInvalid)
	{
		InitTarget();
		NpcMoveEnd();
		StateChange(NPC_STANDING);
		isReturningSpawnSlot = true;
		return m_sStandTime;
	}
	else if (result == CloseTargetInAttackRange && GetProto()->m_byDirectAttack == 2)
	{
		NpcMoveEnd();
		StateChange(NPC_FIGHTING);
		return 0;
	}

	if (m_byActionFlag == ATTACK_TO_TRACE)
	{
		if (!m_bTracing)
		{
			m_fTracingStartX = GetX();
			m_fTracingStartZ = GetZ();
			m_bTracing = true;
			m_Target.m_tLastDamageTime = UNIXTIME2;
		}
	}

	if ((m_Target.m_tLastDamageTime > 0 && (UNIXTIME2 - m_Target.m_tLastDamageTime> 12000)))
	{
		m_byActionFlag = NO_ACTION;
		m_bStopFollowingTarget = true;
	}

	if (m_bStopFollowingTarget)
	{
		InitTarget();
		m_bStopFollowingTarget = false;
		if (!ResetPath())
		{
			TRACE("### NpcTracing Fail : [%s] NpcState set to NPC_STANDIN####\n", GetName().c_str());
			NpcMoveEnd();
			StateChange(NPC_STANDING);
			isReturningSpawnSlot = false;
			return m_sStandTime; //m_sStandTime;
		}
	}

	int nValue = GetTargetPath();
	if (nValue == -1)
	{
		if (!RandomMove())
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return m_sStandTime;
		}

		InitTarget();
		StateChange(NPC_MOVING);
		return m_sSpeed;
	}
	else if (nValue == 0)
	{
		m_fSecForMetor = m_fSpeed_2;
		IsNoPathFind(m_fSecForMetor);
	}

	if ((!m_bPathFlag && !StepMove())
		|| (m_bPathFlag && !StepNoPathMove()))
	{
		StateChange(NPC_STANDING);
		TRACE("### NpcTracing Fail : StepMove %s, %d ### \n", GetName().c_str(), GetID());
		return m_sStandTime;
	}

	if (IsMovingEnd())
		SendMoveResult(GetX(), GetY(), GetZ());
	else
		SendMoveResult(m_fPrevX, m_fPrevY, m_fPrevZ, (float)(m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000)));

	if (result == CloseTargetInAttackRange
		&& GetProto()->m_byDirectAttack == 0
		&& !isHealer()
		&& !isReturningSpawnSlot)
		TracingAttack();

	return m_sSpeed;
}
#pragma endregion

/**
* @brief	The state of the NPC when atacking an enemy.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcAttacking()
time_t CNpc::NpcAttacking()
{
	if (isDead())
		return -1;

	if (isNonAttackingObject())
	{
		StateChange(NPC_STANDING);
		return m_sStandTime / 2;
	}

	auto result = IsCloseTarget(m_byAttackRange, AttackTypeNone);
	if (result == CloseTargetInGeneralRange || result == CloseTargetInAttackRange)
	{
		if (isGuardSummon())
			StateChange(NPC_TRACING);
		else
			StateChange(NPC_FIGHTING);

		return 0;
	}
	else if (isGuardTower())
	{
		StateChange(NPC_TRACING);
		return 0;
	}

	int nValue = GetTargetPath();
	if (nValue == -1)
	{
		if (!RandomMove())
		{
			InitTarget();
			StateChange(NPC_STANDING);
			return m_sStandTime;
		}

		InitTarget();
		StateChange(NPC_MOVING);
		return m_sSpeed;
	}
	else if (nValue == 0)
	{
		m_fSecForMetor = m_fSpeed_2;
		IsNoPathFind(m_fSecForMetor);
	}

	StateChange(NPC_TRACING);
	return 0;
}
#pragma endregion

/**
* @brief	The state of the NPC when moving on the defined or calculated path.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcMoving()
time_t CNpc::NpcMoving()
{
	if (isDead())
		return -1;

	if (m_sStepCount != 0)
	{
		if (m_fPrevX < 0 || m_fPrevZ < 0)
		{
			TRACE("### Npc-Moving Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), m_fPrevX, m_fPrevZ);
		}
		else
		{
			m_curx = m_fPrevX;
			m_curz = m_fPrevZ;
		}
	}

	if (FindEnemy())
	{
		NpcMoveEnd();
		StateChange(NPC_ATTACKING);
		m_Delay = m_sSpeed;
		m_fDelayTime = getMSTime();
		return m_Delay;
	}

	if (IsMovingEnd())
	{
		m_curx = m_fPrevX;
		m_curz = m_fPrevZ;

		if (GetX() < 0 || GetZ() < 0)	
			TRACE("Npc-NpcMoving-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());

		SendMoveResult(m_fPrevX, m_fPrevY, m_fPrevZ);
		StateChange(NPC_STANDING);

		if (isInSpawnRange((int)GetX(), (int)GetZ()))
			isReturningSpawnSlot = false;

		if (isReturningSpawnSlot == true)
			return m_sSpeed; //m_sStandTime /2;
		else
			return m_sStandTime;
	}

	if ((!m_bPathFlag && !StepMove())
		|| (m_bPathFlag && !StepNoPathMove()))
	{
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	SendMoveResult(m_fPrevX, m_fPrevY, m_fPrevZ, (float)(m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000)));

	if (isReturningSpawnSlot == true)
		return m_sSpeed / 2;
	else
		return m_sSpeed;
}
#pragma endregion

/**
* @brief	The state of the NPC when sleeping. This could be either its night or
*			Priest's sleeping skills might have been applied on that NPC/MOB
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcSleeping()
time_t CNpc::NpcSleeping()
{
	if (UNIXTIME < m_tFaintingTime)
		return m_sStandTime;

	StateChangeServerDirect(1, 5);
	m_tFaintingTime = 0;
	return 0;
}
#pragma endregion

/**
* @brief	The state of the NPC when fainting. This could happen when the NPC/MOB
*			gets stunned by a stun skill. AFAIK, a NPC/MOB cannot get stunned by
*			a warrior stun skill but with a very low change by a lightning mage skill.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcFainting()
time_t CNpc::NpcFainting()
{
	if (UNIXTIME < (m_tFaintingTime + FAINTING_TIME))
		return -1;

	StateChangeServerDirect(1, 6);
	m_tFaintingTime = 0;
	return 0;
}
#pragma endregion

/**
* @brief	The state of the NPC when healing
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcHealing()
time_t CNpc::NpcHealing()
{
	if (!isHealer())
	{
		InitTarget();
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	auto result = IsCloseTarget(m_byAttackRange, AttackTypeMagic);
	if (result == CloseTargetNotInRange)
	{
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInAttackRange)
	{
		if (GetProto()->m_byDirectAttack == 2)
			return LongAndMagicAttack();

		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		StateChange(NPC_TRACING);
		return 0;
	}
	else if (result == CloseTargetInvalid)
	{
		StateChange(NPC_STANDING);
		InitTarget();
		return 0;
	}

	if (hasTarget()
		&& m_Target.id >= NPC_BAND)
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(m_Target.id, GetZoneID());
		if (pNpc == nullptr
			|| pNpc->isDead())
		{
			InitTarget();
			return m_sStandTime;
		}

		int iHP = (int)(pNpc->GetMaxHealth() * 0.9);
		if (pNpc->GetHealth() >= iHP)
		{
			InitTarget();
		}
		else
		{
			MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), m_Target.id);
			return m_sAttackDelay;
		}
	}

	int iMonsterNid = FindFriend(MonSearchNeedsHealing);
	if (iMonsterNid == 0)
	{
		InitTarget();
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	MagicPacket(MAGIC_EFFECTING, m_proto->m_iMagic3, GetID(), iMonsterNid);
	return m_sAttackDelay;
}
#pragma endregion

/**
* @brief	The state of the NPC when casting a skill upon a unit.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcCasting()
time_t CNpc::NpcCasting()
{
	if (isDead())
		return -1;

	// Officially the attack delay overlaps with the cast time, so more often than not
	// by the time the skill's cast, there should be very little to no delay for all cases I can see.
	// Regardless, we'll allow for longer delays if set.
	// NOTE: If it goes below 0 (which it will most of the time), the caller won't care to handle it.
	time_t tAttackDelay = m_sAttackDelay - m_sActiveCastTime;

	MagicPacket(MAGIC_EFFECTING, m_nActiveSkillID, GetID(), m_sActiveTargetID, m_sSkillX, m_sSkillY, m_sSkillZ);

	StateChange(m_OldNpcState);
	m_nActiveSkillID = 0;
	m_sActiveTargetID = -1;
	m_sActiveCastTime = 0;
	m_sSkillX = 0;
	m_sSkillY = 0;
	m_sSkillZ = 0;

	return tAttackDelay;
}
#pragma endregion

/**
* @brief	Handles the NPC state NPC_STANDING.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcCasting()
time_t CNpc::NpcStanding()
{
	KOMap* pMap = GetMap();
	if (pMap == nullptr)
	{
		TRACE("### NpcStanding Zone Index Error : nid=%d, name=%s, zone=%d ###\n", GetID(), GetName().c_str(), GetZoneID());
		return -1;
	}

	if (RandomMove())
	{
		m_iAniFrameCount = 0;
		StateChange(NPC_MOVING);

		if (isReturningSpawnSlot == true)
			return m_sStandTime / 2;
		else
			return m_sStandTime;
	}

	StateChange(NPC_STANDING);

	if (GetType() == NPC_SPECIAL_GATE 
		&& GetMap()->isWarZone() 
		&& g_pMain->m_byBattleOpen == NATION_BATTLE 
		|| GetType() == NPC_SPECIAL_GATE 
		&& GetMap()->isWarZone() 
		&& g_pMain->m_byBattleOpen == SNOW_BATTLE)
	{
		if (m_byGateOpen == 0 
			&& GetType() == NPC_SPECIAL_GATE)
		{
			m_byGateOpen = 1;
			SendGateFlag(m_byGateOpen);
			return m_sStandTime * 10;
		}
		else if (m_byGateOpen == 1 
			&& GetType() == NPC_SPECIAL_GATE)
		{
			m_byGateOpen = 0;
			SendGateFlag(m_byGateOpen);
			return m_sStandTime * 60;
		}
	}
	/*else if (GetType() == NPC_SPECIAL_GATE 
		&& GetZoneID() == ZONE_DELOS 
		&& g_pMain->m_byBattleOpen == SIEGE_BATTLE)
	{
		if (m_byGateOpen == 0 && GetType() == NPC_SPECIAL_GATE)
		{
			m_byGateOpen = 1;
			SendGateFlag(m_byGateOpen);
			return m_sStandTime * 30;
		}
		else if (m_byGateOpen == 1 
			&& GetType() == NPC_SPECIAL_GATE)
		{
			m_byGateOpen = 0;
			SendGateFlag(m_byGateOpen);
			return m_sStandTime * 60;
		}
	}*/
	else if (GetType() == NPC_OBJECT_WOOD
		&& GetMap()->isWarZone()
		&& g_pMain->m_byBattleOpen == NATION_BATTLE)
	{
		if (m_byGateOpen == 1
			&& GetType() == NPC_OBJECT_WOOD
			&& WoodCooldownClose++ >= 30)
		{
			StateChange(SetLive() ? NPC_STANDING : NPC_LIVE);
			m_byGateOpen = 0;
			SendGateFlag(m_byGateOpen);
			WoodCooldownClose = 0;
		}
	}
	else if (GetType() == NPC_KROWAZ_GATE
		&& GetZoneID() == ZONE_KROWAZ_DOMINION)
	{
		if (m_byGateOpen == 1
			&& GetType() == NPC_KROWAZ_GATE)
		{
			m_byGateOpen = 0;
			SendGateFlag(m_byGateOpen);
			return m_sStandTime * 10;
		}
	}
	else if (GetType() == NPC_ROLLINGSTONE
		&& GetZoneID() == ZONE_KROWAZ_DOMINION)
	{
		StateChange(SetLive() ? NPC_DEAD : NPC_LIVE);
	}
	return m_sStandTime;
}
#pragma endregion

/**
* @brief	Handles the NPC state NPC_BACK.
*
* @return	Returns the next time that it shall be executed by the NpcThread proc.
*/
#pragma region time_t CNpc::NpcCasting()
time_t CNpc::NpcBack()
{
	if (isDead())
		return -1;

	if ((hasTarget() && g_pMain->GetUnitPtr(m_Target.id, GetZoneID()) == nullptr)
		|| (m_Target.m_tLastDamageTime > 0 && (UNIXTIME2 - m_Target.m_tLastDamageTime> 6000)))
	{
		StateChange(NPC_STANDING);
		return m_sSpeed;//STEP_DELAY;
	}

	if (m_sStepCount != 0)
	{
		if (m_fPrevX < 0 || m_fPrevZ < 0)
		{
			TRACE("### Npc-NpcBack Fail-1 : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), m_fPrevX, m_fPrevZ);
		}
		else
		{
			m_curx = m_fPrevX;
			m_curz = m_fPrevZ;
		}
	}

	if (IsMovingEnd())
	{
		m_curx = m_fPrevX;
		m_curz = m_fPrevZ;

		if (GetX() < 0 || GetZ() < 0)
		{
			TRACE("Npc-NpcBack-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());
			return m_sStandTime;
		}
		SendMoveResult(GetX(), GetY(), GetZ());
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	if ((!m_bPathFlag && !StepMove())
		|| (m_bPathFlag && !StepNoPathMove()))
	{
		StateChange(NPC_STANDING);
		return m_sStandTime;
	}

	SendMoveResult(m_fPrevX, m_fPrevY, m_fPrevZ, (float)(m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000)));
	return m_sSpeed;
}
#pragma endregion

#pragma endregion

/*
* @brief	Sets a NPC alive. Sets all of its parametes to defaults
*			or zero depending on their purposes.
*/
#pragma region bool CNpc::SetLive()
bool CNpc::SetLive()
{
	int i = 0, j = 0;
	m_iHP = m_MaxHP;
	m_sMP = m_MaxMP;
	m_sPathCount = 0;
	m_iPatternFrame = 0;
	m_bStopFollowingTarget = false;
	m_byActionFlag = NO_ACTION;
	m_byMaxDamagedNation = 0;

	if (m_proto->m_byGroupSpecial > 0)
		m_bNation = m_proto->m_byGroupSpecial;

	m_sRegionX = m_sRegionZ = -1;
	m_fAdd_x = 0.0f;
	m_fAdd_z = 0.0f;
	m_fStartPoint_X = 0.0f;
	m_fStartPoint_Y = 0.0f;
	m_fEndPoint_X = 0.0f;
	m_fEndPoint_Y = 0.0f;
	m_min_x = m_min_y = m_max_x = m_max_y = 0;
	m_buffLock.lock();
	m_buffMap.clear();
	m_buffLock.unlock();
	InitTarget();
	InitUserList();
	ClearPathFindData();
	m_sMaxDamageUserid = -1;
	m_TotalDamage = 0;

	if (m_bIsEventNpc && !m_bFirstLive)
	{
		TRACE("Npc-delete : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());
		m_bDelete = true;
		m_tDeleteTime = uint32(UNIXTIME);
		return true;
	}

	KOMap* pMap = GetMap();
	if (pMap == nullptr)
		return false;

	if (m_bFirstLive)
	{
		m_nInitX = m_fPrevX = GetX();
		m_nInitY = m_fPrevY = GetY();
		m_nInitZ = m_fPrevZ = GetZ();
	}

	if (GetX() < 0 || GetZ() < 0)
	{
		TRACE("Npc-SetLive-1 : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());
		return false;
	}

	int dest_x = (int)m_nInitX / TILE_SIZE;
	int dest_z = (int)m_nInitZ / TILE_SIZE;

	bool bMove = pMap->IsMovable(dest_x, dest_z);

	if (GetType() != NPCTYPE_MONSTER || m_bIsEventNpc)
	{
		m_curx = m_fPrevX = m_nInitX;
		m_cury = m_fPrevY = m_nInitY;
		m_curz = m_fPrevZ = m_nInitZ;
	}
	else
	{
		int nX = 0;
		int nZ = 0;
		int nTileX = 0;
		int nTileZ = 0;
		int nRandom = 0;

		while (1)
		{
			i++;
			nRandom = abs(m_nInitMinX - m_nInitMaxX);
			if (nRandom <= 1)
				nX = m_nInitMinX;
			else
			{
				if (m_nInitMinX < m_nInitMaxX)
					nX = myrand(m_nInitMinX, m_nInitMaxX);
				else
					nX = myrand(m_nInitMaxX, m_nInitMinX);
			}
			nRandom = abs(m_nInitMinY - m_nInitMaxY);
			if (nRandom <= 1)
				nZ = m_nInitMinY;
			else
			{
				if (m_nInitMinY < m_nInitMaxY)
					nZ = myrand(m_nInitMinY, m_nInitMaxY);
				else
					nZ = myrand(m_nInitMaxY, m_nInitMinY);
			}

			nTileX = nX / TILE_SIZE;
			nTileZ = nZ / TILE_SIZE;

			if (nTileX >= pMap->GetMapSize())
				nTileX = pMap->GetMapSize();
			if (nTileZ >= pMap->GetMapSize())
				nTileZ = pMap->GetMapSize();

			if (nTileX < 0 || nTileZ < 0)
			{
				TRACE("#### Npc-SetLive() Fail : nTileX=%d, nTileZ=%d #####\n", nTileX, nTileZ);
				return false;
			}

			m_nInitX = m_fPrevX = m_curx = (float)nX;
			m_nInitZ = m_fPrevZ = m_curz = (float)nZ;

			if (GetX() < 0 || GetZ() < 0)
			{
				TRACE("Npc-SetLive-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", GetID(), GetName().c_str(), GetX(), GetZ());
			}
			break;
		}
	}

	m_fHPChangeTime = getMSTime();
	m_fHPType4CheckTime = getMSTime();
	m_tFaintingTime = 0;

	if (m_bFirstLive)
	{
		switch (m_byActType)
		{
		case 1:
		case 2:
			m_tNpcAttType = TENDER_ATTACK_TYPE;
			break;

		case 3:
		case 4:
			if (GetZoneID() == ZONE_ARDREAM
				|| GetZoneID() == ZONE_RONARK_LAND_BASE
				|| GetZoneID() == ZONE_RONARK_LAND)
				m_bHasFriends = false; // yay!
			else
				m_bHasFriends = true; // yay!
			m_tNpcAttType = TENDER_ATTACK_TYPE;
			break;

		case 6:
			break;

		default:
			m_tNpcAttType = ATROCITY_ATTACK_TYPE;
		}

		m_bFirstLive = false;

		if (g_pMain->m_CurrentNPC.increment() == g_pMain->m_TotalNPC
			&& !m_bIsEventNpc)
		{
			TRACE("Monster All Init Success - %d\n", (uint16)g_pMain->m_TotalNPC);
		}
	}

	if (m_byMoveType == 3 && m_sMaxPathCount == 2)
	{
		__Vector3 vS, vE, vDir;
		float fDir;
		vS.Set((float)m_PathList.pPatternPos[0].x, 0, (float)m_PathList.pPatternPos[0].z);
		vE.Set((float)m_PathList.pPatternPos[1].x, 0, (float)m_PathList.pPatternPos[1].z);
		vDir = vE - vS;
		vDir.Normalize();
		Yaw2D(vDir.x, vDir.z, fDir);
		m_byDirection = (uint8)fDir;
	}

	RegisterRegion();
	m_byDeadType = 0;

	SendInOut(INOUT_IN, GetX(), GetZ(), GetY());
	pKiller = nullptr;
	m_bSendDeathPacket = true;
	m_iSpawnedTime = int32(UNIXTIME);
	return true;
}
#pragma endregion

/**
* @brief	Clears the damaged user list of this NPC.
*
*/
#pragma region void CNpc::InitUserList()
void CNpc::InitUserList()
{
	m_sMaxDamageUserid = -1;
	m_TotalDamage = 0;
	Guard lock(m_damageListLock);
	m_DamagedUserList.clear();
}
#pragma endregion 

/**
* @brief	Checks whether this NPC/MOB is in its spawn slot area.
*
*/
#pragma region bool CNpc::isInSpawnRange(int nX, int nZ)
bool CNpc::isInSpawnRange(int nX, int nZ)
{
	CRect r(m_nLimitMinX, m_nLimitMinZ, m_nLimitMaxX, m_nLimitMaxZ);
	return r.PtInRect(nX, nZ);
}
#pragma endregion

/**
* @brief	Handles the random movement operations of a NPC or MONSTER.
*
* @return	Returns true if a true path was found.
*/
#pragma region bool CNpc::RandomMove()
bool CNpc::RandomMove()
{
#pragma region Random Move Eligiblity Controls
	if (GetMap() == nullptr
		|| (m_bySearchRange == 0)
		// 0 means no move type, but moves when recvs attack.
		|| (m_byMoveType == 0)
		// if no-user in view, then why bother with moving, let it stay standing.
		|| (!GetUserInView() && GetType() != NPC_ROLLINGSTONE && GetType() != NPC_OBJECT_WOOD)
		// 4 means non-moving. 
		|| (m_byMoveType == 4)
		// GuardSummon
		|| (isGuardSummon()))
		return false;
#pragma endregion
	
#pragma region Local Variable Definitions

	float fDestX = -1.0f, fDestZ = -1.0f;
	int max_xx = GetMap()->GetMapSize();
	int max_zz = GetMap()->GetMapSize();
	int x = 0, y = 0;

	__Vector3 vStart, vEnd, vNewPos;
	float fDis = 0.0f;

	int nPathCount = 0;

	int random_x = 0, random_z = 0;
	bool bPeedBack = false;

	m_fSecForMetor = m_fSpeed_1;

#pragma endregion

#pragma region  Npc Move Type = 1
	
	if (m_byMoveType == 1)	// we are randomly moving around
	{
		// if we are at the beginning of the movement, then we shall
		// store the initial position of our NPC/MOB to be able to 
		// return back to that position after some moves.
		if (m_iPatternFrame == 0)
		{
			m_pPatternPos.x = (short)m_nInitX;
			m_pPatternPos.z = (short)m_nInitZ;
		}

		random_x = myrand(-((int)m_fSecForMetor), (int)m_fSecForMetor); // randomly we move in X axis inbetween 0 - 3 pixels
		random_z = myrand(-((int)m_fSecForMetor), (int)m_fSecForMetor); // randomly we move in Z axis inbetween 0 - 3 pixels


		fDestX = GetX() + (float)random_x; // our next pattern X position
		fDestZ = GetZ() + (float)random_z; // our next pattern Z position

		if (m_iPatternFrame == 2)
		{
			fDestX = m_pPatternPos.x;
			fDestZ = m_pPatternPos.z;
			m_iPatternFrame = 0;
		}
		else
		{
			m_iPatternFrame++;
		}

		vStart.Set(GetX(), 0, GetZ());
		vEnd.Set(fDestX, 0, fDestZ);
		if (!isInSpawnRange((int)GetX(), (int)GetZ()))
		{
			if (isReturningSpawnSlot)
			{
				m_fSecForMetor = m_fSpeed_2;
				fDestX = m_nInitX;
				fDestZ = m_nInitZ;
				m_iPatternFrame = 2;
			}
			bPeedBack = true;
		}
	}
#pragma endregion

#pragma region  Npc Move Type = 2
	else if (m_byMoveType == 2)  // we are moving on a predefined path (kaul in moradon, saw blade in chaos dungeon, etc.)
	{
		if (m_sPathCount == m_sMaxPathCount)
			m_sPathCount = 0;

		if (m_sPathCount != 0 && IsInPathRange() == false)
		{
			m_sPathCount--;
			nPathCount = GetNearPathPoint();

			if (nPathCount == -1)
			{
				TRACE("##### RandomMove Fail : [nid = %d, sid=%d], path = %d/%d, NPC with Path type 2 #####\n",
				GetID(), GetProtoID(), m_sPathCount, m_sMaxPathCount);

				vStart.Set(GetX(), 0, GetZ());
				fDestX = (float)m_PathList.pPatternPos[0].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPatternPos[0].z + m_fBattlePos_z;
				vEnd.Set(fDestX, 0, fDestZ);
				GetVectorPosition(vStart, vEnd, 40, &vNewPos);
				fDestX = vNewPos.x;
				fDestZ = vNewPos.z;
			}
			else
			{
				if (nPathCount < 0)
					return false;

				fDestX = (float)m_PathList.pPatternPos[nPathCount].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPatternPos[nPathCount].z + m_fBattlePos_z;
				m_sPathCount = nPathCount;
			}
		}
		else
		{
			if (m_sPathCount < 0)
				return false;

			fDestX = (float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z;
		}

		m_sPathCount++;
	}
#pragma endregion

#pragma region Npc Move Type = 3
	else if (m_byMoveType == 3)
	{
		if (m_sPathCount == m_sMaxPathCount)
		{
			m_byMoveType = 0;
			m_sPathCount = 0;
			return false;
		}

		if (m_sPathCount != 0 && IsInPathRange() == false)
		{
			m_sPathCount--;
			nPathCount = GetNearPathPoint();
			if (nPathCount == -1)
			{
				TRACE("##### RandomMove Fail : [nid = %d, sid=%d], path = %d/%d, NPC with pathlist type 3 #####\n", GetID(), GetProtoID(), m_sPathCount, m_sMaxPathCount);
				vStart.Set(GetX(), 0, GetZ());
				fDestX = (float)m_PathList.pPatternPos[0].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPatternPos[0].z + m_fBattlePos_z;
				vEnd.Set(fDestX, 0, fDestZ);
				GetVectorPosition(vStart, vEnd, 40, &vNewPos);
				fDestX = vNewPos.x;
				fDestZ = vNewPos.z;
			}
			else
			{
				if (nPathCount < 0)
					return false;

				fDestX = (float)m_PathList.pPatternPos[nPathCount].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPatternPos[nPathCount].z + m_fBattlePos_x;
				m_sPathCount = nPathCount;
			}
		}
		else
		{
			if (m_sPathCount < 0)
				return false;

			fDestX = (float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_x;
		}

		m_sPathCount++;
	}
#pragma endregion

#pragma region  Npc Move Type = 5
	else if (m_byMoveType == 5)  // we are moving on a predefined path (kaul in moradon, saw blade in chaos dungeon, etc.)
	{
		if (isGuard() || isGuardTower())
			return false;

		switch (GetZoneID())
		{
		case ZONE_KROWAZ_DOMINION:
			if (GetType() != NPC_ROLLINGSTONE)
				return false;

			fDestX = (float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z;

			if (m_sPathCount++ >= m_sMaxPathCount)
				Dead(this);
			break;
		case ZONE_BATTLE6:
			if (GetType() != NPC_OBJECT_WOOD)
				return false;

			if (m_byGateOpen == 0)
				return false;

			fDestX = (float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z;

			m_byGateOpen = 1;
			SendGateFlag(m_byGateOpen);

			if (m_sPathCount++ >= m_sMaxPathCount)
				Dead(this);
			break;
		default:
		{
			if (m_sPathCount == m_sMaxPathCount)
				m_sPathCount = 0;

			if (m_sPathCount != 0 && IsInPathRange() == false)
			{
				m_sPathCount--;
				nPathCount = GetNearPathPoint();

				if (nPathCount == -1)
				{
					TRACE("##### RandomMove Fail : [nid = %d, sid=%d], path = %d/%d, NPC with Path type 2 #####\n",
					GetID(), GetProtoID(), m_sPathCount, m_sMaxPathCount);

					vStart.Set(GetX(), 0, GetZ());
					fDestX = (float)m_PathList.pPatternPos[0].x + m_fBattlePos_x;
					fDestZ = (float)m_PathList.pPatternPos[0].z + m_fBattlePos_z;
					vEnd.Set(fDestX, 0, fDestZ);
					GetVectorPosition(vStart, vEnd, 40, &vNewPos);
					fDestX = vNewPos.x;
					fDestZ = vNewPos.z;
				}
				else
				{
					if (nPathCount < 0)
						return false;

					fDestX = (float)m_PathList.pPatternPos[nPathCount].x + m_fBattlePos_x;
					fDestZ = (float)m_PathList.pPatternPos[nPathCount].z + m_fBattlePos_z;
					m_sPathCount = nPathCount;
				}
			}
			else
			{
				if (m_sPathCount < 0)
					return false;

				fDestX = (float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z;
			}
		}break;
		}
		m_sPathCount++;
	}
#pragma endregion

#pragma region Common Random Move Operations

	vStart.Set(GetX(), 0, GetZ());
	vEnd.Set(fDestX, 0, fDestZ);

	if (GetX() < 0 || GetZ() < 0 || fDestX < 0 || fDestZ < 0)
	{
		//TRACE("##### m_byMoveType = %d  [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f#####\n", m_byMoveType, GetID(), GetName().c_str(), GetX(), GetZ(), fDestX, fDestZ);
		return false;
	}

	int mapWidth = (int)(max_xx * GetMap()->GetUnitDistance());

	if (GetX() >= mapWidth || GetZ() >= mapWidth || fDestX >= mapWidth || fDestZ >= mapWidth)
	{
		TRACE("##### RandomMove Fail : value is overflow .. [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f#####\n", 
			GetID(), GetName().c_str(), GetX(), GetZ(), fDestX, fDestZ);
		return false;
	}

	if (GetType() == NPC_DUNGEON_MONSTER
		&& !isInSpawnRange((int)fDestX, (int)fDestZ))
		return false;

	fDis = GetDistance(vStart, vEnd);
	if (fDis > NPC_MAX_MOVE_RANGE)
	{
		if (m_byMoveType == 2 
			|| m_byMoveType == 3
			|| m_byMoveType == 5)
		{
			if (--m_sPathCount <= 0)
				m_sPathCount = 0;
		}

		if (!isReturningSpawnSlot)
			return false;
	}

	if (fDis <= m_fSecForMetor)
	{
		ClearPathFindData();
		m_fStartPoint_X = GetX();
		m_fStartPoint_Y = GetZ();
		m_fEndPoint_X = fDestX;
		m_fEndPoint_Y = fDestZ;
		m_bPathFlag = true;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		return true;
	}

	float fTempRange = (float)fDis;
	int min_x = (int)(GetX() - fTempRange) / TILE_SIZE;	if (min_x < 0) min_x = 0;
	int min_z = (int)(GetZ() - fTempRange) / TILE_SIZE;	if (min_z < 0) min_z = 0;
	int max_x = (int)(GetX() + fTempRange) / TILE_SIZE;	if (max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(GetZ() + fTempRange) / TILE_SIZE;	if (min_z >= max_zz) min_z = max_zz - 1;

	CPoint start, end;
	start.x = (int)(GetX() / TILE_SIZE) - min_x;
	start.y = (int)(GetZ() / TILE_SIZE) - min_z;
	end.x = (int)(fDestX / TILE_SIZE) - min_x;
	end.y = (int)(fDestZ / TILE_SIZE) - min_z;

	if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
		return false;

	m_fStartPoint_X = GetX();
	m_fStartPoint_Y = GetZ();
	m_fEndPoint_X = fDestX;
	m_fEndPoint_Y = fDestZ;

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	if (m_byMoveType == 2 
		|| m_byMoveType == 3 
		|| m_byMoveType == 5
		|| bPeedBack == true)
	{
		IsNoPathFind(m_fSecForMetor);
		return true;
	}

	return PathFind(start, end, m_fSecForMetor) == 1;
#pragma endregion
}

#pragma endregion

/**
* @brief	This method supposedly handles NPC/MOBs returning spawned slot.
*			Yet, this isn't used for now.
*
* @return	Returns true if a true path was found.
*/
#pragma region bool CNpc::RandomBackMove()
bool CNpc::RandomBackMove()
{
	m_fSecForMetor = m_fSpeed_2;

	if (m_bySearchRange == 0) return false;

	float fDestX = -1.0f, fDestZ = -1.0f;
	if (GetMap() == nullptr)
	{
		TRACE("#### Npc-RandomBackMove Zone Fail : [name=%s], zone=%d #####\n", GetName().c_str(), GetZoneID());
		return false;
	}

	int max_xx = GetMap()->GetMapSize();
	int max_zz = GetMap()->GetMapSize();
	int x = 0, y = 0;
	float fTempRange = (float)m_bySearchRange * 2;
	int min_x = (int)(GetX() - fTempRange) / TILE_SIZE;	if (min_x < 0) min_x = 0;
	int min_z = (int)(GetZ() - fTempRange) / TILE_SIZE;	if (min_z < 0) min_z = 0;
	int max_x = (int)(GetX() + fTempRange) / TILE_SIZE;	if (max_x > max_xx) max_x = max_xx;
	int max_z = (int)(GetZ() + fTempRange) / TILE_SIZE;	if (max_z > max_zz) max_z = max_zz;

	__Vector3 vStart, vEnd, vEnd22;
	float fDis = 0.0f;
	vStart.Set(GetX(), GetY(), GetZ());

	uint16 nID = m_Target.id;					// Target À» ±¸ÇÑ´Ù.
	CUser* pUser = nullptr;

	int iDir = 0;

	int iRandomX = 0, iRandomZ = 0, iRandomValue = 0;
	iRandomValue = rand() % 2;

	// Head towards player
	if (nID < NPC_BAND)
	{
		pUser = g_pMain->GetUserPtr(nID);
		if (pUser == nullptr || !pUser->isInGame())
			return false;

		if ((int)pUser->GetX() != (int)GetX())
		{
			iRandomX = myrand((int)m_bySearchRange, (int)(m_bySearchRange*1.5));
			iRandomZ = myrand(0, (int)m_bySearchRange);

			if ((int)pUser->GetX() > (int)GetX())
				iDir = 1;
			else
				iDir = 2;
		}
		else	// zÃàÀ¸·Î
		{
			iRandomZ = myrand((int)m_bySearchRange, (int)(m_bySearchRange*1.5));
			iRandomX = myrand(0, (int)m_bySearchRange);
			if ((int)pUser->GetZ() > (int)GetZ())
				iDir = 3;
			else
				iDir = 4;
		}

		switch (iDir)
		{
		case 1:
			fDestX = GetX() - iRandomX;
			if (iRandomValue == 0)
				fDestZ = GetZ() - iRandomX;
			else
				fDestZ = GetZ() + iRandomX;
			break;
		case 2:
			fDestX = GetX() + iRandomX;
			if (iRandomValue == 0)
				fDestZ = GetZ() - iRandomX;
			else
				fDestZ = GetZ() + iRandomX;
			break;
		case 3:
			fDestZ = GetZ() - iRandomX;
			if (iRandomValue == 0)
				fDestX = GetX() - iRandomX;
			else
				fDestX = GetX() + iRandomX;
			break;
		case 4:
			fDestZ = GetZ() - iRandomX;
			if (iRandomValue == 0)
				fDestX = GetX() - iRandomX;
			else
				fDestX = GetX() + iRandomX;
			break;
		}

		vEnd.Set(fDestX, 0, fDestZ);
		fDis = GetDistance(vStart, vEnd);
		if (fDis > 20)	// 20¹ÌÅÍ ÀÌ»óÀÌ¸é 20¹ÌÅÍ·Î ¸ÂÃá´Ù,,
		{
			GetVectorPosition(vStart, vEnd, 20, &vEnd22);
			fDestX = vEnd22.x;
			fDestZ = vEnd22.z;
		}
	}
	// Head towards monster/NPC
	else
	{
	}

	CPoint start, end;
	start.x = (int)(GetX() / TILE_SIZE) - min_x;
	start.y = (int)(GetZ() / TILE_SIZE) - min_z;
	end.x = (int)(fDestX / TILE_SIZE) - min_x;
	end.y = (int)(fDestZ / TILE_SIZE) - min_z;

	if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
		return false;

	m_fStartPoint_X = GetX();		m_fStartPoint_Y = GetZ();
	m_fEndPoint_X = fDestX;	m_fEndPoint_Y = fDestZ;

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	int nValue = PathFind(start, end, m_fSecForMetor);
	if (nValue == 1)
		return true;

	return false;
}
#pragma endregion

/**
* @brief	Checks whether the next pattern among our pathlist
*			is in our range.
*/
#pragma region bool CNpc::IsInPathRange()
bool CNpc::IsInPathRange()
{
	if (m_sPathCount < 0)
		return false;

	static const float fPathRange = 40.0f;
	return isInRangeSlow((float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x,
		(float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z,
		fPathRange + 1);
}
#pragma endregion

/**
* @brief	Gets the nearest path point.
*/
#pragma region int CNpc::GetNearPathPoint()
int CNpc::GetNearPathPoint()
{
	__Vector3 vStart, vEnd;
	float fMaxPathRange = (float)NPC_MAX_MOVE_RANGE;
	float fDis1 = 0.0f, fDis2 = 0.0f;
	int nRet = -1;
	vStart.Set(GetX(), 0, GetZ());
	vEnd.Set((float)m_PathList.pPatternPos[m_sPathCount].x + m_fBattlePos_x, 0, (float)m_PathList.pPatternPos[m_sPathCount].z + m_fBattlePos_z);
	fDis1 = GetDistance(vStart, vEnd);

	if (m_sPathCount + 1 > m_sMaxPathCount)
	{
		if (m_sPathCount - 1 > 0)
		{
			vEnd.Set((float)m_PathList.pPatternPos[m_sPathCount - 1].x + m_fBattlePos_x, 0, (float)m_PathList.pPatternPos[m_sPathCount - 1].z + m_fBattlePos_z);
			fDis2 = GetDistance(vStart, vEnd);
		}
		else
		{
			vEnd.Set((float)m_PathList.pPatternPos[0].x + m_fBattlePos_x, 0, (float)m_PathList.pPatternPos[0].z + m_fBattlePos_z);
			fDis2 = GetDistance(vStart, vEnd);
		}
	}
	else
	{
		vEnd.Set((float)m_PathList.pPatternPos[m_sPathCount + 1].x + m_fBattlePos_x, 0, (float)m_PathList.pPatternPos[m_sPathCount + 1].z + m_fBattlePos_z);
		fDis2 = GetDistance(vStart, vEnd);
	}

	if (fDis1 <= fDis2)
	{
		if (fDis1 <= fMaxPathRange)
			nRet = m_sPathCount;
	}
	else
	{
		if (fDis2 <= fMaxPathRange)
			nRet = m_sPathCount + 1;
	}

	return nRet;
}
#pragma endregion

/**
* @brief	Finds enemy's region
*/
#pragma region int CNpc::FindEnemyRegion()
int CNpc::FindEnemyRegion()
{
	/*
	1	2	3
	4	0	5
	6	7	8
	*/
	int iRetValue = 0;
	int  iSX = GetRegionX() * VIEW_DIST;
	int  iSZ = GetRegionZ() * VIEW_DIST;
	int  iEX = (GetRegionX() + 1) * VIEW_DIST;
	int  iEZ = (GetRegionZ() + 1) * VIEW_DIST;
	int  iSearchRange = m_bySearchRange;
	int iCurSX = (int)GetX() - m_bySearchRange;
	int iCurSY = (int)GetX() - m_bySearchRange;
	int iCurEX = (int)GetX() + m_bySearchRange;
	int iCurEY = (int)GetX() + m_bySearchRange;

	int iMyPos = GetMyField();

	switch (iMyPos)
	{
	case 1:
		if ((iCurSX < iSX) && (iCurSY < iSZ))
			iRetValue = 1;
		else if ((iCurSX > iSX) && (iCurSY < iSZ))
			iRetValue = 2;
		else if ((iCurSX < iSX) && (iCurSY > iSZ))
			iRetValue = 4;
		else if ((iCurSX >= iSX) && (iCurSY >= iSZ))
			iRetValue = 0;
		break;
	case 2:
		if ((iCurEX < iEX) && (iCurSY < iSZ))
			iRetValue = 2;
		else if ((iCurEX > iEX) && (iCurSY < iSZ))
			iRetValue = 3;
		else if ((iCurEX <= iEX) && (iCurSY >= iSZ))
			iRetValue = 0;
		else if ((iCurEX > iEX) && (iCurSY > iSZ))
			iRetValue = 5;
		break;
	case 3:
		if ((iCurSX < iSX) && (iCurEY < iEZ))
			iRetValue = 4;
		else if ((iCurSX >= iSX) && (iCurEY <= iEZ))
			iRetValue = 0;
		else if ((iCurSX < iSX) && (iCurEY > iEZ))
			iRetValue = 6;
		else if ((iCurSX > iSX) && (iCurEY > iEZ))
			iRetValue = 7;
		break;
	case 4:
		if ((iCurEX <= iEX) && (iCurEY <= iEZ))
			iRetValue = 0;
		else if ((iCurEX > iEX) && (iCurEY < iEZ))
			iRetValue = 5;
		else if ((iCurEX < iEX) && (iCurEY > iEZ))
			iRetValue = 7;
		else if ((iCurEX > iEX) && (iCurEY > iEZ))
			iRetValue = 8;
		break;
	}

	if (iRetValue <= 0) // ÀÓ½Ã·Î º¸Á¤(¹®Á¦½Ã),, ÇÏ±â À§ÇÑ°Í..
		iRetValue = 0;

	switch (iRetValue)
	{
	case 0:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 1:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = -1;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = -1;
		m_iFind_X[2] = -1;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 2:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = -1;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 3:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 1;  m_iFind_Y[3] = 1;
		break;
	case 4:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 5:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 6:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = -1;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 1;
		break;
	case 7:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 8:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 1;  m_iFind_Y[3] = 1;
		break;
	}

	return iRetValue;
}
#pragma endregion

/**
* @brief	Checks whether this NPC can have an enemy at that moment.
*/
#pragma region bool CNpc::CheckFindEnemy()
bool CNpc::CheckFindEnemy()
{
	if (isDead())
		return false;

	// guards and guard towers ought to always be ready for searching for enemies.
	// no matter why
	if (isGuard() || isGuardTower() || isGuardSummon())
		return true;

	KOMap* pMap = GetMap();
	if (pMap == nullptr) // a map cannot be empty, if it is, then cannot find enemy
		return false;

	if (pMap->GetRegion(GetRegionX(), GetRegionZ()) == nullptr) // region cannot be empty, if it is, then cannot find enemy
		return false;
	else
		return pMap->GetRegion(GetRegionX(), GetRegionZ())->m_byMoving == 1; // if there is a user in the region, 

	return false;
}
#pragma endregion

/**
* @brief	Finds the enemies around that NPC/MOB and chooses one as
*			to be attacked.
*
* @return	Returns the compared distance on success.
*/
#pragma region float CNpc::FindEnemyExpand(int nRX, int nRZ, float fCompDis, UnitType unitType)
float CNpc::FindEnemyExpand(int nRX, int nRZ, float fCompDis, UnitType unitType)
{
	KOMap* pMap = GetMap();
	float fDis = 0.0f;
	if (pMap == nullptr)
		return fDis;

	float fComp = fCompDis;
	float fSearchRange = (float)m_bySearchRange;
	uint16 target_uid;
	__Vector3 vUser, vNpc, vMon;
	vNpc.Set(GetX(), GetY(), GetZ());

	// Finding players
	if (unitType == UnitPlayer)
	{
		int16 rx = nRX, rz = nRZ;
		foreach_region(x, z)
		{
			uint16 region_x = rx + x, region_z = rz + z;
			CRegion * pRegion = pMap->GetRegion(region_x, region_z);
			if (pRegion == nullptr)
				continue;

			pRegion->m_lockUserArray.lock();
			if (pRegion->m_RegionUserArray.size() <= 0)
			{
				pRegion->m_lockUserArray.unlock();
				continue;
			}

			ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
			pRegion->m_lockUserArray.unlock();
			foreach(itr, cm_RegionUserArray)
			{
				CUser *pUser = g_pMain->GetUserPtr(*itr);
				if (pUser == nullptr
					|| pUser->isDead()
					|| !CanAttack(pUser)
					|| pUser->m_bInvisibilityType
					|| pUser->isGM())
					continue;

				float fDis = Unit::GetDistanceSqrt(pUser);
				if (fDis > fSearchRange
					|| (fComp > 0 && fDis > fComp))
					continue;

				target_uid = pUser->GetID();
				fComp = fDis;

				// Aggressive spawns don't mind attacking first.
				if (m_tNpcAttType == ATROCITY_ATTACK_TYPE
					// Passive spawns will only attack if they've been attacked first, or they've got backup! (Cowards!)
					|| (m_tNpcAttType == TENDER_ATTACK_TYPE && (IsDamagedUserList(pUser) || (m_bHasFriends && m_Target.id == target_uid))))
				{
					m_Target.id = target_uid;
					m_Target.bSet = true;
					m_Target.x = pUser->GetX();
					m_Target.y = pUser->GetY();
					m_Target.z = pUser->GetZ();
					break;
				}
			}
		}
	}
	// Finding NPCs/monsters
	else if (unitType == UnitNPC)
	{
		CRegion * pRegion = pMap->GetRegion(nRX, nRZ);
		if (pRegion == nullptr)
			return 0.0f;

		pRegion->m_lockNpcArray.lock();
		if (pRegion->m_RegionNpcArray.size() <= 0)
		{
			pRegion->m_lockNpcArray.unlock();
			return 0.0f;
		}

		ZoneNpcArray cm_RegionNpcArray = pRegion->m_RegionNpcArray;
		pRegion->m_lockNpcArray.unlock();
		foreach(itr, cm_RegionNpcArray)
		{
			int nNpcid = (*itr);
			if (nNpcid < NPC_BAND)
				continue;

			CNpc *pNpc = g_pMain->GetNpcPtr(nNpcid, GetZoneID());

			if (pNpc == nullptr
				|| pNpc == this
				|| pNpc->isDead()
				|| pNpc->isNonAttackableObject()
				|| !isHostileTo(pNpc))
				continue;

			float fDis = Unit::GetDistanceSqrt(pNpc);
			if (fDis > fSearchRange
				|| (fComp > 0 && fDis > fComp))
				continue;

			target_uid = nNpcid;
			fComp = fDis;
			m_Target.id = target_uid;
			m_Target.bSet = true;
			m_Target.x = pNpc->GetX();
			m_Target.y = pNpc->GetY();
			m_Target.z = pNpc->GetZ();
		}
	}

	return fComp;
}
#pragma endregion

bool CNpc::isInMeleeArena()
{
	if (!isInMoradon())
		return false;

	if ((GetX() < 735.0f && GetX() > 684.0f) && (GetZ() < 491.0f && GetZ() > 440.0f))
		return true;

	return false;
}

bool CNpc::isInPartyArena()
{
	if (!isInMoradon())
		return false;

	if ((GetX() < 735.0f && GetX() > 684.0f) && (GetZ() < 411.0f && GetZ() > 360.0f))
		return true;

	return false;
}


bool CUser::isInMeleeArena()
{
	if (!isInMoradon())
		return false;

	if ((GetX() < 735.0f && GetX() > 684.0f) && (GetZ() < 491.0f && GetZ() > 440.0f))
		return true;

	return false;
}

bool CUser::isInPartyArena()
{
	if (!isInMoradon())
		return false;

	if ((GetX() < 735.0f && GetX() > 684.0f) && (GetZ() < 411.0f && GetZ() > 360.0f))
		return true;

	return false;
}

/**
* @brief	Searches for enemy in the surrounding region.
*/
#pragma region bool CNpc::FindEnemy()
bool CNpc::FindEnemy()
{
	if (isNonAttackingObject())
		return false;

	bool bIsGuard = isGuard();

	// We shouldn't really need this anymore...
	bool bIsNeutralZone = (isInMoradon() || GetZoneID() == ZONE_ARENA);

	if (!isGuardSummon())
	{
		// Guards and monsters are, however, allowed.
		if (!isMonster()
			&& !bIsGuard
			&& bIsNeutralZone)
			return false;
	}

	// Healer Npc
	int iMonsterNid = 0;
	if (isHealer())
	{
		iMonsterNid = FindFriend(MonSearchNeedsHealing);
		if (iMonsterNid != 0)
			return true;
	}

	KOMap* pMap = GetMap();
	if (pMap == nullptr)
		return false;

	CUser *pUser = nullptr;
	CNpc *pNpc = nullptr;

	int target_uid = 0;
	__Vector3 vUser, vNpc;
	float fDis = 0.0f;
	float fCompareDis = 0.0f;
	vNpc.Set(GetX(), GetY(), GetZ());

	float fSearchRange = (float)m_bySearchRange;

	int iExpand = FindEnemyRegion();

	if (GetRegionX() > pMap->GetXRegionMax() || GetRegionZ() > pMap->GetZRegionMax())
		return false;

	/*** If we're a monster, we can find user enemies anywhere. If we're an NPC, we must not be friendly. ***/
	if (isMonster() || (!GetMap()->areNPCsFriendly() || GetNation() != Nation::ALL))
	{
		fCompareDis = FindEnemyExpand(GetRegionX(), GetRegionZ(), fCompareDis, UnitPlayer);

		int x = 0, y = 0;

		for (int l = 0; l < 4; l++)
		{
			if (m_iFind_X[l] == 0 && m_iFind_Y[l] == 0)
				continue;

			x = GetRegionX() + (m_iFind_X[l]);
			y = GetRegionZ() + (m_iFind_Y[l]);

			if (x < 0 || y < 0 || x > pMap->GetXRegionMax() || y > pMap->GetZRegionMax())
				continue;

			fCompareDis = FindEnemyExpand(x, y, fCompareDis, UnitPlayer);
		}

		if (hasTarget() && (fCompareDis <= fSearchRange))
			return true;

		fCompareDis = 0.0f;
	}

	/*** Only find NPC/mob enemies if we are a guard ***/
	if (bIsGuard || isMonster() || isGuardSummon())
	{
		fCompareDis = FindEnemyExpand(GetRegionX(), GetRegionZ(), fCompareDis, UnitNPC);

		int x = 0, y = 0;

		for (int l = 0; l < 4; l++)
		{
			if (m_iFind_X[l] == 0 && m_iFind_Y[l] == 0)
				continue;

			x = GetRegionX() + (m_iFind_X[l]);
			y = GetRegionZ() + (m_iFind_Y[l]);

			if (x < 0 || y < 0 || x > pMap->GetXRegionMax() || y > pMap->GetZRegionMax())	continue;

			fCompareDis = FindEnemyExpand(x, y, fCompareDis, UnitNPC);
		}

		if (hasTarget() && (fCompareDis <= fSearchRange))
			return true;
	}

	InitUserList();
	InitTarget();
	return false;
}
#pragma endregion

/**
* @brief	Gets the current field value.
*
*/
#pragma region int CNpc::GetMyField()
int CNpc::GetMyField()
{
	int iRet = 0;
	int iX = GetRegionX() * VIEW_DIST;
	int iZ = GetRegionZ() * VIEW_DIST;
	int iAdd = VIEW_DIST / 2;
	int iCurX = (int)GetX();	// monster current position_x
	int iCurZ = (int)GetZ();
	if (COMPARE(iCurX, iX, iX + iAdd) && COMPARE(iCurZ, iZ, iZ + iAdd))
		iRet = 1;
	else if (COMPARE(iCurX, iX + iAdd, iX + VIEW_DIST) && COMPARE(iCurZ, iZ, iZ + iAdd))
		iRet = 2;
	else if (COMPARE(iCurX, iX, iX + iAdd) && COMPARE(iCurZ, iZ + iAdd, iZ + VIEW_DIST))
		iRet = 3;
	else if (COMPARE(iCurX, iX + iAdd, iX + VIEW_DIST) && COMPARE(iCurZ, iZ + iAdd, iZ + VIEW_DIST))
		iRet = 4;

	return iRet;
}
#pragma endregion


/**
* @brief Calculates the direction.
*
* @param	x1
* @param	z1
* @param	x2
* @param	z2
*
*   |Coordinate Plane|
*			   x1
*	--------------------> +x
*	|          |						 DIR_UP
*	|          |				DIR_UPLEFT ʌ DIR_UPRIGHT
*	|		   |						  \|/
* y1|-------- .A (x1, y1)	   DIR_LEFT <-----> DIR_RIGHT
*	|                                     /|\
*	|						  DIR_DOWNLEFT v DIR_DOWNRIGHT
*	v									DIR_DOWN
*	+y
*/
#pragma region int CNpc::GetDir(float x1, float z1, float x2, float z2)
int CNpc::GetDir(float x1, float z1, float x2, float z2)
{
	int nDir;
	//	3 4 5
	//	2 8 6
	//	1 0 7

	int nDirCount = 0;

	int x11 = (int)x1 / TILE_SIZE;
	int y11 = (int)z1 / TILE_SIZE;
	int x22 = (int)x2 / TILE_SIZE;
	int y22 = (int)z2 / TILE_SIZE;

	int deltax = x22 - x11;
	int deltay = y22 - y11;

	int fx = ((int)x1 / TILE_SIZE) * TILE_SIZE;
	int fy = ((int)z1 / TILE_SIZE) * TILE_SIZE;

	float add_x = x1 - fx;
	float add_y = z1 - fy;

	if (deltay == 0) // if we moving directly through the X-axis
	{
		if (x22 > x11)
			nDir = DIR_RIGHT;
		else
			nDir = DIR_LEFT;

		goto result_value;
	}

	if (deltax == 0) // if we moving directly through the Y-axis
	{
		if (y22 > y11)
			nDir = DIR_DOWN;
		else
			nDir = DIR_UP;

		goto result_value;
	}
	else // nah we taking it veryy orthogonal!
	{
		if (y22 > y11)
		{
			if (x22 > x11)
				nDir = DIR_DOWNRIGHT;
			else
				nDir = DIR_DOWNLEFT;
		}
		else
		{
			if (x22 > x11)
				nDir = DIR_UPRIGHT;
			else
				nDir = DIR_UPLEFT;
		}
	}

result_value:

	switch (nDir)
	{
	case DIR_DOWN:
		m_fAdd_x = add_x;
		m_fAdd_z = 3;
		break;
	case DIR_DOWNLEFT:
		m_fAdd_x = 1;
		m_fAdd_z = 3;
		break;
	case DIR_LEFT:
		m_fAdd_x = 1;
		m_fAdd_z = add_y;
		break;
	case DIR_UPLEFT:
		m_fAdd_x = 1;
		m_fAdd_z = 1;
		break;
	case DIR_UP:
		m_fAdd_x = add_x;
		m_fAdd_z = 1;
		break;
	case DIR_UPRIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = 1;
		break;
	case DIR_RIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = add_y;
		break;
	case DIR_DOWNRIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = 3;
		break;
	}

	return nDir;
}
#pragma endregion


/**
* @brief	Updates user with the NPC/MOB's movement's ending status.
*
*/
#pragma region void CNpc::NpcMoveEnd()
void CNpc::NpcMoveEnd()
{
	//RegisterRegion();
	isReturningSpawnSlot = true;
	RegisterRegionNpc(GetX(), GetZ());
	SendMoveResult(GetX(), GetY(), GetZ());
}
#pragma endregion

/**
* @brief	Finds a proper path to trace for the NPC/MOB
*
* @return	Returns 1 on success.
*/
#pragma region int CNpc::PathFind(CPoint start, CPoint end, float fDistance)
int CNpc::PathFind(CPoint start, CPoint end, float fDistance)
{
	{
		Guard lock(m_buffLock);

		if (!m_buffMap.empty())
		{
			auto itr = m_buffMap.find(BUFF_TYPE_SPEED2);
			if (itr != m_buffMap.end())
			{
				fDistance /= 3;
			}
		}
	}

	ClearPathFindData();

	if (start.x < 0 || start.y < 0
		|| end.x < 0 || end.y < 0)
		return -1;

	if (start.x == end.x && start.y == end.y)
	{
		m_bPathFlag = true;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		return 1;
	}

	if (IsPathFindCheck(fDistance))
	{
		m_bPathFlag = true;
		return 1;
	}

	m_vMapSize.cx = m_max_x - m_min_x + 1;
	m_vMapSize.cy = m_max_y - m_min_y + 1;

	m_pPath = nullptr;

	m_vPathFind.SetMap(m_vMapSize.cx, m_vMapSize.cy, GetMap(), m_min_x, m_min_y);
	m_pPath = m_vPathFind.FindPath(end.x, end.y, start.x, start.y);

	int count = 0;
	while (m_pPath != nullptr)
	{
		m_pPath = m_pPath->Parent;
		if (m_pPath == nullptr)
			break;

		m_pPoint[count].pPoint.x = m_pPath->x + m_min_x;
		m_pPoint[count++].pPoint.y = m_pPath->y + m_min_y;
	}

	if (count <= 0 || count >= MAX_PATH_LINE)
		return 0;

	m_iAniFrameIndex = count - 1;
	int nAdd = GetDir(m_fStartPoint_X, m_fStartPoint_Y, m_fEndPoint_X, m_fEndPoint_Y);

	for (int i = 0; i < count; i++)
	{
		if (i == (count - 1))
		{
			m_pPoint[i].fXPos = m_fEndPoint_X;
			m_pPoint[i].fZPos = m_fEndPoint_Y;
		}
		else
		{
			m_pPoint[i].fXPos = (float)(m_pPoint[i].pPoint.x * TILE_SIZE + m_fAdd_x);
			m_pPoint[i].fZPos = (float)(m_pPoint[i].pPoint.y * TILE_SIZE + m_fAdd_z);
		}
	}

	return 1;
}
#pragma endregion


void CNpc::GetVectorPosition(__Vector3 & vOrig, __Vector3 & vDest, float fDis, __Vector3 * vResult)
{
	*vResult = vDest - vOrig;
	vResult->Normalize();
	*vResult *= fDis;
	*vResult += vOrig;
}

void CNpc::CalcAdaptivePosition(__Vector3 & vPosOrig, __Vector3 & vPosDest, float fAttackDistance, __Vector3 * vResult)
{
	*vResult = vPosOrig - vPosDest;
	vResult->Normalize();
	*vResult *= fAttackDistance;
	*vResult += vPosDest;
}

void CNpc::ComputeDestPos(__Vector3 & vCur, float fDegree, float fDistance, __Vector3 * vResult)
{
	__Matrix44 mtxRot;
	vResult->Set(0.0f, 0.0f, 1.0f);
	mtxRot.RotationY(D3DXToRadian(fDegree));
	*vResult *= mtxRot;
	*vResult *= fDistance;
	*vResult += vCur;
}

void CNpc::Yaw2D(float fDirX, float fDirZ, float& fYawResult)
{
	if (fDirX >= 0.0f)
	{
		if (fDirZ >= 0.0f)
			fYawResult = (float)(asin(fDirX));
		else
			fYawResult = D3DXToRadian(90.0f) + (float)(acos(fDirX));
	}
	else
	{
		if (fDirZ >= 0.0f)
			fYawResult = D3DXToRadian(270.0f) + (float)(acos(-fDirX));
		else
			fYawResult = D3DXToRadian(180.0f) + (float)(asin(-fDirX));
	}
}

float CNpc::GetDistance(__Vector3 & vOrig, __Vector3 & vDest)
{
	return (vOrig - vDest).Magnitude();
}


/**
* @brief	Checks whether there exists any user in the view range.
*
* @return	Returns true on success.
*/
bool CNpc::GetUserInView()
{
	KOMap* pMap = GetMap();
	if (pMap == nullptr)	return false;

	int min_x = (int)(GetX() - NPC_VIEW_RANGE) / VIEW_DIST;	if (min_x < 0) min_x = 0;
	int min_z = (int)(GetZ() - NPC_VIEW_RANGE) / VIEW_DIST;	if (min_z < 0) min_z = 0;
	int max_x = (int)(GetX() + NPC_VIEW_RANGE) / VIEW_DIST;	if (max_x > pMap->GetXRegionMax()) max_x = pMap->GetXRegionMax();
	int max_z = (int)(GetZ() + NPC_VIEW_RANGE) / VIEW_DIST;	if (max_z > pMap->GetZRegionMax()) max_z = pMap->GetZRegionMax();

	int search_x = max_x - min_x + 1;
	int search_z = max_z - min_z + 1;

	bool bFlag = false;
	int i, j;

	for (i = 0; i < search_x; i++)
	{
		for (j = 0; j < search_z; j++)
		{
			bFlag = GetUserInViewRange(min_x + i, min_z + j);
			if (bFlag == true)
				return true;
		}
	}

	return false;
}

/**
* @brief	Searches for user in view range.
*/
bool CNpc::GetUserInViewRange(int x, int z)
{
	KOMap* pMap = GetMap();
	if (pMap == nullptr || x < 0 || z < 0 || x > pMap->GetXRegionMax() || z > pMap->GetZRegionMax())
	{
		TRACE("#### Npc-GetUserInViewRange() Fail : [nid=%d, sid=%d], x1=%d, z1=%d #####\n", GetID(), GetProtoID(), x, z);
		return false;
	}

	CRegion * pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return false;

	return pRegion->m_byMoving == 1;// m_RegionUserArray.size() > 0;
}

void CNpc::MoveAttack()
{
	if (!hasTarget())
		return;

	__Vector3 vUser, vNpc;
	__Vector3 vDistance, vEnd22;

	float fDis = 0.0f;
	float fX = 0.0f, fZ = 0.0f;

	vNpc.Set(GetX(), GetY(), GetZ());

	if (m_Target.id < NPC_BAND)	// Target is a player
	{
		__Vector3 vUser;
		CUser * pUser = g_pMain->GetUserPtr(m_Target.id);
		if (pUser == nullptr || (m_Target.m_tLastDamageTime > 0 && (UNIXTIME2 - m_Target.m_tLastDamageTime> 6000)))
		{
			InitTarget();
			return;
		}
		vUser.Set(pUser->GetX(), pUser->GetY(), pUser->GetZ());

		CalcAdaptivePosition(vNpc, vUser, 2, &vEnd22);

		if (m_byAttackPos > 0 && m_byAttackPos < 9)
		{
			fX = vUser.x + surround_fx[m_byAttackPos - 1];	fZ = vUser.z + surround_fz[m_byAttackPos - 1];
			vEnd22.Set(fX, 0, fZ);
		}
		else
		{
			fX = vEnd22.x;	fZ = vEnd22.z;
		}
	}
	else // Target is an NPC/monster
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(m_Target.id, GetZoneID());
		if (pNpc == nullptr)
		{
			InitTarget();
			return;
		}
		vUser.Set(pNpc->GetX(), pNpc->GetY(), pNpc->GetZ());

		CalcAdaptivePosition(vNpc, vUser, 2, &vEnd22);
		fX = vEnd22.x;	fZ = vEnd22.z;
	}

	vDistance = vUser - vNpc;
	fDis = vDistance.Magnitude();

	if ((int)fDis < 3) return;

	vDistance = vEnd22 - vNpc;
	fDis = vDistance.Magnitude();
	m_curx = vEnd22.x;
	m_curz = vEnd22.z;

	if (GetX() < 0 || GetZ() < 0)
	{
		TRACE("Npc-MoveAttack : nid=(%d, %s), x=%.2f, z=%.2f\n",
		GetID(), GetName().c_str(), GetX(), GetZ());
		return;
	}

	// Move to target... then stop (this is really awkward behaviour.)
	SendMoveResult(GetX(), GetY(), GetZ(), fDis);
	SendMoveResult(GetX(), GetY(), GetZ());

	RegisterRegionNpc(GetX(), GetZ());

	m_fEndPoint_X = GetX();
	m_fEndPoint_Y = GetZ();
}


int	CNpc::ItemProdution(int item_number)
{
	int iItemNumber = 0, iRandom = 0, i = 0, iItemGrade = 0, iItemLevel = 0;
	int iDefault = 0, iItemCode = 0, iItemKey = 0, iRand2 = 0, iRand3 = 0, iRand4 = 0, iRand5 = 0;
	int iTemp1 = 0, iTemp2 = 0, iTemp3 = 0;

	iRandom = myrand(1, 10000);

	iItemGrade = GetItemGrade(item_number);
	if (iItemGrade == 0)		return 0;
	iItemLevel = GetLevel() / 5;

	if (COMPARE(iRandom, 1, 4001))
	{
		iDefault = 100000000;
		iRandom = myrand(1, 10000);
		if (COMPARE(iRandom, 1, 701))			iRand2 = 10000000;
		else if (COMPARE(iRandom, 701, 1401))	iRand2 = 20000000;
		else if (COMPARE(iRandom, 1401, 2101))	iRand2 = 30000000;
		else if (COMPARE(iRandom, 2101, 2801))	iRand2 = 40000000;
		else if (COMPARE(iRandom, 2801, 3501))	iRand2 = 50000000;
		else if (COMPARE(iRandom, 3501, 5501))	iRand2 = 60000000;
		else if (COMPARE(iRandom, 5501, 6501))	iRand2 = 70000000;
		else if (COMPARE(iRandom, 6501, 8501))	iRand2 = 80000000;
		else if (COMPARE(iRandom, 8501, 10001))	iRand2 = 90000000;

		iTemp1 = GetWeaponItemCodeNumber(true);

		if (iTemp1 == 0)
			return 0;

		iItemCode = iTemp1 * 100000;

		iRand3 = myrand(1, 10000);
		if (COMPARE(iRand3, 1, 5000))
			iRand3 = 10000;
		else
			iRand3 = 50000;

		iRand4 = myrand(1, 10000);
		if (COMPARE(iRand4, 1, 5000))
			iRand4 = 0;
		else
			iRand4 = 5000000;

		iRandom = GetItemCodeNumber(iItemLevel, 1);

		if (iRandom == -1)
			return 0;

		iRand5 = iRandom * 10;
		iItemNumber = iDefault + iItemCode + iRand2 + iRand3 + iRand4 + iRand5 + iItemGrade;
	}
	else if (COMPARE(iRandom, 4001, 8001))
	{
		iDefault = 200000000;

		iTemp1 = GetWeaponItemCodeNumber(false);

		if (iTemp1 == 0)
			return 0;

		iItemCode = iTemp1 * 1000000;

		if (m_byMaxDamagedNation == KARUS)
		{
			iRandom = myrand(0, 10000);
			if (COMPARE(iRandom, 0, 2000))
			{
				iRand2 = 0;
				iRand3 = 10000;
			}
			else if (COMPARE(iRandom, 2000, 4000))
			{
				iRand2 = 40000000;
				iRand3 = 20000;
			}
			else if (COMPARE(iRandom, 4000, 6000))
			{
				iRand2 = 60000000;
				iRand3 = 30000;
			}
			else if (COMPARE(iRandom, 6000, 10001))
			{
				iRand2 = 80000000;
				iRandom = myrand(0, 10000);
				if (COMPARE(iRandom, 0, 5000))
					iRand3 = 20000;
				else
					iRand3 = 40000;
			}
		}
		else if (m_byMaxDamagedNation == ELMORAD)
		{
			iRandom = myrand(0, 10000);
			if (COMPARE(iRandom, 0, 3300))
			{
				iRand2 = 0;
				iItemKey = myrand(0, 10000);
				if (COMPARE(iItemKey, 0, 3333))
					iRand3 = 110000;
				else if (COMPARE(iItemKey, 3333, 6666))
					iRand3 = 120000;
				else if (COMPARE(iItemKey, 6666, 10001))
					iRand3 = 130000;
			}
			else if (COMPARE(iRandom, 3300, 5600))
			{
				iRand2 = 40000000;
				iItemKey = myrand(0, 10000);
				if (COMPARE(iItemKey, 0, 5000))
					iRand3 = 120000;
				else
					iRand3 = 130000;
			}
			else if (COMPARE(iRandom, 5600, 7800))
			{
				iRand2 = 60000000;
				iItemKey = myrand(0, 10000);
				if (COMPARE(iItemKey, 0, 5000))
					iRand3 = 120000;
				else
					iRand3 = 130000;
			}
			else if (COMPARE(iRandom, 7800, 10001))
			{
				iRand2 = 80000000;
				iItemKey = myrand(0, 10000);
				if (COMPARE(iItemKey, 0, 5000))	iRand3 = 120000;
				else								iRand3 = 130000;
			}

		}

		iTemp2 = myrand(0, 10000);

		if (COMPARE(iTemp2, 0, 2000))			iRand4 = 1000;
		else if (COMPARE(iTemp2, 2000, 4000))	iRand4 = 2000;
		else if (COMPARE(iTemp2, 4000, 6000))	iRand4 = 3000;
		else if (COMPARE(iTemp2, 6000, 8000))	iRand4 = 4000;
		else if (COMPARE(iTemp2, 8000, 10001))	iRand4 = 5000;

		iRandom = GetItemCodeNumber(iItemLevel, 2);
		if (iRandom == -1)
			return 0;
		iRand5 = iRandom * 10;
		iItemNumber = iDefault + iRand2 + iItemCode + iRand3 + iRand4 + iRand5 + iItemGrade;	// iItemGrade : 
	}
	else if (COMPARE(iRandom, 8001, 10001))
	{
		iDefault = 300000000;
		iRandom = myrand(0, 10000);

		if (COMPARE(iRandom, 0, 2500))			iRand2 = 10000000;
		else if (COMPARE(iRandom, 2500, 5000))	iRand2 = 20000000;
		else if (COMPARE(iRandom, 5000, 7500))	iRand2 = 30000000;
		else if (COMPARE(iRandom, 7500, 10001))	iRand2 = 40000000;

		iRand3 = myrand(1, 10000);
		if (COMPARE(iRand3, 1, 5000))
			iRand3 = 110000;
		else
			iRand3 = 150000;
		iRandom = GetItemCodeNumber(iItemLevel, 3);

		if (iRandom == -1)
			return 0;

		iRand4 = iRandom * 10;
		iItemNumber = iDefault + iRand2 + iRand3 + iRand4 + iItemGrade;
	}
	return iItemNumber;
}

int CNpc::GetItemGrade(int item_grade)
{
	int iPercent = 0, iRandom = 0, i = 0;
	_MAKE_ITEM_GRADE_CODE* pItemData = nullptr;

	iRandom = myrand(1, 1000);
	pItemData = g_pMain->m_MakeGradeItemArray.GetData(item_grade);
	if (pItemData == nullptr)	return 0;


	for (i = 0; i<9; i++)
	{
		if (i == 0)
		{
			if (pItemData->sGrade[i] == 0)
			{
				iPercent += pItemData->sGrade[i];
				continue;
			}
			if (COMPARE(iRandom, 0, pItemData->sGrade[i]))
				return i + 1;
			else
			{
				iPercent += pItemData->sGrade[i];
				continue;
			}
		}
		else
		{
			if (pItemData->sGrade[i] == 0)
			{
				iPercent += pItemData->sGrade[i];
				continue;
			}

			if (COMPARE(iRandom, iPercent, iPercent + pItemData->sGrade[i]))
				return i + 1;
			else
			{
				iPercent += pItemData->sGrade[i];
				continue;
			}
		}

	}

	return 0;
}

int CNpc::GetWeaponItemCodeNumber(bool bWeapon)
{
	_MAKE_WEAPON * pItemData = nullptr;

	if (bWeapon)
		pItemData = g_pMain->m_MakeWeaponItemArray.GetData(GetLevel() / 10);
	else
		pItemData = g_pMain->m_MakeDefensiveItemArray.GetData(GetLevel() / 10);

	if (pItemData == nullptr)
		return 0;

	for (int i = 0, iPercent = 0, iRandom = myrand(0, 1000); i < MAX_UPGRADE_WEAPON; i++)
	{
		if (pItemData->sClass[i] == 0)
		{
			iPercent += pItemData->sClass[i];
			continue;
		}

		if (COMPARE(iRandom, iPercent, iPercent + pItemData->sClass[i]))
			return i + 1;

		iPercent += pItemData->sClass[i];
	}

	return 0;
}

int CNpc::GetItemCodeNumber(int level, int item_type)
{
	int iItemCode = 0, iItemType = 0, iPercent = 0;

	_MAKE_ITEM_LARE_CODE * pItemData = g_pMain->m_MakeLareItemArray.GetData(level);
	if (pItemData == nullptr)
		return -1;

	int iItemPercent[] = { pItemData->sLareItem, pItemData->sMagicItem, pItemData->sGeneralItem };
	int iRandom = myrand(0, 1000);
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			if (COMPARE(iRandom, 0, iItemPercent[i]))
			{
				iItemType = i + 1;
				break;
			}
			else
			{
				iPercent += iItemPercent[i];
				continue;
			}
		}
		else
		{
			if (COMPARE(iRandom, iPercent, iPercent + iItemPercent[i]))
			{
				iItemType = i + 1;
				break;
			}
			else
			{
				iPercent += iItemPercent[i];
				continue;
			}
		}
	}

	switch (iItemType)
	{
	case 1:						// lare item
		if (item_type == 1)
			iItemCode = myrand(16, 24);
		else if (item_type == 2)
			iItemCode = myrand(12, 24);
		else if (item_type == 3)
			iItemCode = myrand(0, 10);
		break;

	case 2:						// magic item
		if (item_type == 1)
			iItemCode = myrand(6, 15);
		else if (item_type == 2)
			iItemCode = myrand(6, 11);
		else if (item_type == 3)
			iItemCode = myrand(0, 10);
		break;

	case 3:						// general item
		if (item_type == 1
			|| item_type == 2)
			iItemCode = 5;
		else if (item_type == 3)
			iItemCode = myrand(0, 10);
		break;
	}

	return iItemCode;
}

int CNpc::GetPartyExp(int party_level, int man, int nNpcExp)
{
	int nPartyExp = 0;
	int nLevel = party_level / man;
	double TempValue = 0;
	nLevel = GetLevel() - nLevel;

	if (nLevel < 2)
		nPartyExp = nNpcExp; // x1

	else if (nLevel >= 2 && nLevel < 5)
	{
		TempValue = nNpcExp * 1.2; // x1.2
		nPartyExp = (int)TempValue;
		if (TempValue > nPartyExp)
			nPartyExp++;
	}
	else if (nLevel >= 5 && nLevel < 8)
	{
		TempValue = nNpcExp * 1.5; // x1.5
		nPartyExp = (int)TempValue;
		if (TempValue > nPartyExp)
			nPartyExp++;
	}
	else if (nLevel >= 8)
	{
		nPartyExp = nNpcExp * 2; // x2
	}

	return nPartyExp;
}

void CNpc::ChangeAbility(int iChangeType)
{
	if (iChangeType > 2)
		return;

	int nHP = 0, nAC = 0, nDamage = 0, nMagicR = 0, nDiseaseR = 0, nPoisonR = 0, nLightningR = 0, nFireR = 0, nColdR = 0;
	CNpcTable*	pNpcTable = GetProto();

	if (iChangeType == BATTLEZONE_OPEN)
	{
		nHP = (int)(pNpcTable->m_MaxHP * 1.2);
		nAC = (int)(pNpcTable->m_sDefense * 1.2);
		nDamage = (int)(pNpcTable->m_sDamage * 0.5);
		nMagicR = (int)(pNpcTable->m_byMagicR * 2);
		nDiseaseR = (int)(pNpcTable->m_byDiseaseR * 2);
		nPoisonR = (int)(pNpcTable->m_byPoisonR * 2);
		nLightningR = (int)(pNpcTable->m_byLightningR * 2);
		nFireR = (int)(pNpcTable->m_byFireR * 2);
		nColdR = (int)(pNpcTable->m_byColdR * 2);
		m_MaxHP = nHP;

		if (GetMaxHealth() > GetHealth())
		{
			m_iHP = GetMaxHealth() - 50;
			HpChange();
		}

		m_sTotalAc = nAC;
		m_sTotalHit = nDamage;
		m_sFireR = nFireR;
		m_sColdR = nColdR;
		m_sLightningR = nLightningR;
		m_sMagicR = nMagicR;
		m_sDiseaseR = nDiseaseR;
		m_sPoisonR = nPoisonR;
	}
	else if (iChangeType == BATTLEZONE_CLOSE)
	{
		m_MaxHP = pNpcTable->m_MaxHP;
		if (GetMaxHealth() > GetHealth())
		{
			m_iHP = GetMaxHealth() - 50;
			HpChange();
		}

		m_sPid = GetProto()->m_sPid;
		GetProto()->m_byGroupSpecial = 0;
		bool bMonster = (m_byInitMoveType < 100);
		m_bNation = bMonster == 0 ? m_proto->m_byGroup : bMonster;

		m_sTotalHit = pNpcTable->m_sDamage;
		m_sTotalAc = pNpcTable->m_sDefense;
		m_sFireR = pNpcTable->m_byFireR;
		m_sColdR = pNpcTable->m_byColdR;
		m_sLightningR = pNpcTable->m_byLightningR;
		m_sMagicR = pNpcTable->m_byMagicR;
		m_sDiseaseR = pNpcTable->m_byDiseaseR;
		m_sPoisonR = pNpcTable->m_byPoisonR;
	}
}

void CNpc::HpChange()
{
	m_fHPChangeTime = getMSTime();

	if (isDead())
		return;

	if (!isMonster()
		&& GetProtoID() == 511)
		return;

	if (GetHealth() != GetMaxHealth())
		HpChange((int)ceil((double(m_MaxHP * 10) / 100)));

	if (isPet())
	{
		if (GetMana() != GetMaxMana())
			MSpChange((int)ceil((double(m_MaxMP * 10) / 100)));
	}
}

void CNpc::HPTimeChangeType3()
{
	if (isDead()
		|| !m_bType3Flag)
		return;

	uint16	totalActiveDurationalSkills = 0,
		totalActiveDOTSkills = 0;
	bool bIsDOT = false;
	for (int i = 0; i < MAX_TYPE3_REPEAT; i++)
	{
		MagicType3 * pEffect = &m_durationalSkills[i];
		if (!pEffect->m_byUsed)
			continue;

		// Has the required interval elapsed before using this skill?
		if ((UNIXTIME - pEffect->m_tHPLastTime) >= pEffect->m_bHPInterval)
		{
			Unit * pUnit = g_pMain->GetUnitPtr(pEffect->m_sSourceID, GetZoneID());

			// Reduce the HP 
			HpChange(pEffect->m_sHPAmount, pUnit); // do we need to specify the source of the DOT?
			pEffect->m_tHPLastTime = UNIXTIME;

			if (pEffect->m_sHPAmount < 0)
				bIsDOT = true;

			// Has the skill expired yet?
			if (++pEffect->m_bTickCount == pEffect->m_bTickLimit)
			{
				pEffect->Reset();
			}
		}

		if (pEffect->m_byUsed)
		{
			totalActiveDurationalSkills++;
			if (pEffect->m_sHPAmount < 0)
				totalActiveDOTSkills++;
		}
	}

	// Have all the skills expired?
	if (totalActiveDurationalSkills == 0)
		m_bType3Flag = false;
}


void CNpc::Type4Duration()
{

	Guard lock(m_buffLock);
	if (m_buffMap.empty())
	{
		return;
	}

	foreach(itr, m_buffMap)
	{
		if (itr->second.m_tEndTime > UNIXTIME)
			continue;

		CMagicProcess::RemoveType4Buff(itr->first, this);
		break; // only ever handle one at a time with the current logic
	}
}


void CNpc::SendMoveResult(float fX, float fY, float fZ, float fSpeed /*= 0.0f*/)
{
	if (isDead())
		return;

	SetPosition(fX, fY, fZ);
	RegisterRegion();

	Packet result(WIZ_NPC_MOVE);
	result << uint8(1) << GetID() << GetSPosX() << GetSPosZ() << GetSPosY() << uint16(fSpeed * 10);
	SendToRegion(&result);
}


void CNpc::SendExpToUserList()
{
	if (GetMap() == nullptr)
		return;

	uint32 nMaxDamage = 0;

	Guard lock(m_damageListLock);
	std::map<CNpc *, uint32> filteredDamagePetList;
	std::map<CUser *, uint32> filteredDamageList;
	std::map<uint16, CUser *> partyIndex;

	// Filter the damage list first, so we only send one packet per party.
	// Rewards are shared based upon the total amount of damage dealt.

	// NOTE:	If a player logs out & another takes its place, it is currently possible
	//			For this new session to be counted as the old when the mob is killed.
	//			This does, however, requires the player to be in the same zone, in range at the time
	//			of the mob's death -- so it is rather unlikely. We should fix this later.
	foreach(itr, m_DamagedUserList)
	{
		Unit * pUnit = g_pMain->GetUnitPtr(itr->first, GetZoneID());

		if (pUnit == nullptr)
			continue;

		if (pUnit->isNPC())
		{
			CNpc *pNpc = g_pMain->GetNpcPtr(itr->first, GetZoneID());
			if (pNpc == nullptr
				|| !pNpc->isPet()
				|| !isInRangeSlow(pNpc, NPC_EXP_RANGE))
				continue;

			auto filteredDamageListItr = filteredDamagePetList.find(pNpc);

			if (filteredDamageListItr == filteredDamagePetList.end()) 
				filteredDamagePetList.insert(std::make_pair(pNpc, itr->second));
			else 
				filteredDamagePetList[pNpc] = itr->second;
		}
		else if (pUnit->isPlayer())
		{
			CUser * pUser = g_pMain->GetUserPtr(itr->first);
			if (pUser == nullptr
				|| !pUser->isInGame()
				|| !isInRangeSlow(pUser, NPC_EXP_RANGE))
				continue;

			// Not in a party, we can add them to the list as a solo attacker.
			if (!pUser->isInParty())
			{
				filteredDamageList[pUser] = itr->second;
				continue;
			}

			// In a party, so check if another party member is already in the list first.
			auto partyItr = partyIndex.find(pUser->GetPartyID());

			// No other party member, so add us to the filtered damage list & the party index
			// for future reference.
			if (partyItr == partyIndex.end())
			{
				filteredDamageList[pUser] = itr->second;
				partyIndex.insert(std::make_pair(pUser->GetPartyID(), pUser));
			}
			// There is another pf pir party members in the damage list already, so just add 
			// to their damage total.
			else
			{
				filteredDamageList[partyItr->second] += itr->second;
			}
		}
	}

	// Now we can go through the filtered list and tell the game server to distribute rewards
	// for the kill.
	foreach(itr, filteredDamageList)
	{
		CUser * pUser = itr->first;
		if (pUser == nullptr || !pUser->isInGame())
			continue;

		pUser->RecvUserExp(this, itr->second, m_TotalDamage);

		if (itr->second > nMaxDamage)
		{
			m_sMaxDamageUserid = pUser->GetID();
			strMaxDamageUser = pUser->GetName();
			nMaxDamage = itr->second;
		}
	}

	// Now we can go through the filtered list and tell the game server to distribute rewards
	// for the kill.
	foreach(itr, filteredDamagePetList)
	{
		CNpc * pNpc = itr->first;
		if (pNpc == nullptr 
			|| !pNpc->isPet())
			continue;

		pNpc->RecvPetExp(this, itr->second, m_TotalDamage, m_proto->m_iExp, m_proto->m_iLoyalty);

		if (itr->second > nMaxDamage)
		{
			m_sMaxDamageUserid = pNpc->GetPetID();
			strMaxDamageUser = pNpc->GetNameUser();
			nMaxDamage = itr->second;
		}
	}

	if (g_pMain->isWarOpen()
		/*&& !strMaxDamageUser.empty()*/
		&& m_bySpecialType >= NpcSpecialTypeKarusWarder1 && m_bySpecialType <= NpcSpecialTypeElmoradKeeper)
	{
		int nResourceID = 0;
		string chatstr, strKnightsName;
		CKnights* pKnights = nullptr;

		CUser *pUser = g_pMain->GetUserPtr(strMaxDamageUser, TYPE_CHARACTER);
		if (pUser != nullptr && pUser->isInGame())
		{
			pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
			if (pKnights)
				strKnightsName = pKnights->GetName();
		}

		switch (m_bySpecialType)
		{
		case NpcSpecialTypeKarusWarder1:	 // Karus warder 1
			g_pMain->m_sKilledKarusNpc++;
			nResourceID = IDS_KILL_KARUS_GUARD1;
			break;
		case NpcSpecialTypeKarusWarder2:	 // Karus warder 2
			g_pMain->m_sKilledKarusNpc++;
			nResourceID = IDS_KILL_KARUS_GUARD2;
			break;
		case NpcSpecialTypeElmoradWarder1: // El Morad warder 1
			g_pMain->m_sKilledElmoNpc++;
			nResourceID = IDS_KILL_ELMO_GUARD1;
			break;
		case NpcSpecialTypeElmoradWarder2:	// El Morad warder 2
			g_pMain->m_sKilledElmoNpc++;
			nResourceID = IDS_KILL_ELMO_GUARD2;
			break;
		case NpcSpecialTypeKarusKeeper:// Karus Keeper
			g_pMain->m_sKilledKarusNpc++;
			nResourceID = IDS_KILL_GATEKEEPER;
			break;
		case NpcSpecialTypeElmoradKeeper:// El Morad Keeper
			g_pMain->m_sKilledElmoNpc++;
			nResourceID = IDS_KILL_GATEKEEPER;
			break;
		}

		if (g_pMain->m_bResultDelay == false
			&& (g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE
				|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE2
				|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE3
				|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE6))
		{
			if (g_pMain->m_sKilledKarusNpc == 3)
			{
				g_pMain->m_bResultDelay = true;
				g_pMain->m_bResultDelayVictory = ELMORAD;
			}
			else if (g_pMain->m_sKilledElmoNpc == 3)
			{
				g_pMain->m_bResultDelay = true;
				g_pMain->m_bResultDelayVictory = KARUS;
			}
		}

		if (nResourceID == 0)
		{
			TRACE("RecvBattleEvent: could not establish resource for result %d", m_bySpecialType);
			return;
		}

		g_pMain->GetServerResource(nResourceID, &chatstr, strKnightsName.c_str(), strMaxDamageUser.c_str());

		Packet result;
		string finalstr;

		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &finalstr);
		g_pMain->Send_All(&result);

		ChatPacket::Construct(&result, PUBLIC_CHAT, &finalstr);
		g_pMain->Send_All(&result);
	}

}

bool CNpc::isShowBox()
{
	uint8 bType = GetType();

	if (bType == NPC_CHAOS_STONE
		|| bType == NPC_PVP_MONUMENT
		|| bType == NPC_BIFROST_MONUMENT
		|| bType == NPC_GUARD_TOWER1
		|| bType == NPC_GUARD_TOWER2
		|| bType == NPC_SCARECROW
		|| bType == NPC_KARUS_WARDER1
		|| bType == NPC_KARUS_WARDER2
		|| bType == NPC_ELMORAD_WARDER1
		|| bType == NPC_ELMORAD_WARDER2
		|| bType == NPC_KARUS_GATEKEEPER
		|| bType == NPC_ELMORAD_GATEKEEPER
		|| bType == NPC_BATTLE_MONUMENT
		|| bType == NPC_KARUS_MONUMENT
		|| bType == NPC_HUMAN_MONUMENT
		|| bType == NPC_CLAN_WAR_MONUMENT
		|| GetZoneID() == ZONE_FORGOTTEN_TEMPLE
		|| GetZoneID() == ZONE_PRISON
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
		return false;

	if (GetZoneID() == ZONE_JURAID_MOUNTAIN
		&& (GetProtoID() == MONSTER_APOSTLE_SSID
		|| GetProtoID() == MONSTER_DOOM_SOLDIER_SSID
		|| GetProtoID() == MONSTER_TROLL_CAPTAIN_SSID))
		return false;

	return true;
}

/**
* @brief	Handles the process of determining the items to be dropped
*			in the bundle.
*
*/
#if 1
void CNpc::GiveNpcHaveItem(Unit *pKiller)
{
	KOMap *pMap = g_pMain->GetZoneByID(GetZoneID());
	if (pMap == nullptr)
		return;

	if (m_sMaxDamageUserid < 0 || m_sMaxDamageUserid > MAX_USER)
		return;

	int iPer = 0, iMakeItemCode = 0, iMoney = 0, iRandom, nCount = 0;

	iRandom = myrand(70, 100);
	iMoney = m_iMoney * iRandom / 100;

	_NpcGiveItem m_GiveItemList[NPC_HAVE_ITEM_LIST];
	CUser *pKillerUser = nullptr;

	if (pKiller == nullptr)
		iMoney = m_iMoney * iRandom / 100;
	else if (pKiller != nullptr)
	{
		pKillerUser = g_pMain->GetUserPtr(pKiller->GetName(), TYPE_CHARACTER);

		if (pKillerUser == nullptr)
			iMoney = m_iMoney * iRandom / 100;
		else if (pKillerUser != nullptr)
		{
			if (pKiller->GetLevel() >= (GetLevel() + 14))
				iMoney = 0;
			else
				iMoney = m_iMoney * iRandom / 100;
		}			
	}
	
	if (iMoney > 0)
	{
		if (iMoney >= SHRT_MAX)
			iMoney = 32000;

		m_GiveItemList[nCount].sSid = ITEM_GOLD;
		m_GiveItemList[nCount++].count = iMoney;
	}

	if (isMonster())
	{
		_K_MONSTER_ITEM* pItem = g_pMain->m_MonsterItemArray.GetData(m_iItem);
		if (pItem != nullptr)
		{
			for (int j = 0; j < LOOT_DROP_ITEMS; j++)
			{
				if (pItem->iItem[j] == 0 
					|| pItem->sPercent[j] == 0)
					continue;

				iRandom = myrand(1, 10000);
				iPer = pItem->sPercent[j];

				if (pKillerUser != nullptr)
				{
					if (pKillerUser->GetPremiumProperty(PremiumDropPercent) > 0)
						iPer = iPer * (100 + pKillerUser->GetPremiumProperty(PremiumDropPercent)) / 100;

					if (pKillerUser->GetClanPremiumProperty(PremiumDropPercent) > 0)
						iPer = iPer * (100 + pKillerUser->GetClanPremiumProperty(PremiumDropPercent)) / 100;

					if (pKillerUser->m_FlashDcBonus > 0)
						iPer = iPer * (100 + pKillerUser->m_FlashDcBonus) / 100;
				}

				if (iPer > 10000)
					iPer = 10000;

				if (iRandom > iPer)
					continue;

				if (pItem->iItem[j] < 100000000)
				{
					if (pItem->iItem[j] < 100)
						iMakeItemCode = ItemProdution(pItem->iItem[j]);
					else
					{
						_MAKE_ITEM_GROUP* pGroup = g_pMain->m_MakeItemGroupArray.GetData(pItem->iItem[j]);
						if (pGroup == nullptr
							|| pGroup->iItems.size() <= 0)
							continue;

						iMakeItemCode = pGroup->iItems[myrand(1, (int32)pGroup->iItems.size()) - 1];
					}
					if (iMakeItemCode == 0)
						continue;

					m_GiveItemList[nCount].sSid = iMakeItemCode;
					m_GiveItemList[nCount].count = 1;
				}
				else
				{
					m_GiveItemList[nCount].sSid = pItem->iItem[j];
					if (COMPARE(m_GiveItemList[nCount].sSid, ARROW_MIN, ARROW_MAX))
						m_GiveItemList[nCount].count = 20;
					else
						m_GiveItemList[nCount].count = 1;
				}
				nCount++;
			}
		}
	}
	else
	{
		_K_NPC_ITEM* pItem = g_pMain->m_NpcItemArray.GetData(m_iItem);
		if (pItem != nullptr)
		{
			for (int j = 0; j < LOOT_DROP_ITEMS; j++)
			{
				if (pItem->iItem[j] == 0 
					|| pItem->sPercent[j] == 0)
					continue;

				iRandom = myrand(1, 10000);
				iPer = pItem->sPercent[j];

				if (pKillerUser != nullptr)
				{
					if (pKillerUser->GetPremiumProperty(PremiumDropPercent) > 0)
						iPer = iPer * (100 + pKillerUser->GetPremiumProperty(PremiumDropPercent)) / 100;

					if (pKillerUser->GetClanPremiumProperty(PremiumDropPercent) > 0)
						iPer = iPer * (100 + pKillerUser->GetClanPremiumProperty(PremiumDropPercent)) / 100;

					if (pKillerUser->m_FlashDcBonus > 0)
						iPer = iPer * (100 + pKillerUser->m_FlashDcBonus) / 100;
				}

				if (iPer > 10000)
					iPer = 10000;

				if (iRandom > iPer)
					continue;

				if (pItem->iItem[j] < 100000000)
				{
					if (pItem->iItem[j] < 100)
						iMakeItemCode = ItemProdution(pItem->iItem[j]);
					else
					{
						_MAKE_ITEM_GROUP* pGroup = g_pMain->m_MakeItemGroupArray.GetData(pItem->iItem[j]);
						if (pGroup == nullptr
							|| pGroup->iItems.size() <= 0)
							continue;

						iMakeItemCode = pGroup->iItems[myrand(1, (int32)pGroup->iItems.size()) - 1];
					}
					if (iMakeItemCode == 0)
						continue;

					m_GiveItemList[nCount].sSid = iMakeItemCode;
					m_GiveItemList[nCount].count = 1;
				}
				else
				{
					m_GiveItemList[nCount].sSid = pItem->iItem[j];
					if (COMPARE(m_GiveItemList[nCount].sSid, ARROW_MIN, ARROW_MAX))
						m_GiveItemList[nCount].count = 20;
					else
						m_GiveItemList[nCount].count = 1;
				}
				nCount++;
			}
		}
	}

	struct items {
		uint32 itemid;
		uint32 slot;
	};

	vector<items> itemList;
	CUser* pUser = nullptr;
	_LOOT_BUNDLE* pBundle = new _LOOT_BUNDLE;

	pBundle->tDropTime = UNIXTIME;
	pBundle->x = GetX();
	pBundle->z = GetZ();
	pBundle->y = GetY();

	for (int i = 0; i < nCount; i++)
	{
		if (g_pMain->GetItemPtr(m_GiveItemList[i].sSid))
		{
			items item;
			_LOOT_ITEM pItem(m_GiveItemList[i].sSid, m_GiveItemList[i].count);
			if (m_GiveItemList[i].sSid == ITEM_GOLD)
			{
				// Add on any additional coins earned because of a global coin event.
				// NOTE: Officially it caps at SHRT_MAX, but that's really only for technical reasons.
				// Using the unsigned range gives us a little bit of wiggle room.
				uint32 coinAmount = m_GiveItemList[i].count * (100 + g_pMain->m_byCoinEventAmount) / 100;
				if (m_GiveItemList[i].count + coinAmount > USHRT_MAX)
					coinAmount = USHRT_MAX;

				pItem.sCount = coinAmount;
				item.itemid = m_GiveItemList[i].sSid;
				item.slot = pItem.sCount;
			}
			item.itemid = m_GiveItemList[i].sSid;
			item.slot = m_GiveItemList[i].count;
			itemList.push_back(item);
			pBundle->Items.push_back(pItem); // emplace_back() would be so much more useful here, but requires C++11.
		}
	}

	pUser = g_pMain->GetUserPtr(m_sMaxDamageUserid);
	if (pUser == nullptr
		|| !pUser->isInGame())
	{
		if (pUser == nullptr)
			delete pBundle;
		return;
	}

	if (!pMap->RegionItemAdd(GetRegionX(), GetRegionZ(), pBundle))
	{
		delete pBundle;
		return;
	}

	switch (GetProtoID())
	{
	case 4301:
	case 4351:
		if (!pUser->VaccuniBoxExp(1))
			return;
		break;
	case 616:
	case 666:
		if (!pUser->VaccuniBoxExp(2))
			return;
		break;
	case 605:
	case 611:
	case 655:
		if (!pUser->VaccuniBoxExp(3))
			return;
		break;
	default:
		break;
	}

	bool bIsNeutralZone = (pUser->GetZoneID() == ZONE_KNIGHT_ROYALE || pUser->GetZoneID() == ZONE_CHAOS_DUNGEON || pUser->GetZoneID() == ZONE_DUNGEON_DEFENCE);

	if (!bIsNeutralZone)
	{
		/*Robin Looting System*/
		_ITEM_DATA* pItemsLooting = pUser->GetItem(SHOULDER);
		if (!pUser->isInParty())
		{
			if (pItemsLooting->nNum == ROBIN_LOOT_ITEM)
			{
				for (items itr : itemList) {
					pUser->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
				}
			}
			else if (pItemsLooting->nNum == AUTOMATIC_DROP_MINING)
			{
				for (items itr : itemList) {
					pUser->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
				}
			}
			else if (pItemsLooting->nNum == AUTOMATIC_DROP_FISHING)
			{
				for (items itr : itemList) {
					pUser->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
				}
			}
		}
		else if (pUser->isInParty())
		{
			_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
			if (pParty != nullptr)
			{
				for (int i = 0; i < MAX_PARTY_USERS; i++)
				{
					if (pParty->uid[i] >= 0)
					{
						CUser* pUserParty = g_pMain->GetUserPtr(pParty->uid[i]);
						if (pUserParty == nullptr)
							continue;

						if (!isInRangeSlow(pUserParty, float(RANGE_50M + RANGE_50M)))
							continue;

						_ITEM_DATA* ShoulderItem = pUserParty->GetItem(SHOULDER);

						if (ShoulderItem->nNum == ROBIN_LOOT_ITEM)
						{
							for (items itr : itemList) {
								pUserParty->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
								break;
							}
						}
						else if (ShoulderItem->nNum == AUTOMATIC_DROP_MINING)
						{
							for (items itr : itemList) {
								pUserParty->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
								break;
							}
						}
						else if (ShoulderItem->nNum == AUTOMATIC_DROP_FISHING)
						{
							for (items itr : itemList) {
								pUserParty->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
								break;
							}
						}
					}
				}
			}
		}
	}

	if (!bIsNeutralZone)
	{
		/*Pet Looting System*/
		if (!pUser->isInParty())
			pUser->LootingPet(GetX(), GetZ());
		else
		{
			_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
			if (pParty != nullptr)
			{
				for (int i = 0; i < MAX_PARTY_USERS; i++)
				{
					if (pParty->uid[i] >= 0)
					{
						CUser* pUserParty = g_pMain->GetUserPtr(pParty->uid[i]);
						if (pUserParty == nullptr)
							continue;

						if (!isInRangeSlow(pUserParty, RANGE_50M))
							continue;

						pUserParty->LootingPet(GetX(), GetZ());
					}
				}
			}
		}
	}

	Packet result(WIZ_ITEM_DROP);
	result << GetID() << pBundle->nBundleID << uint8(1);

	if (!pUser->isInParty())
		pUser->Send(&result);
	else
		g_pMain->Send_PartyMember(pUser->GetPartyID(), &result);
}
#endif
#if 0
void CNpc::GiveNpcHaveItem(Unit *pKiller)
{
	KOMap *pMap = g_pMain->GetZoneByID(GetZoneID());
	if (pMap == nullptr)
		return;

	if (m_sMaxDamageUserid < 0 || m_sMaxDamageUserid > MAX_USER)
		return;

	_NpcGiveItem m_GiveItemList[NPC_HAVE_ITEM_LIST];
	int nCount = 0;

	if (isMonster())
	{
		_MONSTER_DROP_MAIN* pMonsterDropMain = g_pMain->m_MonsterDropMainArray.GetData(GetProtoID());
		if (pMonsterDropMain != nullptr)
		{
			if (pMonsterDropMain->MonsterOpenDrop != 1)
				return;

			int MoneyRandom = myrand(70, 100);
			int iMoney = pMonsterDropMain->MonsterMoney * MoneyRandom / 100;

			if (pKiller != nullptr)
			{
				CUser* pKillerUser = g_pMain->GetUserPtr(pKiller->GetName(), TYPE_CHARACTER);
				if (pKillerUser != nullptr)
				{
					if (pKiller->GetLevel() >= (GetLevel() + 14))
						iMoney = 0;
				}
			}

			if (iMoney > 0)
			{
				if (iMoney >= SHRT_MAX)
					iMoney = 32000;

				m_GiveItemList[nCount].sSid = ITEM_GOLD;
				m_GiveItemList[nCount++].count = iMoney;
			}

			for (int a = 0; a < LOOT_GROUP_ITEMS; a++)
			{
				if (nCount > NPC_HAVE_ITEM_LIST)
					continue;

				if (pMonsterDropMain->ItemListID[a] == 0 ||
					pMonsterDropMain->ItemListPersent[a] == 0)
					continue;

				int iRandom = myrand(1, 10000);
				uint16 PremiumDropUpper = 0;

				if (pKiller != nullptr)
				{
					CUser* pKillerUser = g_pMain->GetUserPtr(pKiller->GetName(), TYPE_CHARACTER);
					if (pKillerUser != nullptr)
					{
						if (pKillerUser->GetPremiumProperty(PremiumDropPercent) > 0)
							PremiumDropUpper += pKillerUser->GetPremiumProperty(PremiumDropPercent) * 20;

						if (pKillerUser->GetClanPremiumProperty(PremiumDropPercent) > 0)
							PremiumDropUpper += pKillerUser->GetClanPremiumProperty(PremiumDropPercent) * 20;

						if (pKillerUser->m_FlashDcBonus > 0)
							PremiumDropUpper += pKillerUser->m_FlashDcBonus * 10;
					}
				}

				int iPer = pMonsterDropMain->ItemListPersent[a] + (int)PremiumDropUpper;

				if (iPer > 10000)
					iPer = 10000;

				if (iRandom > iPer)
					continue;

				printf("ItemListID0 %u \n", pMonsterDropMain->ItemListID[a]);
				_MONSTER_GROUP_ITEM_LIST* pGroupItemList = g_pMain->m_MonsterItemGroupItemListArray.GetData(pMonsterDropMain->ItemListID[a]);
				if (pGroupItemList == nullptr
					|| pGroupItemList->ItemID.size() <= 0)
					continue;

				printf("ItemListID1 %u \n", pMonsterDropMain->ItemListID[a]);
				int RandomItemID = pGroupItemList->ItemID[myrand(1, (int32)pGroupItemList->ItemID.size()) - 1];
				if (RandomItemID == 0)
					continue;

				_ITEM_TABLE* pMakeItemData = g_pMain->m_ItemtableArray.GetData(RandomItemID);
				if (pMakeItemData == nullptr)
					continue;

				m_GiveItemList[nCount].sSid = RandomItemID;
				if (COMPARE(m_GiveItemList[nCount].sSid, ARROW_MIN, ARROW_MAX))
					m_GiveItemList[nCount].count = 20;
				else
					m_GiveItemList[nCount].count = 1;

				nCount++;

				if (nCount > NPC_HAVE_ITEM_LIST)
					continue;
			}

			if (nCount < NPC_HAVE_ITEM_LIST)
			{
				int Counter = NPC_HAVE_ITEM_LIST - nCount;

				for (int b = 0; b < Counter; b++)
				{
					if (Counter > nCount
						|| nCount > NPC_HAVE_ITEM_LIST)
						continue;

					if (pMonsterDropMain->ItemSingleID[b] == 0 ||
						pMonsterDropMain->ItemSinglePersent[b] == 0)
						continue;

					int iRandom = myrand(1, 10000);
					uint16 PremiumDropUpper = 0;

					if (pKiller != nullptr)
					{
						CUser* pKillerUser = g_pMain->GetUserPtr(pKiller->GetName(), TYPE_CHARACTER);
						if (pKillerUser != nullptr)
						{
							if (pKillerUser->GetPremiumProperty(PremiumDropPercent) > 0)
								PremiumDropUpper += pKillerUser->GetPremiumProperty(PremiumDropPercent) * 20;

							if (pKillerUser->GetClanPremiumProperty(PremiumDropPercent) > 0)
								PremiumDropUpper += pKillerUser->GetClanPremiumProperty(PremiumDropPercent) * 20;

							if (pKillerUser->m_FlashDcBonus > 0)
								PremiumDropUpper += pKillerUser->m_FlashDcBonus * 10;
						}
					}
					int iPer = pMonsterDropMain->ItemSinglePersent[b] + (int)PremiumDropUpper;

					if (iPer > 10000)
						iPer = 10000;

					if (iRandom > iPer)
						continue;

					printf("ItemSingleID %u \n", pMonsterDropMain->ItemSingleID[b]);
					int MakeItemID = pMonsterDropMain->ItemSingleID[b];
					if (MakeItemID == 0)
						continue;

					_ITEM_TABLE* pMakeItemData = g_pMain->m_ItemtableArray.GetData(MakeItemID);
					if (pMakeItemData == nullptr)
						continue;

					m_GiveItemList[nCount].sSid = MakeItemID;
					if (COMPARE(m_GiveItemList[nCount].sSid, ARROW_MIN, ARROW_MAX))
						m_GiveItemList[nCount].count = 20;
					else
						m_GiveItemList[nCount].count = 1;

					nCount++;

					if (nCount > NPC_HAVE_ITEM_LIST)
						continue;
				}
			}
		}
		else
		{
			printf("Monster Drop Item is Null Monster. Monster ID: %d \n", GetProtoID());
		}
	}

	if (nCount <= 0)
		return;

	struct items {
		uint32 itemid;
		uint32 slot;
	};

	vector<items> itemList;
	CUser* pUser = nullptr;
	_LOOT_BUNDLE * pBundle = new _LOOT_BUNDLE;

	pBundle->tDropTime = UNIXTIME;
	pBundle->x = GetX();
	pBundle->z = GetZ();
	pBundle->y = GetY();

	for (int i = 0; i < nCount; i++)
	{
		if (g_pMain->GetItemPtr(m_GiveItemList[i].sSid))
		{
			items item;
			_LOOT_ITEM pItem(m_GiveItemList[i].sSid, m_GiveItemList[i].count);
			if (m_GiveItemList[i].sSid == ITEM_GOLD)
			{
				// Add on any additional coins earned because of a global coin event.
				// NOTE: Officially it caps at SHRT_MAX, but that's really only for technical reasons.
				// Using the unsigned range gives us a little bit of wiggle room.
				uint32 coinAmount = m_GiveItemList[i].count * (100 + g_pMain->m_byCoinEventAmount) / 100;
				if (m_GiveItemList[i].count + coinAmount > USHRT_MAX)
					coinAmount = USHRT_MAX;

				pItem.sCount = coinAmount;
				item.itemid = m_GiveItemList[i].sSid;
				item.slot = pItem.sCount;
			}
			item.itemid = m_GiveItemList[i].sSid;
			item.slot = m_GiveItemList[i].count;
			itemList.push_back(item);
			pBundle->Items.push_back(pItem); // emplace_back() would be so much more useful here, but requires C++11.
		}
	}

	pUser = g_pMain->GetUserPtr(m_sMaxDamageUserid);
	if (pUser == nullptr
		|| !pUser->isInGame())
	{
		if (pUser == nullptr)
			delete pBundle;

		return;
	}

	if (!pMap->RegionItemAdd(GetRegionX(), GetRegionZ(), pBundle))
	{
		delete pBundle;
		return;
	}

	switch (GetProtoID())
	{
	case 4301:
	case 4351:
		if (!pUser->VaccuniBoxExp(1))
			return;
		break;
	case 616:
	case 666:
		if (!pUser->VaccuniBoxExp(2))
			return;
		break;
	case 605:
	case 611:
	case 655:
		if (!pUser->VaccuniBoxExp(3))
			return;
		break;
	default:
		break;
	}

	_ITEM_DATA* pItemsLooting = pUser->GetItem(SHOULDER);
	if (!pUser->isInParty()) {
		if (pItemsLooting->nNum == ROBIN_LOOT_ITEM)
		{
			for (items itr : itemList) {
				pUser->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
			}
		}
	}
	else if (pUser->isInParty()) {
		_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
		if (pParty != nullptr)
		{
			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				if (pParty->uid[i] >= 0)
				{
					CUser* pUserParty = g_pMain->GetUserPtr(pParty->uid[i]);
					if (pUserParty == nullptr)
						continue;

					if (!isInRangeSlow(pUserParty, RANGE_50M))
						continue;

					_ITEM_DATA* ShoulderItem = pUserParty->GetItem(SHOULDER);
					if (ShoulderItem->nNum != ROBIN_LOOT_ITEM)
						continue;

					for (items itr : itemList) {
						pUserParty->ItemGetLooting(pBundle->nBundleID, itr.itemid, itr.slot);
						break;
					}
				}
			}
		}
	}

	Packet result(WIZ_ITEM_DROP);
	result << GetID() << pBundle->nBundleID << uint8(1);

	pUser->LootingPet(GetX(), GetZ());

	if (!pUser->isInParty())
		pUser->Send(&result);
	else
		g_pMain->Send_PartyMember(pUser->GetPartyID(), &result);
}
#endif

void CNpc::StateChange(uint8 bState)
{
	switch (bState)
	{
	case NPC_HIDE:
	{
		SendInOut(INOUT_OUT, GetX(), GetZ(), GetY());
		return;
	}break;
	case NPC_SHOW:
	{
		SendInOut(INOUT_IN, GetX(), GetZ(), GetY());
		return;
	}break;
	}

	SetNpcOldState(m_NpcState);
	SetNpcState(bState);

	switch (GetNpcState())
	{
	case NPC_SLEEPING:
		m_Delay = 0;
		m_Target.id = -1;
		m_Target.bSet = false;
		InitTarget();
		break;
	}
}

time_t CNpc::MagicPacket(uint8 opcode, uint32 nSkillID, int16 sCasterID, int16 sTargetID, int16 sData1, int16 sData2, int16 sData3)
{
	_MAGIC_TABLE * pSkill = g_pMain->m_MagictableArray.GetData(nSkillID);
	if (pSkill == nullptr)
		return -1;

	Packet result(WIZ_MAGIC_PROCESS, opcode); // here we emulate a skill packet to be handled.
	result << nSkillID << sCasterID << sTargetID << sData1 << sData2 << sData3;

	CMagicProcess::MagicPacketNpc(result, this);

	// NOTE: Client controls skill hits.
	// Since we block these, we need to handle this ourselves.
	// This WILL need to be rewritten once spawns are though, as it is HORRIBLE.
	if (opcode != MAGIC_EFFECTING)
	{
		CNpc * pNpc = g_pMain->GetNpcPtr(sCasterID, GetZoneID());
		if (pNpc == nullptr)
			return -1;

		pNpc->StateChange(NPC_CASTING);
		pNpc->m_nActiveSkillID = nSkillID;
		pNpc->m_sActiveTargetID = sTargetID;
		pNpc->m_sActiveCastTime = pSkill->bCastTime;
		pNpc->m_sSkillX = sData1;
		pNpc->m_sSkillY = sData2;
		pNpc->m_sSkillZ = sData3;
		return pNpc->m_sActiveCastTime;
	}

	return -1;
}

bool CNpc::RegisterRegionNpc(float x, float z)
{
	KOMap* pMap = GetMap();
	if (pMap == nullptr)
	{
		TRACE("#### Npc-SetUid Zone Fail : [name=%s], zone=%d #####\n", GetName().c_str(), GetZoneID());
		return false;
	}

	int x1 = (int)x / TILE_SIZE;
	int z1 = (int)z / TILE_SIZE;
	int nRX = (int)x / VIEW_DIST;
	int nRY = (int)z / VIEW_DIST;

	if (x1 < 0 || z1 < 0 || x1 >= pMap->GetMapSize() || z1 >= pMap->GetMapSize())
	{
		TRACE("#### SetUid failed : [nid=%d, sid=%d, zone=%d], coordinates out of range of map x=%d, z=%d, map size=%d #####\n",
		GetID(), GetProtoID(), GetZoneID(), x1, z1, pMap->GetMapSize());
		return false;
	}

	if (nRX > pMap->GetXRegionMax() || nRY > pMap->GetZRegionMax() || nRX < 0 || nRY < 0)
	{
		TRACE("#### SetUid Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", GetID(), GetProtoID(), nRX, nRY);
		return false;
	}

	if (GetRegionX() != nRX || GetRegionZ() != nRY)
	{
		int nOld_RX = GetRegionX();
		int nOld_RZ = GetRegionZ();

		AddToRegion(nRX, nRY);
		if (GetRegion())
		{
			RemoveRegion(nOld_RX - nRX, nOld_RZ - nRY);
			InsertRegion(nRX - nOld_RX, nRY - nOld_RZ);
		}
		else
			InsertRegion(nRX, nRY);
	}

	return true;
}

void CUser::MonsterDeathCount()
{
	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing()
		|| GetZoneID() == ZONE_KNIGHT_ROYALE
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
		return;

	uint32 bRandArray[10000];
	memset(&bRandArray, 0, sizeof(bRandArray));
	uint32 bRandSlot = 0;
	uint32 nItemID = 0;
	int offset = 0;

	if (g_pMain->MonsterDeadCount < 0)
		g_pMain->MonsterDeadCount = 0;

	g_pMain->MonsterDeadCount++;

	if (g_pMain->MonsterDeadCount >= 30000)
	{
		if (g_pMain->m_DkmMonsterDeadGiftArray.GetSize() > 0)
		{
			foreach_stlmap(itr, g_pMain->m_DkmMonsterDeadGiftArray)
			{
				_DKM_MONSTER_DEAD_GIFT * pnGiveItemList = itr->second;
				if (pnGiveItemList == nullptr)
					continue;

				if (offset >= 10000)
					break;

				for (int z = 0; z < int(pnGiveItemList->nGiveItemPercent / 5); z++)
				{
					if (z + offset >= 10000)
						break;

					bRandArray[offset + z] = pnGiveItemList->nGiveItemNum;
				}

				offset += int(pnGiveItemList->nGiveItemPercent / 5);
			}
			bRandSlot = myrand(0, offset);
			nItemID = bRandArray[bRandSlot];
		}

		_ITEM_TABLE *pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
		if (pItem != nullptr)
		{
			if (isInParty())
			{
				_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
				if (pParty != nullptr)
				{
					uint8  PartyUserCount = -1;
					uint16 PartyUserUid = -1;

					short partyUsers[MAX_PARTY_USERS];

					for (int i = 0; i < MAX_PARTY_USERS; i++)
						partyUsers[i] = pParty->uid[i];

					for (int i = 0; i < MAX_PARTY_USERS; i++)
					{
						CUser * pUser = g_pMain->GetUserPtr(partyUsers[i]);
						if (pUser == nullptr
							|| !pUser->isInGame())
							continue;

						if (GetEventRoom()
							!= pUser->GetEventRoom())
							continue;

						PartyUserCount++;
					}

					if (PartyUserCount >= 0)
					{
						int PartyRandomUser = myrand(0, PartyUserCount);

						CUser * pPartyUser = g_pMain->GetUserPtr(partyUsers[PartyRandomUser]);
						if (pPartyUser != nullptr)
						{
							if (pPartyUser->GiveItem(nItemID, 1))
							{
								C3DMap * pZoneInfo = g_pMain->m_ZoneArray.GetData(GetZoneID());
								if (pZoneInfo != nullptr)
								{
									if (pZoneInfo != nullptr)
									{
										std::string sNoticeMessage;
										sNoticeMessage = string_format("%s from %s has received %s at %s.", GetNation() == ELMORAD ? "El Morad" : "Karus", GetName().c_str(), pZoneInfo->m_nZoneName.c_str(), pItem->m_sName.c_str());

										Packet result(WIZ_CHAT, uint8(GENERAL_CHAT));
										result << uint8(0) << uint16(-1) << uint8(0) << sNoticeMessage;
										g_pMain->Send_All(&result, nullptr, Nation::ALL);
									}
								}
							}
						}

					}
				}
			}
			else
			{
				if (GiveItem(nItemID, 1))
				{
					C3DMap * pZoneInfo = g_pMain->m_ZoneArray.GetData(GetZoneID());
					if (pZoneInfo != nullptr)
					{
						if (pZoneInfo != nullptr)
						{
							std::string sNoticeMessage;
							sNoticeMessage = string_format("%s from %s has received %s at %s.", GetNation() == ELMORAD ? "El Morad" : "Karus", GetName().c_str(), pZoneInfo->m_nZoneName.c_str(), pItem->m_sName.c_str());

							Packet result(WIZ_CHAT, uint8(GENERAL_CHAT));
							result << uint8(0) << uint16(-1) << uint8(0) << sNoticeMessage;
							g_pMain->Send_All(&result, nullptr, Nation::ALL);
						}
					}
				}
			}
		}
		g_pMain->MonsterDeadCount = 0;
	}
}

void CNpc::RecvPetExp(CNpc* pNpc, int32 iDamage, int iTotalDamage, int iNpcExp, int iNpcLoyalty)
{
	if (!isPet())
		return;
	
	CUser *pUser = g_pMain->GetUserPtr(GetPetID());
	if (pUser == nullptr)
		return;

	_PARTY_GROUP * pParty;
	uint32 nFinalExp, nFinalLoyalty;
	double TempValue = 0;

	if (pNpc == nullptr
		|| !isInRangeSlow(pNpc, RANGE_50M)
		|| (iNpcExp <= 0 && iNpcLoyalty <= 0))
		return;

	// Calculate base XP earned for the damage dealt.
	if (iNpcExp > 0)
	{
		TempValue = iNpcExp * ((double)iDamage / (double)iTotalDamage);
		nFinalExp = (int)TempValue;
		if (TempValue > nFinalExp)
			nFinalExp++;
	}

	// Calculate base NP earned for the damage dealt.
	if (iNpcLoyalty > 0)
	{
		TempValue = iNpcLoyalty * ((double)iDamage / (double)iTotalDamage);
		nFinalLoyalty = (int)TempValue;
		if (TempValue > nFinalLoyalty)
			nFinalLoyalty++;
	}

	// Give solo XP to PET
	if (iNpcExp > 0)
	{
		float fModifier = pNpc->GetRewardModifier(GetLevel());

		TempValue = nFinalExp * fModifier;
		nFinalExp = (int)TempValue;
		if (TempValue > nFinalExp)
			nFinalExp++;

		pUser->SendPetExpChange(nFinalExp, GetID());
	}


	// Handle solo XP/NP gain
	if (!pUser->isInParty()
		|| (pParty = g_pMain->GetPartyPtr(pUser->GetPartyID())) == nullptr)
	{
		if (pUser->isDead() 
			|| isDead())
			return;

		// Calculate the amount to adjust the XP/NP based on level difference.
		float fModifier = pNpc->GetRewardModifier(pUser->GetLevel());

		// Give solo XP
		if (iNpcExp > 0)
		{
			TempValue = nFinalExp * fModifier;
			nFinalExp = (int)TempValue;
			if (TempValue > nFinalExp)
				nFinalExp++;

			pUser->ExpChange(nFinalExp);
		}

		// Give solo NP
		if (iNpcLoyalty > 0)
		{
			bool UseModifier = false;

			if (UseModifier)
			{
				TempValue = nFinalLoyalty * fModifier;
				nFinalLoyalty = (int)TempValue;
				if (TempValue > nFinalLoyalty)
					nFinalLoyalty++;
			}

			pUser->SendLoyaltyChange(nFinalLoyalty);
		}

		return;
	}

	// Handle party XP/NP gain
	std::vector<CUser *> partyUsers;
	uint32 nTotalLevel = 0;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;

		partyUsers.push_back(pUser);
		nTotalLevel += pUser->GetLevel();
	}

	const float fPartyModifierXP = 0.3f;
	const float fPartyModifierNP = 0.2f;

	uint32 nPartyMembers = (uint32)partyUsers.size();

	// Calculate the amount to adjust the XP/NP based on level difference.
	float fModifier = pNpc->GetPartyRewardModifier(nTotalLevel, nPartyMembers);
	//float fModifier = 1.0f;

	if (iNpcExp > 0)
	{
		TempValue = nFinalExp * fModifier;
		nFinalExp = (int)TempValue;
		if (TempValue > nFinalExp)
			nFinalExp++;
	}

	if (iNpcLoyalty > 0)
	{
		TempValue = nFinalLoyalty * fModifier;
		nFinalLoyalty = (int)TempValue;
		if (TempValue > nFinalLoyalty)
			nFinalLoyalty++;
	}

	// Hand out kill rewards to all users in the party and still in range.
	foreach(itr, partyUsers)
	{
		CUser * pUser = (*itr);
		if (pUser->isDead()
			|| !pUser->isInRange(pNpc, RANGE_50M))
			continue;

		if (iNpcExp > 0)
		{
			TempValue = (nFinalExp * (1 + fPartyModifierXP * (nPartyMembers - 1))) * (double)pUser->GetLevel() / (double)nTotalLevel;
			int iExp = (int)TempValue;
			if (TempValue > iExp)
				iExp++;

			pUser->ExpChange(iExp);
		}

		if (iNpcLoyalty > 0)
		{
			TempValue = (nFinalLoyalty * (1 + fPartyModifierNP * (nPartyMembers - 1))) * (double)pUser->GetLevel() / (double)nTotalLevel;
			int iLoyalty = (int)TempValue;
			if (TempValue > iLoyalty)
				iLoyalty++;

			pUser->SendLoyaltyChange(iLoyalty);
		}
	}
}

#pragma region CUser::HandleMonsterDropTester(uint16 sGetID, uint8 sHours)
void CUser::HandleMonsterDropTester(uint16 sGetID, uint8 sHours)
{
	CNpc* pNpc = g_pMain->GetNpcPtr(sGetID, GetZoneID());
	if (pNpc == nullptr)
		return;

	uint32 TotalGold = 0;
	int iPer = 0, iMakeItemCode = 0, iMoney = 0, iRandom, nCount = 0;

	struct GIVE_DROP_LIST { uint32 ItemID = 0; uint16 ItemCount = 0; std::string ItemName = ""; };
	std::vector<GIVE_DROP_LIST> pMonsterDropList;
	
	int t1 = sHours * 60;
	int t2 = t1 * 60;
	int t3 = t2 / 5;

	if (t3 <= 0) return;

	_K_MONSTER_ITEM* pItem = g_pMain->m_MonsterItemArray.GetData(pNpc->m_iItem);

	if (pItem == nullptr) return;

	for (int i = 0; i < t3; i++)
	{
		iRandom = myrand(70, 100);
		iMoney = pNpc->m_iMoney * iRandom / 100;

		if (iMoney > 0)
		{
			if (iMoney >= SHRT_MAX)
				iMoney = 32000;

			uint32 sGoldAmount = iMoney * (100 + g_pMain->m_byCoinEventAmount) / 100;
			TotalGold += sGoldAmount;
		}

		for (int j = 0; j < LOOT_DROP_ITEMS; j++)
		{
			if(pItem->iItem[j] == 0
				|| pItem->sPercent[j] == 0)
				continue;

			iRandom = myrand(1, 10000);
			iPer = pItem->sPercent[j];

			if (GetPremiumProperty(PremiumDropPercent) > 0)
				iPer = iPer * (100 + GetPremiumProperty(PremiumDropPercent)) / 100;

			if (GetClanPremiumProperty(PremiumDropPercent) > 0)
				iPer = iPer * (100 + GetClanPremiumProperty(PremiumDropPercent)) / 100;

			if (m_FlashDcBonus > 0)
				iPer = iPer * (100 + m_FlashDcBonus) / 100;

			if (iPer > 10000)
				iPer = 10000;

			if (iRandom > iPer)
				continue;

			if (pItem->iItem[j] < 100000000)
			{
				if (pItem->iItem[j] < 100)
					iMakeItemCode = pNpc->ItemProdution(pItem->iItem[j]);
				else
				{
					_MAKE_ITEM_GROUP* pGroup = g_pMain->m_MakeItemGroupArray.GetData(pItem->iItem[j]);
					if (pGroup == nullptr
						|| pGroup->iItems.size() <= 0)
						continue;

					iMakeItemCode = pGroup->iItems[myrand(1, (int32)pGroup->iItems.size()) - 1];
				}
				if (iMakeItemCode == 0)
					continue;

				_ITEM_TABLE* pItemTable = g_pMain->m_ItemtableArray.GetData(iMakeItemCode);
				if (pItemTable == nullptr)
					continue;

				bool sAvailable = false;
				foreach(itr, pMonsterDropList)
				{
					if (itr->ItemID == iMakeItemCode)
					{
						itr->ItemCount++;
						sAvailable = true;
						break;
					}
				}
				if (!sAvailable)
				{
					GIVE_DROP_LIST pList;
					pList.ItemID = iMakeItemCode;
					pList.ItemCount = 1;
					pList.ItemName = pItemTable->m_sName;
					pMonsterDropList.push_back(pList);
				}
			}
			else
			{
				_ITEM_TABLE* pItemTable = g_pMain->m_ItemtableArray.GetData(pItem->iItem[j]);
				if (pItemTable == nullptr)
					continue;

				bool sAvailable = false;
				foreach(itr, pMonsterDropList)
				{
					if (itr->ItemID == pItem->iItem[j])
					{
						itr->ItemCount++;
						sAvailable = true;
						break;
					}
				}
				if (!sAvailable)
				{
					GIVE_DROP_LIST pList;
					pList.ItemID = pItem->iItem[j];
					pList.ItemCount = 1;
					pList.ItemName = pItemTable->m_sName;
					pMonsterDropList.push_back(pList);
				}
			}
		}
	}

	Packet result;

	std::string teeext = string_format("--------------------Monster Drop System--------------------");
	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << teeext;
	Send(&result);

	std::string ExpCommand = string_format("[Monster Drop] Total Gold: %d", TotalGold);

	result.clear();
	result.Initialize(WIZ_CHAT);
	result << uint8(ChatType::GM_CHAT) << uint8(0) << uint16(-1) << uint8(0) << ExpCommand;
	Send(&result);

	foreach(itr, pMonsterDropList)
	{
		std::string Command = string_format("[Monster Drop] ItemName: %s, ItemCount: %d", itr->ItemName.c_str(), itr->ItemCount);

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