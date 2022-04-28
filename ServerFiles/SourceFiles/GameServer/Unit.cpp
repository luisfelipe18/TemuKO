#include "stdafx.h"
#include "Map.h"
#include "GameServerDlg.h"
#include "MagicInstance.h"

#include <cfloat>

Unit::Unit(UnitType unitType)
	: m_pMap(nullptr), m_pRegion(nullptr), m_sRegionX(0), m_sRegionZ(0), m_unitType(unitType)
{
	Initialize();
}

void Unit::Initialize()
{
	m_pMap = nullptr;
	m_pRegion = nullptr;

	SetPosition(0.0f, 0.0f, 0.0f);
	m_bLevel = 0;
	m_bNation = 0;

	InitType3();
	InitType4(true);

	AbsorbedAmmount = 0;
	m_sTotalHit = 0;
	m_sTotalAc = 0;
	m_fTotalHitrate = 0.0f;
	m_fTotalEvasionrate = 0.0f;

	m_bResistanceBonus = 0;
	m_sFireR = m_sColdR = m_sLightningR = m_sMagicR = m_sDiseaseR = m_sPoisonR = 0;
	m_sDaggerR = m_sSwordR = m_sJamadarR = m_sAxeR = m_sMaceR = m_sSpearR = m_sBowR = 0;
	m_byDaggerRAmount = m_byBowRAmount = 100;

	Guard lock(m_equippedItemBonusLock);
	m_equippedItemBonuses.clear();

	m_bCanStealth = true;
	m_bReflectArmorType = 0;
	m_bIsBlinded = false;
	m_bCanUseSkills = m_bCanUsePotions = m_bCanTeleport = m_bGearSkills = true;
	m_bInstantCast = false;
	m_bIsUndead = m_bIsKaul = m_bIsDevil = m_bDamageAC = false;

	m_bBlockPhysical = m_bBlockMagic = false;
	m_bBlockCurses = m_bReflectCurses = false;
	m_bMirrorDamage = false;
	m_bMirrorDamageType = false;
	m_byMirrorAmount = 0;

	m_transformationType = TransformationNone;
	m_sTransformID = 0;
	m_sTransformSkillID = 0;
	m_sTransformMpchange = false;
	m_sTransformHpchange = false;
	m_tTransformationStartTime = 0;
	m_sTransformationDuration = 0;

	m_sAttackSpeedAmount = 100;		// this is for the duration spells Type 4
	m_bSpeedAmount = 100;
	m_sACAmount = 0;
	m_sACPercent = 100;
	m_bAttackAmount = 100;
	m_sMagicAttackAmount = 0;
	m_sMaxHPAmount = m_sMaxMPAmount = 0;
	m_bHitRateAmount = 100;
	m_sAvoidRateAmount = 100;
	m_bAddFireR = m_bAddColdR = m_bAddLightningR = 0;
	m_bAddMagicR = m_bAddDiseaseR = m_bAddPoisonR = 0;
	m_bPctFireR = m_bPctColdR = m_bPctLightningR = 100;
	m_bPctMagicR = m_bPctDiseaseR = m_bPctPoisonR = 100;
	m_bMagicDamageReduction = 100;
	m_bManaAbsorb = 0;
	m_bRadiusAmount = 0;
	m_buffCount = 0;

	m_oSocketID = m_oBarrackID = -1;
	m_bEventRoom = 0;
}

/*
NOTE: Due to KO messiness, we can really only calculate a 2D distance/
There are a lot of instances where the y (height level, in this case) coord isn't set,
which understandably screws things up a lot.
*/
// Calculate the distance between 2 2D points.
float Unit::GetDistance(float fx, float fz)
{
	return (float)GetDistance(GetX(), GetZ(), fx, fz);
}

// Calculate the 2D distance between Units.
float Unit::GetDistance(Unit * pTarget)
{
	if (pTarget == nullptr)
		return -FLT_MAX;

	if (GetZoneID() != pTarget->GetZoneID())
		return -FLT_MAX;

	return GetDistance(pTarget->GetX(), pTarget->GetZ());
}

float Unit::GetDistanceSqrt(Unit * pTarget)
{
	if (pTarget == nullptr)
		return -FLT_MAX;

	if (GetZoneID() != pTarget->GetZoneID())
		return -FLT_MAX;

	return sqrtf(GetDistance(pTarget->GetX(), pTarget->GetZ()));
}

// Check to see if the Unit is in 2D range of another Unit.
// Range MUST be squared already.
bool Unit::isInRange(Unit * pTarget, float fSquaredRange)
{
	return (GetDistance(pTarget) <= fSquaredRange);
}

// Check to see if we're in the 2D range of the specified coordinates.
// Range MUST be squared already.
bool Unit::isInRange(float fx, float fz, float fSquaredRange)
{
	return (GetDistance(fx, fz) <= fSquaredRange);
}

// Check to see if the Unit is in 2D range of another Unit.
// Range must NOT be squared already.
// This is less preferable to the more common precalculated range.
bool Unit::isInRangeSlow(Unit * pTarget, float fNonSquaredRange)
{
	return isInRange(pTarget, pow(fNonSquaredRange, 2.0f));
}

// Check to see if the Unit is in 2D range of the specified coordinates.
// Range must NOT be squared already.
// This is less preferable to the more common precalculated range.
bool Unit::isInRangeSlow(float fx, float fz, float fNonSquaredRange)
{
	return isInRange(fx, fz, pow(fNonSquaredRange, 2.0f));
}

float Unit::GetDistance(float fStartX, float fStartZ, float fEndX, float fEndZ)
{
	return pow(fStartX - fEndX, 2.0f) + pow(fStartZ - fEndZ, 2.0f);
}

bool Unit::isInRange(float fStartX, float fStartZ, float fEndX, float fEndZ, float fSquaredRange)
{
	return (GetDistance(fStartX, fStartZ, fEndX, fEndZ) <= fSquaredRange);
}

bool Unit::isInRangeSlow(float fStartX, float fStartZ, float fEndX, float fEndZ, float fNonSquaredRange)
{
	return isInRange(fStartX, fStartZ, fEndX, fEndZ, pow(fNonSquaredRange, 2.0f));
}

void Unit::SetRegion(uint16 x /*= -1*/, uint16 z /*= -1*/)
{
	m_sRegionX = x; m_sRegionZ = z;
	m_pRegion = m_pMap->GetRegion(x, z); // TODO: Clean this up
}

bool Unit::RegisterRegion()
{
	uint16
		new_region_x = GetNewRegionX(), new_region_z = GetNewRegionZ(),
		old_region_x = GetRegionX(), old_region_z = GetRegionZ();

	if (GetRegion() == nullptr || (old_region_x == new_region_x && old_region_z == new_region_z))
		return false;

	AddToRegion(new_region_x, new_region_z);

	RemoveRegion(old_region_x - new_region_x, old_region_z - new_region_z);
	InsertRegion(new_region_x - old_region_x, new_region_z - old_region_z);

	return true;
}

void Unit::RemoveRegion(int16 del_x, int16 del_z)
{
	if (GetMap() == nullptr)
		return;

	Packet result;
	GetInOut(result, INOUT_OUT);
	g_pMain->Send_OldRegions(&result, del_x, del_z, GetMap(), GetRegionX(), GetRegionZ(), nullptr, GetEventRoom());
}

void Unit::InsertRegion(int16 insert_x, int16 insert_z)
{
	if (GetMap() == nullptr)
		return;

	Packet result;
	GetInOut(result, INOUT_IN);
	g_pMain->Send_NewRegions(&result, insert_x, insert_z, GetMap(), GetRegionX(), GetRegionZ(), nullptr, GetEventRoom());
}

/* These should not be declared here, but it's necessary until the AI server better shares unit code */

/**
* @brief	Calculates damage for players attacking either monsters/NPCs or other players.
*
* @param	pTarget			Target unit.
* @param	pSkill			The skill used in the attack, if applicable..
* @param	bPreviewOnly	true to preview the damage only and not apply any item bonuses.
* 							Used by AI logic to determine who to target (by checking who deals the most damage).
*
* @return	The damage.
*/
short CUser::GetDamage(Unit *pTarget, _MAGIC_TABLE *pSkill /*= nullptr*/, bool bPreviewOnly /*= false*/)
{
	/*
	This seems identical to users attacking NPCs/monsters.
	The only differences are:
	- GetACDamage() is not called
	- the resulting damage is not divided by 3.
	*/
	int32 damage = 0;
	int random = 0;
	int32 temp_hit = 0, temp_ac = 0, temp_ap = 0, temp_hit_B = 0;
	uint8 result;

	if (pTarget == nullptr
		|| pTarget->isDead())
		return -1;

	// Trigger item procs
	if (pTarget->isPlayer()
		&& !bPreviewOnly)
	{
		OnAttack(pTarget, AttackTypePhysical);
		pTarget->OnDefend(this, AttackTypePhysical);
	}

	temp_ac = pTarget->m_sTotalAc + pTarget->m_sACAmount;
	// A unit's total AC shouldn't ever go below 0.
	if (temp_ac < 0)
		temp_ac = 0;

	temp_ap = m_sTotalHit * m_bAttackAmount;
	// Apply player vs player AC/AP bonuses.
	if (pTarget->isPlayer())
	{
		CUser* pTUser = TO_USER(pTarget);	// NOTE: using a = a*v instead of a *= v because the compiler assumes different 
		// types being multiplied, which results in these calcs not behaving correctly.

		// adjust player AP by percent, for skills like "Critical Point"
		temp_ap = temp_ap * m_bPlayerAttackAmount / 100;
		// Now handle class-specific AC/AP bonuses.
		temp_ac = temp_ac * (100 + pTUser->m_byAcClassBonusAmount[pTUser->GetBaseClass() - 1]) / 100;
		temp_ap = temp_ap * (100 + m_byAPClassBonusAmount[pTUser->GetBaseClass() - 1]) / 100;
	}

	// Allow for complete physical damage blocks.
	// NOTE: Unsure if we need to count skill usage as magic damage or if we 
	// should only block that in explicit magic damage skills (CMagicProcess::GetMagicDamage()).
	if (pTarget->m_bBlockPhysical)
		return 0;

	temp_hit_B = (int)((temp_ap * 20 / 10) / (temp_ac + 240));
	// Skill/arrow hit.    
	if (pSkill != nullptr)
	{
		// SKILL HIT! YEAH!	                                
		if (pSkill->bType[0] == 1)
		{
			_MAGIC_TYPE1* pType1 = g_pMain->m_Magictype1Array.GetData(pSkill->iNum);
			if (pType1 == nullptr)
				return -1;

			// Non-relative hit.
			if (pType1->bHitType == 2)
			{
				result = (pType1->sHitRate <= myrand(0, 100) ? FAIL : SUCCESS);
				m_bDamageAC = true;
			}
			else if (pType1->bHitType == 1)
				result = SUCCESS;
			else
				result = GetHitRate((m_fTotalHitrate / pTarget->m_fTotalEvasionrate) * (pType1->sHitRate / 100.0f));

			temp_hit = (int32)(temp_hit_B * (pType1->sHit / 100.0f));
		}
		// ARROW HIT! YEAH!
		else if (pSkill->bType[0] == 2)
		{
			_MAGIC_TYPE2* pType2 = g_pMain->m_Magictype2Array.GetData(pSkill->iNum);
			if (pType2 == nullptr)
				return -1;

			// Non-relative/Penetration hit.
			if (pType2->bHitType == 0)
				result = (pType2->sHitRate <= myrand(0, 100) ? FAIL : SUCCESS);
			else
				result = GetHitRate((m_fTotalHitrate / pTarget->m_fTotalEvasionrate) * (pType2->sHitRate / 100.0f));

			if (pType2->bHitType == 1)
				temp_hit = (int32)(m_sTotalHit * m_bAttackAmount * (pType2->sAddDamage / 80.0f) / 100);
			else
				temp_hit = (int32)(temp_hit_B * (pType2->sAddDamage / 80.0f));
		}
	}
	// Normal hit (R attack)     
	else
	{
		temp_hit = temp_ap / 100;
		result = GetHitRate(m_fTotalHitrate / pTarget->m_fTotalEvasionrate + 1.0f);
	}

	switch (result)
	{						// 1. Magical item damage....
	case GREAT_SUCCESS:
	case SUCCESS:
	case NORMAL:
		if (pSkill != nullptr)
		{	 // Skill Hit.
			damage = temp_hit;

			if (pSkill->bType[0] != 2)
				random = myrand(0, damage);

			if (pSkill->bType[0] == 1)
				damage = (short)((temp_hit + 0.3f * random) + 0.99f);
			else
				damage = (short)(((temp_hit * 0.6f) + 1.0f * random) + 0.99f);
		}
		else
		{	// Normal Hit.

			if (isGM() && !pTarget->isPlayer())
			{
				if (g_pMain->m_nGameMasterRHitDamage != 0)
				{
					damage = g_pMain->m_nGameMasterRHitDamage;
					return damage;
				}
			}

			damage = temp_hit_B;
			random = myrand(0, damage);
			damage = (short)((0.85f * temp_hit_B) + 0.3f * random);
		}

		break;
	case FAIL:
		if (pSkill != nullptr)
		{	 // Skill Hit.

		}
		else
		{ // Normal Hit.
			if (isGM() && !pTarget->isPlayer())
			{
				damage = 30000;
				return damage;
			}
		}
	}

	// Apply item bonuses
	damage = GetMagicDamage(damage, pTarget, bPreviewOnly);

	// These two only apply to players
	if (pTarget->isPlayer())
	{
		damage = GetACDamage(damage, pTarget, m_bDamageAC);		// 3. Additional AC calculation....	
		// Give increased damage in war zones (as per official 1.298~1.325)
		// This may need to be revised to use the last nation to win the war, or more accurately, 
		// the nation that last won the war 3 times in a row (whether official behaves this way now is unknown).
		if (isPriest()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& (TO_USER(pTarget)->isWarrior()))
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.40f;
			damage = (int32)dm;
		}
		else if (isPriest()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isMage())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.20f;
			damage = (int32)dm;
		}
		else if (isPriest()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPriest())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.40f;
			damage = (int32)dm;
		}
		else if (isPriest()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isRogue())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.40f;
			damage = (int32)dm;
		}
		else if (isPriest()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& (TO_USER(pTarget)->isPortuKurian()))
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.10f;
			damage = (int32)dm;
		}
		else if (isWarrior()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isRogue())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.30f;
			damage = (int32)dm;
		}
		else if (isWarrior()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isMage())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.10f;
			damage = (int32)dm;
		}
		else if (isWarrior()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isWarrior())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.95f;
			damage = (int32)dm;
		}
		else if (isWarrior()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPriest())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.55f;
			damage = (int32)dm;
		}
		else if (isWarrior()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPortuKurian())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.95f;
			damage = (int32)dm;
		}
		else if (isPortuKurian()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isRogue())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 0.80f;
			damage = (int32)dm;
		}
		else if (isPortuKurian()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isMage())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.10f;
			damage = (int32)dm;
		}
		else if (isPortuKurian()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isWarrior())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.12f;
			damage = (int32)dm;
		}
		else if (isPortuKurian()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPriest())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 0.70f;
			damage = (int32)dm;
		}
		else if (isPortuKurian()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPortuKurian())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.55f;
			damage = (int32)dm;
		}
		else if (isRogue()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isMage())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 0.85f;
			damage = (int32)dm;
		}
		else if (isRogue()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isWarrior())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.25f;
			damage = (int32)dm;
		}
		else if (isRogue()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isRogue())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.25f;
			damage = (int32)dm;
		}
		else if (isRogue()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPriest())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.35f;
			damage = (int32)dm;
		}
		else if (isRogue()
			&& pTarget != nullptr
			&& pTarget->isPlayer()
			&& TO_USER(pTarget)->isPortuKurian())
		{
			damage /= 2;
			double dm = (double)damage;
			dm *= 1.4f;
			damage = (int32)dm;
		}
		else
		{
			damage /= 3;
		}
	}

	// Enforce damage cap
	if (damage > MAX_DAMAGE)
		damage = MAX_DAMAGE;

	if (m_bDamageAC)
		m_bDamageAC = false;

	return damage;
}

void CUser::OnAttack(Unit* pTarget, AttackType attackType)
{
	if (!pTarget->isPlayer()
		|| attackType == AttackTypeMagic)
		return;

	// Trigger weapon procs for the attacker on attack
	static const uint8 itemSlots[] = { GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND, GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND };
	foreach_array(i, itemSlots)
	{
		// If we hit an applicable weapon, don't try proc'ing the other weapon. 
		// It is only ever meant to proc on the dominant weapon.
		if (TriggerProcItem(itemSlots[i], pTarget, TriggerTypeAttack))
			break;
	}
}

void CUser::OnDefend(Unit* pAttacker, AttackType attackType)
{
	if (!pAttacker->isPlayer())
		return;

	// Trigger defensive procs for the defender when being attacked
	static const uint8 itemSlots[] = { GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND, GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND };
	foreach_array(i, itemSlots)
	{
		// If we hit an applicable weapon, don't try proc'ing the other weapon. 
		// It is only ever meant to proc on the dominant weapon.
		if (TriggerProcItem(itemSlots[i], pAttacker, TriggerTypeDefend))
			break;
	}
}

/**
* @brief	Trigger item procs.
*
* @param	bSlot	   	Slot of item to attempt to proc.
* @param	pTarget	   	Target of the skill (attacker/defender depending on the proc type).
* @param	triggerType	Which type of item to proc (offensive/defensive).
*
* @return	true if there's an applicable item to proc, false if not.
*/
bool CUser::TriggerProcItem(uint8 bSlot, Unit * pTarget, ItemTriggerType triggerType)
{
	// Don't proc weapon skills if our weapon is disabled.
	if (triggerType == TriggerTypeAttack
		&& isWeaponsDisabled())
		return false;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE
		|| GetZoneID() == ZONE_CHAOS_DUNGEON
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
		return false;

	// Ensure there's an item in this slot, 
	_ITEM_DATA * pItem = GetItem(bSlot);
	if (pItem == nullptr
		// and that it doesn't need to be repaired.
		|| pItem->sDuration == 0)
		return false; // not an applicable item

	// Ensure that this item has an attached proc skill in the table
	_ITEM_OP * pData = g_pMain->m_ItemOpArray.GetData(pItem->nNum);
	if (pData == nullptr // no skill to proc
		|| pData->bTriggerType != triggerType) // don't use an offensive proc when we're defending (or vice versa)
		return false; // not an applicable item

	// At this point, we have determined there is an applicable item in the slot.
	// Should it proc now? (note: CheckPercent() checks out of 1000)
	if (!CheckPercent(pData->bTriggerRate * 10))
		return true; // it is an applicable item, it just didn't proc. No need to proc subsequent items.

	MagicInstance instance;

	instance.bIsRunProc = true;
	instance.bIsItemProc = true;
	instance.sCasterID = GetID();
	instance.nSkillID = pData->nSkillID;
	instance.sSkillCasterZoneID = pTarget->GetZoneID();

	_MAGIC_TABLE *pTable = g_pMain->m_MagictableArray.GetData(pData->nSkillID);
	if (pTable == nullptr)
		return false;

	// For AOE skills such as "Splash", the AOE should be focus on the target.
	switch (pTable->bMoral)
	{
	case MORAL_ENEMY:
	case MORAL_NPC:
		instance.sTargetID = pTarget->GetID();
		break;
	case MORAL_SELF:
		instance.sTargetID = GetID();
		break;
	case MORAL_AREA_ENEMY:
		instance.sTargetID = -1;
		instance.sData[0] = (uint16)pTarget->GetX();
		instance.sData[2] = (uint16)pTarget->GetZ();
		break;
	}

	instance.Run();
	return true; // it is an applicable item, and it proc'd. No need to proc subsequent items.
}

short CNpc::GetDamage(Unit *pTarget, _MAGIC_TABLE *pSkill /*= nullptr*/, bool bPreviewOnly /*= false*/)
{
	if (pTarget->isPlayer())
		return GetDamage(TO_USER(pTarget), pSkill);

	return GetDamage(TO_NPC(pTarget), pSkill);
}

/**
* @brief	Calculates damage for monsters/NPCs attacking players.
*
* @param	pTarget			Target player.
* @param	pSkill			The skill (not used here).
* @param	bPreviewOnly	true to preview the damage only and not apply any item bonuses (not used here).
*
* @return	The damage.
*/
short CNpc::GetDamage(CUser *pTarget, _MAGIC_TABLE *pSkill /*= nullptr*/, bool bPreviewOnly /*= false*/)
{
	if (pTarget == nullptr)
		return 0;

	int32 damage = 0, HitB;
	int32 Ac = pTarget->m_sTotalAc + pTarget->m_sACAmount;

	// A unit's total AC shouldn't ever go below 0.
	if (Ac < 0)
		Ac = 0;

	HitB = (int)((m_sTotalHit * m_bAttackAmount * 200 / 100) / (Ac + 240));

	if (HitB <= 0)
		return 0;

	uint8 result = GetHitRate(m_fTotalHitrate / pTarget->m_fTotalEvasionrate);
	switch (result)
	{
	case GREAT_SUCCESS:
		damage = (int)ceil((0.3f * myrand(1, HitB)));
		damage += (short)(0.85f * (float)HitB);
		damage = (damage * 3) / 2;
		break;

	case SUCCESS:
	case NORMAL:
		damage = (int)ceil((0.3f * myrand(1, HitB)));
		damage += (short)(0.85f * (float)HitB);
		break;
	}

	int nMaxDamage = (int)(2.6 * m_sTotalHit);
	if (damage > nMaxDamage)
		damage = nMaxDamage;

	// Enforce overall damage cap
	if (damage > MAX_DAMAGE)
		damage = MAX_DAMAGE;

	return (short)damage;
}

/**
* @brief	Calculates damage for monsters/NPCs attacking other monsters/NPCs.
*
* @param	pTarget			Target NPC/monster.
* @param	pSkill			The skill (not used here).
* @param	bPreviewOnly	true to preview the damage only and not apply any item bonuses (not used here).
*
* @return	The damage.
*/
short CNpc::GetDamage(CNpc *pTarget, _MAGIC_TABLE *pSkill /*= nullptr*/, bool bPreviewOnly /*= false*/)
{
	if (pTarget == nullptr)
		return 0;

	short damage = 0, Hit = m_sTotalHit, Ac = pTarget->m_sTotalAc;
	uint8 result = GetHitRate(m_fTotalHitrate / pTarget->m_fTotalEvasionrate);

	if (isPet()
		|| pTarget->isPet()
		|| isGuardSummon()
		|| pTarget->isGuardSummon())
		result = GREAT_SUCCESS;

	switch (result)
	{
	case GREAT_SUCCESS:
		damage = (short)(0.6 * Hit);
		if (damage <= 0)
		{
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.7 * Hit);
		break;

	case SUCCESS:
	case NORMAL:
		if (Hit - Ac > 0)
		{
			damage = (short)(0.6 * (Hit - Ac));
			if (damage <= 0)
			{
				damage = 0;
				break;
			}
			damage = myrand(0, damage);
			damage += (short)(0.7 * (Hit - Ac));
		}
		break;
	}

	// Enforce damage cap
	if (damage > MAX_DAMAGE)
		damage = MAX_DAMAGE;

	return damage;
}

short Unit::GetMagicDamage(int damage, Unit *pTarget, bool bPreviewOnly /*= false*/)
{
	if (pTarget->isDead()
		|| pTarget->isBlinking())
		return 0;

	Guard lock(m_equippedItemBonusLock);
	int16 sReflectDamage = 0;

	foreach(itr, m_equippedItemBonuses)
	{
		foreach(bonusItr, itr->second)
		{
			short total_r = 0, temp_damage = 0;
			uint8 bType = bonusItr->first;
			int16 sAmount = bonusItr->second;

			bool bIsDrain = (bType >= ITEM_TYPE_HP_DRAIN && bType <= ITEM_TYPE_MP_DRAIN);
			if (bIsDrain)
				temp_damage = damage * sAmount / 60;

			switch (bType)
			{
			case ITEM_TYPE_FIRE:
				total_r = (pTarget->m_sFireR + pTarget->m_bAddFireR) * pTarget->m_bPctFireR / 100;
				break;
			case ITEM_TYPE_COLD:
				total_r = (pTarget->m_sColdR + pTarget->m_bAddColdR) * pTarget->m_bPctColdR / 100;
				break;
			case ITEM_TYPE_LIGHTNING:
				total_r = (pTarget->m_sLightningR + pTarget->m_bAddLightningR) * pTarget->m_bPctLightningR / 100;
				break;
			case ITEM_TYPE_HP_DRAIN:
				HpChange(temp_damage);
				goto fail_return;
			case ITEM_TYPE_MP_DAMAGE:
				pTarget->MSpChange(-temp_damage);
				break;
			case ITEM_TYPE_MP_DRAIN:
				MSpChange(temp_damage);
				goto fail_return;
			}

			total_r += pTarget->m_bResistanceBonus;
			if (!bIsDrain)
			{
				if (total_r > 200)
					total_r = 200;

				temp_damage = sAmount - sAmount * total_r / 200;
				damage += temp_damage;
			}
		}
	}
fail_return:
	{
		Guard lock(pTarget->m_equippedItemBonusLock);
		foreach(itr, pTarget->m_equippedItemBonuses)
		{
			foreach(bonusItr, itr->second)
			{
				int16 sAmount = bonusItr->second;

				if (bonusItr->first == ITEM_TYPE_MIRROR_DAMAGE)
					sReflectDamage += sAmount;
			}
		}
	}

	// If any items have have damage reflection enabled, we should reflect this back to the client.
	// NOTE: This should only apply to shields, so it should only ever apply once.
	// We do this here to ensure it's taking into account the total calculated damage.
	if (sReflectDamage > 0)
	{
		short temp_damage = damage * sReflectDamage / 300;
		HpChange(-temp_damage, pTarget);
	}

	return damage;
}

short Unit::GetACDamage(int damage, Unit *pTarget, bool bDamageAC)
{
	// This isn't applicable to NPCs.
	if (!isPlayer()
		|| !pTarget->isPlayer())
		return damage;

	if (pTarget->isDead())
		return 0;

	CUser * pUser = TO_USER(this);

	if (pUser->isWeaponsDisabled())
		return damage;

	uint8 weaponSlots[] = { GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_LEFTHAND : LEFTHAND, GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND };
	int firstdamage = damage;

	foreach_array(slot, weaponSlots)
	{
		_ITEM_TABLE* pWeapon = pUser->GetItemPrototype(weaponSlots[slot]);
		if (pWeapon == nullptr)
			continue;

		if (pWeapon->isDagger())
			damage -= damage * ((bDamageAC ? int16(0) : pTarget->m_sDaggerR) * pTarget->m_byDaggerRAmount / 100) / 200;
		else if (pWeapon->isJamadar())
			damage -= damage * ((bDamageAC ? int16(0) : pTarget->m_sJamadarR) * pTarget->m_byDaggerRAmount / 100) / 200;
		else if (pWeapon->isSword())
			damage -= damage * pTarget->m_sSwordR / 200;
		else if (pWeapon->isAxe())
			damage -= damage * pTarget->m_sAxeR / 200;
		else if (pWeapon->isMace())
			damage -= damage * pTarget->m_sMaceR / 200;
		else if (pWeapon->isSpear())
			damage -= damage * pTarget->m_sSpearR / 200;
		else if (pWeapon->isBow())
			damage -= damage * ((bDamageAC ? int16(0) : pTarget->m_sBowR) * pTarget->m_byBowRAmount / 100) / 200;
	}

	return damage;
}

uint8 Unit::GetHitRate(float rate)
{
	int random = myrand(1, 10000);
	if (rate >= 5.0f)
	{
		if (random >= 1 && random <= 3500)
			return GREAT_SUCCESS;
		else if (random >= 3501 && random <= 7500)
			return SUCCESS;
		else if (random >= 7501 && random <= 9800)
			return NORMAL;
	}
	else if (rate < 5.0f && rate >= 3.0f)
	{
		if (random >= 1 && random <= 2500)
			return GREAT_SUCCESS;
		else if (random >= 2501 && random <= 6000)
			return SUCCESS;
		else if (random >= 6001 && random <= 9600)
			return NORMAL;
	}
	else if (rate < 3.0f && rate >= 2.0f)
	{
		if (random >= 1 && random <= 2000)
			return GREAT_SUCCESS;
		else if (random >= 2001 && random <= 5000)
			return SUCCESS;
		else if (random >= 5001 && random <= 9400)
			return NORMAL;
	}
	else if (rate < 2.0f && rate >= 1.25f)
	{
		if (random >= 1 && random <= 1500)
			return GREAT_SUCCESS;
		else if (random >= 1501 && random <= 4000)
			return SUCCESS;
		else if (random >= 4001 && random <= 9200)
			return NORMAL;
	}
	else if (rate < 1.25f && rate >= 0.8f)
	{
		if (random >= 1 && random <= 1000)
			return GREAT_SUCCESS;
		else if (random >= 1001 && random <= 3000)
			return SUCCESS;
		else if (random >= 3001 && random <= 9000)
			return NORMAL;
	}
	else if (rate < 0.8f && rate >= 0.5f)
	{
		if (random >= 1 && random <= 800)
			return GREAT_SUCCESS;
		else if (random >= 801 && random <= 2500)
			return SUCCESS;
		else if (random >= 2501 && random <= 8000)
			return NORMAL;
	}
	else if (rate < 0.5f && rate >= 0.33f)
	{
		if (random >= 1 && random <= 600)
			return GREAT_SUCCESS;
		else if (random >= 601 && random <= 2000)
			return SUCCESS;
		else if (random >= 2001 && random <= 7000)
			return NORMAL;
	}
	else if (rate < 0.33f && rate >= 0.2f)
	{
		if (random >= 1 && random <= 400)
			return GREAT_SUCCESS;
		else if (random >= 401 && random <= 1500)
			return SUCCESS;
		else if (random >= 1501 && random <= 6000)
			return NORMAL;
	}
	else
	{
		if (random >= 1 && random <= 200)
			return GREAT_SUCCESS;
		else if (random >= 201 && random <= 1000)
			return SUCCESS;
		else if (random >= 1001 && random <= 5000)
			return NORMAL;
	}

	return FAIL;
}

void Unit::SendToRegion(Packet* result) { g_pMain->Send_Region(result, GetMap(), GetRegionX(), GetRegionZ(), nullptr, GetEventRoom()); }

void Unit::InitType3()
{
	for (int i = 0; i < MAX_TYPE3_REPEAT; i++)
		m_durationalSkills[i].Reset();

	m_bType3Flag = false;
}

void Unit::InitType4(bool bRemoveSavedMagic /*= false*/, uint8 buffType /* = 0 */)
{
	// Remove all buffs that should not be recast.
	Guard lock(m_buffLock);
	Type4BuffMap buffMap = m_buffMap; // copy the map

	for (auto itr = buffMap.begin(); itr != buffMap.end(); itr++)
	{
		if (buffType > 0 && itr->second.m_bBuffType != buffType)
			continue;

		CMagicProcess::RemoveType4Buff(itr->first, this, bRemoveSavedMagic, buffType > 0 ? true : false);
	}
}

/**
* @brief	Determine if this unit is basically able to attack the specified unit.
* 			This should only be called to handle the minimal shared logic between
* 			NPCs and players.
*
* 			You should use the more appropriate CUser or CNpc specialization.
*
* @param	pTarget	Target for the attack.
*
* @return	true if we can attack, false if not.
*/
bool Unit::CanAttack(Unit * pTarget)
{
	if (pTarget == nullptr)
		return false;

	// Units cannot attack units in different event room.
	if (GetEventRoom() != pTarget->GetEventRoom())
		return false;

	// Units cannot attack units in different zones.
	if (GetZoneID() != pTarget->GetZoneID())
		return false;

	// We cannot attack our target if we are incapacitated 
	// (should include debuffs & being blinded)
	if (isIncapacitated()
		// or if our target is in a state in which
			// they should not be allowed to be attacked
		|| pTarget->isDead()
		|| pTarget->isBlinking())
		return false;

	// Finally, we can only attack the target if we are hostile towards them.
	return isHostileTo(pTarget);
}

/**
* @brief	Determine if this unit is basically able to attack the specified unit.
* 			This should only be called to handle the minimal shared logic between
* 			NPCs and players.
*
* 			You should use the more appropriate CUser or CNpc specialization.
*
* @param	pTarget	Target for the attack.
*
* @return	true if we attackable, false if not.
*/
bool Unit::isAttackable(Unit * pTarget)
{
	if (pTarget == nullptr)
		pTarget = this;

	if (pTarget)
	{
		if (pTarget->isNPC())
		{
			CNpc * pNpc = TO_NPC(pTarget);
			if (pNpc != nullptr)
			{
				if (pNpc->GetType() == NPC_BIFROST_MONUMENT)
					return (g_pMain->m_bAttackBifrostMonument);
				else if (pNpc->GetType() == NPC_PVP_MONUMENT)
				{
					if ((GetNation() == KARUS && pNpc->GetPID() == MONUMENT_KARUS_SPID)
						|| (GetNation() == ELMORAD && pNpc->GetPID() == MONUMENT_ELMORAD_SPID))
						return false;
				}
				else if (pNpc->GetType() == NPC_GUARD_TOWER1
					|| pNpc->GetType() == NPC_GUARD_TOWER2
					|| pNpc->GetType() == NPC_GATE2
					|| pNpc->GetType() == NPC_VICTORY_GATE
					|| pNpc->GetType() == NPC_PHOENIX_GATE
					|| pNpc->GetType() == NPC_SPECIAL_GATE
					|| pNpc->GetType() == NPC_GATE_LEVER)
					return false;
			}
		}
	}

	return true;
}

bool Unit::CanCastRHit(uint16 m_socketID)
{
	CUser *pUser = g_pMain->GetUserPtr(m_socketID);

	if (pUser == nullptr)
		return true;

	if (pUser->m_RHitRepeatList.find(m_socketID) != pUser->m_RHitRepeatList.end())
	{
		RHitRepeatList::iterator itr = pUser->m_RHitRepeatList.find(m_socketID);
		if (float(UNIXTIME2 - itr->second) < PLAYER_R_HIT_REQUEST_INTERVAL)
			return false;
		else
		{
			pUser->m_RHitRepeatList.erase(m_socketID);
			return true;
		}
	}

	return true;
}

void Unit::OnDeath(Unit *pKiller)
{
	SendDeathAnimation(pKiller);
}

void Unit::SendDeathAnimation(Unit * pKiller /*= nullptr*/)
{
	Packet result(WIZ_DEAD);
	result << GetID();
	SendToRegion(&result);
}

void Unit::AddType4Buff(uint8 bBuffType, _BUFF_TYPE4_INFO & pBuffInfo)
{
	Guard lock(m_buffLock);
	m_buffMap.insert(std::make_pair(bBuffType, pBuffInfo));

	if (pBuffInfo.isBuff())
		m_buffCount++;
}

/**************************************************************************
* The following methods should not be here, but it's necessary to avoid
* code duplication between AI and GameServer until they're better merged.
**************************************************************************/
/**
* @brief	Sets zone attributes for the loaded zone.
*
* @param	zoneNumber	The zone number.
*/
void KOMap::SetZoneAttributes(_ZONE_INFO *pZone)
{
	m_zoneFlags = 0;
	m_byTariff = 10; // defaults to 10 officially for zones that don't use it.
	m_byMinLevel = pZone->m_MinLevel;
	m_byMaxLevel = pZone->m_MaxLevel;
	m_zoneType = (ZoneAbilityType)pZone->m_ZoneType;

	if (pZone->m_kTradeOtherNation)
		m_zoneFlags |= ZF_TRADE_OTHER_NATION;
	if (pZone->m_kTalkOtherNation)
		m_zoneFlags |= ZF_TALK_OTHER_NATION;
	if (pZone->m_kAttackOtherNation)
		m_zoneFlags |= ZF_ATTACK_OTHER_NATION;
	if (pZone->m_kAttackSameNation)
		m_zoneFlags |= ZF_ATTACK_SAME_NATION;
	if (pZone->m_kFriendlyNpc)
		m_zoneFlags |= ZF_FRIENDLY_NPCS;
	if (pZone->m_kWarZone)
		m_zoneFlags |= ZF_WAR_ZONE;
	if (pZone->m_kClanUpdates)
		m_zoneFlags |= ZF_CLAN_UPDATE;

	m_bBlink = pZone->m_bBlink;
	m_bTeleport = pZone->m_bTeleport;
	m_bGate = pZone->m_bGate;
	m_bEscape = pZone->m_bEscape;
	m_bCallingFriend = pZone->m_bCallingFriend;
	m_bTeleportFriend = pZone->m_bTeleportFriend;
	m_bPetSpawn = pZone->m_bPetSpawn;
	m_bExpLost = pZone->m_bExpLost;
	m_bGiveLoyalty = pZone->m_bGiveLoyalty;
	m_bGuardSummon = pZone->m_bGuardSummon;
	m_bBlinkZone = pZone->m_bBlinkZone;
}

void KOMap::UpdateDelosDuringCSW(bool Open, ZoneAbilityType type)
{
	if (Open)
	{
		m_zoneType = type;
		m_zoneFlags = ZF_ATTACK_OTHER_NATION | ZF_ATTACK_SAME_NATION | ZF_FRIENDLY_NPCS;
	}
	else
	{
		m_zoneType = ZoneAbilitySiegeDisabled;
		m_zoneFlags = ZF_TRADE_OTHER_NATION | ZF_TALK_OTHER_NATION | ZF_FRIENDLY_NPCS;
	}
}

/**
* @brief	Determines if an NPC is hostile to a unit.
* 			Non-hostile units cannot be attacked.
*
* @param	pTarget	Target unit
*
* @return	true if hostile to, false if not.
*/
bool CNpc::isHostileTo(Unit * pTarget)
{
	if (pTarget == nullptr)
		return false;

	// Units cannot attack units in different event room.
	if (GetEventRoom() != pTarget->GetEventRoom())
		return false;

	// Only players can attack these targets.
	if (pTarget->isPlayer())
	{
		// Scarecrows are NPCs that the client allows us to attack
		// however, since they're not a monster, and all NPCs in neutral zones
		// are friendly, we need to override to ensure we can attack them server-side.
		switch (GetType())
		{
		case NPC_CLAN_WAR_MONUMENT:
			return true;
			break;
		case NPC_SCARECROW:
			return true;
			break;
		case NPC_BIFROST_MONUMENT:
			if (g_pMain->m_bAttackBifrostMonument)
				return true;
			break;
		case NPC_BORDER_MONUMENT:
		{
			if (g_pMain->isBorderDefenceWarActive()
				&& isInTempleEventZone())
				return true;
		}
		break;
		}

		if (GetZoneID() == ZONE_DELOS)
		{
			if (GetType() == NPC_DESTROYED_ARTIFACT
				|| isCswDoors())
			{
				_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
				if (pKnightSiege == nullptr
					|| !TO_USER(pTarget)->isInClan()
					|| !g_pMain->pCswEvent.Started
					|| g_pMain->pCswEvent.Status != CswOperationStatus::CastellanWar
					|| g_pMain->m_byBattleOpen != SIEGE_BATTLE)
					return false;

				if (pKnightSiege->sMasterKnights != 0)
				{
					if (TO_USER(pTarget)->GetClanID() == pKnightSiege->sMasterKnights)
						return false;
				}

				_CASTELLAN_WAR_INFO* pCastellanInfo = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(TO_USER(pTarget)->GetClanID());
				if (pCastellanInfo == nullptr)
				{
					_CASTELLAN_OLD_CSW_WINNER* pOldWinnerInfo = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(TO_USER(pTarget)->GetClanID());
					if (pOldWinnerInfo == nullptr)
					{
						return false;
					}
				}
			}
			return true;
		}

		if (isGuardSummon())
		{
			if (isInMoradon())
			{
				if (TO_USER(pTarget)->isInPartyArena())
				{
					CUser* pUser = g_pMain->GetUserPtr(GetGuardID());

					if (pUser == nullptr)
						return false;

					if (pUser->isInParty() && pTarget->isPlayer()
						&& pUser->GetPartyID() == TO_USER(pTarget)->GetPartyID())
						return false;
					else if (pUser->GetName() == TO_USER(pTarget)->GetName())
						return false;
					else
						return true;
				}
				else if (TO_USER(pTarget)->isInMeleeArena())
				{
					CUser* pUser = g_pMain->GetUserPtr(GetGuardID());

					if (pUser == nullptr)
						return false;

					if (pUser->isInParty() && pTarget->isPlayer()
						&& pUser->GetPartyID() == TO_USER(pTarget)->GetPartyID())
						return false;
					else if (pUser->GetName() == TO_USER(pTarget)->GetName())
						return false;
					else
						return true;
				}
				else
					return false;
			}
			else
			{
				if (TO_USER(pTarget)->isInEnemySafetyArea())
					return false;
				else
				{
					CUser* pUser = g_pMain->GetUserPtr(GetGuardID());

					if (pUser == nullptr)
						return false;

					if (pUser->isInParty() && pTarget->isPlayer()
						&& pUser->GetPartyID() == TO_USER(pTarget)->GetPartyID())
						return false;
					else if (pUser->GetName() == TO_USER(pTarget)->GetName())
						return false;
					else if (pUser->GetNation() == TO_USER(pTarget)->GetNation())
						return false;
					else
						return true;
				}
			}
		}
	}

	if (g_pMain->m_byBattleOpen == NATION_BATTLE && GetMap()->isWarZone())
	{
		if (GetProtoID() > 1100 && GetProtoID() <= 2203
			|| GetProtoID() > 2301 && GetProtoID() <= 2306)
			return true;
	}

	if (!isMonster() && GetProtoID() == 511)
	{
		if (pTarget->GetZoneID() == ZONE_DELOS)
		{
			if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	if (!pTarget->isPlayer())
	{
		// Scarecrows are NPCs that the client allows us to attack
		// however, since they're not a monster, and all NPCs in neutral zones
		// are friendly, we need to override to ensure we can attack them server-side.

		switch (TO_NPC(pTarget)->GetType())
		{
		case NPC_CLAN_WAR_MONUMENT:
			return false;
			break;
		case NPC_SCARECROW:
			return false;
			break;
		case NPC_BIFROST_MONUMENT:
			return false;
			break;
		case NPC_BORDER_MONUMENT:
			return false;
			break;
		case NPC_OBJECT_FLAG:
			CNpc *pNpc = g_pMain->GetPetPtr(TO_NPC(pTarget)->GetPetID(), TO_NPC(pTarget)->GetZoneID());
			if (pNpc != nullptr)
			{
				switch (GetType())
				{
				case NPC_GUARD:
					return false;
					break;
				case NPC_PATROL_GUARD:
					return false;
					break;
				case NPC_STORE_GUARD:
					return false;
					break;
				}

				if (!pNpc->isDead())
					return true;
			}
			break;
		}

		if (isGuardSummon())
		{
			if (TO_NPC(pTarget)->isMonster())
				return true;
			else
				return false;
		}

		if (TO_NPC(pTarget)->isGuardSummon())
			return false;

		if (isMonster())
		{
			if (TO_NPC(pTarget)->isMonster())
				return false;
		}
	}

	// A nation of 0 indicates friendliness to all
	if (GetNation() == Nation::ALL
		// Also allow for cases when all NPCs in this zone are inherently friendly.
		|| (!isMonster() && GetMap()->areNPCsFriendly()))
		return false;

	// A nation of 3 indicates hostility to all (or friendliness to none)
	if (GetNation() == Nation::NONE
		&& pTarget->GetNation() != Nation::NONE)
		return true;

	if (GetMap()->isWarZone()
		&& g_pMain->m_bResultDelay == true
		&& g_pMain->m_bResultDelayVictory != pTarget->GetNation()
		&& GetNation() != pTarget->GetNation())
		return false;

	// An NPC cannot attack a unit of the same nation
	return (GetNation() != pTarget->GetNation());
}

/**
* @brief	Determines if a player is hostile to a unit.
* 			Non-hostile units cannot be attacked.
*
* @param	pTarget	Target unit
*
* @return	true if hostile to, false if not.
*/
bool CUser::isHostileTo(Unit * pTarget)
{
	if (pTarget == nullptr)
		return false;

	// Units cannot attack units in different event room.
	if (GetEventRoom() != pTarget->GetEventRoom())
		return false;

	// For non-player hostility checks, refer to the appropriate override.
	if (!pTarget->isPlayer())
		return pTarget->isHostileTo(this);

	// Players can attack other players in the Moradon arena.
	if (isInArena() && TO_USER(pTarget)->isInArena())
	{
		if ((GetX() < 735.0f && GetX() > 684.0f)
			&& (GetZ() < 411.0f && GetZ() > 360.0f)) // party arena in moradon
		{
			if (isInParty() && pTarget->isPlayer()
				&& GetPartyID() == TO_USER(pTarget)->GetPartyID())
				return false;
			else
				return true;
		}
		else if ((GetX() < 735.0f && GetX() > 684.0f)
			&& (GetZ() < 491.0f && GetZ() > 440.0f)) // melee arena in moradon
		{
			if (GetName() == pTarget->GetName())
				return false;
			else
				return true;
		}
	}

	// Players can attack other players in the arena.
	if (GetZoneID() == ZONE_ARENA)
	{
		if (isInRangeSlow(64.0f, 178.0f, 60)
			|| isInRangeSlow(192.0f, 178.0f, 60)) // we are at the rose clan arena
		{
			if (!isInClan() || !TO_USER(pTarget)->isInClan())
				return false;

			if (isInClan() && TO_USER(pTarget)->isInClan()
				&& GetClanID() == TO_USER(pTarget)->GetClanID())
				return false;
		}

		if (isInRangeSlow(69.0f, 64.0f, 40)
			|| isInRangeSlow(180.0f, 64.0f, 40)) // we are at the rose mellee arena
		{
			if (GetName() == pTarget->GetName())
				return false;
		}

		if (isInEnemySafetyArea())
			return false;

		return true;
	}

	// Players can attack other players in the safety area.
	if (GetNation() != pTarget->GetNation()
		&& TO_USER(pTarget)->isInOwnSafetyArea())
		return false;

	// Players can attack opposing nation players when they're in PVP zones.
	if (GetNation() != pTarget->GetNation()
		&& isInPVPZone())
		return true;

	// Players can attack opposing nation players when they're in PVP zones.
	if (GetNation() != pTarget->GetNation()
		&& (GetZoneID() == ZONE_DESPERATION_ABYSS
			|| GetZoneID() == ZONE_HELL_ABYSS
			|| GetZoneID() == ZONE_DRAGON_CAVE))
		return true;

	if (ChaosTempleEventZone())
		return true;

	if (GetZoneID() == ZONE_DELOS)
	{
		if (g_pMain->pCswEvent.Started)
		{
			if (!isInOwnSafetyArea() && !TO_USER(pTarget)->isInOwnSafetyArea())
			{
				CKnights* pKnightsSource = g_pMain->GetClanPtr(GetClanID());
				CKnights* pKnightsTarget = g_pMain->GetClanPtr(TO_USER(pTarget)->GetClanID());
				if (pKnightsSource != nullptr && pKnightsTarget != nullptr)
				{
					_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
					if (pKnightSiege != nullptr)
					{
						if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
						{
							_DEATHMATCH_WAR_INFO* pDeathmatchSource = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
							_DEATHMATCH_WAR_INFO* pDeathmatchTarget = g_pMain->m_KnightSiegeWarClanList.GetData(TO_USER(pTarget)->GetClanID());
							if (pDeathmatchSource != nullptr && pDeathmatchTarget != nullptr)
							{
								if (GetClanID() != TO_USER(pTarget)->GetClanID())
									return true;
							}
						}
						else if (g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
						{
							_CASTELLAN_OLD_CSW_WINNER* pOldWinnerSource = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(GetClanID());
							if (pOldWinnerSource != nullptr)
							{
								_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(TO_USER(pTarget)->GetClanID());
								if (pInfo1 != nullptr
									&& (GetClanID() != TO_USER(pTarget)->GetClanID()))
									return true;
							}
							else
							{
								if (pKnightSiege->sMasterKnights == GetClanID())
								{
									_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(TO_USER(pTarget)->GetClanID());
									if (pInfo1 != nullptr
										&& (GetClanID() != TO_USER(pTarget)->GetClanID()))
										return true;
								}
							}

							_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
							if (pInfo1 != nullptr && GetClanID() != pKnightSiege->sMasterKnights)
							{
								_CASTELLAN_OLD_CSW_WINNER* pOldWinnerTarget = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(TO_USER(pTarget)->GetClanID());
								if (pOldWinnerTarget != nullptr)
									return true;
								else
								{
									if (TO_USER(pTarget)->GetClanID() == pKnightSiege->sMasterKnights)
										return true;
								}
							}
						}
					}
				}
			}
		}
		return false;
	}

	if (GetZoneID() == ZONE_KNIGHT_ROYALE
		&& pTarget->GetZoneID() == ZONE_KNIGHT_ROYALE)
	{
		if (g_pMain->isKnightRoyaleActive())
			return true;
	}

	// Players can attack opposing nation players during wars.
	if (GetNation() != pTarget->GetNation()
		&& (isInElmoradCastle() || isInLufersonCastle())
		&& (g_pMain->m_byElmoradOpenFlag || g_pMain->m_byKarusOpenFlag))
		return true;

	if (isGM())
	{
		// Players can attack opposing nation players during wars.
		if (GetNation() != pTarget->GetNation()
			&& (isInElmoradCastle() || isInLufersonCastle()))
			return true;
	}

	// Players cannot attack other players in any other circumstance.
	return false;
}

/**
* @brief	Determine if this user is in an arena area.
*
* @return	true if in arena, false if not.
*/
bool CUser::isInArena()
{
	/*
	All of this needs to be handled more generically
	(i.e. bounds loaded from the database, or their existing SMD method).
	*/

	// If we're in the Arena zone, assume combat is acceptable everywhere.
	// NOTE: This is why we need generic bounds checks, to ensure even attacks in the Arena zone are in one of the 4 arena locations.
	if (GetZoneID() == ZONE_ARENA)
		return true;

	bool bIsNeutralZone = (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5);

	// The only other arena is located in Moradon. If we're not in Moradon, then it's not an Arena.
	if (!bIsNeutralZone)
		return false;

	// Moradon outside arena spawn bounds.
	return ((GetX() < 735.0f && GetX() > 684.0f)
		&& ((GetZ() < 491.0f && GetZ() > 440.0f)
			|| (GetZ() < 411.0f && GetZ() > 360.0f)));
}

/**
* @brief	Determine if this user is in a normal PVP zone.
* 			That is, they're in an PK zone that allows combat
* 			against the opposite nation.
*
* @return	true if in PVP zone, false if not.
*/
bool CUser::isInPVPZone()
{
	if (GetZoneID() == ZONE_RONARK_LAND
		|| GetZoneID() == ZONE_RONARK_LAND_BASE
		|| GetZoneID() == ZONE_ARDREAM
		|| GetZoneID() == ZONE_SNOW_BATTLE
		|| GetZoneID() == ZONE_BATTLE
		|| GetZoneID() == ZONE_BATTLE2
		|| GetZoneID() == ZONE_BATTLE3
		|| GetZoneID() == ZONE_BATTLE4
		|| GetZoneID() == ZONE_BATTLE5
		|| GetZoneID() == ZONE_BATTLE6
		|| GetZoneID() == ZONE_JURAID_MOUNTAIN
		|| GetZoneID() == ZONE_BORDER_DEFENSE_WAR
		|| GetZoneID() == ZONE_CLAN_WAR_ARDREAM
		|| GetZoneID() == ZONE_CLAN_WAR_RONARK
		|| GetZoneID() == ZONE_PARTY_VS_1
		|| GetZoneID() == ZONE_PARTY_VS_2
		|| GetZoneID() == ZONE_PARTY_VS_3
		|| GetZoneID() == ZONE_PARTY_VS_4
		|| GetZoneID() == ZONE_BIFROST
		|| GetZoneID() == ZONE_KROWAZ_DOMINION)
		return true;
	else
		return false;
}

bool CUser::isInClanArena()
{
	if (GetZoneID() != ZONE_ARENA)
		return false;

	return (isInRangeSlow(64.0f, 178.0f, 60)
		|| isInRangeSlow(192.0f, 178.0f, 60));
}

/**
* @brief	Determine if this user is in an safety area.
*
* @return	true if in safety area, false if not.
*/
bool CUser::isInEnemySafetyArea()
{
	switch (GetZoneID())
	{
	case ZONE_DELOS:
		return isInRangeSlow(500.0f, 180.0f, 115);

	case ZONE_BIFROST:
		if (GetNation() == ELMORAD)
			return ((GetX() < 124.0f && GetX() > 56.0f) && ((GetZ() < 840.0f && GetZ() > 700.0f)));
		else
			return ((GetX() < 270.0f && GetX() > 190.0f) && ((GetZ() < 970.0f && GetZ() > 870.0f)));

	case ZONE_ARENA:
		return isInRangeSlow(127.0f, 113.0f, 36);

	case ZONE_ELMORAD:
	case ZONE_ELMORAD2:
	case ZONE_ELMORAD3:
		if (GetNation() == ELMORAD) // 210 , 1853 emc
			return isInRangeSlow(float(210.0f), float(1853.0f), 50);// (GetX() < 244.0f && GetX() > 176.0f) && ((GetZ() < 1880.0f && GetZ() > 1820.0f)));
		break;

	case ZONE_KARUS:
	case ZONE_KARUS2:
	case ZONE_KARUS3:
		if (GetNation() == KARUS)
			return isInRangeSlow(float(1860.0f), float(174.0f), 50);
		break;

	case ZONE_BATTLE:
		if (GetNation() == KARUS)
			return ((GetX() < 125.0f && GetX() > 98.0f) && ((GetZ() < 780.0f && GetZ() > 755.0f)));
		else if (GetNation() == ELMORAD)
			return ((GetX() < 831.0f && GetX() > 805.0f) && ((GetZ() < 110.0f && GetZ() > 85.0f)));

	case ZONE_BATTLE2:
		if (GetNation() == KARUS)
			return ((GetX() < 977.0f && GetX() > 942.0f) && ((GetZ() < 904.0f && GetZ() > 863.0f)));
		else if (GetNation() == ELMORAD)
			return ((GetX() < 80.0f && GetX() > 46.0f) && ((GetZ() < 174.0f && GetZ() > 142.0f)));

	case ZONE_BATTLE4:
		if (GetNation() == KARUS)
			return isInRangeSlow(float(235.0f), float(228.0f), 80)
			|| isInRangeSlow(float(846.0f), float(362.0f), 20)
			|| isInRangeSlow(float(338.0f), float(807.0f), 20);
		else if (GetNation() == ELMORAD)
			return isInRangeSlow(float(809.0f), float(783.0f), 80)
			|| isInRangeSlow(float(182.0f), float(668.0f), 20)
			|| isInRangeSlow(float(670.0f), float(202.0f), 20);
	}
	return false;
}

/**
* @brief	Determine if this user is in an safety area.
*
* @return	true if in safety area, false if not.
*/
bool CUser::isInOwnSafetyArea()
{
	switch (GetZoneID())
	{
	case ZONE_DELOS:
		return isInRangeSlow(500.0f, 180.0f, 115);

	case ZONE_BIFROST:
		if (GetNation() == KARUS)
			return ((GetX() < 124.0f && GetX() > 56.0f) && ((GetZ() < 840.0f && GetZ() > 700.0f)));
		else
			return ((GetX() < 270.0f && GetX() > 190.0f) && ((GetZ() < 970.0f && GetZ() > 870.0f)));

	case ZONE_ARENA:
		return isInRangeSlow(127.0f, 113.0f, 36);

	case ZONE_ELMORAD:
	case ZONE_ELMORAD2:
	case ZONE_ELMORAD3:
		if (GetNation() == KARUS) // 210 , 1853 emc
			return isInRangeSlow(float(210.0f), float(1853.0f), 50);// (GetX() < 244.0f && GetX() > 176.0f) && ((GetZ() < 1880.0f && GetZ() > 1820.0f)));
		break;

	case ZONE_KARUS:
	case ZONE_KARUS2:
	case ZONE_KARUS3:
		if (GetNation() == ELMORAD) // 1860 , 174 emc
			return isInRangeSlow(float(1860.0f), float(174.0f), 50);
		break;

	case ZONE_BATTLE:
		if (GetNation() == ELMORAD)
			return ((GetX() < 125.0f && GetX() > 98.0f) && ((GetZ() < 780.0f && GetZ() > 755.0f)));
		else if (GetNation() == KARUS)
			return ((GetX() < 831.0f && GetX() > 805.0f) && ((GetZ() < 110.0f && GetZ() > 85.0f)));

	case ZONE_BATTLE2:
		if (GetNation() == ELMORAD)
			return ((GetX() < 977.0f && GetX() > 942.0f) && ((GetZ() < 904.0f && GetZ() > 863.0f)));
		else if (GetNation() == KARUS)
			return ((GetX() < 80.0f && GetX() > 46.0f) && ((GetZ() < 174.0f && GetZ() > 142.0f)));

	case ZONE_BATTLE4:
		if (GetNation() == ELMORAD)
			return isInRangeSlow(float(235.0f), float(228.0f), 80)
			|| isInRangeSlow(float(846.0f), float(362.0f), 20)
			|| isInRangeSlow(float(338.0f), float(807.0f), 20);
		else if (GetNation() == KARUS)
			return isInRangeSlow(float(809.0f), float(783.0f), 80)
			|| isInRangeSlow(float(182.0f), float(668.0f), 20)
			|| isInRangeSlow(float(670.0f), float(202.0f), 20);
	}
	return false;
}

bool Unit::isSameEventRoom(Unit *pTarget)
{
	if (pTarget == nullptr)
		return false;

	if (pTarget->isNPC()) {
		if (GetEventRoom() == ((CNpc*)pTarget)->GetEventRoom())
			return true;
	}
	else {
		if (GetEventRoom() == ((CUser*)pTarget)->GetEventRoom())
			return true;
	}
	return false;
}

/**
* @brief	Calculates and resets the player's stats/resistances.
*
* @param	bSendPacket	true to send a subsequent item movement packet
* 						which is almost always required in addition to
* 						using this method.
*/
void Unit::SetNpcAbility(bool bSendPacket /*= true*/)
{
	if (m_sACAmount < 0)
		m_sACAmount = 0;

	m_sTotalAc = m_sTotalAcTemp;
	if (m_sACPercent <= 0)
		m_sTotalAc = 0;
	else
		m_sTotalAc = m_sTotalAc * m_sACPercent / 100;

	m_MaxHP = m_MaxHPTemp + m_sMaxHPAmount;
}

bool CUser::SendPrisonZoneArea()
{
	if (!isGM())
	{
		switch (GetZoneID())
		{
		case ZONE_ELMORAD:
		case ZONE_ELMORAD2:
		case ZONE_ELMORAD3:
			if (GetNation() == ELMORAD)
			{
				return false;
			}
			else if (GetNation() == KARUS)
			{
				return false;
			}
			break;
		}
	}
	return false;
}