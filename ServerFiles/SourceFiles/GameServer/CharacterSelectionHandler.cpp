#include "stdafx.h"
#include "Map.h"
#include "DBAgent.h"
#include "KingSystem.h"
#include "../GameServer/GameDefine.h"

#define ARRANGE_LINE	800444000

using std::string;

void CUser::SelNationToAgent(Packet & pkt)
{
	Packet result(WIZ_SEL_NATION);
	uint8 nation = pkt.read<uint8>();
	if (nation != KARUS && nation != ELMORAD)
	{
		result << uint8(0);
		Send(&result);
		return;
	}

	result << nation; 
	g_pMain->AddDatabaseRequest(result, this);
}

#pragma region
void CUser::AllCharInfo(Packet &pkt)
{
	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
	case 1:
		AllCharInfoToAgent();
		break;
	case 2:
		AllCharNameChangeInfo(pkt);
		break;
	case 3:
		AllCharLocationSendToAgent(pkt);
		break;
	case 4:
		AllCharLocationRecvToAgent(pkt);
		break;
	default:
		TRACE("Unhandle AllCharInfo Opecode %d", opcode);
		printf("Unhandle AllCharInfo Opecode %d", opcode);
		break;
	}
}
#pragma endregion

#pragma region
void CUser::AllCharInfoToAgent()
{
	Packet result(WIZ_ALLCHAR_INFO_REQ, uint8(AllCharInfoOpcode));
	g_pMain->AddDatabaseRequest(result, this);
}
#pragma endregion

#pragma region
void CUser::AllCharNameChangeInfo(Packet & pkt)
{
	uint16 CharRanking;
	string sOldCharID, sNewCharID;
	pkt >> CharRanking >> sOldCharID >> sNewCharID;

	Packet result(WIZ_ALLCHAR_INFO_REQ, uint8(AlreadyCharName));
	if (sNewCharID.empty()
		|| sNewCharID.length() > MAX_ID_SIZE
		|| sOldCharID.empty())
	{
		result << uint8(0);
		Send(&result);
		return;
	}

	result << sOldCharID << sNewCharID;
	g_pMain->AddDatabaseRequest(result, this);
}
#pragma endregion

#pragma region
void CUser::AllCharLocationSendToAgent(Packet & pkt)
{
	Packet result(WIZ_ALLCHAR_INFO_REQ, uint8(ArrangeOpen));
	string strCharID1, strCharID2, strCharID3, strCharID4;

	if (!g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4))
		return;

	if (!isInGame()
		|| isMining()
		|| isFishing()
		|| isMerchanting()
		|| isBuyingMerchant()
		|| isSellingMerchant()
		|| isStoreOpen()
		|| isTrading())
		return;

	if (strCharID1.length() == 0 && strCharID2.length() == 0 && strCharID3.length() == 0 && strCharID4.length() == 0)
		return;

	if (strCharID1.length() != 0)
		result << strCharID1;
	else
		result << "";
	if (strCharID2.length() != 0)
		result << strCharID2;
	else
		result << "";
	if (strCharID3.length() != 0)
		result << strCharID3;
	else
		result << "";
	if (strCharID4.length() != 0)
		result << strCharID4;
	else
		result << "";

	Send(&result);
}
#pragma endregion

#pragma region
void CUser::AllCharLocationRecvToAgent(Packet & pkt)
{
	/*
	1 = Success.
	1 = Failed.
	3 = Required item on iventory is not available.
	4 = 2 locations have set on one character.
	*/
	enum ResultOpCodes
	{
		Success = 1,
		Failed = 2,
		ItemIsNot = 3,
		TwoLocations = 4,
	};

	Packet result(WIZ_ALLCHAR_INFO_REQ, uint8(ArrangeRecvSend));
	uint8 CharRankingI, CharRankingII, CharRankingIII, CharRankingIIII;
	string strCharID1, strCharID2, strCharID3, strCharID4;
	string ID1, ID2, ID3, ID4;
	pkt >> CharRankingI 
		>> CharRankingII 
		>> CharRankingIII 
		>> CharRankingIIII;

	if (CharRankingI == CharRankingII 
		|| CharRankingI == CharRankingIII
		|| CharRankingI == CharRankingIIII)
	{
		result << uint8(TwoLocations);
		Send(&result);
		return;
	}

	if (CharRankingII == CharRankingI 
		|| CharRankingII == CharRankingIII 
		|| CharRankingII == CharRankingIIII)
	{
		result << uint8(TwoLocations);
		Send(&result);
		return;
	}

	if (CharRankingIII == CharRankingI 
		|| CharRankingIII == CharRankingII 
		|| CharRankingIII == CharRankingIIII)
	{
		result << uint8(TwoLocations);
		Send(&result);
		return;
	}

	if (CharRankingIIII == CharRankingI 
		|| CharRankingIIII == CharRankingII 
		|| CharRankingIIII == CharRankingIII)
	{
		result << uint8(TwoLocations);
		Send(&result);
		return;
	}

	if (!g_DBAgent.GetAllCharID(m_strAccountID
		, strCharID1
		, strCharID2
		, strCharID3
		, strCharID4))
	{
		result << uint8(Failed);
		Send(&result);
		return;
	}

	if (!isInGame()
		|| isTrading() 
		|| isMerchanting() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isStoreOpen() 
		|| isMining() 
		|| isDead() 
		|| isFishing())
	{
		result << uint8(Failed);
		Send(&result);
		return;
	}

	if (strCharID1.length() == 0 
		&& strCharID2.length() == 0 
		&& strCharID3.length() == 0 
		&& strCharID4.length() == 0)
	{
		result << uint8(Failed);
		Send(&result);
		return;
	}

	if (!CheckExistItem(ARRANGE_LINE, 1))
	{
		result << uint8(ItemIsNot);
		Send(&result);
		return;
	}

	if (strCharID1.length() != 0)
	{
		switch (CharRankingI)
		{
		case 1:
			ID1 = strCharID1;
			break;
		case 2:
			ID2 = strCharID1;
			break;
		case 3:
			ID3 = strCharID1;
			break;
		case 4:
			ID4 = strCharID1;
			break;
		}
	}
	else
	{
		switch (CharRankingI)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			ID1 = "";
			break;
		}
	}
	if (strCharID2.length() != 0)
	{
		switch (CharRankingII)
		{
		case 1:
			ID1 = strCharID2;
			break;
		case 2:
			ID2 = strCharID2;
			break;
		case 3:
			ID3 = strCharID2;
			break;
		case 4:
			ID3 = strCharID2;
			break;
		}
	}
	else
	{
		switch (CharRankingII)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			ID2 = "";
			break;
		}
	}
	if (strCharID3.length() != 0)
	{
		switch (CharRankingIII)
		{
		case 1:
			ID1 = strCharID3;
			break;
		case 2:
			ID2 = strCharID3;
			break;
		case 3:
			ID3 = strCharID3;
			break;
		case 4:
			ID4 = strCharID3;
			break;
		}
	}
	else
	{
		switch (CharRankingIII)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			ID3 = "";
			break;
		}
	}

	if (strCharID4.length() != 0)
	{
		switch (CharRankingIIII)
		{
		case 1:
			ID1 = strCharID4;
			break;
		case 2:
			ID2 = strCharID4;
			break;
		case 3:
			ID3 = strCharID4;
			break;
		case 4:
			ID4 = strCharID4;
			break;
		}
	}
	else
	{
		switch (CharRankingIIII)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			ID4 = "";
			break;
		}
	}

	if (!g_DBAgent.SetAllCharID(m_strAccountID
		, ID1
		, ID2
		, ID3
		, ID4))
	{
		result << uint8(Failed);
		Send(&result);
		return;
	}

	if (!RobItem(ARRANGE_LINE, 1))
	{
		result << uint8(Failed);
		Send(&result);
		return;
	}

	result << uint8(Succes);
	Send(&result);
}
#pragma endregion

void CUser::ChangeHair(Packet & pkt)
{
	std::string strUserID;
	uint32 nHair;
	uint8 bOpcode, bFace;
	Packet result(WIZ_CHANGE_HAIR);

	// The opcode:
	// 0 seems to be an in-game implementation for converting from old -> new hair data
	// 2 seems to be used with the hair change item(?).

	// Another note: there's 4 bytes at the end of the packet data that I can't account for (maybe a[nother] checksum?)

	pkt.SByte();
	pkt >> bOpcode >> strUserID >> bFace >> nHair;
	result.SByte();
	if(bOpcode == 1)
		result << bOpcode << m_strUserID << bFace << nHair;
	else
		result << bOpcode << strUserID << bFace << nHair;

	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::NewCharToAgent(Packet & pkt)
{
	Packet result(WIZ_NEW_CHAR);
	uint32 nHair;
	uint16 sClass;
	uint8 bCharIndex, bRace, bFace, str, sta, dex, intel, cha, errorCode = 0;
	std::string strUserID;

	pkt >> bCharIndex >> strUserID >> bRace >> sClass >> bFace >> nHair
		>> str >> sta >> dex >> intel >> cha;

	_CLASS_COEFFICIENT* p_TableCoefficient = g_pMain->m_CoefficientArray.GetData(sClass);

	if (bCharIndex > 3)
		errorCode = NEWCHAR_NO_MORE;
	else if (p_TableCoefficient == nullptr
		|| (str + sta + dex + intel + cha) > 300
		|| !NewCharValid(bRace, sClass))
		errorCode = NEWCHAR_INVALID_DETAILS;
	else if (!NewCharClassVaid(sClass))
		errorCode = NEWCHAR_INVALID_CLASS;
	else if (!NewCharRaceVaid(bRace))
		errorCode = NEWCHAR_INVALID_RACE;
	else if ((str + sta + dex + intel + cha) < 300)
		errorCode = NEWCHAR_POINTS_REMAINING;
	else if (str < 50 || sta < 50 || dex < 50 || intel < 50 || cha < 50)
		errorCode = NEWCHAR_STAT_TOO_LOW;
	else if (strUserID.empty() || strUserID.length() > MAX_ID_SIZE)
		errorCode = NEWCHAR_INVALID_NAME;

	//if (bRace == 6 || bRace == 14)
	//errorCode = NEWCHAR_NOT_SUPPORTED_RACE;

	if (errorCode != 0)
	{
		result << errorCode;
		Send(&result);
		return;
	}

	result << bCharIndex
		<< strUserID << bRace << sClass << bFace << nHair
		<< str << sta << dex << intel << cha;
	g_pMain->AddDatabaseRequest(result, this);
}

/**
* @brief	Checks whether incoming create character request details
*			are correct.
*/
bool CUser::NewCharRaceVaid(uint16 bRace)
{
	bool Result = false;

	switch (bRace)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 11:
	case 12:
	case 13:
	case 14:
		Result = true;
		break;
	default:
		Result = false;
		break;
	}
	return Result;
}

/**
* @brief	Checks whether incoming create character request details
*			are correct.
*/
bool CUser::NewCharClassVaid(uint16 bClass)
{
	bool Result = false;

	switch (bClass)
	{
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:
	case 107:
	case 109:
	case 111:
	case 106:
	case 108:
	case 110:
	case 112:
	case 201:
	case 202:
	case 203:
	case 204:
	case 205:
	case 207:
	case 209:
	case 211:
	case 206:
	case 208:
	case 210:
	case 212:
	case 113:
	case 114:
	case 115:
	case 213:
	case 214:
	case 215:
		Result = true;
		break;
	default:
		Result = false;
		break;
	}

	return Result;
}

/**
* @brief	Checks whether incoming create character request details
*			are correct.
*/
bool CUser::NewCharValid(uint8 bRace, uint16 bClass)
{
	bool Result = false;
	switch (bRace)
	{
	case 1:
		if (bClass == 101)
			Result = true;
		break;
	case 2:
		if (bClass == 102 || bClass == 104)
			Result = true;
		break;
	case 3:
		if (bClass == 103)
			Result = true;
		break;
	case 4:
		if (bClass == 103 || bClass == 104)
			Result = true;
		break;
	case 6:
		if (bClass == 113)
			Result = true;
		break;
	case 11:
		if (bClass == 201)
			Result = true;
		break;
	case 12:
	case 13:
		if (bClass == 201
			|| bClass == 202
			|| bClass == 203
			|| bClass == 204)
			Result = true;
		break;
	case 14:
		if (bClass == 213)
			Result = true;
		break;
	default:
		Result = false;
		break;
	}
	return Result;
#if 0
	bool Result = false;
	switch (bRace)
	{
	case 1:
		if (bClass == 101 || bClass == 105 || bClass == 106)
			Result = true;
		break;
	case 2:
		if (bClass == 102 || bClass == 104 || bClass == 107
			|| bClass == 111 || bClass == 108 || bClass == 112)
			Result = true;
		break;
	case 3:
		if (bClass == 103 || bClass == 109 || bClass == 110)
			Result = true;
		break;
	case 4:
		if (bClass == 103 || bClass == 104 || bClass == 109
			|| bClass == 111 || bClass == 110 || bClass == 112)
			Result = true;
		break;
	case 6:
		if (bClass == 113 || bClass == 114 || bClass == 115)
			Result true;
		break;
	case 14:
		if (bClass == 213 || bClass == 214 || bClass == 215)
			Result true;
		break;
	case 11:
		if (bClass == 201 || bClass == 205 || bClass == 206)
			Result = true;
		break;
	case 12:
	case 13:
		if (bClass == 201 || bClass == 205 || bClass == 206
			|| bClass == 202 || bClass == 207 || bClass == 208
			|| bClass == 203 || bClass == 209 || bClass == 210
			|| bClass == 204 || bClass == 211 || bClass == 212)
			Result = true;
		break;
	default:
		Result = false;
		break;
	}
	return Result;
#endif
}

void CUser::DelCharToAgent(Packet & pkt)
{
	Packet result(WIZ_DEL_CHAR);
	std::string strUserID, strSocNo;
	uint8 bCharIndex;
	pkt >> bCharIndex >> strUserID >> strSocNo; 

	if (bCharIndex > 2
		|| strUserID.empty() || strUserID.size() > MAX_ID_SIZE
		|| strSocNo.empty() || strSocNo.size() > 15
		|| isClanLeader())
	{
		result << uint8(0) << uint8(-1);
		Send(&result);
		return;
	}

	// Process the deletion request in the database
	result	<< bCharIndex << strUserID << strSocNo;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::SelCharToAgent(Packet & pkt)
{
	Packet result(WIZ_SEL_CHAR);
	std::string strUserID, strAccountID;
	uint8 bInit;

	pkt >> strAccountID >> strUserID >> bInit;
	if (strAccountID.empty() || strAccountID.size() > MAX_ID_SIZE
		|| strUserID.empty() || strUserID.size() > MAX_ID_SIZE
		|| strAccountID != m_strAccountID)
	{
		Disconnect();
		return;
	}

	// Disconnect any currently logged in sessions.
	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);

	if (pUser != nullptr)
	{
		if (pUser->GetSocketID() != GetSocketID())
		{
			pUser->Disconnect();

			// And reject the login attempt (otherwise we'll probably desync char data)
			result << uint8(CannotSelectingCharacter);
			Send(&result);
			return;
		}
	}

	result << strUserID << bInit;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::SelectCharacter(Packet & pkt)
{
	Packet result(WIZ_SEL_CHAR);
	uint8 bResult, bInit;

	pkt >> bResult >> bInit;

	switch (bResult)
	{
	case 0: 
	{
		result << uint8(CannotSelectingCharacter);
		Send(&result);
	}break;
	case 1:
	{
		m_pMap = g_pMain->GetZoneByID(GetZoneID());
		if (GetMap() == nullptr)
		{
			result << uint8(CannotSelectingCharacter);
			Send(&result);
			return;
		}

		if (g_pMain->m_nServerNo != GetMap()->m_nServerNo)
		{
			_ZONE_SERVERINFO* pInfo = g_pMain->m_ServerArray.GetData(GetMap()->m_nServerNo);
			if (pInfo == nullptr)
			{
				result << uint8(CannotSelectingCharacter);
				Send(&result);
				return;
			}

			SendServerChange(pInfo->strServerIP, bInit);
			return;
		}

		if (GetLevel() > g_pMain->m_byMaxLevel)
		{
			Disconnect();
			return;
		}

		bool ElMoradCastleOtherNation = (GetZoneID() == ZONE_ELMORAD && !g_pMain->m_byElmoradOpenFlag && GetNation() == KARUS);
		bool LufersonCastleOtherNation = (GetZoneID() == ZONE_KARUS && !g_pMain->m_byKarusOpenFlag && GetNation() == ELMORAD);
		bool WarZoneInButNotWarZoneOpening = (GetMap()->isWarZone() && !g_pMain->isWarOpen());
		bool WarZoneOpeningButWictroryWrong = (GetMap()->isWarZone() && g_pMain->isWarOpen() && g_pMain->m_bVictory != 0 && g_pMain->m_bVictory != GetNation());
		bool isInTempleEventZoneOk = (isInTotalTempleEventZone());
		bool isWarZoneOpenButinUserOtherZone = (isOpenWarZoneKickOutOtherZone());
		bool isCastleSiegeWarOpenOK = (GetZoneID() == ZONE_DELOS && !CanEnterDelos());
		if (!isGM())
		{
			if ((ElMoradCastleOtherNation)
				|| (LufersonCastleOtherNation)
				|| (WarZoneInButNotWarZoneOpening)
				|| (WarZoneOpeningButWictroryWrong)
				|| (isInTempleEventZoneOk)
				|| (isWarZoneOpenButinUserOtherZone && g_pMain->isWarOpen())
				|| (isCastleSiegeWarOpenOK))
			{
				NativeZoneReturn();
				UserDataSaveToAgent();
				Disconnect();
				return;
			}

			if (isPartyTournamentinZone() || isClanTournamentinZone())
			{
				_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(GetZoneID());
				if ((TournamentClanInfo == nullptr) || (TournamentClanInfo->aTournamentClanNum[0] != GetClanID()
					&& TournamentClanInfo->aTournamentClanNum[1] != GetClanID()))
				{
					NativeZoneReturn();
					UserDataSaveToAgent();
					Disconnect();
					return;
				}
			}
		}

		if (!g_pMain->isWarOpen() && GetFame() == COMMAND_CAPTAIN)
			m_bFame = CHIEF;

		SetLogInInfoToDB(bInit);
		SetUserAbility(false);

		m_iMaxExp = g_pMain->GetExpByLevel(GetLevel(), GetRebirthLevel());

		SetRegion(GetNewRegionX(), GetNewRegionZ());

		result << bResult << GetZoneID() << GetSPosX() << GetSPosZ() << GetSPosY() << GetNation();
		m_bSelectedCharacter = true;
		Send(&result);

		if (GetClanID() <= NO_CLAN)
		{
			SetClanID(NO_CLAN);
			m_bFame = 0;
			return;
		}
		else if (GetClanID() != 0
			&& GetZoneID() > 2)
		{
			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result << uint8(KNIGHTS_LIST_REQ) << GetClanID();
			g_pMain->AddDatabaseRequest(result, this);
		}
	}break;
	case 2:
	{
		result << uint8(AccountBlocked);
		Send(&result);
	}break;
	case 3:
	{
		result << uint8(AlreadyCharacterName);
		Send(&result);
	}break;
	case 4:
	{
		string strCharID1, strCharID2, strCharID3, strCharID4, strPrısonID;
		uint8 ZoneID = 0;

		if (!g_DBAgent.GetAllCharID(m_strAccountID, strCharID1, strCharID2, strCharID3, strCharID4))
		{
			result << uint8(CannotSelectingCharacter);
			Send(&result);
			return;
		}

		result.SByte();
		if (strCharID1.length() != 0)
		{
			if (g_DBAgent.CharacterSelectPrisonCheck(strCharID1, ZoneID))
			{
				if (ZoneID == ZONE_PRISON)
				{
					result << uint8(PrisonCharacter) << strCharID1 << "SERVER 1";
					Send(&result);
					return;
				}
			}
		}
		if (strCharID2.length() != 0)
		{
			if (g_DBAgent.CharacterSelectPrisonCheck(strCharID2, ZoneID))
			{
				if (ZoneID == ZONE_PRISON)
				{
					result << uint8(PrisonCharacter) << strCharID2 << "SERVER 2";
					Send(&result);
					return;
				}
			}
		}
		if (strCharID3.length() != 0)
		{
			if (g_DBAgent.CharacterSelectPrisonCheck(strCharID3, ZoneID))
			{
				if (ZoneID == ZONE_PRISON)
				{
					result << uint8(PrisonCharacter) << strCharID3 << "SERVER 3";
					Send(&result);
					return;
				}
			}
		}
		if (strCharID4.length() != 0)
		{
			if (g_DBAgent.CharacterSelectPrisonCheck(strCharID4, ZoneID))
			{
				if (ZoneID == ZONE_PRISON)
				{
					result << uint8(PrisonCharacter) << strCharID4 << "SERVER 4";
					Send(&result);
					return;
				}
			}
		}

		if (ZoneID == 0)
		{
			Disconnect();
			return;
		}
	}break;
	case 5:
	{
		result << uint8(MaintanenceMode);
		Send(&result);
	}break;
	case 6:
		Disconnect();
		break;
	default:
		result << uint8(CannotSelectingCharacter);
		Send(&result);
		break;
	}
}

void CUser::SendServerChange(std::string & ip, uint8 bInit)
{
	Packet result(WIZ_SERVER_CHANGE);
	result << ip 
		<< uint16(g_pMain->m_GameServerPort) 
		<< bInit 
		<< GetZoneID() 
		<< GetNation();
	Send(&result);
}

// happens on character selection
void CUser::SetLogInInfoToDB(uint8 bInit)
{
	_ZONE_SERVERINFO *pInfo = g_pMain->m_ServerArray.GetData(g_pMain->m_nServerNo);
	if (pInfo == nullptr) 
	{
		Disconnect();
		return;
	}

	Packet result(WIZ_LOGIN_INFO);
	result	<< GetName() 
		<< pInfo->strServerIP 
		<< uint16(g_pMain->m_GameServerPort) 
		<< GetRemoteIP() 
		<< bInit;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::RecvLoginInfo(Packet & pkt)
{
	int8 bResult = pkt.read<uint8>();
	if (bResult < 0)
		Disconnect();
}

// This packet actually contains the char name after the opcode
void CUser::GameStart(Packet & pkt)
{
	if (isInGame())
		return;

	uint8 opcode = pkt.read<uint8>();
	Packet result(WIZ_GAMESTART);

	if (opcode == 1)
	{
		SendMyInfo();
		g_pMain->UserInOutForMe(this);
		g_pMain->NpcInOutForMe(this);
		g_pMain->MerchantUserInOutForMe(this);
		SendNotice();
		TopSendNotice();
		SendTime();
		SendWeather();
		UserRegionChat(pkt);
		QuestDataRequest();
		QuestOpenSkillRequest();
		HackToolList(false);
		SendInfo();
		result.clear();
		result.Initialize(WIZ_STORY);
		result << uint32(0) << uint16(0);
		Send(&result);
		result.clear();
		result.Initialize(WIZ_GAMESTART);
		Send(&result);
	}
	else if (opcode == 2)
	{
		m_state = GAME_STATE_INGAME;
		UserInOut(INOUT_RESPAWN);

		if (!m_bCity && m_sHp <= 0)
			m_bCity = -1;

		if (m_bCity > 0)
		{
			int level = GetLevel();
			if (m_bCity <= 100)
				level--;

			// make sure we don't exceed bounds
			if (level > g_pMain->m_byMaxLevel)
				level = g_pMain->m_byMaxLevel;
			else if (level < 1)
				level = 1;

			int LostExp = 0;
			LostExp = ((level) * (m_bCity % 10) / 100);
			m_iLostExp = g_pMain->GetExpByLevel(LostExp, GetRebirthLevel());

			if (((m_bCity % 10) / 100) == 1)
				m_iLostExp /= 2;
		}
		else
		{
			m_iLostExp = 0;
		}
		
		BlinkStart();
		SetUserAbility();
		QuestDataRequest(true);
		QuestOpenSkillRequest();
		HackToolList(true);

		if (GetLevel() >= 1 && GetLevel() <= g_pMain->m_byMaxLevel)
			AchieveNormalCountAdd(UserAchieveNormalTypes::AchieveReachLevel, 0, nullptr);
		
		if (isPortuKurian())
			SpChange(m_MaxSp);

		// If we've relogged while dead, we need to make sure the client 
		// is still given the option to revive.
		if (isDead())
		{
			CheckRespawnScroll();
			SendDeathAnimation();
		}

		g_pMain->TempleEventGetActiveEventTime(this);
		g_pMain->KnightRoyaleGetActiveEventTime(this);
		m_tGameStartTimeSavedMagic = UNIXTIME;

		m_LastX = GetX();
		m_LastZ = GetZ();

		if (GetFame() == COMMAND_CAPTAIN 
			&& isClanLeader())
			ChangeFame(CHIEF);

		CKnights* pKnight = g_pMain->GetClanPtr(GetClanID());
		if (pKnight) {
			pKnight->CheckKnightBuffSystem();
			pKnight->CheckKnightCastellanCapesBuffSystem(this);
		}

		SendClanPremiumInfo();
	}

	m_tHPLastTimeNormal = UNIXTIME;
	m_tSpLastTimeNormal = UNIXTIME;

	HOOK_SendLifeSkills();

	if (g_pMain->isWarOpen())
	{
		g_pMain->m_CommanderArrayLock.lock();
		foreach(itr, g_pMain->m_CommanderArray)
		{
			if (GetName() == itr->second->strName && GetFame() != COMMAND_CAPTAIN)
			{
				ChangeFame(COMMAND_CAPTAIN);
				break;
			}
		}
		g_pMain->m_CommanderArrayLock.unlock();
	}

	if (GetZoneID() == ZONE_DELOS)
		CastleSiegeWarFlag();
}

void CUser::LoadingLogin(Packet &pkt)
{
	Packet result(WIZ_LOADING_LOGIN);
	result << uint8(1) << uint32(0);
	Send(&result);
}

/*void CUser::LoadingLogin(Packet &pkt)
{
	uint16 count = 0, remainCount = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame())
			count++;
	}

	if (count >= MAX_USER)
	{
		if (g_pMain->m_sLoginingPlayer > 1000)
			g_pMain->m_sLoginingPlayer = 0;

		g_pMain->m_sLoginingPlayer++;
		m_bWaitingOrder = true;
		m_tOrderRemain = UNIXTIME + 10;

		Packet result(WIZ_LOADING_LOGIN, uint8(1));
		result << int32(g_pMain->m_sLoginingPlayer);
		Send(&result);
		return;
	}

	if (g_pMain->m_sLoginingPlayer > 0)
		g_pMain->m_sLoginingPlayer--;

	// Online User Counts
	Packet result(WIZ_LOADING_LOGIN, uint8(1));
	result << int32(g_pMain->m_sLoginingPlayer);
	Send(&result);
}*/