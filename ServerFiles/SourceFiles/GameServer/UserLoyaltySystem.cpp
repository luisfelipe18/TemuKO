#include "stdafx.h"
#include "KnightsManager.h"

#pragma region CUser::LoyaltyDivide(int16 tid, uint16 bonusNP /*= 0*/)
/**
* @brief	Takes a target's loyalty points (NP)
* 			and rewards some/all to the killer's party (current user).
*
* @param	tid		The target's ID.
* @param	bonusNP Bonus NP to be awarded to the killer's party as-is.
*/
void CUser::LoyaltyDivide(int16 tid, uint16 bonusNP /*= 0*/)
{
	int16 loyalty_source = 0, loyalty_target = 0;
	uint8 total_member = 0;

	_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	CUser* pTUser = g_pMain->GetUserPtr(tid);
	if (pTUser == nullptr 
		|| !pTUser->isInGame())
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser *pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;

		total_member++;
	}

	if (total_member <= 0)
		return;

	//	This is for the Event Battle on Wednesday :(
	if (g_pMain->isWarOpen()
		&& GetZoneID() == (ZONE_BATTLE_BASE + g_pMain->m_byBattleZone))
	{
		if (pTUser->GetNation() == KARUS)
			g_pMain->m_sKarusDead++;
		else
			g_pMain->m_sElmoradDead++;
	}

	if (pTUser->GetZoneID() == ZONE_DELOS && GetZoneID() == ZONE_DELOS)
	{
		if (g_pMain->pCswEvent.Started)
		{
			if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
			{
				if (pTUser->GetLoyalty() == 0) // No cheats allowed...
				{
					loyalty_source = 0;
					loyalty_target = 0;
				}
				else
				{
					loyalty_source = GetLoyaltyDivideSource(total_member);
					loyalty_target = GetLoyaltyDivideTarget();

					if (loyalty_source == 0)
					{
						loyalty_source = 0;
						loyalty_target = 0;
					}
				}
			}
			else if (g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
			{
				if (pTUser->GetLoyalty() == 0) // No cheats allowed...
				{
					loyalty_source = 0;
					loyalty_target = 0;
				}
				else
				{
					loyalty_source = GetLoyaltyDivideSource(total_member);
					loyalty_target = GetLoyaltyDivideTarget();

					if (loyalty_source == 0)
					{
						loyalty_source = 0;
						loyalty_target = 0;
					}
				}
			}
		}
	}
	else if (pTUser->GetNation() != GetNation())
	{
		if (pTUser->GetLoyalty() == 0) // No cheats allowed...
		{
			loyalty_source = 0;
			loyalty_target = 0;
		}
		else
		{
			loyalty_source = GetLoyaltyDivideSource(total_member);
			loyalty_target = GetLoyaltyDivideTarget();

			if (loyalty_source == 0)
			{
				loyalty_source = 0;
				loyalty_target = 0;
			}
		}
	}
	else
		return;

	for (int j = 0; j < MAX_PARTY_USERS; j++) // Distribute loyalty amongst party members.
	{
		bonusNP = 0;
		CUser *pUser = g_pMain->GetUserPtr(pParty->uid[j]);
		if (pUser == nullptr)
			continue;

		if (pUser->isPriest()
			&& pUser->GetID() != GetID()
			&& pUser->hasRival()
			&& !pUser->hasRivalryExpired()
			&& pUser->GetRivalID() == pTUser->GetID())
		{
			bonusNP = RIVALRY_NP_BONUS;
			pUser->RemoveRival();
		}
		else if (pUser->GetID() == GetID()
			&& pUser->hasRival()
			&& !pUser->hasRivalryExpired()
			&& pUser->GetRivalID() == pTUser->GetID())
		{
			bonusNP = RIVALRY_NP_BONUS;
			pUser->RemoveRival();
		}

		if (pUser->isAlive())
			pUser->SendLoyaltyChange(loyalty_source + bonusNP, true, false, pTUser->GetMonthlyLoyalty() > 0 ? true : false);
	}

	pTUser->SendLoyaltyChange(loyalty_target, true, false, pTUser->GetMonthlyLoyalty() > 0 ? true : false);
}
#pragma endregion

#pragma region CUser::GetLoyaltyDivideSource(uint8 totalmember)

int16 CUser::GetLoyaltyDivideSource(uint8 totalmember)
{
	int16 nBaseLoyalty = 0;

	if (GetZoneID() == ZONE_ARDREAM)
		nBaseLoyalty = g_pMain->m_Loyalty_Ardream_Source;
	else if (GetZoneID() == ZONE_RONARK_LAND_BASE)
		nBaseLoyalty = g_pMain->m_Loyalty_Ronark_Land_Base_Source;
	else if (GetZoneID() == ZONE_RONARK_LAND)
		nBaseLoyalty = g_pMain->m_Loyalty_Ronark_Land_Source;
	else if (GetZoneID() == ZONE_KROWAZ_DOMINION)
		nBaseLoyalty = (g_pMain->m_Loyalty_Other_Zone_Source / 100) * 20;
	else if (GetZoneID() == ZONE_KARUS
		|| GetZoneID() == ZONE_ELMORAD
		|| (GetZoneID() >=  ZONE_BATTLE && GetZoneID() <=  ZONE_BATTLE6 ))
		nBaseLoyalty = g_pMain->m_Loyalty_Ronark_Land_Source;
	else
		nBaseLoyalty = g_pMain->m_Loyalty_Other_Zone_Source;

	int16 nMaxLoyalty = (nBaseLoyalty * 3) - 2;
	int16 nMinLoyalty = nMaxLoyalty / MAX_PARTY_USERS;
	int16 nLoyaltySource = nMinLoyalty;

	if (nLoyaltySource > 0)
	{
		for (int i = 0; i < (MAX_PARTY_USERS - totalmember); i++)
			nLoyaltySource += 2;
	}

	return nLoyaltySource -1;
}
#pragma endregion

#pragma region CUser::GetLoyaltyDivideTarget()

int16 CUser::GetLoyaltyDivideTarget()
{
	if (GetZoneID() == ZONE_ARDREAM)
		return g_pMain->m_Loyalty_Ardream_Target;
	else if (GetZoneID() == ZONE_RONARK_LAND_BASE)
		return g_pMain->m_Loyalty_Ronark_Land_Base_Target;
	else if (GetZoneID() == ZONE_RONARK_LAND)
		return g_pMain->m_Loyalty_Ronark_Land_Target;
	else if (GetZoneID() == ZONE_KROWAZ_DOMINION)
		return (g_pMain->m_Loyalty_Other_Zone_Target / 100) * 20;
	else
		return g_pMain->m_Loyalty_Other_Zone_Target;

	return 0;
}

#pragma endregion

#pragma region CUser::SendLoyaltyChange(int32 nChangeAmount /*= 0*/, bool bIsKillReward /* false */, bool bIsBonusReward /* false */, bool bIsAddLoyaltyMonthly /* true */)
/**
* @brief	Adjusts a player's loyalty (NP) and sends the loyalty
* 			change packet.
*
* @param	nChangeAmount	The amount to adjust the loyalty points by.
* @param	bIsKillReward	When set to true, enables the use of NP-modifying buffs
*							and includes monthly NP gains.
*/
void CUser::SendLoyaltyChange(int32 nChangeAmount /*= 0*/, bool bIsKillReward /* false */, bool bIsBonusReward /* false */, bool bIsAddLoyaltyMonthly /* true */)
{
	Packet result(WIZ_LOYALTY_CHANGE, uint8(LOYALTY_NATIONAL_POINTS));
	uint32 nClanLoyaltyAmount = 0;

	int32 nChangeAmountLoyaltyMonthly = m_bNPGainAmount * nChangeAmount / 100;

	// If we're taking NP, we need to prevent us from hitting values below 0.
	if (nChangeAmount < 0)
	{
		// Negate the value so it becomes positive (i.e. -50 -> 50)
		// so we can determine if we're trying to take more NP than we have.
		uint32 amt = -nChangeAmount; /* avoids unsigned/signed comparison warning */

		if (amt > m_iLoyalty)
			m_iLoyalty = 0;
		else
			m_iLoyalty -= amt;

		// We should only adjust monthly NP when NP was lost when killing a player.
		if (bIsKillReward)
		{
			/*if (GetZoneID() == ZONE_ARDREAM 
				|| GetZoneID() == ZONE_RONARK_LAND_BASE)
				bIsAddLoyaltyMonthly = false;
				*/
			if (bIsAddLoyaltyMonthly)
			{
				if (amt > m_iLoyaltyMonthly)
					m_iLoyaltyMonthly = 0;
				else
					m_iLoyaltyMonthly -= amt;
			}
		}
	}
	// We're simply adding NP here.
	else
	{
		AchieveNormalCountAdd(UserAchieveNormalTypes::AchieveReachNP, 0, nullptr);

		// If you're using an NP modifying buff then add the bonus
		nChangeAmount = m_bNPGainAmount * nChangeAmount / 100;

		// Add on any additional NP earned because of a global NP event.
		// NOTE: They officially check to see if the NP is <= 100,000.
		nChangeAmount = nChangeAmount * (100 + g_pMain->m_byNPEventAmount) / 100;

		// We should only apply NP bonuses when NP was gained as a reward for killing a player.
		if (bIsKillReward)
		{
			// Add on any additional NP gained from items/skills.
			nChangeAmount += m_bItemNPBonus + m_bSkillNPBonus;

			// Add monument bonus.
			if (isInPKZone() && GetPVPMonumentNation() == GetNation())
				nChangeAmount += PVP_MONUMENT_NP_BONUS;

			if (GetPremiumProperty(PremiumBonusLoyalty) > 0)
			{
				nChangeAmount += (GetPremiumProperty(PremiumBonusLoyalty));
				m_PlayerKillingLoyaltyPremiumBonus += (GetPremiumProperty(PremiumBonusLoyalty));
			}

			if (GetClanPremiumProperty(PremiumBonusLoyalty) > 0)
			{
				nChangeAmount += (GetClanPremiumProperty(PremiumBonusLoyalty));
				m_PlayerKillingLoyaltyPremiumBonus += (GetClanPremiumProperty(PremiumBonusLoyalty));
			}

			if (m_FlashWarBonus > 0)
			{
				nChangeAmount += m_FlashWarBonus;
				m_PlayerKillingLoyaltyPremiumBonus += m_FlashWarBonus;
			}
		}

		if (m_iLoyalty + nChangeAmount > LOYALTY_MAX)
			m_iLoyalty = LOYALTY_MAX;
		else
			m_iLoyalty += nChangeAmount;

		if ((isInPKZone() && !bIsBonusReward) 
			|| (isClanTournamentinZone() && !bIsBonusReward))
		{
			/*if (GetZoneID() == ZONE_ARDREAM 
				|| GetZoneID() == ZONE_RONARK_LAND_BASE 
				|| GetZoneID() == ZONE_CLAN_WAR_ARDREAM)
				bIsAddLoyaltyMonthly = false;*/

			if (m_PlayerKillingLoyaltyDaily + nChangeAmount > LOYALTY_MAX)
				m_PlayerKillingLoyaltyDaily = LOYALTY_MAX;
			else
				m_PlayerKillingLoyaltyDaily += nChangeAmount;

			UpdatePlayerKillingRank();
		}

		//// We should only apply additional monthly NP when NP was gained as a reward for killing a player.
		if (!bIsBonusReward)
		{
			if (bIsAddLoyaltyMonthly)
			{
				if (nChangeAmountLoyaltyMonthly > 40)
					nChangeAmountLoyaltyMonthly -= 20;
				else if (nChangeAmountLoyaltyMonthly >= 20 && nChangeAmountLoyaltyMonthly < 40)
					nChangeAmountLoyaltyMonthly -= 10;

				if (GetPremiumProperty(PremiumBonusLoyalty) > 0)
					nChangeAmountLoyaltyMonthly += (GetPremiumProperty(PremiumBonusLoyalty));

				if (GetClanPremiumProperty(PremiumBonusLoyalty) > 0)
					nChangeAmountLoyaltyMonthly += (GetClanPremiumProperty(PremiumBonusLoyalty));

				if (m_FlashWarBonus > 0)
					nChangeAmountLoyaltyMonthly += m_FlashWarBonus;

				if (m_iLoyaltyMonthly + nChangeAmountLoyaltyMonthly > LOYALTY_MAX)
					m_iLoyaltyMonthly = LOYALTY_MAX;
				else
					m_iLoyaltyMonthly += nChangeAmountLoyaltyMonthly;
			}
		}

		if (bIsKillReward)
		{
			if (GetPremiumProperty(PremiumBonusLoyalty) > 0)
			{
				m_iLoyalty += (GetPremiumProperty(PremiumBonusLoyalty));

				if (bIsAddLoyaltyMonthly)
					m_iLoyaltyMonthly += (GetPremiumProperty(PremiumBonusLoyalty));

				m_PlayerKillingLoyaltyPremiumBonus += (GetPremiumProperty(PremiumBonusLoyalty));
			}

			if (GetClanPremiumProperty(PremiumBonusLoyalty) > 0)
			{
				m_iLoyalty += (GetClanPremiumProperty(PremiumBonusLoyalty));

				if (bIsAddLoyaltyMonthly)
					m_iLoyaltyMonthly += (GetClanPremiumProperty(PremiumBonusLoyalty));

				m_PlayerKillingLoyaltyPremiumBonus += (GetClanPremiumProperty(PremiumBonusLoyalty));
			}

			if (m_FlashWarBonus > 0)
			{
				m_iLoyalty += m_FlashWarBonus;

				if (bIsAddLoyaltyMonthly)
					m_iLoyaltyMonthly += m_FlashWarBonus;

				m_PlayerKillingLoyaltyPremiumBonus += m_FlashWarBonus;
			}
		}

		CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());

		if (pKnights && pKnights->m_byFlag >= ClanTypeAccredited5 && pKnights->GetClanPointMethod() == 0 && !bIsBonusReward)
		{
			if (pKnights->m_sMembers > 0 && pKnights->m_sMembers <= MAX_CLAN_USERS)
			{
				if (pKnights->m_sMembers <= 5)
					nClanLoyaltyAmount = 1;
				else if (pKnights->m_sMembers <= 10)
					nClanLoyaltyAmount = 2;
				else if (pKnights->m_sMembers <= 15)
					nClanLoyaltyAmount = 3;
				else if (pKnights->m_sMembers <= 20)
					nClanLoyaltyAmount = 4;
				else if (pKnights->m_sMembers <= 25)
					nClanLoyaltyAmount = 5;
				else if (pKnights->m_sMembers <= 30)
					nClanLoyaltyAmount = 6;
				else if (pKnights->m_sMembers <= 35)
					nClanLoyaltyAmount = 7;
				else if (pKnights->m_sMembers <= 40)
					nClanLoyaltyAmount = 8;
				else if (pKnights->m_sMembers <= 45)
					nClanLoyaltyAmount = 9;
				else if (pKnights->m_sMembers > 45)
					nClanLoyaltyAmount = 10;

				m_iLoyalty -= nClanLoyaltyAmount;
				CKnightsManager::AddUserDonatedNP(GetClanID(), m_strUserID, nClanLoyaltyAmount, true);
				AchieveNormalCountAdd(UserAchieveNormalTypes::AchieveKnightContribution, 0, nullptr);
			}

			if (pKnights->sClanBuffLoyalty > 0)
			{
				m_iLoyalty += pKnights->sClanBuffLoyalty;

				if (bIsAddLoyaltyMonthly)
					m_iLoyaltyMonthly += pKnights->sClanBuffLoyalty;

				m_PlayerKillingLoyaltyPremiumBonus += pKnights->sClanBuffLoyalty;
			}
		}
	}

	// Player is give first np, second exp and third meat dumpling etc.
	if (bIsKillReward && nChangeAmount > 0)
	{
		uint16 sEffect = 0;

		if (isInPKZone() || GetMap()->isWarZone())
		{
			uint32 rand = myrand(0, 1000);
			uint32 ExpRate = 700;

			// Harf Toplama Etkinliði
			auto index = myrand(0, g_pMain->m_GiftItemArray.size() - 1);
			auto pitemdata = g_pMain->m_GiftItemArray.at(index);

			auto percent = (uint32_t)myrand(0, 1000);
			if (percent > pitemdata->Percent)
			{
				/*Ardream için harf etkinligi süreli item verilmeyecek */

				//GiveTimedItem(pitemdata->ItemID, 1, pitemdata->Days);
				GiveItem(pitemdata->ItemID, pitemdata->Count);
				GoldGain(pitemdata->Coin);

				//GiveItem(ITEM_KILL_QUEST);
				TRACE("Hediye : %d", pitemdata->ItemID);
			}
			else
			{
				TRACE("Þansýnýz yok\n");
			}
			// Harf Toplama Etkinliði

			if (GetLevel() >= 1 && GetLevel() <= 10)
			{
				if (rand >= ExpRate)
					ExpChange(2500, true);
				else
					ExpChange(1000, true);
			}
			else if (GetLevel() >= 11 && GetLevel() <= 25)
			{
				if (rand >= ExpRate)
					ExpChange(5500, true);
				else
					ExpChange(3500, true);
			}
			else if (GetLevel() >= 26 && GetLevel() <= 30)
			{
				if (rand >= ExpRate)
					ExpChange(7700, true);
				else
					ExpChange(6700, true);
			}
			else if (GetLevel() >= 31 && GetLevel() <= 40)
			{
				if (rand >= ExpRate)
					ExpChange(12540, true);
				else
					ExpChange(9750, true);
			}
			else if (GetLevel() >= 41 && GetLevel() <= 50)
			{
				if (rand >= ExpRate)
					ExpChange(19510, true);
				else
					ExpChange(14510, true);
			}
			else if (GetLevel() >= 51 && GetLevel() <= 60)
			{
				if (rand >= ExpRate)
					ExpChange(33401, true);
				else
					ExpChange(23401, true);
			}
			else if (GetLevel() >= 61 && GetLevel() <= 65)
			{
				if (rand >= ExpRate)
					ExpChange(42410, true);
				else
					ExpChange(35410, true);
			}
			else if (GetLevel() >= 65 && GetLevel() <= 67)
			{
				if (rand >= ExpRate)
					ExpChange(48250, true);
				else
					ExpChange(37880, true);
			}
			else if (GetLevel() >= 68 && GetLevel() <= 70)
			{
				if (rand >= ExpRate)
					ExpChange(62741, true);
				else
					ExpChange(51452, true);
			}
			else if (GetLevel() >= 71 && GetLevel() <= 73)
			{
				if (rand >= ExpRate)
					ExpChange(79440, true);
				else
					ExpChange(65265, true);
			}
			else if (GetLevel() >= 74 && GetLevel() <= 76)
			{
				if (rand >= ExpRate)
					ExpChange(83568, true);
				else
					ExpChange(75412, true);
			}
			else if (GetLevel() >= 77 && GetLevel() <= 79)
			{
				if (rand >= ExpRate)
					ExpChange(126842, true);
				else
					ExpChange(108202, true);
			}
			else if (GetLevel() >= 80 && GetLevel() <= 81)
			{
				if (rand >= ExpRate)
					ExpChange(143294, true);
				else
					ExpChange(128681, true);
			}
			else if (GetLevel() >= 82 && GetLevel() <= 83)
			{
				if (rand >= ExpRate)
					ExpChange(165341, true);
				else
					ExpChange(149726, true);
			}

			uint16 resultCode = MiningResultSuccess;
			sEffect = 13082;

			Packet x(WIZ_MINING, uint8(MiningAttempt));
			x << resultCode << GetID() << sEffect;
			Send(&x);
		}
	}
	result << m_iLoyalty << m_iLoyaltyMonthly
		<< uint32(0) // Clan donations(? Donations made by this user? For the clan overall?)
		<< nClanLoyaltyAmount; // Premium NP(? Additional NP gained?)
	Send(&result);

	// give first np, second exp and third meat dumpling etc.
	if (bIsKillReward 
		&& nChangeAmount > 0)
	{
		if (isInPKZone() 
			|| GetMap()->isWarZone())
			GiveItem(810261000, 1);
	}
}
#pragma endregion

#pragma region CUser::LoyaltyLose(uint32 Loyalty)
bool CUser::LoyaltyLose(uint32 Loyalty)
{
	if (!hasLoyalty(Loyalty))
		return false;

	m_iLoyalty -= Loyalty;

	Packet result(WIZ_LOYALTY_CHANGE, uint8(LOYALTY_NATIONAL_POINTS));
	result << m_iLoyalty << m_iLoyaltyMonthly
		<< uint32(0)
		<< uint32(0);
	Send(&result);

	return true;
}
#pragma endregion