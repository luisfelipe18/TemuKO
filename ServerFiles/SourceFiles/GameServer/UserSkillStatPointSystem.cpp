#include "stdafx.h"

void CUser::SkillPointChange(Packet & pkt)
{
	uint8 type = pkt.read<uint8>();
	Packet result(WIZ_SKILLPT_CHANGE, type);
	// invalid type
	if (type < SkillPointCat1 || type > SkillPointMaster
		// not enough free skill points to allocate
		|| m_bstrSkill[0] < 1
		// restrict skill points per category to your level
		|| m_bstrSkill[type] + 1 > GetLevel()
		// we need our first job change to assign skill points
		|| (GetClass() % 100) <= 4
		// to set points in the mastery category, we need to be mastered.
		|| (type == SkillPointMaster && (!isMastered()
				// force a limit of MAX_LEVEL - 60 (the level you can do the mastery quest)
				// on the master skill category, so the limit's 23 skill points with a level 83 cap.
				|| m_bstrSkill[type] >= (g_pMain->m_byMaxLevel - 60)
				// allow only 1 point in the master category for every level above 60.
				|| m_bstrSkill[type] >= (GetLevel() - 60))))

	{
		result << m_bstrSkill[type]; // only send the packet on failure
		Send(&result);
		return;
	}

	--m_bstrSkill[0];
	++m_bstrSkill[type];
}

// Dialog
void CUser::SendStatSkillDistribute()
{
	Packet result(WIZ_CLASS_CHANGE, uint8(CLASS_CHANGE_REQ));
	Send(&result);
}

#pragma region CUser::AllSkillPointChange
void CUser::AllSkillPointChange(bool bIsFree)
{
	Packet result(WIZ_CLASS_CHANGE, uint8(ALL_SKILLPT_CHANGE));
	int index = 0, skill_point = 0, money = 0, temp_value = 0, old_money = 0;
	uint8 type = 0;

	temp_value = (int)pow((GetLevel() * 2.0f), 3.4f);

	if (GetLevel() < 30)
		temp_value = (int)(temp_value * 0.4f);
	else if (GetLevel() >= 60)
		temp_value = (int)(temp_value * 1.5f);

	temp_value = (int)(temp_value * 1.5f);

	// Level too low.
	if (GetLevel() < 10)
		goto fail_return;

	if (GetPremium() == WAR_Premium
		|| GetPremium() == Royal_Premium
		|| CheckExistItem(RETURNTOKENS))
		temp_value = 0;

	// Get total skill points
	for (int i = 1; i < 9; i++)
		skill_point += m_bstrSkill[i];

	// If we don't have any skill points, there's no point resetting now is there.
	if (skill_point <= 0)
	{
		type = 2;
		goto fail_return;
	}

	// Not enough money.
	if (!bIsFree & !GoldLose(temp_value, false))
		goto fail_return;

	// Reset skill points.
	m_bstrSkill[0] = (GetLevel() - 9) * 2;
	for (int i = 1; i < 9; i++)
		m_bstrSkill[i] = 0;

	//RobItem(700001000, 1);

	result << uint8(1) << GetCoins() << m_bstrSkill[0];
	Send(&result);
	return;

fail_return:
	result << type << temp_value;
	Send(&result);
}
#pragma endregion

#pragma region CUser::AllPointChange
void CUser::AllPointChange(bool bIsFree)
{
	Packet result(WIZ_CLASS_CHANGE, uint8(ALL_POINT_CHANGE));
	int temp_money;
	uint16 statTotal;

	uint16 byStr, bySta, byDex, byInt, byCha;
	uint8 bResult = 0;

	if (GetLevel() > g_pMain->m_byMaxLevel)
		goto fail_return;

	temp_money = (int)pow((GetLevel() * 2.0f), 3.4f);

	if (GetLevel() < 30)
		temp_money = (int)(temp_money * 0.4f);
	else if (GetLevel() >= 60)
		temp_money = (int)(temp_money * 1.5f);

	if (GetPremium() == WAR_Premium
		|| GetPremium() == Royal_Premium
		|| CheckExistItem(RETURNTOKENS))
		temp_money = 0;

	for (int i = 0; i < SLOT_MAX; i++)
	{
		if (m_sItemArray[i].nNum) {
			bResult = 4;
			goto fail_return;
		}
	}

	// It's 300-10 for clarity (the 10 being the stat points assigned on char creation)
	if (GetStatTotal() == 290)
	{
		bResult = 2; // don't need to reallocate stats, it has been done already...
		goto fail_return;
	}

	// Not enough coins
	if (!bIsFree & !GoldLose(temp_money, false))
		goto fail_return;

	// TODO: Pull this from the database.
	switch (m_bRace)
	{

	case KARUS_BIG:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
	case KURIAN:
		if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case KARUS_MIDDLE:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case KARUS_SMALL:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case KARUS_WOMAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case BABARIAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case ELMORAD_MAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case ELMORAD_WOMAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case PORUTU:
		if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;

	}

	// Players gain 3 stats points for each level up to and including 60.
	// They also received 10 free stat points on creation. 
	m_sPoints = 10 + (GetLevel() - 1) * 3;

	// For every level after 60, we add an additional two points.
	if (GetLevel() > 60)
		m_sPoints += 2 * (GetLevel() - 60);

	statTotal = GetStatTotal();
	ASSERT(statTotal == 290);

	SetUserAbility();

	byStr = GetStat(STAT_STR);
	bySta = GetStat(STAT_STA),
	byDex = GetStat(STAT_DEX);
	byInt = GetStat(STAT_INT),
	byCha = GetStat(STAT_CHA);

	result << uint8(1) // result (success)
		<< GetCoins()
		<< byStr << bySta << byDex << byInt << byCha
		<< m_MaxHp << m_MaxMp << m_sTotalHit << m_sMaxWeight << m_sPoints;
	Send(&result);
	return;

fail_return:
	result << bResult << temp_money;
	Send(&result);
}
#pragma endregion

#pragma region CUser::AllSkillPointChange2
void CUser::AllSkillPointChange2(bool bIsFree)
{
	Packet result(WIZ_CLASS_CHANGE, uint8(ALL_SKILLPT_CHANGE));
	int index = 0, skill_point = 0, money = 0, temp_value = 0, old_money = 0;
	uint8 type = 0;

	// Level too low.
	if (GetLevel() < 10)
		goto fail_return;

	if (GetPremium() == WAR_Premium
		|| GetPremium() == Royal_Premium)
		temp_value = 0;

	// Get total skill points
	for (int i = 1; i < 9; i++)
		skill_point += m_bstrSkill[i];

	// If we don't have any skill points, there's no point resetting now is there.
	if (skill_point <= 0)
	{
		type = 2;
		goto fail_return;
	}
	// Reset skill points.
	m_bstrSkill[0] = (GetLevel() - 9) * 2;
	for (int i = 1; i < 9; i++)
		m_bstrSkill[i] = 0;

	RobItem(700001000, 1);

	result << uint8(1) << GetCoins() << m_bstrSkill[0];
	Send(&result);
	return;

fail_return:
	result << type << temp_value;
	Send(&result);
}
#pragma endregion

#pragma region CUser::AllPointChange2
void CUser::AllPointChange2(bool bIsFree)
{
	Packet result(WIZ_CLASS_CHANGE, uint8(ALL_POINT_CHANGE));
	int temp_money = 0;
	uint16 statTotal;

	uint16 byStr, bySta, byDex, byInt, byCha;
	uint8 bResult = 0;

	if (GetLevel() > g_pMain->m_byMaxLevel)
		goto fail_return;

	for (int i = 0; i < SLOT_MAX; i++)
	{
		if (m_sItemArray[i].nNum) {
			bResult = 4;
			goto fail_return;
		}
	}

	// It's 300-10 for clarity (the 10 being the stat points assigned on char creation)
	if (GetStatTotal() == 290)
	{
		bResult = 2; // don't need to reallocate stats, it has been done already...
		goto fail_return;
	}

	// TODO: Pull this from the database.
	switch (m_bRace)
	{

	case KARUS_BIG:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
	case KURIAN:
		if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case KARUS_MIDDLE:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case KARUS_SMALL:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case KARUS_WOMAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case BABARIAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
	case ELMORAD_MAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case ELMORAD_WOMAN:
		if (isWarrior())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isRogue())
		{
			SetStat(STAT_STR, 60);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 70);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		else if (isPriest())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isMage())
		{
			SetStat(STAT_STR, 50);
			SetStat(STAT_STA, 60);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 70);
			SetStat(STAT_CHA, 50);
		}
		else if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	case PORUTU:
		if (isPortuKurian())
		{
			SetStat(STAT_STR, 65);
			SetStat(STAT_STA, 65);
			SetStat(STAT_DEX, 60);
			SetStat(STAT_INT, 50);
			SetStat(STAT_CHA, 50);
		}
		break;
	}

	// Players gain 3 stats points for each level up to and including 60.
	// They also received 10 free stat points on creation. 
	m_sPoints = 10 + (GetLevel() - 1) * 3;

	// For every level after 60, we add an additional two points.
	if (GetLevel() > 60)
		m_sPoints += 2 * (GetLevel() - 60);

	statTotal = GetStatTotal();
	ASSERT(statTotal == 290);

	SetUserAbility();

	byStr = GetStat(STAT_STR);
	bySta = GetStat(STAT_STA),
	byDex = GetStat(STAT_DEX);
	byInt = GetStat(STAT_INT),
	byCha = GetStat(STAT_CHA);


	RobItem(700001000, 1);

	result << uint8(1) // result (success)
		<< GetCoins()
		<< byStr << bySta << byDex << byInt << byCha
		<< m_MaxHp << m_MaxMp << m_sTotalHit << m_sMaxWeight << m_sPoints;
	Send(&result);
	return;

fail_return:
	result << bResult << temp_money;
	Send(&result);
}
#pragma endregion

void CUser::ClassChangeReq()
{
	Packet result(WIZ_CLASS_CHANGE, uint8(CLASS_CHANGE_RESULT));
	if (GetLevel() < 10) // if we haven't got our first job change
		result << uint8(2);
	else if ((m_sClass % 100) > 4)// if we've already got our job change
	{ 
		if ((isPortuKurian()) && (m_sClass % 100) == 13)
			result << uint8(1);
		else
			result << uint8(3);
	}
	else // otherwise
		result << uint8(1);
	Send(&result);
}

bool CUser::CheckSkillPoint(uint8 skillnum, uint8 min, uint8 max)
{
	if (skillnum < 5 || skillnum > 8)
		return false;

	return (m_bstrSkill[skillnum] >= min && m_bstrSkill[skillnum] <= max);
}

bool CUser::CheckClass(short class1, short class2, short class3, short class4, short class5, short class6)
{
	return (JobGroupCheck(class1) || JobGroupCheck(class2) || JobGroupCheck(class3) || JobGroupCheck(class4) || JobGroupCheck(class5) || JobGroupCheck(class6));
}

bool CUser::JobGroupCheck(short jobgroupid)
{
	if (jobgroupid > 100)
		return GetClass() == jobgroupid;

	ClassType subClass = GetBaseClassType();
	switch (jobgroupid)
	{
	case GROUP_WARRIOR:
		return (subClass == ClassWarrior || subClass == ClassWarriorNovice || subClass == ClassWarriorMaster);

	case GROUP_ROGUE:
		return (subClass == ClassRogue || subClass == ClassRogueNovice || subClass == ClassRogueMaster);

	case GROUP_MAGE:
		return (subClass == ClassMage || subClass == ClassMageNovice || subClass == ClassMageMaster);

	case GROUP_CLERIC:
		return (subClass == ClassPriest || subClass == ClassPriestNovice || subClass == ClassPriestMaster);

	case GROUP_PORTU_KURIAN:
		return (subClass == ClassPortuKurian || subClass == ClassPortuKurianNovice || subClass == ClassPortuKurianMaster);
	}

	return (subClass == jobgroupid);
}

/**
* @brief	Handles player stat assignment.
*
* @param	pkt	The packet.
*/
void CUser::PointChange(Packet & pkt)
{
	uint8 type = pkt.read<uint8>();
	StatType statType = (StatType)(type - 1);

	if (statType < STAT_STR || statType >= STAT_COUNT
		|| m_sPoints < 1
		|| GetStat(statType) >= STAT_MAX)
		return;

	Packet result(WIZ_POINT_CHANGE, type);

	m_sPoints--; // remove a free point
	result << uint16(++m_bStats[statType]); // assign the free point to a stat
	SetUserAbility();
	result << m_MaxHp << m_MaxMp << m_sTotalHit << m_sMaxWeight;
	Send(&result);
	SendItemMove(1, 1);
}

#pragma region CUser::GetBaseClass()
uint8 CUser::GetBaseClass()
{
	switch (GetBaseClassType())
	{
	case ClassWarrior:
	case ClassWarriorNovice:
	case ClassWarriorMaster:
		return GROUP_WARRIOR;

	case ClassRogue:
	case ClassRogueNovice:
	case ClassRogueMaster:
		return GROUP_ROGUE;

	case ClassMage:
	case ClassMageNovice:
	case ClassMageMaster:
		return GROUP_MAGE;

	case ClassPriest:
	case ClassPriestNovice:
	case ClassPriestMaster:
		return GROUP_CLERIC;

	case ClassPortuKurian:
	case ClassPortuKurianNovice:
	case ClassPortuKurianMaster:
		return GROUP_PORTU_KURIAN;
	}

	return 0;
}

#pragma endregion 