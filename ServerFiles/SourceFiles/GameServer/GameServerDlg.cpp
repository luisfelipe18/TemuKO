#include "stdafx.h"
#include "KingSystem.h"
#include "KnightsManager.h"

#include "../shared/ClientSocketMgr.h"
#include "../shared/Ini.h"
#include "../shared/DateTime.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include "Map.h"

#include "AISocket.h"

#include "DBAgent.h"

using namespace std;

std::vector<Thread *> g_timerThreads;

#pragma region CGameServerDlg::CGameServerDlg()
/**
* @brief The constructor.
*/
CGameServerDlg::CGameServerDlg()
{
	Initialize();
}
#pragma endregion

#pragma region CGameServerDlg::Initialize()

void CGameServerDlg::Initialize()
{
	DateTime now;
	m_iItemUniqueID = 0;
	g_bNpcExit = false;
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();
	m_sSec = now.GetSecond();
	m_ReloadKnightAndUserRanksMinute = 0;
	m_byWeather = 0;
	m_sWeatherAmount = 0;
	m_byKingWeatherEvent = 0;
	m_byKingWeatherEvent_Day = 0;
	m_byKingWeatherEvent_Hour = 0;
	m_byKingWeatherEvent_Minute = 0;
	m_byExpEventAmount = 0;
	m_byCoinEventAmount = 0;
	m_byNPEventAmount = 0;
	m_sPartyIndex = 0;
	m_nCastleCapture = 0;
	m_nServerNo = 0;
	m_nServerGroupNo = 0;
	m_nServerGroup = 0;
	m_sDiscount = 0;
	MonsterDeadCount = 0;
	m_bPermanentChatMode = false;
	m_bSantaOrAngel = FLYING_NONE;
	m_sLoginingPlayer = 0;
	m_DrakiRiftTowerRoomIndex = 0;
	m_DungeonDefenceRoomIndex = 0;
	sGameMasterSocketID = -1;

	/* Boss Event */
	BossEventDurumu = false;
	BossEventTimer = 0;

	/* Chat Event */
	UserChatEventOn = false;
	m_ChatEventStatus = false;
	UserChatEventKey = "NULL";
	ChatEventUnixTime = UNIXTIME;

	OpenArdream = false;
	OpenCZ = false;
	OpenBaseLand = false;
	//OpenAllClanWar = false;

	ArdreamEventFinishTime = 0;
	ArdreamEventGameServerSatus = false;

	CzEventFinishTime = 0;
	CzEventGameServerSatus = false;

	BaseLandEventFinishTime = 0;
	BaseLandEventGameServerSatus = false;

	/* Vanguard System Start */
	VanGuardSelectStatus = false;
	VanGuardTime = 0;
	VanGuardSelect = false;
	VanGuardSelectTime = 0;
	WantedEventManuelStarted = 0;
	/* Vanguard System End */
}
#pragma endregion

/**
* @brief	Loads config, table data, initialises sockets and generally
* 			starts up the server.
*
* @return	true if it succeeds, false if it fails.
*/
bool CGameServerDlg::Startup()
{
	color_c("", FOREGROUND_INTENSITY + 6); // sari
	DateTime time;
	// Server Start
	printf("Server started on %04d-%02d-%02d at %02d:%02d\n\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());

	color_c("", FOREGROUND_INTENSITY + FOREGROUND_GREEN); // yeşil
	// License System..
	m_HardwareIDArray.push_back(230662953965053);
	m_HardwareIDArray.push_back(4563295485398);
	m_HardwareIDArray.push_back(287021517914848);
	m_HardwareIDArray.push_back(620371517914848);
	m_HardwareIDArray.push_back(3748943933151791338);
	m_HardwareIDArray.push_back(9223372036854775807);
	m_HardwareIDArray.push_back(260732953912417);
	m_HardwareIDArray.push_back(260732953912417);
	m_HardwareIDArray.push_back(99442953912417);
	m_HardwareIDArray.push_back(254922953945256);
	m_HardwareIDArray.push_back(290191518222173);
	m_HardwareIDArray.push_back(311982953912417);//HARDWARE ID LICENCIA
	/*
	 * Esta parte es la parte de la licencia del número de serie del HDD.
	 * Cuando abra Gameserver.exe, mostrará el número de serie que debe escribir aquí en la pantalla.
	*/

	m_sHardwareIpArray.push_back("127.0.0.1"); // IP LICENCIA
	/*
	 *Esta parte es la parte de la licencia de propiedad intelectual de los archivos.
	 *Como estamos trabajando en local, si va a trabajar en un servidor con una dirección IP local,
	 *se escribirá la dirección IP del servidor.
	*/

	if (!g_HardwareInformation.IsValidHardwareID(m_HardwareIDArray))
	{
		printf("License Number: %lld\n", g_HardwareInformation.GetHardwareID());
		return false;
	}
	printf("### -> License Successfuly\n\n");
	color_c("", FOREGROUND_BLUE + FOREGROUND_BLUE + FOREGROUND_GREEN);
	// License System..

	GetTimeFromIni();
	GetEventAwardsIni();
	GetChatIni();

	if (!StartLisansSystem())
		return false;

	if (!g_DBAgent.Startup(m_bMarsEnabled, 
		m_strAccountDSN, m_strAccountUID, m_strAccountPWD, 
		m_strGameDSN, m_strGameUID, m_strGamePWD)) 
	{
		printf("ERROR: Unable to connect to the database using the details configured.\n");
		return false;
	}

	printf("Database connection is established successfully.\n");
	Sleep(100);

	// Load all tables from the database
	if(!LoadTables()) 
		return false;

	// Clear any remaining users in the currently logged in list
	// that may be left as a result of an improper shutdown.
	g_DBAgent.ClearRemainUsers();
	KnightLogger::Startup();
	CreateDirectories();

	LoadNoticeData();
	LoadNoticeUpData();

	printf("\n");
	if (!m_luaEngine.Initialise())
		return false;

	// Initialise the command tables
	InitServerCommands();
	CUser::InitChatCommands();

	if (MaxLevel62Control == 0)
	{
		OpenArdream = false;
		//OpenAllClanWar = false;
		OpenBaseLand = false;
		OpenCZ = true;
	}
	else
	{
		OpenArdream = true;
		OpenCZ = false;
		OpenBaseLand = false;
		//OpenAllClanWar = false;
	}

	/*Prinft Temizleme*/
	system("cls");

	// Server Start
	printf("Server started on %04d-%02d-%02d at %02d:%02d\n\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());

	if (!StartTheardSocketSystem())
		return false;

	if (!StartUserSocketSystem())
	{
		printf(_T("ERROR : Failed to listen on server port (%d).\n"), m_GameServerPort);
		return false;
	}

	CastleSiegeWarGameStartController();
	SetMiningDrops();
	UpdateCollectionRaceInfoTime();
	return true; 
}

#pragma region CGameServerDlg::StartLisansSystem()
bool CGameServerDlg::StartLisansSystem()
{
	_ZONE_SERVERINFO* pInfo = g_pMain->m_ServerArray.GetData(g_pMain->m_nServerNo);

	if (pInfo == nullptr)
		return false;

	foreach(itr, m_sHardwareIpArray)
	{
		if ((*itr) == pInfo->strServerIP)
			return true;
	}

	printf("Lisans IP Number: %s\n", pInfo->strServerIP.c_str());
	return false;
}
#pragma region CGameServerDlg::StartTheardSocketSystem()

bool CGameServerDlg::StartTheardSocketSystem()
{
	g_timerThreads.push_back(new Thread(Timer_CheckGameEvents));
	g_timerThreads.push_back(new Thread(Timer_UpdateGameTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateSessions));
	g_timerThreads.push_back(new Thread(Timer_UpdateConcurrent));
	g_timerThreads.push_back(new Thread(Timer_NpcThreadHandleRequests));
	g_timerThreads.push_back(new Thread(Timer_BifrostTime));
	return true;
}

#pragma endregion

#pragma region CGameServerDlg::StartUserSocketSystem()

bool CGameServerDlg::StartUserSocketSystem()
{
	_ZONE_SERVERINFO *pInfo = g_pMain->m_ServerArray.GetData(g_pMain->m_nServerNo);

	if (pInfo == nullptr)
		return false;

	if (!g_pMain->m_socketMgr.Listen(pInfo->strServerIP, m_GameServerPort, MAX_USER))
		return false;

	g_pMain->m_socketMgr.RunServer();
	return true;
}

#pragma endregion

#pragma region CGameServerDlg::LoadTables()

bool CGameServerDlg::LoadTables()
{
	if (!GameStartClearRemainUser()
		|| !LoadItemTable()
		|| !LoadItemSellTable()
		|| !LoadSetItemTable()
		|| !LoadItemExchangeTable()
		|| !LoadItemExchangeExpTable()
		|| !LoadItemSpecialExchangeTable()
		|| !LoadItemExchangeCrashTable()
		|| !LoadItemUpgradeTable()
		|| !LoadItemOpTable()
		|| !LoadServerResourceTable()
		|| !LoadQuestHelperTable()
		|| !LoadQuestMonsterTable()
		|| !LoadMagicTable()
		|| !LoadMagicType1()
		|| !LoadMagicType2()
		|| !LoadMagicType3()
		|| !LoadMagicType4()
		|| !LoadMagicType5()
		|| !LoadMagicType6()
		|| !LoadMagicType7()
		|| !LoadMagicType8()
		|| !LoadMagicType9()
		|| !LoadObjectPosTable()
		|| !LoadRentalList()
		|| !LoadMakeLareItemTableData()
		|| !LoadMakeGradeItemTableData()
		|| !LoadMakeDefensiveItemTableData()
		|| !LoadMakeWeaponItemTableData()
		|| !LoadMakeItemGroupTable()
		|| !LoadNpcItemTable()
		|| !LoadMonsterItemTable()
		|| !LoadNpcTableData(false) // Load NPC Data
		|| !LoadNpcTableData(true) // Load Monster Data
		|| !MapFileLoad()
		|| !LoadCoefficientTable()
		|| !LoadLevelUpTable()
		|| !LoadAllKnights()
		|| !LoadKnightsAllianceTable()
		|| !LoadKnightsSiegeWarsTable()
		|| !LoadUserRankings()
		|| !LoadCharacterSealItemTable()
		|| !LoadExpirationItemTable()
		|| !LoadCharacterSealItemTableAll()
		|| !LoadKnightsCapeTable()
		|| !LoadKnightsRankTable()
		|| !LoadStartPositionTable()
		|| !LoadStartPositionRandomTable()
		|| !LoadKnightRoyaleStartPositionRandomTable()
		|| !LoadBattleTable()
		|| !MapFileLoad()
		|| !LoadKingSystem()
		|| !LoadSheriffTable()
		|| !LoadEventTriggerTable()
		|| !LoadMonsterResourceTable()
		|| !LoadMonsterChallengeTable()
		|| !LoadMonsterChallengeSummonListTable()
		|| !LoadMonsterSummonListTable()
		|| !LoadChaosStoneMonsterListTable()
		|| !LoadChaosStoneCoordinateTable()
		|| !LoadChaosStoneStage()
		|| !LoadMonsterUnderTheCastleTable()
		|| !LoadMonsterStoneListInformationTable()
		|| !LoadJuraidMountionListInformationTable()
		|| !LoadMiningFishingItemTable()
		|| !LoadPremiumItemTable()
		|| !LoadPremiumItemExpTable()
		|| !LoadItemPremiumGiftTable()
		|| !LoadMiningExchangeListTable()
		|| !LoadUserDailyOpTable()
		|| !LoadUserItemTable()
		|| !LoadAchieveTitleTable()
		|| !LoadAchieveMainTable()
		|| !LoadAchieveMonsterTable()
		|| !LoadAchieveNormalTable()
		|| !LoadAchieveComTable()
		|| !LoadAchieveWarTable()
		|| !LoadKnightSiegeWarRankTable()
		|| !LoadKnightSiegeCastellanRankTable()
		|| !LoadDrakiTowerTables()
		|| !LoadUserHackToolTables()
		|| !LoadDungeonDefenceMonsterTable()
		|| !LoadDungeonDefenceStageTable()
		|| !LoadKnightRoyaleStatsTable()
		|| !LoadKnightRoyaleBeginnerItemTable()
		|| !LoadKnightRoyaleChestListTable()
		|| !LoadKnightReturnLetterGiftItemTable()
		|| !LoadKnightsCastellanCapeTable()
		|| !LoadDarkKnightMaterialMonsterDeadTable()
		|| !LoadBanishWinnerTable()
		|| !LoadGiveItemExchangeTable()
		|| !LoadEventScheduleStatusTable()
		|| !LoadRoomEventPlayTimerTable()
		|| !LoadRoyaleEventPlayTimerTable()
		|| !LoadCswEventPlayTimerTable()
		|| !LoadServerSettingsData()
		|| !LoadPetStatsInfoTable()
		|| !LoadPetImageChangeTable()
		|| !LoadQuestSkillSetUpTable()
		|| !LoadMonsterRespawnVeriableListTable()
		|| !LoadMonsterRespawnStableListTable()
		/*|| !LoadMonsterDropMainTable()
		|| !LoadMonsterItemGroupItemTable()*/
		|| !LoadUserKillGiftArray()
		|| !LoadBotTable()
		|| !LoadSettings()
		|| !LoadPusLoad()
		|| !LoadCollectionRaceSettings()
		|| !LoadCollectionRaceHuntList()
		//|| !LoadPPCardTable()
		|| !LoadNpcPosTable()
		|| !CreateNpcThread()
		|| !ChaosStoneLoad())
		return false;

	foreach_stlmap(itr, SettingsArray)
		Settings = itr->second;

	if (Settings == nullptr)
	{
		printf(" Settings Tables Nullptr \n");
		return false;
	}
		
	CollectionRaceSetting = CollectionRaceSettingsArray.GetData(m_nServerNo);
	if (CollectionRaceSetting == nullptr)
	{
		printf(" CollectionRace Settings Tables Nullptr \n");
		return false;
	}

	CollectionRaceHuntList = CollectionRaceHuntListArray.GetData(m_nServerNo);
	if (CollectionRaceHuntList == nullptr)
	{
		printf(" CollectionRace Hunt List Tables Nullptr \n");
		return false;
	}
	return true;
}
#pragma endregion

#pragma region CGameServerDlg::CreateDirectories()

void CGameServerDlg::CreateDirectories()
{
	// Logs Start
	CreateDirectory("Logs",NULL);
	CreateDirectory("Logs/BanLogs",NULL);
	CreateDirectory("Logs/ChatLogs",NULL);
	CreateDirectory("Logs/CheatLogs",NULL);
	CreateDirectory("Logs/DeathNpcLogs",NULL);
	CreateDirectory("Logs/DeathUserLogs",NULL);
	CreateDirectory("Logs/ItemLogs",NULL);
	CreateDirectory("Logs/MerchantLogs",NULL);
	CreateDirectory("Logs/TradeLogs",NULL);
	CreateDirectory("Logs/LetterLogs",NULL);
	CreateDirectory("Logs/ItemUpgradeLogs",NULL);
	CreateDirectory("Logs/CharacterIpLogs", NULL);
	CreateDirectory("Logs/TournamentLogs", NULL);
	CreateDirectory("Logs/OfflineSystemLogs", NULL);
	CreateDirectory("Logs/HookPusSystemLogs", NULL);
}

#pragma endregion

void CGameServerDlg::GetEventAwardsIni()
{
	CIni ini(CONF_EVENT_AWARDS);
	/*Chaos Expansion Defense Ödülleri*/
	m_sChaosFinishItem = ini.GetInt("CHAOS_EXPANSION", "WINNING_1|2|3_01", BLUE_TREASURE_CHEST);
	m_sChaosFinishItems = ini.GetInt("CHAOS_EXPANSION", "WINNING_1|2|3_02", VOUCHER_OF_ORACLE);
	m_sChaosFinishItem1 = ini.GetInt("CHAOS_EXPANSION", "WINNING_4|5|6|7", GREEN_TREASURE_CHEST);
	m_sChaosFinishItem2 = ini.GetInt("CHAOS_EXPANSION", "WINNING_8|9|10", RED_TREASURE_CHEST);
	m_sChaosFinishItem3 = ini.GetInt("CHAOS_EXPANSION", "WINNING_NEXT_01", VOUCHER_OF_CHAOS);
	m_sChaosFinishItem4 = ini.GetInt("CHAOS_EXPANSION", "WINNING_NEXT_02", LUNAR_ORDER_TOKEN);
	/*Border Defance War Ödülleri*/
	m_sBorderFinishItem = ini.GetInt("BORDER_DEFANCE_WAR", "WINNING_ITEM_01", LUNAR_ORDER_TOKEN);
	m_sBorderFinishItem1 = ini.GetInt("BORDER_DEFANCE_WAR", "WINNING_ITEM_02", RED_TREASURE_CHEST);
	m_sBorderFinishItem2 = ini.GetInt("BORDER_DEFANCE_WAR", "WINNING_ITEM_03", CERTIFICATE_OF_VICTORY);
	m_sBorderFinishItem3 = ini.GetInt("BORDER_DEFANCE_WAR", "WINNING_LOYALTY", 500);
	/*Border Defance War Ödülleri*/
	m_sBorderFinishItem4 = ini.GetInt("BORDER_DEFANCE_WAR", "LOSER_ITEM_01", LUNAR_ORDER_TOKEN);
	m_sBorderFinishItem5 = ini.GetInt("BORDER_DEFANCE_WAR", "LOSER_LOYALTY", 100);
	/*Juraid Mountain Defense Ödülleri*/
	m_sJuraidFinishItem	= ini.GetInt("JURAID_MOUNTAIN", "WINNING_ITEM_01", SILVERY_GEM);
	m_sJuraidFinishItem1 = ini.GetInt("JURAID_MOUNTAIN", "LOSER_ITEM_01", BLACK_GEM);
	/*Castle Siege Warfare Defense Ödülleri*/
	m_sCastleSiegeWarDeathMatchWinner = ini.GetInt("CASTLE_SIEGE_WARFARE", "DEATH_MATCH_WINNER_01", 914006000);
	m_sCastleSiegeWarDeathMatchWinner1 = ini.GetInt("CASTLE_SIEGE_WARFARE", "DEATH_MATCH_WINNER_02", 914007000);
	m_sCastleSiegeWarDeathMatchWinner2 = ini.GetInt("CASTLE_SIEGE_WARFARE", "DEATH_MATCH_WINNER_03", 914007000);
	m_sCastleSiegeWarDeathMatchWinner3 = ini.GetInt("CASTLE_SIEGE_WARFARE", "DEATH_MATCH_WINNER_LOYALTY", 1000);
	/*Castle Siege Warfare Defense Ödülleri*/
	m_sCastleSiegeWarWinner = ini.GetInt("CASTLE_SIEGE_WARFARE", "WINNING_LOYALTY", 3000);
	m_sCastleSiegeWarWinner1 = ini.GetInt("CASTLE_SIEGE_WARFARE", "WINNING_KCPOINT", 300);
	m_sCastleSiegeWarWinner2 = ini.GetInt("CASTLE_SIEGE_WARFARE", "LOSER_LOYALTY", 500);
	m_sCastleSiegeWarWinner3 = ini.GetInt("CASTLE_SIEGE_WARFARE", "LOSER_KCPOINT", 100);
	/*Knight Royal Defense Ödülleri*/
	m_sKnightRoyalWinner = ini.GetInt("KNIGHT_ROYAL", "WINNING_01", 930670000);
}

void CGameServerDlg::GetChatIni()
{
	CIni ini(CONF_CHAT);
	/*ini.GetString("LOGIN NOTICE","General Chat 1","This server is protected by current N4CSGuard",m_GirisNotice1);
	ini.GetString("LOGIN NOTICE","General Chat 2","Welcome to [ KnightOnline (-_-) ] , please report bugs.",m_GirisNotice2);
	ini.GetString("LOGIN NOTICE","General Chat 3","[ONEMLI BILGI] : Karakter ID & PW'nizi Asla Paylaşmayın ! Olacak Sorunlar İçin Sorumluluk Tamamen Size Ait !",m_GirisNotice3);

	ini.GetString("LOGIN NOTICE","Private Chat Content(içerik) 1","All information about our 'DEATHKO' is available on our forum page. forum.thedeathko.com",m_pmicerik1);
	ini.GetString("LOGIN NOTICE","Private Chat Content(içerik) 2","'F10' by pressing the button to increase your game experience can reach settings. You can also reach the details of the quests with the 'J' key.",m_pmicerik2);
	ini.GetString("LOGIN NOTICE","Private Chat Content(içerik) 3","'F10' Tuşuna Basarak Oyun Deneyiminizi Arttıracak Ayarlara Ulaşabilirsiniz. Ayrıca 'J' Tuşu İle Görevlerle Alakalı Detaylara Ulaşablirsiniz.",m_pmicerik3);
	ini.GetString("LOGIN NOTICE","Private Chat Content(içerik) 4","DEATHKO sunucumuzla alakalı bütün bilgilere forum sayfamızdan ulaşabilirsiniz. forum.thedeathko.com",m_pmicerik4);

	ini.GetString("LOGIN NOTICE","Private Chat Heading(başlık) 1","INFO - 1",m_pmbaslik1);
	ini.GetString("LOGIN NOTICE","Private Chat Heading(başlık) 2","INFO - 2",m_pmbaslik2);
	ini.GetString("LOGIN NOTICE","Private Chat Heading(başlık) 3","INFO - 3",m_pmbaslik3);
	ini.GetString("LOGIN NOTICE","Private Chat Heading(başlık) 4","INFO - 4",m_pmbaslik4);

	m_GirisNoticeActive = ini.GetInt("LOGIN NOTICE ACTIVE", "Active",1);*/

	ini.GetString("EXTRA NOTICE", "Content(içerik) 1", "[GM]HUMAN / [GM]KARUS", Cmd1, false);
	sag_notice_icerik1 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_icerik1, Cmd1.c_str());
	ini.GetString("EXTRA NOTICE", "Content(içerik) 2", "Anticheat active..", Cmd1, false);
	sag_notice_icerik2 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_icerik2, Cmd1.c_str());
	ini.GetString("EXTRA NOTICE", "Content(içerik) 3", "klasgame.com/N4CSGuard", Cmd1, false);
	sag_notice_icerik3 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_icerik3, Cmd1.c_str());

	ini.GetString("EXTRA NOTICE", "Heading(başlık) 1", "GM List", Cmd1, false);
	sag_notice_baslik1 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_baslik1, Cmd1.c_str());
	ini.GetString("EXTRA NOTICE", "Heading(başlık) 2", "N4CSGuard", Cmd1, false);
	sag_notice_baslik2 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_baslik2, Cmd1.c_str());
	ini.GetString("EXTRA NOTICE", "Heading(başlık) 3", "E-Pin Satış Noktası", Cmd1, false);
	sag_notice_baslik3 = new char[Cmd1.length() + 1];
	std::strcpy(sag_notice_baslik3, Cmd1.c_str());
}

/**
* @brief	Loads the server's config from the INI file.
*/
void CGameServerDlg::GetTimeFromIni()
{
	CIni ini(CONF_GAME_SERVER);
	int year = 0, month = 0, date = 0, hour = 0, server_count = 0, sgroup_count = 0, i = 0;
	char ipkey[20];

	// This is so horrible.
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);

	m_bMarsEnabled = ini.GetBool("ODBC", "GAME_MARS", false);

	ini.GetString("ODBC", "ACCOUNT_DSN", "KO_MAIN", m_strAccountDSN, false);
	ini.GetString("ODBC", "ACCOUNT_UID", "username", m_strAccountUID, false);
	ini.GetString("ODBC", "ACCOUNT_PWD", "password", m_strAccountPWD, false);

	bool bMarsEnabled = ini.GetBool("ODBC", "ACCOUNT_MARS", false);

	// Both need to be enabled to use MARS.
	if (!m_bMarsEnabled 
		|| !bMarsEnabled)
		m_bMarsEnabled = false;
	
	m_byWeather = ini.GetInt("TIMER", "WEATHER", 1);
	m_nCastleCapture = ini.GetInt("CASTLE", "NATION", 1);
	m_nServerNo = ini.GetInt("ZONE_INFO", "MY_INFO", 1);
	m_nServerGroup = ini.GetInt("ZONE_INFO", "SERVER_NUM", 0);
	server_count = ini.GetInt("ZONE_INFO", "SERVER_COUNT", 1);
	if (server_count < 1)
	{
		printf("ERROR: Invalid SERVER_COUNT property in INI.\n");
		return;
	}

	for (i = 0; i < server_count; i++)
	{
		_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
		sprintf(ipkey, "SERVER_%02d", i);
		pInfo->sServerNo = ini.GetInt("ZONE_INFO", ipkey, 1);
		sprintf(ipkey, "SERVER_IP_%02d", i);
		ini.GetString("ZONE_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);
		m_ServerArray.PutData(pInfo->sServerNo, pInfo);
	}

	if (m_nServerGroup != 0)
	{
		m_nServerGroupNo = ini.GetInt("SG_INFO", "GMY_INFO", 1);
		sgroup_count = ini.GetInt("SG_INFO", "GSERVER_COUNT", 1);
		if (server_count < 1)
		{
			printf("ERROR: Invalid GSERVER_COUNT property in INI.\n");
			return;
		}

		for (i = 0; i < sgroup_count; i++)
		{
			_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
			sprintf(ipkey, "GSERVER_%02d",i );
			pInfo->sServerNo = ini.GetInt("SG_INFO", ipkey, 1);
			sprintf(ipkey, "GSERVER_IP_%02d", i);
			ini.GetString("SG_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);

			m_ServerGroupArray.PutData(pInfo->sServerNo, pInfo);
		}
	}

	m_GameServerPort = ini.GetInt("SETTINGS","PORT", 15001);
	m_byMaxLevel = ini.GetInt("SETTINGS","GAMEMAXLEVEL", 83);
	m_nGameMasterRHitDamage = ini.GetInt("SETTINGS","GAME_MASTER_R_HIT_DAMAGE", 30000);
	m_nPlayerRankingResetTime = ini.GetInt("SETTINGS","PLAYER_RANKINGS_RESET_TIME", 12);
	ini.GetString("SETTINGS", "PLAYER_RANKINGS_REWARD_ZONES","71,72,73", m_sPlayerRankingsRewardZones, false);
	m_nPlayerRankingKnightCashReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_KNIGHT_CASH_REWARD", 0);
	m_nPlayerRankingLoyaltyReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_LOYALTY_REWARD", 0);

	m_Grade1 = ini.GetInt("CLAN_GRADE","GRADE1", 720000);
	m_Grade2 = ini.GetInt("CLAN_GRADE","GRADE2", 360000);
	m_Grade3 = ini.GetInt("CLAN_GRADE","GRADE3", 144000);
	m_Grade4 = ini.GetInt("CLAN_GRADE","GRADE4", 72000);

	m_byExpEventAmount = ini.GetInt("BONUS","EXP", 0);
	m_byCoinEventAmount = ini.GetInt("BONUS","MONEY", 0);
	m_byNPEventAmount = ini.GetInt("BONUS","NP", 0);

	m_Loyalty_Ardream_Source = 	ini.GetInt("NATIONAL_POINTS","ARDREAM_SOURCE",32);
	m_Loyalty_Ardream_Target = ini.GetInt("NATIONAL_POINTS","ARDREAM_TARGET",-25);
	m_Loyalty_Ronark_Land_Base_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_SOURCE",64);
	m_Loyalty_Ronark_Land_Base_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_TARGET",-50);
	m_Loyalty_Ronark_Land_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_SOURCE",64);
	m_Loyalty_Ronark_Land_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_TARGET",-50);
	m_Loyalty_Other_Zone_Source = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_SOURCE",64);
	m_Loyalty_Other_Zone_Target = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_TARGET",-50);

	m_sKarusMilitary = ini.GetInt("MILITARY_CAMP","KARUS_ZONE",3);
	m_sKarusEslantMilitary = ini.GetInt("MILITARY_CAMP", "KARUS_ESLANT_ZONE", 3);
	m_sHumanMilitary = ini.GetInt("MILITARY_CAMP","HUMAN_ZONE",3);
	m_sHumanEslantMilitary = ini.GetInt("MILITARY_CAMP", "HUMAN_ESLANT_ZONE", 3);
	m_sMoradonMilitary = ini.GetInt("MILITARY_CAMP","MORADON_ZONE",5);

	m_sCapeAC = ini.GetInt("SUPERIOR_CAPE_BONUS", "DEFANCE_BONUS", 5);
	m_sCapeAtk = ini.GetInt("SUPERIOR_CAPE_BONUS", "ATTACK_BONUS", 10);
	m_sCapeHp = ini.GetInt("SUPERIOR_CAPE_BONUS", "HEALTH_BONUS", 15);
	m_sCapeNp = ini.GetInt("SUPERIOR_CAPE_BONUS", "LOYALTY_BONUS", 20);

	/* Wanted List Event */
	WantedEventSystemStatus = ini.GetBool("WANTED_EVENT", "STATUS", true);//
	WantedEventSystemWinItem = ini.GetInt("WANTED_EVENT", "WIN_ITEM", 914052000);
	WantedEventSystemWinItemCount = ini.GetInt("WANTED_EVENT", "WIN_ITEM_COUNT", 1);
	WantedEventSystemWinItemDays = ini.GetInt("WANTED_EVENT", "WIN_ITEM_DAYS", 0);
	WantedEventSystemWinNP = ini.GetInt("WANTED_EVENT", "WIN_NATIONAL_POINT", 100);
	WantedEventSystemWinKC = ini.GetInt("WANTED_EVENT", "WIN_CASH_POINT", 10);
	WantedEventSystemWinNationNP = ini.GetInt("WANTED_EVENT", "NATIONS_NP_POINT", 50);

	Dakika1 = ini.GetInt("ONLINE_SYSTEM", "PK Min", 60);
	Dakika2 = ini.GetInt("ONLINE_SYSTEM", "Moradon Min", 60);
	HediyeKC = ini.GetInt("ONLINE_SYSTEM", "Moradon KC", 5);
	HediyeNP = ini.GetInt("ONLINE_SYSTEM", "Moradon NP", 5);
	HediyeKC2 = ini.GetInt("ONLINE_SYSTEM", "PK KC", 15);
	HediyeNP2 = ini.GetInt("ONLINE_SYSTEM", "PK NP", 15);

	/* Game */
	RoyalG1 = ini.GetBool("GAME", "AutoRoyalG1", 1);
	MaxLevel62Control = ini.GetInt("GAME", "MAXLV62CONTROL", 0);

	m_sJackExpPots = ini.GetInt("JACK_POTS","JACK_EXP_POT",0);
	m_sJackGoldPots = ini.GetInt("JACK_POTS","JACK_NOAH_POT",0);

	/* Pm Event */
	m_ChatEventCashPoint = ini.GetInt("PM_EVENT", "KC MIKTARI", 5);
	ChatEventDakika = ini.GetInt("PM_EVENT", "MINUTE", 5);

	if (m_sKarusMilitary > 3
		|| m_sKarusEslantMilitary > 3
		|| m_sHumanMilitary > 3
		|| m_sHumanEslantMilitary > 3
		|| m_sMoradonMilitary > 5)
	{
		if (m_sKarusMilitary > 3)
			m_sKarusMilitary = 3;

		if (m_sKarusEslantMilitary > 3)
			m_sKarusEslantMilitary = 3;

		if (m_sHumanMilitary > 3)
			m_sHumanMilitary = 3;

		if (m_sHumanEslantMilitary > 3)
			m_sHumanEslantMilitary = 3;

		if (m_sMoradonMilitary > 5)
			m_sMoradonMilitary = 5;
	}

	if (m_sCapeAC > 100
		|| m_sCapeAtk > 100
		|| m_sCapeHp > 100
		|| m_sCapeNp > 100)
	{
		if (m_sCapeAC > 100)
			m_sCapeAC = 100;

		if (m_sCapeAtk > 100)
			m_sCapeAtk = 100;

		if (m_sCapeHp > 100)
			m_sCapeHp = 100;

		if (m_sCapeNp > 100)
			m_sCapeNp = 100;
	}

	m_sRankResetHour = 0;
	m_GameServerShutDownSeconds = 0;
	m_GameServerShutDownOK = false;

	m_GameInfo1Time = 300;
	m_GameInfo2Time = 450;
	m_GameInfo3Time = 120;

	m_UserPlayerKillingZoneRankingArray[0].DeleteAllData();
	m_UserPlayerKillingZoneRankingArray[1].DeleteAllData();
	m_UserBorderDefenceWarRankingArray[0].DeleteAllData();
	m_UserBorderDefenceWarRankingArray[1].DeleteAllData();
	m_UserChaosExpansionRankingArray.DeleteAllData();

	ChaosStoneRespawnOkey = true;

	m_xBifrostRemainingTime = (240 * MINUTE);		// Bifrost remaining time ( 4 hour ).
	m_xBifrostMonumentAttackTime = (30 * MINUTE);	// Players is attack a monument last 30 minute.
	m_xBifrostTime = (120 * MINUTE);				// Victory nation time ( 1 hour and 30 minute )
	m_xJoinOtherNationBifrostTime = (60 * MINUTE);	// Other nation join time ( last 1 hour )
	m_bAttackBifrostMonument = false;
	m_BifrostVictory = 0;
	m_sBifrostRemainingTime = m_xBifrostRemainingTime + 60;
	m_sBifrostTime = 0;
	m_sBifrostWarStart = false;
	m_sBifrostVictoryAll = 3;
	m_sBifrostVictoryNoticeAll = false;

	m_byBattleOpenedTime = 0;
	m_byBattleNoticeTime = 0;
	m_byBattleTime = (2 * 60) * 60;					// 2 Hours

	m_IsMagicTableInUpdateProcess = false;
	m_IsPlayerRankingUpdateProcess = false;

	m_nPVPMonumentNation[ZONE_RONARK_LAND] = 0;

	//Monster Stone EventRoom
	m_MonsterStoneSquadEventRoom = MAX_TEMPLE_QUEST_EVENT_ROOM;
	m_MonsterStoneEventRoom = MAX_TEMPLE_EVENT_ROOM;

	m_CurrentNPC = 0;
	m_sMapEventNpc = 0;
	m_TotalNPC = 0;
	m_FreeNpcList.clear();
	for (int i = NPC_BAND; i < INVALID_BAND; i++)
		m_FreeNpcList.push_back(uint32(i));

	//Soccer Event System
	pSoccerEvent.m_SoccerActive		= false;
	pSoccerEvent.m_SoccerTimer		= false;
	pSoccerEvent.m_SoccerSocketID	= -1;
	pSoccerEvent.m_SoccerTime		= 0;
	pSoccerEvent.m_SoccerTimers		= 0;
	pSoccerEvent.m_SoccerRedColour	= 0;
	pSoccerEvent.m_SoccerBlueColour = 0;
	pSoccerEvent.m_SoccerBlueGool	= 0;
	pSoccerEvent.m_SoccerRedGool	= 0;

	//Event system
	for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
	{
		_JURAID_ROOM_INFO *pRoomInfo = new _JURAID_ROOM_INFO();
		pRoomInfo->Initialize();
		m_TempleEventJuraidRoomList.PutData(i, pRoomInfo);
	}

	for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
	{
		_BDW_ROOM_INFO *pRoomInfo = new _BDW_ROOM_INFO();
		pRoomInfo->Initialize();
		m_TempleEventBDWRoomList.PutData(i, pRoomInfo);
	}

	for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
	{
		_CHAOS_ROOM_INFO *pRoomInfo = new _CHAOS_ROOM_INFO();
		pRoomInfo->Initialize();
		m_TempleEventChaosRoomList.PutData(i, pRoomInfo);
	}

	for (int i = 1; i <= 3; i++)
	{
		_CHAOS_STONE_INFO *pRoomInfo = new _CHAOS_STONE_INFO();
		pRoomInfo->Initialize();
		pRoomInfo->sChaosIndex = i;
		m_ChaosStoneInfoArray.PutData(i, pRoomInfo);
	}

	if (m_MonsterUnderTheCastleList.GetSize() == 0)
	{
		_UNDER_THE_CASTLE_INFO *pRoomInfo = new _UNDER_THE_CASTLE_INFO();
		pRoomInfo->Initialize();
		m_MonsterUnderTheCastleList.PutData(1, pRoomInfo);
	}

	if (m_MonsterDrakiTowerList.GetSize() == 0)
	{
		for (int i = 1; i <= EVENTMAXROOM; i++)
		{
			_DRAKI_TOWER_INFO *pRoomInfo = new _DRAKI_TOWER_INFO();
			pRoomInfo->Initialize();
			pRoomInfo->m_tDrakiRoomID = i;
			m_MonsterDrakiTowerList.PutData(pRoomInfo->m_tDrakiRoomID, pRoomInfo);
		}
	}

	if (m_DungeonDefenceRoomListArray.GetSize() == 0)
	{
		for (int i = 1; i <= EVENTMAXROOM; i++)
		{
			_DUNGEON_DEFENCE_ROOM_INFO *pRoomInfo = new _DUNGEON_DEFENCE_ROOM_INFO();
			pRoomInfo->Initialize();
			pRoomInfo->m_DefenceRoomID = i;
			m_DungeonDefenceRoomListArray.PutData(pRoomInfo->m_DefenceRoomID, pRoomInfo);
		}
	}

	if (m_KnightRoyaleRoomInfo.GetSize() == 0)
	{
		_KNIGHT_ROYALE_INFO *pRoomInfo = new _KNIGHT_ROYALE_INFO();
		pRoomInfo->Initialize();
		m_KnightRoyaleRoomInfo.PutData(1, pRoomInfo);
	}

	m_nVirtualEventRoomRemainSeconds = 0;
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.ClosedTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;
	pTempleEvent.EventTimerStartControl = false;
	pTempleEvent.EventTimerAttackOpenControl = false;
	pTempleEvent.EventTimerAttackCloseControl = false;
	pTempleEvent.EventTimerFinishControl = false;
	pTempleEvent.EventTimerResetControl = false;
	pTempleEvent.EventCloseMainControl = false;
	m_TempleEventLastUserOrder = 1;

	pEventBridge.isBridgeSystemActive = false;
	pEventBridge.isBridgeTimerControl1 = false;
	pEventBridge.isBridgeTimerControl2 = false;
	pEventBridge.isBridgeTimerControl3 = false;
	pEventBridge.isBridgeSystemStartMinutes = 0;

	pCswEvent.Started = false;
	pCswEvent.isMonumentFinish = false;
	pCswEvent.isMonumentActive = false;
	pCswEvent.BarrackCreateTimerControl = false;
	pCswEvent.DeathMatchTimerControl = false;
	pCswEvent.PreparationTimerControl = false;
	pCswEvent.CastellanWarTimerControl = false;
	pCswEvent.Status = CswOperationStatus::NotOperation;
	pCswEvent.CswTime = 0;
	pCswEvent.MonumentTime = 0;
	pCswEvent.MonumentType = 0;
	pCswEvent.BarrackNoticeTime = 0;
	pCswEvent.DeathMatchNoticeTime = 0;
	pCswEvent.PreparationNoticeTime = 0;
	pCswEvent.CastellanWarNoticeTime = 0;
	pCswEvent.MonumentNoticeTime = 0;
	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;

	m_nKnightRoyaleEventRemainSeconds = 0;
	pKnightRoyaleEvent.ActiveEvent = -1;
	pKnightRoyaleEvent.isActive = false;
	pKnightRoyaleEvent.AllowJoin = false;
	pKnightRoyaleEvent.AllUserCount = 0;
	pKnightRoyaleEvent.EventTimerStartControl = false;
	pKnightRoyaleEvent.StartTime = 0;
	pKnightRoyaleEvent.CloseTime = 0;
	pKnightRoyaleEvent.EventCloseMainControl = false;
	m_KnightRoyaleLastUserRequestID = 1;

	//Forgetten Temple
	m_bForgettenTempleIsActive = false;
	m_nForgettenTempleStartHour = 0;
	m_nForgettenTempleLevelMin = 0;
	m_nForgettenTempleLevelMax = 0;
	m_nForgettenTempleStartTime = 0;
	m_nForgettenTempleChallengeTime = 0;
	m_bForgettenTempleSummonMonsters = false;
	m_nForgettenTempleCurrentStage = 0;
	m_nForgettenTempleLastStage = 0;
	m_nForgettenTempleLastSummonTime = 0;
	m_nForgettenTempleBanishFlag = false;
	m_nForgettenTempleBanishTime = 0;

	// Under The Castle
	m_bUnderTheCastleIsActive = false;
	m_bUnderTheCastleMonster = false;
	m_nUnderTheCastleEventTime = 0;
	m_checktime = 60 * SECOND;

	//CollectionRace
	CollectionRaceCounts = 0;
	CollectionRaceFinishTime = 0;
	CollectionRaceGameServerSatus = false;
	memset(CollectionRaceQuestID, 0, sizeof(CollectionRaceQuestID));
	memset(CollectionRaceQuestZone, 0, sizeof(CollectionRaceQuestZone));
	memset(CollectionRaceMonsterNum1, 0, sizeof(CollectionRaceMonsterNum1));
	memset(CollectionRaceMonsterNum2, 0, sizeof(CollectionRaceMonsterNum2));
	memset(CollectionRaceMonsterNum3, 0, sizeof(CollectionRaceMonsterNum3));
	memset(CollectionRaceMonsterNum4, 0, sizeof(CollectionRaceMonsterNum4));
	memset(CollectionRaceMonsterKillCount1, 0, sizeof(CollectionRaceMonsterKillCount1));
	memset(CollectionRaceMonsterKillCount2, 0, sizeof(CollectionRaceMonsterKillCount2));
	memset(CollectionRaceMonsterKillCount3, 0, sizeof(CollectionRaceMonsterKillCount3));
	memset(CollectionRaceMonsterKillCount4, 0, sizeof(CollectionRaceMonsterKillCount4));
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CGameServerDlg::GetServerResource(int nResourceID, string * result, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
	{
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

/**
* @brief	Gets the starting positions (for both nations) 
* 			for the specified zone.
*
* @param	nZoneID	Identifier for the zone.
*/
_START_POSITION *CGameServerDlg::GetStartPosition(int nZoneID)
{
	return m_StartPositionArray.GetData(nZoneID);
}

/**
* @brief	Gets the experience points required for the 
* 			specified level.
*
* @param	nLevel	The level.
*
* @return	The experience points required to level up from 
* 			the specified level.
*/
int64 CGameServerDlg::GetExpByLevel(int nLevel, int RebithLevel)
{
	foreach(itr, g_pMain->m_LevelUpArray)
	{
		if (nLevel < g_pMain->m_byMaxLevel && RebithLevel > 0){
			if (itr->second->Level == nLevel && itr->second->RebithLevel == 0)
				return itr->second->Exp;
		}
		else {
			if (itr->second->Level == nLevel && itr->second->RebithLevel == RebithLevel)
				return itr->second->Exp;
		}
	}
	return 0;
}

/**
* @brief	Gets zone by its identifier.
*
* @param	zoneID	Identifier for the zone.
*
* @return	null if it fails, otherwise the zone.
*/
C3DMap * CGameServerDlg::GetZoneByID(int zoneID)
{
	return m_ZoneArray.GetData(zoneID);
}

/**
* @brief	Looks up a user by name.
*
* @param	findName	The name to find.
* @param	type		The type of name (account, character).
*
* @return	null if it fails, else the user pointer.
*/
CBot* CGameServerDlg::GetBotPtr(string findName, NameType type)
{
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	BotNameMap::iterator itr;
	if (type == TYPE_CHARACTER)
	{
		Guard lock(m_BotcharacterNameLock);
		itr = m_BotcharacterNameMap.find(findName);
		return (itr != m_BotcharacterNameMap.end() ? itr->second : nullptr);
	}

	return nullptr;
}

/**
* @brief	Looks up a user by name.
*
* @param	findName	The name to find.
* @param	type		The type of name (account, character).
*
* @return	null if it fails, else the user pointer.
*/
CUser* CGameServerDlg::GetUserPtr(string findName, NameType type)
{
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	NameMap::iterator itr;
	if (type == TYPE_ACCOUNT)
	{
		Guard lock(m_accountNameLock);
		itr = m_accountNameMap.find(findName);
		return (itr != m_accountNameMap.end() ? itr->second : nullptr);
	}
	else if (type == TYPE_CHARACTER)
	{
		Guard lock(m_characterNameLock);
		itr = m_characterNameMap.find(findName);
		return (itr != m_characterNameMap.end() ? itr->second : nullptr);
	}

	return nullptr;
}

/**
* @brief	Adds the account name & session to a hashmap (on login)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddAccountName(CUser *pSession)
{
	Guard lock(m_accountNameLock);
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);
	m_accountNameMap[upperName] = pSession;
}

/**
* @brief	Adds the character name & session to a hashmap (when in-game)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddCharacterName(CUser *pSession)
{
	Guard lock(m_characterNameLock);
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap[upperName] = pSession;
}

/**
* @brief	Removes an existing character name/session from the hashmap, 
* 			replaces the character's name and reinserts the session with 
* 			the new name into the hashmap.
*
* @param	pSession		The session.
* @param	strNewUserID	Character's new name.
*/
void CGameServerDlg::ReplaceCharacterName(CUser *pSession, std::string & strNewUserID)
{
	Guard lock(m_characterNameLock);

	// Remove the old name from the map
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap.erase(upperName);

	// Update the character's name & re-add them to the map.
	pSession->m_strUserID = strNewUserID;
	AddCharacterName(pSession);
}

/**
* @brief	Removes the account name & character names from the hashmaps (on logout)
*
* @param	pSession	The session.
*/
void CGameServerDlg::RemoveSessionNames(CUser *pSession)
{
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);

	{ // remove account name from map (limit scope)
		Guard lock(m_accountNameLock);
		m_accountNameMap.erase(upperName);
	}

	if (pSession->isInGame())
	{
		upperName = pSession->GetName();
		STRTOUPPER(upperName);

		Guard lock(m_characterNameLock);
		m_characterNameMap.erase(upperName);
	}
}

CUser				* CGameServerDlg::GetUserPtr(uint16 sUserId) { return m_socketMgr[sUserId]; }
CNpc				* CGameServerDlg::GetNpcPtr(uint16 sNpcId, uint16 sZoneID)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(sZoneID);
	if (zoneitrThread == nullptr)
		return nullptr;

	return zoneitrThread->m_arNpcArray.GetData(sNpcId);
}
_PARTY_GROUP		* CGameServerDlg::GetPartyPtr(uint16 sPartyID) { return m_PartyArray.GetData(sPartyID); }
CKnights			* CGameServerDlg::GetClanPtr(uint16 sClanID) { return m_KnightsArray.GetData(sClanID); }
_KNIGHTS_ALLIANCE	* CGameServerDlg::GetAlliancePtr(uint16 sAllianceID) { return m_KnightsAllianceArray.GetData(sAllianceID); }
_ITEM_TABLE			* CGameServerDlg::GetItemPtr(uint32 nItemID) { return m_ItemtableArray.GetData(nItemID); }
_KNIGHTS_SIEGE_WARFARE	* CGameServerDlg::GetSiegeMasterKnightsPtr(uint16 sCastleIndex) { return m_KnightsSiegeWarfareArray.GetData(sCastleIndex); }

Unit * CGameServerDlg::GetUnitPtr(uint16 id, uint16 sZoneID /*= 0*/)
{
	if (id < NPC_BAND)
		return GetUserPtr(id);

	if (sZoneID == 0)
		return nullptr;

	return GetNpcPtr(id, sZoneID);
}

#pragma region CGameServerDlg::SpawnEventNpc

/**
* @brief	Spawn one or more event NPC/monsters.
*
* @param	sSid	  	The database ID of the NPC/monster to spawned.
* @param	bIsMonster	true if we are spawning a monster, false for an NPC.
* @param	byZone	  	The zone ID to spawn the monster to.
* @param	fX		  	The x coordinate.
* @param	fY		  	The y coordinate.
* @param	fZ		  	The z coordinate.
* @param	sCount	  	Number of spawns to create.
* @param	sRadius	  	Spawn radius.
* @param	byDirection The rotation of the monster
*/
void CGameServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ,
	uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, uint16 sDuration /*= 0*/,
	uint8 nation /*= 0*/, int16 socketID /*= -1*/, uint16 nEventRoom /* = 0 */,
	uint8 byDirection /* = 0 */, uint8 bMoveType /* = 0 */, uint8 bGateOpen/* = 0 */,
	uint16 nSummonSpecialType/* = 0 */, uint32 nSummonSpecialID/* = 0 */)
{

	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->AddRequest(new Signal(new AddNPCSignals(sSid, bIsMonster, byZone, fX, fY, fZ, sCount, sRadius, sDuration,
		nation, socketID, nEventRoom, byDirection, bMoveType, bGateOpen, nSummonSpecialType, nSummonSpecialID)));
}

#pragma endregion

#pragma region CGameServerDlg::SpawnEventPet

/**
* @brief	Spawn one or more event NPC/monsters.
*
* @param	sSid	  	The database ID of the PET to spawned.
* @param	bIsMonster	true if we are spawning a monster, false for an NPC.
* @param	byZone	  	The zone ID to spawn the monster to.
* @param	fX		  	The x coordinate.
* @param	fY		  	The y coordinate.
* @param	fZ		  	The z coordinate.
* @param	sCount	  	Number of spawns to create.
* @param	sRadius	  	Spawn radius.
* @param	byDirection The rotation of the monster
*/
void CGameServerDlg::SpawnEventPet(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ,
	uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, uint16 sDuration /*= 0*/,
	uint8 nation /*= 0*/, int16 socketID /*= -1*/, uint16 nEventRoom /* = 0 */,
	uint8 nType /* = 0 */, uint32 nSkillID /* = 0 */)
{

	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->AddRequest(new Signal(new AddPETSignals(sSid, bIsMonster, byZone, fX, fY, fZ, sCount, sRadius, sDuration,
		nation, socketID, nEventRoom, nType, nSkillID)));
	
}

#pragma endregion

#pragma region CGameServerDlg::RemoveAllEventNpc

/**
* @brief	Removes all the NPC/monsters in the speficied event zone.
*
* @param	byZone	  	The zone ID to remote the monster from.

*/
void CGameServerDlg::RemoveAllEventNpc(uint8 byZone)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->AddRequest(new Signal(NpcThreadSignalType::NpcAllRemove));

	if (byZone == ZONE_BORDER_DEFENSE_WAR)
		zoneitrThread->AddRequest(new Signal(NpcThreadSignalType::NpcAllReset));
}

#pragma endregion

#pragma region CGameServerDlg::ResetAllEventObject(uint8 byZone)

/**
* @brief	Removes all the object in the speficied event zone.
*
* @param	byZone	  	The zone ID to remote the monster from.
*/
void CGameServerDlg::ResetAllEventObject(uint8 byZone)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->AddRequest(new Signal(NpcThreadSignalType::NpcAllReset));
}


#pragma endregion

#pragma region CGameServerDlg::KillNpc(uint16 sNid, uint8 byZone, Unit *pKiller)

/**
* @brief	Kills the NPC specified.
*
* @param	sNid	the NPC ID
*/
void CGameServerDlg::KillNpc(uint16 sNid, uint8 byZone, Unit *pKiller)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);

	if (zoneitrThread == nullptr)
		return;

	if (sNid < NPC_BAND)
	{
		foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
		{
			CNpc *pNpc = TO_NPC(itr->second);
			if (pNpc == nullptr)
				continue;

			if (pNpc->m_oSocketID != sNid)
				continue;

			pNpc->m_oSocketID = -1;
			pNpc->DeadReq(pKiller);
		}
	}
	else
	{
		CNpc* pNpc = (pKiller == nullptr ? g_pMain->GetNpcPtr(sNid, byZone) : g_pMain->GetNpcPtr(sNid, pKiller->GetZoneID()));

		if (pNpc)
			pNpc->DeadReq(pKiller);
	}
}

#pragma endregion

#pragma region CGameServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid)

void CGameServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid)
{
	CNpcTable * proto = nullptr;

	if (bIsMonster)
		proto = m_arMonTable.GetData(sSid);
	else
		proto = m_arNpcTable.GetData(sSid);

	if (proto == nullptr)
		return;

	if (byGroup > 0)
		proto->m_byGroupSpecial = byGroup;

	if (sPid > 0)
		proto->m_sPid = sPid;
}

#pragma endregion

#pragma region _PARTY_GROUP* CGameServerDlg::CreateParty(CUser* pLeader)

_PARTY_GROUP* CGameServerDlg::CreateParty(CUser* pLeader)
{
	_PARTY_GROUP * pParty = new _PARTY_GROUP;

	pLeader->m_bInParty = true;
	pLeader->m_sPartyIndex = m_sPartyIndex.increment();

	pParty->wIndex = pLeader->GetPartyID();
	pParty->uid[0] = pLeader->GetSocketID();
	if (!m_PartyArray.PutData(pParty->wIndex, pParty))
	{
		delete pParty;
		pLeader->m_bInParty = false;
		pLeader->m_sPartyIndex = -1;
		pParty = nullptr;
	}


	return pParty;
}

#pragma endregion

#pragma region CGameServerDlg::Timer_CheckGameEvents(void * lpParam)

uint32 CGameServerDlg::Timer_CheckGameEvents(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->GameInfoNoticeTimer();
		g_pMain->GameServerShutDownTimer();
		g_pMain->GameEventMainTimer();
		g_pMain->CollectionRaceFinish();
		g_pMain->AutoOnlineCount();

		sleep(1 * SECOND);
	}
	return 0;
}

#pragma endregion

void CGameServerDlg::GameEventMainTimer()
{
	g_pMain->EventMainTimer();
	g_pMain->TempleSoccerEventTimer();
	g_pMain->TempleMonsterStoneTimer();
	g_pMain->DrakiTowerLimitReset();
	g_pMain->BDWMonumentAltarTimer();
	g_pMain->ClanTournamentTimer();
	g_pMain->DungeonDefenceTimer();
	g_pMain->DrakiTowerRoomCloseTimer();
	g_pMain->KnightRoyaleGasTimer();
	g_pMain->KnightRoyaleTreasureChestSpawnTimer();
	g_pMain->ChaosStoneRespawnTimer();
}

void CGameServerDlg::ResetLoyaltyMonthly()
{
	Packet pkt(WIZ_RESET_LOYALTY);

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (pUser == nullptr 
			|| pUser->isInGame())
			continue;

		pUser->m_iLoyaltyMonthly = 0;
		pUser->SendLoyaltyChange(); // update the client (note: official doesn't bother)
	}

	// Attempt to update the database in this thread directly, while the session map's locked.
	// This prevents new users from logging in before they've been reset (hence keeping last month's total).
	g_pMain->AddDatabaseRequest(pkt);
}

void CGameServerDlg::UpdateWeather()
{
	if (m_byKingWeatherEvent)
	{
		int16 sEventExpiry;
		if (g_localTime.tm_mday == m_byKingWeatherEvent_Day)
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour) - m_byKingWeatherEvent_Minute;
		else
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour + 24) - m_byKingWeatherEvent_Minute;

		// Weather events last for 5 minutes
		if (sEventExpiry > 5)
		{
			m_byKingWeatherEvent = 0;
			m_byKingWeatherEvent_Day = 0;
			m_byKingWeatherEvent_Hour = 0;
			m_byKingWeatherEvent_Minute = 0;
		}
	}
	else
	{
		int weather = 0, rnd = myrand( 0, 100 );
		if (rnd < 2)		weather = WEATHER_SNOW;
		else if (rnd < 7)	weather = WEATHER_RAIN;
		else				weather = WEATHER_FINE;

		m_sWeatherAmount = myrand(0, 100);
		if (weather == WEATHER_FINE)
		{
			if (m_sWeatherAmount > 70)
				m_sWeatherAmount /= 2;
			else
				m_sWeatherAmount = 0;
		}
		m_byWeather = weather;
	}

	// Real weather data for most users.
	Packet realWeather(WIZ_WEATHER, m_byWeather);
	realWeather << m_sWeatherAmount;

	// Fake, clear weather for users in certain zones (e.g. Desp & Hell Abysses, Arena)
	Packet fakeWeather(WIZ_WEATHER, uint8(WEATHER_FINE));
	fakeWeather << m_sWeatherAmount;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame())
			continue;

		if (pUser->GetZoneID() == 32 
			|| pUser->GetZoneID() == 33
			|| pUser->GetZoneID() == 48)
			pUser->Send(&fakeWeather);
		else
			pUser->Send(&realWeather);
	}
}

void CGameServerDlg::SetGameTime()
{
	CIni ini(CONF_GAME_SERVER);
	ini.SetInt( "TIMER", "WEATHER", m_byWeather );
}



void CGameServerDlg::AddDatabaseRequest(Packet & pkt, CUser *pUser /*= nullptr*/)
{
	Packet *newPacket = new Packet(pkt.GetOpcode(), pkt.size() + 2);
	*newPacket << int16(pUser == nullptr ? -1 : pUser->GetSocketID());
	if (pkt.size())
		newPacket->append(pkt.contents(), pkt.size());
	DatabaseThread::AddRequest(newPacket);
}

void CGameServerDlg::AddLogRequest(std::string& LogMsg, LogTypes logtype)
{
	Packet *newPacket = new Packet(WIZ_TEST_PACKET,uint8(logtype));
	*newPacket << LogMsg;
	KnightLogger::AddRequest(newPacket);
}

void CGameServerDlg::HandleConsoleCommand(const char * msg) 
{
	string message = msg;
	if (message.empty())
		return;

	if (ProcessServerCommand(message))
	{
		printf("Command accepted.\n");
		return;
	}

	printf("Invalid command. If you're trying to send a notice, please use /notice\n");
}

bool CGameServerDlg::LoadNoticeData()
{
	ifstream file("./Notice.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_ppNotice, 0, sizeof(m_ppNotice));

	if (!file)
	{
		TRACE("Notice.txt could not be opened.\n");
		printf("Notice.txt could not be opened.\n");
		return false;
	}

	while (!file.eof())
	{
		if (count > 19)
		{
			TRACE("Too many lines in Notice.txt\n");
			printf("Too many lines in Notice.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128)
		{
			TRACE("Notice.txt contains line that exceeds the limit of 128 characters.\n");
			printf("Notice.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_ppNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

bool CGameServerDlg::LoadNoticeUpData()
{
	ifstream file("./Notice_up.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_peNotice, 0, sizeof(m_peNotice));

	if (!file)
	{
		TRACE("Notice_up.txt could not be opened.\n");
		printf("Notice_up.txt could not be opened.\n");
		return false;
	}

	while (!file.eof())
	{
		if (count > 19)
		{
			TRACE("Too many lines in Notice_up.txt\n");
			printf("Too many lines in Notice_up.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128)
		{
			TRACE("Notice_up.txt contains line that exceeds the limit of 128 characters.\n");
			printf("Notice_up.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_peNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

#pragma region CGameServerDlg::FindNpcInZone(uint16 sPid, uint8 byZone)

/**
* @brief	Searches for the first NPC in the specified zone
* 			with the specified picture/model ID.
*
* @param	sPid	Picture/model ID of the NPC.
* @param	byZone	Zone to search in.
*
* @return	null if it fails, else the NPC instance we found.
*/
CNpc*  CGameServerDlg::FindNpcInZone(uint16 sPid, uint8 byZone)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return nullptr;

	foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc * pNpc = itr->second;
		if (pNpc == nullptr || pNpc->GetZoneID() != byZone
			// This isn't a typo, it's actually just a hack.
			// The picture/model ID of most spawns is the same as their prototype ID.
			// When there's more than one spawn prototype (i.e. different sSid), we keep using
			// the same picture/model ID. So we check this instead of the sSid...
			|| pNpc->GetPID() != sPid)
			continue;

		return pNpc;
	}

	return nullptr;
}

#pragma endregion

#pragma region CNpc*  CGameServerDlg::GetPetPtr(int16 m_sPetID, uint8 byZone)

/**
* @brief	Searches for the first PET in the specified zone
* 			with the specified picture/model ID.
*
* @param	sPid	Picture/model ID of the NPC.
* @param	byZone	Zone to search in.
*
* @return	null if it fails, else the NPC instance we found.
*/
CNpc*  CGameServerDlg::GetPetPtr(int16 m_sPetID, uint8 byZone)
{
	CNpcThread* zoneitrThread = m_arNpcThread.GetData(byZone);
	if (zoneitrThread == nullptr)
		return nullptr;

	foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc * pNpc = itr->second;
		if (pNpc == nullptr 
			|| pNpc->GetZoneID() != byZone
			// This isn't a typo, it's actually just a hack.
			// The picture/model ID of most spawns is the same as their prototype ID.
			// When there's more than one spawn prototype (i.e. different sSid), we keep using
			// the same picture/model ID. So we check this instead of the sSid...
			|| pNpc->GetPetID() != m_sPetID)
			continue;

		return pNpc;
	}

	return nullptr;
}

#pragma endregion
void CGameServerDlg::TheEventUserKickOut(uint8 ZoneID)
{
	uint8 nZoneID = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (!pUser->isInGame())
			continue;

		if (ZoneID == ZONE_UNDER_CASTLE)
		{
			if(pUser->GetLevel() >= 70 
				&& pUser->GetLevel() <= 83 
				&& pUser->GetNation() == Nation::KARUS)
				nZoneID = ZONE_KARUS;
			else if(pUser->GetLevel() >= 70 
				&& pUser->GetLevel() <= 83 
				&& pUser->GetNation() == Nation::ELMORAD)
				nZoneID = ZONE_ELMORAD;

			if (ZoneID == 0)
				continue;

			pUser->ZoneChange(nZoneID, 0.0f,0.0f);
		}
	}
}

void CGameServerDlg::Announcement(uint16 type, int nation, int chat_type, CUser* pExceptUser, CNpc *pExpectNpc)
{
	string chatstr; 
	uint8 ZoneID = 0;
	std::string sEventName;

	switch (type)
	{
	case BATTLEZONE_OPEN:
		if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE3)
			GetServerResource(IDS_MID_LEVEL_BATTLEZONE_OPEN, &chatstr);
		else
			GetServerResource(IDP_BATTLEZONE_OPEN, &chatstr);
		break;
	case DECLARE_WINNER:
		if (m_bVictory == KARUS)
			GetServerResource(IDP_KARUS_VICTORY, &chatstr, m_sElmoradDead, m_sKarusDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDP_ELMORAD_VICTORY, &chatstr, m_sKarusDead, m_sElmoradDead);
		else 
			return;
		break;
	case DECLARE_LOSER:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_ELMORAD_LOSER, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_KARUS_LOSER, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case DECLARE_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_BANISH_USER, &chatstr);
		break;
	case SNOW_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_SNOWBATTLE_BANISH_USER, &chatstr);
		break;
	case DECLARE_BATTLE_ZONE_STATUS:
		if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_STATUS, &chatstr,  m_sKarusMonumentPoint, m_sElmoMonumentPoint, m_sKarusDead, m_sElmoradDead);
			SendNotice<PUBLIC_CHAT>(chatstr.c_str(), ZONE_BATTLE4, Nation::ALL, true);
		}
		else if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE5)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_STATUS, &chatstr,  m_sKarusMonumentPoint, m_sElmoMonumentPoint, m_sKarusDead, m_sElmoradDead);
			SendNotice<PUBLIC_CHAT>(chatstr.c_str(), ZONE_BATTLE5, Nation::ALL, true);
		}
		else
			GetServerResource(IDS_BATTLEZONE_STATUS, &chatstr,  m_sKarusDead, m_sElmoradDead);
		break;
	case DECLARE_BATTLE_MONUMENT_STATUS:
		if (pExceptUser)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_WON_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID());
			GetServerResource(IDS_BATTLE_MONUMENT_LOST_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID());
			return;
		}
		break;
	case DECLARE_NATION_MONUMENT_STATUS:
		if (pExceptUser)
		{
			uint16 nTrapNumber = pExceptUser->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			if ((pExceptUser->GetNation() == KARUS && pExceptUser->GetZoneID() == ELMORAD) || (pExceptUser->GetNation() == ELMORAD && pExceptUser->GetZoneID() == KARUS))
			{
				GetServerResource(IDS_INFILTRATION_CONQUER, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
				g_pMain->SendAnnouncement(chatstr.c_str());
				return;
			}
			else if ((pExceptUser->GetNation() == KARUS && pExceptUser->GetZoneID() == KARUS) || (pExceptUser->GetNation() == ELMORAD && pExceptUser->GetZoneID() == ELMORAD))
			{
				GetServerResource(IDS_INFILTRATION_RECAPTURE, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
				g_pMain->SendAnnouncement(chatstr.c_str());
				return;
			}
			return;
		}
		break;
	case DECLARE_NATION_REWARD_STATUS:
		if (pExpectNpc)
		{
			uint16 nTrapNumber = pExpectNpc->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(pExpectNpc->GetNation() == KARUS ? IDS_INFILTRATION_REWARD_KARUS : IDS_INFILTRATION_REWARD_ELMORAD, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExpectNpc->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), Nation::ALL);
			return;
		}
		break;
	case SNOW_BATTLEZONE_OPEN:
		GetServerResource(IDS_SNOWBATTLE_OPEN, &chatstr);
		break;
	case UNDER_ATTACK_NOTIFY:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_UNDER_ATTACK_ELMORAD, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_UNDER_ATTACK_KARUS, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case BATTLEZONE_CLOSE:
		GetServerResource(IDS_BATTLE_CLOSE, &chatstr);
		break;
	case SNOW_BATTLEZONE_CLOSE:
		GetServerResource(IDS_SNOWBATTLE_CLOSE, &chatstr);
		break;
	case KARUS_CAPTAIN_NOTIFY:
		GetServerResource(IDS_KARUS_CAPTAIN, &chatstr, m_strKarusCaptain.c_str());
		break;
	case ELMORAD_CAPTAIN_NOTIFY:
		GetServerResource(IDS_ELMO_CAPTAIN, &chatstr, m_strElmoradCaptain.c_str());
		break;
	case KARUS_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			if (!pExceptUser->isInClan())
				return;

			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_KARUS_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pExceptUser->GetName().c_str());
		}
		break;
	case ELMORAD_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			if (!pExceptUser->isInClan())
				return;

			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_ELMO_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(),  pExceptUser->GetName().c_str());
		}
		break;
	case IDS_MONSTER_CHALLENGE_ANNOUNCEMENT:
		GetServerResource(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT, &chatstr, m_nForgettenTempleStartHour,m_nForgettenTempleLevelMin, m_nForgettenTempleLevelMax);
		break;
	case IDS_MONSTER_CHALLENGE_OPEN:
		GetServerResource(IDS_MONSTER_CHALLENGE_OPEN, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_START:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_START, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_VICTORY:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_VICTORY, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_CLOSE:
		GetServerResource(IDS_MONSTER_CHALLENGE_CLOSE, &chatstr);
		break;
	/*case IDS_UNDER_THE_CASTLE_ANNOUNCEMENT:
		GetServerResource(IDS_UNDER_THE_CASTLE_ANNOUNCEMENT, &chatstr, m_nUnderTheCastleLevelMin, m_nUnderTheCastleLevelMax, m_nUnderTheCastleStartHour);
		break;*/
	case IDS_UNDER_THE_CASTLE_OPEN:
		GetServerResource(IDS_UNDER_THE_CASTLE_OPEN, &chatstr);
		break;
	/*case IDS_UNDER_THE_CASTLE_START:
		ZoneID = ZONE_UNDER_CASTLE;
		GetServerResource(IDS_UNDER_THE_CASTLE_START, &chatstr);
		break;
	case IDS_UNDER_THE_CASTLE_VICTORY:
		ZoneID = ZONE_UNDER_CASTLE;
		GetServerResource(IDS_UNDER_THE_CASTLE_VICTORY, &chatstr);*/
		break;
	case IDS_UNDER_THE_CASTLE_CLOSE:
		GetServerResource(IDS_UNDER_THE_CASTLE_CLOSE, &chatstr);
		break;
	/*case IDS_SIEGE_WAR_TIME_NOTICE:
		GetServerResource(IDS_SIEGE_WAR_TIME_NOTICE, &chatstr, pCswEvent.m_SiegeWarNoticeTimev1);
		break;	*/
	case IDS_SIEGE_WAR_START:
		GetServerResource(IDS_SIEGE_WAR_START, &chatstr);
		break;		
	case IDS_SIEGE_WAR_END:
		GetServerResource(IDS_SIEGE_WAR_END, &chatstr);
		break;		
	case IDS_NPC_GUIDON_DESTORY:
		{
			if (m_KnightsSiegeWarfareArray.GetSize() < 1)
				return;

			_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
			ZoneID = ZONE_DELOS;
			GetServerResource(IDS_NPC_GUIDON_DESTORY, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str());
		}
		break;		
	case IDS_SIEGE_WAR_VICTORY:
		{
			if (m_KnightsSiegeWarfareArray.GetSize() < 1)
				return;

			_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
			ZoneID = ZONE_DELOS;
			GetServerResource(IDS_SIEGE_WAR_VICTORY, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str());
		}
		break;	
	case IDS_PRE_BATTLE_ANNOUNCEMENT:
		GetServerResource(IDS_PRE_BATTLE_ANNOUNCEMENT, &chatstr, m_byBattleNoticeTime);
		break;
	case IDS_REMNANT_SUMMON_INFO:
		GetServerResource(IDS_REMNANT_SUMMON_INFO, &chatstr);
		break;
	}

	Packet result;
	string finalstr;
	GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
	ChatPacket::Construct(&result, (uint8) chat_type, &finalstr);
	Send_All(&result, nullptr, nation, ZoneID);
}

/**
* @brief	Loads the specified user's NP ranks
* 			from the rankings tables.
*
* @param	pUser	The user.
*/
void CGameServerDlg::GetUserRank(CUser *pUser)
{
	// Acquire the lock for thread safety
	Guard lock(m_userRankingsLock);
	// Get character's name & convert it to upper case for case insensitivity
	string strUserID = pUser->GetName();
	STRTOUPPER(strUserID);

	// Grab the personal rank from the map, if applicable.
	if (pUser->GetNation() == KARUS)
	{
		UserNameRankMap::iterator itr = m_UserKarusPersonalRankMap.find(strUserID);
		pUser->m_bPersonalRank = itr != m_UserKarusPersonalRankMap.end() ? uint8(itr->second->nRank) : -1;

		// Grab the knights rank from the map, if applicable.
		itr = m_UserKarusKnightsRankMap.find(strUserID);
		pUser->m_bKnightsRank = itr != m_UserKarusKnightsRankMap.end() ? uint8(itr->second->nRank) : -1;
	}
	else if (pUser->GetNation() == ELMORAD)
	{
		UserNameRankMap::iterator itr = m_UserElmoPersonalRankMap.find(strUserID);
		pUser->m_bPersonalRank = itr != m_UserElmoPersonalRankMap.end() ? uint8(itr->second->nRank) : -1;

		// Grab the knights rank from the map, if applicable.
		itr = m_UserElmoKnightsRankMap.find(strUserID);
		pUser->m_bKnightsRank = itr != m_UserElmoKnightsRankMap.end() ? uint8(itr->second->nRank) : -1;
	}
}

/**
* @brief	Calculates the clan grade from the specified
* 			loyalty points (NP).
*
* @param	nPoints	Loyalty points (NP). 
* 					The points will be converted to clan points 
* 					by this method.
*
* @return	The clan grade.
*/
int CGameServerDlg::GetKnightsGrade(uint32 nPoints)
{
	uint32 nClanPoints = nPoints;

	if (nClanPoints >= m_Grade1)
		return 1;
	else if (nClanPoints >= m_Grade2)
		return 2;
	else if (nClanPoints >= m_Grade3)
		return 3;
	else if (nClanPoints >= m_Grade4)
		return 4;

	return 5;
}

/**
* @brief	Disconnects all players in the server.
*
* @return	The number of users who were in-game.
*/
int CGameServerDlg::KickOutAllUsers()
{
	int count = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (pUser->isInGame())
			count++;

		pUser->UserDataSaveToAgent();
		pUser->Disconnect();
	}
	return count;
}

/**
* @brief	Kick out all users from the specified zone
* 			to their home zone.
*
* @param	zone	The zone to kick users out from.
*/
void CGameServerDlg::KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID, uint8 bNation)
{
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	C3DMap	*pKarusMap		= GetZoneByID(KARUS),
		*pElMoradMap	= GetZoneByID(ELMORAD);	

	ASSERT (pKarusMap != nullptr && pElMoradMap != nullptr);

	foreach (itr, sessMap)
	{
		// Only kick users from requested zone.
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		if (TargetZoneID > 0 && bNation == pUser->GetNation())
		{
			pUser->ZoneChange(TargetZoneID, 0.0f,0.0f);
			continue;
		}
		else if (TargetZoneID > 0 && bNation == ALL)
		{
			pUser->ZoneChange(TargetZoneID, 0.0f,0.0f);
			continue;
		}

		C3DMap * pMap = (pUser->GetNation() == KARUS ? pKarusMap : pElMoradMap);
		if(TargetZoneID == 0)
			pUser->ZoneChange(pMap->m_nZoneNumber, pMap->m_fInitX, pMap->m_fInitZ);
	}
}

void CGameServerDlg::SendItemUnderTheCastleRoomUsers(uint8 ZoneID, uint16 Room, float GetX, float GetZ)
{
	if (Room != 1 
		&& Room != 2
		&& Room != 3 
		&& Room != 4 
		&& Room != 5)
		return;

	if (Room == 1 || Room == 2)
	{
		if (Room == 1)
		{
			SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
			foreach(itr, sessMap)
			{
				CUser * pUtcPlayer = TO_USER(itr->second);
				if (!pUtcPlayer->isInGame()
					|| pUtcPlayer->GetZoneID() != ZoneID
					|| (!pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
						&& !pUtcPlayer->isInRangeSlow(GetX, GetZ, 15)))
					continue;

				if (pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
					&& pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
					pUtcPlayer->GiveItem(TROPHY_OF_FLAME, 2);
				else if (pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
					|| pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
					pUtcPlayer->GiveItem(TROPHY_OF_FLAME);

				pUtcPlayer->GiveItem(LUNAR_ORDER_TOKEN);
			}
		}
		else if (Room == 2)
		{
			SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
			foreach(itr, sessMap)
			{
				CUser * pUtcPlayer = TO_USER(itr->second);
				if (!pUtcPlayer->isInGame()
					|| pUtcPlayer->GetZoneID() != ZoneID
					|| (!pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
						&& !pUtcPlayer->isInRangeSlow(GetX, GetZ, 15)))
					continue;

				if (pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
					&& pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
					pUtcPlayer->GiveItem(TROPHY_OF_FLAME, 2);
				else if (pUtcPlayer->isInRangeSlow(121.0f, 297.0f, 80) 
					|| pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
					pUtcPlayer->GiveItem(TROPHY_OF_FLAME);

				pUtcPlayer->GiveItem(DENTED_IRONMASS);
				pUtcPlayer->GiveItem(LUNAR_ORDER_TOKEN);
			}
		}
	}
	else if (Room == 3)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser * pUtcPlayer = TO_USER(itr->second);
			if (!pUtcPlayer->isInGame()
				|| pUtcPlayer->GetZoneID() != ZoneID
				|| (!pUtcPlayer->isInRangeSlow(520.0f, 494.0f, 115) 
					&& !pUtcPlayer->isInRangeSlow(GetX, GetZ, 15)))
				continue;

			if (pUtcPlayer->isInRangeSlow(520.0f, 494.0f, 115) 
				&& pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME, 2);
			else if (pUtcPlayer->isInRangeSlow(520.0f, 494.0f, 115) 
				|| pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME);

			pUtcPlayer->GiveItem(PETRIFIED_WEAPON_SHRAPNEL);
			pUtcPlayer->GiveItem(LUNAR_ORDER_TOKEN);
		}
	}
	else if (Room == 4)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser * pUtcPlayer = TO_USER(itr->second);
			if (!pUtcPlayer->isInGame()
				|| pUtcPlayer->GetZoneID() != ZoneID
				|| (!pUtcPlayer->isInRangeSlow(642.0f, 351.0f, 100) 
					&& !pUtcPlayer->isInRangeSlow(GetX, GetZ, 15)))
				continue;

			if (pUtcPlayer->isInRangeSlow(642.0f, 351.0f, 100) 
				&& pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME, 2);
			else if (pUtcPlayer->isInRangeSlow(642.0f, 351.0f, 100) 
				|| pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME);

			pUtcPlayer->GiveItem(IRON_POWDER_OF_CHAIN);
			pUtcPlayer->GiveItem(LUNAR_ORDER_TOKEN);
		}
	}
	else if (Room == 5)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser * pUtcPlayer = TO_USER(itr->second);
			if (!pUtcPlayer->isInGame()
				|| pUtcPlayer->GetZoneID() != ZoneID
				|| (!pUtcPlayer->isInRangeSlow(803.0f, 839.0f, 110) 
					&& !pUtcPlayer->isInRangeSlow(GetX, GetZ, 15)))
				continue;

			if (pUtcPlayer->isInRangeSlow(803.0f, 839.0f, 110) 
				&& pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME, 2);
			else if (pUtcPlayer->isInRangeSlow(803.0f, 839.0f, 110) 
				|| pUtcPlayer->isInRangeSlow(GetX, GetZ, 15))
				pUtcPlayer->GiveItem(TROPHY_OF_FLAME);

			pUtcPlayer->GiveItem(PLWITOONS_TEAR);
			pUtcPlayer->GiveItem(HORN_OF_PLUWITOON);
			pUtcPlayer->GiveItem(LUNAR_ORDER_TOKEN);
		}
	}
}

void CGameServerDlg::SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount /*= 1*/, uint32 Time/*= 0*/)
{
	// TO-DO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		pUser->GiveItem(nItemID, sCount, true, Time);
	}
}

void CGameServerDlg::SendItemEventRoom(uint16 nEventRoom,uint32 nItemID, uint16 sCount /*= 1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetEventRoom() != nEventRoom) 
			continue;
		
		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::GetCaptainUserPtr()
{
	foreach_stlmap (itr, m_KnightsArray)
	{
		CKnights *pKnights = itr->second;
		if (pKnights->m_byRanking != 1)
			continue;
	}
}

/**
* @brief	Sends the flying santa/angel packet to all users in the server.
*/
void CGameServerDlg::SendFlyingSantaOrAngel()
{
	Packet result(WIZ_SANTA, uint8(m_bSantaOrAngel));
	Send_All(&result);
}

CGameServerDlg::~CGameServerDlg() 
{
	g_bNpcExit = true;

	printf("Online User Disconnect...");
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pUser->OnDisconnect();
	}
	printf(" done.\n");
	Sleep(1 * SECOND);

	printf("Waiting for timer threads to exit...");
	foreach(itr, g_timerThreads)
	{
		if ((*itr) == nullptr)
			continue;

		if ((*itr)->isStarted())
			(*itr)->waitForExit();

		delete (*itr);
	}
	printf(" done.\n");
	Sleep(1 * SECOND);

	printf("Waiting for NPC threads to exit...");
	foreach_stlmap(itr, m_arNpcThread)
	{
		CNpcThread * pThread = itr->second;
		if (pThread == nullptr)
			continue;

		pThread->Shutdown();
	}

	printf(" done.\n");
	Sleep(1 * SECOND);

	// Cleanup our script pool & consequently ensure all scripts 
	// finish execution before proceeding.
	// This prevents us from freeing data that's in use.
	printf("Shutting down Lua engine...");
	m_luaEngine.Shutdown();
	printf(" done.\n");
	Sleep(1 * SECOND);

	printf("Shutting down game database system...");
	DatabaseThread::Shutdown();
	printf(" done.\n");
	printf("Shutting down account database system...");
	printf(" done.\n");
	Sleep(1 * SECOND);

	printf("Shutting down logger system...");
	KnightLogger::Shutdown();
	printf(" done.\n");
	Sleep(1 * SECOND);

	printf("Shutting down socket system...");
	m_socketMgr.Shutdown();
	printf(" done.\n");
	Sleep(1 * SECOND);

	CUser::CleanupChatCommands();
	CGameServerDlg::CleanupServerCommands();

	CleanupUserRankings();
	m_LevelUpArray.clear();
}

void CGameServerDlg::ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID, uint16 nEventRoom /* = 0 */)
{
	Packet result(WIZ_OBJECT_EVENT, uint8(OBJECT_NPC));
	result << uint8(3) << sNpcID << nEffectID;
	g_pMain->Send_Zone(&result, ZoneID, nullptr, 0, nEventRoom, 0.0f);
}

#pragma region Logger Methods

void CGameServerDlg::WriteDeathUserLogFile(string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::DEADLOGUSER);
}

void CGameServerDlg::WriteDeathNpcLogFile(string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::DEADLOGNPC);
}

void CGameServerDlg::WriteChatLogFile(string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::CHATLOG);
}

void CGameServerDlg::WriteCheatLogFile(string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::CHEATLOG);
}

void CGameServerDlg::WriteTradeLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::TRADELOG);
}

void CGameServerDlg::WriteMerchantLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::MERTCHANTLOG);
}

void CGameServerDlg::WriteLetterLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::LETTERLOG);
}

void CGameServerDlg::WriteBanListLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::BANLOG);
}

void CGameServerDlg::WriteItemTransactionLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::ITEMLOG);
}

void CGameServerDlg::WriteOfflineSystemLog(std::string& logMessage)
{
	AddLogRequest(logMessage, LogTypes::OFFLINE_MERCHANT);
}

void CGameServerDlg::WriteUpgradeLogFile(std::string & logMessage)
{
	AddLogRequest(logMessage,LogTypes::ITEMUPGRADELOG);
}

void CGameServerDlg::WriteHookPusSystemLog(std::string& logMessage)
{
	AddLogRequest(logMessage, LogTypes::HOOK_PUS);
}

void CGameServerDlg::WriteCharacterIpLogs(std::string & logMessage)
{
	AddLogRequest(logMessage, LogTypes::CHARACTERIPLOG);
}

void CGameServerDlg::WriteTournamentLogs(std::string & logMessage)
{
	AddLogRequest(logMessage, LogTypes::TOURNAMENTLOG);
}
#pragma endregion

#pragma region Duper Scanner Methods
bool CGameServerDlg::IsDuplicateItem(uint32 nItemID, uint64 nItemSerial)
{
	if (nItemID <= 0 
		|| nItemSerial <= 0)
		return false;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
		return false;

	uint32 nDuplicateCount = 0;

	foreach(itr, pUserItem->nItemSerial)
	{
		if (*itr == nItemSerial)
			nDuplicateCount++;

		if (nDuplicateCount > 1)
			return true;
	}

	return false;
}

void CGameServerDlg::AddUserItem(uint32 nItemID, uint64 nItemSerial)
{
	if (nItemID <= 0 || nItemSerial <= 0)
		return;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
	{
		pUserItem = new _USER_ITEM;
		pUserItem->nItemID = nItemID;
		pUserItem->nItemSerial.push_back(nItemSerial);

		if (!g_pMain->m_UserItemArray.PutData(pUserItem->nItemID, pUserItem))
			delete pUserItem;
	}
	else
		pUserItem->nItemSerial.push_back(nItemSerial);
}

void CGameServerDlg::DeleteUserItem(uint32 nItemID, uint64 nItemSerial)
{
	if (nItemID <= 0 
		|| nItemSerial <= 0)
		return;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
		return;

	pUserItem->nItemSerial.erase(std::remove(pUserItem->nItemSerial.begin(), pUserItem->nItemSerial.end(), nItemSerial), pUserItem->nItemSerial.end());
}
#pragma endregion

void CGameServerDlg::ReloadKnightAndUserRanks()
{
	Packet result(WIZ_DB_UPDATE_RANKING);
	AddDatabaseRequest(result);
}

void CGameServerDlg::SetPlayerRankingRewards(uint16 ZoneID)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| pUser->isGM()
			|| pUser->GetZoneID() != ZoneID)
			continue;
		
		if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 0
			&& pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 10)
		{
			if (m_nPlayerRankingLoyaltyReward > 0)
				pUser->SendLoyaltyChange(m_nPlayerRankingLoyaltyReward, false, true, false);
			if (m_nPlayerRankingKnightCashReward > 0)
				g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), m_nPlayerRankingKnightCashReward);
		}
	}
}

void CGameServerDlg::HandleSiegeDatabaseRequest(CUser * pUser, Packet & pkt)
{
	uint8 bOpcode;
	pkt >> bOpcode;

	switch(bOpcode)
	{
	case 0:
		{
			int16 m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute;
			pkt >> m_sCastleIndex >> m_sMasterKnights >> m_bySiegeType >> m_byWarDay >> m_byWarTime >> m_byWarMinute;
			g_DBAgent.UpdateSiege(m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute);
		}
		break;
	case 1:
		{
			uint8 Zone;
			int16 ZoneTarrif;
			pkt >> Zone >> ZoneTarrif;
			g_DBAgent.UpdateSiegeTax(Zone, ZoneTarrif);
		}
		break;
	}
}

void CGameServerDlg::UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute)
{
	Packet result(WIZ_SIEGE, uint8(0));
	result << m_sCastleIndex << m_sMasterKnights << m_bySiegeType << m_byWarDay << m_byWarTime << m_byWarMinute;
	g_pMain->AddDatabaseRequest(result);
}

void CGameServerDlg::UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif)
{
	Packet result(WIZ_SIEGE, uint8(1));
	result << Zone << ZoneTarrif;
	g_pMain->AddDatabaseRequest(result);
}


void CGameServerDlg::GameServerShutDownTimer()
{
	if (m_GameServerShutDownOK == true)
	{
		if (m_GameServerShutDownSeconds > 0)
			m_GameServerShutDownSeconds--;

		Packet x;
		x.Initialize(WIZ_LOGOSSHOUT);

		if (m_GameServerShutDownSeconds == 60)
		{
			x << uint8(2) << uint8(6) << uint8(1);
			Send_All(&x);
			printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);
		}
		else if (m_GameServerShutDownSeconds == 120)
		{
			x << uint8(2) << uint8(6) << uint8(2);
			Send_All(&x);
			printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);
		}
		else if (m_GameServerShutDownSeconds == 180)
		{
			x << uint8(2) << uint8(6) << uint8(3);
			Send_All(&x);
			printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);
		}
		else if (m_GameServerShutDownSeconds == 240)
		{
			x << uint8(2) << uint8(6) << uint8(4);
			Send_All(&x);
			printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);
		}
		else if (m_GameServerShutDownSeconds == 300)
		{
			x << uint8(2) << uint8(6) << uint8(5);
			Send_All(&x);
			printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);
		}
		else if (m_GameServerShutDownSeconds == 0)
		{
			m_GameServerShutDownOK = false;
			GameServerShutDown();
		}
	}
}

void CGameServerDlg::GameServerShutDown()
{
	printf("Server shutdown, %d users kicked out.\n", KickOutAllUsers());
	printf("Waiting Please \n");
	m_socketMgr.Shutdown();
	Sleep(4 * SECOND);
	g_DBAgent.UpdateRanks();
	Sleep(4 * SECOND);
	g_DBAgent.CheckBannedAccounts();
	Sleep(4 * SECOND);
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_SAVE));
	g_pMain->AddDatabaseRequest(result);
	Sleep(8 * SECOND);
	g_pMain->s_hEvent->Signal();
	Sleep(2 * SECOND);
	printf("\nAll online users data were saved successfully .... OK!\n");
	Sleep(10 * SECOND);
	ExitProcess(1);
}

void CGameServerDlg::SendYesilNotice(CUser* pUser, std::string sHelpMessage)
{
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result;
	std::string buffer = string_format("%s", sHelpMessage.c_str());
	ChatPacket::Construct(&result, KNIGHTS_CHAT, &buffer);
	pUser->Send(&result);

}

#pragma region CGameServerDlg::SetMiningDrops()

void CGameServerDlg::SetMiningDrops()
{
	memset(m_ItemMiningDropListArrayNormal, 0, sizeof(m_ItemMiningDropListArrayNormal));
	memset(m_ItemFishingDropListArrayNormal, 0, sizeof(m_ItemFishingDropListArrayNormal));
	uint32 offset = 0;
	g_pMain->m_MiningFishingItemArray.m_lock.lock();
	foreach_stlmap_nolock(itr, g_pMain->m_MiningFishingItemArray)
	{
		if (itr->second->nTableType != 0)
			continue;

		if (itr->second->UseItemType != 1)
			continue;

		if (itr->second->nWarStatus != MiningResultError)
			continue;

		if (offset > 10000)
			break;

		for (int i = 0; i < int(itr->second->SuccessRate / 5); i++)
		{
			if (i + offset > 10000)
				break;

			m_ItemMiningDropListArrayNormal[offset + i] = itr->second->nGiveItemID;
		}
		offset += uint32(itr->second->SuccessRate / 5);
	}
	g_pMain->m_MiningFishingItemArray.m_lock.unlock();

	offset = 0;

	g_pMain->m_MiningFishingItemArray.m_lock.lock();
	foreach_stlmap_nolock(itr, g_pMain->m_MiningFishingItemArray)
	{
		if (itr->second->nTableType != 1)
			continue;

		if (itr->second->UseItemType != 1)
			continue;

		if (itr->second->nWarStatus != MiningResultError)
			continue;

		if (offset > 10000)
			break;

		for (int i = 0; i < int(itr->second->SuccessRate / 5); i++)
		{
			if (i + offset > 10000)
				break;

			m_ItemFishingDropListArrayNormal[offset + i] = itr->second->nGiveItemID;
		}
		offset += uint32(itr->second->SuccessRate / 5);
	}
	g_pMain->m_MiningFishingItemArray.m_lock.unlock();
}

#pragma endregion

#pragma region CGameServerDlg::SendSpecialNotice(std::string Sender, std::string Content, uint8 NoticeType, CUser* pSending /*= nullptr*/) 

void CGameServerDlg::SendSpecialNotice(std::string Sender, std::string Content, uint8 NoticeType, CUser* pSending /*= nullptr*/)
{
	if (pSending)
	{
		Packet DuyuruNotice(WIZ_CHAT, uint8(NoticeType));
		DuyuruNotice << pSending->GetNation() << int16(3);
		DuyuruNotice.SByte();
		DuyuruNotice << Sender;
		DuyuruNotice.DByte();
		DuyuruNotice << Content << uint8(1);
		pSending->Send(&DuyuruNotice);
		return;
	}

	Guard lock(m_characterNameLock);
	NameMap sessMap = m_characterNameMap;
	foreach(itr, sessMap)
	{
		CUser* pUser1 = TO_USER(itr->second);
		if (pUser1 == nullptr
			|| !pUser1->isInGame())
			continue;

		Packet DuyuruNotice(WIZ_CHAT, uint8(NoticeType));
		DuyuruNotice << pUser1->GetNation() << int16(3);
		DuyuruNotice.SByte();
		DuyuruNotice << Sender;
		DuyuruNotice.DByte();
		DuyuruNotice << Content << uint8(1);
		pUser1->Send(&DuyuruNotice);
	}
}

#pragma endregion

void CGameServerDlg::LogosYolla(std::string LogosName, std::string LogosMessage, uint8 R, uint8 G, uint8 B)
{

	Packet Logos(WIZ_LOGOSSHOUT);
	string Title = LogosName;
	string Message = LogosMessage;
	string Birlestir = Title + ": " + Message;
	Logos.SByte();
	Logos << uint8(2) << uint8(1) << R << G << B << uint8(0) << Birlestir;
	g_pMain->Send_All(&Logos);

}
void CGameServerDlg::LogosYolla_Zone(std::string LogosName, std::string LogosMessage, uint8 R, uint8 G, uint8 B, uint8 ZoneNumber)
{
	Packet Logos(WIZ_LOGOSSHOUT);

	std::string Birlestir = LogosName + " : " + LogosMessage;
	Logos.SByte();
	Logos << uint8(2) << uint8(1) << R << G << B << uint8(0) << Birlestir;
	g_pMain->Send_Zone(&Logos, ZoneNumber);

}