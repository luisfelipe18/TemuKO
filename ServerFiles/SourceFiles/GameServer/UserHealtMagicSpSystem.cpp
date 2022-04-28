#include "stdafx.h"
#include "MagicInstance.h"

using namespace std;

#pragma region CUser::SiegeTransformHpChange(int amount)
void CUser::SiegeTransformHpChange(int amount)
{
	if (amount < 0)
		return;

	Packet result(WIZ_HP_CHANGE);

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	if (amount < 0 && -amount >= m_sHp)
		m_sHp = 0;
	else if (amount >= 0 && m_sHp + amount > m_MaxHp)
		m_sHp = m_MaxHp;
	else
		m_sHp += amount;

	result << m_MaxHp << m_sHp << uint16(-1);
	Send(&result);

	if (isInParty() && GetZoneID() != ZONE_CHAOS_DUNGEON)
		SendPartyHPUpdate();
}
#pragma endregion

#pragma region CUser::HpChange(int amount, Unit *pAttacker /*= nullptr*/, bool isDOT /*= false*/)
/**
* @brief	Changes a user's HP.
*
* @param	amount   	The amount to change by.
* @param	pAttacker	The attacker.
* @param	bSendToAI	true to update the AI server.
*/
void CUser::HpChange(int amount, Unit *pAttacker /*= nullptr*/, bool isDOT /*= false*/)
{
	Packet result;
	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);
	int16 oldHP = m_sHp;
	int originalAmount = amount;
	int mirrorDamage = 0;

	// No cheats allowed
	if (pAttacker && pAttacker->GetZoneID() != GetZoneID())
		return;

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	//printf("%s \n",m_strUserID.c_str());

	// If we're taking damage...
	if (amount < 0)
	{
		if (isGM())
			return;

		if (!isDOT)
			RemoveStealth();

		bool NotUseZone = (GetZoneID() == ZONE_CHAOS_DUNGEON || GetZoneID() == ZONE_KNIGHT_ROYALE);

		// Handle the mirroring of damage.
		if (m_bMirrorDamage && !NotUseZone)
		{
			if (m_bMirrorDamageType)
			{
				CUser *pUserAttacker = g_pMain->GetUserPtr(pAttacker->GetID());;
				if (pUserAttacker != nullptr)
				{
					mirrorDamage = (m_byMirrorAmount * amount) / 100;
					amount -= mirrorDamage;
					pUserAttacker->HpChange(mirrorDamage);
				}
			}
			else
			{
				if (isInParty())
				{
					_PARTY_GROUP *pParty = nullptr;
					CUser *pUser = nullptr;
					mirrorDamage = (m_byMirrorAmount * amount) / 100;
					amount -= mirrorDamage;
					pParty = g_pMain->GetPartyPtr(GetPartyID());
					if (pParty != nullptr)
					{
						mirrorDamage = mirrorDamage / (GetPartyMemberAmount(pParty) - 1);
						for (int i = 0; i < MAX_PARTY_USERS; i++)
						{
							pUser = g_pMain->GetUserPtr(pParty->uid[i]);
							if (pUser == nullptr || pUser == this)
								continue;

							pUser->HpChange(mirrorDamage);
						}
					}
				}
			}
		}

		// Handle mastery passives
		if (isMastered() && !NotUseZone)
		{
			// Matchless: [Passive]Decreases all damages received by 15%
			if (CheckSkillPoint(SkillPointMaster, 10, g_pMain->m_byMaxLevel))
				amount = (85 * amount) / 100;
			// Absoluteness: [Passive]Decrease 10 % demage of all attacks
			else if (CheckSkillPoint(SkillPointMaster, 5, 9))
				amount = (90 * amount) / 100;
		}

		//özel skiller için kod yazýlacak bool for skill
		// Handle mana absorb skills
		if (m_bManaAbsorb > 0 && !NotUseZone)
		{
			int toBeAbsorbed = 0, absortedmana = 0;
			toBeAbsorbed = (originalAmount * m_bManaAbsorb) / 100;
			amount -= toBeAbsorbed;

			if (amount > 0)
				amount = 0;

			absortedmana = amount;

			if (m_bManaAbsorb == 15)
				absortedmana *= 4;

			MSpChange(absortedmana);
		}
	}
	// If we're receiving HP and we're undead, all healing must become damage.
	else if (m_bIsUndead)
	{
		amount = -amount;
		originalAmount = amount;
	}

	if (amount < 0 && -amount >= m_sHp)
		m_sHp = 0;
	else if (amount >= 0 && m_sHp + amount > m_MaxHp)
		m_sHp = m_MaxHp;
	else
		m_sHp += amount;

	bool NotUseZone2 = (GetZoneID() == ZONE_CHAOS_DUNGEON || GetZoneID() == ZONE_KNIGHT_ROYALE);

	if (pAttacker != nullptr
		&& pAttacker->isPlayer()
		&& isDevil())
	{
		if (amount < 0)
		{
			int32 Receive = int32(amount / 3.1);

			result.clear();
			result.Initialize(WIZ_KURIAN_SP_CHANGE);
			result << uint8(2) << uint8(1);
			result << Receive;
			Send(&result);

			AbsorbedAmmount += Receive;

			if (m_sHp > 0)
				m_sHp -= int16(Receive);

			if (AbsorbedAmmount <= ABSORBED_TOTAL)
				CMagicProcess::RemoveType4Buff(BUFF_TYPE_DEVIL_TRANSFORM, this);
		}
	}

	if (pAttacker != nullptr
		&& pAttacker->isPlayer()
		&& m_sHp > 0 && amount < 0
		&& !NotUseZone2)
	{
		if (isWarrior() && isMastered())
		{
			if (CheckSkillPoint(PRO_SKILL4, 10, 23))
			{
				int16 NewHP = oldHP - m_sHp;
				m_sHp += (15 * NewHP) / 100;
			}
		}
		else if ((isRogue() || isMage() || isPriest()) && isMastered())
		{
			if (CheckSkillPoint(PRO_SKILL4, 5, 9))
			{
				int16 NewHP = oldHP - m_sHp;
				m_sHp += (10 * NewHP) / 100;
			}
			else if (CheckSkillPoint(PRO_SKILL4, 10, 23))
			{
				int16 NewHP = oldHP - m_sHp;
				m_sHp += (15 * NewHP) / 100;
			}
		}
	}

	result.clear();
	result.Initialize(WIZ_HP_CHANGE);
	result << m_MaxHp << m_sHp << tid;

	if (GetHealth() > 0
		&& isMastered()
		&& !isMage() && !NotUseZone2)
	{
		const uint16 hp30Percent = (30 * GetMaxHealth()) / 100;
		if ((oldHP >= hp30Percent && m_sHp < hp30Percent)
			|| (m_sHp > hp30Percent))
		{
			SetUserAbility();

			if (m_sHp < hp30Percent)
				ShowEffect(106800); // skill ID for "Boldness", shown when a player takes damage.
		}
	}

	Send(&result);

	if (isInParty() && !NotUseZone2)
		SendPartyHPUpdate();

	// Ensure we send the original damage (prior to passives) amount to the attacker 
	// as it appears to behave that way officially.
	if (pAttacker != nullptr
		&& pAttacker->isPlayer())
		TO_USER(pAttacker)->SendTargetHP(0, GetID(), originalAmount);

	if (m_sHp == 0)
	{
		PetOnDeath();
		OnDeath(pAttacker);
	}
}

#pragma endregion

#pragma region CUser::SetMaxHp(int iFlag)
/**
* @brief	Calculates & sets a player's maximum HP.
*
* @param	iFlag	If set to 1, additionally resets the HP to max.
* 					If set to 2, additionally resets the max HP to 100 (i.e. Snow war).
*/
void CUser::SetMaxHp(int iFlag)
{
	_CLASS_COEFFICIENT* p_TableCoefficient = nullptr;
	p_TableCoefficient = g_pMain->m_CoefficientArray.GetData(GetClass());

	if (!p_TableCoefficient)
		return;

	int temp_sta = getStatTotal(STAT_STA);

	if (GetZoneID() == ZONE_SNOW_BATTLE && iFlag == 0)
	{
		if (GetFame() == COMMAND_CAPTAIN || isKing())
			m_MaxHp = 300;
		else
			m_MaxHp = 100;
	}
	else if (GetZoneID() == ZONE_CHAOS_DUNGEON && iFlag == 0 
		|| (GetZoneID() == ZONE_DUNGEON_DEFENCE && iFlag == 0))
		m_MaxHp = 10000 / 10;
	else
	{
		m_MaxHp = (short)(((p_TableCoefficient->HP * GetLevel() * GetLevel() * temp_sta)
			+ 0.1 * (GetLevel() * temp_sta) + (temp_sta / 5)) + m_sMaxHPAmount + m_sItemMaxHp + 20);

		// A player's max HP should be capped at (currently) 14,000 HP.
		if (m_MaxHp > MAX_PLAYER_HP && !isGM())
			m_MaxHp = MAX_PLAYER_HP;

		if (iFlag == 1)
		{
			m_MaxHp = MAX_PLAYER_HP;
			HpChange(m_MaxHp);
		}
		else if (iFlag == 2)
			m_MaxHp = 100;
	}
	// Awakening Max Healt %20 Arttýrma.
	if (isMasteredKurianPortu())
	{
		if (GetZoneID() != ZONE_KNIGHT_ROYALE 
			&& GetZoneID() != ZONE_CHAOS_DUNGEON)
		{
			if (CheckSkillPoint(PRO_SKILL4, 2, 23))
				m_MaxHp += m_MaxHp * 20 / 100;
		}
	}

	//Transformation stats need to be applied here
	if (GetZoneID() == ZONE_DELOS && isSiegeTransformation())
	{
		_MAGIC_TYPE6 * pType = g_pMain->m_Magictype6Array.GetData(m_sTransformSkillID);
		
		if (pType != nullptr)
			m_MaxHp = (short) pType->sMaxHp;

		if (m_MaxHp > 0)
		{
			if (m_sTransformHpchange)
			{
				m_sTransformHpchange = false;
				SiegeTransformHpChange(m_MaxHp);
				return;
			}
		}
	}

	if (m_MaxHp < m_sHp)
	{
		m_sHp = m_MaxHp;
		HpChange(m_sHp);
	}
}

#pragma endregion

#pragma region CUser::SetMaxMp()
/**
* @brief	Calculates & sets a player's maximum MP.
*/
void CUser::SetMaxMp()
{
	_CLASS_COEFFICIENT* p_TableCoefficient = nullptr;
	p_TableCoefficient = g_pMain->m_CoefficientArray.GetData(GetClass());
	if (!p_TableCoefficient) return;

	int temp_intel = 0, temp_sta = 0;
	temp_intel = getStatTotal(STAT_INT) + 30;
	//	if( temp_intel > 255 ) temp_intel = 255;
	temp_sta = getStatTotal(STAT_STA);
	//	if( temp_sta > 255 ) temp_sta = 255;

	if (p_TableCoefficient->MP != 0)
	{
		m_MaxMp = (short)((p_TableCoefficient->MP * GetLevel() * GetLevel() * temp_intel)
			+ (0.1f * GetLevel() * 2 * temp_intel) + (temp_intel / 5) + m_sMaxMPAmount + m_sItemMaxMp + 20);
	}
	else if (p_TableCoefficient->SP != 0)
	{
		m_MaxMp = (short)((p_TableCoefficient->SP * GetLevel() * GetLevel() * temp_sta)
			+ (0.1f * GetLevel() * temp_sta) + (temp_sta / 5) + m_sMaxMPAmount + m_sItemMaxMp);
	}

	//Transformation stats need to be applied here
	if (GetZoneID() == ZONE_DELOS && isSiegeTransformation())
	{
		_MAGIC_TYPE6 * pType = g_pMain->m_Magictype6Array.GetData(m_sTransformSkillID);
		
		if (pType != nullptr)
			m_MaxMp = (short) pType->sMaxMp;

		if (m_MaxMp > 0) {
			if (m_sTransformMpchange)
			{
				m_sTransformMpchange = false;
				MSpChange(m_MaxMp);
				return;
			}
		}
	}

	if (m_MaxMp < m_sMp) {
		m_sMp = m_MaxMp;
		MSpChange(m_sMp);
	}
}
#pragma endregion

#pragma region CUser::MSpChange(int amount)
/**
* @brief	Changes a user's mana points.
*
* @param	amount	The amount to adjust by.
*/
void CUser::MSpChange(int amount)
{
	Packet result(WIZ_MSP_CHANGE);
	int16 oldMP = m_sMp;

	if (isGM() && amount < 0)
		return;

	// TODO: Make this behave unsigned.
	m_sMp += amount;
	if (m_sMp < 0)
		m_sMp = 0;
	else if (m_sMp > m_MaxMp)
		m_sMp = m_MaxMp;

	if (isMasteredMage())
	{
		const uint16 mp30Percent = (30 * GetMaxMana()) / 100;
		if (oldMP >= mp30Percent
			&& GetMana() < mp30Percent)
			ShowEffect(106800); // skill ID for "Boldness", shown when a player loses mana.
	}

	result << m_MaxMp << m_sMp;
	Send(&result);

	if (isInParty() && GetZoneID() != ZONE_CHAOS_DUNGEON)
		SendPartyHPUpdate(); // handles MP too
}

#pragma endregion

void CUser::OnDeathitDoesNotMatter()
{
	switch (GetZoneID())
	{
	case ZONE_CHAOS_DUNGEON:
		if (isInTempleEventZone(GetZoneID()))
		{
			if (isEventUser())
				RobChaosSkillItems();
		}
		break;
	case ZONE_BORDER_DEFENSE_WAR:
		if (isInTempleEventZone(GetZoneID()))
		{
			if (isEventUser())
				BDWDeath();
		}
		break;
	case ZONE_KNIGHT_ROYALE:
		KnightRoyaleisDeathProcess();
		break;
	case ZONE_BATTLE6:
		if (isTowerOwner())
			TowerExitsFunciton(true);
		break;
	case ZONE_DRAKI_TOWER:
		DrakiTowerKickOuts();
		break;
	case ZONE_FORGOTTEN_TEMPLE:
	case ZONE_DELOS_CASTELLAN:
	case ZONE_DUNGEON_DEFENCE:
		KickOutZoneUser(true, ZONE_MORADON);
		break;
	case ZONE_UNDER_CASTLE:
		ItemWoreOut(UTC_ATTACK, -MAX_DAMAGE);
		ItemWoreOut(UTC_DEFENCE, -MAX_DAMAGE);
		break;
	}
}

void CUser::OnDeathKilledPlayer(CUser* pKiller)
{
	if (pKiller != nullptr)
	{
		if (pKiller != this)
		{
			uint16 bonusNP = 0;
			switch (pKiller->GetZoneID())
			{
			case ZONE_CHAOS_DUNGEON:
				if (pKiller->isInTempleEventZone(pKiller->GetZoneID()))
				{
					if (pKiller->isEventUser())
					{
						m_ChaosExpansionDeadCount++;
						UpdateChaosExpansionRank();

						pKiller->m_ChaosExpansionKillCount++;
						pKiller->UpdateChaosExpansionRank();

						pKiller->AchieveWarCountAdd(UserAchieveWarTypes::AchieveKillCountChaos, 0, this);
						SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
					}
				}
				break;
			case ZONE_BORDER_DEFENSE_WAR:
				if (pKiller->isInTempleEventZone(pKiller->GetZoneID()))
				{
					if (pKiller->isEventUser())
					{
						pKiller->BDWUpdateRoomKillCount();
						SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
					}
				}
				break;
			case ZONE_JURAID_MOUNTAIN:
				if (pKiller->isInTempleEventZone(pKiller->GetZoneID()))
				{
					if (pKiller->isEventUser())
					{
						pKiller->JRUpdateRoomKillCount();
						SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
					}
				}
				break;
			case ZONE_DELOS:
				if (g_pMain->m_byBattleOpen == SIEGE_BATTLE)
				{
					_DEATHMATCH_WAR_INFO* pClanInfo = g_pMain->m_KnightSiegeWarClanList.GetData(pKiller->GetClanID());
					if (pClanInfo != nullptr)
					{
						if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
						{
							pKiller->UpateCSWTotalKillCounts();
							SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
						}
					}
				}
				break;
			case ZONE_MORADON:
			case ZONE_MORADON2:
			case ZONE_MORADON3:
			case ZONE_MORADON4:
			case ZONE_MORADON5:
				SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
				break;
			case ZONE_KNIGHT_ROYALE:
				pKiller->KnightRoyaleisKillerProcess();
				break;
			case ZONE_CLAN_WAR_RONARK:
			case ZONE_CLAN_WAR_ARDREAM:
			case ZONE_PARTY_VS_1:
			case ZONE_PARTY_VS_2:
			case ZONE_PARTY_VS_3:
			case ZONE_PARTY_VS_4:
				if (pKiller->isClanTournamentinZone() 
					|| pKiller->isPartyTournamentinZone())
				{
					g_pMain->UpdateClanTournamentScoreBoard(pKiller);
					SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
				}
				break;
			case ZONE_ARDREAM:
			case ZONE_RONARK_LAND:
			case ZONE_RONARK_LAND_BASE:
				if (pKiller->isInPKZone())
				{
					bool bKilledByRival = false;

					// Show death notices in PVP zones
					SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);

					// If the killer has us set as their rival, reward them & remove the rivalry.
					bKilledByRival = (!pKiller->hasRivalryExpired() && pKiller->GetRivalID() == GetID());
					if (bKilledByRival)
					{
						// If we are our killer's rival, use the rival notice instead.
						SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeRival, true);

						// Apply bonus NP for rival kills
						bonusNP += RIVALRY_NP_BONUS;

						// This player is no longer our rival
						if (pKiller->isInGame())
							pKiller->RemoveRival();

						pKiller->AchieveWarCountAdd(UserAchieveWarTypes::AchieveTakeRevange, 0, this);
					}
					// The anger gauge is increased on each death.
					// When your anger gauge is full (5 deaths), you can use the "Anger Explosion" skill.
					if (!hasFullAngerGauge())
						UpdateAngerGauge(++m_byAngerGauge);

					if (pKiller->isInParty() && isPlayer())
					{
						_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pKiller->GetPartyID());
						if (pParty == nullptr)
							return;

						short partyUsers[MAX_PARTY_USERS];
						for (int i = 0; i < MAX_PARTY_USERS; i++)
							partyUsers[i] = pParty->uid[i];

						for (int i = 0; i < MAX_PARTY_USERS; i++)
						{
							CUser* PartyUser = g_pMain->GetUserPtr(partyUsers[i]);
							if (PartyUser == nullptr
								|| !isInRangeSlow(PartyUser, RANGE_50M)
								|| PartyUser->isDead()
								|| !PartyUser->isInGame()
								|| !PartyUser->isPlayer())
								continue;

							if (GetNation() == KARUS)
								PartyUser->QuestV2MonsterCountAdd(KARUS);
							else if (GetNation() == ELMORAD)
								PartyUser->QuestV2MonsterCountAdd(ELMORAD);
						}
					}
					else
					{
						if (!pKiller->isInParty() && isPlayer())
						{
							if (GetNation() == KARUS)
								pKiller->QuestV2MonsterCountAdd(KARUS);
							else if (GetNation() == ELMORAD)
								pKiller->QuestV2MonsterCountAdd(ELMORAD);
						}
					}

					// If we don't have a rival, this player is now our rival for 3 minutes.
					if (!hasRival())
						SetRival(pKiller);

					/* Vanguard System Start */
					CUser *pUser = TO_USER(pKiller);

					/*if (pUser && GetZoneID() == ZONE_ARENA && GetEventRoom() > 2000)
						VSEventProcess(1);*/

					if (pUser != nullptr && g_pMain->VanGuardSelectStatus && VanGuardStatus)
					{

						g_pMain->LogosYolla_Zone("Wanted User", string_format("Aranan %s %s Tarafýndan Öldürülmüþtür!", GetName().c_str(), pUser->GetName().c_str()), 0, 204, 102, ZONE_RONARK_LAND);
						g_pMain->VanGuardSelect = false;
						g_pMain->VanGuardSelectTime = 0;
						g_pMain->VanGuardSelectStatus = false;
						g_pMain->VanGuardTime = 0;
						g_pMain->VanGuardUserName.clear();
						VanGuardStatus = false;

						if (g_pMain->WantedEventSystemWinItem > 1)
							pUser->GiveItem(g_pMain->WantedEventSystemWinItem, g_pMain->WantedEventSystemWinItemCount, true, g_pMain->WantedEventSystemWinItemDays);

						if (g_pMain->WantedEventSystemWinNP > 1)
							pUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNP, false, false, false);

						if (g_pMain->WantedEventSystemWinKC > 1)
							pUser->GiveKnightCash(g_pMain->WantedEventSystemWinKC);

						if (g_pMain->WantedEventSystemWinNationNP > 1)
						{
							SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
							foreach(itr, sessMap)
							{
								CUser* OdulUser = TO_USER(itr->second);

								if (pUser == nullptr)
									continue;

								//if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != pUser->GetNation())
								if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != pUser->GetNation() || pUser->isInPKZone())
									continue;

								OdulUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNationNP, false, false, false);
							}

							g_pMain->LogosYolla_Zone("Wanted User", string_format("%s Karakteri Irkýna %d National Point Kazandýrdý Tebrikler!", pUser->GetName().c_str(), g_pMain->WantedEventSystemWinNationNP), 0, 204, 102, ZONE_RONARK_LAND);

							/*for (int i = 0; i < MAX_USER; i++) //SessionMap ile deðiþecek.
							{

								CUser * OdulUser = g_pMain->GetUserPtr(i);

								//if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != pUser->GetNation())
								if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != pUser->GetNation() || pUser->isInPKZone())
									continue;

								OdulUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNationNP, false, false, false);
							}

							g_pMain->LogosYolla_Zone("Wanted User", string_format("%s Karakteri Irkýna %d National Point Kazandýrdý Tebrikler!", pUser->GetName().c_str(), g_pMain->WantedEventSystemWinNationNP), 0, 204, 102, ZONE_RONARK_LAND);*/
						}
					}
					/* Vanguard System End */

					if (g_pMain->CollectionRaceGameServerSatus)
						pKiller->CollectionRaceProcess(1);
				}
				break;
			case ZONE_ELMORAD:
			case ZONE_KARUS:
				if (g_pMain->isWarOpen())
					SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, false);
				break;

			case ZONE_BATTLE:
			case ZONE_BATTLE2:
			case ZONE_BATTLE3:
			case ZONE_BATTLE4:
			case ZONE_BATTLE5:
			case ZONE_BATTLE6:
				if (g_pMain->isWarOpen())
					SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, false);
				break;
			case ZONE_SNOW_BATTLE:
				if (g_pMain->m_byBattleOpen == SNOW_BATTLE)
				{
					pKiller->GoldGain(SNOW_EVENT_MONEY);
					GetNation() == KARUS ? g_pMain->m_sKarusDead++ : g_pMain->m_sElmoradDead++;;
					SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
				}
				break;
			}

			if (pKiller->GetMap()->m_bGiveLoyalty != 0)
			{
				if (pKiller->isInParty())
					pKiller->LoyaltyDivide(GetID(), bonusNP);
				else
					pKiller->LoyaltyChange(GetID(), bonusNP);
			}

			pKiller->AchieveWarCountAdd(UserAchieveWarTypes::AchieveDefeatEnemy, 0, this);
			m_sWhoKilledMe = pKiller->GetID();
		}
		else
			m_sWhoKilledMe = -1;

		//Logger
		string pKillerPartyUsers;
		string pTargetPartyUsers;
		if (pKiller->isInParty() 
			|| isInParty())
		{
			CUser* pPartyUser;
			_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pKiller->GetPartyID());
			if (pParty)
			{
				for (int i = 0; i < MAX_PARTY_USERS; i++)
				{
					pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
					if (pPartyUser)
						pKillerPartyUsers += string_format("%s,", pPartyUser->GetName().c_str());
				}
			}

			pParty = g_pMain->GetPartyPtr(GetPartyID());
			if (pParty)
			{
				for (int i = 0; i < MAX_PARTY_USERS; i++)
				{
					pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
					if (pPartyUser)
						pTargetPartyUsers += string_format("%s,", pPartyUser->GetName().c_str());
				}
			}

			if (!pKillerPartyUsers.empty())
				pKillerPartyUsers = pKillerPartyUsers.substr(0, pKillerPartyUsers.length() - 1);

			if (!pTargetPartyUsers.empty())
				pTargetPartyUsers = pTargetPartyUsers.substr(0, pTargetPartyUsers.length() - 1);
		}

		DateTime time;
		if (pKillerPartyUsers.empty() && pTargetPartyUsers.empty())
			g_pMain->WriteDeathUserLogFile(string_format("[ USER - %d:%d:%d ] Killer=%s,Target=%s,Zone=%d,X=%d,Z=%d,LoyaltyKiller=%d,LoyaltyMonthlyKiller=%d,LoyaltyTarget=%d,LoyaltyMonthlyTarget=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()), TO_USER(pKiller)->GetLoyalty(), TO_USER(pKiller)->GetMonthlyLoyalty(), GetLoyalty(), GetMonthlyLoyalty()));
		else if (pKillerPartyUsers.empty() && !pTargetPartyUsers.empty())
			g_pMain->WriteDeathUserLogFile(string_format("[ USER - %d:%d:%d ] Killer=%s,Target=%s,TargetParty=%s,Zone=%d,X=%d,Z=%d,LoyaltyKiller=%d,LoyaltyMonthlyKiller=%d,LoyaltyTarget=%d,LoyaltyMonthlyTarget=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), GetName().c_str(), pTargetPartyUsers.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()), TO_USER(pKiller)->GetLoyalty(), TO_USER(pKiller)->GetMonthlyLoyalty(), GetLoyalty(), GetMonthlyLoyalty()));
		else if (!pKillerPartyUsers.empty() && pTargetPartyUsers.empty())
			g_pMain->WriteDeathUserLogFile(string_format("[ USER - %d:%d:%d ] Killer=%s,KillerParty=%s,Target=%s,Zone=%d,X=%d,Z=%d,LoyaltyKiller=%d,LoyaltyMonthlyKiller=%d,LoyaltyTarget=%d,LoyaltyMonthlyTarget=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), pKillerPartyUsers.c_str(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()), TO_USER(pKiller)->GetLoyalty(), TO_USER(pKiller)->GetMonthlyLoyalty(), GetLoyalty(), GetMonthlyLoyalty()));
		else if (!pKillerPartyUsers.empty() && !pTargetPartyUsers.empty())
			g_pMain->WriteDeathUserLogFile(string_format("[ USER - %d:%d:%d ] Killer=%s,KillerParty=%s,Target=%s,TargetParty=%s,Zone=%d,X=%d,Z=%d,LoyaltyKiller=%d,LoyaltyMonthlyKiller=%d,LoyaltyTarget=%d,LoyaltyMonthlyTarget=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), pKillerPartyUsers.c_str(), GetName().c_str(), pTargetPartyUsers.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()), TO_USER(pKiller)->GetLoyalty(), TO_USER(pKiller)->GetMonthlyLoyalty(), GetLoyalty(), GetMonthlyLoyalty()));

	}
}

void CUser::OnDeathKilledNpc(CNpc* pKiller)
{
	if (pKiller != nullptr)
	{
		int64 nExpLost = 0;
		if (GetMap()->m_bExpLost != 0)
		{
			bool isNationZoneExpLost = ((GetNation() == KARUS && GetZoneID() == ZONE_ELMORAD) || (GetNation() == ELMORAD && GetZoneID() == ZONE_KARUS));

			if (isNationZoneExpLost
				|| pKiller->GetType() == NPC_PATROL_GUARD)
				nExpLost = m_iMaxExp / 100;
			else
				nExpLost = m_iMaxExp / 20;

			if (GetPremiumProperty(PremiumExpRestorePercent) > 0)
				nExpLost = nExpLost * (GetPremiumProperty(PremiumExpRestorePercent)) / 100;

			if (GetClanPremiumProperty(PremiumExpRestorePercent) > 0)
				nExpLost = nExpLost * (GetClanPremiumProperty(PremiumExpRestorePercent)) / 100;

			if (pKiller->GetType() != NPC_ROLLINGSTONE
				&& pKiller->GetProtoID() != SAW_BLADE_SSID)
				ExpChange(-nExpLost);
		}

		switch (pKiller->GetZoneID())
		{
		case ZONE_ARDREAM:
		case ZONE_RONARK_LAND:
		case ZONE_RONARK_LAND_BASE:
			if (pKiller->GetType() == NPC_GUARD_TOWER1 || pKiller->GetType() == NPC_GUARD_TOWER2)
				SendDeathNotice(pKiller, DeathNoticeType::DeathNoticeCoordinates, true);
			break;
		case ZONE_KROWAZ_DOMINION:
			if (pKiller->GetType() == NPC_ROLLINGSTONE || pKiller->GetProtoID() == SAW_BLADE_SSID)
				SendLoyaltyChange(-100);
			break;
		case ZONE_BATTLE6:
			if (pKiller->GetType() == NPC_OBJECT_WOOD)
			{
				CUser* pWoodOwner = g_pMain->GetUserPtr(pKiller->GetWoodUserID());
				if (pWoodOwner != nullptr)
				{
					if (pWoodOwner->GetMap()->m_bGiveLoyalty != 0)
					{
						if (pWoodOwner->isInParty())
							pWoodOwner->LoyaltyDivide(GetID(), 0);
						else
							pWoodOwner->LoyaltyChange(GetID(), 0);
					}
				}
			}
			break;
		}
		DateTime time;
		m_AchieveInfo.MonsterDefeatCount = 0;
		g_pMain->WriteDeathUserLogFile(string_format("NPC/MONSTER - %d:%d:%d || SID=%d,Killer=%s,Target=%s,Zone=%d,X=%d,Z=%d,TargetExp=%d,LostExp=%d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetProtoID(), pKiller->GetName().c_str(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()), m_iExp, nExpLost));
	}
}

void CUser::OnDeath(Unit* pKiller)
{
	if (m_bResHpType == USER_DEAD)
		return;

	m_bResHpType = USER_DEAD;
	OnDeathitDoesNotMatter();

	if (pKiller != nullptr)
	{
		if (pKiller->isPlayer())
		{
			CUser* pUserKiller = TO_USER(pKiller);
			{
				if (pUserKiller != nullptr)
				{
					if (pUserKiller != this)
					{
						OnDeathKilledPlayer(pUserKiller);
					}
				}
			}
		}
		else if (pKiller->isNPC())
		{
			CNpc* pNpcKiller = TO_NPC(pKiller);
			if (pNpcKiller != nullptr)
			{
				OnDeathKilledNpc(pNpcKiller);
			}
		}
		else
		{
			printf("OnDeath warning \n");
			TRACE("OnDeath warning \n");
		}
	}
	InitOnDeath(pKiller);
}

#pragma region CUser::InitOnDeath(Unit *pKiller)
/**
* @brief	Inits the user after ondeath is proceeded.
*
*/
void CUser::InitOnDeath(Unit *pKiller)
{
	CheckRespawnScroll();
	Unit::OnDeath(pKiller);
	
	// Player is dead stop other process.
	ResetWindows();
	InitType3();
	InitType4();

	CMagicProcess::CheckExpiredType9Skills(this, true);

	if (isTransformed() && !isNPCTransformation())
	{
		MagicInstance instance;
		instance.pSkillCaster = this;
		instance.nSkillID = m_sTransformSkillID;
		instance.Type6Cancel(true);
	}
}
#pragma endregion

#pragma region CUser::CheckRespawnScroll()
/**
* @brief	Checks if any respawn scroll exists on the user. If so, sends the
*			emulating packets to the user.
*
*/
void CUser::CheckRespawnScroll()
{
	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA *pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		// This implementation fixes a bug where it ignored the possibility for multiple stacks.
		if ((pItem->nNum == 800036000 
			&& pItem->sCount >= 1)
			|| (pItem->nNum == 800039000 
				&& pItem->sCount >= 1)
			|| (pItem->nNum == 910022000 
				&& pItem->sCount >= 1)
			|| (pItem->nNum == 900699000 
				&& pItem->sCount >= 1)
			|| (pItem->nNum == 810036000
				&& pItem->sCount >= 1)
			|| (pItem->nNum == 900136000 
				&& pItem->sCount >= 1)
			|| (pItem->nNum == 910948000 
				&& pItem->sCount >= 1))
		{
			Packet result(WIZ_DEAD);
			result << GetID();
			result << uint32(18483) << uint64(0);
			Send(&result);
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::SpChange(int amount)
void CUser::SpChange(int amount)
{
	Packet result(WIZ_KURIAN_SP_CHANGE);

	if (isBeginnerKurianPortu())
		m_MaxSp = 100;
	else if (isNoviceKurianPortu())
		m_MaxSp = 150;
	else if (isMasteredKurianPortu())
	{
		if (CheckSkillPoint(PRO_SKILL4, 0, 2))
			m_MaxSp = 200;
		else if (CheckSkillPoint(PRO_SKILL4, 3, 23))
			m_MaxSp = 250;
		else
			m_MaxSp = 200;
	}
	else
		m_MaxSp = 200;

	m_sSp += amount;

	if (m_sSp < 0)
		m_sSp = 0;
	else if (m_sSp >= m_MaxSp)
		m_sSp = m_MaxSp;

	result << uint8(1) << uint8(1);
	result << uint8(m_MaxSp) << uint8(m_sSp);
	Send(&result);
}
#pragma endregion

void CUser::HealMagic()
{
	C3DMap* pMap = GetMap();

	if (pMap == nullptr)
		return;

	int16 rx = GetRegionX(), rz = GetRegionZ();

	foreach_region(x, z)
		HealAreaCheck(pMap, rx + x, rz + z);
}

void CUser::HealAreaCheck(C3DMap * pMap, int rx, int rz)
{
	static const float fRadius = 10.0f; // 30m

	if (pMap == nullptr)
		return;

	CRegion * pRegion = pMap->GetRegion(rx, rz);
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
			|| !pNpc->isHostileTo(this)
			|| (GetEventRoom() >= 0 && GetEventRoom() != pNpc->GetEventRoom())
			|| pNpc->GetProtoID() == SAW_BLADE_SSID)
			continue;

		if (isInRangeSlow(pNpc, fRadius))
			pNpc->ChangeTarget(1004, this);
	}
}

void CUser::SendMannerChange(int32 iMannerPoints)
{
	//Make sure we don't have too many or too little manner points!
	if (m_iMannerPoint + iMannerPoints > LOYALTY_MAX)
		m_iMannerPoint = LOYALTY_MAX;
	else if (m_iMannerPoint + iMannerPoints < 0)
		m_iMannerPoint = 0;
	else
		m_iMannerPoint += iMannerPoints;

	Packet pkt(WIZ_LOYALTY_CHANGE, uint8(LOYALTY_MANNER_POINTS));
	pkt << m_iMannerPoint;
	Send(&pkt);
}

void CUser::SetMaxSp()
{
	if (isBeginnerKurianPortu())
		m_MaxSp = 100;
	else if (isNoviceKurianPortu())
		m_MaxSp = 150;
	else if (isMasteredKurianPortu())
	{
		if (CheckSkillPoint(PRO_SKILL4, 0, 2))
			m_MaxSp = 200;
		else if (CheckSkillPoint(PRO_SKILL4, 3, 23))
			m_MaxSp = 250;
		else
			m_MaxSp = 200;
	}
	else
		m_MaxSp = 200;

	if (m_MaxSp < m_sSp)
	{
		m_sSp = m_MaxSp;
		SpChange(m_sSp);
	}
}