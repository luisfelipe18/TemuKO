#include "stdafx.h"

#pragma region CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser)
void CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr)
			continue;

		if (pUser->isInGame() && (nation == 0 || nation == pUser->GetNation()))
			pUser->Send(pkt);
	}
}
#pragma endregion

#pragma region CGameServerDlg::UserInOutForMe(CUser *pSendUser)
void CGameServerDlg::UserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr || pSendUser->m_strUserID.empty())
		return;
	Packet result(WIZ_REQ_USERIN);

	C3DMap* pMap = pSendUser->GetMap();

	if (pMap == nullptr)
		return;

	uint16 user_count = 0;
	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z) {
		GetRegionUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());
		GetRegionBotIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());
	}

	result.put(0, uint16(user_count));
	pSendUser->SendCompressed(&result);
}
#pragma endregion

#pragma region CGameServerDlg::RegionUserInOutForMe(CUser *pSendUser)
void CGameServerDlg::RegionUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REGIONCHANGE, uint8(0));
	pSendUser->SendCompressed(&result);

	Packet result1(WIZ_REGIONCHANGE, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();

	if (pMap == nullptr)
		return;

	uint16 user_count = 0;
	result1 << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z) {
		GetRegionUserList(pMap, rx + x, rz + z, result1, user_count, pSendUser->GetEventRoom());
		GetRegionBotList(pMap, rx + x, rz + z, result1, user_count, pSendUser->GetEventRoom());
	}

	result1.put(1, user_count);
	pSendUser->SendCompressed(&result1);

	Packet result2(WIZ_REGIONCHANGE, uint8(2));
	pSendUser->SendCompressed(&result2);
}
#pragma endregion

#pragma region CGameServerDlg::GetRegionUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
void CGameServerDlg::GetRegionUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockUserArray.lock();
	if (pRegion->m_RegionUserArray.size() <= 0)
	{
		pRegion->m_lockUserArray.unlock();
		return;
	}

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	pRegion->m_lockUserArray.unlock();
	foreach(itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (pUser->m_bAbnormalType == ABNORMAL_INVISIBLE) // we should not see GMs if they "unview"
			continue;

		pkt << uint8(0) << pUser->GetSocketID();
		pUser->GetUserInfo(pkt);
		t_count++;
	}
}
#pragma endregion

#pragma region CGameServerDlg::GetRegionBotIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
void CGameServerDlg::GetRegionBotIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet& pkt, uint16& t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	CRegion* pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockBotArray.lock();
	if (pRegion->m_RegionBotArray.size() <= 0)
	{
		pRegion->m_lockBotArray.unlock();
		return;
	}

	ZoneBotArray cm_RegionUserBotArray = pRegion->m_RegionBotArray;
	pRegion->m_lockBotArray.unlock();

	foreach(itr, cm_RegionUserBotArray)
	{
		CBot* pUser = m_BotArray.GetData(*itr);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pkt << uint8(0) << pUser->GetID();
		pUser->GetUserInfo(pkt);
		t_count++;
	}
}
#pragma endregion

#pragma region  CGameServerDlg::GetRegionBotList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, bool SurUpdate /* = false */)
void CGameServerDlg::GetRegionBotList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet& pkt, uint16& t_count, uint16 nEventRoom, bool SurUpdate /* = false */)
{
	if (pMap == nullptr)
		return;

	CRegion* pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockBotArray.lock();
	if (pRegion->m_RegionBotArray.size() <= 0)
	{
		pRegion->m_lockBotArray.unlock();
		return;
	}

	ZoneBotArray cm_RegionUserBotArray = pRegion->m_RegionBotArray;
	pRegion->m_lockBotArray.unlock();

	foreach(itr, cm_RegionUserBotArray)
	{
		CBot* pUser = m_BotArray.GetData(*itr);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pkt << pUser->GetID();
		t_count++;
	}
}
#pragma endregion

#pragma region  CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, bool SurUpdate /* = false */)
void CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, bool SurUpdate /* = false */)
{
	if (pMap == nullptr)
		return;

	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockUserArray.lock();
	if (pRegion->m_RegionUserArray.size() <= 0)
	{
		pRegion->m_lockUserArray.unlock();
		return;
	}

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	pRegion->m_lockUserArray.unlock();
	foreach(itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (pUser->m_bAbnormalType == ABNORMAL_INVISIBLE) // we should not see GMs if they "unview"
			continue;

		pkt << pUser->GetSocketID();
		t_count++;
	}
}

#pragma endregion

#pragma region CGameServerDlg::MerchantUserInOutForMe(CUser *pSendUser)
void CGameServerDlg::MerchantUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_MERCHANT_INOUT, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	if (pMap == nullptr)
		return;
	uint16 user_count = 0;

	result << uint16(0); // placeholder for user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z) {
		GetRegionMerchantUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());
		GetRegionMerchantBotIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());
	}

	result.put(1, user_count);
	pSendUser->SendCompressed(&result);
}
#pragma endregion

#pragma region  CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
void CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockUserArray.lock();
	if (pRegion->m_RegionUserArray.size() <= 0)
	{
		pRegion->m_lockUserArray.unlock();
		return;
	}

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	pRegion->m_lockUserArray.unlock();
	foreach(itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (pUser->m_bAbnormalType == ABNORMAL_INVISIBLE)
			continue;

		pkt << pUser->GetSocketID()
			<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< (pUser->GetMerchantState() == 1 ? false : pUser->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->GetMerchantState() == 1 ? 4 : (pUser->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++)
			pkt << pUser->m_arMerchantItems[i].nNum;

		t_count++;
	}
}
#pragma endregion

#pragma region  CGameServerDlg::GetRegionMerchantBotIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
void CGameServerDlg::GetRegionMerchantBotIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet& pkt, uint16& t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	CRegion* pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockBotArray.lock();
	if (pRegion->m_RegionBotArray.size() <= 0)
	{
		pRegion->m_lockBotArray.unlock();
		return;
	}

	ZoneBotArray cm_RegionUserBotArray = pRegion->m_RegionBotArray;
	pRegion->m_lockBotArray.unlock();

	foreach(itr, cm_RegionUserBotArray)
	{
		CBot* pUser = g_pMain->m_BotArray.GetData(*itr);

		if (pUser == nullptr
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		pkt << pUser->GetID()
			<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< (pUser->GetMerchantState() == 1 ? false : pUser->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->GetMerchantState() == 1 ? 4 : (pUser->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++)
			pkt << pUser->m_arMerchantItems[i].nNum;
		t_count++;
	}
}
#pragma endregion

#pragma region CGameServerDlg::NpcInOutForMe(CUser* pSendUser)
void CGameServerDlg::NpcInOutForMe(CUser* pSendUser)
{
	if (pSendUser == nullptr)
		return;

	C3DMap* pMap = pSendUser->GetMap();
	if (pMap == nullptr)
		return;

	Packet result(WIZ_REQ_NPCIN);

	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcIn(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom(), pSendUser);

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}
#pragma endregion

#pragma region CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, CUser* pSendUser)
void CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, CUser* pSendUser)
{
	if (pMap == nullptr)
		return;

	uint16 sZoneID = pMap->GetID();

	CRegion * pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockNpcArray.lock();
	if (pRegion->m_RegionNpcArray.size() <= 0)
	{
		pRegion->m_lockNpcArray.unlock();
		return;
	}

	ZoneNpcArray cm_RegionNpcArray = pRegion->m_RegionNpcArray;
	CKnights *pKnights = g_pMain->GetClanPtr(pSendUser->GetClanID());
	_KNIGHTS_SIEGE_WARFARE *pSiegeWars = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	pRegion->m_lockNpcArray.unlock();

	foreach(itr, cm_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr, pMap->GetID());
		if (pNpc == nullptr
			|| pNpc->isDead())
			continue;

		if (nEventRoom >= 0 
			&& nEventRoom != pNpc->GetEventRoom())
			continue;

		if (pNpc->GetZoneID() == ZONE_DELOS)
		{
			if (pNpc->GetType() == NPC_DESTROYED_ARTIFACT 
				|| pNpc->isCswDoors())
			{
				CKnights* pKnights = g_pMain->GetClanPtr(pSendUser->GetClanID());
				_KNIGHTS_SIEGE_WARFARE *pSiegeWars = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
				if (pKnights != nullptr 
					&& pSiegeWars != nullptr)
				{
					if (pSiegeWars->sMasterKnights == pKnights->GetID())
						pNpc->m_bNation = 3;
					else
						pNpc->m_bNation = 0;
				}
				else
					pNpc->m_bNation = 0;
			}
		}

		pkt << pNpc->GetID();
		pNpc->GetNpcInfo(pkt);
		t_count++;
	}
}
#pragma endregion

#pragma region CGameServerDlg::RegionNpcInfoForMe(CUser *pSendUser)
void CGameServerDlg::RegionNpcInfoForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_NPC_REGION);
	C3DMap* pMap = pSendUser->GetMap();
	if (pMap == nullptr)
		return;
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcList(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom());

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}
#pragma endregion

#pragma region CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
void CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	CRegion * pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;
	pRegion->m_lockNpcArray.lock();
	if (pRegion->m_RegionNpcArray.size() <= 0)
	{
		pRegion->m_lockNpcArray.unlock();
		return;
	}

	ZoneNpcArray cm_RegionNpcArray = pRegion->m_RegionNpcArray;
	pRegion->m_lockNpcArray.unlock();
	foreach(itr, cm_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr, pMap->GetID());
		if (pNpc == nullptr || pNpc->isDead())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pNpc->GetEventRoom())
			continue;

		pkt << pNpc->GetID();
		t_count++;
	}
}
#pragma endregion

#pragma region CGameServerDlg::GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList)
void CGameServerDlg::GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList)
{
	if (pOwner == nullptr)
		return;

	C3DMap * pMap = pOwner->GetMap();
	if (pMap == nullptr)
		return;

	int16 rx = pOwner->GetRegionX(), rz = pOwner->GetRegionZ();

	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);
		if (pRegion == nullptr)
			continue;

		pRegion->m_lockNpcArray.lock();
		if (pRegion->m_RegionNpcArray.size() <= 0)
		{
			pRegion->m_lockNpcArray.unlock();
			continue;
		}

		ZoneNpcArray cm_RegionNpcArray = pRegion->m_RegionNpcArray;
		pRegion->m_lockNpcArray.unlock();
		foreach(itr, cm_RegionNpcArray)
		{
			CNpc *pNpc = GetNpcPtr(*itr, pMap->GetID());
			if (pNpc == nullptr || pNpc->isDead())
				continue;

			if (pOwner->GetEventRoom() >= 0 && pOwner->GetEventRoom() != pNpc->GetEventRoom())
				continue;

			pList->push_back(*itr);
		}
	}

	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);
		if (pRegion == nullptr)
			continue;

		// Add all potential users to list
		pRegion->m_lockUserArray.lock();
		if (pRegion->m_RegionUserArray.size() <= 0)
		{
			pRegion->m_lockUserArray.unlock();
			continue;
		}

		ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
		pRegion->m_lockUserArray.unlock();
		foreach(itr, cm_RegionUserArray)
		{
			CUser *pUser = GetUserPtr(*itr);
			if (pUser == nullptr
				|| !pUser->isInGame())
				continue;

			if (pOwner->GetEventRoom() >= 0 && pOwner->GetEventRoom() != pUser->GetEventRoom())
				continue;

			pList->push_back(*itr);

		}

	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom)
/**
* @brief	Sends a packet to all users in the zone matching the specified class types.
*
* @param	pkt				   	The packet.
* @param	bZoneID			   	Identifier for the zone.
* @param	pExceptUser		   	The except user.
* @param	nation			   	The nation.
* @param	seekingPartyOptions	Bitmask of classes to send to.
*/
void CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| pUser->isInParty()) // looking for users to join the party
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		// If we're in the neutral zone (Moradon), it doesn't matter which nation we party with.
		// For all other zones, we must party with a player of the same nation.
		if (pUser->isInMoradon()
			|| pUser->GetNation() == nation)
		{
			if (((seekingPartyOptions & 1) && pUser->JobGroupCheck(ClassWarrior))
				|| ((seekingPartyOptions & 2) && pUser->JobGroupCheck(ClassRogue))
				|| ((seekingPartyOptions & 4) && pUser->JobGroupCheck(ClassMage))
				|| ((seekingPartyOptions & 8) && pUser->JobGroupCheck(ClassPriest))
				|| ((seekingPartyOptions & 10) && pUser->JobGroupCheck(ClassPortuKurian)))
				pUser->Send(pkt);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_Player(CUser* pUser, Packet *pkt)

/**
* @brief	Sends a packet to all players in the specified zone
* 			matching the specified criteria.
*
* @param	pkt		   	The packet.
* @param	bZoneID	   	Zone's identifier.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL
* 						which will send to all/both nations.
* @param	nEventRoom  Room number to be allowed. If not specified, will default to 0 which is not event users.
*/
void CGameServerDlg::Send_Player(CUser* pUser, Packet* pkt)
{
	if (pUser == nullptr || !pUser->isInGame())
		return;

	pUser->Send(pkt);
}

#pragma endregion

#pragma region CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint16 nEventRoom /*= 0*/, float fRange /*= 0.0f*/)
void CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint16 nEventRoom /*= 0*/, float fRange /*= 0.0f*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (pUser == nullptr)
			continue;

		if (!pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
		{
			if (pExceptUser != nullptr)
			{
				if (pUser == pExceptUser
					|| (fRange > 0.0f && pUser->isInRange(pExceptUser, fRange)))
					continue;
			}
			continue;
		}

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}

}
#pragma endregion

#pragma region CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/,uint8 ZoneID /*= 0*/, bool isSendEventUsers /* false */, uint16 nEventRoom /*= -1*/)
/**
* @brief	Sends a packet to all users connected and logged into the server.
*
* @param	pkt		   	The packet.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/,
	uint8 ZoneID /*= 0*/, bool isSendEventUsers /* false */, uint16 nEventRoom /*= -1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame())
			continue;

		if (nation != Nation::NONE
			&& nation != Nation::ALL)
		{
			if (nation != pUser->GetNation())
				continue;
		}

		if (ZoneID != 0 && pUser->GetZoneID() != ZoneID)
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (isSendEventUsers && !pUser->isEventUser())
			continue;

		pUser->Send(pkt);
	}
}
#pragma endregion

#pragma region  CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
void CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	foreach_region(rx, rz)
		Send_UnitRegion(pkt, pMap, rx + x, rz + z, pExceptUser, nEventRoom);
}
#pragma endregion

#pragma region CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, uint16 nEventRoom)
void CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr || pMap->m_nZoneNumber == 0)
		return;

	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockUserArray.lock();
	if (pRegion->m_RegionUserArray.size() <= 0)
	{
		pRegion->m_lockUserArray.unlock();
		return;
	}

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	pRegion->m_lockUserArray.unlock();
	foreach(itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
// TODO: Move the following two methods into a base CUser/CNpc class
void CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (old_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z - 1, pExceptUser, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z, pExceptUser, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z + 1, pExceptUser, nEventRoom);
	}

	if (old_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x + old_x, z + old_z * 2, pExceptUser, nEventRoom);
		if (old_x < 0)
			Send_UnitRegion(pkt, pMap, x + old_x + 1, z + old_z * 2, pExceptUser, nEventRoom);
		else if (old_x > 0)
			Send_UnitRegion(pkt, pMap, x + old_x - 1, z + old_z * 2, pExceptUser, nEventRoom);
		else
		{
			Send_UnitRegion(pkt, pMap, x + old_x - 1, z + old_z * 2, pExceptUser, nEventRoom);
			Send_UnitRegion(pkt, pMap, x + old_x + 1, z + old_z * 2, pExceptUser, nEventRoom);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
void CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (new_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x + new_x, z - 1, pExceptUser, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + new_x, z, pExceptUser, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + new_x, z + 1, pExceptUser, nEventRoom);
	}

	if (new_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x, z + new_z, pExceptUser, nEventRoom);

		if (new_x < 0)
			Send_UnitRegion(pkt, pMap, x + 1, z + new_z, pExceptUser, nEventRoom);
		else if (new_x > 0)
			Send_UnitRegion(pkt, pMap, x - 1, z + new_z, pExceptUser, nEventRoom);
		else
		{
			Send_UnitRegion(pkt, pMap, x - 1, z + new_z, pExceptUser, nEventRoom);
			Send_UnitRegion(pkt, pMap, x + 1, z + new_z, pExceptUser, nEventRoom);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, uint16 nEventRoom)
void CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, uint16 nEventRoom)
{
	int left_border = region_x * VIEW_DISTANCE, top_border = region_z * VIEW_DISTANCE;
	Send_FilterUnitRegion(pkt, pMap, region_x, region_z, curx, curz, pExceptUser, nEventRoom);
	if (((curx - left_border) > (VIEW_DISTANCE / 2.0f)))
	{			// RIGHT
		if (((curz - top_border) > (VIEW_DISTANCE / 2.0f)))
		{	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z + 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z + 1, curx, curz, pExceptUser, nEventRoom);
		}
		else
		{													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z - 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z - 1, curx, curz, pExceptUser, nEventRoom);
		}
	}
	else
	{														// LEFT
		if (((curz - top_border) > (VIEW_DISTANCE / 2.0f)))
		{	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z + 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z + 1, curx, curz, pExceptUser, nEventRoom);
		}
		else
		{													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z - 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z - 1, curx, curz, pExceptUser, nEventRoom);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, uint16 nEventRoom)
void CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr || pMap->m_nZoneNumber == 0)
		return;

	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	pRegion->m_lockUserArray.lock();
	if (pRegion->m_RegionUserArray.size() <= 0)
	{
		pRegion->m_lockUserArray.unlock();
		return;
	}

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	pRegion->m_lockUserArray.unlock();
	foreach(itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame())
			continue;

		if (nEventRoom >= 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (sqrt(pow((pUser->m_curx - ref_x), 2) + pow((pUser->m_curz - ref_z), 2)) < 32)
			pUser->Send(pkt);
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_PartyMember(int party, Packet *result)
void CGameServerDlg::Send_PartyMember(int party, Packet *result)
{
	_PARTY_GROUP* pParty = GetPartyPtr(party);
	if (pParty == nullptr)
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser *pUser = GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;

		pUser->Send(result);
	}
}
#pragma endregion

#pragma region CGameServerDlg::Send_KnightsMember(int index, Packet *pkt)
void CGameServerDlg::Send_KnightsMember(int index, Packet *pkt)
{
	CKnights* pKnights = GetClanPtr(index);
	if (pKnights == nullptr)
		return;

	pKnights->Send(pkt);
}
#pragma endregion

#pragma region CGameServerDlg::Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt)
void CGameServerDlg::Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt)
{
	_KNIGHTS_ALLIANCE* pAlliance = GetAlliancePtr(sAllianceID);
	if (pAlliance == nullptr)
		return;

	Send_KnightsMember(pAlliance->sMainAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sSubAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_1, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_2, pkt);
}
#pragma endregion

#pragma region CGameServerDlg::Send_Noah_Knights(Packet *pkt)
/**
* @brief Sends Noah Chat Packets to all Noah Knights.
*/
void CGameServerDlg::Send_Noah_Knights(Packet *pkt)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (pUser == nullptr ||
			pUser->GetLevel() > 50)
			continue;

		pUser->Send(pkt);
	}
}
#pragma endregion