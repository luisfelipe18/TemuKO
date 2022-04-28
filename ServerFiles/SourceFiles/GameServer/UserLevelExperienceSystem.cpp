#include "stdafx.h"

using namespace std;

/**
* @brief	Changes the player's experience points by iExp.
*
* @param	iExp	The amount of experience points to adjust by.
*/
void CUser::ExpChange(int64 iExp, bool bIsBonusReward)
{
	// GetMap Nullptr crash control
	if (GetMap() == nullptr)
		return;

	// Stop players level 5 or under from losing XP on death.
	if ((GetLevel() < 6 && iExp < 0)
		// Stop players in the war zone (TODO: Add other war zones) from losing XP on death.
		|| (GetMap()->isWarZone() && iExp < 0))
		return;

	// Despite being signed, we don't want m_iExp ever going below 0.
	// If this happens, we need to investigate why -- not sweep it under the rug.
	ASSERT(m_iExp >= 0);

	if (iExp > 0)
	{
		if (!bIsBonusReward)
		{
			// Adjust the exp gained based on the percent set by the buff
			iExp = iExp * (m_sExpGainAmount + m_bItemExpGainAmount) / 100;

			// Add on any additional XP earned because of a global XP event.
			// NOTE: They officially check to see if the XP is <= 100,000.
			iExp = iExp * (100 + g_pMain->m_byExpEventAmount) / 100;

			if (GetPremiumProperty(PremiumExpPercent) > 0)
				iExp = iExp * (100 + GetPremiumProperty(PremiumExpPercent)) / 100;

			if (GetClanPremiumProperty(PremiumExpPercent) > 0)
				iExp = iExp * (100 + GetClanPremiumProperty(PremiumExpPercent)) / 100;

			CKnights* pKnight = g_pMain->GetClanPtr(GetClanID());
			if (pKnight != nullptr) {
				if (pKnight->sClanBuffExp > 0) {
					iExp = iExp * (100 + pKnight->sClanBuffExp) / 100;
				}
			}

			if (m_FlashExpBonus > 0)
				iExp = iExp * (100 + m_FlashExpBonus) / 100;
		}
	}

	bool bLevel = true;
	if (iExp < 0
		&& (m_iExp + iExp) < 0)
		bLevel = false;
	else
	{
		if (bExpSealStatus)
			ExpSealChangeExp(iExp);
		else
			m_iExp += iExp;
	}
	// If we need to delevel...
	if (!bLevel)
	{
		// Drop us back a level.
		m_bLevel--;

		// Get the excess XP (i.e. below 0), so that we can take it off the max XP of the previous level
		// Remember: we're deleveling, not necessarily starting from scratch at the previous level
		int64 diffXP = m_iExp + iExp;

		// Now reset our XP to max for the former level.
		m_iExp = g_pMain->GetExpByLevel(GetLevel(), GetRebirthLevel());

		// Get new stats etc.
		LevelChange(GetLevel(), false);

		// Take the remainder of the XP off (and delevel again if necessary).
		ExpChange(diffXP);
		return;
	}
	// If we've exceeded our XP requirement, we've leveled.
	else if (m_iExp >= m_iMaxExp)
	{
		if (GetLevel() < g_pMain->m_byMaxLevel
			&& !(g_pMain->MaxLevel62Control && GetLevel() == 62))
		{
			// Reset our XP, level us up.
			m_iExp -= m_iMaxExp;
			LevelChange(++m_bLevel);
			return;
		}

		// Hit the max level? Can't level any further. Cap the XP.
		m_iExp = m_iMaxExp;
	}

	if (GetLevel() < GetMap()->GetMinLevelReq()
		|| GetLevel() > GetMap()->GetMaxLevelReq()) {
		KickOutZoneUser();
	}

	// Tell the client our new XP
	Packet result(WIZ_EXP_CHANGE);
	result << uint8(1) << m_iExp; // NOTE: Use proper flag
	Send(&result);

	// If we've lost XP, save it for possible refund later.
	if (iExp < 0)
		m_iLostExp = -iExp;
}

/**
* @brief	Handles stat updates after a level change.
* 			It does not change the level.
*
* @param	level   	The level we've changed to.
* @param	bLevelUp	true to level up, false for deleveling.
*/
void CUser::LevelChange(uint8 level, bool bLevelUp)
{
	if (level < 1 || level > g_pMain->m_byMaxLevel)
		return;

	if (g_pMain->MaxLevel62Control
		&& ((GetLevel() == 62 && level == 83) || (GetLevel() == 83 && level == 62)))
	{
		for (int i = 0; i < SLOT_MAX; i++)
		{
			if (m_sItemArray[i].nNum)
				return;
		}
	}

	if (bLevelUp && level > GetLevel() + 1)
	{
		int16 nStatTotal = 300 + (level - 1) * 3;
		uint8 nSkillTotal = (level - 9) * 2;

		if (level > 60)
			nStatTotal += 2 * (level - 60);

		m_sPoints += nStatTotal - GetStatTotal();
		m_bstrSkill[SkillPointFree] += nSkillTotal - GetTotalSkillPoints();
		m_bLevel = level;
	}
	else if (bLevelUp)
	{
		// On each level up, we should give 3 stat points for levels 1-60.
		// For each level above that, we give an additional 2 stat points (so 5 stat points per level).
		int levelsAfter60 = (level > 60 ? level - 60 : 0);
		if ((m_sPoints + GetStatTotal()) < int32(297 + (3 * level) + (2 * levelsAfter60)))
			m_sPoints += (levelsAfter60 == 0 ? 3 : 5);

		if (level >= 10 && GetTotalSkillPoints() < 2 * (level - 9))
			m_bstrSkill[SkillPointFree] += 2;
		
		AchieveNormalCountAdd(UserAchieveNormalTypes::AchieveReachLevel, 0, nullptr);
	}

	m_iMaxExp = g_pMain->GetExpByLevel(level, GetRebirthLevel());

	SetUserAbility();
	m_sMp = m_MaxMp;
	HpChange(GetMaxHealth());

	if (m_FlashExpBonus > 0 
		|| m_FlashDcBonus > 0 
		|| m_FlashWarBonus > 0)
	{
		SendFlashDCNotice(true);
		SendFlashEXPNotice(true);
		SendFlashCountNotice(true);

		if (m_FlashExpBonus > 0)
			m_FlashExpBonus = 0;

		if (m_FlashDcBonus > 0)
			m_FlashDcBonus = 0;

		if (m_FlashWarBonus > 0)
			m_FlashWarBonus = 0;

		CMagicProcess::RemoveType4Buff(BUFF_TYPE_FISHING, this, true);
	}

	Packet result(WIZ_LEVEL_CHANGE);
	result << GetSocketID()
		<< GetLevel() 
		<< m_sPoints 
		<< m_bstrSkill[SkillPointFree]
		<< m_iMaxExp 
		<< m_iExp
		<< m_MaxHp 
		<< m_sHp
		<< m_MaxMp 
		<< m_sMp
		<< m_sMaxWeight 
		<< m_sItemWeight;

	g_pMain->Send_Region(&result, GetMap(), GetRegionX(), GetRegionZ(), nullptr, GetEventRoom());
	if (isInParty())
	{
		// TODO: Move this to party specific code
		result.Initialize(WIZ_PARTY);
		result << uint8(PARTY_LEVELCHANGE) << GetSocketID() << GetLevel();
		g_pMain->Send_PartyMember(GetPartyID(), &result);

		if (m_bIsChicken)
			GrantChickenManner();
	}

	// We should kick players out of the zone if their level no longer matches the requirements for this zone.
	if (GetLevel() < GetMap()->GetMinLevelReq() || GetLevel() > GetMap()->GetMaxLevelReq())
		KickOutZoneUser();
}

#pragma region CUser::ExpSealHandler(Packet & pkt)
void CUser::ExpSealHandler(Packet & pkt)
{
	uint8 bOpcode;

	pkt >> bOpcode;
	
	if (bOpcode == 1)
		ExpSealUpdateStatus(1);
	else if (bOpcode == 2)
		ExpSealUpdateStatus(0);
	else if (bOpcode == 3)
		ExpSealSealedPotion(pkt);
}
#pragma endregion

#pragma region CUser::ExpSealUpdateStatus(uint8 status)
void CUser::ExpSealUpdateStatus(uint8 status)
{
	Packet result(WIZ_EXP_SEAL);
	bExpSealStatus = status;
	result << uint8(status == 1 ? 1 : 2)
		<< uint8(1);
	Send(&result);
}
#pragma endregion

#pragma region CUser::ExpSealSealedPotion(Packet & pkt)
#define SEALED_JAR		810354000
#define SEALED_JAR_500M 810355000
#define SEALED_JAR_1B	810356000
#define SEALED_JAR_100M 810357000

void CUser::ExpSealSealedPotion(Packet & pkt)
{
	uint32 JarItemId;
	uint64 SealedExp;
	uint8 bPos;
	pkt >> JarItemId >> bPos >> SealedExp;

	Packet result(WIZ_EXP_SEAL, uint8(0x03));

	if (JarItemId != SEALED_JAR)
		goto fail_return;

	_ITEM_DATA* pItem = GetItem(bPos + SLOT_MAX);

	if (pItem == nullptr || pItem->nNum != JarItemId)
		goto fail_return;

	if (SealedExp > m_iSealedExp)
		goto fail_return;

	memset(pItem, 0x00, sizeof(pItem));
	SendStackChange(0, 0, 0, bPos + SLOT_MAX, true, 0, 0);

	if (SealedExp == 100000000)
		JarItemId = SEALED_JAR_100M;
	else if (SealedExp == 500000000)
		JarItemId = SEALED_JAR_500M;
	else if (SealedExp == 1000000000)
		JarItemId = SEALED_JAR_1B;
	else
		goto fail_return;

	_ITEM_TABLE* pTItem = g_pMain->GetItemPtr(JarItemId);

	if (pTItem == nullptr)
		goto fail_return;

	uint8 pos = GiveItem(JarItemId, 1, true);
	m_iSealedExp -= (uint32) SealedExp;
	ExpSealChangeExp(0);

	result << JarItemId << pos << uint32(SealedExp) << uint32(0);
fail_return:
	Send(&result);
}
#pragma endregion

#pragma region CUser::ExpSealChangeExp(uint64 amount)
void CUser::ExpSealChangeExp(uint64 amount)
{
	if (m_iSealedExp >= 1000000000 && amount > 0)
	{
		ExpSealUpdateStatus(0);
		ExpChange(amount);
		return;
	}

	m_iSealedExp += (uint32) amount;
	if (m_iSealedExp >= 1000000000)
		m_iSealedExp = 1000000000;
	else if (m_iSealedExp <= 0)
		m_iSealedExp = 0;


	Packet result(WIZ_EXP_SEAL, uint8(0x04));
	result << m_iSealedExp << uint32(0);
	Send(&result);

	result.clear();
	result.Initialize(WIZ_EXP_CHANGE);
	result << uint8(1) << m_iExp; // NOTE: Use proper flag
	Send(&result);
}
#pragma endregion

void CUser::ExpFlash()
{
	if (GetLevel() >= g_pMain->m_byMaxLevel
		&& m_iExp >= m_iMaxExp)
		return;

	if (GetPremium() == Royal_Premium && m_FlashExpBonus < 100)
	{
		SendFlashEXPNotice(true);
		m_FlashExpBonus += 10;

		if (m_FlashExpBonus > 100)
			m_FlashExpBonus = 100;

		SendFlashEXPNotice();
	}

	if (GetPremium() == EXP_Premium && m_FlashExpBonus < 100)
	{
		SendFlashNotice(true);

		m_FlashWarBonus = 0;
		m_FlashDcBonus = 0;
		m_FlashExpBonus += 10;
		if (m_FlashExpBonus > 100)
			m_FlashExpBonus = 100;

		SendFlashNotice();
	}
}

void CUser::DcFlash()
{
	if (GetPremium() == Royal_Premium && m_FlashDcBonus < 100)
	{
		SendFlashDCNotice(true);
		m_FlashDcBonus += 10;

		if (m_FlashDcBonus > 100)
			m_FlashDcBonus = 100;

		SendFlashDCNotice();
	}

	if (GetPremium() == DISC_Premium && m_FlashDcBonus < 100)
	{
		SendFlashNotice(true);
		m_FlashExpBonus = 0;
		m_FlashWarBonus = 0;
		m_FlashDcBonus += 10;
		if (m_FlashDcBonus > 100)
			m_FlashDcBonus = 100;

		SendFlashNotice();
	}
}

void CUser::WarFlash()
{
	if (GetPremium() == Royal_Premium && m_FlashWarBonus < 10)
	{
		SendFlashCountNotice(true);
		m_FlashWarBonus += 1;

		if (m_FlashWarBonus > 10)
			m_FlashWarBonus = 10;

		SendFlashCountNotice();
	}

	if (GetPremium() == WAR_Premium && m_FlashWarBonus < 10)
	{
		SendFlashNotice(true);
		m_FlashExpBonus = 0;
		m_FlashDcBonus = 0;
		m_FlashWarBonus += 1;
		if (m_FlashWarBonus > 10)
			m_FlashWarBonus = 10;

		SendFlashNotice();
	}
}

void CUser::SendFlashNotice(bool isRemove)
{
	string header;

	if (m_FlashExpBonus > 0)
		header = string_format("Exp +%d%%", m_FlashExpBonus);
	else if (m_FlashDcBonus > 0)
		header = string_format("Item Drop +%d%%", m_FlashDcBonus);
	else if (m_FlashWarBonus > 0)
		header = string_format("Cont +%d%%", m_FlashWarBonus);
	else
		return;

	Packet result(WIZ_NOTICE);
	result.DByte();
	result << uint8(4) << uint8(isRemove == true ? 2 : 1) << header << header;
	Send(&result);
}

void CUser::SendFlashDCNotice(bool isRemove)
{
	string header;

	if (m_FlashDcBonus > 0)
		header = string_format("Item Drop +%d%%", m_FlashDcBonus);
	else
		return;

	Packet result(WIZ_NOTICE);
	result.DByte();
	result << uint8(4) << uint8(isRemove == true ? 2 : 1) << header << header;
	Send(&result);
}

void CUser::SendFlashEXPNotice(bool isRemove)
{
	string header;

	if (m_FlashExpBonus > 0)
		header = string_format("Exp +%d%%", m_FlashExpBonus);
	else
		return;

	Packet result(WIZ_NOTICE);
	result.DByte();
	result << uint8(4) << uint8(isRemove == true ? 2 : 1) << header << header;
	Send(&result);
}

void CUser::SendFlashCountNotice(bool isRemove)
{
	string header;

	if (m_FlashWarBonus > 0)
		header = string_format("Cont +%d%%", m_FlashWarBonus);
	else
		return;

	Packet result(WIZ_NOTICE);
	result.DByte();
	result << uint8(4) << uint8(isRemove == true ? 2 : 1) << header << header;
	Send(&result);
}

void CUser::JackPotNoah(uint32 gold)
{
	if (ITEM_GOLD)
	{
		Packet result(WIZ_GOLD_CHANGE, uint8(CoinEvent));

		uint32 m_bGoldEvent = 0;

		int randomsay = 0;

		randomsay = rand() % 2000 + 1;

		if (randomsay > 0 && randomsay < 1100)
			randomsay = 2;
		else if (randomsay >= 1100 && randomsay < 1300)
			randomsay = 10;
		else if (randomsay >= 1300 && randomsay < 1500)
			randomsay = 50;
		else if (randomsay >= 1500 && randomsay < 1900)
			randomsay = 100;
		else if (randomsay >= 1900 && randomsay < 1950)
			randomsay = 500;
		else if (randomsay >= 1950 && randomsay < 2000)
			randomsay = 1000;

		m_bGoldEvent = gold * randomsay;

		result.SByte();
		result << uint16(740) << uint16(0) << uint32(0) << uint16(randomsay) << GetID();
		SendToRegion(&result);

		GoldGain(m_bGoldEvent, false, true);
	}
}

void CUser::JackPotExp(int64 iExp)
{
	uint64 m_bExpEvent = 0;

	if (GetLevel() >= g_pMain->m_byMaxLevel && m_iExp >= m_iMaxExp)
		return;

	Packet result(WIZ_EXP_CHANGE, uint8(2));

	int randomsay = 0;

	randomsay = rand() % 2000 + 1;

	if (randomsay > 0 && randomsay < 1100)
		randomsay = 2;
	else if (randomsay >= 1100 && randomsay < 1300)
		randomsay = 10;
	else if (randomsay >= 1300 && randomsay < 1500)
		randomsay = 50;
	else if (randomsay >= 1500 && randomsay < 1900)
		randomsay = 100;
	else if (randomsay >= 1900 && randomsay < 1950)
		randomsay = 500;
	else if (randomsay >= 1950 && randomsay < 2000)
		randomsay = 1000;

	m_bExpEvent = iExp * randomsay;

	result.SByte();
	result << GetID() << randomsay << m_bExpEvent;
	SendToRegion(&result);

	ExpChange(m_bExpEvent);
}