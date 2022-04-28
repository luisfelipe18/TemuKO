#include "stdafx.h"
#include "DBAgent.h"

#pragma region GenderChangeV2(Packet & pkt)
void CUser::GenderChangeV2(Packet& pkt)
{
	Packet result(WIZ_GENDER_CHANGE);

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| isFishing())
		return;

	int8 opcode, gRace, gFace, Results = 0;
	uint32 bHair;

	pkt >> opcode >> gRace >> gFace >> bHair;

	if (!CheckExistItem(ITEM_GENDER_CHANGE))
		goto fail_return;

	if (gRace == 0 || gFace == 0 || bHair == 0)
		goto fail_return;

	if (gRace < 10 && GetNation() != 1 || (gRace > 10 && GetNation() != 2) || (gRace > 5 && GetNation() == 1))
		goto fail_return;

	if (gRace > 13 || gRace == GetRace())
		goto fail_return;

	m_bRace = gRace;
	m_bFace = gFace;
	m_nHair = bHair;

	Results = g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);

	if (Results < 1)
		goto fail_return;

	result << m_bRace << m_bFace << m_nHair << GetClass();
	Send(&result);

	if (GetHealth() < (GetMaxHealth() / 2))
		HpChange(GetMaxHealth());

fail_return:
	result << Results;
	Send(&result);
}
#pragma endregion


#pragma region Gender Change
bool CUser::GenderChange(uint8 Race /*= 0*/)
{
	if (Race == 0 || Race > 13)
		return false;

	if (!CheckExistItem(ITEM_GENDER_CHANGE))
		return false;

	RobItem(ITEM_GENDER_CHANGE);

	m_bRace = Race;

	g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);

	if (GetHealth() < (GetMaxHealth() / 2))
		HpChange(GetMaxHealth());

	SendMyInfo();

	UserInOut(INOUT_OUT);
	SetRegion(GetNewRegionX(), GetNewRegionZ());
	UserInOut(INOUT_WARP);

	g_pMain->UserInOutForMe(this);
	g_pMain->NpcInOutForMe(this);
	g_pMain->MerchantUserInOutForMe(this);

	ResetWindows();

	InitType4();
	RecastSavedMagic();

	return true;
}
#pragma endregion

#pragma region Job Change

uint8 CUser::JobChange(uint8 NewJob /*= 0*/)
{
	uint8 bNewClass = 0, bNewRace = 0;
	uint8 bResult = 0;

	if (NewJob < 1 || NewJob > 4)
		return 5; // Unknown job is selected...

	if (!CheckExistItem(ITEM_JOB_CHANGE))
		return 6; // Job Change scroll is not found...

	for (int i = 0; i < SLOT_MAX; i++)
	{
		_ITEM_DATA* pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		if (pItem->nNum != 0)
		{
			bResult = 7;
			break;
		}
	}

	if (bResult == 7)
	{
		Packet result(WIZ_CLASS_CHANGE, uint8(ALL_POINT_CHANGE));
		result << uint8(4) << int(0);
		Send(&result);
		return bResult; // While there are items equipped on you.
	}

	// If bResult between 1 and 4 character already selected job...

	// If selected a new job Warrior
	if (NewJob == 1)
	{
		if (GetNation() == KARUS)
		{
			// Beginner Karus Rogue, Magician, Priest
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest())
			{
				bNewClass = KARUWARRIOR;
				bNewRace = KARUS_BIG;
			}
			// Skilled Karus Rogue, Magician, Priest
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest())
			{
				bNewClass = BERSERKER;
				bNewRace = KARUS_BIG;
			}
			// Mastered Karus Rogue, Magician, Priest
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest())
			{
				bNewClass = GUARDIAN;
				bNewRace = KARUS_BIG;
			}
		}
		else
		{
			// Beginner El Morad Rogue, Magician, Priest
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest())
			{
				bNewClass = ELMORWARRRIOR;
				bNewRace = GetRace();
			}
			// Skilled El Morad Rogue, Magician, Priest
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest())
			{
				bNewClass = BLADE;
				bNewRace = GetRace();
			}
			// Mastered El Morad Rogue, Magician, Priest
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest())
			{
				bNewClass = PROTECTOR;
				bNewRace = GetRace();
			}
		}

		// Character already Warrior.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Rogue
	if (NewJob == 2)
	{
		if (GetNation() == KARUS)
		{
			// Beginner Karus Warrior, Magician, Priest
			if (isBeginnerWarrior() || isBeginnerMage() || isBeginnerPriest())
			{
				bNewClass = KARUROGUE;
				bNewRace = KARUS_MIDDLE;
			}
			// Skilled Karus Warrior, Magician, Priest
			else if (isNoviceWarrior() || isNoviceMage() || isNovicePriest())
			{
				bNewClass = HUNTER;
				bNewRace = KARUS_MIDDLE;
			}
			// Mastered Karus Warrior, Magician, Priest
			else if (isMasteredWarrior() || isMasteredMage() || isMasteredPriest())
			{
				bNewClass = PENETRATOR;
				bNewRace = KARUS_MIDDLE;
			}
		}
		else
		{
			// Beginner El Morad Warrior, Magician, Priest
			if (isBeginnerWarrior() || isBeginnerMage() || isBeginnerPriest())
			{
				bNewClass = ELMOROGUE;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Magician, Priest
			else if (isNoviceWarrior() || isNoviceMage() || isNovicePriest())
			{
				bNewClass = RANGER;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Magician, Priest
			else if (isMasteredWarrior() || isMasteredMage() || isMasteredPriest())
			{
				bNewClass = ASSASSIN;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Rogue.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Magician
	if (NewJob == 3)
	{
		if (GetNation() == KARUS)
		{
			// Beginner Karus Warrior, Rogue, Priest
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerPriest())
			{
				bNewClass = KARUWIZARD;

				if (GetRace() == KARUS_BIG 
					|| GetRace() == KARUS_MIDDLE)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
			// Skilled Karus Warrior, Rogue, Priest
			else if (isNoviceWarrior() || isNoviceRogue() || isNovicePriest())
			{
				bNewClass = SORSERER;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_MIDDLE)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
			// Mastered Karus Warrior, Rogue, Priest
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredPriest())
			{
				bNewClass = NECROMANCER;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_MIDDLE)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
		}
		else
		{
			// Beginner El Morad Warrior, Rogue, Priest
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerPriest())
			{
				bNewClass = ELMOWIZARD;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Rogue, Priest
			else if (isNoviceWarrior() || isNoviceRogue() || isNovicePriest())
			{
				bNewClass = MAGE;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Rogue, Priest
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredPriest())
			{
				bNewClass = ENCHANTER;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Magician.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Priest
	if (NewJob == 4)
	{
		if (GetNation() == KARUS)
		{
			// Beginner Karus Warrior, Rogue, Magician
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerMage())
			{
				bNewClass = KARUPRIEST;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
			// Skilled Karus Warrior, Rogue, Magician
			else if (isNoviceWarrior() || isNoviceRogue() || isNoviceMage())
			{
				bNewClass = SHAMAN;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
			// Mastered Karus Warrior, Rogue, Magician
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredMage())
			{
				bNewClass = DARKPRIEST;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
		}
		else
		{
			// Beginner El Morad Warrior, Rogue, Magician
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerMage())
			{
				bNewClass = ELMOPRIEST;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Rogue, Magician
			else if (isNoviceWarrior() || isNoviceRogue() || isNoviceMage())
			{
				bNewClass = CLERIC;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Rogue, Magician
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredMage())
			{
				bNewClass = DRUID;

				if (GetRace() == BABARIAN)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Priest.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	if (bResult == 0)
	{
		RobItem(ITEM_JOB_CHANGE);

		m_sClass = bNewClass;
		m_bRace = bNewRace;

		// Reset stat and skill points...
		AllPointChange(true);
		AllSkillPointChange(true);

		g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);

		if (GetHealth() < (GetMaxHealth() / 2))
			HpChange(GetMaxHealth());

		SendMyInfo();

		UserInOut(INOUT_OUT);
		SetRegion(GetNewRegionX(), GetNewRegionZ());
		UserInOut(INOUT_WARP);

		g_pMain->UserInOutForMe(this);
		g_pMain->NpcInOutForMe(this);
		g_pMain->MerchantUserInOutForMe(this);

		ResetWindows();

		InitType4();
		RecastSavedMagic();

		foreach_stlmap(itr, g_pMain->m_LoadOpenQuestArray)
		{
			if (itr->second == nullptr)
				continue;

			if (itr->second->nEventDataIndex != 0)
				SaveEvent(itr->second->nEventDataIndex, 4);
		}
	}

	return bResult;
}
#pragma endregion