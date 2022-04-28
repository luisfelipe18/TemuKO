#include "stdafx.h"

#include "Map.h"
#include "GameEvent.h"

#include "KnightsManager.h"
#include "DBAgent.h"
#include "KingSystem.h"

#include "../shared/database/OdbcRecordset.h"
#include "../shared/database/ItemTableSet.h"
#include "../shared/database/ItemSellTableSet.h"
#include "../shared/database/SetItemTableSet.h"
#include "../shared/database/ItemExchangeSet.h"
#include "../shared/database/ItemExpirationSet.h"
#include "../shared/database/ItemExchangeExpSet.h"
#include "../shared/database/ItemSpecialExchangeSet.h"
#include "../shared/database/ItemExchangeCrashSet.h"
#include "../shared/database/ItemUpgradeSet.h"
#include "../shared/database/ItemOpSet.h"
#include "../shared/database/MagicTableSet.h"
#include "../shared/database/MagicType1Set.h"
#include "../shared/database/MagicType2Set.h"
#include "../shared/database/MagicType3Set.h"
#include "../shared/database/MagicType4Set.h"
#include "../shared/database/MagicType5Set.h"
#include "../shared/database/MagicType6Set.h"
#include "../shared/database/MagicType7Set.h"
#include "../shared/database/MagicType8Set.h"
#include "../shared/database/MagicType9Set.h"
#include "../shared/database/ObjectPosSet.h"
#include "../shared/database/ZoneInfoSet.h"
#include "../shared/database/EventSet.h"
#include "../shared/database/CoefficientSet.h"
#include "../shared/database/LevelUpTableSet.h"
#include "../shared/database/ServerResourceSet.h"
#include "../shared/database/MonsterResourceSet.h"
#include "../shared/database/QuestHelperSet.h"
#include "../shared/database/QuestMonsterSet.h"
#include "../shared/database/KnightsAllianceSet.h"
#include "../shared/database/KnightsRankSet.h"
#include "../shared/database/KnightsCapeSet.h"
#include "../shared/database/UserPersonalRankSet.h"
#include "../shared/database/UserKnightsRankSet.h"
#include "../shared/database/CharacterSealSet.h"
#include "../shared/database/StartPositionSet.h"
#include "../shared/database/StartPositionRandomSet.h"
#include "../shared/database/KNStartPositionRandomSet.h"
#include "../shared/database/BattleSet.h"
#include "../shared/database/RentalItemSet.h"
#include "../shared/database/KingSystemSet.h"
#include "../shared/database/KingCandidacyNoticeBoardSet.h"
#include "../shared/database/KingElectionListSet.h"
#include "../shared/database/KingNominationListSet.h"
#include "../shared/database/SheriffReportListSet.h"
#include "../shared/database/EventTriggerSet.h"
#include "../shared/database/MonsterChallenge.h"
#include "../shared/database/MonsterChallengeSummonList.h"
#include "../shared/database/MonsterSummonListSet.h"
#include "../shared/database/MonsterUnderTheCastleSet.h"
#include "../shared/database/MonsterStoneListInformationSet.h"
#include "../shared/database/JuraidMountionListInformationSet.h"
#include "../shared/database/MiningFishingTableSet.h"
#include "../shared/database/PremiumItemSet.h"
#include "../shared/database/PremiumItemExpSet.h"
#include "../shared/database/UserDailyOpSet.h"
#include "../shared/database/UserItemSet.h"
#include "../shared/database/KnightsSiegeWar.h"
#include "../shared/database/AchieveTitle.h"
#include "../shared/database/AchieveMain.h"
#include "../shared/database/AchieveMonster.h"
#include "../shared/database/AchieveNormal.h"
#include "../shared/database/AchieveWar.h"
#include "../shared/database/AchieveCom.h"
#include "../shared/database/PremiumBonusItemSet.h"
#include "../shared/database/MiningExchange.h"
#include "../shared/database/ChaosStoneSummonList.h"
#include "../shared/database/ChaosStoneRespawn.h"
#include "../shared/database/KnightsCastellanCapeList.h"
#include "../shared/database/BanishOfWinnerNpc.h"
#include "../shared/database/GiveItemLuaFunctionSet.h"
#include "../shared/database/DarkKnightMaterialMonsterDeathSet.h"
#include "../shared/database/EventScheduleStatusSet.h"
#include "../shared/database/EventRoomSchedulePlayTimerSet.h"
#include "../shared/database/EventRoyaleSchedulePlayTimerSet.h"
#include "../shared/database/EventCswSchedulePlayTimerSet.h"
#include "../shared/database/DungeonDefenceMonsterListSet.h"
#include "../shared/database/DungeonDefenceStageListSet.h"
#include "../shared/database/KnightRoyaleStatSet.h"
#include "../shared/database/KnightRoyaleBeginnerItem.h"
#include "../shared/database/ReturnLetterGiftItemSet.h"
#include "../shared/database/PetStatsInfo.h"
#include "../shared/database/PetTransformSet.h"
#include "../shared/database/PetTransformSet.h"
#include "../shared/database/Kill_Random_Gift.h"
#include "../shared/database/ppcardtable.h"

#include "../shared/database/MonsterRespawnVeriableListSet.h"
#include "../shared/database/MonsterRespawnStableListSet.h"

// AI Server Related Includes
#include "../shared/database/NpcItemSet.h"
#include "../shared/database/MonsterItemSet.h"
#include "../shared/database/MakeItemGroupSet.h"
#include "../shared/database/MakeWeaponTableSet.h"
#include "../shared/database/MakeDefensiveTableSet.h"
#include "../shared/database/MakeGradeItemTableSet.h"
#include "../shared/database/MakeLareItemTableSet.h"
#include "../shared/database/NpcTableSet.h"
#include "../shared/database/NpcPosSet.h"

// NPC Related Load Methods

bool CGameServerDlg::LoadNpcItemTable()
{
	LOAD_TABLE(CNpcItemSet, g_DBAgent.m_GameDB, &m_NpcItemArray, false, false);
}

bool CGameServerDlg::LoadMonsterItemTable()
{
	LOAD_TABLE(CMonsterItemSet, g_DBAgent.m_GameDB, &m_MonsterItemArray, false, false);
}

bool CGameServerDlg::LoadMakeItemGroupTable()
{
	LOAD_TABLE(CMakeItemGroupSet, g_DBAgent.m_GameDB, &m_MakeItemGroupArray, false, false);
}

bool CGameServerDlg::LoadMakeWeaponItemTableData()
{
	LOAD_TABLE(CMakeWeaponTableSet, g_DBAgent.m_GameDB, &m_MakeWeaponItemArray, true, false);
}

bool CGameServerDlg::LoadMakeDefensiveItemTableData()
{
	LOAD_TABLE(CMakeDefensiveTableSet, g_DBAgent.m_GameDB, &m_MakeDefensiveItemArray, true, false);
}

bool CGameServerDlg::LoadMakeGradeItemTableData()
{
	LOAD_TABLE(CMakeGradeItemTableSet, g_DBAgent.m_GameDB, &m_MakeGradeItemArray, false, false);
}

bool CGameServerDlg::LoadMakeLareItemTableData()
{
	LOAD_TABLE(CMakeLareItemTableSet, g_DBAgent.m_GameDB, &m_MakeLareItemArray, false, false);
}

bool CGameServerDlg::LoadNpcTableData(bool bNpcData /*= true*/)
{
	if (bNpcData) { LOAD_TABLE(CNpcTableSet, g_DBAgent.m_GameDB, &m_arNpcTable, false, false); }
	else { LOAD_TABLE(CMonTableSet, g_DBAgent.m_GameDB, &m_arMonTable, false, false); }
}

bool CGameServerDlg::LoadNpcPosTable()
{
	LOAD_TABLE(CNpcPosSet, g_DBAgent.m_GameDB, &m_NpcPosArray, false, false);
}

/**
* @brief	Creates the threads that will operate the processes
*			of the NPC/MONSTER's artificial related works.
*/
bool CGameServerDlg::CreateNpcThread()
{
	Sleep(2000);
	foreach_stlmap(itr, m_ZoneArray)
	{
		CNpcThread * pNpcThread = new CNpcThread(itr->first);
		if (!m_arNpcThread.PutData(itr->first, pNpcThread))
			return false;

		pNpcThread->AddRequest(new Signal(NpcThreadSignalType::NpcAllLoad));
		Sleep(100);
	}

	printf("\nMonster Threads created for all zones. Total NPC Num = %d, threads = %d\n", (uint16)m_TotalNPC, m_arNpcThread.GetSize());
	return true;
}

bool CGameServerDlg::LoadItemTable()
{
	LOAD_TABLE(CItemTableSet, g_DBAgent.m_GameDB, &m_ItemtableArray, false, false);
}

bool CGameServerDlg::LoadItemSellTable()
{
	LOAD_TABLE(CItemSellTableSet, g_DBAgent.m_GameDB, &m_ItemSellTableArray, false, false);
}

bool CGameServerDlg::LoadItemPremiumGiftTable()
{
	LOAD_TABLE(CItemPremiumGift, g_DBAgent.m_GameDB, &m_ItemPremiumGiftArray, false, false);
}

bool CGameServerDlg::LoadSetItemTable()
{
	LOAD_TABLE(CSetItemTableSet, g_DBAgent.m_GameDB, &m_SetItemArray, true, false);
}

bool CGameServerDlg::LoadItemExchangeTable()
{
	LOAD_TABLE(CItemExchangeSet, g_DBAgent.m_GameDB, &m_ItemExchangeArray, true, false);
}

bool CGameServerDlg::LoadItemExchangeExpTable()
{
	LOAD_TABLE(CItemExchangeExpSet, g_DBAgent.m_GameDB, &m_ItemExchangeExpArray, true, false);
}

bool CGameServerDlg::LoadItemSpecialExchangeTable()
{
	LOAD_TABLE(CItemSpecialExchangeSet, g_DBAgent.m_GameDB, &m_ItemSpecialExchangeArray, true, false);
}

bool CGameServerDlg::LoadItemExchangeCrashTable()
{
	LOAD_TABLE(CItemExchangeCreashSet, g_DBAgent.m_GameDB, &m_ItemExchangeCrashArray, true, false);
}

bool CGameServerDlg::LoadItemUpgradeTable()
{
	LOAD_TABLE(CItemUpgradeSet, g_DBAgent.m_GameDB, &m_ItemUpgradeArray, false, false);
}

bool CGameServerDlg::LoadItemOpTable()
{
	LOAD_TABLE(CItemOpSet, g_DBAgent.m_GameDB, &m_ItemOpArray, true, false);
}

bool CGameServerDlg::LoadServerResourceTable()
{
	LOAD_TABLE(CServerResourceSet, g_DBAgent.m_GameDB, &m_ServerResourceArray, false, false);
}

bool CGameServerDlg::LoadQuestHelperTable()
{
	Guard lock(m_questNpcLock);
	m_QuestNpcList.clear();
	LOAD_TABLE(CQuestHelperSet, g_DBAgent.m_GameDB, &m_QuestHelperArray, true, false);
}

bool CGameServerDlg::LoadQuestMonsterTable()
{
	LOAD_TABLE(CQuestMonsterSet, g_DBAgent.m_GameDB, &m_QuestMonsterArray, true, false);
}

bool CGameServerDlg::LoadMagicTable()
{
	LOAD_TABLE(CMagicTableSet, g_DBAgent.m_GameDB, &m_MagictableArray, false, false);
}

bool CGameServerDlg::LoadMagicType1()
{
	LOAD_TABLE(CMagicType1Set, g_DBAgent.m_GameDB, &m_Magictype1Array, false, false);
}

bool CGameServerDlg::LoadMagicType2()
{
	LOAD_TABLE(CMagicType2Set, g_DBAgent.m_GameDB, &m_Magictype2Array, false, false);
}

bool CGameServerDlg::LoadMagicType3()
{
	LOAD_TABLE(CMagicType3Set, g_DBAgent.m_GameDB, &m_Magictype3Array, false, false);
}

bool CGameServerDlg::LoadMagicType4()
{
	LOAD_TABLE(CMagicType4Set, g_DBAgent.m_GameDB, &m_Magictype4Array, false, false);
}

bool CGameServerDlg::LoadMagicType5()
{
	LOAD_TABLE(CMagicType5Set, g_DBAgent.m_GameDB, &m_Magictype5Array, false, false);
}

bool CGameServerDlg::LoadMagicType6()
{
	LOAD_TABLE(CMagicType6Set, g_DBAgent.m_GameDB, &m_Magictype6Array, false, false);
}

bool CGameServerDlg::LoadMagicType7()
{
	LOAD_TABLE(CMagicType7Set, g_DBAgent.m_GameDB, &m_Magictype7Array, false, false);
}

bool CGameServerDlg::LoadMagicType8()
{
	LOAD_TABLE(CMagicType8Set, g_DBAgent.m_GameDB, &m_Magictype8Array, false, false);
}

bool CGameServerDlg::LoadMagicType9()
{
	LOAD_TABLE(CMagicType9Set, g_DBAgent.m_GameDB, &m_Magictype9Array, false, false);
}

bool CGameServerDlg::LoadRentalList()
{
	LOAD_TABLE(CRentalItemSet, g_DBAgent.m_GameDB, &m_RentalItemArray, true, false);
}

bool CGameServerDlg::LoadCoefficientTable()
{
	LOAD_TABLE(CCoefficientSet, g_DBAgent.m_GameDB, &m_CoefficientArray, false, false);
}

bool CGameServerDlg::LoadLevelUpTable()
{
	LOAD_TABLE(CLevelUpTableSet, g_DBAgent.m_GameDB, &m_LevelUpArray, false, false);
}

bool CGameServerDlg::LoadAllKnights()
{
	return g_DBAgent.LoadAllKnights(m_KnightsArray);
}

bool CGameServerDlg::LoadKnightsAllianceTable(bool bIsSlient)
{
	LOAD_TABLE(CKnightsAllianceSet, g_DBAgent.m_GameDB, &m_KnightsAllianceArray, true, bIsSlient);
}

bool CGameServerDlg::LoadMiningExchangeListTable()
{
	LOAD_TABLE(CMiningExchange, g_DBAgent.m_GameDB, &m_MiningExchangeArray, true, false);
}

bool CGameServerDlg::LoadEventScheduleStatusTable()
{
	LOAD_TABLE(CEventScheduleStatus, g_DBAgent.m_GameDB, &m_EventScheduleArray, true, false);
}

bool CGameServerDlg::LoadRoomEventPlayTimerTable()
{
	LOAD_TABLE(CEventSchedulePlayTimer, g_DBAgent.m_GameDB, &m_RoomEventPlayTimerArray, true, false);
}

bool CGameServerDlg::LoadRoyaleEventPlayTimerTable()
{
	LOAD_TABLE(CEventRoyaleSchedulePlayTimer, g_DBAgent.m_GameDB, &m_RoyaleEventPlayTimerArray, true, false);
}

bool CGameServerDlg::LoadCswEventPlayTimerTable()
{
	LOAD_TABLE(CEventCswSchedulePlayTimer, g_DBAgent.m_GameDB, &m_CswEventPlayTimerArray, true, false);
}

bool CGameServerDlg::LoadAchieveTitleTable()
{
	LOAD_TABLE(CAchieveTitleSet, g_DBAgent.m_GameDB, &m_AchieveTitleArray, true, false);
}

bool CGameServerDlg::LoadAchieveWarTable()
{
	LOAD_TABLE(CAchieveWarSet, g_DBAgent.m_GameDB, &m_AchieveWarArray, true, false);
}

bool CGameServerDlg::LoadAchieveMainTable()
{
	LOAD_TABLE(CAchieveMainSet, g_DBAgent.m_GameDB, &m_AchieveMainArray, true, false);
}

bool CGameServerDlg::LoadAchieveMonsterTable()
{
	LOAD_TABLE(CAchieveMonsterSet, g_DBAgent.m_GameDB, &m_AchieveMonsterArray, true, false);
}

bool CGameServerDlg::LoadAchieveNormalTable()
{
	LOAD_TABLE(CAchieveNormalSet, g_DBAgent.m_GameDB, &m_AchieveNormalArray, true, false);
}

bool CGameServerDlg::LoadAchieveComTable()
{
	LOAD_TABLE(CAchieveComSet, g_DBAgent.m_GameDB, &m_AchieveComArray, true, false);
}

bool CGameServerDlg::LoadDungeonDefenceMonsterTable()
{
	LOAD_TABLE(CDungeonDefenceMonsterListSet, g_DBAgent.m_GameDB, &m_DungeonDefenceMonsterListArray, true, false);
}

bool CGameServerDlg::LoadDungeonDefenceStageTable()
{
	LOAD_TABLE(CDungeonDefenceStageListSet, g_DBAgent.m_GameDB, &m_DungeonDefenceStageListArray, true, false);
}

bool CGameServerDlg::LoadKnightRoyaleStatsTable()
{
	LOAD_TABLE(CKnightRoyaleStatstSet, g_DBAgent.m_GameDB, &m_KnightRoayleStatsListArray, true, false);
}

bool CGameServerDlg::LoadKnightRoyaleBeginnerItemTable()
{
	LOAD_TABLE(CKnightRoyaleBeginnerItem, g_DBAgent.m_GameDB, &m_KnightRoyaleBeginnerItemListArray, true, false);
}
bool CGameServerDlg::LoadKnightReturnLetterGiftItemTable()
{
	LOAD_TABLE(CUserReturnLetterGiftItem, g_DBAgent.m_GameDB, &m_KnightReturnLetterGiftListArray, true, false);
}

bool CGameServerDlg::LoadKnightsCastellanCapeTable()
{
	LOAD_TABLE(CKnightsCastellanCapeListSet, g_DBAgent.m_GameDB, &m_KnightCastellanCapeArray, true, false);
}

bool CGameServerDlg::LoadDarkKnightMaterialMonsterDeadTable()
{
	LOAD_TABLE(CDarkKnightMaterialDeadGiftSet, g_DBAgent.m_GameDB, &m_DkmMonsterDeadGiftArray, true, false);
}

bool CGameServerDlg::LoadKnightsSiegeWarsTable(bool bIsSlient)
{
	LOAD_TABLE(CKnightsSiegeWarfare, g_DBAgent.m_GameDB, &m_KnightsSiegeWarfareArray, true, bIsSlient);
}

//Pet System
bool CGameServerDlg::LoadPetStatsInfoTable()
{
	LOAD_TABLE(CPetStatsInfoSet, g_DBAgent.m_GameDB, &m_PetInfoSystemArray, true, false);
}

bool CGameServerDlg::LoadPetImageChangeTable()
{
	LOAD_TABLE(CPetImageChane, g_DBAgent.m_GameDB, &m_PetTransformSystemArray, true, false);
}

bool CGameServerDlg::LoadUserRankings()
{
	CUserPersonalRankSet UserPersonalRankSet(g_DBAgent.m_GameDB, &m_UserKarusPersonalRankMap, &m_UserElmoPersonalRankMap);
	CUserKnightsRankSet  UserKnightsRankSet(g_DBAgent.m_GameDB, &m_UserKarusKnightsRankMap, &m_UserElmoKnightsRankMap);
	TCHAR * szError = nullptr;

	// Cleanup first, in the event it's already loaded (we'll have this automatically reload in-game)
	CleanupUserRankings();

	// Acquire the lock for thread safety, and load both tables.
	Guard lock(m_userRankingsLock);

	szError = UserPersonalRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load personal rankings, error:\n%s\n", szError);
		return false;
	}

	foreach(itr, m_UserKarusPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->second->strUserName, TYPE_CHARACTER);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pUser->m_bPersonalRank = uint8(itr->second->nRank);
	}

	foreach(itr, m_UserElmoPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->second->strUserName, TYPE_CHARACTER);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pUser->m_bPersonalRank = uint8(itr->second->nRank);
	}

	szError = UserKnightsRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load user knights rankings, error:\n%s\n", szError);
		return false;
	}

	foreach(itr, m_UserKarusKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->second->strUserName, TYPE_CHARACTER);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;
		pUser->m_bKnightsRank = uint8(itr->second->nRank);
	}

	foreach(itr, m_UserElmoKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->second->strUserName, TYPE_CHARACTER);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;
		pUser->m_bKnightsRank = uint8(itr->second->nRank);
	}

	return true;
}

bool CGameServerDlg::LoadCharacterSealItemTable()
{
	LOAD_TABLE(CCharacterSealSet, g_DBAgent.m_GameDB, &m_CharacterSealItemArray, true, false);
}

/**
* @brief	Loads the expiration item table from the database.
*/
bool CGameServerDlg::LoadExpirationItemTable()
{
	LOAD_TABLE(CItemExpirationSet, g_DBAgent.m_GameDB, &m_UserExpirationItemArray, true, false);
}

/**
* @brief	Loads the user item table from the database.
*/
bool CGameServerDlg::LoadCharacterSealItemTableAll()
{
	return g_DBAgent.LoadAllCharacterSealItems(m_CharacterSealItemArray);
}

void CGameServerDlg::CleanupUserRankings()
{
	std::set<_USER_RANK *> deleteSet;
	Guard lock(m_userRankingsLock);

	// Go through the personal rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach(itr, m_UserElmoPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bPersonalRank = -1;

		deleteSet.insert(itr->second);
	}

	foreach(itr, m_UserKarusPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bPersonalRank = -1;

		deleteSet.insert(itr->second);
	}

	// Go through the knights rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach(itr, m_UserElmoKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bKnightsRank = -1;

		deleteSet.insert(itr->second);
	}

	foreach(itr, m_UserKarusKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bKnightsRank = -1;

		deleteSet.insert(itr->second);
	}

	// Clear out the maps
	m_UserElmoPersonalRankMap.clear();
	m_UserKarusPersonalRankMap.clear();
	m_UserElmoKnightsRankMap.clear();
	m_UserKarusKnightsRankMap.clear();

	// Free the memory used by the _USER_RANK structs
	foreach(itr, deleteSet)
		delete *itr;

	// These only store pointers to memory that was already freed by the primary rankings maps.
	m_playerRankings[KARUS_ARRAY].clear();
	m_playerRankings[ELMORAD_ARRAY].clear();
}

bool CGameServerDlg::LoadKnightsCapeTable()
{
	LOAD_TABLE(CKnightsCapeSet, g_DBAgent.m_GameDB, &m_KnightsCapeArray, false, false);
}

bool CGameServerDlg::LoadKnightsRankTable(bool bWarTime /*= false*/, bool bIsSlient /*= false*/)
{
	std::string strKarusCaptainNames, strElmoCaptainNames;
	LOAD_TABLE_ERROR_ONLY(CKnightsRankSet, g_DBAgent.m_GameDB, nullptr, true, bIsSlient);

	if (!bWarTime)
		return true;

	CKnightsRankSet & pSet = _CKnightsRankSet; // kind ugly generic naming

	if (pSet.nKarusCount > 0)
	{
		Packet result;
		GetServerResource(IDS_KARUS_CAPTAIN, &strKarusCaptainNames, 
			pSet.strKarusCaptain[0], pSet.strKarusCaptain[1], pSet.strKarusCaptain[2], 
			pSet.strKarusCaptain[3], pSet.strKarusCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strKarusCaptainNames);
		Send_All(&result, nullptr, KARUS);
	}

	if (pSet.nElmoCount > 0)
	{
		Packet result;
		GetServerResource(IDS_ELMO_CAPTAIN, &strElmoCaptainNames,
			pSet.strElmoCaptain[0], pSet.strElmoCaptain[1], pSet.strElmoCaptain[2], 
			pSet.strElmoCaptain[3], pSet.strElmoCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strElmoCaptainNames);
		Send_All(&result, nullptr, ELMORAD);
	}

	return true;
}

bool CGameServerDlg::LoadStartPositionTable()
{
	LOAD_TABLE(CStartPositionSet, g_DBAgent.m_GameDB, &m_StartPositionArray, false, false);
}

bool CGameServerDlg::LoadBattleTable()
{
	LOAD_TABLE(CBattleSet, g_DBAgent.m_GameDB, &m_byOldVictory, true, false);
}

bool CGameServerDlg::LoadKingSystem()
{
	LOAD_TABLE_ERROR_ONLY(CKingSystemSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE_ERROR_ONLY(CKingCandidacyNoticeBoardSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE_ERROR_ONLY(CKingNominationListSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE(CKingElectionListSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
}

bool CGameServerDlg::LoadSheriffTable()
{
	LOAD_TABLE(CSheriffReportList, g_DBAgent.m_GameDB, &m_SheriffArray, true, false);
}

bool CGameServerDlg::LoadMonsterResourceTable()
{
	LOAD_TABLE(CMonsterResourceSet, g_DBAgent.m_GameDB, &m_MonsterResourceArray, false, false);
}

bool CGameServerDlg::LoadMonsterChallengeTable()
{
	LOAD_TABLE(CMonsterChallenge, g_DBAgent.m_GameDB, &m_MonsterChallengeArray, true, false);
}

bool CGameServerDlg::LoadMonsterChallengeSummonListTable()
{
	LOAD_TABLE(CMonsterChallengeSummonList, g_DBAgent.m_GameDB, &m_MonsterChallengeSummonListArray, true, false);
}

bool CGameServerDlg::LoadMonsterSummonListTable()
{
	LOAD_TABLE(CMonsterSummonListSet, g_DBAgent.m_GameDB, &m_MonsterSummonList, true, false);
}

bool CGameServerDlg::LoadChaosStoneMonsterListTable()
{
	LOAD_TABLE(CChaosStoneSummonListSet, g_DBAgent.m_GameDB, &m_ChaosStoneSummonListArray, true, false);
}

bool CGameServerDlg::LoadChaosStoneCoordinateTable()
{
	LOAD_TABLE(CChaosStoneCoordinate, g_DBAgent.m_GameDB, &m_ChaosStoneRespawnCoordinateArray, true, false);
}

bool CGameServerDlg::LoadMonsterRespawnVeriableListTable()
{
	LOAD_TABLE(CMonsterRespawnVeriableListSet, g_DBAgent.m_GameDB, &m_MonsterVeriableRespawnListArray, true, false);
}

bool CGameServerDlg::LoadMonsterRespawnStableListTable()
{
	LOAD_TABLE(CMonsterRespawnStableListSet, g_DBAgent.m_GameDB, &m_MonsterStableRespawnListArray, true, false);
}

bool CGameServerDlg::LoadMonsterUnderTheCastleTable()
{
	LOAD_TABLE(CMonsterUnderTheCastleSet, g_DBAgent.m_GameDB, &m_MonsterUnderTheCastleArray, true, false);
}

bool CGameServerDlg::LoadMonsterStoneListInformationTable()
{
	LOAD_TABLE(CMonsterStoneListInformationSet, g_DBAgent.m_GameDB, &m_MonsterStoneListInformationArray, true, false);
}

bool CGameServerDlg::LoadJuraidMountionListInformationTable()
{
	LOAD_TABLE(CJuraidMountionListInformationSet, g_DBAgent.m_GameDB, &m_JuraidMountionListInformationArray, true, false);
}

bool CGameServerDlg::LoadMiningFishingItemTable()
{
	LOAD_TABLE(CMiningFishingTableSet, g_DBAgent.m_GameDB, &m_MiningFishingItemArray, true, false);
}

bool CGameServerDlg::LoadPremiumItemTable()
{
	LOAD_TABLE(CPremiumItemSet, g_DBAgent.m_GameDB, &m_PremiumItemArray, true, false);
}

bool CGameServerDlg::LoadPremiumItemExpTable()
{
	LOAD_TABLE(CPremiumItemExpSet, g_DBAgent.m_GameDB, &m_PremiumItemExpArray, true, false);
}

bool CGameServerDlg::LoadUserDailyOpTable()
{
	LOAD_TABLE(CUserDailyOpSet, g_DBAgent.m_GameDB, &m_UserDailyOpMap, true, false);
}

bool CGameServerDlg::LoadEventTriggerTable()
{
	LOAD_TABLE(CEventTriggerSet, g_DBAgent.m_GameDB, &m_EventTriggerArray, true, false);
}

bool CGameServerDlg::LoadStartPositionRandomTable()
{
	LOAD_TABLE(CStartPositionRandomSet, g_DBAgent.m_GameDB, &m_StartPositionRandomArray, true, false);
}

bool CGameServerDlg::LoadKnightRoyaleStartPositionRandomTable()
{
	LOAD_TABLE(CKnightRoyaleStartPositionRandomSet, g_DBAgent.m_GameDB, &m_KnStartPositionRandomArray, true, false);
}

bool CGameServerDlg::LoadBanishWinnerTable()
{
	LOAD_TABLE(CBanishWinner, g_DBAgent.m_GameDB, &m_WarBanishOfWinnerArray, true, false);
}

bool CGameServerDlg::LoadGiveItemExchangeTable()
{
	LOAD_TABLE(CLuaGiveItemExchangeSet, g_DBAgent.m_GameDB, &m_LuaGiveItemExchangeArray, true, false);
}

bool CGameServerDlg::LoadUserItemTable()
{
	LOAD_TABLE(CUserItemSet, g_DBAgent.m_GameDB, &m_UserItemArray, true, false);
}

bool CGameServerDlg::LoadObjectPosTable()
{
	LOAD_TABLE(CObjectPosSet, g_DBAgent.m_GameDB, &m_ObjectEventArray, true, false);
}

bool CGameServerDlg::MapFileLoad()
{
	ZoneInfoMap zoneMap;
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, g_DBAgent.m_GameDB, &zoneMap, false, false); 

	foreach (itr, zoneMap)
	{
		C3DMap *pMap = new C3DMap();
		_ZONE_INFO *pZone = itr->second;
		if (!pMap->Initialize(pZone))
		{
			printf("ERROR: Unable to load SMD - %s\n", pZone->m_MapName.c_str());
			delete pZone;
			delete pMap;
			m_ZoneArray.DeleteAllData();
			return false;
		}

		delete pZone;
		m_ZoneArray.PutData(pMap->m_nZoneNumber, pMap);
	}

	LOAD_TABLE_ERROR_ONLY(CEventSet, g_DBAgent.m_GameDB, &m_ZoneArray, true, false);
	return true;
}

bool CGameServerDlg::LoadUserKillGiftArray()
{
	LOAD_TABLE(CGiftUserGiftAray, g_DBAgent.m_GameDB, &m_GiftItemArray, true, false);
}

bool CGameServerDlg::LoadDrakiTowerTables() { return g_DBAgent.LoadDrakiTowerTables(); }
bool CGameServerDlg::LoadUserHackToolTables() { return g_DBAgent.LoadUserHackToolTables(); }
bool CGameServerDlg::LoadKnightSiegeWarRankTable() { return g_DBAgent.LoadKnightSiegeWarRankTable(); }
bool CGameServerDlg::LoadKnightSiegeCastellanRankTable() { return g_DBAgent.LoadKnightSiegeCastellanRankTable(); }
bool CGameServerDlg::LoadKnightRoyaleChestListTable() { return g_DBAgent.LoadKnightRoyaleChestList(); }
bool CGameServerDlg::LoadServerSettingsData() { return g_DBAgent.LoadServerSettings(); }
bool CGameServerDlg::LoadChaosStoneStage() { return g_DBAgent.LoadChaosStoneFamilyStage(); }
bool CGameServerDlg::GameStartClearRemainUser() { return g_DBAgent.GameStartClearRemainUser(); }
bool CGameServerDlg::LoadQuestSkillSetUpTable() { return g_DBAgent.LoadQuestSkillSetUp(); }
bool CGameServerDlg::LoadMonsterDropMainTable() { return g_DBAgent.LoadMonsterDropMainTable(); }
bool CGameServerDlg::LoadMonsterItemGroupItemTable() { return g_DBAgent.LoadMonsterItemGroupItemTable(); }
bool CGameServerDlg::LoadBotTable() { return g_DBAgent.LoadBotTable(); }
bool CGameServerDlg::LoadSettings() { return g_DBAgent.LoadSettings(); }
bool CGameServerDlg::LoadPusLoad() { return g_DBAgent.LoadPusLoad(); }

bool CGameServerDlg::LoadCollectionRaceSettings() { return g_DBAgent.LoadCollectionRaceSettings(); }
bool CGameServerDlg::LoadCollectionRaceHuntList() { return g_DBAgent.LoadCollectionRaceHuntList(); }

bool CGameServerDlg::LoadPPCardTable() {LOAD_TABLE(CPPCardTable, g_DBAgent.m_GameDB, &m_PPCardArray, true, false);}