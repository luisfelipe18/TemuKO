#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "../shared/tstring.h"
#include "DBAgent.h"

using namespace std;

#pragma region CKnightsManager::KnightsAllianceProcess(CUser* pUser, Packet & pkt, uint8 opcode)
/**
* @brief	Handles the client packet responsible for all incoming alliance packets
*			from the client.
* 			
* @param	pUser	The user.
* @param	pkt  	The packet.
* @param	opcode  The opcode.
*/
void CKnightsManager::KnightsAllianceProcess(CUser* pUser, Packet & pkt, uint8 opcode)
{
	switch (opcode)
	{
	case KNIGHTS_ALLY_CREATE:
		KnightsAllianceCreate(pUser, pkt);
		break;
	case KNIGHTS_ALLY_REQ:
		KnightsAllianceRequest(pUser, pkt);
		break;
	case KNIGHTS_ALLY_INSERT:
		KnightsAllianceInsert(pUser, pkt);
		break;
	case KNIGHTS_ALLY_REMOVE:
		KnightsAllianceRemove(pUser, pkt);
		break;
	case KNIGHTS_ALLY_PUNISH:
		KnightsAlliancePunish(pUser, pkt);
		break;
	case KNIGHTS_ALLY_LIST:
		KnightsAllianceList(pUser, pkt);
		break;
	default:
		TRACE("Unhandled alliance system opcode: %X\n", opcode);
		printf("Unhandled alliance system opcode: %X\n", opcode);
		break;
	}

}

#pragma endregion

#pragma region CKnightsManager::CheckAlliance(CKnights * pMainClan, CKnights * pTargetClan)
/**
* @brief	Performs CSW/Delos checks to determine whether an alliance
* 			can be made with the specified clan.
*
* @param	pMainClan  	The clan seeking to form an alliance.
* @param	pTargetClan	Target clan an alliance is being formed with.
*
* @return	true if it succeeds, false if it fails.
*/
bool CKnightsManager::CheckAlliance(CKnights * pMainClan, CKnights * pTargetClan)
{
	if(pMainClan == nullptr  || pTargetClan == nullptr 
		|| pMainClan->m_byFlag <= 1
		|| pMainClan->m_byNation != pTargetClan->m_byNation)
		return false;

	return true;
}
#pragma endregion

#pragma region CKnightsManager::KnightsAllianceCreate(CUser* pUser, Packet & pkt)
/**
* @brief	Handles the client packet responsible for forming an alliance
* 			with another clan. We must already lead an alliance.
* 			
* 			NOTE: Most of the logic (error codes also) is for the most part 
* 			identical to the creation of the alliance. 
* 			It just doesn't _create_ an alliance (and requires the alliance to exist).
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::KnightsAllianceCreate(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| pUser->isDead()
		|| !pUser->isClanLeader())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_CREATE));

	CKnights * pMainClan = g_pMain->GetClanPtr(pUser->GetClanID());

	uint16 TargetClanLeaderID = pkt.read<uint16>();

	CUser * pTargetUser = g_pMain->GetUserPtr(TargetClanLeaderID);
	if(pTargetUser == nullptr) 
		return;

	CKnights *pTargetClan = g_pMain->GetClanPtr(pTargetUser->GetClanID());

	if (pMainClan == nullptr 
		// Cannot create a new alliance if we are already in  one.
			|| pMainClan->isInAlliance()
			// Cannot create an alliance if the target clan does not exist.
			|| pTargetClan == nullptr
			// We already have one alliance request. Proceed first that one.
			|| pTargetClan->m_sAllianceReq > 0
			// if the target clan is in another alliance
			|| pTargetClan->isInAlliance()
			// Only clans having Flag greater than 1 can create alliances.
			|| pMainClan->m_byFlag <= 1
			// Only the clan leaders can initiate creation of alliances.
			|| !pUser->isClanLeader() || !pTargetUser->isClanLeader()
			// The clans must share the same nation.
			|| pUser->GetNation() != pTargetUser->GetNation())
	{
		result << uint8(0);
		pUser->Send(&result);
		return;
	}

	pTargetClan->m_sAllianceReq = pMainClan->GetID();
	result.SByte();
	result << uint8(1) << pMainClan->GetName() << pMainClan->GetID();

	if(pTargetUser->isInGame())
		pTargetUser->Send(&result);

}
#pragma endregion

#pragma region CKnightsManager::KnightsAllianceInsert(CUser* pUser, Packet & pkt)

/**
* @brief	Handles the client packet responsible for forming an alliance
* 			with another clan. We must already lead an alliance.
* 			
* 			NOTE: Most of the logic (error codes also) is for the most part 
* 			identical to the creation of the alliance. 
* 			It just doesn't _create_ an alliance (and requires the alliance to exist).
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::KnightsAllianceInsert(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| pUser->isDead()
		|| !pUser->isClanLeader())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_REQ));

	CKnights * pMainClan = g_pMain->GetClanPtr(pUser->GetClanID());

	uint16 TargetClanLeaderID = pkt.read<uint16>();

	CUser * pTargetUser = g_pMain->GetUserPtr(TargetClanLeaderID);
	if(pTargetUser == nullptr)
		return;

	CKnights *pTargetClan = g_pMain->GetClanPtr(pTargetUser->GetClanID());

	_KNIGHTS_ALLIANCE * pAlliance =  g_pMain->GetAlliancePtr(pMainClan->GetID());
	if (pMainClan == nullptr || pTargetClan == nullptr
		|| (pAlliance != nullptr
		&& (pAlliance->sMercenaryClan_1 > NO_CLAN && pAlliance->sMercenaryClan_2 > NO_CLAN && pAlliance->sSubAllianceKnights > NO_CLAN))
		// We already have one alliance request. Proceed first that one.
		//|| pTargetClan->m_sAllianceReq > 0
		// if the target clan is in another alliance
		|| pTargetClan->isInAlliance()
		// Main clan must hold clan grade greater than or equal to 3.
		|| pMainClan->m_byFlag <= 1
		// The alliance clans must share the same nation.
		|| pUser->GetNation() != pTargetUser->GetNation() 
		// 
		|| !pTargetUser->isClanLeader())
	{
		result << uint8(0);
		if(pUser->isInGame())
			pUser->Send(&result);
		return;
	}

	pTargetClan->m_sAllianceReq = pMainClan->GetID();

	result.SByte();
	result << uint8(1)  << pMainClan->GetName() << pMainClan->GetID();
	if(pTargetUser->isInGame())
		pTargetUser->Send(&result);
}
#pragma endregion

#pragma region CKnightsManager::KnightsAllianceRequest(CUser* pUser, Packet & pkt)
/**
* @brief	Handles the client packet responsible for forming an alliance
* 			with another clan. We must already lead an alliance.
* 			
* 			NOTE: Most of the logic (error codes also) is for the most part 
* 			identical to the creation of the alliance. 
* 			It just doesn't _create_ an alliance (and requires the alliance to exist).
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::KnightsAllianceRequest(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| pUser->isDead()
		|| !pUser->isClanLeader())
		return;

	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if(pKnights == nullptr 
		// Did we really received any alliance request?
			|| !pKnights->isAnyAllianceRequest()
			// Are we already in an alliance? Betraying the other one! No, no, no!
			|| pKnights->isInAlliance())
			return;

	uint8 decision;
	pkt >> decision;

	if(decision != 0x01) // if the decision is not 1, then the rest is accepted as we are not accepting.
		goto fail_return;

	CKnights * pMainClan = g_pMain->GetClanPtr(pKnights->m_sAllianceReq);
	if(pMainClan == nullptr 
		// Are we entering an alliance of noobs? Oops, that's too bad!
			|| pMainClan->m_byFlag <= 1)
			goto fail_return;

	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(pMainClan->GetID());
	if(pAlliance == nullptr) // There has no alliance created before. So let us make one.
	{
		Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_CREATE));
		result << pMainClan->GetID() << pKnights->GetID();
		g_pMain->AddDatabaseRequest(result, pUser);
		goto fail_return;
	}

	if(!pMainClan->isInAlliance() 
		// if the clan is already in another alliance
			|| pAlliance->sMainAllianceKnights != pMainClan->GetAllianceID()
			//We cannot be in the alliance already?
			|| pAlliance->sMercenaryClan_1 == pKnights->GetID()
			|| pAlliance->sMercenaryClan_2 == pKnights->GetID()
			|| pAlliance->sSubAllianceKnights == pKnights->GetID()
			// The alliance is already full. SHIT!
			|| (pAlliance->sMercenaryClan_1 > NO_CLAN && pAlliance->sMercenaryClan_2 > NO_CLAN && pAlliance->sSubAllianceKnights > NO_CLAN))
	{
		/*
		The clients expects no error message ideally. But we still need to listen up some packets,
		to see if this holds.
		*/
		Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_REQ));
		result << uint8(0);
		pUser->Send(&result);
		goto fail_return;
	}
	// scoped
	{
		Packet ally_db(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_INSERT));
		ally_db << pMainClan->GetID() << pKnights->GetID();
		g_pMain->AddDatabaseRequest(ally_db, pUser);
	}
fail_return:
	pKnights->m_sAllianceReq = 0;
}
#pragma endregion

#pragma region CKnightsManager::KnightsAllianceRemove(CUser* pUser, Packet & pkt)

void CKnightsManager::KnightsAllianceRemove(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| pUser->isDead()
		// We cannot end an alliance if we aint a hacking clan leader.
		|| !pUser->isClanLeader())
		return;

	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if( pKnights == nullptr)
		return;

	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(pKnights->GetAllianceID());
	if( pAlliance == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_REMOVE));  // ally disband or leaving from the alliance
	result << pUser->GetName() << pAlliance->sMainAllianceKnights << pKnights->GetID();
	g_pMain->AddDatabaseRequest(result,pUser);
}

#pragma endregion

#pragma region CKnightsManager::KnightsAlliancePunish(CUser* pUser, Packet & pkt)

/**
* @brief	Removes a sub clan from the alliance
*
* @param	pUser	The clan leader.
* @param	pkt		The packet.
*/
void CKnightsManager::KnightsAlliancePunish(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| pUser->isDead()
		|| !pUser->isClanLeader())
		return;

	uint16 targetClanID = pkt.read<uint16>();

	CKnights * pTargetKnights = g_pMain->GetClanPtr(targetClanID);
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(pUser->GetClanID());

	if(pAlliance == nullptr
		|| pTargetKnights == nullptr
		// there exists no such clan.
		|| pKnights == nullptr
		// we cannot ban ourselves from the alliance. this has to go with the remove packet.
		|| pTargetKnights->GetID() == pAlliance->sMainAllianceKnights
		// only alliance owner clan make some manipulations on the alliance.
		|| !pKnights->isAllianceLeader()
		// if the target clan isnt in the alliance?
		|| !pTargetKnights->isInAlliance()
		|| (pTargetKnights->GetID() != pAlliance->sMercenaryClan_1 
		&& pTargetKnights->GetID() != pAlliance->sMercenaryClan_2
		&& pTargetKnights->GetID() != pAlliance->sSubAllianceKnights))
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_PUNISH)); // knights ban
	result << pUser->GetName() << targetClanID;
	g_pMain->AddDatabaseRequest(result,pUser);
}

#pragma endregion

#pragma region CKnightsManager::KnightsAllianceList(CUser* pUser, Packet & pkt)

/**
* @brief	Handles the client packet responsible for displaying
* 			all clans that are part of the user's clan's alliance.
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::KnightsAllianceList(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| !pUser->isInClan())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_LIST));
	_KNIGHTS_ALLIANCE * pAlliance;
	CKnights * pClan = g_pMain->GetClanPtr(pUser->GetClanID());

	if (pClan == nullptr
		|| !pClan->isInAlliance()
		|| (pAlliance = g_pMain->GetAlliancePtr(pClan->GetAllianceID())) == nullptr)
	{
		result << uint8(0);
		pUser->Send(&result);
		return;
	}

	uint16 clans[] = 
	{ 
		pAlliance->sMainAllianceKnights, pAlliance->sSubAllianceKnights, 
		pAlliance->sMercenaryClan_1, pAlliance->sMercenaryClan_2 
	};

	size_t wpos = result.wpos();
	uint8 clanCount = 0;

	result << clanCount << pAlliance->strAllianceNotice;

	result.SByte();
	foreach_array(i, clans)
	{
		uint16 sClanID = clans[i]; 
		CKnights * pTmp = g_pMain->GetClanPtr(sClanID);
		if (pTmp == nullptr)
			continue;
		uint8 infocount = 0;
		result << pTmp->GetID() << pTmp->GetName() << pTmp->isInAlliance();
		size_t wpos2 = result.wpos();
		result << infocount;
		if(!pTmp->m_strChief.empty())
		{
			infocount++;
			result << uint8(CHIEF) << pTmp->m_strChief;
		}
		if(!pTmp->m_strViceChief_1.empty())
		{
			infocount++;
			result << uint8(VICECHIEF) << pTmp->m_strViceChief_1;
		}
		if(!pTmp->m_strViceChief_2.empty())
		{
			infocount++;
			result << uint8(VICECHIEF) << pTmp->m_strViceChief_2;
		}
		if(!pTmp->m_strViceChief_3.empty())
		{
			infocount++;
			result << uint8(VICECHIEF) << pTmp->m_strViceChief_3;
		}
		result.put(wpos2, infocount);
		clanCount++;
	}

	if (clanCount == 0)
		return;

	result.put(wpos, clanCount);
	pUser->Send(&result);
}

#pragma endregion

/////////////////////////////////
// DATABASE REQUEST METHODS
/////////////////////////////////

#pragma region CKnightsManager::ReqKnightsAllianceCreate(CUser* pUser, Packet & pkt)
/**
* @brief	Handles the create alliance requests.
*
* @param	pUser	the leader of the alliance owner
* @param	pkt		the packet.
*/
void CKnightsManager::ReqKnightsAllianceCreate(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	uint16 mainid, targetid;
	pkt >> mainid >>targetid;

	CKnights * pTargetClan = g_pMain->GetClanPtr(targetid);
	CKnights * pMainClan = g_pMain->GetClanPtr(mainid);

	if(pTargetClan == nullptr
		|| pMainClan == nullptr)
		return;

	_KNIGHTS_ALLIANCE *pAlliance = new _KNIGHTS_ALLIANCE();
	pAlliance->sMainAllianceKnights = pMainClan->GetID();
	pAlliance->sSubAllianceKnights = pTargetClan->GetID();
	pAlliance->sMercenaryClan_1 = NO_CLAN;
	pAlliance->sMercenaryClan_2 = NO_CLAN;

	if(!g_pMain->m_KnightsAllianceArray.PutData(pAlliance->sMainAllianceKnights, pAlliance))
	{
		delete pAlliance;
		return;
	}

	int bResult = g_DBAgent.KnightsAllianceCreate(pMainClan->GetID(), pTargetClan->GetID(), 0);
	if (bResult > 0)
	{
		g_pMain->m_KnightsAllianceArray.DeleteData(pMainClan->GetID());
		return;
	}

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_INSERT));
	result << uint8(1) << pMainClan->GetID() << pTargetClan->GetID() << pMainClan->GetCapeID();

	if(pUser->isInGame())
		pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());

	pMainClan->m_sAlliance = pMainClan->GetID();
	pTargetClan->m_sAlliance = pMainClan->GetID();

	pTargetClan->SendUpdate();

	CKnights *pSendClan = nullptr;
	g_pMain->m_KnightsArray.m_lock.lock();
	foreach_stlmap_nolock(itr, g_pMain->m_KnightsArray)
	{
		if (itr->second->GetID() == pTargetClan->GetID())
			pSendClan = itr->second;
	}
	g_pMain->m_KnightsArray.m_lock.unlock();

	pSendClan->m_arKnightsUser.m_lock.lock();
	foreach_stlmap_nolock(itr, pSendClan->m_arKnightsUser)
	{
		_KNIGHTS_USER * pSendClanKnightUser = itr->second;
		if (pSendClanKnightUser == nullptr)
			continue;

		CUser* pSendClanUser = g_pMain->GetUserPtr(pSendClanKnightUser->strUserName, TYPE_CHARACTER);
		if (pSendClanUser == nullptr)
			continue;

		result.clear();
		result.Initialize(WIZ_KNIGHTS_PROCESS);
		result << uint8(KNIGHTS_ALLY_INSERT) << uint8(1) << pMainClan->GetID()
			<< pTargetClan->GetID() << pMainClan->GetCapeID();

		pSendClanUser->Send(&result);
	}
	pSendClan->m_arKnightsUser.m_lock.unlock();

	// Send the notices
	std::string noticeText;
	g_pMain->GetServerResource(IDS_INSERT_KNIGHTS_ALLIANCE, &noticeText, pTargetClan->GetName().c_str());
	pMainClan->SendChatAlliance("%s", noticeText.c_str());
	pTargetClan->SendChatAlliance("%s", noticeText.c_str());
	return;
}

#pragma endregion

#pragma region CKnightsManager::ReqKnightsAllianceInsert(CUser* pUser, Packet & pkt)
/**
* @brief	Handles the insertion of alliance requests.
*
* @param	pUser	the leader of the alliance owner
* @param	pkt		the packet.
*/
void CKnightsManager::ReqKnightsAllianceInsert(CUser* pUser, Packet & pkt)
{
	if (pUser == nullptr
		|| !pUser->isInGame())
		return;

	uint16 MainClanID, TargetClanID;
	pkt >> MainClanID >> TargetClanID;

	CKnights *pMainClan = g_pMain->GetClanPtr(MainClanID);
	CKnights *pTargetClan = g_pMain->GetClanPtr(TargetClanID);
	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(MainClanID);

	if(pMainClan == nullptr
		|| pTargetClan == nullptr
		|| pAlliance == nullptr
		|| pAlliance->sMainAllianceKnights != pMainClan->GetID())
		return;

	uint8 byEmptyIndex = 5; // some dummy number

	if (pAlliance->sSubAllianceKnights == NO_CLAN)
		byEmptyIndex = 1;
	else if (pAlliance->sMercenaryClan_1 == NO_CLAN)
		byEmptyIndex = 2;

	else if (pAlliance->sMercenaryClan_2 == NO_CLAN)
		byEmptyIndex = 3;

	if(byEmptyIndex == 5)
		return;

	// there only can be one clan of knights or above in the alliance.
	// that is so called by people, only one clan can wave its symbol back on the cape at the same time.
	/*if(byEmptyIndex != 1 && pTargetClan->m_byFlag > 1)
	return;*/

	int bResult = g_DBAgent.KnightsAllianceInsert(pMainClan->GetID(), pTargetClan->GetID(), byEmptyIndex);
	if (bResult > 0)
		return;

	if (byEmptyIndex == 1)
		pAlliance->sSubAllianceKnights = pTargetClan->GetID();
	else if (byEmptyIndex == 2)
		pAlliance->sMercenaryClan_1 = pTargetClan->GetID();
	else if (byEmptyIndex == 3)
		pAlliance->sMercenaryClan_2 = pTargetClan->GetID();

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_INSERT));
	result << uint8(1) << pMainClan->GetID() << pTargetClan->GetID() << pMainClan->GetCapeID();
	if(pUser->isInGame())
		pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());

	pTargetClan->m_sAlliance = pMainClan->GetID();
	pTargetClan->SendUpdate();

	CKnights * pKnights1 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_1),
		* pKnights2 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_2),
		* pKnights3 = g_pMain->GetClanPtr(pAlliance->sSubAllianceKnights);

	std::string noticeText;
	g_pMain->GetServerResource(IDS_INSERT_KNIGHTS_ALLIANCE, &noticeText, pTargetClan->GetName().c_str());
	pMainClan->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights1 != nullptr)
		pKnights1->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights2 != nullptr)
		pKnights2->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights3 != nullptr)
		pKnights3->SendChatAlliance("%s", noticeText.c_str());
}
#pragma endregion

#pragma region CKnightsManager::ReqKnightsAllianceRemove(CUser* pUser, Packet & pkt)
/**
* @brief	Leaving from an alliance. Note that this results in disbanding the
*			alliance if the main clan is leaving.
*
* @param	pUser	the user who wants to leave the alliance.
* @param	pkt		the packet.
*/
void CKnightsManager::ReqKnightsAllianceRemove(CUser* pUser, Packet & pkt)
{
	string requesterID;
	uint16 allianceID, clanid;
	pkt >> requesterID >> allianceID >> clanid;

	if (pUser == nullptr || !pUser->isClanLeader() || pUser->GetName() != requesterID)
		return;

	CKnights * pClan = g_pMain->GetClanPtr(clanid);
	if( pClan == nullptr)
		return;

	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(allianceID);
	if( pAlliance == nullptr)
		return;

	CKnights * pMainClan = g_pMain->GetClanPtr(pAlliance->sMainAllianceKnights);
	CKnights * pKnights1 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_1),
		* pKnights2 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_2),
		* pKnights3 = g_pMain->GetClanPtr(pAlliance->sSubAllianceKnights);


	if(pClan->GetID() == pAlliance->sMainAllianceKnights) // we are destroying the alliance.
	{
		int bResult = g_DBAgent.KnightsAllianceDestroy(allianceID);
		if (bResult > 0)
			return;

		std::string noticeText;
		g_pMain->GetServerResource(IDS_REMOVE_KNIGHTS_ALLIANCE, &noticeText, pClan->GetName().c_str());

		if(pMainClan != nullptr)
		{
			pMainClan->m_sAlliance = NO_CLAN;
			pMainClan->SendUpdate();
			pMainClan->SendChatAlliance("%s", noticeText.c_str());
		}
		if(pKnights1 != nullptr)
		{
			pKnights1->m_sAlliance = NO_CLAN;
			pKnights1->SendUpdate();
			pKnights1->SendChatAlliance("%s", noticeText.c_str());
		}
		if(pKnights2 != nullptr)
		{
			pKnights2->m_sAlliance = NO_CLAN;
			pKnights2->SendUpdate();
			pKnights2->SendChatAlliance("%s", noticeText.c_str());
		}
		if(pKnights3 != nullptr)
		{
			pKnights3->m_sAlliance = NO_CLAN;
			pKnights3->SendUpdate();
			pKnights3->SendChatAlliance("%s", noticeText.c_str());
		}

		g_pMain->m_KnightsAllianceArray.DeleteData(pAlliance->sMainAllianceKnights);

		Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_REMOVE));
		result << uint8(1) << pClan->GetAllianceID() << pClan->GetID() << uint16(-1);
		pClan->Send(&result);
		if(pUser->isInGame())
			pUser->Send(&result);

		return;
	}

	int bResult = g_DBAgent.KnightsAllianceRemove(pAlliance->sMainAllianceKnights, pClan->GetID());
	if (bResult > 0)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_REMOVE));
	result << uint8(1) << pClan->GetAllianceID() << pClan->GetID() << uint16(-1);
	pClan->Send(&result);
	if(pUser->isInGame())
		pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());

	if(pAlliance->sMercenaryClan_1 == pClan->GetID())
		pAlliance->sMercenaryClan_1 = NO_CLAN;
	if(pAlliance->sMercenaryClan_2 == pClan->GetID())
		pAlliance->sMercenaryClan_2 = NO_CLAN;
	if(pAlliance->sSubAllianceKnights == pClan->GetID())
		pAlliance->sSubAllianceKnights = NO_CLAN;

	if(pAlliance->sSubAllianceKnights == NO_CLAN
		&& pAlliance->sMercenaryClan_1 == NO_CLAN
		&& pAlliance->sMercenaryClan_2 == NO_CLAN)
	{
		int bResult = g_DBAgent.KnightsAllianceDestroy(allianceID);
		if (bResult > 0)
			return;

		std::string noticeText;
		g_pMain->GetServerResource(IDS_REMOVE_KNIGHTS_ALLIANCE, &noticeText, pClan->GetName().c_str());

		if(pMainClan != nullptr)
		{
			pMainClan->m_sAlliance = NO_CLAN;
			pMainClan->SendUpdate();
			pMainClan->SendChatAlliance("%s", noticeText.c_str());

			if(pClan->GetID() != pMainClan->GetID())
			{
				pClan->m_sAlliance = NO_CLAN;
				pClan->SendUpdate();
				pClan->SendChatAlliance("%s", noticeText.c_str());
			}
		}

		if(pUser->isInGame())
			pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());

		g_pMain->m_KnightsAllianceArray.DeleteData(pAlliance->sMainAllianceKnights);		
		return;
	}

	pClan->m_sAlliance = NO_CLAN;
	pClan->SendUpdate();
	std::string noticeText;
	g_pMain->GetServerResource(IDS_REMOVE_KNIGHTS_ALLIANCE, &noticeText, pClan->GetName().c_str());
	if(pMainClan != nullptr)
		pMainClan->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights1 != nullptr)
		pKnights1->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights2 != nullptr)
		pKnights2->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights3 != nullptr)
		pKnights3->SendChatAlliance("%s", noticeText.c_str());
}
#pragma endregion

#pragma region CKnightsManager::ReqKnightsAlliancePunish(CUser* pUser, Packet & pkt)
/**
* @brief	Leaving from an alliance. Note that this results in disbanding the
*			alliance if the main clan is leaving.
*
* @param	pUser	the user who wants to leave the alliance.
* @param	pkt		the packet.
*/
void CKnightsManager::ReqKnightsAlliancePunish(CUser* pUser, Packet & pkt)
{
	string requesterID;
	uint16 TargetClanID;
	pkt >> requesterID >> TargetClanID;

	if (pUser == nullptr || !pUser->isClanLeader() || pUser->GetName() != requesterID)
		return;

	CKnights * pMainClan = g_pMain->GetClanPtr(pUser->GetClanID());
	if(pMainClan == nullptr)
		return;

	CKnights * pTargetClan = g_pMain->GetClanPtr(TargetClanID);
	if(pTargetClan == nullptr)
		return;
	_KNIGHTS_ALLIANCE * pAlliance = g_pMain->GetAlliancePtr(pMainClan->GetID());

	if(pAlliance == nullptr
		|| !pMainClan->isAllianceLeader()
		|| pTargetClan->GetID() == pAlliance->sMainAllianceKnights
		|| (pTargetClan->GetID() != pAlliance->sMercenaryClan_1 
		&& pTargetClan->GetID() != pAlliance->sMercenaryClan_2
		&& pTargetClan->GetID() != pAlliance->sSubAllianceKnights))
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_ALLY_PUNISH));
	result << uint8(1) << pMainClan->GetID() << pTargetClan->GetID() << pMainClan->GetCapeID();

	CKnights * pKnights1 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_1),
		* pKnights2 = g_pMain->GetClanPtr(pAlliance->sMercenaryClan_2),
		* pKnights3 = g_pMain->GetClanPtr(pAlliance->sSubAllianceKnights);


	int bResult = g_DBAgent.KnightsAlliancePunish(pAlliance->sMainAllianceKnights, TargetClanID);
	if (bResult > 0)
		return;

	if (pAlliance->sSubAllianceKnights ==  pTargetClan->GetID())
		pAlliance->sSubAllianceKnights = NO_CLAN;
	else if (pAlliance->sMercenaryClan_1 ==  pTargetClan->GetID())
		pAlliance->sMercenaryClan_1 = NO_CLAN;
	else if (pAlliance->sMercenaryClan_2 ==  pTargetClan->GetID())
		pAlliance->sMercenaryClan_2 = NO_CLAN;

	if(pAlliance->sSubAllianceKnights == NO_CLAN
		&& pAlliance->sMercenaryClan_1 == NO_CLAN
		&& pAlliance->sMercenaryClan_2 == NO_CLAN)
	{
		int bResult = g_DBAgent.KnightsAllianceDestroy(pAlliance->sMainAllianceKnights);
		if (bResult > 0)
			return;
		std::string noticeText;
		g_pMain->GetServerResource(IDS_PUNISH_KNIGHTS_ALLIANCE, &noticeText, pTargetClan->GetName().c_str());
		if(pMainClan != nullptr)
		{
			pMainClan->m_sAlliance = NO_CLAN;
			pMainClan->SendUpdate();
			pMainClan->SendChatAlliance("%s", noticeText.c_str());
		}

		if(pTargetClan != nullptr && pTargetClan->GetID() != pMainClan->GetID())
		{
			pTargetClan->m_sAlliance = NO_CLAN;
			pTargetClan->SendUpdate();
			pTargetClan->SendChatAlliance("%s", noticeText.c_str());
		}

		if(pUser->isInGame())
			pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());

		g_pMain->m_KnightsAllianceArray.DeleteData(pAlliance->sMainAllianceKnights);
		return;
	}

	CUser * pTargetUser = g_pMain->GetUserPtr(pTargetClan->m_strChief, TYPE_CHARACTER);
	if(pUser->isInGame())
		pUser->SendToRegion(&result,nullptr,pUser->GetEventRoom());
	if(pTargetUser != nullptr && pTargetUser->isInGame())
		pTargetUser->SendToRegion(&result,nullptr,pTargetUser->GetEventRoom());

	pTargetClan->Send(&result);
	pTargetClan->m_sAlliance = NO_CLAN;
	pTargetClan->SendUpdate();

	std::string noticeText;
	g_pMain->GetServerResource(IDS_PUNISH_KNIGHTS_ALLIANCE, &noticeText, pTargetClan->GetName().c_str());
	if(pMainClan != nullptr)
		pMainClan->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights1 != nullptr)
		pKnights1->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights2 != nullptr)
		pKnights2->SendChatAlliance("%s", noticeText.c_str());
	if(pKnights3 != nullptr)
		pKnights3->SendChatAlliance("%s", noticeText.c_str());
}

#pragma endregion