#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "../shared/tstring.h"
#include "DBAgent.h"

#pragma region CKnightsManager::PacketProcess(CUser *pUser, Packet & pkt)
// TODO: Move this to the CUser class.
void CKnightsManager::PacketProcess(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
	case KNIGHTS_CREATE:
		CreateKnights(pUser, pkt);
		break;
	case KNIGHTS_JOIN:
		KnightsJoin(pUser, pkt);
		break;
	case KNIGHTS_WITHDRAW:
		KnightsWithdraw(pUser, pkt);
		break;
	case KNIGHTS_REMOVE:
		KnightsRemove(pUser, pkt);
		break;
	case KNIGHTS_DESTROY:
		KnightsDestroy(pUser);
		break;
	case KNIGHTS_ADMIT:// done
		KnightsAdmit(pUser, pkt);
		break;
	case KNIGHTS_REJECT:
		KnightsReject(pUser, pkt);
		break;
	case KNIGHTS_PUNISH:
		KnightsPunish(pUser, pkt); 
		break;
	case KNIGHTS_CHIEF:
		KnightsChief(pUser, pkt);
		break;
	case KNIGHTS_VICECHIEF:
		KnightsViceChief(pUser, pkt);
		break;
	case KNIGHTS_OFFICER:
		KnightsOfficer(pUser, pkt);
		break;
	case KNIGHTS_ALLLIST_REQ:
		KnightsAllList(pUser, pkt);
		break;
	case KNIGHTS_MEMBER_REQ:
		KnightsAllMember(pUser);
		break;
	case KNIGHTS_CURRENT_REQ:
		KnightsCurrentMember(pUser, pkt);
		break;
	case KNIGHTS_JOIN_REQ:
		KnightsJoinReq(pUser, pkt);
		break;
	case KNIGHTS_MARK_REGISTER:
		KnightsRegisterSymbol(pUser, pkt);
		break;
	case KNIGHTS_MARK_VERSION_REQ:
		KnightsRequestSymbolVersion(pUser, pkt);
		break;
	case KNIGHTS_MARK_REGION_REQ:
		KnighsRequestSymbols(pUser, pkt);
		break;
	case KNIGHTS_MARK_REQ:
		KnightsGetSymbol(pUser, pkt.read<uint16>());
		break;
	case KNIGHTS_ALLY_CREATE:
	case KNIGHTS_ALLY_REQ:
	case KNIGHTS_ALLY_INSERT:
	case KNIGHTS_ALLY_REMOVE:
	case KNIGHTS_ALLY_PUNISH:
	case KNIGHTS_ALLY_LIST:
		KnightsAllianceProcess(pUser,pkt, opcode);
		break;
	case KNIGHTS_TOP10:
		ListTop10Clans(pUser);
		break;
	case KNIGHTS_POINT_REQ:
		DonateNPReq(pUser, pkt);
		break;
	case KNIGHTS_DONATE_POINTS:
		DonateNP(pUser, pkt);
		break;
	case KNIGHTS_DONATION_LIST:
		DonationList(pUser, pkt);
		break;
	case KNIGHTS_UPDATENOTICE:
		ClanNoticeUpdateProcess(pUser,pkt);
		break;
	case KNIGHTS_UPDATEMEMO:
		UserMemoUpdateProcess(pUser,pkt);
		break;
	case KNIGHTS_HANDOVER_VICECHIEF_LIST:
		KnightsHandoverList(pUser, pkt);
		break;
	case KNIGHTS_HANDOVER_REQ:
		KnightsHandoverReq(pUser, pkt);
		break;
	case KNIGHTS_HANDOVER:
		break;
	case KNIGHTS_POINT_METHOD:
		KnightsPointMethodModify(pUser, pkt);
		break;
	case KNIGHTS_VS_LIST:
		KnightsVsLoginList(pUser);
		break;
	default:
		TRACE("Unhandled clan system opcode: %X\n", opcode);
		printf("Unhandled clan system opcode: %X\n", opcode);
		break;
	}
}
#pragma endregion

#pragma region Knight Packet Handler Methods

#pragma region KnightsVsLoginList(CUser* pUser)
void CKnightsManager::KnightsVsLoginList(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	bool TournamentTrueZone = (pUser->GetZoneID() == 77
		|| pUser->GetZoneID() == 78
		|| pUser->GetZoneID() == 96
		|| pUser->GetZoneID() == 97
		|| pUser->GetZoneID() == 98
		|| pUser->GetZoneID() == 99);

	if (!TournamentTrueZone)
		return;

	_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(pUser->GetZoneID());
	if (TournamentClanInfo == nullptr)
		return;

	if (TournamentClanInfo->aTournamentisStarted == false)
		return;

	CKnights *pRedClan = g_pMain->GetClanPtr(TournamentClanInfo->aTournamentClanNum[0]);/*Red Clan*/
	if (pRedClan == nullptr)
		return;

	CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentClanInfo->aTournamentClanNum[1]);/*Blue Clan*/
	if (pBlueClan == nullptr)
		return;

	Packet TournamentClanPacket(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_VS_LIST));
	TournamentClanPacket.SByte();
	TournamentClanPacket << uint8(TournamentClanInfo->aTournamentZoneID)
		<< pRedClan->GetID()
		<< pRedClan->m_sMarkVersion
		<< pRedClan->m_sMarkVersion
		<< pRedClan->GetName()
		<< pRedClan->GetName()
		<< pBlueClan->GetID()
		<< pBlueClan->m_sMarkVersion
		<< pBlueClan->m_sMarkVersion
		<< pBlueClan->GetName()
		<< pBlueClan->GetName();
	pUser->Send(&TournamentClanPacket);
}
#pragma endregion

#pragma region CKnightsManager::CreateKnights(CUser* pUser, Packet & pkt)
/**
* @brief	Handles the Knight creation requests and sends request
*			to the database to check if the knights could be created.
*/
void CKnightsManager::CreateKnights(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_CREATE));
	std::string idname;
	uint8 ret_value = 0;
	pkt >> idname;

	if (idname.empty() || idname.size() > MAX_ID_SIZE
		|| !IsAvailableName(idname.c_str()))
		ret_value = 3; // Invalid clan name or invalid clan name
	else if (pUser->GetClanID() > NO_CLAN)
		ret_value = 5; // user already in a clan.
	else if (g_pMain->m_nServerGroup == 2)
		ret_value = 8; // cannot create clan in the second servers.
	else if (!pUser->GetMap()->canUpdateClan())
		ret_value = 9; // cannot create in unallowed zones
	else if (pUser->GetLevel() < CLAN_LEVEL_REQUIREMENT)
		ret_value = 2; // level is below 20
	else if (!pUser->hasCoins(CLAN_COIN_REQUIREMENT))
		ret_value = 4; // dont have enough coins to create a clan

	if (ret_value == 0)
	{
		uint16 knightindex = GetKnightsIndex(pUser->m_bNation);
		if (knightindex >= 0)
		{	
			//result	<< uint8(ClanTypeTraining) 
			result << uint8(g_pMain->RoyalG1 ? ClanTypeRoyal1 : ClanTypeTraining)
				<< knightindex << pUser->GetNation()
				<< idname << pUser->GetName();
			g_pMain->AddDatabaseRequest(result, pUser);
			return;
		}

		ret_value = 6; // cannot create clan at this time.
	}

	result << ret_value;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsJoin(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the join requests. This does not send any database
*			request since we do nothing until the requested player answers 
*			to this request.
*/
void CKnightsManager::KnightsJoin(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	uint8 bResult = 0;

	do
	{
		if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (!pUser->isClanLeader() && !pUser->isClanAssistant())
			bResult = 6;

		if (bResult != 0)
			break;

		uint16 sClanID = pUser->GetClanID();
		CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
		if (pKnights == nullptr)
		{
			bResult = 7;
			break;
		}

		CUser *pTUser = g_pMain->GetUserPtr(pkt.read<uint16>());
		if (pTUser == nullptr)
			bResult = 2; // target is invalid.
		else if (pTUser->isDead())
			bResult = 3; // target is dead.
		else if (pTUser->GetNation() != pUser->GetNation())
			bResult = 4; // cannot add an opposing nation's player
		else if (pTUser->GetClanID() > NO_CLAN)
			bResult = 5; // target user is in a clan already.

		if (bResult != 0)
			break;

		pTUser->m_bKnightsReq = sClanID;

		result	<< uint8(KNIGHTS_JOIN_REQ) << uint8(1)
			<< pUser->GetSocketID() << sClanID << pKnights->m_strName;
		pTUser->Send(&result);
		return;
	} while (0);


	result << uint8(KNIGHTS_JOIN) << bResult;
	pUser->Send(&result);
}
#pragma endregion

#pragma region CKnightsManager::KnightsWithdraw(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the leaving requests. 
*/
void CKnightsManager::KnightsWithdraw(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if(pKnights == nullptr)
		return;


	Packet result(WIZ_KNIGHTS_PROCESS);
	uint8 bResult = 0;
	do
	{
		if (!pUser->isInClan())
			bResult = 10; // user not in a clan.
		else if (pUser->isClanLeader() && !pUser->GetMap()->canUpdateClan())
			bResult = 12; 

		if (bResult != 0)
			break;

		result	<< uint8(pUser->isClanLeader() ? KNIGHTS_DESTROY : KNIGHTS_WITHDRAW)
			<< pUser->GetClanID() << pUser->GetName();
		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);

	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsRemove(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the removing a clan member requests. 
*/
void CKnightsManager::KnightsRemove(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_REMOVE));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if(pKnights == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;	
		else if (!pUser->isClanLeader())
			bResult = 6;

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser != nullptr)
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsDestroy( CUser* pUser )

void CKnightsManager::KnightsDestroy( CUser* pUser )
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_DESTROY));
	uint8 bResult = 1;
	if (!pUser->isClanLeader())
		bResult = 0;
	else if (!pUser->GetMap()->canUpdateClan())
		bResult = 12;

	if (bResult == 1)
	{
		result << pUser->GetClanID() << pUser->GetName();
		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	}
	else
	{
		result << bResult;
		pUser->Send(&result);
		return;
	}
}
#pragma endregion

#pragma region CKnightsManager::KnightsAdmit(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the admitting a clan member requests. 
*/
void CKnightsManager::KnightsAdmit(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ADMIT));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;
		else if (pUser->GetFame() < OFFICER)
			bResult = 0;	

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsReject(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the rejecting a clan member requests. 
*/
void CKnightsManager::KnightsReject(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_REJECT));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;
		else if (pUser->GetFame() < OFFICER)
			bResult = 0;	

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pTUser->GetName();
		else
			result << pUser->GetClanID() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsPunish(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the rejecting a clan member requests. 
*/
void CKnightsManager::KnightsPunish(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_PUNISH));
	uint8 bResult = 1, bRemoveFlag = 0;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;
		else if (pUser->GetFame() < VICECHIEF)
			bResult = 0;	

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsChief(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the assignment of chieftain to a clan member requests. 
*/
void CKnightsManager::KnightsChief(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_CHIEF));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;	
		else if (!pUser->isClanLeader())
			bResult = 6;

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsViceChief(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the rejecting a clan member requests. 
*/
void CKnightsManager::KnightsViceChief(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_VICECHIEF));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;	
		else if (!pUser->isClanLeader())
			bResult = 6;

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;

			if (bResult == 1)
			{
				if (pTUser->isClanAssistant())
					bResult = 23; // already clan assistant

				if(pTUser == pUser)
					bResult = 9; // you cannot chouse yourself

				if(!pKnights->m_strViceChief_1.empty() && !pKnights->m_strViceChief_2.empty() && !pKnights->m_strViceChief_3.empty())
					bResult = 24; // the number of assistant users has exceeded.
			}
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsOfficer(CUser *pUser, Packet & pkt)
/**
* @brief	Handles the rejecting a clan member requests. 
*/
void CKnightsManager::KnightsOfficer(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights *pKnights = nullptr;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_OFFICER));
	uint8 bResult = 1;
	std::string strUserID;

	pkt >> strUserID;
	if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		bResult = 7; // The clan is invalid.
		goto fail_return;
	}

	do
	{
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			bResult = 2;
		else if (!pUser->GetMap()->canUpdateClan())
			bResult = 12;
		else if (STRCASECMP(strUserID.c_str(), pUser->GetName().c_str()) == 0)
			bResult = 9;	
		else if (!pUser->isClanLeader())
			bResult = 6;

		if (bResult != 1)
			break;

		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr)
			bResult = 2;
		else
		{
			if (pUser->GetNation() != pTUser->GetNation())
				bResult = 4;
			else if (pUser->GetClanID() != pTUser->GetClanID())
				bResult = 5;
		}

		if (bResult != 1)
			break;

		if(pTUser != nullptr)
			result << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		else
			result << pUser->GetClanID() << pUser->GetName() << strUserID;

		g_pMain->AddDatabaseRequest(result, pUser);
		return;
	} while (0);
fail_return:
	result << bResult;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsJoinReq(CUser *pUser, Packet & pkt)

/**
* @brief	Handles the response made by the user for the clan join 
*			requests.
*/
void CKnightsManager::KnightsJoinReq(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr || pUser->m_bKnightsReq == 0)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_JOIN));
	uint8 bFlag, bResult = 0;
	uint16 sid, sClanID;
	pkt >> bFlag >> sid >> sClanID;
	CUser *pTUser = g_pMain->GetUserPtr(sid);
	if (pTUser == nullptr)
		bResult = 2;
	else if (bFlag == 0)
		bResult = 11;
	else
	{
		CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
		if (pKnights == nullptr)
			bResult = 7;
		else if (pKnights->m_sMembers >= MAX_CLAN_USERS)
			bResult = 8;
	}

	if (bResult != 0)
	{
		result << bResult;
		pUser->Send(&result);
		if(pTUser)
			pTUser->Send(&result);
	}
	else
	{
		result << sClanID << pUser->GetName();
		g_pMain->AddDatabaseRequest(result, pUser);
	}

	pUser->m_bKnightsReq = 0;
}
#pragma endregion

#pragma region CKnightsManager::AllKnightsMember(CUser *pUser)
/**
* @brief	Gets the information of all knight members.
*/
void CKnightsManager::KnightsAllMember(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MEMBER_REQ));
	uint8 bResult = 1;
	CKnights *pKnights;

	if (!pUser->isInClan())
		bResult = 2;
	else if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
		bResult = 7;

	result << bResult;
	if (bResult == 1)
	{
		result.DByte();
		uint16 pktSize = 0, count = 0;

		result << pktSize << uint16(2) << uint16(MAX_CLAN_USERS) << pKnights->m_strClanNotice;

		size_t pos = result.wpos();
		result	<< count; // placeholder

		pktSize = (uint16)result.size();
		count = pKnights->GetKnightsAllMembers(result, pktSize, pUser->isClanLeader());

		if (count > MAX_CLAN_USERS) 
			return;

		// TO-DO: Uint16(2) sanirim yanlis burada count geliyor olabilir
		pktSize = ((uint16)result.size() - pktSize) + 4;
		result.put(2, pktSize);
		result.put(pos, count);
	}
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsCurrentMember(CUser *pUser, Packet & pkt)
/**
* @brief	Used at some quest issues. Dunno what this is.
*/
void CKnightsManager::KnightsCurrentMember(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_CURRENT_REQ));
	CKnights *pKnights = nullptr;
	if (!pUser->isInClan()
		|| (pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
	{
		result << uint8(0); // failed
		result << "is this error still used?";
		pUser->Send(&result);
		return;
	}

	uint16 page = pkt.read<uint16>();
	uint16 start = page * 10;
	uint16 count = 0;

	result	<< uint8(1) // success
		<< pKnights->m_strChief
		<< page;

	size_t pos = result.wpos();
	result	<< count; // placeholder

	foreach_stlmap (i, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = i->second;
		if (p == nullptr || count++ < start)
			continue;

		CUser* pTUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		if (pTUser == nullptr)
			continue;

		result << pTUser->GetName() << pTUser->GetFame() << pTUser->GetLevel() << pTUser->GetClass();
		count++;
		if (count >= start + 10)
			break;
	}

	count -= start;
	result.put(pos, count);
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsAllList(CUser *pUser, Packet & pkt)

void CKnightsManager::KnightsAllList(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLLIST_REQ));
	uint16 sPage = pkt.read<uint16>(), start = sPage * 10, count = 0;
	result << uint8(1) << sPage << count;
	foreach_stlmap (itr, g_pMain->m_KnightsArray)
	{
		CKnights* pKnights = itr->second;
		if (pKnights == nullptr
			|| !pKnights->isPromoted()
			|| pKnights->m_byNation != pUser->GetNation()
			|| count++ < start) 
			continue;

		result	<< uint16(pKnights->m_sIndex) << pKnights->m_strName 
			<< uint16(pKnights->m_sMembers) << pKnights->m_strChief 
			<< uint32(pKnights->m_nPoints);
		if (count >= start + 10)
			break;
	}

	count -= start;
	result.put(4, count);
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::KnightsRegisterSymbol(CUser* pUser, Packet & pkt)
/**
* @brief	Handles knights symbol registration process.
*/
void CKnightsManager::KnightsRegisterSymbol(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr || !pUser->isInClan())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MARK_REGISTER));
	CKnights *pKnights = nullptr;
	char clanSymbol[MAX_KNIGHTS_MARK];
	uint16 sFailCode = 1, sSymbolSize = pkt.read<uint16>();

	// Are they even a clan leader?
	if (!pUser->isClanLeader())
		sFailCode = 11;
	// Invalid zone (only in home zones)
	else if (pUser->GetZoneID() != pUser->GetNation())
		sFailCode = 12;
	// Invalid symbol size (or invalid packet)
	else if (sSymbolSize == 0 
		|| sSymbolSize > MAX_KNIGHTS_MARK
		|| pkt.size() < sSymbolSize)
		sFailCode = 13;
	// User doesn't have enough coins
	else if (pUser->m_iGold < CLAN_SYMBOL_COST)
		sFailCode = 14;
	// Clan doesn't exist
	else if ((pKnights = g_pMain->GetClanPtr(pUser->GetClanID())) == nullptr)
		sFailCode = 20;
	// Clan not promoted
	else if (!pKnights->isPromoted())
		sFailCode = 11;

	// Uh oh, did we error?
	if (sFailCode != 1)
	{
		result << sFailCode;
		pUser->Send(&result);
		return;
	}

	// Read the clan symbol from the packet
	pkt.read(clanSymbol, sSymbolSize);

	// Nope? Let's update the clan symbol.
	result	<< pUser->GetClanID() << sSymbolSize;
	result.append(clanSymbol, sSymbolSize);
	g_pMain->AddDatabaseRequest(result, pUser);
}
#pragma endregion

#pragma region CKnightsManager::KnightsRequestSymbolVersion(CUser* pUser, Packet & pkt)

void CKnightsManager::KnightsRequestSymbolVersion(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| !pUser->isInClan())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_MARK_VERSION_REQ));
	int16 sFailCode = 1;

	CKnights *pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr || !pKnights->isPromoted() || !pUser->isClanLeader())
		sFailCode = 11;
	else if (pUser->GetZoneID() != pUser->GetNation())
		sFailCode = 12;

	result << sFailCode;

	if (sFailCode == 1)
		result << uint16(pKnights->m_sMarkVersion);

	pUser->Send(&result);
}
#pragma endregion

#pragma region CKnightsManager::KnighsRequestSymbols(CUser* pUser, Packet & pkt)
// TO-DO: Make this server sided.
/**
* The clan member (leader only?) tells groups of users to update the clan symbols
* they have for this clan. This is a horrible, horrible idea.
*/
void CKnightsManager::KnighsRequestSymbols(CUser* pUser, Packet & pkt)
{
	// Should we force them to be a clan leader too? 
	// Need to check if *any* clan member can trigger this, or if it's just leaders.
	if (pUser == nullptr
		|| !pUser->isInClan())
		return;

	uint16 sCount = pkt.read<uint16>();
	if (sCount > 100)
		sCount = 100;

	for (int i = 0; i < sCount; i++)
	{
		uint16 sid = pkt.read<uint16>();
		CUser *pTUser = g_pMain->GetUserPtr(sid);
		if (pTUser == nullptr
			|| !pTUser->isInGame())
			continue;

		// This is really quite scary that users can send directly to specific players like this.
		// Quite possibly we should replace this with a completely server-side implementation.
		KnightsGetSymbol(pTUser, pUser->GetClanID());
	}
}
#pragma endregion

#pragma region CKnightsManager::KnightsGetSymbol(CUser* pUser, uint16 sClanID)

void CKnightsManager::KnightsGetSymbol(CUser* pUser, uint16 sClanID)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	CKnights *pKnights = g_pMain->GetClanPtr(sClanID);

	// Dose that clan exist?
	if (pKnights == nullptr 
		// Are they promoted?
			|| !pKnights->isPromoted()
			// Is their symbol version set?
			|| pKnights->m_sMarkVersion == 0
			// The clan symbol is more than 0 bytes, right?
			|| pKnights->m_sMarkLen <= 0)
			return;

	result	<< uint8(KNIGHTS_MARK_REQ) << uint16(1); // success
	result	<< uint16(pKnights->m_byNation) << sClanID
		<< uint16(pKnights->m_sMarkVersion) << uint16(pKnights->m_sMarkLen);
	result.append(pKnights->m_Image, pKnights->m_sMarkLen);
	pUser->SendCompressed(&result);
}
#pragma endregion

#pragma region CKnightsManager::KnightsHandoverList(CUser *pUser, Packet & pkt)

void CKnightsManager::KnightsHandoverList(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	uint8 isClanLeader = pUser->isClanLeader() ? 1 : 2;
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_HANDOVER_VICECHIEF_LIST));
	result << isClanLeader;

	size_t wpos = result.wpos();
	uint16 ViceChiefCount = 0;

	result << ViceChiefCount;
	if (g_pMain->GetUserPtr(pKnights->m_strViceChief_1, TYPE_CHARACTER))
	{
		result << pKnights->m_strViceChief_1;
		ViceChiefCount++;
	}

	if (g_pMain->GetUserPtr(pKnights->m_strViceChief_2, TYPE_CHARACTER))
	{
		result << pKnights->m_strViceChief_2;
		ViceChiefCount++;
	}

	if (g_pMain->GetUserPtr(pKnights->m_strViceChief_3, TYPE_CHARACTER))
	{
		result << pKnights->m_strViceChief_3;
		ViceChiefCount++;
	}

	result.put(wpos, ViceChiefCount);
	pUser->Send(&result);
}
#pragma endregion

#pragma region CKnightsManager::KnightsHandoverReq(CUser *pUser, Packet & pkt)

void CKnightsManager::KnightsHandoverReq(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);

	if (pUser->isClanLeader())
	{
		std::string strUserID;
		pkt >> strUserID;
		CUser *pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);

		if (pTUser == nullptr 
			// is the target is a vicechief?
				|| pTUser->GetFame() != VICECHIEF)
				goto fail_handover;

		result << uint8(KNIGHTS_HANDOVER) << pUser->GetClanID() << pUser->GetName() << pTUser->GetName();
		g_pMain->AddDatabaseRequest(result, pUser);
		return;
fail_handover:
		result << uint8(KNIGHTS_HANDOVER_REQ) << uint8(3);
		pUser->Send(&result);
		return;
	}

}
#pragma endregion

#pragma region CKnightsManager::KnightsPointMethodModify(CUser *pUser, Packet & pkt)

void CKnightsManager::KnightsPointMethodModify(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr 
		|| !pUser->isClanLeader())
		return;

	CKnights *pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

	if (pKnights == nullptr)
		return;


	uint8 subCode = 0;
	pkt >> subCode;

	uint8 bResult = 1;

	if (pKnights->m_byFlag >= ClanTypeAccredited5 && pUser->GetName() == pKnights->m_strChief)
	{
		uint8 method  = subCode != 0 ? subCode - 1 : pKnights->m_sClanPointMethod;
		Packet DB_pkt(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_POINT_METHOD));
		DB_pkt << pUser->GetName() << pUser->GetClanID() << method;
		g_pMain->AddDatabaseRequest(DB_pkt, pUser);
		return;
	}
	else
		bResult = 2;


	Packet result(WIZ_KNIGHTS_PROCESS, (uint8)KNIGHTS_POINT_METHOD);
	result << bResult << pKnights->GetClanPointMethod();
	pUser->Send(&result);
}
#pragma endregion

#pragma region CKnightsManager::ListTop10Clans(CUser *pUser)
/**
* @brief	Lists the top 10 clan, 5 from each nation.
*/
void CKnightsManager::ListTop10Clans(CUser *pUser)
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_TOP10));
	result << uint16(0);

	// List top 5 clans of each nation
	for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
	{
		uint16 i = 0;
		foreach_stlmap (itr, g_pMain->m_KnightsRatingArray[nation])
		{
			CKnights *pKnights = g_pMain->GetClanPtr(itr->second->sClanID);

			if (pKnights != nullptr && (pKnights->m_byNation == nation + 1))
				result << pKnights->m_sIndex << pKnights->m_strName << pKnights->m_sMarkVersion << int16(i++);

			if(i == 5)
				break;
		}
		for(int k = i; k < 5; k++)
		{
			result	<< int16(-1)	// Clan ID
				<< ""			// Clan name (2 byte length)
				<< int16(-1)	// Symbol version
				<< int16(k);	// Rank (0 - 4)
		}

	}

	pUser->Send(&result);
}
#pragma endregion

#pragma endregion

#pragma region Helper Methods
/**
* @brief	Checks whether the specified clan name is available.
*/
bool CKnightsManager::IsAvailableName( const char *strname)
{
	foreach_stlmap (itr, g_pMain->m_KnightsArray)
		if (STRCASECMP(itr->second->GetName().c_str(), strname) == 0)
			return false;

	return true;
}

/**
* @brief	Gets a Knight Index for the specified nation to be used on
*			creation of a new clan.
*
* @return	Returns the Knight index to be used on new clan creation on
*			success.
*/
int CKnightsManager::GetKnightsIndex(int nation)
{
	int knightindex = 0;
	if (nation == ELMORAD)	
		knightindex = 15000;

	foreach_stlmap (itr, g_pMain->m_KnightsArray)
	{
		if (itr->second != nullptr && knightindex < itr->second->GetID())
		{
			if (nation == KARUS && itr->second->GetID() >= 15000)
				continue;

			knightindex = itr->second->GetID();
		}
	}

	knightindex++;
	if ((nation == KARUS && (knightindex >= 15000 || knightindex < 0))
		|| nation == ELMORAD && (knightindex < 15000 || knightindex > 30000)
		|| g_pMain->GetClanPtr(knightindex))
		return -1;

	return knightindex;
}
#pragma endregion

#pragma region CKnightsManager::AddKnightsUser(int index, std::string & strUserID, std::string & strMemo, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, int32 Loyalty)

bool CKnightsManager::AddKnightsUser(int index, std::string & strUserID, std::string & strMemo, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, int32 Loyalty)
{
	CKnights *pKnights = g_pMain->GetClanPtr(index);
	if(pKnights == nullptr)
		return false;

	if(!pKnights->AddUser(strUserID, bFame, sClass, bLevel, lastlogin, Loyalty))
		return false;

	if(!pKnights->LoadUserMemo(strUserID,strMemo))
		return false;

	pKnights->m_nPoints += Loyalty;
	pKnights->m_byGrade = g_pMain->GetKnightsGrade(pKnights->m_nPoints);

	if(bFame == CHIEF)
		pKnights->m_strChief = strUserID;

	else if(bFame == VICECHIEF)
	{
		if(pKnights->m_strViceChief_1  == "")
			pKnights->m_strViceChief_1 = strUserID;
		else if(pKnights->m_strViceChief_2  == "")
			pKnights->m_strViceChief_2 = strUserID;
		else if(pKnights->m_strViceChief_3  == "")
			pKnights->m_strViceChief_3 = strUserID;
	}

	return true;
}

#pragma endregion 

#pragma region CKnightsManager::RemoveKnightsUser(int index, std::string & strUserID)

bool CKnightsManager::RemoveKnightsUser(int index, std::string & strUserID)
{
	CKnights *pKnights = g_pMain->GetClanPtr(index);
	if(pKnights == nullptr)
		return false;
	return pKnights->RemoveUser(strUserID);
}

#pragma endregion 

#pragma region CKnightsManager::UpdateKnightsGrade(uint16 sClanID, uint8 byFlag)

void CKnightsManager::UpdateKnightsGrade(uint16 sClanID, uint8 byFlag)
{
	CKnights * pClan = g_pMain->GetClanPtr(sClanID);
	if (pClan == nullptr)
		return;

	if (byFlag == ClanTypeTraining)
		pClan->m_sCape = -1;
	else if (byFlag == ClanTypePromoted)
		pClan->m_sCape = 0;

	pClan->m_byFlag = byFlag;
	pClan->SendUpdate();

	// Update the database server
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_UPDATE_GRADE));
	result << sClanID << byFlag << pClan->m_sCape;
	g_pMain->AddDatabaseRequest(result);
}

#pragma endregion 

#pragma region CKnightsManager::UpdateClanPoint(uint16 sClanID, int32 nChangeAmount)

void CKnightsManager::UpdateClanPoint(uint16 sClanID, int32 nChangeAmount)
{
	CKnights * pClan = g_pMain->GetClanPtr(sClanID);
	if (pClan == nullptr)
		return;

	if (nChangeAmount > 0)
	{
		if (pClan->m_nClanPointFund + nChangeAmount > LOYALTY_MAX)
			pClan->m_nClanPointFund = LOYALTY_MAX;
		else
			pClan->m_nClanPointFund += nChangeAmount;
	}
	else
	{
		uint32 pChangeAmount = -nChangeAmount;

		if (pChangeAmount > pClan->m_nClanPointFund)
			pClan->m_nClanPointFund = 0;
		else
			pClan->m_nClanPointFund -= pChangeAmount;
	}

	pClan->UpdateClanFund();
}

#pragma endregion 

#pragma region CKnightsManager::AddUserDonatedNP(int index, std::string & strUserID, uint32 nDonatedNP, bool isAddClanFund /*= false*/)

void CKnightsManager::AddUserDonatedNP(int index, std::string & strUserID, uint32 nDonatedNP, bool isAddClanFund /*= false*/)
{
	CKnights *pKnights = g_pMain->GetClanPtr(index);
	if (pKnights == nullptr)
		return;

	// Add to the clan point fund
	if(isAddClanFund)
		pKnights->m_nClanPointFund += nDonatedNP;

	std:: string userid= strUserID;
	STRTOUPPER(userid);
	_KNIGHTS_USER * pKnightUser = pKnights->m_arKnightsUser.GetData(userid);
	if(pKnightUser == nullptr)
		return;

	pKnightUser->nDonatedNP += nDonatedNP;
}

#pragma endregion 

#pragma region CKnightsManager::RecvKnightsAllList(Packet & pkt)

void CKnightsManager::RecvKnightsAllList(Packet & pkt)
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLLIST_REQ));
	uint8 count = pkt.read<uint8>(), send_count = 0;
	result << send_count; // placeholder for count
	for (int i = 0; i < count; i++)
	{
		uint32 nPoints; uint16 sClanID; uint8 bRank;
		pkt >> sClanID >> nPoints >> bRank;

		CKnights *pKnights = g_pMain->GetClanPtr(sClanID);
		if (pKnights == nullptr)
			continue;

		if (pKnights->m_nPoints != nPoints
			|| pKnights->m_byRanking != bRank)
		{
			pKnights->m_nPoints = nPoints;
			pKnights->m_byRanking = bRank;
			pKnights->m_byGrade = g_pMain->GetKnightsGrade(nPoints);

			result << sClanID << pKnights->m_byGrade << pKnights->m_byRanking;
			send_count++;
		}
	}

	result.put(1, send_count);
	g_pMain->Send_All(&result);
}

#pragma endregion 

#pragma region CKnightsManager::DonateNPReq(CUser * pUser, Packet & pkt)
/**
* @brief	Handles the clan NP info packet from the client.
* 			It is designed to tell the user how many points are
* 			currently stored, and how much they can donate.
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::DonateNPReq(CUser * pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| !pUser->isInClan())
		return;

	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_POINT_REQ));
	result	<< uint8(1) 
		<< uint32(pUser->GetLoyalty()) 
		<< uint32(pKnights->m_nClanPointFund); // note: amount shown is in NP form
	pUser->Send(&result);
}

#pragma endregion

#pragma region CKnightsManager::DonateNP(CUser *pUser, Packet & pkt)

/**
* @brief	Handles the clan NP donations packet from the client.
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::DonateNP(CUser *pUser, Packet & pkt)
{
	// Ensure the user's valid and in a clan.
	if (pUser == nullptr 
		|| !pUser->isInClan()
		// Ensure users don't try to donate NP when their NP is at or below the minimum.
		|| pUser->GetLoyalty() < MIN_NP_TO_DONATE)
		return;

	// Ensure the clan exists and the clan is at least Accredited.
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr
		|| pKnights->m_byFlag < ClanTypeAccredited5)
		return;

	// Pass the packet straight to the database thread for further processing.
	g_pMain->AddDatabaseRequest(pkt, pUser);
}

#pragma endregion 

#pragma region CKnightsManager::DonationList(CUser *pUser, Packet & pkt)

/**
* @brief	Handles the clan NP donations list packet from the client.
* 			i.e. the "save cont" button's "accumulation status" list.
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::DonationList(CUser *pUser, Packet & pkt)
{
	// Ensure the user's valid and in a clan.
	if (pUser == nullptr 
		|| !pUser->isInClan())
		return;

	// Ensure the clan exists and the clan is at least Accredited.
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr
		|| pKnights->m_byFlag < ClanTypeAccredited5)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_DONATION_LIST));
	uint8 count = 0;
	size_t wpos = result.wpos();
	result << count;

	foreach_stlmap(user, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER * p = user->second;
		if(p == nullptr)
			continue;
		result << p->strUserName << p->nDonatedNP;
		count++;
	}

	result.put(wpos, count);
	pUser->Send(&result);
}

#pragma endregion 

#pragma region CKnightsManager::ClanNoticeUpdateProcess(CUser *pUser, Packet & pkt)

void CKnightsManager::ClanNoticeUpdateProcess(CUser *pUser, Packet & pkt)
{
	pkt.DByte();
	std::string StrNotice;
	pkt >> StrNotice;
	if (pUser->isInClan() 
		&& pUser->isClanLeader())
	{
		CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
		if (pKnights == nullptr)
			return;

		pKnights->UpdateClanNotice(StrNotice);
	}
}

#pragma endregion

#pragma region CKnightsManager::UserMemoUpdateProcess(CUser *pUser, Packet & pkt) 

void CKnightsManager::UserMemoUpdateProcess(CUser *pUser, Packet & pkt)
{
	uint8 type;
	pkt >> type;

	switch (type)
	{
	case 2:
		{
			Packet result;
			std::string StrAllianceNotice; 
			pkt.DByte();
			pkt >> StrAllianceNotice;

			if (StrAllianceNotice.length() > MAX_CLAN_NOTICE_LENGTH)
				return;

			CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

			if (pKnights == nullptr 
				|| !pUser->isClanLeader())
				return;

			if (!pKnights->isInAlliance()
				|| !pKnights->isAllianceLeader())
				return;

			_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());

			if (pAlliance == nullptr)
				return;

			pAlliance->strAllianceNotice = StrAllianceNotice;
			pKnights->SendChatAlliance("### %s has updated the alliance announcement. ###", pUser->GetName().c_str());
			
			// Tell the database to update the strAllianceNotice.
			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result << uint8(KNIGHTS_UPDATEMEMO) << type << pKnights->GetAllianceID() << pUser->GetName() << StrAllianceNotice;
			g_pMain->AddDatabaseRequest(result);

			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result.DByte();
			result << uint8(KNIGHTS_UPDATEMEMO) << type << uint8(1) << StrAllianceNotice;
			g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
		}
		break;
	case 3:
		{
			std::string StrUserMemo, UpdateNotice;
			Packet result;
			pkt.DByte();
			pkt >> StrUserMemo;

			if (StrUserMemo.length() > MAX_USER_MEMO_LENGTH)
				return;

			CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());

			if (pKnights == nullptr)
				return;

			pKnights->LoadUserMemo(pUser->GetName(), StrUserMemo);
			
			// Construct the update notice packet to inform players the clan notice has changed
			UpdateNotice = string_format("### %s updated his/her memo. ###", pUser->GetName().c_str());
			ChatPacket::Construct(&result, KNIGHTS_CHAT, &UpdateNotice);
			pKnights->Send(&result);

			// Tell the database to update the strAllianceNotice.
			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result << uint8(KNIGHTS_UPDATEMEMO) << type << pUser->GetClanID() << pUser->GetName() << StrUserMemo;
			g_pMain->AddDatabaseRequest(result);

			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result.DByte();
			result << uint8(KNIGHTS_UPDATEMEMO) << type << uint8(1) << StrUserMemo;
			pKnights->Send(&result);
		}
		break;
	case 6:
		if (pUser->isInClan())
		{
			CKnights* pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
			Packet result;
			std::string username, title;
			pkt.SByte();
			pkt >> username >> title;
			bool bResult = true;

			if(pKnights == nullptr
				|| username != pUser->GetName())
			{
				if (pKnights == nullptr)
					bResult = false;
				if (username != pUser->GetName())
					bResult = false;
				goto error_title;
			}

			bResult = false; // TO-DO: Acco update paketi dinlenecek
error_title:
			result.Initialize(WIZ_KNIGHTS_PROCESS);
			result << uint8(KNIGHTS_UPDATEMEMO) << type << uint8(bResult) << username << title;
			
			if(bResult)
				pKnights->Send(&result);
			else
				pUser->Send(&result);
		}
		break;
	default:
		printf("Knight Memo undhandled packets %d \n",type);
		TRACE("Knight Memo undhandled packets %d \n", type);
		break;
	}
}

#pragma endregion 
