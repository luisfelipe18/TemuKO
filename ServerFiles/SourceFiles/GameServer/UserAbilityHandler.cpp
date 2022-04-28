#include "StdAfx.h"

float CUser::SetCoefficient()
{
	_CLASS_COEFFICIENT* pCoefficient = g_pMain->m_CoefficientArray.GetData(GetClass());
	if (pCoefficient == nullptr)
		return 0.0f;

	_ITEM_TABLE* pRightHand = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND);
	if (pRightHand != nullptr)
	{
		switch (pRightHand->m_bKind)
		{
		case WEAPON_KIND_DAGGER:
			return pCoefficient->ShortSword;
			break;
		case WEAPON_KIND_1H_SWORD:
		case WEAPON_KIND_2H_SWORD:
			return pCoefficient->Sword;
			break;
		case WEAPON_KIND_1H_AXE:
		case WEAPON_KIND_2H_AXE:
			return pCoefficient->Axe;
			break;
		case WEAPON_KIND_1H_CLUP:
		case WEAPON_KIND_2H_CLUP:
			return pCoefficient->Club;
			break;
		case WEAPON_KIND_1H_SPEAR:
		case WEAPON_KIND_2H_SPEAR:
			return pCoefficient->Spear;
			break;
		case WEAPON_KIND_BOW:
		case WEAPON_KIND_CROSSBOW:
			return pCoefficient->Bow;
			break;
		case WEAPON_KIND_STAFF:
			return pCoefficient->Staff;
			break;
		case WEAPON_KIND_JAMADHAR:
			return pCoefficient->Jamadar;
			break;
		case WEAPON_KIND_MACE:
			return pCoefficient->Pole;
			break;
		}
	}
	else
	{
		_ITEM_TABLE* pLeftHand = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND);
		if (pLeftHand != nullptr)
		{
			switch (pLeftHand->m_bKind)
			{
			case WEAPON_KIND_DAGGER:
				return pCoefficient->ShortSword;
				break;
			case WEAPON_KIND_1H_SWORD:
			case WEAPON_KIND_2H_SWORD:
				return pCoefficient->Sword;
				break;
			case WEAPON_KIND_1H_AXE:
			case WEAPON_KIND_2H_AXE:
				return pCoefficient->Axe;
				break;
			case WEAPON_KIND_1H_CLUP:
			case WEAPON_KIND_2H_CLUP:
				return pCoefficient->Club;
				break;
			case WEAPON_KIND_1H_SPEAR:
			case WEAPON_KIND_2H_SPEAR:
				return pCoefficient->Spear;
				break;
			case WEAPON_KIND_BOW:
			case WEAPON_KIND_CROSSBOW:
				return pCoefficient->Bow;
				break;
			case WEAPON_KIND_STAFF:
				return pCoefficient->Staff;
				break;
			case WEAPON_KIND_JAMADHAR:
				return pCoefficient->Jamadar;
				break;
			case WEAPON_KIND_MACE:
				return pCoefficient->Pole;
				break;
			}
		}
	}
	return 0.0f;
}

void CUser::SetUserAbility(bool bSendPacket)
{
	_CLASS_COEFFICIENT* pCoefficient = g_pMain->m_CoefficientArray.GetData(GetClass());
	if (pCoefficient == nullptr)
		return;

	float Coefficient = SetCoefficient();

	uint16 righthandattackpower = 0, lefthandattackpower = 0;

	_ITEM_TABLE* pRightHand = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND);
	if (pRightHand != nullptr)
	{
		_ITEM_DATA* pRightData = nullptr;
		if (GetZoneID() == ZONE_KNIGHT_ROYALE)
			pRightData = GetKnightRoyaleItem(KNIGHT_ROYAL_RIGHTHAND);
		else
			pRightData = GetItem(RIGHTHAND);

		if (pRightData != nullptr)
		{
			if (pRightData->sDuration == 0)
				righthandattackpower += (pRightHand->m_sDamage + m_bAddWeaponDamage) / 2;
			else
				righthandattackpower += pRightHand->m_sDamage + m_bAddWeaponDamage;
		}
	}

	_ITEM_TABLE* pLeftHand = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND);
	if (pLeftHand != nullptr)
	{
		_ITEM_DATA* pLeftData = nullptr;
		if (GetZoneID() == ZONE_KNIGHT_ROYALE)
			pLeftData = GetKnightRoyaleItem(KNIGHT_ROYAL_LEFTHAND);
		else
			pLeftData = GetItem(LEFTHAND);

		if (pLeftData != nullptr)
		{
			if (pLeftHand->isBow())
			{
				if (pLeftData->sDuration == 0)
					lefthandattackpower += (pLeftHand->m_sDamage + m_bAddWeaponDamage) / 2;
				else
					lefthandattackpower += pLeftHand->m_sDamage + m_bAddWeaponDamage;
			}
			else
			{
				if (pLeftData->sDuration == 0)
					lefthandattackpower += ((pLeftHand->m_sDamage + m_bAddWeaponDamage) / 2) / 2;
				else
					lefthandattackpower += (pLeftHand->m_sDamage + m_bAddWeaponDamage) / 2;
			}
		}
	}

	if (righthandattackpower < 0) righthandattackpower = 0;
	if (lefthandattackpower < 0) lefthandattackpower = 0;

	uint16 Totalattackpower = righthandattackpower + lefthandattackpower;

	if (Totalattackpower < 3)
		Totalattackpower = 3;

	// Update stats based on item data
	SetSlotItemValue();

	int mainstr = GetStat(STAT_STR), maindex = GetStat(STAT_DEX);

	uint32 BaseAp = 0, ApStat = 0;

	if (mainstr > 150)
		BaseAp = mainstr - 150;

	if (mainstr == 160)
		BaseAp--;

	int totalstr = mainstr + GetStatBonusTotal(STAT_STR), totaldex = maindex + GetStatBonusTotal(STAT_DEX);

	uint32 tempMaxWeight = m_sMaxWeight;
	m_sMaxWeight = ((((GetStatWithItemBonus(STAT_STR) + GetLevel()) * 50) + m_sMaxWeightBonus) * (m_bMaxWeightAmount <= 0 ? 1 : m_bMaxWeightAmount / 100)) / 2;

	m_sTotalHit = 0;

	float BonusAp = 1.0;
	BonusAp = (m_byAPBonusAmount + 100) / 100.0f;

	uint16 AchieveAttackBonus = m_sStatAchieveBonuses[ACHIEVE_STAT_ATTACK];

	if (isRogue())
		m_sTotalHit = (uint16)(((0.005f * Totalattackpower * (totaldex + 40)) + (Coefficient * Totalattackpower * GetLevel() * totaldex) + 3) * BonusAp);
	else
		m_sTotalHit = (uint16)(((0.005f * Totalattackpower * (totalstr + 40)) + (Coefficient * Totalattackpower * GetLevel() * totalstr) + 3) * BonusAp) + BaseAp;

	m_sTotalHit += AchieveAttackBonus;

	if (m_sACAmount < 0) m_sACAmount = 0;

	m_sTotalAc = 0;
	m_sTotalAc = (short)(pCoefficient->AC * (GetLevel() + m_sItemAc));

	if (m_sACPercent <= 0) m_sACPercent = 100;

	uint8 bDefenseBonus = 0, bResistanceBonus = 0;

	// Reset resistance bonus
	m_bResistanceBonus = 0;

	if (isWarrior())
	{
		if (isNoviceWarrior())
		{
			//Hinder: [Passive]Increase defense by 10%. If a shield is not equipped, the effect will decrase by half.
			if (CheckSkillPoint(PRO_SKILL2, 5, 14))
				bDefenseBonus = 5;
			//Arrest: Passive]Increase defense by 15 % .If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 15, 34))
				bDefenseBonus = 8;
			//Bulwark: [Passive]Increase defense by 20%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 35, 54))
				bDefenseBonus = 10;
			//Evading: [Passive]Increase defense by 25%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 55, 69))
				bDefenseBonus = 13;

			// Resist: [Passive]Increase all resistance by 30. If a shield is not equipped, the effect will decrease by half.
			if (CheckSkillPoint(PRO_SKILL2, 10, 19))
				bResistanceBonus = 15;
			// Endure: [Passive]Increase all resistance by 60. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 20, 39))
				bResistanceBonus = 30;
			//Immunýty: [Passive]Increase all resistance by 90. If a shield is not equipped, the effect will decrase by half.
			else if (CheckSkillPoint(PRO_SKILL2, 40, 83))
				bResistanceBonus = 45;

			m_bResistanceBonus += bResistanceBonus;
			m_sTotalAc += bDefenseBonus * m_sTotalAc / 100;
		}
		else if (isMasteredWarrior())
		{
			//Hinder: [Passive]Increase defense by 10%. If a shield is not equipped, the effect will decrase by half.
			if (CheckSkillPoint(PRO_SKILL2, 5, 14))
				bDefenseBonus = 20;
			//Arrest: Passive]Increase defense by 15 % .If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 15, 34))
				bDefenseBonus = 34;
			//Bulwark: [Passive]Increase defense by 20%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 35, 54))
				bDefenseBonus = 40;
			//Evading: [Passive]Increase defense by 25%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 55, 69))
				bDefenseBonus = 50;
			// Iron Skin: [Passive]Increase defense by 30%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 70, 79))
				bDefenseBonus = 60;
			//Iron Body: [Passive]Increase defense by 40%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 80, 83))
				bDefenseBonus = 80;

			// Resist: [Passive]Increase all resistance by 30. If a shield is not equipped, the effect will decrease by half.
			if (CheckSkillPoint(PRO_SKILL2, 10, 19))
				bResistanceBonus = 30;
			// Endure: [Passive]Increase all resistance by 60. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 20, 39))
				bResistanceBonus = 60;
			//Immunýty: [Passive]Increase all resistance by 90. If a shield is not equipped, the effect will decrase by half.
			else if (CheckSkillPoint(PRO_SKILL2, 40, 83))
				bResistanceBonus = 90;

			_ITEM_TABLE* pLeftHand = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND);
			if (pLeftHand == nullptr || !pLeftHand->isShield())
			{
				bDefenseBonus /= 2;
				bResistanceBonus /= 2;
			}

			if (bDefenseBonus < 0)
				bDefenseBonus = 0;

			if (bResistanceBonus < 0)
				bResistanceBonus = 0;

			m_bResistanceBonus += bResistanceBonus;
			m_sTotalAc += bDefenseBonus * m_sTotalAc / 100;
		}
	}
	else if (isPortuKurian())
	{
		if (isNoviceKurianPortu())
		{
			//Hinder: [Passive]Increase defense by 10%. If a shield is not equipped, the effect will decrase by half.
			if (CheckSkillPoint(PRO_SKILL2, 5, 14))
				bDefenseBonus = 5;
			//Arrest: Passive]Increase defense by 15 % .If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 15, 34))
				bDefenseBonus = 8;
			//Bulwark: [Passive]Increase defense by 20%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 35, 54))
				bDefenseBonus = 10;
			//Evading: [Passive]Increase defense by 25%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 55, 69))
				bDefenseBonus = 13;

			// Resist: [Passive]Increase all resistance by 30. If a shield is not equipped, the effect will decrease by half.
			if (CheckSkillPoint(PRO_SKILL2, 10, 19))
				bResistanceBonus = 15;
			// Endure: [Passive]Increase all resistance by 60. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 20, 39))
				bResistanceBonus = 30;
			//Immunýty: [Passive]Increase all resistance by 90. If a shield is not equipped, the effect will decrase by half.
			else if (CheckSkillPoint(PRO_SKILL2, 40, 83))
				bResistanceBonus = 45;

			m_bResistanceBonus += bResistanceBonus;
			m_sTotalAc += bDefenseBonus * m_sTotalAc / 100;
		}
		else if (isMasteredKurianPortu())
		{
			//Hinder: [Passive]Increase defense by 10%. If a shield is not equipped, the effect will decrase by half.
			if (CheckSkillPoint(PRO_SKILL2, 5, 14))
				bDefenseBonus = 5;
			//Arrest: Passive]Increase defense by 15 % .If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 15, 34))
				bDefenseBonus = 8;
			//Bulwark: [Passive]Increase defense by 20%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 35, 54))
				bDefenseBonus = 10;
			//Evading: [Passive]Increase defense by 25%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 55, 69))
				bDefenseBonus = 13;
			// Iron Skin: [Passive]Increase defense by 30%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 70, 79))
				bDefenseBonus = 15;
			//Iron Body: [Passive]Increase defense by 40%. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 80, 83))
				bDefenseBonus = 20;

			// Resist: [Passive]Increase all resistance by 30. If a shield is not equipped, the effect will decrease by half.
			if (CheckSkillPoint(PRO_SKILL2, 10, 19))
				bResistanceBonus = 15;
			// Endure: [Passive]Increase all resistance by 60. If a shield is not equipped, the effect will decrease by half.
			else if (CheckSkillPoint(PRO_SKILL2, 20, 39))
				bResistanceBonus = 30;
			//Immunýty: [Passive]Increase all resistance by 90. If a shield is not equipped, the effect will decrase by half.
			else if (CheckSkillPoint(PRO_SKILL2, 40, 83))
				bResistanceBonus = 45;

			if (bDefenseBonus < 0)
				bDefenseBonus = 0;

			if (bResistanceBonus < 0)
				bResistanceBonus = 0;

			m_bResistanceBonus += bResistanceBonus;
			m_sTotalAc += bDefenseBonus * m_sTotalAc / 100;
		}
	}

	if (isMasteredPriest() 
		|| isMasteredWarrior())
	{
		// Boldness/Daring [Passive]Increase your defense by 20% when your HP is down to 30% or lower.
		if (m_sHp < 30 * m_MaxHp / 100)
			m_sTotalAc += 20 * m_sTotalAc / 100;
	}
	else if (isMasteredRogue())
	{
		// Valor: [Passive]Increase your resistance by 50 when your HP is down to 30% or below.
		if (m_sHp < 30 * m_MaxHp / 100)
			m_bResistanceBonus += 50;
	}
	else if (isMasteredKurianPortu())
	{
		//Axid Break: [Passive]When HP less than 30% Attack increases by 20%.
		if (CheckSkillPoint(PRO_SKILL4, 15, 23))
		{
			if (m_sHp < 30 * m_MaxHp / 100)
				m_sTotalHit += 20 * m_sTotalHit / 100;
		}
	}

	if (m_bAddWeaponDamage > 0)
		++m_sTotalHit;

	if (m_sAddArmourAc > 0 || m_bPctArmourAc > 100)
		++m_sTotalAc;

	uint8 bSta = GetStat(STAT_STA);
	if (bSta > 100)
		m_sTotalAc += bSta - 100;

	uint8 bInt = GetStat(STAT_INT);
	if (bInt > 100)
		m_bResistanceBonus += (bInt - 100) / 2;

	if (m_sACPercent <= 0) m_sACPercent = 100;

	m_sTotalAc += m_sStatAchieveBonuses[ACHIEVE_STAT_DEFENCE];
	m_sTotalAc = m_sTotalAc * m_sACPercent / 100;

	m_fTotalHitrate = ((1 + pCoefficient->Hitrate * GetLevel() * totaldex) * m_sItemHitrate / 100) * (m_bHitRateAmount / 100);
	m_fTotalEvasionrate = ((1 + pCoefficient->Evasionrate * GetLevel() * totaldex) * m_sItemEvasionrate / 100) * (m_sAvoidRateAmount / 100);

	SetMaxHp();
	SetMaxMp();

	if (isPortuKurian())
		SetMaxSp();

	// TODO: Transformation stats need to be applied here
	if (GetZoneID() == ZONE_DELOS && isSiegeTransformation())
	{
		_MAGIC_TYPE6* pType = g_pMain->m_Magictype6Array.GetData(m_sTransformSkillID);
		if (pType != nullptr)
		{
			m_sTotalHit = pType->sTotalHit;
			m_sTotalAc = pType->sTotalAc;
			m_MaxHp = pType->sMaxHp;
			m_MaxMp = pType->sMaxMp;
			m_sSpeed = pType->bSpeed;
			m_sFireR = pType->sTotalFireR;
			m_sColdR = pType->sTotalColdR;
			m_sLightningR = pType->sTotalLightningR;
			m_sMagicR = pType->sTotalMagicR;
			m_sDiseaseR = pType->sTotalDiseaseR;
			m_sPoisonR = pType->sTotalPoisonR;
		}
	}
	if (GetZoneID() == ZONE_KNIGHT_ROYALE
		&& !isSiegeTransformation())
	{
		if (g_pMain->isKnightRoyaleActive())
		{
			_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
			if (pKnightRoyaleInfo != nullptr)
			{
				_KNIGHT_ROYALE_STARTED_USER* pSurvivor = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
				if (pSurvivor != nullptr)
				{
					_KNIGHT_ROYALE_STATS_LIST* pRoyaleStats = g_pMain->m_KnightRoayleStatsListArray.GetData(pSurvivor->KnightRoyaleLevel);
					if (pRoyaleStats != nullptr)
					{
						m_sTotalHit = pRoyaleStats->KnightRoyaleStatsTotalHit;
						m_sTotalAc = pRoyaleStats->KnightRoyaleStatsTotalAc;
						m_sFireR = pRoyaleStats->KnightRoyaleStatsFireR;
						m_sColdR = pRoyaleStats->KnightRoyaleStatsColdR;
						m_sLightningR = pRoyaleStats->KnightRoyaleStatsLightningR;
						m_sMagicR = pRoyaleStats->KnightRoyaleStatsMagicR;
						m_sDiseaseR = pRoyaleStats->KnightRoyaleStatsDiseaseR;
						m_sPoisonR = pRoyaleStats->KnightRoyaleStatsPoisonR;
						m_MaxHp = pRoyaleStats->KnightRoyaleStatiMaxHp;
						m_MaxMp = pRoyaleStats->KnightRoyaleStatiMaxMp;
					}
				}
			}
		}
	}

	if (bSendPacket)
		SendItemMove(1, 2);
}