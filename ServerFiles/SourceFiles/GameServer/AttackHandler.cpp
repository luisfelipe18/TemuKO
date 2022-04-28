#include "stdafx.h"
#include "Map.h"

void CUser::Attack(Packet & pkt)
{
	int16 sid = -1, tid = -1, damage, delaytime, distance;
	uint8 bType, bResult = 0, unknown;
	Unit * pTarget = nullptr;

	pkt >> bType >> bResult >> tid >> delaytime >> distance >> unknown;

	if (isIncapacitated())
		return;

	if (isInEnemySafetyArea())
		return;

	RemoveStealth();

	// If you're holding a weapon, do a client-based (ugh, do not trust!) delay check.
	_ITEM_TABLE* pTable = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND);
	if (pTable != nullptr 
		&& !isMage())
	{
		if (!pTable->isPickaxe()) 
		{
			if (pTable->isTimingDelay())
			{
				if (delaytime < (pTable->m_sDelay + 9)
					|| distance > pTable->m_sRange)
					return;
			}
			else if (pTable->isWirinomUniqDelay()
				|| pTable->isWirinomRebDelay()
				|| pTable->isGargesSwordDelay())
			{
				if (delaytime < (pTable->m_sDelay - 4)
					|| distance > pTable->m_sRange)
					return;
			}
			else
			{
				if (delaytime < (pTable->m_sDelay + 10)
					|| distance > pTable->m_sRange)
					return;
			}
		}
	}
	// Empty handed.
	else if (delaytime < 100)
		return;

	pTarget = g_pMain->GetUnitPtr(tid, GetZoneID());
	bResult = ATTACK_FAIL;

	if (pTarget != nullptr
		&& isInAttackRange(pTarget)
		&& CanAttack(pTarget))
	{
		if (isAttackable(pTarget) 
			&& CanCastRHit(GetSocketID()))
		{
			if (isInTempleEventZone()
				&& (!isSameEventRoom(pTarget)
					|| !g_pMain->pTempleEvent.isAttackable))
				return;

			if (isInTempleQuestEventZone()
				&& (!isSameEventRoom(pTarget)
					&& isQuestEventUser()))
				return;

			if (pTarget->isPlayer() 
				&& pTarget->hasBuff(BUFF_TYPE_FREEZE))
				return;

			CUser *pUser = g_pMain->GetUserPtr(GetSocketID());

			if (pUser != nullptr)
				pUser->m_RHitRepeatList.insert(std::make_pair(GetSocketID(), UNIXTIME));

			damage = GetDamage(pTarget);

			// Can't use R attacks in the Snow War.
			if (GetZoneID() == ZONE_SNOW_BATTLE 
				&& g_pMain->m_byBattleOpen == SNOW_BATTLE)
				damage = 0;
			else if (GetZoneID() == ZONE_CHAOS_DUNGEON 
				&& g_pMain->pTempleEvent.isAttackable)
				damage = 500 / 10;
			
			if (GetZoneID() == ZONE_DUNGEON_DEFENCE)
			{
				_DUNGEON_DEFENCE_ROOM_INFO* pRoomBilgi = g_pMain->m_DungeonDefenceRoomListArray.GetData(GetEventRoom());
				if (pRoomBilgi != nullptr)
				{
					if (pRoomBilgi->m_DefenceisStarted == true)
						damage = 500 / 10;
				}
			}

			if (!pTarget->isPlayer())
			{
				if (TO_NPC(pTarget)->GetType() == NPC_PRISON)
				{
					if (GetMana() < ((int32)m_MaxMp * 5 / 100))
						return;

					_ITEM_DATA * pItem;
					_ITEM_TABLE* pTable = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND, pItem);
					if (pItem == nullptr || pTable == nullptr
						|| pItem->sDuration <= 0 // are we supposed to wear the pickaxe on use? Need to verify.
						|| !pTable->isPunishmentStick())
						damage = 0;
					else
					{
						damage = 1;
						MSpChange(-((int32)m_MaxMp * 5 / 100));
					}
				}
				else if (TO_NPC(pTarget)->GetType() == NPC_FOSIL)
				{
					_ITEM_DATA * pItem;
					_ITEM_TABLE* pTable = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND, pItem);
					if (pItem == nullptr || pTable == nullptr
						|| pItem->sDuration <= 0 // are we supposed to wear the pickaxe on use? Need to verify.
						|| !pTable->isPickaxe())
						damage = 0;
					else
						damage = 1;
				}
				else if (TO_NPC(pTarget)->GetType() == NPC_OBJECT_FLAG 
					&& TO_NPC(pTarget)->GetProtoID() == 511)
					damage = 1;
				else if (TO_NPC(pTarget)->GetType() == NPC_REFUGEE)
				{
					if (TO_NPC(pTarget)->isMonster())
					{
						if (TO_NPC(pTarget)->GetProtoID() == 3202 
							|| TO_NPC(pTarget)->GetProtoID() == 3203
							|| TO_NPC(pTarget)->GetProtoID() == 3252 
							|| TO_NPC(pTarget)->GetProtoID() == 3253)
							damage = 20;
						else
							damage = 10;
					}
					else
						damage = 10;
				}
				else if (TO_NPC(pTarget)->GetType() == NPC_TREE)
					damage = 20;
				else if (TO_NPC(pTarget)->GetNation() == Nation::NONE 
					&& TO_NPC(pTarget)->GetType() == NPC_PARTNER_TYPE)
					damage = 0;
				else if (TO_NPC(pTarget)->GetType() == NPC_BORDER_MONUMENT)
					damage = 10;
				else if (!VaccuniAttack())
					damage = 30000;
			}

			if (damage > 0)
			{
				pTarget->HpChange(-damage, this);
				if (pTarget->isDead())
					bResult = ATTACK_TARGET_DEAD;
				else
					bResult = ATTACK_SUCCESS;

				// Every attack takes a little of your weapon's durability.
				ItemWoreOut(ATTACK, damage);

				// Every hit takes a little of the defender's armour durability.
				if (pTarget->isPlayer())
					TO_USER(pTarget)->ItemWoreOut(DEFENCE, damage);
			}
		}
	}

	Packet result(WIZ_ATTACK, bType);
	result << bResult << GetSocketID() << tid << unknown;
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, nullptr, GetEventRoom());
	else
		SendToRegion(&result);
}

void CUser::Regene(uint8 regene_type, uint32 magicid /*= 0*/)
{
	if(GetMap() == nullptr)
		return;

	_OBJECT_EVENT* pEvent = nullptr;
	_START_POSITION* pStartPosition = nullptr;
	float x = 0.0f, z = 0.0f;

	if (!isDead())
		return;

	if (regene_type != 1 && regene_type != 2)
		regene_type = 1;

	if (regene_type == 2) 
	{
		// Is our level high enough to be able to resurrect using this skill?
		if (GetLevel() <= 5
			// Do we have enough resurrection stones?
				|| !RobItem(379006000, 3 * GetLevel()))
				return;
	}

	// If we're in a home zone, we'll want the coordinates from there. Otherwise, assume our own home zone.
	pStartPosition = g_pMain->m_StartPositionArray.GetData(GetZoneID());
	if (pStartPosition == nullptr)
		return;

	UserInOut(INOUT_OUT);

	pEvent = GetMap()->GetObjectEvent(m_sBind);	

	// If we're not using a spell to resurrect.
	if (magicid == 0) 
	{
		// Resurrect at a bind/respawn point
		if (pEvent && pEvent->byLife == 1)
		{
			SetPosition(pEvent->fPosX + x, 0.0f, pEvent->fPosZ + z);
			x = pEvent->fPosX;
			z = pEvent->fPosZ;
		}
		// Are we trying to respawn in a home zone?
		// If we're in a war zone (aside from snow wars, which apparently use different coords), use BattleZone coordinates.
		else if ((GetZoneID() <= ZONE_ELMORAD) || (GetZoneID() != ZONE_SNOW_BATTLE && GetZoneID() == (ZONE_BATTLE_BASE + g_pMain->m_byBattleZone))) 
		{
			// Use the proper respawn area for our nation, as the opposite nation can
			// enter this zone at a war's invasion stage.
			x = (float)((GetNation() == KARUS ? pStartPosition->sKarusX :  pStartPosition->sElmoradX) + myrand(0, pStartPosition->bRangeX));
			z = (float)((GetNation() == KARUS ? pStartPosition->sKarusZ :  pStartPosition->sElmoradZ) + myrand(0, pStartPosition->bRangeZ));
		}
		else
		{
			short sx, sz;
			// If we're in a war zone (aside from snow wars, which apparently use different coords), use BattleZone coordinates.
			if (isInMoradon() && isInArena())
			{
				x = (float)(MINI_ARENA_RESPAWN_X + myrand(-MINI_ARENA_RESPAWN_RADIUS, MINI_ARENA_RESPAWN_RADIUS));
				z = (float)(MINI_ARENA_RESPAWN_Z + myrand(-MINI_ARENA_RESPAWN_RADIUS, MINI_ARENA_RESPAWN_RADIUS));
			}
			else if (GetZoneID() == ZONE_CHAOS_DUNGEON)
			{
				GetStartPositionRandom(sx, sz);
				x = sx;
				z = sz;
			}
			else if (GetZoneID() == ZONE_JURAID_MOUNTAIN && isDevaStage())
			{
				if (GetNation() == KARUS)
				{
					x = float(512 + myrand(0, 3));
					z = float(364 + myrand(0, 3));
				}
				else
				{
					x = float(512 + myrand(0, 3));
					z = float(659 + myrand(0, 3));
				}
			}
			else if (GetZoneID() == ZONE_DELOS)
			{
				if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
				{
					_DEATHMATCH_BARRACK_INFO* pData4 = g_pMain->m_KnightSiegeWarBarrackList.GetData(GetClanID());
					_DEATHMATCH_WAR_INFO* pKnightList = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
					if (pData4 != nullptr && pKnightList != nullptr)
					{
						if (pKnightList->m_tRemainBarracks == 1)
						{
							x = float(pData4->m_tBaseX + myrand(1, 5));
							z = float(pData4->m_tBaseZ + myrand(1, 5));
						}
						else
						{
							GetStartPosition(sx, sz);
							x = sx;
							z = sz;
						}
					}
					else
					{
						GetStartPosition(sx, sz);
						x = sx;
						z = sz;
					}
				}
				else
				{
					GetStartPosition(sx, sz);
					x = sx;
					z = sz;
				}
			}
			// For all else, just grab the start position (/town coordinates) from the START_POSITION table.
			else
			{
				GetStartPosition(sx, sz);
				x = sx;
				z = sz;
			}
		}

		SetPosition(x, 0.0f, z);

		m_LastX = x;
		m_LastZ = z;

		m_bResHpType = USER_STANDING;	
		m_bRegeneType = REGENE_NORMAL;
	}
	else // we're respawning using a resurrect skill.
	{
		_MAGIC_TYPE5 * pType = g_pMain->m_Magictype5Array.GetData(magicid);     
		if (pType == nullptr)
			return;

		if (GetZoneID() != ZONE_UNDER_CASTLE)
		{
			MSpChange(-((int32)m_MaxMp)); // reset us to 0 MP. 

			if (m_sWhoKilledMe == -1)
				ExpChange((m_iLostExp * pType->bExpRecover) / 100); // Restore 
		}

		m_bResHpType = USER_STANDING;
		m_bRegeneType = REGENE_MAGIC;
	}

	Packet result(WIZ_REGENE);
	result << GetSPosX() << GetSPosZ() << GetSPosY();
	Send(&result);

	m_tLastRegeneTime = UNIXTIME;
	m_sWhoKilledMe = -1;
	m_iLostExp = 0;


	SetRegion(GetNewRegionX(), GetNewRegionZ());

	UserInOut(INOUT_RESPAWN);		

	g_pMain->RegionUserInOutForMe(this);
	g_pMain->RegionNpcInfoForMe(this);

	InitializeStealth();
	SendUserStatusUpdate(USER_STATUS_DOT, USER_STATUS_CURE);
	SendUserStatusUpdate(USER_STATUS_POISON, USER_STATUS_CURE);

	if (isInArena())
		SendUserStatusUpdate(USER_STATUS_SPEED, USER_STATUS_CURE);

	HpChange(GetMaxHealth());

	if (GetZoneID() == ZONE_UNDER_CASTLE)
		MSpChange(GetMaxMana());

	if (!isBlinking()
		&& GetZoneID() != ZONE_CHAOS_DUNGEON
		&& GetZoneID() != ZONE_DUNGEON_DEFENCE
		&& GetZoneID() != ZONE_KNIGHT_ROYALE)
	{
		InitType4();
		RecastSavedMagic();
	}

	if(GetZoneID() == ZONE_CHAOS_DUNGEON)
		BlinkStart(-10);
	else if (magicid == 0 
		&& !isNPCTransformation())
		BlinkStart();

	if (magicid == 0 
		&& GetZoneID() == ZONE_KNIGHT_ROYALE)
		KnightRoyaleRegeneProcess();

	// If we actually respawned (i.e. we weren't resurrected by a skill)...
	if (magicid == 0)
	{
		// In PVP zones (not war zones), we must kick out players if they no longer
		// have any national points.
		if (GetLoyalty() == 0 
			&& (GetMap()->isWarZone()
			|| isInPKZone()))
			KickOutZoneUser();
	}
}