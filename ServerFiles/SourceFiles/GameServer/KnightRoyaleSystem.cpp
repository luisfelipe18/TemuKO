#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;
extern CDBAgent g_DBAgent;
using namespace std;

#pragma region CGameServerDlg::KnightRoyaleGasTimer()
void CGameServerDlg::KnightRoyaleGasTimer()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	if (pKnightRoyaleInfo->m_KnightRoyaleGasTimer > 0)
		pKnightRoyaleInfo->m_KnightRoyaleGasTimer--;

	if (pKnightRoyaleInfo->m_KnightRoyaleGasTimer <= 0)
		pKnightRoyaleInfo->m_KnightRoyaleSendGasArea = true;

	if (pKnightRoyaleInfo->m_KnightRoyaleGasTimer <= 0
		&& pKnightRoyaleInfo->m_KnightRoyaleSendGasArea == true)
	{
		pKnightRoyaleInfo->m_KnightRoyaleSendGasArea = false;
		KnightRoyaleGasStepSendPacket();
	}
}
#pragma endregion

#pragma region CUser::KnightRoyalAlan
bool CUser::KnightRoyalAlan1_1()
{
	if (GetX() >= 4 && GetX() <= 1019 && GetZ() >= 4 && GetZ() <= 1019)
		return true;

	return false;
}

bool CUser::KnightRoyalAlan2_1()
{
	if (GetX() >= 249 && GetX() <= 773 && GetZ() >= 243 && GetZ() <= 790)
		return true;

	return false;
}

bool CUser::KnightRoyalAlan3_1()
{
	if (GetX() >= 335 && GetX() <= 704 && GetZ() >= 327 && GetZ() <= 708)
		return true;

	return false;
}

bool CUser::KnightRoyalAlan4_1()
{
	if (GetX() >= 394 && GetX() <= 621 && GetZ() >= 410 && GetZ() <= 615)
		return true;

	return false;
}

bool CUser::KnightRoyalAlan5_1()
{
	if (GetX() >= 455 && GetX() <= 555 && GetZ() >= 479 && GetZ() <= 540)
		return true;

	return false;
}
#pragma endregion

#pragma region CUser::KnightRoyaleGasinDamage()
void CUser::KnightRoyaleGasinDamage()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;

	switch (pKnightRoyaleInfo->m_KnightRoyaleGasStepCount)
	{
	case 1:
		if (KnightRoyalAlan1_1()
			&& !KnightRoyalAlan2_1()
			&& !KnightRoyalAlan3_1()
			&& !KnightRoyalAlan4_1()
			&& !KnightRoyalAlan5_1())
		{
			if (!isDead())
			{
				HpChange(-1);
			}
		}
		break;
	case 2:
		if ((KnightRoyalAlan1_1()
			|| KnightRoyalAlan2_1())
			&& (!KnightRoyalAlan3_1()
				&& !KnightRoyalAlan4_1()
				&& !KnightRoyalAlan5_1()))
		{
			if (!isDead())
			{
				HpChange(-2);
			}
		}
		break;
	case 3:
		if ((KnightRoyalAlan1_1()
			|| KnightRoyalAlan2_1()
			|| KnightRoyalAlan3_1())
			&& (!KnightRoyalAlan4_1()
				&& !KnightRoyalAlan5_1()))
		{
			if (!isDead())
			{
				HpChange(-3);
			}
		}
		break;
	case 4:
		if ((KnightRoyalAlan1_1()
			|| KnightRoyalAlan2_1()
			|| KnightRoyalAlan3_1()
			|| KnightRoyalAlan4_1())
			&& !KnightRoyalAlan5_1())
		{
			if (!isDead())
			{
				HpChange(-4);
			}
		}
		break;
	case 5:
		if (KnightRoyalAlan1_1()
			|| KnightRoyalAlan2_1()
			|| KnightRoyalAlan3_1()
			|| KnightRoyalAlan4_1()
			|| KnightRoyalAlan5_1())
		{
			if (!isDead())
			{
				HpChange(-5);
			}
		}
		break;
	}
}
#pragma endregion

#pragma region CUser::KnightRoyaleAreaControl()
void CUser::KnightRoyaleAreaControl()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;

	bool Gas = false;
	switch (pKnightRoyaleInfo->m_KnightRoyaleGasStepCount)
	{
	case 1:
		if (KnightRoyalAlan1_1())
		{
			if (!KnightRoyalAlan2_1() && !KnightRoyalAlan3_1() && !KnightRoyalAlan4_1() && !KnightRoyalAlan5_1())
				Gas = true;
		}
		break;
	case 2:
		if (KnightRoyalAlan1_1() || KnightRoyalAlan2_1())
		{
			if (!KnightRoyalAlan3_1() && !KnightRoyalAlan4_1() && !KnightRoyalAlan5_1())
				Gas = true;
		}
		break;
	case 3:
		if (KnightRoyalAlan1_1() || KnightRoyalAlan2_1() || KnightRoyalAlan3_1())
		{
			if (!KnightRoyalAlan4_1() && !KnightRoyalAlan5_1())
				Gas = true;
		}
		break;
	case 4:
		if (KnightRoyalAlan1_1() || KnightRoyalAlan2_1() || KnightRoyalAlan3_1() || KnightRoyalAlan4_1())
		{
			if (!KnightRoyalAlan5_1())
				Gas = true;
		}
		break;
	case 5:
		if (KnightRoyalAlan1_1() || KnightRoyalAlan2_1() || KnightRoyalAlan3_1() || KnightRoyalAlan4_1() || KnightRoyalAlan5_1())
			Gas = true;
		break;
	}
	KnightRoyaleSendGas(Gas);
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleSendGas(bool Gas)
void CUser::KnightRoyaleSendGas(bool Gas)
{
	if (Gas)
	{
		if (!m_isRoyalPoison)
			SendUserStatusUpdate(USER_STATUS_BLIND, USER_STATUS_INFLICT);
	}
	else
	{
		m_isRoyalPoison = false;
		SendUserStatusUpdate(USER_STATUS_DOT, USER_STATUS_CURE);
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleTreasureChestSpawnTimer()
void CGameServerDlg::KnightRoyaleTreasureChestSpawnTimer()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	for (int i = 1; i <= 5; i++)
	{
		_KNIGHT_ROYALE_TREASURE_CHEST_LIST* pChest = m_KnightRoyaleTreasureChestListArray.GetData(i);
		if (pChest == nullptr
			|| pChest->TresureisDead == true)
			continue;

		if (pChest->TreasureDeadSpawnTimer > 0)
			pChest->TreasureDeadSpawnTimer--;


		uint8 ID = pChest->TreasureChestID;
		if (pChest->TreasureDeadSpawnTimer <= 0
			&& pChest->TresureisDead == false)
		{
			pChest->TresureisDead = true;
			KnightRoyaleTreasureChestSpawnPacket(ID);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleStart()
void CGameServerDlg::KnightRoyaleStart()
{
	if (pKnightRoyaleEvent.ActiveEvent != -1)
	{
		Packet result(WIZ_KNIGHT_ROYALE, uint8(1));
		result << uint8(1)
			<< uint8(0) //number
			<< uint16(m_nKnightRoyaleEventRemainSeconds);
		Send_All(&result);
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleGetActiveEventTime(CUser *pUser)
void CGameServerDlg::KnightRoyaleGetActiveEventTime(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	if (pKnightRoyaleEvent.ActiveEvent != -1)
	{
		int16 KnightRoyaleActiveEvent = g_pMain->pKnightRoyaleEvent.ActiveEvent;
		int16 OtherActiveEvent = g_pMain->pTempleEvent.ActiveEvent;

		if (KnightRoyaleActiveEvent == -1
			|| OtherActiveEvent > 0)
			return;

		Packet result(WIZ_KNIGHT_ROYALE, uint8(1));
		result << uint8(1) << uint8(pKnightRoyaleEvent.AllUserCount) << uint16(m_nKnightRoyaleEventRemainSeconds);
		pUser->Send(&result);
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleEventManage()
void CGameServerDlg::KnightRoyaleEventManage()
{
	uint8 nMaxUserCount = 80, GasAreBeginnerStep = 0;
	std::queue<std::string> k_KnightRoyaleuserContainer;

	if (pKnightRoyaleEvent.ActiveEvent != -1)
	{
		_KNIGHT_ROYALE_INFO* pRoomInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
		if (pRoomInfo == nullptr)
			return;

		int UserCounter = 0;
		foreach_stlmap(itr, m_KnightRoyaleEventUserMap)
		{
			_KNIGHT_ROYALE_EVENT_USER* pRoyaleUser = itr->second;
			if (pRoyaleUser == nullptr)
				continue;

			CUser* pUser = g_pMain->GetUserPtr(pRoyaleUser->KnightRoyaleStrUserID, TYPE_CHARACTER);
			if (pUser == nullptr
				|| !pUser->isInGame()
				|| pUser->GetZoneID() == ZONE_KNIGHT_ROYALE)
				continue;

			UserCounter++;
			std::string strUserID = pRoyaleUser->KnightRoyaleStrUserID;
			k_KnightRoyaleuserContainer.push(strUserID);
		}

		pRoomInfo->m_KnightRoyaleStartedUserCount = UserCounter;
		pRoomInfo->m_KnightRoyaleRemaingUserCount = UserCounter;

		if (UserCounter < 40)
			pRoomInfo->m_KnightRoyaleGasStepCount = 2;
		else if (UserCounter >= 40)
			pRoomInfo->m_KnightRoyaleGasStepCount = 0;

		for (int x = 1; x <= nMaxUserCount; x++)
		{
			if (k_KnightRoyaleuserContainer.empty())
				break;

			std::string strUserID = k_KnightRoyaleuserContainer.front();
			k_KnightRoyaleuserContainer.pop();

			_KNIGHT_ROYALE_STARTED_USER* pEventUser = new  _KNIGHT_ROYALE_STARTED_USER;
			pEventUser->KnightRoyaleStrUserID = strUserID;
			pEventUser->KnightRoyaleisDead = false;
			pEventUser->KnightRoyaleinWinner = false;
			pEventUser->KnightRoyaleisExit = false;
			pEventUser->KnightRoyaleKillCount = 0;
			pEventUser->KnightRoyaleLevel = 1;
			pEventUser->KnightRoyaleTotalExp = 50;
			pEventUser->KnightRoyaleUserExp = 0;
			if (!pRoomInfo->m_KnightRoyaleUserList.PutData(pEventUser->KnightRoyaleStrUserID, pEventUser))
			{
				delete pEventUser;
				continue;
			}
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleGetExpByLevel(int nLevel)
uint16 CGameServerDlg::KnightRoyaleGetExpByLevel(int nLevel)
{
	_KNIGHT_ROYALE_STATS_LIST* pStats = m_KnightRoayleStatsListArray.GetData(nLevel);
	if (pStats == nullptr)
		return 0;

	foreach_stlmap(itr, g_pMain->m_KnightRoayleStatsListArray)
	{
		if (itr->second == nullptr)
			continue;

		if (itr->second->KnightRoyaleStatsLevel == nLevel)
			return itr->second->KnightRoyaleStatsRequiredExperience;
	}

	return 0;
}
#pragma endregion

#pragma region CUser::KnightRoyaleLevelChange(uint8 level, bool bLevelUp)
void CUser::KnightRoyaleLevelChange(uint8 level, bool bLevelUp)
{
	_KNIGHT_ROYALE_INFO* pRoomInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pRoomInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pSurvivorUser = pRoomInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pSurvivorUser == nullptr)
		return;

	_KNIGHT_ROYALE_STATS_LIST* pSurvivorStats = g_pMain->m_KnightRoayleStatsListArray.GetData(level);
	if (pSurvivorStats == nullptr)
		return;

	if (level < 1 || level > 10)
		return;

	if (bLevelUp && level > pSurvivorUser->KnightRoyaleLevel + 1)
		pSurvivorUser->KnightRoyaleLevel = level;

	uint16 SurvivorExp = 0;
	SurvivorExp = g_pMain->KnightRoyaleGetExpByLevel(pSurvivorUser->KnightRoyaleLevel);

	SetUserAbility();
	m_sMp = pSurvivorStats->KnightRoyaleStatiMaxMp;
	HpChange(pSurvivorStats->KnightRoyaleStatiMaxHp);

	Packet result(WIZ_KNIGHT_ROYALE);
	result << uint8(3) << uint8(2)
		<< GetSocketID()
		<< pSurvivorUser->KnightRoyaleLevel//->level
		<< SurvivorExp //max exp
		<< pSurvivorUser->KnightRoyaleUserExp //kastýðý exp
		<< pSurvivorStats->KnightRoyaleStatiMaxHp //maxp hp
		<< m_sHp
		<< pSurvivorStats->KnightRoyaleStatiMaxMp //max hp
		<< m_sMp;
	g_pMain->Send_Region(&result, GetMap(), GetRegionX(), GetRegionZ(), nullptr, GetEventRoom());
}
#pragma endregion

#pragma region CUser::KnightRoyaleExpChange(uint16 iExp)
void CUser::KnightRoyaleExpChange(uint16 iExp)
{
	_KNIGHT_ROYALE_INFO* pRoomInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pRoomInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pSurvivorUser = pRoomInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pSurvivorUser == nullptr)
		return;

	bool bLevel = true;

	if (iExp < 0 && (pSurvivorUser->KnightRoyaleUserExp + iExp) < 0)
		bLevel = false;

	pSurvivorUser->KnightRoyaleUserExp += iExp;

	uint16 SurvivorExp = 0;
	SurvivorExp = g_pMain->KnightRoyaleGetExpByLevel(pSurvivorUser->KnightRoyaleLevel);

	if (!bLevel)
	{
		// Drop us back a level.
		pSurvivorUser->KnightRoyaleLevel--;

		// Get the excess XP (i.e. below 0), so that we can take it off the max XP of the previous level
		// Remember: we're deleveling, not necessarily starting from scratch at the previous level
		uint16 diffXP = pSurvivorUser->KnightRoyaleUserExp + iExp;

		// Now reset our XP to max for the former level.
		pSurvivorUser->KnightRoyaleUserExp = SurvivorExp;

		// Get new stats etc.
		KnightRoyaleLevelChange(pSurvivorUser->KnightRoyaleLevel, false);

		// Take the remainder of the XP off (and delevel again if necessary).
		KnightRoyaleExpChange(diffXP);
		return;
	}
	else if (pSurvivorUser->KnightRoyaleUserExp >= SurvivorExp)
	{
		if (pSurvivorUser->KnightRoyaleLevel <= 10)
		{
			// Reset our XP, level us up.
			pSurvivorUser->KnightRoyaleUserExp -= SurvivorExp;
			KnightRoyaleLevelChange(++pSurvivorUser->KnightRoyaleLevel);
			return;
		}

		// Hit the max level? Can't level any further. Cap the XP.
		pSurvivorUser->KnightRoyaleUserExp = SurvivorExp;
	}

	Packet result(WIZ_KNIGHT_ROYALE);
	result << uint8(3) << uint8(1) << pSurvivorUser->KnightRoyaleUserExp;
	Send(&result);
}
#pragma endregion

#pragma region CUser::KnightRoyaleLoginSendPacket()
void CUser::KnightRoyaleLoginSendPacket()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pRoyaleUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pRoyaleUser == nullptr)
		return;

#define BEGINNER_MAX_ITEM 13

	uint8 GasArea = 0;
	Packet result;
#pragma region Royale Survivor Count
	result.Initialize(WIZ_KNIGHT_ROYALE);
	result << uint8(1) << uint8(1)
		<< pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount;
	Send(&result);
#pragma endregion

#pragma region Royale Info User Panel
	result.clear();
	result.Initialize(WIZ_KNIGHT_ROYALE);
	result << uint8(2) << uint8(1)
		<< pRoyaleUser->KnightRoyaleLevel
		<< pRoyaleUser->KnightRoyaleTotalExp
		<< pRoyaleUser->KnightRoyaleUserExp;
	Send(&result);
#pragma endregion

#pragma region Royale Item Add
	for (int i = 1; i <= BEGINNER_MAX_ITEM; i++)
	{
		_KNIGHT_ROYALE_BEGINNER_ITEM* pRoyaleItem = g_pMain->m_KnightRoyaleBeginnerItemListArray.GetData(i);
		if (pRoyaleItem == nullptr
			|| pRoyaleItem->nItemID == 0)
			continue;

		_ITEM_TABLE* pTable = g_pMain->GetItemPtr(pRoyaleItem->nItemID);
		if (pTable == nullptr)
			continue;

		_ITEM_DATA* pItem = GetKnightRoyaleItem(INVENTORY_INVENT + pRoyaleItem->ItemPos);
		pItem->nNum = pRoyaleItem->nItemID;
		pItem->sDuration = pRoyaleItem->nDurability;
		pItem->sCount = pRoyaleItem->nCount;
		pItem->nSerialNum = g_pMain->GenerateItemSerial();
		pItem->nExpirationTime = 0;
		pItem->bFlag = pRoyaleItem->nFlag;
		pItem->sRemainingRentalTime = 0;

		/*
		UNKNOWN -> B7 06 00 40CE7837 01 0100 00
		UNKNOWN -> B7 06 01 F8FC8737 01 8813 00
		UNKNOWN -> B7 06 02 E0008837 01 8813 00
		UNKNOWN -> B7 06 03 D0564E17 0F 0100 00
		UNKNOWN -> B7 06 04 B0088837 1E 0100 00
		UNKNOWN -> B7 06 05 80108837 1E 0100 00
		*/

		result.clear();
		result.Initialize(WIZ_KNIGHT_ROYALE);
		result << uint8(6)
			<< pRoyaleItem->ItemPos
			<< pItem->nNum
			<< pItem->sCount
			<< pItem->sDuration
			<< pItem->bFlag;
		Send(&result);
	}
#pragma endregion

#pragma region Gas Info
	result.clear();
	result.Initialize(WIZ_KNIGHT_ROYALE);
	result << uint8(4)
		<< uint8(2)
		<< pKnightRoyaleInfo->m_KnightRoyaleGasStepCount
		<< uint8(3); /*Time 1:60 2:120 3:180 */
	Send(&result);

	pKnightRoyaleInfo->m_KnightRoyaleGasTimer = 180;
#pragma endregion
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleUserTeleport()
void CGameServerDlg::KnightRoyaleUserTeleport()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr)
		return;

	if (pKnightRoyaleEvent.ActiveEvent != -1)
	{
		foreach_stlmap(itrUser, pKnightRoyaleInfo->m_KnightRoyaleUserList)
		{
			_KNIGHT_ROYALE_STARTED_USER* pKnightRoyaleUser = itrUser->second;
			if (pKnightRoyaleUser == nullptr)
				continue;

			CUser* pLoginUser = GetUserPtr(pKnightRoyaleUser->KnightRoyaleStrUserID, TYPE_CHARACTER);
			if (pLoginUser == nullptr
				|| !pLoginUser->isInGame()
				|| pLoginUser->isDead())
				continue;

			pLoginUser->UserInOut(INOUT_OUT);
			pLoginUser->ZoneChange(ZONE_KNIGHT_ROYALE, 0.0f, 0.0f);
			pLoginUser->ResetKnightRoyaleUserRegeneData();
			pLoginUser->KnightRoyaleLoginSendPacket();
			pLoginUser->SetUserAbility();
			pLoginUser->KnightRoyaleHealtMagicChange();
		}
	}
}
#pragma endregion

#pragma region CUser::KnightRoyaleHealtMagicChange()

void CUser::KnightRoyaleHealtMagicChange()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;


	_KNIGHT_ROYALE_STARTED_USER* pSurvivor = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pSurvivor == nullptr)
		return;

	_KNIGHT_ROYALE_STATS_LIST* pRoyaleStats = g_pMain->m_KnightRoayleStatsListArray.GetData(pSurvivor->KnightRoyaleLevel);
	if (pRoyaleStats == nullptr)
		return;


	if (pRoyaleStats->KnightRoyaleStatiMaxHp > 0)
	{
		m_MaxHp = (short)pRoyaleStats->KnightRoyaleStatiMaxHp;
		HpChange(pRoyaleStats->KnightRoyaleStatiMaxHp);
	}

	if (pRoyaleStats->KnightRoyaleStatiMaxMp > 0)
	{
		m_MaxMp = (short)pRoyaleStats->KnightRoyaleStatiMaxMp;
		MSpChange(pRoyaleStats->KnightRoyaleStatiMaxMp);
	}
}

#pragma endregion

#pragma region CUser::KnightRoyaleisKillerProcess()
void CUser::KnightRoyaleisKillerProcess()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pKillerSurvivorUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pKillerSurvivorUser != nullptr)
		pKillerSurvivorUser->KnightRoyaleKillCount++;
	else
	{
		ResetKnightRoyaleUserRegeneData();
		ResetKnightRoyaleEventData();
		ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
	}
}
#pragma endregion

#pragma region CUser::KnightRoyaleisWinner()
void CUser::KnightRoyaleisWinner()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pWinnerSurvivorUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pWinnerSurvivorUser == nullptr)
		return;

	uint8 KillTotalRewardGems = 0;
	uint16 RankTotalRewardGems = 0;

	KillTotalRewardGems = (pWinnerSurvivorUser->KnightRoyaleKillCount + 1);
	RankTotalRewardGems = 60;

	GiveItemKnightRoyalReward(g_pMain->m_sKnightRoyalWinner, KillTotalRewardGems + RankTotalRewardGems);

	/*Reward Notice*/
	Packet result;
	result.Initialize(WIZ_KNIGHT_ROYALE);
	result << uint8(4)
		<< uint8(4)
		<< pKnightRoyaleInfo->m_KnightRoyaleStartedUserCount
		<< pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount
		<< KillTotalRewardGems
		<< RankTotalRewardGems;
	Send(&result);

	result.Initialize(WIZ_KNIGHT_ROYALE);
	result.SByte();
	result << uint8(4) << uint8(5) << GetName();
	g_pMain->Send_All(&result);
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleGetRankReward()
uint16 CGameServerDlg::KnightRoyaleGetRankReward()
{
	uint16 RankTotalRewardGems = 1;
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr)
		return RankTotalRewardGems;

	if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount == 1)
		RankTotalRewardGems = 60;
	else if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount == 2)
		RankTotalRewardGems = 40;
	else if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount == 3)
		RankTotalRewardGems = 35;
	else if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount >= 4
		&& pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount <= 15)
		RankTotalRewardGems = 25;
	else if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount >= 16
		&& pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount <= 50)
		RankTotalRewardGems = 20;
	else if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount >= 51
		&& pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount <= 80)
		RankTotalRewardGems = 10;
	else
		RankTotalRewardGems = 1;

	return RankTotalRewardGems;
}
#pragma endregion

#pragma region CUser::KnightRoyaleRegeneProcess()
void CUser::KnightRoyaleRegeneProcess()
{
	ResetKnightRoyaleEventData();
	ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
	KnightRoyaleLoqOutPacket();
	SetUserAbility();
	GiveItemKnightRoyalReward(g_pMain->m_sKnightRoyalWinner, m_RoyaleForUserKillRewarCount + m_RoyaleForUserRankRewarCount);

	/*Reward Notice*/
	Packet result;
	result.Initialize(WIZ_KNIGHT_ROYALE);
	result << uint8(4)
		<< uint8(4)
		<< m_RoyaleForUserRegeneStartedCount
		<< m_RoyaleForUserRegeneRemainingCount
		<< m_RoyaleForUserKillRewarCount
		<< m_RoyaleForUserRankRewarCount;
	Send(&result);

	ResetKnightRoyaleUserRegeneData();
}
#pragma endregion

#pragma region CUser::KnightRoyaleisDeathProcess()
void CUser::KnightRoyaleisDeathProcess()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	_KNIGHT_ROYALE_STARTED_USER* pDeadSurvivorUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pKnightRoyaleInfo == nullptr || pDeadSurvivorUser == nullptr)
	{
		ResetKnightRoyaleEventData();
		return;
	}

	m_RoyaleForUserRegeneStartedCount = pKnightRoyaleInfo->m_KnightRoyaleStartedUserCount;
	m_RoyaleForUserRegeneRemainingCount = pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount;
	m_RoyaleForUserKillRewarCount = pDeadSurvivorUser->KnightRoyaleKillCount + 1;
	m_RoyaleForUserRankRewarCount = g_pMain->KnightRoyaleGetRankReward();

	pDeadSurvivorUser->KnightRoyaleisDead = true;
	if (pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount > 0)
		pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount--;

	/*Dead Notice*/
	Packet result(WIZ_KNIGHT_ROYALE);
	result.SByte();
	result << uint8(4) << uint8(1) << GetName();
	g_pMain->Send_Zone(&result, ZONE_KNIGHT_ROYALE);

	KnightRoyaleDeathChest();
	g_pMain->KnightRoyaleFinishCheck();
}
#pragma endregion

#pragma region CUser::GiveItemKnightRoyalReward(uint32 itemid, uint16 count, uint32 Time)
bool CUser::GiveItemKnightRoyalReward(uint32 itemid, uint16 count, uint32 Time)
{
	int8 pos;
	bool bNewItem = true;
	_ITEM_TABLE* pTable = g_pMain->GetItemPtr(itemid);
	_ITEM_DATA* pItem = nullptr;

	if (pTable == nullptr)
		return false;

	pos = FindSlotForItem(itemid, count);
	if (pos < 0)
		return false;

	pItem = GetItem(pos);

	if (pItem->nNum != 0 || pItem == nullptr)
		bNewItem = false;

	if (bNewItem)
		pItem->nSerialNum = g_pMain->GenerateItemSerial();

	pItem->nNum = itemid;
	pItem->sCount += count;
	pItem->sDuration = pTable->m_sDuration;

	if (pItem->sCount > MAX_ITEM_COUNT)
		pItem->sCount = MAX_ITEM_COUNT;

	if (Time != 0)
		pItem->nExpirationTime = int32(UNIXTIME) + ((60 * 60 * 24) * Time);
	else
		pItem->nExpirationTime = 0;

	SendStackChange(itemid, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true, pItem->nExpirationTime);

	DateTime time;
	g_pMain->WriteItemTransactionLogFile(string_format("%d:%d:%d || ZoneID = %d(%d,%d),UserID=%s,Task=GiveItem,ItemName=%s,ItemID=%d,Count=%d\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), GetZoneID(), uint16(GetX()), uint16(GetZ()), GetName().c_str(), pTable->m_sName.c_str(), pTable->m_iNum, count));

	return true;
}
#pragma endregion

#pragma region CUser::KnightRoyaleDeathChest()
void CUser::KnightRoyaleDeathChest()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr
		|| GetZoneID() != ZONE_KNIGHT_ROYALE)
		return;

	_KNIGHT_ROYALE_STARTED_USER* pDeadSurvivorUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
	if (pDeadSurvivorUser == nullptr)
		return;

	g_pMain->SpawnEventNpc(9994, true, ZONE_KNIGHT_ROYALE, GetX(), 0, GetZ(), 1, 0, 10 * MINUTE, 0, -1, 0, 0, 0, 0);
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleFinishCheck()
void CGameServerDlg::KnightRoyaleFinishCheck()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr)
		return;

	uint16 RoyaleUserCount = 0;
	foreach_stlmap(RoyaleUser, pKnightRoyaleInfo->m_KnightRoyaleUserList)
	{
		_KNIGHT_ROYALE_STARTED_USER* pKnightRoyaleUser = RoyaleUser->second;
		if (pKnightRoyaleUser == nullptr
			|| pKnightRoyaleUser->KnightRoyaleisDead == true
			|| pKnightRoyaleUser->KnightRoyaleisExit == true
			|| pKnightRoyaleUser->KnightRoyaleinWinner == true)
			continue;

		CUser* pSurvivorUser = GetUserPtr(pKnightRoyaleUser->KnightRoyaleStrUserID, TYPE_CHARACTER);
		if (pSurvivorUser == nullptr
			|| !pSurvivorUser->isInGame()
			|| pSurvivorUser->GetZoneID() != ZONE_KNIGHT_ROYALE)
			continue;

		RoyaleUserCount++;
	}

	if (RoyaleUserCount > 1)
		return;

	if (RoyaleUserCount <= 1)
	{
		pKnightRoyaleInfo->m_KnightRoyaleUserList.m_lock.lock();
		foreach_stlmap(RoyaleUser, pKnightRoyaleInfo->m_KnightRoyaleUserList)
		{
			_KNIGHT_ROYALE_STARTED_USER* pKnightRoyaleUser = RoyaleUser->second;
			if (pKnightRoyaleUser == nullptr
				|| pKnightRoyaleUser->KnightRoyaleisDead == true
				|| pKnightRoyaleUser->KnightRoyaleisExit == true
				|| pKnightRoyaleUser->KnightRoyaleinWinner == true)
				continue;

			CUser* pSurvivorUser = GetUserPtr(pKnightRoyaleUser->KnightRoyaleStrUserID, TYPE_CHARACTER);
			if (pSurvivorUser == nullptr
				|| !pSurvivorUser->isInGame()
				|| pSurvivorUser->GetZoneID() != ZONE_KNIGHT_ROYALE)
				continue;

			pSurvivorUser->KnightRoyaleisWinner();
		}
		pKnightRoyaleInfo->m_KnightRoyaleUserList.m_lock.unlock();
	}

	KnightRoyaleClose();
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleClose()
void CGameServerDlg::KnightRoyaleClose()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr
		|| pKnightRoyaleInfo->m_KnightRoyaleisFinished == true)
		return;

	pKnightRoyaleInfo->m_KnightRoyaleUserList.m_lock.lock();
	foreach_stlmap(RoyaleUser, pKnightRoyaleInfo->m_KnightRoyaleUserList)
	{
		_KNIGHT_ROYALE_STARTED_USER* pKnightRoyaleUser = RoyaleUser->second;
		if (pKnightRoyaleUser == nullptr)
			continue;

		CUser* pSurvivorUser = GetUserPtr(pKnightRoyaleUser->KnightRoyaleStrUserID, TYPE_CHARACTER);
		if (pSurvivorUser == nullptr
			|| !pSurvivorUser->isInGame()
			|| pSurvivorUser->isDead()
			|| pSurvivorUser->GetZoneID() != ZONE_KNIGHT_ROYALE)
			continue;

		pSurvivorUser->ResetKnightRoyaleEventData();
		pSurvivorUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
		pSurvivorUser->KnightRoyaleLoqOutPacket();
		pSurvivorUser->SetUserAbility();
	}
	pKnightRoyaleInfo->m_KnightRoyaleUserList.m_lock.unlock();

	pKnightRoyaleInfo->m_KnightRoyaleisFinished = true;
	KnightRoyaleTotalReset();
}
#pragma endregion

#pragma region CUser::KnightRoyaleLoqOutPacket()
void CUser::KnightRoyaleLoqOutPacket()
{
	Packet result(WIZ_KNIGHT_ROYALE, uint8(7));
	result << GetLevel() << m_iMaxExp << m_iExp;

	for (int i = 0; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA* pItem = GetItem(i);
		if (pItem == nullptr)
			continue;

		result << pItem->nNum
			<< pItem->sDuration
			<< pItem->sCount
			<< pItem->bFlag
			<< pItem->sRemainingRentalTime;	// remaining time

		_ITEM_TABLE* pItemTable = g_pMain->GetItemPtr(pItem->nNum);
		if (pItemTable != nullptr)
		{
			if (pItemTable->isPetItem())
				ShowPetItemInfo(result, pItem->nSerialNum);
			else if (pItemTable->GetNum() == ITEM_CYPHER_RING)
				ShowCyperRingItemInfo(result, pItem->nSerialNum);
			else
				result << uint32(0); // Item Unique ID
		}
		else
			result << uint32(0); // Item Unique ID*/

		result << pItem->nExpirationTime;
	}
	Send(&result);
}
#pragma endregion

#pragma region CDBAgent::LoadKnightRoyaleChestList()
bool CDBAgent::LoadKnightRoyaleChestList()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_KNIGHT_ROYALE_CHEST_LIST}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		_KNIGHT_ROYALE_TREASURE_CHEST_LIST* pData = new _KNIGHT_ROYALE_TREASURE_CHEST_LIST;
		dbCommand->FetchByte(1, pData->TreasureChestID);
		dbCommand->FetchUInt16(2, pData->TreasureChestSsid);
		dbCommand->FetchUInt16(3, pData->TreasureChestPosX);
		dbCommand->FetchUInt16(4, pData->TreasureChestPosZ);
		dbCommand->FetchUInt32(5, pData->TreasureSpawnTimer);

		pData->TreasureDeadSpawnTimer = pData->TreasureSpawnTimer;
		pData->TresureisDead = false;

		if (!g_pMain->m_KnightRoyaleTreasureChestListArray.PutData(pData->TreasureChestID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleTreasureChestSpawn()
void CGameServerDlg::KnightRoyaleTreasureChestSpawn()
{
	for (int i = 1; i <= 5; i++)
	{
		_KNIGHT_ROYALE_TREASURE_CHEST_LIST* pChest = m_KnightRoyaleTreasureChestListArray.GetData(i);
		if (pChest == nullptr)
			continue;

		pChest->TresureisDead = true;
		g_pMain->SpawnEventNpc(pChest->TreasureChestSsid, true, ZONE_KNIGHT_ROYALE, pChest->TreasureChestPosX, 0, pChest->TreasureChestPosZ, 1, 0, 0, 0, -1, 0, 0, 1, 0);
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleTreasureChestSpawnPacket(uint8 ID)
void CGameServerDlg::KnightRoyaleTreasureChestSpawnPacket(uint8 ID)
{
	_KNIGHT_ROYALE_TREASURE_CHEST_LIST* pChest = m_KnightRoyaleTreasureChestListArray.GetData(ID);
	if (pChest == nullptr)
		return;

	Packet result(WIZ_KNIGHT_ROYALE);
	result << uint8(4) << uint8(3) << pChest->TreasureChestPosX << pChest->TreasureChestPosZ;
	Send_Zone(&result, ZONE_KNIGHT_ROYALE);
	g_pMain->SpawnEventNpc(pChest->TreasureChestSsid, true, ZONE_KNIGHT_ROYALE, pChest->TreasureChestPosX, 0, pChest->TreasureChestPosZ, 1, 0, 0, 0, -1, 0, 0, 1, 0);
}
#pragma endregion

#pragma region CNpc::KnightRoyaleTreasureDeathProcess()
void CNpc::KnightRoyaleTreasureDeathProcess()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (!g_pMain->isKnightRoyaleActive()
		|| pKnightRoyaleInfo == nullptr)
		return;

	for (int i = 1; i <= 5; i++)
	{
		_KNIGHT_ROYALE_TREASURE_CHEST_LIST* pChest = g_pMain->m_KnightRoyaleTreasureChestListArray.GetData(i);
		if (pChest == nullptr)
			continue;

		if (pChest->TreasureChestSsid == GetProtoID())
		{
			pChest->TresureisDead = false;
			pChest->TreasureDeadSpawnTimer = pChest->TreasureSpawnTimer;
			break;
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleTotalReset()
void CGameServerDlg::KnightRoyaleTotalReset()
{
	foreach_stlmap(itr, m_KnightRoyaleRoomInfo)
	{
		if (itr->second == nullptr)
			continue;

		itr->second->Initialize();
	}

	g_pMain->m_KnightRoyaleEventUserMap.DeleteAllData();
	pKnightRoyaleEvent.ActiveEvent = -1;
	pKnightRoyaleEvent.AllowJoin = false;
	pKnightRoyaleEvent.AllUserCount = 0;
	pKnightRoyaleEvent.isActive = false;
	pKnightRoyaleEvent.EventTimerStartControl = false;
	pKnightRoyaleEvent.StartTime = 0;
	pKnightRoyaleEvent.EventCloseMainControl = false;
	pKnightRoyaleEvent.CloseTime = 0;
	m_KnightRoyaleLastUserRequestID = 1;
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleGasStepSendPacket()
void CGameServerDlg::KnightRoyaleGasStepSendPacket()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr)
		return;

	if (pKnightRoyaleEvent.ActiveEvent != -1)
	{
		if (pKnightRoyaleEvent.isActive == true)
		{
			if (pKnightRoyaleInfo->m_KnightRoyaleGasStepCount >= 5)
				return;

			pKnightRoyaleInfo->m_KnightRoyaleGasStepCount++;

			uint8 GasStepTimer = 0;

			if (pKnightRoyaleInfo->m_KnightRoyaleGasStepCount >= 0 && pKnightRoyaleInfo->m_KnightRoyaleGasStepCount <= 3)
				GasStepTimer = 3;
			else if (pKnightRoyaleInfo->m_KnightRoyaleGasStepCount >= 4)
				GasStepTimer = 5;
			else
				GasStepTimer = 3;

			Packet result(WIZ_KNIGHT_ROYALE);
			result << uint8(4) << uint8(2) << pKnightRoyaleInfo->m_KnightRoyaleGasStepCount << GasStepTimer;
			g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_KNIGHT_ROYALE, false, 0);

			if (GasStepTimer == 3)
				pKnightRoyaleInfo->m_KnightRoyaleGasTimer = 180;
			else if (GasStepTimer == 5)
				pKnightRoyaleInfo->m_KnightRoyaleGasTimer = 300;
			else
				pKnightRoyaleInfo->m_KnightRoyaleGasTimer = 180;
		}
	}
}
#pragma endregion

#pragma region CUser::KnightRoyaleSurvivorLogOut()
void CUser::KnightRoyaleSurvivorLogOut()
{
	_KNIGHT_ROYALE_INFO* pKnightRoyaleInfo = g_pMain->m_KnightRoyaleRoomInfo.GetData(1);
	if (pKnightRoyaleInfo == nullptr)
		return;

	if (g_pMain->pKnightRoyaleEvent.ActiveEvent != -1)
	{
		Packet result;

		if (g_pMain->pKnightRoyaleEvent.isActive == false
			&& g_pMain->pKnightRoyaleEvent.AllowJoin == true)
		{
			_KNIGHT_ROYALE_EVENT_USER* pKnightRoyaleEventUser = g_pMain->m_KnightRoyaleEventUserMap.GetData(m_KnightRoyaleJoinRequest);
			if (pKnightRoyaleEventUser != nullptr)
			{
				g_pMain->pKnightRoyaleEvent.AllUserCount--;
				g_pMain->m_KnightRoyaleEventUserMap.DeleteData(m_KnightRoyaleJoinRequest);

				result.Initialize(WIZ_KNIGHT_ROYALE);
				result << uint8(1) << uint8(1) << uint8(g_pMain->pKnightRoyaleEvent.AllUserCount) << uint16(g_pMain->m_nKnightRoyaleEventRemainSeconds);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, 0, false, 0);
			}
		}

		if (g_pMain->pKnightRoyaleEvent.isActive == true)
		{
			_KNIGHT_ROYALE_STARTED_USER* pLogOutUser = pKnightRoyaleInfo->m_KnightRoyaleUserList.GetData(GetName());
			if (pLogOutUser != nullptr)
			{
				if (pLogOutUser->KnightRoyaleisExit == false)
				{
					pLogOutUser->KnightRoyaleisExit = true;
					pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount--;

					result.clear();
					result.Initialize(WIZ_KNIGHT_ROYALE);
					result << uint8(1) << uint8(1) << uint8(pKnightRoyaleInfo->m_KnightRoyaleRemaingUserCount) << uint16(0);
					g_pMain->Send_Zone(&result, ZONE_KNIGHT_ROYALE);

					/*LogOut Notice*/
					result.clear();
					result.Initialize(WIZ_KNIGHT_ROYALE);
					result.SByte();
					result << uint8(4) << uint8(1) << GetName();
					g_pMain->Send_Zone(&result, ZONE_KNIGHT_ROYALE);
					g_pMain->KnightRoyaleFinishCheck();
				}
			}
		}
	}
}
#pragma endregion

#pragma region CUser::KnightRoyaleProcess(Packet & pkt)
void CUser::KnightRoyaleProcess(Packet& pkt)
{
	uint8 type, Pos;
	pkt >> type;

	switch (type)
	{
	case 1:
	{
		pkt >> Pos;
		int16 KnightRoyaleActiveEvent = g_pMain->pKnightRoyaleEvent.ActiveEvent;
		int16 OtherActiveEvent = g_pMain->pTempleEvent.ActiveEvent;

		if (KnightRoyaleActiveEvent == -1
			|| OtherActiveEvent > 0
			|| !g_pMain->pKnightRoyaleEvent.AllowJoin)
			return;

		switch (Pos)
		{
		case 1:
		{
			if (g_pMain->pKnightRoyaleEvent.AllUserCount < 80)
			{
				if (g_pMain->KnightRoyaleAddEventUser(this))
				{
					g_pMain->pKnightRoyaleEvent.AllUserCount++;
					m_KnightRoyaleJoinEvent = ZONE_KNIGHT_ROYALE;
					g_pMain->KnightRoyalSendJoinScreen();
				}
			}
			else
			{
				Packet result(WIZ_KNIGHT_ROYALE, uint8(1));
				result << uint8(4);
				Send(&result);
			}
		}
		break;
		case 2:
		{
			g_pMain->pKnightRoyaleEvent.AllUserCount--;
			g_pMain->KnightRoyaleRemoveEventUser(this);
			g_pMain->KnightRoyalSendJoinScreen();
		}
		break;
		}
	}
	break;
	case 5:
		ItemGetKnightRoyal(pkt);
		break;
	case 8:
		ItemRemoveKnightRoyal(pkt);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleAddEventUser(CUser *pUser)
bool CGameServerDlg::KnightRoyaleAddEventUser(CUser* pUser)
{
	if (pUser == nullptr || !pUser->isInGame())
	{
		TRACE("#### Knight Royale AddEventUser : pUser == nullptr ####\n");
		return false;
	}

	_KNIGHT_ROYALE_EVENT_USER* pKnightRoyaleEventUser = new _KNIGHT_ROYALE_EVENT_USER();
	pKnightRoyaleEventUser->KnightRoyaleStrUserID = pUser->GetName();
	pKnightRoyaleEventUser->m_KnightRoyaleJoinRequest = m_KnightRoyaleLastUserRequestID++;

	if (!g_pMain->m_KnightRoyaleEventUserMap.PutData(pKnightRoyaleEventUser->m_KnightRoyaleJoinRequest, pKnightRoyaleEventUser))
	{
		delete pKnightRoyaleEventUser;
		return false;
	}

	pUser->m_KnightRoyaleJoinRequest = pKnightRoyaleEventUser->m_KnightRoyaleJoinRequest;
	pUser->m_KnightRoyaleJoinEvent = g_pMain->pKnightRoyaleEvent.ActiveEvent;
	return true;
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyaleRemoveEventUser(CUser *pUser)
void CGameServerDlg::KnightRoyaleRemoveEventUser(CUser* pUser)
{
	if (pUser == nullptr)
	{
		TRACE("#### Knight Royale RemoveEventUser : pUser == nullptr ####\n");
		return;
	}

	g_pMain->m_KnightRoyaleEventUserMap.DeleteData(pUser->m_KnightRoyaleJoinRequest);
	pUser->ResetKnightRoyaleEventData();
}
#pragma endregion

#pragma region CGameServerDlg::KnightRoyalSendJoinScreen(CUser* pUser /*= nullptr*/)
void CGameServerDlg::KnightRoyalSendJoinScreen(CUser* pUser /*= nullptr*/)
{
	Packet result(WIZ_KNIGHT_ROYALE, uint8(0x01));
	result << uint8(0) << uint8(pKnightRoyaleEvent.AllUserCount);

	if (pUser)
		pUser->Send(&result);
	else
		g_pMain->Send_All(&result, nullptr, Nation::ALL, 0, false, 0);
}
#pragma endregion

#pragma region CUser::ItemGetKnightRoyal(Packet & pkt)
void CUser::ItemGetKnightRoyal(Packet& pkt)
{
	Packet result(WIZ_KNIGHT_ROYALE);
	uint32 nBundleID = pkt.read<uint32>(), nItemID = pkt.read<uint32>();
	_LOOT_BUNDLE* pBundle = nullptr;
	_LOOT_ITEM* pItem = nullptr;
	CRegion* pRegion = GetRegion();
	CUser* pReceiver = nullptr;
	// Lock the array while we process this request
	// to prevent any race conditions between getting/removing the items...
	Guard lock(pRegion->m_RegionItemArray.m_lock);

	// Are we in any region?
	if (pRegion == nullptr
		|| isTrading()
		|| isDead()
		// Does the bundle exist in this region's list?
		|| (pBundle = pRegion->m_RegionItemArray.GetData(nBundleID)) == nullptr
		// Are we close enough to the bundle?
		|| !isInRange(pBundle->x, pBundle->z, MAX_LOOT_RANGE))
	{
		result << uint8(0);
		Send(&result);
		return;
	}

	// Does the specified item we're looting exist in the bundle?
	foreach(itr, pBundle->Items)
	{
		if (itr->nItemID == nItemID)
		{
			pItem = &(*itr);
			break;
		}
	}

	// Attempt to loot the specified item.
	// If looting is possible, we can then give the receiver the item.
	if (pItem == nullptr
		|| pItem->sCount == 0
		|| (pReceiver = GetLootUser(pBundle, pItem)) == nullptr)
	{
		result << uint8(0);
		Send(&result);
		return;
	}

	// If we're dealing with coins, either:
	//  - we're not in a party, in which case the coins go to us. 
	//  - we're in a party, in which case we need to distribute the coins (proportionately, by their level). 
	// Error handling should already have occurred in GetLootUser().
	if (nItemID == ITEM_GOLD)
	{
		uint32 pGold = 0;
		pGold = pItem->sCount;
		GoldGain(pGold, false, true);

		result << uint8(5) << nBundleID << uint16(1) << nItemID << pItem->sCount;
		pReceiver->Send(&result);
	}
	else
	{
		if (pReceiver->isDead() || !pReceiver->isInRange(pBundle->x, pBundle->z, RANGE_50M))
			pReceiver = GetLootUser(pBundle, pItem);

		// Retrieve the position for this item.
		int8 bDstPos = pReceiver->FindSlotForItem(pItem->nItemID, pItem->sCount);

		// This should NOT happen unless their inventory changed after the check.
		// The item won't be removed until after processing's complete, so it's OK to error out here.
		if (bDstPos < 0)
		{
			result << uint8(0);
			Send(&result);
			return;
		}

		// Ensure there's enough room in this user's inventory.
		if (!pReceiver->CheckWeight(pItem->nItemID, pItem->sCount))
		{
			result << uint8(0);
			pReceiver->Send(&result);
			return;
		}

		// Add item to receiver's inventory
		_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID); // note: already looked up in GetLootUser() so it definitely exists
		_ITEM_DATA* pDstItem = &pReceiver->m_sItemKnightRoyalArray[bDstPos];

		pDstItem->nNum = pItem->nItemID;
		pDstItem->sCount += pItem->sCount;

		if (pDstItem->sCount == pItem->sCount)
		{
			pDstItem->nSerialNum = g_pMain->GenerateItemSerial();

			// NOTE: Obscure special items that act as if their durations are their stack sizes
			// will be broken here, but usual cases are typically only given in the PUS.
			// Will need to revise this logic (rather, shift it out into its own method).
			pDstItem->sDuration = pTable->m_sDuration;
		}

		if (pDstItem->sCount > MAX_ITEM_COUNT)
			pDstItem->sCount = MAX_ITEM_COUNT;

		result << uint8(5)
			<< nBundleID
			<< uint16(bDstPos - SLOT_MAX)
			<< pDstItem->nNum << pDstItem->sCount;
		pReceiver->Send(&result);

		pReceiver->SetUserAbility(false);
		pReceiver->SendItemWeight();
	}

	// Everything is OK, we have a target. Now remove the item from the bundle.
	// If there's nothing else in the bundle, remove the bundle from the region.
	if (GetMap())
		GetMap()->RegionItemRemove(pRegion, pBundle, pItem);
}
#pragma endregion

#pragma region CUser::ItemRemoveKnightRoyal(Packet & pkt)
void CUser::ItemRemoveKnightRoyal(Packet& pkt)
{
	Packet result(WIZ_KNIGHT_ROYALE);
	_ITEM_DATA* pItem;
	uint8 bPos;
	uint32 nItemID;
	pkt >> bPos >> nItemID;

	bPos += SLOT_MAX;
	if (bPos >= HAVE_MAX)
	{
		result << uint8(8) << uint8(0);
		Send(&result);
		return;
	}

	// Make sure the item matches what the client says it is
	pItem = GetKnightRoyaleItem(bPos);
	if (pItem == nullptr
		|| pItem->nNum != nItemID
		|| pItem->isSealed()
		|| pItem->isRented()
		|| isMining()
		|| isFishing())
	{
		result << uint8(8) << uint8(0);
		Send(&result);
		return;
	}

	memset(pItem, 0, sizeof(_ITEM_DATA));
	SetUserAbility();
	SendItemWeight();

	result << uint8(8) << uint8(1);
	Send(&result);
}
#pragma endregion