#include "stdafx.h"
#include "../shared/Condition.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "DBAgent.h"

extern CDBAgent g_DBAgent;

using std::string;

static std::queue<Packet *> _queue;
static bool _running = true;
static std::recursive_mutex _lock;

static Condition s_hEvent;
static Thread * s_thread;

void DatabaseThread::Startup()
{
	s_thread = new Thread(ThreadProc, (void *)1);
}

void DatabaseThread::AddRequest(Packet * pkt)
{
	_lock.lock();
	_queue.push(pkt);
	_lock.unlock();
	s_hEvent.Signal();
}

uint32 THREADCALL DatabaseThread::ThreadProc(void * lpParam)
{
	while (true)
	{
		Packet *p = nullptr;

		// Pull the next packet from the shared queue
		_lock.lock();
		if (_queue.size())
		{
			if (_queue.size() > 20)
				TRACE("Database Queue Size = %zd\n", _queue.size());

			p = _queue.front();
			_queue.pop();
		}
		_lock.unlock();

		// If there's no more packets to handle, wait until there are.
		if (p == nullptr)
		{
			// If we're shutting down, don't bother waiting for more (there are no more).
			if (!_running)
				break;

			s_hEvent.Wait();
			continue;
		}

		// References are fun =p
		Packet & pkt = *p;

		// First 2 bytes are always going to be the socket ID
		// or -1 for no user.
		int16 uid = pkt.read<int16>();

		// Attempt to lookup the user if necessary
		CUser *pUser = nullptr;
		if (uid >= 0)
		{
			pUser = g_pMain->GetUserPtr(uid);

			// Check to make sure they're still connected.
			if (pUser == nullptr)
				continue;
		}
		switch (pkt.GetOpcode())
		{
		case WIZ_LOGIN:
			if (pUser) pUser->ReqAccountLogIn(pkt);
			break;
		case WIZ_SEL_NATION:
			if (pUser) pUser->ReqSelectNation(pkt);
			break;
		case WIZ_ALLCHAR_INFO_REQ:
			if (pUser) pUser->ReqAllCharInfo(pkt);
			break;
		case WIZ_CHANGE_HAIR:
			if (pUser) pUser->ReqChangeHair(pkt);
			break;
		case WIZ_NEW_CHAR:
			if (pUser) pUser->ReqCreateNewChar(pkt);
			break;
		case WIZ_DEL_CHAR:
			if (pUser) pUser->ReqDeleteChar(pkt);
			break;
		case WIZ_SEL_CHAR:
			if (pUser) pUser->ReqSelectCharacter(pkt);
			break;
		case WIZ_CHAT:
			break;
		case WIZ_DATASAVE:
			if (pUser) pUser->ReqSaveCharacter();
			break;
		case WIZ_KNIGHTS_PROCESS:
			CKnightsManager::ReqKnightsPacket(pUser, pkt);
			break;
		case WIZ_REMOVE_CURRENT_USER:
			if (pUser) pUser->ReqRemoveCurrentUser(pkt);
			break;
		case WIZ_LOGIN_INFO:
			if (pUser) pUser->ReqSetLogInInfo(pkt);
			break;
		case WIZ_BATTLE_EVENT:
			if (pUser) pUser->BattleEventResult(pkt);
			break;
		case WIZ_SHOPPING_MALL:
			if (pUser) pUser->ReqShoppingMall(pkt);
			break;
		case WIZ_SKILLDATA:
			if (pUser) pUser->ReqSkillDataProcess(pkt);
			break;
		case WIZ_FRIEND_PROCESS:
			if (pUser) pUser->ReqFriendProcess(pkt);
			break;
		case WIZ_NAME_CHANGE:
			if (pUser) pUser->NameChangeSystem(pkt);
			break;
		case WIZ_CAPE:
			if (pUser) pUser->ReqChangeCape(pkt);
			break;
		case WIZ_LOGOUT:
			if (pUser) pUser->ReqUserLogOut();
			break;
		case WIZ_NATION_TRANSFER:
			if (pUser) pUser->ReqNationTransfer(pkt);
			break;
		case WIZ_KING:
			CKingSystem::HandleDatabaseRequest(pUser, pkt);
			break;
		case WIZ_SIEGE:
			g_pMain->HandleSiegeDatabaseRequest(pUser, pkt);
			break;
		case WIZ_ITEM_UPGRADE:
			if (pUser) pUser->ReqItemUpgrade(pkt);
			break;
		case WIZ_VIPWAREHOUSE:
			if (pUser) pUser->ReqVipStorageProcess(pkt);
			break;
		case WIZ_ZONE_CONCURRENT:
			if (pUser) pUser->ReqConcurrentProcess(pkt);
			break;
		case WIZ_PREMIUM:
			if (pUser) g_DBAgent.UpdatePremiumServiceUser(pUser);
			break;
		case WIZ_DB_DAILY_OP:
			if (pUser) pUser->DailyOpProcess(pkt);
			break;
		case WIZ_DB_UPDATE_RANKING:
			g_DBAgent.UpdateRanks();
			sleep(1 * SECOND);
			g_pMain->LoadUserRankings();
			break;
		case WIZ_RESET_LOYALTY:
			g_DBAgent.ResetLoyaltyMonthly();
			sleep(1 * SECOND);
			break;
		case WIZ_DB_LOAD_RANKING:
			g_pMain->LoadUserRankings();
			break;
		case WIZ_DB_NPOINTS:
			if (pUser) pUser->ReqHandleNPoints(pkt);
			break;
		case WIZ_AUTHORITY_CHANGE:
			{
				std::string strUserID, strGM, banReason;
				uint16 authority;
				uint8 banType = 0;
				uint32 sPeriod = 9999;
				pkt >> strGM >> strUserID >> authority >> banType >> sPeriod >> banReason;
				if(pUser)
					g_DBAgent.UpdateUserAuthority(strGM, strUserID, authority, sPeriod, banType, banReason, pUser);
				else
					g_DBAgent.UpdateUserAuthority(strGM,strUserID, authority, sPeriod, banType, banReason);
			}
			break;

		case WIZ_CLAN_PREMIUM:
			if (pUser) g_DBAgent.UpdateClanPremiumServiceUser(pUser);
			break;
		}

		// Free the packet.
		delete p;
	}

	TRACE("[Thread %d] Exiting...\n", lpParam);
	return 0;
}

void CUser::ReqItemUpgrade(Packet & pkt)
{
	uint8 bOpcode = pkt.read<uint8>();

	switch (ItemUpgradeOpcodes(bOpcode))
	{
	case ITEM_SEAL:
		ReqSealItem(pkt);
		break;
	case ITEM_CHARACTER_SEAL:
		ReqCharacterSealProcess(pkt);
		break;
	default:
		printf("ReqItemUpgrade unhandled database packet %d \n", bOpcode);
		TRACE("ReqItemUpgrade unhandled database packet %d \n", bOpcode);
		break;
	}
}

void CUser::ReqRemoveCurrentUser(Packet & pkt)
{
	string m_strAccountID;

	pkt >> m_strAccountID;
	// if there was no error deleting to CURRENTUSER...
	if (!g_DBAgent.RemoveCurrentUser(m_strAccountID))
		printf("An error has occurred during deletion of user from CURRENTUSER table.\n");
}

void CUser::ReqAccountLogIn(Packet & pkt)
{
	string strPasswd;
	pkt >> strPasswd;

	int8 nation = g_DBAgent.AccountLogin(m_strAccountID, strPasswd);

	if (nation >= 0 && nation < 3)
		g_pMain->AddAccountName(this);
	else
		m_strAccountID.clear();

	Packet result(WIZ_LOGIN);
	result << nation << uint32(0);
	Send(&result);
}

void CUser::ReqSelectNation(Packet & pkt)
{
	Packet result(WIZ_SEL_NATION);
	uint8 bNation = pkt.read<uint8>(), bResult;

	bResult = g_DBAgent.NationSelect(m_strAccountID, bNation) ? bNation : 0;
	result << bResult;
	Send(&result);
}

void CUser::ReqAllCharInfo(Packet & pkt)
{
	uint8 oPcode;
	pkt >> oPcode;
	Packet result(WIZ_ALLCHAR_INFO_REQ);
	switch (oPcode)
	{
	case AllCharInfoOpcode:
	{
		string strCharID1, strCharID2, strCharID3, strCharID4;
		result << uint8(oPcode) << uint8(1);

		g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4);
		g_DBAgent.LoadCharInfo(strCharID1, result);
		g_DBAgent.LoadCharInfo(strCharID2, result);
		g_DBAgent.LoadCharInfo(strCharID3, result);
		g_DBAgent.LoadCharInfo(strCharID4, result);
		Send(&result);

	}
	break;
	case AlreadyCharName:
	{
		string sOldCharID, sNewCharID;
		pkt >> sOldCharID >> sNewCharID;

		ServerUniteSelectingCharNameErrorOpcode NameChangeResult;
		NameChangeResult = g_DBAgent.UpdateSelectingCharacterName(GetAccountName(), sOldCharID, sNewCharID);
		result << uint8(AlreadyCharName);

		switch (NameChangeResult)
		{
		case 0:
			result << uint8(CannotCharacterID);
			Send(&result);
			break;
		case 1:
			result << uint8(CharacterIDSuccesfull);
			Send(&result);
		default:
			result << uint8(CannotCharacterID);
			Send(&result);
			break;
		}
	}
	break;
	default:
		break;
	}
}

void CUser::ReqChangeHair(Packet & pkt)
{
	Packet result(WIZ_CHANGE_HAIR);
	string strUserID;
	uint32 nHair;
	uint8 bOpcode, bFace;
	pkt.SByte();
	pkt >> bOpcode >> strUserID >> bFace >> nHair;

	if(bOpcode == 0)
	{
		int8 bResult = g_DBAgent.ChangeHair(m_strAccountID, strUserID, bOpcode, bFace, nHair);

		result << uint8(bResult);
		Send(&result);
	}
	else if(bOpcode == 1)
	{
		if(!isInGame() 
			|| !CheckExistItem(ITEM_MAKE_OVER))
		{
			result << uint8(1);
			Send(&result);
			return;
		}

		int8 bResult = g_DBAgent.ChangeHair(m_strAccountID, strUserID, bOpcode, bFace, nHair);
		if(bResult == 0)
		{
			RobItem(ITEM_MAKE_OVER);

			m_bFace = bFace;
			m_nHair = nHair;
		}

		result << bResult;
		Send(&result);
	}
}

void CUser::ReqCreateNewChar(Packet & pkt)
{
	string strCharID;
	uint32 nHair;
	uint16 sClass;
	uint8 bCharIndex, bRace, bFace, bStr, bSta, bDex, bInt, bCha;
	pkt >> bCharIndex >> strCharID >> bRace >> sClass >> bFace >> nHair >> bStr >> bSta >> bDex >> bInt >> bCha;

	uint8 sonuc = g_DBAgent.CreateNewChar(m_strAccountID, bCharIndex, strCharID, bRace, sClass, nHair, bFace, bStr, bSta, bDex, bInt, bCha);
	Packet result(WIZ_NEW_CHAR);
	result << sonuc;
	if (sonuc == 0)
	{
		g_DBAgent.LoadNewCharSet(strCharID, sClass);
		g_DBAgent.LoadNewCharValue(strCharID, sClass);
	}
	Send(&result);
}

void CUser::ReqDeleteChar(Packet & pkt)
{
	string strCharID, strSocNo;
	uint8 bCharIndex;
	pkt >> bCharIndex >> strCharID >> strSocNo;

	Packet result(WIZ_DEL_CHAR);
	int8 retCode = g_DBAgent.DeleteChar(m_strAccountID, bCharIndex, strCharID, strSocNo);
	result << retCode << uint8(retCode ? bCharIndex : -1);
	Send(&result);
}

void CUser::ReqSelectCharacter(Packet & pkt)
{
	Packet result(WIZ_SEL_CHAR);
	uint8 bInit;
	string strCharID;

	pkt >> strCharID >> bInit;
	if (m_strAccountID.empty() 
		|| strCharID.empty()
		|| !g_DBAgent.LoadUserData(m_strAccountID, strCharID, this)
		|| !g_DBAgent.LoadWarehouseData(m_strAccountID, this)
		|| !g_DBAgent.LoadVipWarehouseData(m_strAccountID, this)
		|| !g_DBAgent.LoadPremiumServiceUser(m_strAccountID, this)
		|| !g_DBAgent.LoadSavedMagic(this)
		|| !g_DBAgent.LoadGenieData(strCharID, this)
		|| !g_DBAgent.LoadQuestData(strCharID, this)
		|| !g_DBAgent.LoadAchieveData(this)
		|| !g_DBAgent.LoadUserSealExpData(this)
		|| !g_DBAgent.LoadUserDrakiTowerData(this)
		|| !g_DBAgent.LoadUserReturnData(this))
	{
		result << uint8(CannotSelectingCharacter);
	}
	else
	{
		result << uint8(g_DBAgent.SelectCharacterChecking(m_strAccountID, GetName())) << bInit;
	}

	SelectCharacter(result); 
}

void CUser::ReqShoppingMall(Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case STORE_CLOSE:
		ReqLoadWebItemMall();
		break;
	case STORE_LETTER:
		ReqLetterSystem(pkt);
		break;
	}
}

void CUser::ReqSkillDataProcess(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();
	if (opcode == SKILL_DATA_LOAD)
		ReqSkillDataLoad(pkt);
	else if (opcode == SKILL_DATA_SAVE)
		ReqSkillDataSave(pkt);
}

void CUser::ReqSkillDataLoad(Packet & pkt)
{
	Packet result(WIZ_SKILLDATA, uint8(SKILL_DATA_LOAD));
	if (!g_DBAgent.LoadSkillShortcut(result, this))
		result << uint16(0);

	Send(&result);
}

void CUser::ReqSkillDataSave(Packet & pkt)
{
	// Initialize our buffer (not all skills are likely to be saved, we need to store the entire 260 bytes).
	char buff[320];
	memset(buff, 0, sizeof(buff));
	short sCount;

	// Read in our skill count
	pkt >> sCount;

	// Make sure we're not going to copy too much (each skill is 1 uint32).
	if ((sCount * sizeof(uint32)) > sizeof(buff))
		return;

	// Copy the skill data directly in from where we left off reading in the packet buffer
	memcpy(buff, (char *)(pkt.contents() + pkt.rpos()), sCount * sizeof(uint32));

	// Finally, save the skill data.
	g_DBAgent.SaveSkillShortcut(sCount, buff, this);
}

void CUser::ReqFriendProcess(Packet & pkt)
{
	uint8 opcode = 0;
	pkt >> opcode;

	switch (opcode)
	{
	case FRIEND_REQUEST:
		ReqRequestFriendList(pkt);
		break;
	case FRIEND_ADD:
		ReqAddFriend(pkt);
		break;
	case FRIEND_REMOVE:
		ReqRemoveFriend(pkt);
		break;
	default:
		printf("Friend Process unhandled packet %d", opcode);
		TRACE("Friend Process unhandled packet %d", opcode);
		break;
	}
}

void CUser::ReqRequestFriendList(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	std::vector<string> friendList;

	g_DBAgent.RequestFriendList(friendList, this);

	result << uint16(friendList.size());
	foreach (itr, friendList)
		result << (*itr);

	FriendReport(result);
}

void CUser::ReqAddFriend(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	string strCharID;
	int16 tid;

	pkt.SByte();
	pkt >> tid >> strCharID;

	FriendAddResult resultCode = g_DBAgent.AddFriend(GetSocketID(), tid);
	result.SByte();
	result << tid << uint8(resultCode) << strCharID;

	RecvFriendModify(result, FRIEND_ADD);
}

void CUser::ReqRemoveFriend(Packet & pkt)
{
	Packet result(WIZ_FRIEND_PROCESS);
	string strCharID;

	pkt.SByte();
	pkt >> strCharID;

	FriendRemoveResult resultCode = g_DBAgent.RemoveFriend(strCharID, this);
	result.SByte();
	result << uint8(resultCode) << strCharID;

	RecvFriendModify(result, FRIEND_REMOVE);
}

/**
* @brief	Handles clan cape update requests.
*
* @param	pkt	The packet.
*/
void CUser::ReqChangeCape(Packet & pkt)
{
	uint16 sClanID, sCapeID;
	uint8 r, g, b, type;
	pkt >> type >> sClanID >> sCapeID >> r >> g >> b;

	switch (type)
	{
	case 1:
	{
		_KNIGHTS_CASTELLAN_CAPE * pCastellanInfo = g_pMain->m_KnightCastellanCapeArray.GetData(sClanID);
		if (pCastellanInfo != nullptr)
			g_DBAgent.KnightsCastellanCapeUpdate(sClanID, sCapeID, r, g, b, pCastellanInfo->sRemainingTime, pCastellanInfo->sActive);
	}
	break;
	case 2:
		g_DBAgent.KnightsCapeUpdate(sClanID, sCapeID, r, g, b);
		break;
	}
}

void CUser::ReqUserLogOut()
{
	if (m_bLevel == 0)
	{
		TRACE("### ReqUserLogOut - Level is Zero : bRoom=%d, bNation=%d, bZone=%d ####\n", GetEventRoom(), GetNation(), GetZoneID());
		m_bCharacterDataLoaded = false;
		m_deleted = false;
		m_bIsLoggingOut = false;
		return;
	}

	BDWUserHasObtainedGameLoqOut();
	KnightRoyaleSurvivorLogOut();
	TowerExitsFunciton();

	if (m_sFirstUsingGenie <= 0)
		m_GenieTime = 0;

	PlayerKillingRemovePlayerRank();
	BorderDefenceRemovePlayerRank();
	ChaosExpansionRemovePlayerRank();
	RobChaosSkillItems();

	g_pMain->KillNpc(GetSocketID(), GetZoneID(), this);

	if (g_pMain->pTempleEvent.ActiveEvent != -1)
	{
		TempleDisbandEvent(GetJoinedEvent());

		if (!g_pMain->pTempleEvent.isActive)
			TempleDisbandEvent(g_pMain->pTempleEvent.ActiveEvent);
	}

	if (isQuestEventUser())
		MonsterStoneExitProcess();

	if(isSoccerEventUser())
	{
		isEventSoccerEnd();

		if (g_pMain->m_TempleSoccerEventUserArray.GetData(GetSocketID()) != nullptr)
			g_pMain->m_TempleSoccerEventUserArray.DeleteData(GetSocketID());
	}

	g_DBAgent.UpdateUser(GetName(), UPDATE_LOGOUT, this);
	g_DBAgent.UpdateQuestData(GetName(), this);
	g_DBAgent.UpdateGenieData(GetName(), this);
	g_DBAgent.UpdatePremiumServiceUser(this);
	g_DBAgent.UpdateAchieveData(this);
	g_DBAgent.UpdateUserSealExpData(this);
	g_DBAgent.UpdateUserReturnData(GetName(), this);
	g_DBAgent.UpdateWarehouseData(GetAccountName(), UPDATE_LOGOUT, this);
	g_DBAgent.UpdateVipWarehouseData(GetAccountName(), UPDATE_LOGOUT, this);
	g_DBAgent.UpdateSavedMagic(this);

	PartyBBSUserLoqOut();

	if (m_bLogout != 2)	// zone change logout
		g_DBAgent.AccountLogout(GetAccountName());

	// this session can be used again.
	m_bCharacterDataLoaded = false;
	m_deleted = false;
	m_bIsLoggingOut = false;
}

void CUser::ReqSaveCharacter()
{
	g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);
	g_DBAgent.UpdateQuestData(GetName(), this);
	g_DBAgent.UpdateGenieData(GetName(), this);
	g_DBAgent.UpdatePremiumServiceUser(this);
	g_DBAgent.UpdateAchieveData(this);
	g_DBAgent.UpdateUserSealExpData(this);
	g_DBAgent.UpdateUserReturnData(GetName(), this);
	g_DBAgent.UpdateWarehouseData(GetAccountName(), UPDATE_PACKET_SAVE, this);
	g_DBAgent.UpdateVipWarehouseData(GetAccountName(), UPDATE_PACKET_SAVE, this);
	g_DBAgent.UpdateSavedMagic(this);
}

void CKnightsManager::ReqKnightsAllMember(CUser *pUser, Packet & pkt)
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MEMBER_REQ));
	int nOffset;
	uint16 sClanID, sCount;

	pkt >> sClanID;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	result << uint8(1);
	nOffset = (int)result.wpos(); // store offset
	result	<< uint16(0) // placeholder for packet length 
		<< uint16(0); // placeholder for user count

	sCount = g_DBAgent.LoadKnightsAllMembers(sClanID, result);
	if (sCount > MAX_CLAN_USERS)
		return;

	pkt.put(nOffset, uint16(result.size() - 3));
	pkt.put(nOffset + 2, sCount);

	pUser->Send(&result);
}

void CUser::ReqSetLogInInfo(Packet & pkt)
{
	DateTime time;
	string strCharID, strServerIP, strClientIP;
	uint16 sServerNo;
	uint8 bInit;

	pkt >> strCharID >> strServerIP >> sServerNo >> strClientIP >> bInit;
	// if there was an error inserting to CURRENTUSER...
	if (!g_DBAgent.SetLogInInfo(m_strAccountID, strCharID, strServerIP, sServerNo, strClientIP, bInit))
		Disconnect();

	g_pMain->WriteCharacterIpLogs(string_format("[IpLogs Time - %d:%d:%d] CharacterName = %s : CharacterIP = %s (ServerIP = %s)\n", time.GetHour(), time.GetMinute(), time.GetSecond(), strCharID.c_str(), strClientIP.c_str(), strServerIP.c_str()));
}

void CUser::BattleEventResult(Packet & pkt)
{
	string strMaxUserName;
	uint8 bType, bNation;

	pkt >> bType >> bNation >> strMaxUserName;
	g_DBAgent.UpdateBattleEvent(strMaxUserName, bNation);
}

/**
* @brief	Handles database requests for the King system.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest(CUser * pUser, Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case KING_ELECTION:
		HandleDatabaseRequest_Election(pUser, pkt);
		break;

	case KING_TAX:
		HandleDatabaseRequest_Tax(pUser, pkt);
		break;

	case KING_EVENT:
		HandleDatabaseRequest_Event(pUser, pkt);
		break;

	case KING_IMPEACHMENT:
		break;

	case KING_NPC:
		break;

	case KING_NATION_INTRO:
		HandleDatabaseRequest_NationIntro(pUser, pkt);
		break;
	}
}


/**
* @brief	Handles database requests for King commands.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_NationIntro(CUser * pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	std::string strKingName, strKingNotice;
	uint8 byNation = 0;
	uint32 ServerNo = 0;
	pkt >> ServerNo >> byNation >> strKingName >> strKingNotice;
	g_DBAgent.UpdateNationIntro(ServerNo, byNation, strKingName, strKingNotice);
}

/**
* @brief	Handles database requests for the election system.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_Election(CUser * pUser, Packet & pkt)
{
	uint8 opcode;
	pkt >> opcode;

	switch (opcode)
	{
		// Special king system/election database requests
	case KING_ELECTION:
		{
			uint8 byNation, byType;
			pkt >> opcode >> byNation >> byType;
			switch (opcode)
			{
			case KING_ELECTION_UPDATE_STATUS: // 7
				g_DBAgent.UpdateElectionStatus(byType, byNation);
				break;

			case KING_ELECTION_UPDATE_LIST: // 6
				{
					bool bDelete;
					uint16 sKnights;
					uint32 nVotes = 0;
					string strNominee;

					pkt >> bDelete >> sKnights >> strNominee;
					g_DBAgent.UpdateElectionList(bDelete ? 2 : 1, byType, byNation, sKnights, nVotes, strNominee);
				} break;
			case KING_ELECTION_VOTE: 
				{
					Packet result(WIZ_KING, uint8(KING_ELECTION));
					result << uint8(KING_ELECTION_POLL) << uint8(2);
					string strVoterAccountID, strVoterUserID, strNominee;
					int16 bResult = -3;
					pkt >> strVoterAccountID >> strVoterUserID >> strNominee;
					bResult = g_DBAgent.UpdateElectionVoteList(byNation, strVoterAccountID, strVoterUserID, strNominee);
					result << uint16(bResult);
					CUser* pVoter = g_pMain->GetUserPtr(strVoterUserID, TYPE_CHARACTER);
					if(pVoter != nullptr && pVoter->isInGame())
						pVoter->Send(&result);
				} return;
			case KING_ELECTION_GET_VOTE_RESULTS: 
				{
					g_DBAgent.GetElectionResults(byNation);
				} return;
			}
		} break;
	case KING_ELECTION_NOMINATE:
		{
			if (pUser == nullptr)
				return;

			Packet result(WIZ_KING, uint8(KING_ELECTION));
			std::string strNominee;
			int16 resultCode;
			pkt >> strNominee;
			resultCode = g_DBAgent.UpdateCandidacyRecommend(pUser->m_strUserID, strNominee, pUser->GetNation());

			// On success, we need to sync the local list.
			if (resultCode >= 0)
			{
				CKingSystem * pData = g_pMain->m_KingSystemArray.GetData(pUser->GetNation());
				if (pData == nullptr)
					return;

				pData->InsertNominee(pUser->m_strUserID, strNominee, resultCode);
				result << opcode << int16(1);
				pUser->Send(&result);
				return;
			}
			result << opcode << resultCode;
			pUser->Send(&result);
		} break;

	case KING_ELECTION_NOTICE_BOARD:
		{
			pkt >> opcode;
			if (pUser == nullptr)
				return;

			if (opcode == KING_CANDIDACY_BOARD_WRITE)
			{
				string strNotice;
				pkt >> strNotice;
				g_DBAgent.UpdateCandidacyNoticeBoard(pUser->m_strUserID, pUser->GetNation(), strNotice);
			}
		} break;
	}
}

/**
* @brief	Handles database requests for King commands.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_Event(CUser * pUser, Packet & pkt)
{
	uint8 opcode, byNation;
	pkt >> opcode >> byNation;

	switch (opcode)
	{
	case KING_EVENT_NOAH:
	case KING_EVENT_EXP:
		{
			uint8 byAmount, byDay, byHour, byMinute;
			uint16 sDuration;
			uint32 nCoins;
			pkt >> byAmount >> byDay >> byHour >> byMinute >> sDuration >> nCoins;

			g_DBAgent.UpdateNoahOrExpEvent(opcode, byNation, byAmount, byDay, byHour, byMinute, sDuration, nCoins);
		} break;

	case KING_EVENT_PRIZE:
		{
			uint32 nCoins;
			string strUserID;
			pkt >> nCoins >> strUserID;

			g_DBAgent.InsertPrizeEvent(opcode, byNation, nCoins, strUserID);
		} break;
	}
}

/**
* @brief	Handles database requests for King commands.
*
* @param	pUser	The user making the request, if applicable. 
* 					nullptr if not.
* @param	pkt  	The packet.
*/
void CKingSystem::HandleDatabaseRequest_Tax(CUser * pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	uint8 opcode = 0, byNation = 0, TerritoryTariff = 0;
	uint16 dummy;
	uint32 TerritoryTax = 0;
	pkt >> opcode;

	switch(opcode)
	{
	case 2: // Collec Kings Fund
		pkt >> TerritoryTax >> byNation;
		break;
	case 4: // Update tax
		{
			pkt >> dummy >> TerritoryTariff >> byNation;
		}
		break;
	}

	g_DBAgent.InsertTaxEvent(opcode, TerritoryTariff, byNation, TerritoryTax);
}

#pragma region CUser::ReqConcurrentProcess(Packet & pkt)
void CUser::ReqConcurrentProcess(Packet & pkt)
{
	uint32 serverNo, count;
	pkt >> serverNo >> count;
	g_DBAgent.UpdateConCurrentUserCount(serverNo, 1, count);
}
#pragma endregion

#pragma region CUser::DailyOpProcess(Packet & pkt)
void CUser::DailyOpProcess(Packet & pkt)
{
	uint8 process = pkt.read<uint8>();
	if (process == 1)
	{
		uint8 type;
		int32 time;
		std::string name;
		pkt >> name >> type >> time;
		g_DBAgent.UpdateUserDailyOp(name, type, time);
	}
	else if (process == 2)
	{
		_USER_DAILY_OP pData;
		pkt >> pData.strUserId;
		pkt >> pData.ChaosMapTime;
		pkt >> pData.UserRankRewardTime;
		pkt >> pData.PersonalRankRewardTime;
		pkt >> pData.KingWingTime;
		pkt >> pData.WarderKillerTime1;
		pkt >> pData.WarderKillerTime2;
		pkt >> pData.KeeperKillerTime;
		pkt >> pData.UserLoyaltyWingRewardTime;
		g_DBAgent.InsertUserDailyOp(&pData);
	}
}
#pragma endregion

void DatabaseThread::Shutdown()
{
	_running = false;

	// Wake them up in case they're sleeping.
	s_hEvent.Broadcast();

	if (s_thread)
	{
		s_thread->waitForExit();
		delete s_thread;
	}

	_lock.lock();
	while (_queue.size())
	{
		Packet *p = _queue.front();
		_queue.pop();
		delete p;
	}
	_lock.unlock();
}
