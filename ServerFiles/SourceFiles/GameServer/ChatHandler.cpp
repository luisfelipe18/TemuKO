#include "stdafx.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

using std::string;

ServerCommandTable CGameServerDlg::s_commandTable;
ChatCommandTable CUser::s_commandTable;

void CGameServerDlg::InitServerCommands()
{
	static Command<CGameServerDlg> commandTable[] =
	{
		{ "help",				&CGameServerDlg::HandleHelpCommand,					"Show all commands." },
		{ "notice",				&CGameServerDlg::HandleNoticeCommand,				"Sends a server-wide chat notice." },
		{ "noticeall",			&CGameServerDlg::HandleNoticeallCommand,			"Sends a server-wide chat notice." },
		{ "kill",				&CGameServerDlg::HandleKillUserCommand,				"Disconnects the specified player" },
		{ "open1",				&CGameServerDlg::HandleWar1OpenCommand,				"Opens war zone 1" },
		{ "open2",				&CGameServerDlg::HandleWar2OpenCommand,				"Opens war zone 2" },
		{ "open3",				&CGameServerDlg::HandleWar3OpenCommand,				"Opens war zone 3" },
		{ "open4",				&CGameServerDlg::HandleWar4OpenCommand,				"Opens war zone 4" },
		{ "open5",				&CGameServerDlg::HandleWar5OpenCommand,				"Opens war zone 5" },
		{ "open6",				&CGameServerDlg::HandleWar6OpenCommand,				"Opens war zone 6" },
		{ "snowopen",			&CGameServerDlg::HandleSnowWarOpenCommand,			"Opens the snow war zone" },
		{ "siegewarfare",		&CGameServerDlg::HandleSiegeWarOpenCommand,			"Opens the Castle Siege War zone" },
		{ "close",				&CGameServerDlg::HandleWarCloseCommand,				"Closes the active war zone" },
		{ "siegewarfareclose",	&CGameServerDlg::HandleCastleSiegeWarClose,			"Closes the active csw zone" },
		{ "down",				&CGameServerDlg::HandleShutdownCommand,				"Shuts down the server" },
		{ "discount",			&CGameServerDlg::HandleDiscountCommand,				"Enables server discounts for the winning nation of the last war" },
		{ "alldiscount",		&CGameServerDlg::HandleGlobalDiscountCommand,		"Enables server discounts for everyone" },
		{ "offdiscount",		&CGameServerDlg::HandleDiscountOffCommand,			"Disables server discounts" },
		{ "captain",			&CGameServerDlg::HandleCaptainCommand,				"Sets the captains/commanders for the war" },
		{ "santa",				&CGameServerDlg::HandleSantaCommand,				"Enables a flying Santa Claus." },
		{ "offsanta",			&CGameServerDlg::HandleSantaOffCommand,				"Disables a flying Santa Claus/angel." },
		{ "angel",				&CGameServerDlg::HandleAngelCommand,				"Enables a flying angel." },
		{ "offangel",			&CGameServerDlg::HandleSantaOffCommand,				"Disables a flying Santa Claus/angel." },
		{ "permanent",			&CGameServerDlg::HandlePermanentChatCommand,		"Sets the permanent chat bar to the specified text." },
		{ "offpermanent",		&CGameServerDlg::HandlePermanentChatOffCommand,		"Resets the permanent chat bar text." },
		{ "reload_notice",		&CGameServerDlg::HandleReloadNoticeCommand,			"Reloads the in-game notice list." },
		{ "reload_tables",		&CGameServerDlg::HandleReloadTablesCommand,			"Reloads the in-game tables." },
		{ "reload_tables2",		&CGameServerDlg::HandleReloadTables2Command,		"Reloads the in-game tables." },
		{ "reload_tables3",		&CGameServerDlg::HandleReloadTables3Command,		"Reloads the in-game tables." },
		{ "reload_magics",		&CGameServerDlg::HandleReloadMagicsCommand,			"Reloads the in-game magic tables." },
		{ "reload_quests",		&CGameServerDlg::HandleReloadQuestCommand,			"Reloads the in-game quest tables." },
		{ "reload_ranks",		&CGameServerDlg::HandleReloadRanksCommand,			"Reloads the in-game rank tables." },
		{ "reload_drops",		&CGameServerDlg::HandleReloadDropsCommand,			"Reloads the in-game drops tables." },
		{ "reload_kings",		&CGameServerDlg::HandleReloadKingsCommand,			"Reloads the in-game King tables." },
		{ "reload_items",		&CGameServerDlg::HandleReloadItemsCommand,			"Reloads the in-game Item tables." },
		{ "reload_dungeon",		&CGameServerDlg::HandleReloadDungeonDefenceTables,	"Reloads the in-game Dungeon Defence tables" },
		{ "reload_draki",		&CGameServerDlg::HandleReloadDrakiTowerTables,		"Reloads the in-game Draki Tower tables" },
		{ "reload_event",		&CGameServerDlg::HandleEventScheduleResetTable,		"Reloads the in-game Event Schedule Timer tables" },
		{ "reload_auto_notice",	&CGameServerDlg::HandleReloadAutoNoticeTables,		"Reloads the in-game SERVER_RESOURCE tables" },
		{ "reload",				&CGameServerDlg::HandleReloadTableCommand,			"Reload the specific table." },
		{ "count",				&CGameServerDlg::HandleCountCommand,				"Get online user count." },
		{ "tp_all",				&CGameServerDlg::HandleTeleportAllCommand,			"Players send to home zone." },
		{ "warresult",			&CGameServerDlg::HandleWarResultCommand,			"Set result for War" },
		{ "underthecastle",		&CGameServerDlg::HandleEventUnderTheCastleCommand,	"Open & close event Under the Castle zone" },
		{ "tournamentstart",	&CGameServerDlg::HandleTournamentStart,				"Start is Clan Tournament" },
		{ "tournamentclose",	&CGameServerDlg::HandleTournamentClose,				"Close is Clan Tournament" },
		{ "chaosopen",			&CGameServerDlg::HandleChaosExpansionOpen,			"Open is Chaos Expansion" },
		{ "borderopen",			&CGameServerDlg::HandleBorderDefenceWar,			"Open is Border Defence War" },
		{ "juraidopen",			&CGameServerDlg::HandleJuraidMountain,				"Open is Juraid Mountain" },
		{ "royalopen",			&CGameServerDlg::HandleKnightRoyale,				"Open is Knight Royale" },
		{ "block",				&CGameServerDlg::HandlePermanentBannedCommand,      "Player ban" },
		{ "unblock",			&CGameServerDlg::HandlePermitConnectCommand,        "Player unban" },
		{ "testing",			&CGameServerDlg::HandleServerGameTestCommand,		"Server Code Test Command" },
	};

	init_command_table(CGameServerDlg, commandTable, s_commandTable);
}

void CGameServerDlg::CleanupServerCommands() { free_command_table(s_commandTable); }

void CUser::InitChatCommands()
{
	static Command<CUser> commandTable[] =
	{
		// Command				Handler											Help message
		{ "help",				&CUser::HandleHelpCommand,						"Show all commands." },
		{ "give_item",			&CUser::HandleGiveItemCommand,					"Gives a player an item. Arguments: character name | item ID | [optional stack size]" },
		{ "zonechange",			&CUser::HandleZoneChangeCommand,				"Teleports you to the specified zone. Arguments: zone ID" },
		{ "monsummon",			&CUser::HandleMonsterSummonCommand,				"Spawns the specified monster (does not respawn). Arguments: monster's database ID" },
		{ "npcsummon",			&CUser::HandleNPCSummonCommand,					"Spawns the specified NPC (does not respawn). Arguments: NPC's database ID" },
		{ "monkill",			&CUser::HandleMonKillCommand,					"Kill a NPC or Monster, Arguments: select an Npc and monster than use this command" },
		{ "open1",				&CUser::HandleWar1OpenCommand,					"Opens war zone 1" },
		{ "open2",				&CUser::HandleWar2OpenCommand,					"Opens war zone 2" },
		{ "open3",				&CUser::HandleWar3OpenCommand,					"Opens war zone 3" },
		{ "open4",				&CUser::HandleWar4OpenCommand,					"Opens war zone 4" },
		{ "open5",				&CUser::HandleWar5OpenCommand,					"Opens war zone 5" },
		{ "open6",				&CUser::HandleWar6OpenCommand,					"Opens war zone 6" },
		{ "captain",			&CUser::HandleCaptainCommand,					"Sets the captains/commanders for the war" },
		{ "snowopen",			&CUser::HandleSnowWarOpenCommand,				"Opens the snow war zone" },
		{ "siegewarfare",		&CUser::HandleSiegeWarOpenCommand,				"Opens the Castle Siege War zone" },
		{ "close",				&CUser::HandleWarCloseCommand,					"Closes the active war zone" },
		{ "siegewarfareclose",	&CUser::HandleCastleSiegeWarClose,				"Closes the active CSW Zone" },
		{ "np_change",			&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ "exp_change",			&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ "gold_change",		&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
		{ "exp_add",			&CUser::HandleExpAddCommand,					"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "np_add",				&CUser::HandleNPAddCommand,						"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CUser::HandleMoneyAddCommand,					"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "tp_all",				&CUser::HandleTeleportAllCommand,				"Players send to home zone." },
		{ "summonknights",		&CUser::HandleKnightsSummonCommand,				"Teleport the clan users. Arguments: clan name" },
		{ "warresult",			&CUser::HandleWarResultCommand,					"Set result for War"},
		{ "resetranking",		&CUser::HandleResetPlayerRankingCommand,		"Reset player ranking. Arguments : Zone ID"},
		{ "block",				&CUser::HandlePermanentCommand,					"Player permanent ban" },
		{ "unblock",			&CUser::HandlePermitConnectCommand,				"Player unban" },
		{ "item",				&CUser::HandleGiveItemSelfCommand,				"Gives the item to the using GM. Arguments: item ID | [optional stack size]" },
		{ "summonuser",			&CUser::HandleSummonUserCommand,				"Summons the specified user to your location: Username" },
		{ "tpon",				&CUser::HandleTpOnUserCommand,					"Teleports you to  the specified user's location: Username" },
		{ "goto",				&CUser::HandleLocationChange,					"Teleports you to the specified location. Arguments: X, Y" },
		{ "mute",				&CUser::HandleMuteCommand,						"Player mute" },
		{ "unmute",				&CUser::HandleUnMuteCommand,					"Player unmute" },
		{ "allow",				&CUser::HandleAllowAttackCommand,				"Player Allow Attack" },
		{ "disable",			&CUser::HandleDisableCommand,					"Player Disable Attack" },
		{ "changeroom",			&CUser::HandleChangeRoom,						"Player Changes Event Room" },
		{ "sendtoprison",		&CUser::HandleSummonPrison,						"Player Send to Prsion Zone" },
		{ "testing",			&CUser::HandleServerGameTestCommand,			"Server Code Test Command" },
		{ "openchaos",			&CUser::HandleChaosExpansionOpen,				"Open is Chaos Expansion" },
		{ "openbdw",			&CUser::HandleBorderDefenceWarOpen,				"Open is Border Defence War" },
		{ "openjr",				&CUser::HandleJuraidMountainOpen,				"Open is Juraid Mountain" },
		{ "openroyal",			&CUser::HandleKnightRoyaleOpen,					"Open is Knight Royale" },
		{ "pm_all",				&CUser::HandlePmallCommand,						"Playes Sending Bulk Messages" },
		{ "user_bots",			&CUser::HandleServerBotCommand,					"Server User Bot Command Count Time(AS MINUTE) ResType(1 Mining 2 Fishing 3 Standing 4 Sitting) minlevel" },
		{ "bots_remove",		&CUser::RemoveAllBots,							"Active Bots in the Game Disconnect Important.." },
		{ "fisdrop",			&CUser::HandleFishingDropTester,				"Fishing Drop Tester.." },
		{ "mindrop",			&CUser::HandleMiningDropTester,					"Mining Drop Tester.." },
		{ "mondrop",			&CUser::HandleMonsterDropTester,				"Monster Drop Tester.." },
		{ "openbf",				&CUser::HandleBeefEventOpen,					"Open is Beef Event" },
		{ "opencr",				&CUser::HandleCollectionOpen,					"CollectionRace Open.." },
		{ "opena",				&CUser::HandleOpenArdreamCommand,				"Opens war zone ardream" },
		{ "opencz",				&CUser::HandleOpenCZCommand,					"Opens war zone cz" },
		{ "openbl",				&CUser::HandleBaseEvent,						"Start Base Land Event" },
		{ "closecr",			&CUser::HandleCollectionClose,					"CollectionRace Close.." },
		{ "closecz",			&CUser::HandleCloseCZEventCommand,				"Close war zone Event" },
		{ "closea",				&CUser::HandleCloseArdreamEventCommand,			"Close war zone Event" },
		{ "closebl",			&CUser::HandleBaseLandCloseCommand,				"Close Base Land event" },
		{ "closebf",			&CUser::HandleBeefEventClose,					"Open is Beef Event" },
		{ "cr",					&CUser::HandleCollectionInfoMessage,			"CollectionRace Info Message.." },
		{ "openboss",			&CUser::HandleBossEvent,				        "Boss Event System Manuel Spawn Bos in To Ronark Land zone" },
		{ "mbot",				&CUser::GmBotSystemOpen,						"Gm Merchant Bot Open" },
		{ "all",				&CUser::HandleAllUpCommand,						"All stats."},
		{ "online_item",		&CUser::HandleOnlineItemCommand,				"Gives an item to the whole player. Arguments: Item ID | Count | Time" },
		{ "clearinv",			&CUser::ClearInvCommand,						"Inventory Clear" },
	};

	init_command_table(CUser, commandTable, s_commandTable);
}

void CUser::CleanupChatCommands() { free_command_table(s_commandTable); }

void CUser::Chat(Packet & pkt)
{
	if (!isInGame() 
		|| UNIXTIME2 - m_tLastChatUseTime < 300) // we cannot flood chat in less than 700ms
		return;

	Packet result;
	uint16 sessID;
	uint8 type = pkt.read<uint8>(), bOutType = type, seekingPartyOptions, bNation;
	string chatstr, finalstr, strSender, *strMessage, chattype;
	CUser *pUser;
	CKnights * pKnights;
	DWORD checknow = GetTickCount();
	DateTime time;

	bool isAnnouncement = false;

	if (!isGM()) {
		if (m_mutetime > checknow) return;//süren dolmamış

		if (checknow - m_lastflood <= 1000) //birsaniyeden az
		{
			floodcounter++;
			if (floodcounter >= 6) {
				floodcounter = 0;
				std::string buffer = string_format("[ UYARI ] Oyunda flood yapmak yasaktır! 120 saniyeliğine mutelendiniz.");
				//adama mute yazısı gönderiyoz
				m_mutetime = (checknow + 120000);
				ChatPacket::Construct(&result, PUBLIC_CHAT, &buffer);
				Send(&result);
				return;
			}
		}
		else floodcounter = 0;
		m_lastflood = checknow;// tamam abi
	}

	if (isMuted() 
		|| (GetZoneID() == ZONE_PRISON 
			&& !isGM()))
		return;

	if (!isGM() && GetLevel() < 30 && (type == GENERAL_CHAT || type == SHOUT_CHAT)) {
		g_pMain->SendHelpDescription(this, "Up to 30 levels of general and shout chat available");
		return;
	}

	pkt >> chatstr;
	if (chatstr.empty() 
		|| chatstr.size() > 128)
		return;

	// Process GM commands
	if (isGM() 
		&& ProcessChatCommand(chatstr))
	{
		chattype = "GAME MASTER";
		g_pMain->WriteChatLogFile(string_format("%s - %d:%d:%d || Source=%s,Message=%s,Zone=%d,X=%d,Z=%d\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		return;
	}

	if (type == SEEKING_PARTY_CHAT)
		pkt >> seekingPartyOptions;

	// Handle GM notice & announcement commands
	if (type == PUBLIC_CHAT 
		|| type == ANNOUNCEMENT_CHAT)
	{
		// Trying to use a GM command without authorisation? Bad player!
		if (!isGM())
			return;

		if (type == ANNOUNCEMENT_CHAT)
			type = WAR_SYSTEM_CHAT;

		bOutType = type;

		// This is horrible, but we'll live with it for now.
		// Pull the notice string (#### NOTICE : %s ####) from the database.
		// Format the chat string around it, so our chat data is within the notice
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		isAnnouncement = true;
	}


	if (isAnnouncement)
	{
		// GM notice/announcements show no name, so don't bother setting it.
		strMessage = &finalstr; // use the formatted message from the user
		bNation = KARUS; // arbitrary nation
		sessID = -1;
	}
	else
	{
		strMessage = &chatstr; // use the raw message from the user
		strSender = GetName(); // everything else uses a name, so set it
		if (type == PRIVATE_CHAT && isGM())
		{
			pUser = g_pMain->GetUserPtr(m_sPrivateChatUser, TYPE_CHARACTER);
			if (pUser == nullptr)
				bNation = GetNation();
			else if (!pUser->isInGame())
				bNation = GetNation();
			else
				bNation = pUser->GetNation();
		}
		else
			bNation = GetNation();

		sessID = GetSocketID();
	}

	// GMs should use GM chat to help them stand out amongst players.
	if (type == GENERAL_CHAT 
		&& isGM())
		bOutType = GM_CHAT;

	ChatPacket::Construct(&result, bOutType, strMessage, &strSender, bNation, sessID);
	switch (type)
	{
	/*case GENERAL_CHAT:
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ(), nullptr, GetEventRoom());
		chattype = "GENERAL_CHAT";
		break;*/

	case GENERAL_CHAT:
	{
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ(), nullptr, GetEventRoom());
		chattype = "GENERAL_CHAT";
		//pmevent
		if (g_pMain->UserChatEventOn == true)
		{
			if (g_pMain->UserChatEventKey == chatstr.c_str())
			{
				std::string Message = string_format("Kazanan Oyuncu = %s. Tebrikler!", GetName().c_str());
				g_pMain->SendNotice(Message.c_str(), Nation::ALL);
				// User Chat Event Finally.
				GiveKnightCash(g_pMain->m_ChatEventCashPoint);
				g_pMain->UserChatEventOn = false;
				printf("[Chat Event] : The system is stopped.\n");
				return;
			}
			/*if (g_pMain->UserChatEventKey != chatstr.c_str())
			{
			g_pMain->SendHelpDescription(this, string_format("Dikkat ! %s Yazmanız Gereken Kelimeyi Yanlış Yazdınız.", GetName().c_str()));
			}*/
		}
	}
	break;

	case PRIVATE_CHAT:
	{
		pUser = g_pMain->GetUserPtr(m_sPrivateChatUser, TYPE_CHARACTER);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			return;

		chattype = "PRIVATE_CHAT";
	}
	case COMMAND_PM_CHAT:
	{
		if (type == COMMAND_PM_CHAT 
			&& GetFame() != COMMAND_CAPTAIN)
			return;

		pUser = g_pMain->GetUserPtr(m_sPrivateChatUser, TYPE_CHARACTER);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			return;

		chattype = "COMMAND_PM_CHAT";
		pUser->Send(&result);
	}
	break;
	case PARTY_CHAT:
		if (isInParty())
		{
			g_pMain->Send_PartyMember(GetPartyID(), &result);
			chattype = "PARTY_CHAT";
		}
		break;
	case SHOUT_CHAT:
		if (m_sMp < (m_MaxMp / 5))
			break;

		// Characters under level 35 require 3,000 coins to shout.
		if (!isGM()
			&& GetLevel() < 35
			&& !GoldLose(SHOUT_COIN_REQUIREMENT))
			break;

		MSpChange(-(m_MaxMp / 5));

		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);

		chattype = "SHOUT_CHAT";
		break;
	case KNIGHTS_CHAT:
		if (isInClan())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());

			if (pKnights != nullptr)
				g_pMain->Send_KnightsMember(pKnights->GetID(), &result);

			chattype = "KNIGHTS_CHAT";
		}
		break;
	case CLAN_NOTICE:
		if (isInClan()
			&& isClanLeader())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights == nullptr)
				return;

			pKnights->UpdateClanNotice(chatstr);
			chattype = "CLAN_NOTICE";
		}
		break;
	case PUBLIC_CHAT:
	case ANNOUNCEMENT_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case COMMAND_CHAT:
		if (GetFame() == COMMAND_CAPTAIN)
		{
			g_pMain->Send_CommandChat(&result, m_bNation, this);
			chattype = "COMMAND_CHAT";
		}
		break;
	case MERCHANT_CHAT:
		if (isMerchanting())
			SendToRegion(&result);
		break;
	case ALLIANCE_CHAT:
		if (isInClan())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());

			if (pKnights == nullptr)
				return;

			if (!pKnights->isInAlliance())
				return;

			g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
			chattype = "ALLIANCE_CHAT";
		}
		break;
	case WAR_SYSTEM_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case SEEKING_PARTY_CHAT:
		if (m_bNeedParty == 2)
		{
			Send(&result);
			g_pMain->Send_Zone_Matched_Class(&result, GetZoneID(), this, GetNation(), seekingPartyOptions);
		}
		break;
	case NOAH_KNIGHTS_CHAT:
		if (GetLevel() > 50)
			break;
		g_pMain->Send_Noah_Knights(&result);
		chattype = "NOAH_KNIGHTS_CHAT";
		break;
	case CHATROM_CHAT:
		ChatRoomChat(strMessage, strSender);
		chattype = "CHATROM_CHAT";
		break;
	default:
		TRACE("Unknow Chat : %d", type);
		printf("Unknow Chat : %d", type);
		break;
	}

	if (!chattype.empty())
	{
		if (pUser 
			&& type == 2)
			g_pMain->WriteChatLogFile(string_format("%s-%d:%d:%d || Sender=%s,Target=%s,Message=%s,Zone=%d,X=%d,Z=%d\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), pUser->GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		else if (pKnights 
			&& (type == 6 
				|| type == 15))
			g_pMain->WriteChatLogFile(string_format("%s-%d:%d:%d || Sender=%s,Target=%s,Message=%s,Zone=%d,X=%d,Z=%d\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), pKnights->GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		else
			g_pMain->WriteChatLogFile(string_format("%s-%d:%d:%d || Sender=%s,Message=%s,Zone=%d,X=%d,Z=%d\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
	m_tLastChatUseTime = UNIXTIME2;
}

void CUser::ChatTargetSelect(Packet & pkt)
{
	uint8 type = pkt.read<uint8>();

	// TO-DO: Replace this with an enum
	// Attempt to find target player in-game
	if (type == 1)
	{
		Packet result(WIZ_CHAT_TARGET, type);
		std::string strUserID;
		pkt >> strUserID;
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			return;

		CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pUser == nullptr)
		{
			CBot* pBotUser = g_pMain->GetBotPtr(strUserID, TYPE_CHARACTER);
			if (pBotUser == nullptr)
				result << int16(0);
			else if (pBotUser->isInGame())
			{
				if (pBotUser->isSlaveMerchant())
				{
					pUser = g_pMain->GetUserPtr(pBotUser->GetSlaveGetID());
					if (pUser != nullptr)
						m_sPrivateChatUser = pUser->GetName();
				}
				result << int16(1) << pBotUser->GetName();
			}
			else
				result << int16(0);
		}
		else if (pUser == this)
			result << int16(0);
		else if (pUser->isBlockingPrivateChat())
			result << int16(-1) << pUser->GetName();
		else
		{
			m_sPrivateChatUser = pUser->GetName();
			result << int16(1) << pUser->GetName();
		}
		Send(&result);
	}
	else if (type == 3)
	{
		DateTime time;
		uint8 sSubType;
		std::string sMessage;
		pkt.SByte();
		pkt >> sSubType >> sMessage;

		if (sMessage.empty() || sMessage.size() > 128)
			return;

		g_pMain->WriteChatLogFile(string_format("[ Bugs - %d:%d:%d ] %s : %s.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), sMessage.c_str()));
	}
	// Allow/block PMs
	else
	{
		m_bBlockPrivateChat = pkt.read<bool>();
	}
}

/**
* @brief	Sends a notice to all users in the current zone
* 			upon death.
*
* @param	pKiller	The killer.
*/
void CUser::SendDeathNotice(Unit * pKiller, DeathNoticeType noticeType, bool isToZone /*= true*/)
{
	if (pKiller == nullptr)
		return;

	Packet result(WIZ_CHAT, uint8(DEATH_NOTICE));

	result.SByte();
	result << GetNation()
		<< uint8(noticeType)
		<< pKiller->GetID() // session ID?
		<< pKiller->GetName()
		<< GetID() // session ID?
		<< GetName()
		<< uint16(GetX()) << uint16(GetZ());

	if (isToZone)
		SendToZone(&result, this, pKiller->GetEventRoom(), (isInArena() ? RANGE_20M : 0.0f));
	else
	{
		Send(&result);
		if (pKiller->isPlayer())
			TO_USER(pKiller)->Send(&result);
	}
}

bool CUser::ProcessChatCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
		|| message[0] != CHAT_COMMAND_PREFIX
		// or if we're saying, say, ++++ (faster than looking for the command in the map)
		|| message[1] == CHAT_COMMAND_PREFIX)
		// we're not a command.
		return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ChatCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}


COMMAND_HANDLER(CUser::HandleWarResultCommand)
{
	return !isGM() ? false : g_pMain->HandleWarResultCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWarResultCommand)
{
	// Nation number
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +warresult 1/2 (KARUS/HUMAN)\n");
		return true;
	}

	if (!isWarOpen())
	{
		// send description
		printf("Warning : Battle is not open.\n");
		return true;
	}

	uint8 winner_nation;
	winner_nation = atoi(vargs.front().c_str());

	if (winner_nation > 0 && winner_nation < 3)
		BattleZoneResult(winner_nation);
	return true;
}

bool CGameServerDlg::ProcessServerCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
		|| message[0] != SERVER_COMMAND_PREFIX)
		// we're not a command.
		return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ServerCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

#pragma region CGameServerDlg::HandleHelpCommand
COMMAND_HANDLER(CGameServerDlg::HandleHelpCommand)
{
	foreach(itr, s_commandTable)
	{
		auto i = itr->second;
		std::string s_Command = string_format("Command: /%s, Description: %s \n", i->Name, i->Help);
		printf(s_Command.c_str());
	}
	return true;
}
#pragma endregion

COMMAND_HANDLER(CGameServerDlg::HandleNoticeCommand)
{
	if (vargs.empty())
		return true;

	SendNotice(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeallCommand)
{
	if (vargs.empty())
		return true;

	SendAnnouncement(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleKillUserCommand)
{
	if (vargs.empty())
	{
		// send description
		printf("Using Sample : +kill CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		printf("Error : User is not online\n");
		return true;
	}

	// Disconnect the player
	pUser->Disconnect();

	// send a message saying the player was disconnected
	return true;
}

COMMAND_HANDLER(CUser::HandleWar1OpenCommand)
{
	if (m_GameMastersWarOpen1 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar1OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar1OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 1);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar2OpenCommand)
{
	if (m_GameMastersWarOpen2 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar2OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar2OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 2);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar3OpenCommand)
{
	if (m_GameMastersWarOpen3 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar3OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar3OpenCommand)
{
	g_pMain->m_byBattleZoneType = ZONE_ARDREAM;
	BattleZoneOpen(BATTLEZONE_OPEN, 3);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar4OpenCommand)
{
	if (m_GameMastersWarOpen4 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar4OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar4OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 4);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar5OpenCommand)
{
	if (m_GameMastersWarOpen5 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar5OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar5OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 5);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar6OpenCommand)
{
	if (m_GameMastersWarOpen6 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWar6OpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWar6OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 6);
	return true;
}

COMMAND_HANDLER(CUser::HandleSnowWarOpenCommand)
{
	if (m_GameMastersWarOpen9 != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleSnowWarOpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleSnowWarOpenCommand)
{
	BattleZoneOpen(SNOW_BATTLE);
	return true;
}

COMMAND_HANDLER(CUser::HandleSiegeWarOpenCommand)
{
	if (m_GameMastersWarSiegeOpen != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleSiegeWarOpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleSiegeWarOpenCommand)
{
	CastleSiegeDeathmatchBarrackCreated();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleChaosExpansionOpen)
{
	ChaosExpansionManuelOpening();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleBorderDefenceWar)
{
	BorderDefenceWarManuelOpening();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleJuraidMountain)
{
	JuraidMountainManuelOpening();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleKnightRoyale)
{
	KnightRoyaleManuelOpening();
	return true;
}

COMMAND_HANDLER(CUser::HandleWarCloseCommand)
{
	if (m_GameMastersWarClose != 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Gerekli Yetki Yok. Lütfen Yetki Sınırlandırması için Yönetici ile konuşunuz.");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleWarCloseCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand)
{
	BattleZoneClose();
	return true;
}

COMMAND_HANDLER(CUser::HandleCastleSiegeWarClose)
{
	return !isGM() ? false : g_pMain->HandleCastleSiegeWarClose(vargs, args, description);
}

COMMAND_HANDLER(CGameServerDlg::HandleCastleSiegeWarClose)
{
	CastleSiegeWarGenelClose();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleTournamentClose)
{
	// string & atoi size
	if (vargs.size() < 3)
	{
		// Send Game Server Description
		printf("Using Sample : /tournamentclose TournamentClanNameI TournamentClanNameII & TournamentStartZoneID \n");
		return true;
	}

	std::string TournamentName1 = vargs.front();
	vargs.pop_front();
	std::string TournamentName2 = vargs.front();
	vargs.pop_front();

	uint8 TournamentStartZoneID = atoi(vargs.front().c_str());

	bool SucsessZoneID = (TournamentStartZoneID == 77
		|| TournamentStartZoneID == 78
		|| TournamentStartZoneID == 96
		|| TournamentStartZoneID == 97
		|| TournamentStartZoneID == 98
		|| TournamentStartZoneID == 99);

	if (!SucsessZoneID)
	{
		// Send Game Server Description
		printf("Error: Invalid Tournament Zone(%d) \n", TournamentStartZoneID);
		return true;
	}

	if (TournamentName1.empty() || TournamentName1.size() > 21)
	{
		// Send Game Server Description
		printf("Error: TournamentName1 is empty or size > 21 \n");
		return true;
	}

	if (TournamentName2.empty() || TournamentName2.size() > 21)
	{
		// Send Game Server Description
		printf("Error: TournamentName2 is empty or size > 21 \n");
		return true;
	}

	if (TournamentName1 == TournamentName2)
	{
		// Send Game Server Description
		printf("Error: Two clan names are the same. \n");
		return true;
	}

	_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(TournamentStartZoneID);
	if (TournamentClanInfo == nullptr)
	{
		// Send Game Server Description
		printf("Error: Tournament is Zone(%d) is Close \n", TournamentStartZoneID);
		return true;
	}

	CKnights *pFirstClan = nullptr, *pSecondClan = nullptr;
	g_pMain->m_KnightsArray.m_lock.lock();
	foreach_stlmap_nolock(itr, g_pMain->m_KnightsArray)
	{
		if (!itr->second->GetName().compare(TournamentName1))
			pFirstClan = itr->second;

		if (!itr->second->GetName().compare(TournamentName2))
			pSecondClan = itr->second;
	}
	g_pMain->m_KnightsArray.m_lock.unlock();

	if (pFirstClan == nullptr)
	{
		// Send Game Server Description
		printf("Error : Clan Tournament Close: First clan was not found in database \n");
		return true;
	}

	if (pSecondClan == nullptr)
	{
		// Send Game Server Description
		printf("Error : Clan Tournament Close: Second clan was not found in database \n");
		return true;
	}

	if (TournamentClanInfo != nullptr)
	{
		KickOutZoneUsers(TournamentStartZoneID);
		g_pMain->m_ClanVsDataList.DeleteData(TournamentStartZoneID);
	}

	printf("Final : Tournament is Close: Red Clan: (%s) - (%s) :Blue Clan Tournament Zone (%d) \n", TournamentName1.c_str(), TournamentName1.c_str(), TournamentStartZoneID);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleTournamentStart)
{
	// string & atoi size
	if (vargs.size() < 3)
	{
		// Send Game Server Description
		printf("Using Sample : /tournamentstart TournamentClanNameI TournamentClanNameII & TournamentStartZoneID \n");
		return true;
	}
	// /tournamentstart TestingOneClan TestingTwoClan 77

	std::string TournamentName1 = vargs.front();
	vargs.pop_front();
	std::string TournamentName2 = vargs.front();
	vargs.pop_front();

	uint8 TournamentStartZoneID = atoi(vargs.front().c_str());

	bool ClaniGM = false;
	bool SucsessZoneID = (TournamentStartZoneID == 77
		|| TournamentStartZoneID == 78
		|| TournamentStartZoneID == 96
		|| TournamentStartZoneID == 97
		|| TournamentStartZoneID == 98
		|| TournamentStartZoneID == 99);

	if (!SucsessZoneID)
	{
		// Send Game Server Description
		printf("Error: Invalid Tournament Zone(%d) \n", TournamentStartZoneID);
		return true;
	}

	if (TournamentName1.empty() || TournamentName1.size() > 21)
	{
		// Send Game Server Description
		printf("Error: TournamentName1 is empty or size > 21 \n");
		return true;
	}

	if (TournamentName2.empty() || TournamentName2.size() > 21)
	{
		// Send Game Server Description
		printf("Error: TournamentName2 is empty or size > 21 \n");
		return true;
	}

	if (TournamentName1.c_str() == TournamentName2.c_str())
	{
		// Send Game Server Description
		printf("Error: Two clan names are the same. \n");
		return true;
	}

	_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(TournamentStartZoneID);
	if (TournamentClanInfo != nullptr)
	{
		// Send Game Server Description
		printf("Error: Tournament is Zone(%d) Already Active \n", TournamentStartZoneID);
		return true;
	}

	_TOURNAMENT_DATA* Tour77 = g_pMain->m_ClanVsDataList.GetData(77);
	_TOURNAMENT_DATA* Tour78 = g_pMain->m_ClanVsDataList.GetData(78);
	_TOURNAMENT_DATA* Tour96 = g_pMain->m_ClanVsDataList.GetData(96);
	_TOURNAMENT_DATA* Tour97 = g_pMain->m_ClanVsDataList.GetData(97);
	_TOURNAMENT_DATA* Tour98 = g_pMain->m_ClanVsDataList.GetData(98);
	_TOURNAMENT_DATA* Tour99 = g_pMain->m_ClanVsDataList.GetData(99);

	CKnights *pAlreadyFirstClan = nullptr, *pAlreadySecondClan = nullptr;
	foreach_stlmap(itr, g_pMain->m_KnightsArray)
	{
		if (!itr->second->GetName().compare(TournamentName1))
			pAlreadyFirstClan = itr->second;

		if (!itr->second->GetName().compare(TournamentName2))
			pAlreadySecondClan = itr->second;
	}

	if (pAlreadyFirstClan != nullptr)
	{
		if (Tour77 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour77->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour77->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour77->aTournamentZoneID);
				return true;

			}
		}

		if (Tour78 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour78->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour78->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour78->aTournamentZoneID);
				return true;

			}
		}

		if (Tour96 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour96->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour96->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour96->aTournamentZoneID);
				return true;

			}
		}

		if (Tour97 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour97->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour97->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour97->aTournamentZoneID);
				return true;

			}
		}

		if (Tour98 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour98->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour98->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour98->aTournamentZoneID);
				return true;

			}
		}

		if (Tour99 != nullptr)
		{
			if (pAlreadyFirstClan->GetID() == Tour99->aTournamentClanNum[0]
				|| pAlreadyFirstClan->GetID() == Tour99->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadyFirstClan->GetName().c_str(), Tour99->aTournamentZoneID);
				return true;

			}
		}
	}

	if (pAlreadySecondClan != nullptr)
	{
		if (Tour77 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour77->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour77->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour77->aTournamentZoneID);
				return true;

			}
		}

		if (Tour78 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour78->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour78->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour78->aTournamentZoneID);
				return true;

			}
		}

		if (Tour96 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour96->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour96->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour96->aTournamentZoneID);
				return true;

			}
		}

		if (Tour97 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour97->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour97->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour97->aTournamentZoneID);
				return true;

			}
		}

		if (Tour98 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour98->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour98->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour98->aTournamentZoneID);
				return true;

			}
		}

		if (Tour99 != nullptr)
		{
			if (pAlreadySecondClan->GetID() == Tour99->aTournamentClanNum[0]
				|| pAlreadySecondClan->GetID() == Tour99->aTournamentClanNum[1])
			{
				// Send Game Server Description
				printf("Error: Second Specified clan (%s) is fighting against another clan. Other ZoneID: (%d) \n", pAlreadySecondClan->GetName().c_str(), Tour99->aTournamentZoneID);
				return true;

			}
		}
	}

	bool isClanVs = (TournamentStartZoneID == 77 || TournamentStartZoneID == 78);
	bool isPartyVs = (TournamentStartZoneID == 96 || TournamentStartZoneID == 97 || TournamentStartZoneID == 98 || TournamentStartZoneID == 99);
	if (isClanVs)
	{
		CKnights *pFirstClan = nullptr, *pSecondClan = nullptr;
		g_pMain->m_KnightsArray.m_lock.lock();
		foreach_stlmap_nolock(itr, g_pMain->m_KnightsArray)
		{
			if (!itr->second->GetName().compare(TournamentName1))
				pFirstClan = itr->second;

			if (!itr->second->GetName().compare(TournamentName2))
				pSecondClan = itr->second;
		}
		g_pMain->m_KnightsArray.m_lock.unlock();

		if (pFirstClan == nullptr)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: First clan was not found in database \n");
			return true;
		}

		if (pSecondClan == nullptr)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: Second clan was not found in database \n");
			return true;
		}

		uint16 FirstClanOnlineCount = 0;
		pFirstClan->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pFirstClan->m_arKnightsUser)
		{
			_KNIGHTS_USER * pFirstClanKnightUser = itr->second;
			if (pFirstClanKnightUser == nullptr)
				continue;

			CUser* pFirstClanUser = g_pMain->GetUserPtr(pFirstClanKnightUser->strUserName, TYPE_CHARACTER);
			if (pFirstClanUser == nullptr)
				continue;

			if (pFirstClanUser->isGM())
			{
				ClaniGM = true;
				break;
			}

			if (pFirstClanUser != nullptr
				&& pFirstClanUser->isInGame()
				&& pFirstClanUser->GetZoneID() != TournamentStartZoneID)
				FirstClanOnlineCount++;
		}
		pFirstClan->m_arKnightsUser.m_lock.unlock();

		if (ClaniGM == true)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: There is an administrator in Clan (%s) \n", TournamentName1.c_str());
			return true;
		}

		if (FirstClanOnlineCount > TOURNAMENT_MAX_CLAN_COUNT)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: %s Clan is clan high count. (count: %d) Maxmium Count: (%d)\n", TournamentName1.c_str(), FirstClanOnlineCount, TOURNAMENT_MAX_CLAN_COUNT);
			return true;
		}

		if (FirstClanOnlineCount < TOURNAMENT_MIN_CLAN_COUNT)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: %s Clan is clan low count. (count: %d) Minumum Count: (%d)\n", TournamentName1.c_str(), FirstClanOnlineCount, TOURNAMENT_MIN_CLAN_COUNT);
			return true;
		}

		uint16 SecondClanOnlineCount = 0;
		pSecondClan->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pSecondClan->m_arKnightsUser)
		{
			_KNIGHTS_USER * pSecondClanKnightUser = itr->second;
			if (pSecondClanKnightUser == nullptr)
				continue;

			CUser* pSecondClanUser = g_pMain->GetUserPtr(pSecondClanKnightUser->strUserName, TYPE_CHARACTER);
			if (pSecondClanUser == nullptr)
				continue;

			if (pSecondClanUser->isGM())
			{
				ClaniGM = true;
				break;
			}

			if (pSecondClanUser != nullptr
				&& pSecondClanUser->isInGame()
				&& pSecondClanUser->GetZoneID() != TournamentStartZoneID)
				SecondClanOnlineCount++;
		}
		pSecondClan->m_arKnightsUser.m_lock.unlock();

		if (ClaniGM == true)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: There is an administrator in Clan (%s) \n", TournamentName2.c_str());
			return true;
		}

		if (SecondClanOnlineCount > TOURNAMENT_MAX_CLAN_COUNT)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: %s Clan is clan high count. (count: %d) Maxmium Count: (%d) \n", TournamentName2.c_str(), SecondClanOnlineCount, TOURNAMENT_MAX_CLAN_COUNT);
			return true;
		}

		if (SecondClanOnlineCount < TOURNAMENT_MIN_CLAN_COUNT)
		{
			// Send Game Server Description
			printf("Error : Clan Tournament: %s Clan is clan low count. (count: %d) Minumum Count: (%d) \n", TournamentName2.c_str(), SecondClanOnlineCount, TOURNAMENT_MIN_CLAN_COUNT);
			return true;
		}

		KickOutZoneUsers(TournamentStartZoneID);

		if (TournamentClanInfo == nullptr)
		{
			_TOURNAMENT_DATA *cTournamentSign = new _TOURNAMENT_DATA;
			cTournamentSign->aTournamentZoneID = TournamentStartZoneID;
			cTournamentSign->aTournamentClanNum[0] = pFirstClan->GetID();
			cTournamentSign->aTournamentClanNum[1] = pSecondClan->GetID();
			cTournamentSign->aTournamentScoreBoard[0] = 0;
			cTournamentSign->aTournamentScoreBoard[1] = 0;
			cTournamentSign->aTournamentTimer = 30 * 60;
			cTournamentSign->aTournamentMonumentKilled = 0;
			cTournamentSign->aTournamentOutTimer = UNIXTIME;
			cTournamentSign->aTournamentisAttackable = false;
			cTournamentSign->aTournamentisStarted = true;
			cTournamentSign->aTournamentisFinished = false;

			if (!m_ClanVsDataList.PutData(cTournamentSign->aTournamentZoneID, cTournamentSign))
				delete cTournamentSign;
		}

		pFirstClan->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pFirstClan->m_arKnightsUser)
		{
			_KNIGHTS_USER * pFirstClanKnightUser = itr->second;
			if (pFirstClanKnightUser == nullptr)
				continue;

			CUser* pFirstClanUser = g_pMain->GetUserPtr(pFirstClanKnightUser->strUserName, TYPE_CHARACTER);
			if (pFirstClanUser == nullptr
				|| !pFirstClanUser->isInGame()
				|| pFirstClanUser->GetZoneID() == TournamentStartZoneID)
				continue;

			pFirstClanUser->ZoneChange(TournamentStartZoneID, 0.0f, 0.0f);
		}
		pFirstClan->m_arKnightsUser.m_lock.unlock();

		pSecondClan->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pSecondClan->m_arKnightsUser)
		{
			_KNIGHTS_USER * pSecondClanKnightUser = itr->second;
			if (pSecondClanKnightUser == nullptr)
				continue;

			CUser* pSecondClanUser = g_pMain->GetUserPtr(pSecondClanKnightUser->strUserName, TYPE_CHARACTER);
			if (pSecondClanUser == nullptr
				|| !pSecondClanUser->isInGame()
				|| pSecondClanUser->GetZoneID() == TournamentStartZoneID)
				continue;

			pSecondClanUser->ZoneChange(TournamentStartZoneID, 0.0f, 0.0f);
		}
		pSecondClan->m_arKnightsUser.m_lock.unlock();
	}
	else if (isPartyVs)
	{
		CKnights *pFirstClanParty = nullptr, *pSecondClanParty = nullptr;
		g_pMain->m_KnightsArray.m_lock.lock();
		foreach_stlmap(itr, g_pMain->m_KnightsArray)
		{
			if (!itr->second->GetName().compare(TournamentName1))
				pFirstClanParty = itr->second;

			if (!itr->second->GetName().compare(TournamentName2))
				pSecondClanParty = itr->second;
		}
		g_pMain->m_KnightsArray.m_lock.unlock();

		if (pFirstClanParty == nullptr)
		{
			// Send Game Server Description
			printf("Error :Party Tournament: First clan was not found in database \n");
			return true;
		}

		if (pSecondClanParty == nullptr)
		{
			// Send Game Server Description
			printf("Error :Party Tournament: Second clan was not found in database \n");
			return true;
		}

		uint16 FirstClanOnlineCount = 0;
		pFirstClanParty->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pFirstClanParty->m_arKnightsUser)
		{
			_KNIGHTS_USER * pFirstClanPartyKnightUser = itr->second;
			if (pFirstClanPartyKnightUser == nullptr)
				continue;

			CUser* pFirstClanUser = g_pMain->GetUserPtr(pFirstClanPartyKnightUser->strUserName, TYPE_CHARACTER);
			if (pFirstClanUser != nullptr
				&& pFirstClanUser->isInGame()
				&& pFirstClanUser->GetZoneID() != TournamentStartZoneID)
				FirstClanOnlineCount++;
		}
		pFirstClanParty->m_arKnightsUser.m_lock.unlock();

		if (FirstClanOnlineCount > TOURNAMENT_MAX_PARTY_COUNT)
		{
			// Send Game Server Description
			printf("Error : Party Tournament: %s Clan is clan high count. (count: %d) Maximum Count (%d) \n", TournamentName1.c_str(), FirstClanOnlineCount, TOURNAMENT_MAX_PARTY_COUNT);
			return true;
		}

		if (FirstClanOnlineCount < TOURNAMENT_MIN_PARTY_COUNT)
		{
			// Send Game Server Description
			printf("Error : Party Tournament: %s Clan is clan low count. (count: %d) Minumum Count (%d)\n", TournamentName1.c_str(), FirstClanOnlineCount, TOURNAMENT_MIN_PARTY_COUNT);
			return true;
		}

		uint16 SecondClanOnlineCount = 0;
		pSecondClanParty->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pSecondClanParty->m_arKnightsUser)
		{
			_KNIGHTS_USER * pSecondClanPartyKnightUser = itr->second;
			if (pSecondClanPartyKnightUser == nullptr)
				continue;

			CUser* pSecondClanUser = g_pMain->GetUserPtr(pSecondClanPartyKnightUser->strUserName, TYPE_CHARACTER);
			if (pSecondClanUser != nullptr
				&& pSecondClanUser->isInGame()
				&& pSecondClanUser->GetZoneID() != TournamentStartZoneID)
				SecondClanOnlineCount++;
		}
		pSecondClanParty->m_arKnightsUser.m_lock.unlock();

		if (SecondClanOnlineCount > TOURNAMENT_MAX_PARTY_COUNT)
		{
			// Send Game Server Description
			printf("Error : Party Tournament: %s Clan is clan high count. (count: %d) Maximum Count (%d) \n", TournamentName2.c_str(), SecondClanOnlineCount, TOURNAMENT_MAX_PARTY_COUNT);
			return true;
		}

		if (SecondClanOnlineCount < TOURNAMENT_MIN_PARTY_COUNT)
		{
			// Send Game Server Description
			printf("Error : Party Tournament: %s Clan is clan low count. (count: %d) Minumum Count (%d)\n", TournamentName2.c_str(), SecondClanOnlineCount, TOURNAMENT_MIN_PARTY_COUNT);
			return true;
		}

		KickOutZoneUsers(TournamentStartZoneID);

		if (TournamentClanInfo == nullptr)
		{
			_TOURNAMENT_DATA *cTournamentSign = new _TOURNAMENT_DATA;
			cTournamentSign->aTournamentZoneID = TournamentStartZoneID;
			cTournamentSign->aTournamentClanNum[0] = pFirstClanParty->GetID();
			cTournamentSign->aTournamentClanNum[1] = pSecondClanParty->GetID();
			cTournamentSign->aTournamentScoreBoard[0] = 0;
			cTournamentSign->aTournamentScoreBoard[1] = 0;
			cTournamentSign->aTournamentTimer = 0;
			cTournamentSign->aTournamentMonumentKilled = 0;
			cTournamentSign->aTournamentOutTimer = UNIXTIME;
			cTournamentSign->aTournamentisAttackable = false;
			cTournamentSign->aTournamentisStarted = true;
			cTournamentSign->aTournamentisFinished = false;

			if (!m_ClanVsDataList.PutData(cTournamentSign->aTournamentZoneID, cTournamentSign))
				delete cTournamentSign;
		}

		pFirstClanParty->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pFirstClanParty->m_arKnightsUser)
		{
			_KNIGHTS_USER * pFirstClanPartyKnightUser = itr->second;
			if (pFirstClanPartyKnightUser == nullptr)
				continue;

			CUser* pFirstClanUser = g_pMain->GetUserPtr(pFirstClanPartyKnightUser->strUserName, TYPE_CHARACTER);
			if (pFirstClanUser == nullptr
				|| !pFirstClanUser->isInGame()
				|| pFirstClanUser->GetZoneID() == TournamentStartZoneID)
				continue;

			pFirstClanUser->ZoneChange(TournamentStartZoneID, 0.0f, 0.0f);
		}
		pFirstClanParty->m_arKnightsUser.m_lock.unlock();

		pSecondClanParty->m_arKnightsUser.m_lock.lock();
		foreach_stlmap_nolock(itr, pSecondClanParty->m_arKnightsUser)
		{
			_KNIGHTS_USER * pSecondClanPartyKnightUser = itr->second;
			if (pSecondClanPartyKnightUser == nullptr)
				continue;

			CUser* pSecondClanUser = g_pMain->GetUserPtr(pSecondClanPartyKnightUser->strUserName, TYPE_CHARACTER);
			if (pSecondClanUser == nullptr
				|| !pSecondClanUser->isInGame()
				|| pSecondClanUser->GetZoneID() == TournamentStartZoneID)
				continue;

			pSecondClanUser->ZoneChange(TournamentStartZoneID, 0.0f, 0.0f);
		}
		pSecondClanParty->m_arKnightsUser.m_lock.unlock();

	}
	printf("Final : Tournament is Start: Red Clan: (%s) - (%s) :Blue Clan Tournament Zone (%d) \n", TournamentName1.c_str(), TournamentName1.c_str(), TournamentStartZoneID);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleEventUnderTheCastleCommand)
{
	// Nation number
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +underthecastle 1/2 (Open/Close)\n");
		return true;
	}

	uint8 type;
	type = atoi(vargs.front().c_str());

	if (type == 1)
	{
		m_bUnderTheCastleIsActive = true;
		m_bUnderTheCastleMonster = true;
		m_nUnderTheCastleEventTime = 180 * MINUTE;
		printf("Under The Castle Stard\n");
	}

	if (type == 2)
	{
		m_nUnderTheCastleEventTime = 10;
		printf("Under The Castle Closed\n");
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentBannedCommand)
{
	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +block CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sOperatorCommandType = "Blocked by the Administrator.";
	std::string Server = "<Server-sided>";
	result << Server << strUserID << uint16(AUTHORITY_BANNED) << uint8(BanTypes::BANNED) << uint32(999) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), Server.c_str(), sOperatorCommandType.c_str(), strUserID.c_str()));
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermitConnectCommand)
{
	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +unblock CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	Packet result(WIZ_AUTHORITY_CHANGE);
	DateTime time;
	std::string sOperatorCommandType = "Unblocked by the Administrator.";
	std::string Server = "<Server-sided>";
	result << Server << strUserID << uint16(AUTHORITY_PLAYER) << uint8(BanTypes::UNBAN) << uint32(0) << sOperatorCommandType;
	g_pMain->AddDatabaseRequest(result);

	g_pMain->WriteBanListLogFile(string_format("%d:%d:%d || Sender=%s,Command=%s,User=%s,Period=Permanent\n",
		time.GetHour(), time.GetMinute(), time.GetSecond(), Server.c_str(), sOperatorCommandType.c_str(), strUserID.c_str()));
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleTeleportAllCommand)
{
	// Zone number
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : /tp_all ZoneNumber | /tp_all ZoneNumber TargetZoneNumber.\n");
		return true;
	}

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID, nTargetZoneID);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleShutdownCommand)
{
	m_GameServerShutDownOK = true;
	m_GameServerShutDownSeconds = 300;

	Packet x;
	x.Initialize(WIZ_LOGOSSHOUT);
	x << uint8(2) << uint8(6) << uint8(5);
	Send_All(&x);
	printf("Server Shut Down in (%d) Seconds \n", m_GameServerShutDownSeconds);

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountCommand)
{
	m_sDiscount = 1;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGlobalDiscountCommand)
{
	m_sDiscount = 2;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountOffCommand)
{
	m_sDiscount = 0;
	return true;
}

COMMAND_HANDLER(CUser::HandleCaptainCommand) { return !isGM() ? false : g_pMain->HandleCaptainCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCaptainCommand)
{
	BattleZoneSelectCommanders();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaCommand)
{
	m_bSantaOrAngel = FLYING_SANTA;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaOffCommand)
{
	m_bSantaOrAngel = FLYING_NONE;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleAngelCommand)
{
	m_bSantaOrAngel = FLYING_ANGEL;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatCommand)
{
	if (vargs.empty())
	{
		// send error saying we need args (unlike the previous implementation of this command)
		return true;
	}

	SetPermanentMessage("%s", args);
	return true;
}

void CGameServerDlg::SendHelpDescription(CUser *pUser, std::string sHelpMessage)
{
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result(WIZ_CHAT, (uint8)PUBLIC_CHAT);
	result << pUser->GetNation() << pUser->GetSocketID() << uint8(0) << sHelpMessage;
	pUser->Send(&result);
}

void CGameServerDlg::SetPermanentMessage(const char * format, ...)
{
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	m_bPermanentChatMode = true;
	m_strPermanentChat = buffer;

	Packet result;
	ChatPacket::Construct(&result, PERMANENT_CHAT, &m_strPermanentChat);
	Send_All(&result);
}

COMMAND_HANDLER(CUser::HandleReload)
{
	if (!isGM())
		return false;
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +reload tableName");
		return true;
	}
	g_pMain->HandleReloadTableCommand(vargs, args, "");
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTableCommand)
{
	if (vargs.size() < 1)
	{
		return true;
	}

	std::string table = vargs.front();
	vargs.pop_front();

	if (table.compare("ITEM_UPGRADE") == 0)
	{
		m_ItemUpgradeArray.DeleteAllData();
		LoadItemUpgradeTable();
	}
	else if (table.compare("ITEM_EXCHANGE") == 0)
	{
		m_ItemExchangeArray.DeleteAllData();
		LoadItemExchangeTable();
	}
	else if (table.compare("ITEM_EXCHANGE_EXP") == 0) {
		m_ItemExchangeExpArray.DeleteAllData();
		LoadItemExchangeExpTable();
	}
	else if (table.compare("K_MONSTER_ITEM") == 0)
	{
		m_NpcItemArray.DeleteAllData();
		m_MakeItemGroupArray.DeleteAllData();
		LoadNpcItemTable();
		LoadMakeItemGroupTable();
	}
	else if (table.compare("MINING_FISHING_ITEM") == 0)
	{
		m_MiningFishingItemArray.DeleteAllData();
		LoadMiningFishingItemTable();
	}
	else if (table.compare("ITEM") == 0)
	{
		m_ItemtableArray.DeleteAllData();
		LoadItemTable();
	}
	else if (table.compare("K_NPC") == 0)
	{
		m_arNpcTable.DeleteAllData();
		LoadNpcTableData(true);
	}
	else if (table.compare("K_MONSTER") == 0)
	{
		m_arMonTable.DeleteAllData();
		LoadNpcTableData(false);
	}
	else if (table.compare("K_NPCPOS") == 0)
	{
		m_arMonTable.DeleteAllData();
		LoadNpcTableData(false);
	}
	/*else if (table.compare("K_NPCPOS") == 0)
	{
		RLOCK(g_pMain->m_arNpcArray)
			g_pAIServer->m_arNpcThread.clear();

		g_pAIServer->m_TotalNPC = 0;
		m_arNpcArray.DeleteAllData();
		LoadNpcPosTable();
		RULOCK(g_pMain->m_arNpcArray)
	}*/
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatOffCommand)
{
	Packet result;
	ChatPacket::Construct(&result, END_PERMANENT_CHAT);
	m_bPermanentChatMode = false;
	Send_All(&result);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadNoticeCommand)
{
	// Reload the notice data
	LoadNoticeData();
	LoadNoticeUpData();

	// and update all logged in players
	// if we're using the new notice format that's always shown
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame())
		{
			pUser->SendNotice();
			pUser->TopSendNotice();
		}
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadDrakiTowerTables)
{
	m_DrakiMonsterListArray.DeleteAllData();
	m_DrakiRoomListArray.DeleteAllData();
	LoadDrakiTowerTables();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadDungeonDefenceTables)
{
	m_DungeonDefenceMonsterListArray.DeleteAllData();
	LoadDungeonDefenceMonsterTable();

	m_DungeonDefenceStageListArray.DeleteAllData();
	LoadDungeonDefenceStageTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTablesCommand)
{
	m_UserExpirationItemArray.DeleteAllData();
	LoadExpirationItemTable();	

	m_StartPositionArray.DeleteAllData();
	LoadStartPositionTable();

	m_StartPositionRandomArray.DeleteAllData();
	LoadStartPositionRandomTable();

	m_ItemExchangeArray.DeleteAllData();
	LoadItemExchangeTable();

	m_ItemExchangeExpArray.DeleteAllData();
	LoadItemExchangeExpTable();

	m_ItemSpecialExchangeArray.DeleteAllData();
	LoadItemSpecialExchangeTable();

	m_ItemExchangeCrashArray.DeleteAllData();
	LoadItemExchangeCrashTable();

	m_ItemUpgradeArray.DeleteAllData();
	LoadItemUpgradeTable();

	m_EventTriggerArray.DeleteAllData();
	LoadEventTriggerTable();

	m_ServerResourceArray.DeleteAllData();
	LoadServerResourceTable();

	m_MonsterResourceArray.DeleteAllData();
	LoadMonsterResourceTable();

	m_MonsterChallengeArray.DeleteAllData();
	LoadMonsterChallengeTable();

	m_MonsterChallengeSummonListArray.DeleteAllData();
	LoadMonsterChallengeSummonListTable();

	PusItemArray.DeleteAllData();
	LoadPusLoad();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTables2Command)
{
	m_WarBanishOfWinnerArray.DeleteAllData();
	LoadBanishWinnerTable();

	m_DungeonDefenceMonsterListArray.DeleteAllData();
	LoadDungeonDefenceMonsterTable();

	m_DungeonDefenceStageListArray.DeleteAllData();
	LoadDungeonDefenceStageTable();

	m_DrakiMonsterListArray.DeleteAllData();
	m_DrakiRoomListArray.DeleteAllData();
	LoadDrakiTowerTables();

	m_LuaGiveItemExchangeArray.DeleteAllData();
	LoadGiveItemExchangeTable();

	m_HackToolListArray.DeleteAllData();
	LoadUserHackToolTables();

	m_KnightRoayleStatsListArray.DeleteAllData();
	LoadKnightRoyaleStatsTable();

	m_KnightRoyaleBeginnerItemListArray.DeleteAllData();
	LoadKnightRoyaleBeginnerItemTable();

	m_KnightRoyaleTreasureChestListArray.DeleteAllData();
	LoadKnightRoyaleChestListTable();

	m_KnStartPositionRandomArray.DeleteAllData();
	LoadKnightRoyaleStartPositionRandomTable();

	m_KnightCastellanCapeArray.DeleteAllData();
	LoadKnightsCastellanCapeTable();
	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleReloadTables3Command)
{
	m_MonsterVeriableRespawnListArray.DeleteAllData();
	LoadMonsterRespawnVeriableListTable();

	m_MonsterStableRespawnListArray.DeleteAllData();
	LoadMonsterRespawnStableListTable();

	m_MonsterSummonList.DeleteAllData();
	LoadMonsterSummonListTable();

	m_MonsterUnderTheCastleArray.DeleteAllData();
	LoadMonsterUnderTheCastleTable();

	m_MonsterStoneListInformationArray.DeleteAllData();
	LoadMonsterStoneListInformationTable();

	m_JuraidMountionListInformationArray.DeleteAllData();
	LoadJuraidMountionListInformationTable();

	m_ChaosStoneSummonListArray.DeleteAllData();
	LoadChaosStoneMonsterListTable();

	m_ChaosStoneRespawnCoordinateArray.DeleteAllData();
	LoadChaosStoneCoordinateTable();

	m_ChaosStoneStageArray.DeleteAllData();
	LoadChaosStoneStage();

	m_MiningExchangeArray.DeleteAllData();
	LoadMiningExchangeListTable();

	m_MiningFishingItemArray.DeleteAllData();
	LoadMiningFishingItemTable();
	SetMiningDrops();

	m_ItemOpArray.DeleteAllData();
	LoadItemOpTable();

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadMagicsCommand)
{
	m_IsMagicTableInUpdateProcess = true;
	m_MagictableArray.DeleteAllData();
	m_Magictype1Array.DeleteAllData();
	m_Magictype2Array.DeleteAllData();
	m_Magictype3Array.DeleteAllData();
	m_Magictype4Array.DeleteAllData();
	m_Magictype5Array.DeleteAllData();
	m_Magictype6Array.DeleteAllData();
	m_Magictype8Array.DeleteAllData();
	m_Magictype9Array.DeleteAllData();
	LoadMagicTable();
	LoadMagicType1();
	LoadMagicType2();
	LoadMagicType3();
	LoadMagicType4();
	LoadMagicType5();
	LoadMagicType6();
	LoadMagicType7();
	LoadMagicType8();
	LoadMagicType9();
	m_IsMagicTableInUpdateProcess = false;

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadAutoNoticeTables)
{
	m_ServerResourceArray.DeleteAllData();
	LoadServerResourceTable();
	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleEventScheduleResetTable)
{
	if (pTempleEvent.ActiveEvent != -1
		|| pKnightRoyaleEvent.ActiveEvent != -1
		|| pCswEvent.Started
		|| m_byBattleOpen != NO_BATTLE)
	{
		printf("ongoing event Warning\n");
		return true;
	}

	m_EventScheduleArray.DeleteAllData();
	LoadEventScheduleStatusTable();

	m_RoomEventPlayTimerArray.DeleteAllData();
	LoadRoomEventPlayTimerTable();

	m_RoyaleEventPlayTimerArray.DeleteAllData();
	LoadRoyaleEventPlayTimerTable();

	m_CswEventPlayTimerArray.DeleteAllData();
	LoadCswEventPlayTimerTable();

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadQuestCommand)
{
	m_QuestHelperArray.DeleteAllData();
	LoadQuestHelperTable();
	m_QuestMonsterArray.DeleteAllData();
	LoadQuestMonsterTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadRanksCommand)
{
	ReloadKnightAndUserRanks();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadDropsCommand)
{
	m_MakeItemGroupArray.DeleteAllData();
	LoadMakeItemGroupTable();
	m_NpcItemArray.DeleteAllData();
	LoadNpcItemTable();
	m_MonsterItemArray.DeleteAllData();
	LoadMonsterItemTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadKingsCommand)
{
	m_KingSystemArray.DeleteAllData();
	LoadKingSystem();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadItemsCommand)
{
	m_ItemtableArray.DeleteAllData();
	LoadItemTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCountCommand)
{
	uint16 count = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame())
			count++;
	}

	/*Guard lock(g_pMain->m_BotcharacterNameLock);
	count += (uint16)g_pMain->m_BotcharacterNameMap.size();*/

	printf("Online User Count : %d\n", count);
	return true;
}

void CGameServerDlg::SendFormattedResource(uint32 nResourceID, uint8 byNation, bool bIsNotice, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
		return;

	string buffer;
	va_list ap;
	va_start(ap, bIsNotice);
	_string_format(pResource->strResource, &buffer, ap);
	va_end(ap);

	if (bIsNotice)
		SendNotice(buffer.c_str(), byNation);
	else
		SendAnnouncement(buffer.c_str(), byNation);
}

COMMAND_HANDLER(CUser::HandleServerGameTestCommand)
{
	if (!isGM())
		return false;

	return true;
}

COMMAND_HANDLER(CUser::HandleServerBotCommand)
{
	if (!isGM())
		return false;

	if (vargs.size() < 4)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +user_bots Count Time(AS MINUTE) ResType(1 Mining 2 Fishing 3 Standing 4 Sitting) minlevel");
		return true;
	}

	int Minute = 0, Restie = 0, minlevel = 0, sCount = 0;

	sCount = atoi(vargs.front().c_str());

	vargs.pop_front();

	Minute = atoi(vargs.front().c_str());

	vargs.pop_front();

	Restie = atoi(vargs.front().c_str());

	vargs.pop_front();

	minlevel = atoi(vargs.front().c_str());


	for (int i = 0; i < sCount; i++)
	{
		float Bonc = myrand(1, 15) * 1.0f;
		float Bonc2 = myrand(1, 15) * 1.0f;

		g_pMain->SpawnUserBot(Minute, GetZoneID(), GetX() + Bonc, GetY(), GetZ() + Bonc2, Restie, minlevel);
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleServerGameTestCommand)
{
	return true;
}

#pragma region CUser::HandleChaosExpansionOpen
COMMAND_HANDLER(CUser::HandleChaosExpansionOpen)
{
	if (!isGM())
		return false;

	g_pMain->ChaosExpansionManuelOpening();
	return true;
}
#pragma endregion

#pragma region CUser::HandleBorderDefenceWarOpen
COMMAND_HANDLER(CUser::HandleBorderDefenceWarOpen)
{
	if (!isGM())
		return false;

	g_pMain->BorderDefenceWarManuelOpening();
	return true;
}
#pragma endregion

#pragma region CUser::HandleJuraidMountainOpen
COMMAND_HANDLER(CUser::HandleJuraidMountainOpen)
{
	if (!isGM())
		return false;

	g_pMain->JuraidMountainManuelOpening();
	return true;
}
#pragma endregion

#pragma region CUser::HandleKnightRoyaleOpen
COMMAND_HANDLER(CUser::HandleKnightRoyaleOpen)
{
	if (!isGM())
		return false;

	g_pMain->KnightRoyaleManuelOpening();
	return true;
}
#pragma endregion

#pragma region CUser::HandlePmallCommand
COMMAND_HANDLER(CUser::HandlePmallCommand)
{
	if (!isGM())
		return false;

	// string & atoi size
	if (vargs.size() < 2)
	{
		// Send Game Server Description
		printf("Using Sample : +pm_all Title Message \n");
		return true;
	}

	Packet result;
	std::string sTitle = vargs.front();
	vargs.pop_front();
	
	uint16 vargsize = (uint16)vargs.size();

	if (vargsize >= (1 + 50))
	{
		// send description
		g_pMain->SendHelpDescription(this, "Error : long word!");
		return true;
	}

	std::string word[50];
	std::string guncelword = "";
	for (int x = 1; x <= vargsize; x++)
	{
		word[x] = vargs.front();
		vargs.pop_front();

		if (!word[x].empty())
		{
			if (word[x].size() > 75)
			{
				// send description
				g_pMain->SendHelpDescription(this, "Error : long word!");
				return true;
			}
		}
		else
		{
			word[x] = "";
		}
		guncelword += word[x] + ' ';
	}

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		ChatPacket::Construct(&result, ChatType::PRIVATE_CHAT, guncelword.c_str(), sTitle.c_str(), pUser->GetNation());
		pUser->Send(&result);
	}

	return true;
}
#pragma endregion

#pragma region CUser::RemoveAllBots
COMMAND_HANDLER(CUser::RemoveAllBots)
{
	if (!isGM())
		return false;

	uint16 sCount = 0;

	foreach_stlmap(itr, g_pMain->m_BotArray)
	{
		CBot* Bot = itr->second;

		if (Bot == nullptr)
			continue;

		if (Bot->m_state != GAME_STATE_INGAME)
			continue;

		Bot->LastWarpTime = UNIXTIME + 5;
		sCount++;
	}

	if (sCount >= 1)
		g_pMain->SendHelpDescription(this, string_format("[Bot Process]:% d Bot is DC", sCount));
	else
		g_pMain->SendHelpDescription(this, "[Bot Process]: No Active Bot");

	return true;
}
#pragma endregion

#pragma region CUser::HandleFishingDropTester
COMMAND_HANDLER(CUser::HandleFishingDropTester)
{
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +fishingdrop sType sHours \n");
		return true;
	}
	uint8 sType = 0; uint8 sHours = 0;

	sType = atoi(vargs.front().c_str());
	vargs.pop_front();
	sHours = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (sHours <= 0)
		return false;

	HandleFishingDropTester(sType, sHours);
	return true;
}
#pragma endregion

#pragma region CUser::HandleMonsterDropTester
COMMAND_HANDLER(CUser::HandleMonsterDropTester)
{
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +monsterdrop sHours \n");
		return true;
	}
	uint8 sHours = 0;

	sHours = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (sHours <= 0)
		return false;

	HandleMonsterDropTester(GetTargetID(), sHours);
	return true;
}
#pragma endregion

#pragma region CUser::HandleMiningDropTester
COMMAND_HANDLER(CUser::HandleMiningDropTester)
{
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +miningdrop sType sHours \n");
		return true;
	}
	uint8 sType = 0; uint8 sHours = 0;

	sType = atoi(vargs.front().c_str());
	vargs.pop_front();
	sHours = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (sHours <= 0)
		return false;

	HandleMiningDropTester(sType, sHours);
	return true;
}
#pragma endregion

COMMAND_HANDLER(CUser::HandleBossEvent)
{
	if (!isGM())
	{
		g_pMain->WriteCheatLogFile(string_format("%s Gm Komutlarını Deniyor. Karakter DC Edildi!\n", GetName().c_str()));
		return false;
		Disconnect();
	}

	std::string sNoticeMessage = string_format("Boss Event etkinliğine geri sayım başlıyor, hediyeleri toplamak üzere BOWL'da hazır ol.");
	if (!sNoticeMessage.empty())
	{
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
		g_pMain->SendAnnouncement(sNoticeMessage.c_str(), Nation::ALL);
		g_pMain->BossEventDurumu = true;
	}
	return true;
}

COMMAND_HANDLER(CUser::GmBotSystemOpen)
{
	if (!isGM())
	{
		//	g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarını Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		return false;
		Disconnect();
	}

	if (!OpenGmBotSystem)
	{
		OpenGmBotSystem = true;
		g_pMain->SendHelpDescription(this, "[Command] = Pazar Bot Sistemi Aktif");
	}
	else if (OpenGmBotSystem)
	{
		OpenGmBotSystem = false;
		g_pMain->SendHelpDescription(this, "[Command] = Pazar Bot Sistemi Pasif");
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleAllUpCommand) {
	if (!isGM())
		return false;

	this->LevelChange(83, true);
	this->SetStat(STAT_STR, 255);
	this->SetStat(STAT_STA, 255);
	this->SetStat(STAT_DEX, 255);
	this->SetStat(STAT_INT, 255);
	this->SetStat(STAT_CHA, 255);
	this->m_sPoints = 0;

	this->ResetWindows();

	SendMyInfo();

	return true;
}

#pragma region CUser::HandleOnlineItemCommand
COMMAND_HANDLER(CUser::HandleOnlineItemCommand)
{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarını Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		g_pMain->WriteCheatLogFile(string_format("%s Gm Komutlarını Deniyor. Karakter DC Edildi!\n", GetName().c_str()));
		return false;
		Disconnect();
	}

	if (vargs.size() < 3)
	{
		g_pMain->SendHelpDescription(this, "Using Sample : +online_item ItemID Count Time"); // +online_item 379021000 25 5
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE* pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		// send error message saying the item does not exist
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
		sCount = atoi(vargs.front().c_str());
	vargs.pop_front();

	uint16 sExpirationTime = 0;
	if (!vargs.empty())
		sExpirationTime = atoi(vargs.front().c_str());

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr)
			continue;

		if (pUser->isOfflineSystem()) // yeni eklendi denencek
			return false;

		if (!pUser->isInGame())
			continue;

		if (!pUser->GiveItem(nItemID, sCount, true, sExpirationTime))
			g_pMain->SendHelpDescription(this, "Error : Item could not be added");
		else
			g_pMain->SendHelpDescription(this, string_format("Item added successfully to %s!", pUser->GetName().c_str()).c_str());
	}
	return true;
}
#pragma endregion

#pragma region CUser::HandleBeefEventOpen
COMMAND_HANDLER(CUser::HandleBeefEventOpen)
{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarını Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		g_pMain->WriteCheatLogFile(string_format("%s Gm Komutlarını Deniyor. Karakter DC Edildi!\n", GetName().c_str()));
		return false;
		Disconnect();
	}

	g_pMain->m_BifrostVictory = 0;

	g_pMain->m_sBifrostTime = 0;
	g_pMain->SendFormattedResource(IDS_BEEF_ROAST_START, Nation::ALL, false);

	if (!g_pMain->m_bAttackBifrostMonument)
		g_pMain->m_bAttackBifrostMonument = true;

	g_pMain->m_sBifrostWarStart = true;
	g_pMain->m_sBifrostRemainingTime = 30 * MINUTE;
	g_pMain->SendEventRemainingTime(true, nullptr, ZONE_RONARK_LAND);
	g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
	return true;
}
#pragma endregion

#pragma region CUser::HandleBeefEventClose
COMMAND_HANDLER(CUser::HandleBeefEventClose)
{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarını Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		g_pMain->WriteCheatLogFile(string_format("%s Gm Komutlarını Deniyor. Karakter DC Edildi!\n", GetName().c_str()));
		return false;
		Disconnect();
	}

	g_pMain->m_sBifrostRemainingTime = 0;
	g_pMain->m_sBifrostWarStart = false;

	g_pMain->m_BifrostVictory = 0;
	g_pMain->m_sBifrostTime = 0;
	g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

	if (g_pMain->m_bAttackBifrostMonument)
		g_pMain->m_bAttackBifrostMonument = false;

	g_pMain->KickOutZoneUsers(ZONE_BIFROST, ZONE_RONARK_LAND);
	g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
	g_pMain->SendEventRemainingTime(true, nullptr, ZONE_RONARK_LAND);

	return true;
}
#pragma endregion

COMMAND_HANDLER(CUser::ClearInvCommand)
{
	if (!isGM())
		return false;

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		_ITEM_DATA* pItem;

		pItem = GetItem(i);

		if (pItem == nullptr)
			continue;

		memset(pItem, 0, sizeof(_ITEM_DATA));
	}

	// Inv Reload
	Packet result(WIZ_ITEM_MOVE, uint8(2));
	result << uint8(1);

	std::vector<_ITEM_DATA> inventoryItems;

	for (int i = INVENTORY_INVENT; i < INVENTORY_COSP; i++)
		inventoryItems.push_back(m_sItemArray[i]);

	for (int k = INVENTORY_INVENT; k < INVENTORY_COSP; k++)
	{
		m_sItemArray[k] = inventoryItems[k - INVENTORY_INVENT];
		result << uint32(m_sItemArray[k].nNum)
			<< uint16(m_sItemArray[k].sDuration)
			<< uint16(m_sItemArray[k].sCount)
			<< uint8(m_sItemArray[k].bFlag)
			<< uint16(m_sItemArray[k].sRemainingRentalTime)
			<< uint32(0)
			<< uint32(m_sItemArray[k].nExpirationTime);
	}

	Send(&result);

	SetUserAbility();
	SendItemWeight();

	g_pMain->SendHelpDescription(this, "Inventory Temizlendi");
	return true;
}