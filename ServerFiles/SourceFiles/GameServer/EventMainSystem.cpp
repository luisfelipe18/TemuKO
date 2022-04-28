#include "stdafx.h"

#pragma region CGameServerDlg::TempleEventManageRoom()
/**
* @brief	Sets the users room before the event starts
*/
void CGameServerDlg::TempleEventManageRoom()
{
	uint8 nMaxUserCount = 0;

	switch (g_pMain->pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
#pragma region BDW Manage Room
	{
		std::queue<std::string> v_elmoradContainer;	// the continer for elmorad players other than priest class
		std::queue<std::string> v_elmoradPriestContainer; // the continer for elmorad priests

		std::queue<std::string> v_karusContainer; // the continer for karus players other than priest class
		std::queue<std::string> v_karusPriestContainer;	// the continer for karus priests

		foreach_stlmap(itr, m_TempleEventUserMap)
		{
			_TEMPLE_EVENT_USER* pEventUser = itr->second;
			if (pEventUser == nullptr)
				continue;

			CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
			if (pUser == nullptr
				|| !pUser->isInGame()
				|| !pUser->isEventUser())
				continue;

			std::string strUserID = pUser->GetName();
			if (pUser->JobGroupCheck(GROUP_WARRIOR)
				|| pUser->JobGroupCheck(GROUP_ROGUE)
				|| pUser->JobGroupCheck(GROUP_MAGE)
				|| pUser->JobGroupCheck(GROUP_PORTU_KURIAN))
				pUser->GetNation() == Nation::KARUS ? v_karusContainer.push(strUserID) : v_elmoradContainer.push(strUserID);
			else if (pUser->JobGroupCheck(GROUP_CLERIC))
				pUser->GetNation() == Nation::KARUS ? v_karusPriestContainer.push(strUserID) : v_elmoradPriestContainer.push(strUserID);
		}

		//containers for all classes. this might be turned into more generic process otherway.
		uint16 nElmoGeneCount = (uint16)v_elmoradContainer.size();
		uint16 nElmoPriestCount = (uint16)v_elmoradPriestContainer.size();

		uint16 nKarusGeneCount = (uint16)v_karusContainer.size();
		uint16 nKarusPriestCount = (uint16)v_karusPriestContainer.size();

		uint16 nElmoEventUserCount = nElmoGeneCount + nElmoPriestCount;
		uint16 nKarusEventUserCount = nKarusGeneCount + nKarusPriestCount;
		int nElmoPartyCount = (ceil(nElmoEventUserCount / 8) == 0 ? 1 : int(ceil(nElmoEventUserCount / 8)));
		int nKarusPartyCount = (ceil(nKarusEventUserCount / 8) == 0 ? 1 : int(ceil(nKarusEventUserCount / 8)));
		int nKarusPriestPerParty = (ceil(nKarusPriestCount / nKarusPartyCount) == 0 ? 1 : int(ceil(nKarusPriestCount / nKarusPartyCount)));
		int nElmoPriestPerParty = (ceil(nElmoPriestCount / nElmoPartyCount) == 0 ? 1 : int(ceil(nElmoPriestCount / nElmoPartyCount)));
		for (int bRoom = 1; bRoom <= MAX_TEMPLE_EVENT_ROOM; bRoom++)
		{
			_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(bRoom);
			if (pRoomInfo == nullptr)
				continue;

			for (int x = 0; x < nElmoPriestPerParty; x++)
			{
				if (v_elmoradPriestContainer.empty()
					|| pRoomInfo->m_ElmoradUserList.GetSize() >= 8)
					break;

				std::string strUserID = v_elmoradPriestContainer.front();
				v_elmoradPriestContainer.pop();

				CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				pEventUser->isLoqOut = false;
				if (!pRoomInfo->m_ElmoradUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = pRoomInfo->m_ElmoradUserList.GetSize(); x < 8; x++)
			{
				if (v_elmoradContainer.empty())
					break;

				std::string strUserID = v_elmoradContainer.front();
				v_elmoradContainer.pop();

				CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				pEventUser->isLoqOut = false;
				if (!pRoomInfo->m_ElmoradUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = 0; x < nKarusPriestPerParty; x++)
			{
				if (v_karusPriestContainer.empty()
					|| pRoomInfo->m_KarusUserList.GetSize() >= 8)
					break;

				std::string strUserID = v_karusPriestContainer.front();
				v_karusPriestContainer.pop();

				CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				pEventUser->isLoqOut = false;
				if (!pRoomInfo->m_KarusUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = pRoomInfo->m_KarusUserList.GetSize(); x < 8; x++)
			{
				if (v_karusContainer.empty())
					break;

				std::string strUserID = v_karusContainer.front();
				v_karusContainer.pop();

				CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				pEventUser->isLoqOut = false;
				if (!pRoomInfo->m_KarusUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}
		}
	}
#pragma endregion
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
#pragma region Juraid Manage Room
	{
		std::queue<std::string> v_elmoradContainer;	// the continer for elmorad players other than priest class
		std::queue<std::string> v_elmoradPriestContainer;		// the continer for elmorad priests

		std::queue<std::string> v_karusContainer;	// the continer for karus players other than priest class
		std::queue<std::string> v_karusPriestContainer;		// the continer for karus priests

		foreach_stlmap(itr, m_TempleEventUserMap)
		{
			_TEMPLE_EVENT_USER* pEventUser = itr->second;
			if (pEventUser == nullptr)
				continue;

			CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
			if (pUser == nullptr
				|| !pUser->isInGame()
				|| !pUser->isEventUser())
				continue;

			std::string strUserID = pUser->GetName();
			if (pUser->JobGroupCheck(GROUP_WARRIOR)
				|| pUser->JobGroupCheck(GROUP_ROGUE)
				|| pUser->JobGroupCheck(GROUP_MAGE)
				|| pUser->JobGroupCheck(GROUP_PORTU_KURIAN))
				pUser->GetNation() == Nation::KARUS ? v_karusContainer.push(strUserID) : v_elmoradContainer.push(strUserID);
			else if (pUser->JobGroupCheck(GROUP_CLERIC))
				pUser->GetNation() == Nation::KARUS ? v_karusPriestContainer.push(strUserID) : v_elmoradPriestContainer.push(strUserID);
		}

		//containers for all classes. this might be turned into more generic process otherway.
		uint16 nElmoGeneCount = (uint16)v_elmoradContainer.size();
		uint16 nElmoPriestCount = (uint16)v_elmoradPriestContainer.size();

		uint16 nKarusGeneCount = (uint16)v_karusContainer.size();
		uint16 nKarusPriestCount = (uint16)v_karusPriestContainer.size();

		uint16 nElmoEventUserCount = nElmoGeneCount + nElmoPriestCount;
		uint16 nKarusEventUserCount = nKarusGeneCount + nKarusPriestCount;
		int nElmoPartyCount = (ceil(nElmoEventUserCount / 8) == 0 ? 1 : int(ceil(nElmoEventUserCount / 8)));
		int nKarusPartyCount = (ceil(nKarusEventUserCount / 8) == 0 ? 1 : int(ceil(nKarusEventUserCount / 8)));
		int nKarusPriestPerParty = (ceil(nKarusPriestCount / nKarusPartyCount) == 0 ? 1 : int(ceil(nKarusPriestCount / nKarusPartyCount)));
		int nElmoPriestPerParty = (ceil(nElmoPriestCount / nElmoPartyCount) == 0 ? 1 : int(ceil(nElmoPriestCount / nElmoPartyCount)));
		for (int bRoom = 1; bRoom <= MAX_TEMPLE_EVENT_ROOM; bRoom++)
		{
			_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(bRoom);
			if (pRoomInfo == nullptr)
				continue;

			for (int x = 0; x < nElmoPriestPerParty; x++)
			{
				if (v_elmoradPriestContainer.empty()
					|| pRoomInfo->m_ElmoradUserList.GetSize() >= 8)
					break;

				std::string strUserID = v_elmoradPriestContainer.front();
				v_elmoradPriestContainer.pop();

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				if (!pRoomInfo->m_ElmoradUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = pRoomInfo->m_ElmoradUserList.GetSize(); x < 8; x++)
			{
				if (v_elmoradContainer.empty())
					break;

				std::string strUserID = v_elmoradContainer.front();
				v_elmoradContainer.pop();

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				if (!pRoomInfo->m_ElmoradUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = 0; x < nKarusPriestPerParty; x++)
			{
				if (v_karusPriestContainer.empty()
					|| pRoomInfo->m_KarusUserList.GetSize() >= 8)
					break;

				std::string strUserID = v_karusPriestContainer.front();
				v_karusPriestContainer.pop();

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				if (!pRoomInfo->m_KarusUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}

			for (int x = pRoomInfo->m_KarusUserList.GetSize(); x < 8; x++)
			{
				if (v_karusContainer.empty())
					break;

				std::string strUserID = v_karusContainer.front();
				v_karusContainer.pop();

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				if (!pRoomInfo->m_KarusUserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}
		}
	}
#pragma endregion
	break;
	case TEMPLE_EVENT_CHAOS:
#pragma region Chaos Manage Room

	{
		nMaxUserCount = 18; // max 18 users
		std::queue<std::string> v_userContainer;		// the continer for all users

		foreach_stlmap(itr, m_TempleEventUserMap)
		{
			_TEMPLE_EVENT_USER* pEventUser = itr->second;
			if (pEventUser == nullptr)
				continue;

			std::string strUserID = pEventUser->strUserID;
			v_userContainer.push(strUserID);
		}

		for (int bRoom = 1; bRoom <= MAX_TEMPLE_EVENT_ROOM; bRoom++)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventChaosRoomList.GetData(bRoom);
			if (pRoomInfo == nullptr)
				continue;

			int counter = 0;
			for (int x = 0; x < nMaxUserCount; x++)
			{
				if (v_userContainer.empty())
					break;

				std::string strUserID = v_userContainer.front();
				v_userContainer.pop();

				_TEMPLE_STARTED_EVENT_USER* pEventUser = new  _TEMPLE_STARTED_EVENT_USER;
				pEventUser->strUserID = strUserID;
				pEventUser->isPrizeGiven = false;
				if (!pRoomInfo->m_UserList.PutData(pEventUser->strUserID, pEventUser))
				{
					delete pEventUser;
					continue;
				}
			}
		}
	}
#pragma endregion
	break;
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventSendAccept()

/**
* @brief	Sends the temple users whether they are accepted for the event.
*/
void CGameServerDlg::TempleEventSendAccept()
{
	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT_COUNTER));
	result << uint16(pTempleEvent.ActiveEvent) << (uint16)pTempleEvent.AllUserCount << uint16(0);

	foreach_stlmap(itr, m_TempleEventUserMap)
	{
		_TEMPLE_EVENT_USER* pEventUser = itr->second;
		if (pEventUser == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		pUser->Send(&result); // initiate screen
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventScreen(uint8 ActiveEvent, uint16 sTime)

/**
* @brief	Sends the event screen packet to the all event players
*			at the beginning.
*/
void CGameServerDlg::TempleEventScreen(uint8 ActiveEvent, uint16 sTime)
{
	Packet Inventory(WIZ_EVENT, uint8(1));	// Inventory Control.

	Packet aresult(WIZ_SELECT_MSG);
	aresult << uint16(0) << uint8(7) << uint64(0);

	if (ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
		aresult << uint32(8) << uint8(7) << uint32(sTime); /* Time */
	else if (ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
		aresult << uint32(6) << uint8(11) << uint32(sTime);/* Time */
	else if (ActiveEvent == TEMPLE_EVENT_CHAOS)
		aresult << uint32(9) << uint8(24) << uint32(sTime);/* Time */

	Packet bresult(WIZ_BIFROST);
	bresult << uint8(5) << uint16(sTime);/* BIFROST */

	switch (ActiveEvent)
	{
	case TEMPLE_EVENT_CHAOS:
	{
		foreach_stlmap(itr, m_TempleEventChaosRoomList)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_UserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_UserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->Send(&aresult);
				pUser->Send(&bresult);

			}
			pRoomInfo->m_UserList.m_lock.unlock();
		}
	}
	break;

	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		foreach_stlmap(itr, m_TempleEventBDWRoomList)
		{
			_BDW_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->Send(&Inventory);
				pUser->Send(&aresult);
				pUser->Send(&bresult);
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->Send(&Inventory);
				pUser->Send(&aresult);
				pUser->Send(&bresult);
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;

	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		foreach_stlmap(itr, m_TempleEventJuraidRoomList)
		{
			_JURAID_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->Send(&aresult);
				pUser->Send(&bresult);
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->Send(&aresult);
				pUser->Send(&bresult);
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;

	default:
		break;
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventBridgeCheck()

/**
* @brief	Checks whether any timer  up for the countdown of the monument holder.
*			If so, ends the event for this room.
*/

void CGameServerDlg::TempleEventBridgeCheck(uint8 DoorNumber)
{
	if (isJuraidMountainActive())
	{
		Packet result;

		switch (DoorNumber)
		{
		case 0:
		{
			for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
			{
				_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(i);
				if (pRoomInfo == nullptr
					|| pRoomInfo->m_bFinished == true)
					continue;

				if (!pRoomInfo->m_sElmoBridgesTimerOpen[0])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sElmoBridges[0], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sElmoBridgesTimerOpen[0] = true;
						}
					}
				}

				if (!pRoomInfo->m_sKarusBridgesTimerOpen[0])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sKarusBridges[0], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sKarusBridgesTimerOpen[0] = true;
						}
					}
				}
			}
		}
		break;
		case 1:
		{
			for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
			{
				_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(i);
				if (pRoomInfo == nullptr
					|| pRoomInfo->m_bFinished == true)
					continue;

				if (!pRoomInfo->m_sElmoBridgesTimerOpen[1])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sElmoBridges[1], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sElmoBridgesTimerOpen[1] = true;
						}
					}
				}

				if (!pRoomInfo->m_sKarusBridgesTimerOpen[1])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sKarusBridges[1], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sKarusBridgesTimerOpen[1] = true;
						}
					}
				}
			}
		}
		break;
		case 2:
		{
			for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
			{
				_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(i);
				if (pRoomInfo == nullptr
					|| pRoomInfo->m_bFinished == true)
					continue;

				if (!pRoomInfo->m_sElmoBridgesTimerOpen[2])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sElmoBridges[2], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sElmoBridgesTimerOpen[2] = true;
						}
					}
				}

				if (!pRoomInfo->m_sKarusBridgesTimerOpen[2])
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sKarusBridges[2], ZONE_JURAID_MOUNTAIN);
					if (pNpc != nullptr)
					{
						if (pNpc->m_byGateOpen != 2)
						{
							pNpc->m_byGateOpen = 2;

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_OUT) << pNpc->GetID();
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							result.clear();
							result.Initialize(WIZ_NPC_INOUT);
							result << uint8(INOUT_IN) << pNpc->GetID();
							pNpc->GetNpcInfo(result);
							g_pMain->Send_All(&result, nullptr, pNpc->GetNation(), ZONE_JURAID_MOUNTAIN, true, pNpc->GetEventRoom());

							pRoomInfo->m_sKarusBridgesTimerOpen[2] = true;
						}
					}
				}
			}
		}
		break;
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventRoomClose()

/**
* @brief	Checks whether any timer  up for the countdown of the monument holder.
*			If so, ends the event for this room.
*/
void CGameServerDlg::TempleEventRoomClose()
{
	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_BDW_ROOM_INFO* pRoomInfo = m_TempleEventBDWRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| !pRoomInfo->m_FinishPacketControl)
				continue;

			if (pRoomInfo->m_tFinishTimeCounter > 0)
			{
				if (pRoomInfo->m_tFinishTimeCounter <= UNIXTIME)
				{
					TempleEventFinish(i);
					pRoomInfo->m_bFinished = true;
				}
			}
		}
	}
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_JURAID_ROOM_INFO* pRoomInfo = m_TempleEventJuraidRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| !pRoomInfo->m_FinishPacketControl)
				continue;

			if (pRoomInfo->m_tFinishTimeCounter > 0)
			{
				if (pRoomInfo->m_tFinishTimeCounter <= UNIXTIME)
				{
					TempleEventFinish(i);
					pRoomInfo->m_bFinished = true;
				}
			}
		}
	}
	break;
	case TEMPLE_EVENT_CHAOS:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = m_TempleEventChaosRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| !pRoomInfo->m_FinishPacketControl)
				continue;

			if (pRoomInfo->m_tFinishTimeCounter > 0)
			{
				if (pRoomInfo->m_tFinishTimeCounter <= UNIXTIME)
				{
					TempleEventFinish(i);
					pRoomInfo->m_bFinished = true;
				}
			}
		}
	}
	break;
	}
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventSendWinnerScreen()

/**
* @brief	Checks whether any timer  up for the countdown of the monument holder.
*			If so, ends the event for this room.
*/
void CGameServerDlg::TempleEventSendWinnerScreen()
{
	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_BDW_ROOM_INFO* pRoomInfo = m_TempleEventBDWRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| pRoomInfo->m_FinishPacketControl)
				continue;

			g_pMain->pTempleEvent.EventCloseMainControl = true; g_pMain->pTempleEvent.isAttackable = false;
			pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;
			pRoomInfo->m_tAltarSpawn = false; pRoomInfo->m_tAltarSpawnTimed = 0; pRoomInfo->m_bWinnerNation = 0;

			if (pRoomInfo->m_KarusScoreBoard > pRoomInfo->m_ElmoScoreBoard)
				pRoomInfo->m_bWinnerNation = KARUS;
			else if (pRoomInfo->m_ElmoScoreBoard > pRoomInfo->m_KarusScoreBoard)
				pRoomInfo->m_bWinnerNation = ELMORAD;
			else
				pRoomInfo->m_bWinnerNation = 0;

			Packet pkt1(WIZ_EVENT);
			pkt1 << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_BORDER_DEFENCE_WAR) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);
			Send_All(&pkt1, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, i);
		}
	}
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_JURAID_ROOM_INFO* pRoomInfo = m_TempleEventJuraidRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| pRoomInfo->m_FinishPacketControl)
				continue;

			g_pMain->pTempleEvent.EventCloseMainControl = true; g_pMain->pTempleEvent.isAttackable = false;
			pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;

			if (pRoomInfo->m_iElmoradKillCount > pRoomInfo->m_iKarusKillCount)
				pRoomInfo->m_bWinnerNation = ELMORAD;
			else if (pRoomInfo->m_iKarusKillCount > pRoomInfo->m_iElmoradKillCount)
				pRoomInfo->m_bWinnerNation = KARUS;
			else
				pRoomInfo->m_bWinnerNation = 0;

			/*Packet pkt1(WIZ_EVENT);
			pkt1 << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_JURAD_MOUNTAIN) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);*/
			Packet pkt1(WIZ_SELECT_MSG);
			pkt1 << uint16(0) << uint8(7) << uint64(0) << uint32(7)
				<< ((pRoomInfo->m_bWinnerNation == Nation::KARUS) ? uint32(1) : uint32(0))
				<< ((pRoomInfo->m_bWinnerNation == Nation::ELMORAD) ? uint32(1) : uint32(0))
				<< uint32(20) << uint32(20) << uint32(0);
			Send_All(&pkt1, nullptr, Nation::ALL, ZONE_JURAID_MOUNTAIN, true, i);
		}
	}
	break;
	case TEMPLE_EVENT_CHAOS:
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = m_TempleEventChaosRoomList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_bFinished
				|| pRoomInfo->m_FinishPacketControl)
				continue;

			g_pMain->pTempleEvent.EventCloseMainControl = true; g_pMain->pTempleEvent.isAttackable = false;
			pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;

			Packet pkt1(WIZ_EVENT);
			pkt1 << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_CHAOS) << uint8(104) << uint16(20) << uint16(0);
			Send_All(&pkt1, nullptr, Nation::ALL, ZONE_CHAOS_DUNGEON, true, i);
		}
		TempleEventChaosShowCounterScreen();
	}
	break;
	}
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventStart()

/**
* @brief Sends the initial invitation screen to the all players.
*/
void CGameServerDlg::TempleEventStart()
{
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR
		|| pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser* pUser = TO_USER(itr->second);
			if (pUser == nullptr
				|| !pUser->isInGame())
				continue;

			Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
			result << (uint16)pTempleEvent.ActiveEvent << m_nVirtualEventRoomRemainSeconds;
			pUser->Send(&result);
		}
	}
}

#pragma endregion

#pragma region CUser::EventPartyCreate()
void CUser::EventPartyCreate()
{
	m_sUserPartyType = 0;
	m_bPartyLeader = true;
	StateChangeServerDirect(6, 1);
}
#pragma endregion

#pragma region CUser::EventPartyInvitationCheck(uint16 EnterPartyGetID)
void CUser::EventPartyInvitationCheck(uint16 EnterPartyGetID)
{
	Packet party(WIZ_PARTY);
	CUser* pEnterPartyUser = g_pMain->GetUserPtr(EnterPartyGetID);
	_PARTY_GROUP* pParty = nullptr;
	int16 PartyMemberCount = 0;

	if (pEnterPartyUser == nullptr
		|| pEnterPartyUser == this
		|| pEnterPartyUser->isInParty()
		|| pEnterPartyUser->isInApprovedParty()
		|| !pEnterPartyUser->isInGame())
		return;

	if (GetNation() != pEnterPartyUser->GetNation())
		return;

	pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	for (PartyMemberCount = 0; PartyMemberCount < MAX_PARTY_USERS; PartyMemberCount++)
	{
		if (pParty->uid[PartyMemberCount] < 0)
			break;
	}

	if (PartyMemberCount == MAX_PARTY_USERS)
		return;

	pEnterPartyUser->m_sPartyIndex = m_sPartyIndex;
	pEnterPartyUser->m_bInParty = true;
	pEnterPartyUser->m_sUserPartyType = 0;
	pEnterPartyUser->EventPartyInvitation();
}
#pragma endregion

#pragma region CUser::EventPartyInvitation()
void CUser::EventPartyInvitation()
{
	Packet result(WIZ_PARTY);
	_PARTY_GROUP* pParty = nullptr;
	uint8 byIndex = 0xFF;
	int leader_id = -1;

	if (!isInApprovedParty())
		return;

	pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		m_bInParty = false;
		m_bInEnterParty = false;
		m_sUserPartyType = 0;
		m_sPartyIndex = -1;
		return;
	}

	CUser* pLeader = g_pMain->GetUserPtr(pParty->uid[0]);
	if (pLeader == nullptr || !pLeader->isInGame())
		return;

	if (pLeader->GetZoneID() != GetZoneID()
		|| GetPartyMemberAmount(pParty) == MAX_PARTY_USERS)
	{
		DoNotAcceptJoiningTheParty();
		return;
	}

	// make sure user isn't already in the array...
	// kind of slow, but it works for the moment
	foreach_array(i, pParty->uid)
	{
		if (pParty->uid[i] == GetSocketID())
		{
			m_bInParty = false;
			m_bInEnterParty = false;
			m_sUserPartyType = 0;
			m_sPartyIndex = -1;
			pParty->uid[i] = -1;
			return;
		}
	}

	int PartyNemberCount = 0;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		if (pParty->uid[i] >= 0)
			PartyNemberCount++;
	}

	if (PartyNemberCount == 1)
		pLeader->m_bInEnterParty = pLeader->m_bInParty = true;

	// Send the player who just joined the existing party list
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		// No player set?
		if (pParty->uid[i] < 0)
		{
			// If we're not in the list yet, add us.
			if (byIndex == 0xFF)
			{
				pParty->uid[i] = GetSocketID();
				byIndex = i;
			}
			continue;
		}

		// For everyone else, 
		CUser* pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr || !pUser->isInGame())
			continue;

		result.clear();
		result << uint8(PARTY_INSERT) << pParty->uid[i]
			<< uint8(1) // success
			<< pUser->GetName()
			<< pUser->m_MaxHp << pUser->m_sHp
			<< pUser->GetLevel() << pUser->m_sClass
			<< pUser->m_MaxMp << pUser->m_sMp
			<< pUser->GetNation()
			<< pUser->m_sUserPartyType;
		Send(&result);
	}

	if (pLeader->m_bNeedParty == 2 || m_bNeedParty == 2)
	{
		if (pLeader->m_bNeedParty == 2)
		{
			g_pMain->m_SeekingPartyArrayLock.lock();
			// You don't need anymore seek
			foreach(itr, g_pMain->m_SeekingPartyArray)
			{
				if ((*itr) == nullptr)
					continue;

				if ((*itr)->m_sGetID == pLeader->GetID())
				{
					g_pMain->m_SeekingPartyArray.erase(itr);
					break;
				}
			}
			g_pMain->m_SeekingPartyArrayLock.unlock();
		}
		if (m_bNeedParty == 2)
		{
			g_pMain->m_SeekingPartyArrayLock.lock();
			// You don't need anymore seek
			foreach(itr, g_pMain->m_SeekingPartyArray)
			{
				if ((*itr) == nullptr)
					continue;

				if ((*itr)->m_sGetID == GetID())
				{
					g_pMain->m_SeekingPartyArray.erase(itr);
					break;
				}
			}
			g_pMain->m_SeekingPartyArrayLock.unlock();
		}
	}

	m_bInEnterParty = m_bInParty = true;
	pLeader->m_bInEnterParty = pLeader->m_bInParty = true;

	if (pLeader->m_bNeedParty == 2 && pLeader->isInParty())
		pLeader->StateChangeServerDirect(2, 1);

	if (m_bNeedParty == 2 && isInParty())
		StateChangeServerDirect(2, 1);

	result.clear();
	result << uint8(PARTY_INSERT) << GetSocketID()
		<< uint8(1)
		<< GetName()
		<< m_MaxHp << m_sHp
		<< GetLevel() << GetClass()
		<< m_MaxMp << m_sMp
		<< GetNation()
		<< m_sUserPartyType;
	g_pMain->Send_PartyMember(GetPartyID(), &result);
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventCreateParties()
void CGameServerDlg::TempleEventCreateParties()
{
	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		foreach_stlmap(itr, g_pMain->m_TempleEventBDWRoomList)
		{
			_BDW_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			CUser* pKarusLeader = nullptr;
			CUser* pElmoLeader = nullptr;
			_PARTY_GROUP* pParty = nullptr;
			_PARTY_GROUP* pParty2 = nullptr;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->isInParty())
					continue;

				if (pRoomInfo->GetRoomKarusUserCount() > 1)
				{
					if (pKarusLeader == nullptr)
					{
						pParty = g_pMain->CreateParty(pUser);
						if (pParty == nullptr) // something wrong has gone here, so do not create a party let the users do
							break;

						pKarusLeader = pUser;
						pKarusLeader->EventPartyCreate();
					}
					else
					{
						pKarusLeader->EventPartyInvitationCheck(pUser->GetSocketID());
					}
				}
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->isInParty())
					continue;

				if (pRoomInfo->GetRoomElmoradUserCount() > 1)
				{
					if (pElmoLeader == nullptr)
					{
						pParty2 = g_pMain->CreateParty(pUser);
						if (pParty2 == nullptr) // something wrong has gone here, so do not create a party let the users do
							break;

						pElmoLeader = pUser;
						pUser->EventPartyCreate();
					}
					else
					{
						pElmoLeader->EventPartyInvitationCheck(pUser->GetSocketID());
					}
				}
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		foreach_stlmap(itr, g_pMain->m_TempleEventJuraidRoomList)
		{
			_JURAID_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			CUser* pKarusLeader = nullptr;
			CUser* pElmoLeader = nullptr;
			_PARTY_GROUP* pParty = nullptr;
			_PARTY_GROUP* pParty2 = nullptr;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->isInParty())
					continue;

				if (pRoomInfo->GetRoomKarusUserCount() > 1)
				{
					if (pKarusLeader == nullptr)
					{
						pParty = g_pMain->CreateParty(pUser);
						if (pParty == nullptr) // something wrong has gone here, so do not create a party let the users do
							break;

						pKarusLeader = pUser;
						pKarusLeader->EventPartyCreate();
					}
					else
					{
						pKarusLeader->EventPartyInvitationCheck(pUser->GetSocketID());
					}
				}
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->isInParty())
					continue;

				if (pRoomInfo->GetRoomElmoradUserCount() > 1)
				{
					if (pElmoLeader == nullptr)
					{
						pParty2 = g_pMain->CreateParty(pUser);
						if (pParty2 == nullptr) // something wrong has gone here, so do not create a party let the users do
							break;

						pElmoLeader = pUser;
						pElmoLeader->EventPartyCreate();
					}
					else
					{
						pElmoLeader->EventPartyInvitationCheck(pUser->GetSocketID());
					}
				}
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;
	default:
		break;
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventTeleportUsers()

void CGameServerDlg::TempleEventTeleportUsers()
{
	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		foreach_stlmap(itr, g_pMain->m_TempleEventBDWRoomList)
		{
			_BDW_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->UserInOut(INOUT_OUT);
				pUser->m_bEventRoom = itr->first;
				pUser->BorderDefenceAddPlayerRank();
				pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->UserInOut(INOUT_OUT);
				pUser->m_bEventRoom = itr->first;
				pUser->BorderDefenceAddPlayerRank();
				pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		foreach_stlmap(itr, g_pMain->m_TempleEventJuraidRoomList)
		{
			_JURAID_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->UserInOut(INOUT_OUT);
				pUser->m_bEventRoom = itr->first;
				pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				pUser->UserInOut(INOUT_OUT);
				pUser->m_bEventRoom = itr->first;
				pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
	}
	break;
	case TEMPLE_EVENT_CHAOS:
	{
		foreach_stlmap(itr, g_pMain->m_TempleEventChaosRoomList)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_UserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_UserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser())
					continue;

				if (!pUser->CheckExistItem(CHAOS_MAP, 1))
					continue;

				pUser->RobItem(CHAOS_MAP, 1);
				pUser->UserInOut(INOUT_OUT);
				pUser->m_bEventRoom = itr->first;
				pUser->ChaosExpansionAddPlayerRank();
				pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
			}
			pRoomInfo->m_UserList.m_lock.unlock();
		}
	}
	break;
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventGetRoomUserCount(uint16 nEventRoom, uint16 sActiveEvent, uint8 bNation/* = Nation::NONE*/) 
uint8 CGameServerDlg::TempleEventGetRoomUserCount(uint16 nEventRoom, uint16 sActiveEvent, uint8 bNation /*= Nation::NONE*/)
{
	uint8 nEventRoomUserCount = 0;

	switch (sActiveEvent)
	{
	case TEMPLE_EVENT_CHAOS:
	{
		_CHAOS_ROOM_INFO* pChaosRoomInfo = m_TempleEventChaosRoomList.GetData(nEventRoom);
		if (pChaosRoomInfo != nullptr)
			nEventRoomUserCount = pChaosRoomInfo->GetRoomTotalUserCount();
	}
	break;
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
	{
		_BDW_ROOM_INFO* pBDWRoomInfo = m_TempleEventBDWRoomList.GetData(nEventRoom);
		if (pBDWRoomInfo != nullptr)
		{
			if (bNation == Nation::NONE)
				nEventRoomUserCount = pBDWRoomInfo->GetRoomTotalUserCount();
			else if (bNation == Nation::KARUS)
				nEventRoomUserCount = pBDWRoomInfo->GetRoomKarusUserCount();
			else if (bNation == Nation::ELMORAD)
				nEventRoomUserCount = pBDWRoomInfo->GetRoomElmoradUserCount();
		}
	}
	break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
	{
		_JURAID_ROOM_INFO* pJuraidRoomInfo = m_TempleEventJuraidRoomList.GetData(nEventRoom);
		if (pJuraidRoomInfo != nullptr)
		{
			if (bNation == Nation::NONE)
				nEventRoomUserCount = pJuraidRoomInfo->GetRoomTotalUserCount();
			else if (bNation == Nation::KARUS)
				nEventRoomUserCount = pJuraidRoomInfo->GetRoomKarusUserCount();
			else if (bNation == Nation::ELMORAD)
				nEventRoomUserCount = pJuraidRoomInfo->GetRoomElmoradUserCount();
		}
	}
	break;
	default:
		break;
	}

	return nEventRoomUserCount;
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventFinish(int16 nRoom /* = -1 */)
/**
* @brief	Finishes the event and if nRoom is different than 0
*			then this method finishes the event just for this room.
*
* @param	nRoom the Room that the event to be finished if specified.
*/
void CGameServerDlg::TempleEventFinish(int16 nRoom /* = -1 */)
{
	std::vector<uint16> m_tpList;

	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_CHAOS:
#pragma region Chaos Finish
	{
		if (nRoom != -1)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventChaosRoomList.GetData(nRoom);
			if (pRoomInfo == nullptr || pRoomInfo->m_bFinished == true)
				return;

			pRoomInfo->m_bFinished = true;

			foreach_stlmap(itrUser, pRoomInfo->m_UserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_CHAOS_DUNGEON)
					continue;

				pEventUser->isPrizeGiven = true;

				int64 nChangeExp = -1;
				int64 nGainedExp = int64(pow(pUser->GetLevel(), 3) * 0.15 * (5 * (uint16)pUser->m_ChaosExpansionKillCount - (uint16)pUser->m_ChaosExpansionDeadCount));
				int64 nPremiumGainedExp = nGainedExp * 2;

				if (nGainedExp < 0)
					nGainedExp = 0;

				if (nGainedExp > 8000000)
					nGainedExp = 8000000;

				if (nPremiumGainedExp > 10000000)
					nPremiumGainedExp = 10000000;

				nChangeExp = pUser->GetPremium() != 0 ? nPremiumGainedExp : nGainedExp;

				if (nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

				int32 nUserRank = pUser->GetPlayerRank(RANK_TYPE_CHAOS_DUNGEON);
				if (nUserRank == 1 || nUserRank == 2 || nUserRank == 3)
				{
					pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos1, 0, nullptr);
					pUser->GiveItem(g_pMain->m_sChaosFinishItem, 1);
					pUser->GiveItem(g_pMain->m_sChaosFinishItems, 1);
				}
				else if (nUserRank == 4 || nUserRank == 5 || nUserRank == 6 || nUserRank == 7)
				{
					pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos2, 0, nullptr);
					pUser->GiveItem(g_pMain->m_sChaosFinishItem1, 1);
				}
				else if (nUserRank == 8 || nUserRank == 9 || nUserRank == 10)
				{
					pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos3, 0, nullptr);
					pUser->GiveItem(g_pMain->m_sChaosFinishItem2, 1);
				}
				pUser->GiveItem(g_pMain->m_sChaosFinishItem3, 1);
				pUser->GiveItem(g_pMain->m_sChaosFinishItem4, 1);
				m_tpList.push_back(pUser->GetID());
			}
		}
		else
		{
			foreach_stlmap(itr, g_pMain->m_TempleEventChaosRoomList)
			{
				_CHAOS_ROOM_INFO* pRoomInfo = itr->second;
				if (pRoomInfo == nullptr || pRoomInfo->m_bFinished == true)
					continue;

				pRoomInfo->m_bFinished = true;

				pRoomInfo->m_UserList.m_lock.lock();
				foreach_stlmap_nolock(itrUser, pRoomInfo->m_UserList)
				{
					_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
					if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
						continue;

					CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| !pUser->isEventUser()
						|| pUser->GetZoneID() != ZONE_CHAOS_DUNGEON)
						continue;

					pEventUser->isPrizeGiven = true;

					int64 nChangeExp = -1;
					int64 nGainedExp = int64(pow(pUser->GetLevel(), 3) * 0.15 * (5 * pUser->m_ChaosExpansionKillCount - pUser->m_ChaosExpansionDeadCount));
					int64 nPremiumGainedExp = nGainedExp * 2;

					if (nGainedExp < 0)
						nGainedExp = 0;

					if (nGainedExp > 8000000)
						nGainedExp = 8000000;

					if (nPremiumGainedExp > 10000000)
						nPremiumGainedExp = 10000000;

					nChangeExp = pUser->GetPremium() != 0 ? nPremiumGainedExp : nGainedExp;

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					int32 nUserRank = pUser->GetPlayerRank(RANK_TYPE_CHAOS_DUNGEON);
					if (nUserRank == 1 || nUserRank == 2 || nUserRank == 3)
					{
						pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos1, 0, nullptr);
						pUser->GiveItem(g_pMain->m_sChaosFinishItem, 1);
						pUser->GiveItem(g_pMain->m_sChaosFinishItems , 1);
					}
					else if (nUserRank == 4 || nUserRank == 5 || nUserRank == 6 || nUserRank == 7)
					{
						pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos2, 0, nullptr);
						pUser->GiveItem(g_pMain->m_sChaosFinishItem1, 1);
					}
					else if (nUserRank == 8 || nUserRank == 9 || nUserRank == 10)
					{
						pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinChaos3, 0, nullptr);
						pUser->GiveItem(g_pMain->m_sChaosFinishItem2, 1);
					}
					pUser->GiveItem(g_pMain->m_sChaosFinishItem3, 1);
					pUser->GiveItem(g_pMain->m_sChaosFinishItem4, 1);
					m_tpList.push_back(pUser->GetID());
				}
				pRoomInfo->m_UserList.m_lock.unlock();
			}
		}
	}
#pragma endregion
	break;

	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
#pragma region BDW Finish
	{
		if (nRoom != -1)
		{
			_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(nRoom);
			if (pRoomInfo == nullptr || pRoomInfo->m_bFinished == true)
				return;

			pRoomInfo->m_bFinished = true;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_BORDER_DEFENSE_WAR)
					continue;

				pEventUser->isPrizeGiven = true;

				int64 nChangeExp = -1;

				if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
				{
					pUser->GiveItem(g_pMain->m_sBorderFinishItem, 1);
					pUser->GiveItem(g_pMain->m_sBorderFinishItem1, 1);
					pUser->GiveItem(g_pMain->m_sBorderFinishItem2, 1);
					pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem3);

					if (pUser->GetLevel() < 58)
						nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 2000));
					else
						nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 1000));
				}
				else
				{
					pUser->GiveItem(g_pMain->m_sBorderFinishItem4, 1);
					pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem5);

					if (pUser->GetLevel() < 58)
						nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 1000));
					else
						nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 500));
				}

				if (nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

				m_tpList.push_back(pUser->GetID());
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_BORDER_DEFENSE_WAR)
					continue;

				pEventUser->isPrizeGiven = true;

				int64 nChangeExp = -1;

				if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
				{
					pUser->GiveItem(g_pMain->m_sBorderFinishItem, 1);
					pUser->GiveItem(g_pMain->m_sBorderFinishItem1, 1);
					pUser->GiveItem(g_pMain->m_sBorderFinishItem2, 1);
					pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem3);

					if (pUser->GetLevel() < 58)
						nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 2000));
					else
						nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 1000));
				}
				else
				{
					pUser->GiveItem(g_pMain->m_sBorderFinishItem4, 1);
					pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem5);

					if (pUser->GetLevel() < 58)
						nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 1000));
					else
						nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 500));
				}

				if (nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

				m_tpList.push_back(pUser->GetID());
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
		else
		{
			foreach_stlmap(itr, g_pMain->m_TempleEventBDWRoomList)
			{
				_BDW_ROOM_INFO* pRoomInfo = itr->second;
				if (pRoomInfo == nullptr
					|| pRoomInfo->m_bFinished == true)
					continue;

				pRoomInfo->m_bFinished = true;

				pRoomInfo->m_KarusUserList.m_lock.lock();
				foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
				{
					_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
					if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
						continue;

					CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| !pUser->isEventUser()
						|| pUser->GetZoneID() != ZONE_BORDER_DEFENSE_WAR)
						continue;

					pEventUser->isPrizeGiven = true;

					int64 nChangeExp = -1;

					if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
					{
						pUser->GiveItem(g_pMain->m_sBorderFinishItem, 1);
						pUser->GiveItem(g_pMain->m_sBorderFinishItem1, 1);
						pUser->GiveItem(g_pMain->m_sBorderFinishItem2, 1);
						pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem3);

						if (pUser->GetLevel() < 58)
							nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 2000));
						else
							nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 1000));
					}
					else
					{
						pUser->GiveItem(g_pMain->m_sBorderFinishItem4, 1);
						pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem5);

						if (pUser->GetLevel() < 58)
							nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 1000));
						else
							nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 500));
					}

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					m_tpList.push_back(pUser->GetID());
				}
				pRoomInfo->m_KarusUserList.m_lock.unlock();

				pRoomInfo->m_ElmoradUserList.m_lock.lock();
				foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
				{
					_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
					if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
						continue;

					CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| !pUser->isEventUser()
						|| pUser->GetZoneID() != ZONE_BORDER_DEFENSE_WAR)
						continue;

					pEventUser->isPrizeGiven = true;

					int64 nChangeExp = -1;

					if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
					{
						pUser->GiveItem(g_pMain->m_sBorderFinishItem, 1);
						pUser->GiveItem(g_pMain->m_sBorderFinishItem1, 1);
						pUser->GiveItem(g_pMain->m_sBorderFinishItem2, 1);
						pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem3);

						if (pUser->GetLevel() < 58)
							nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 2000));
						else
							nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 1000));
					}
					else
					{
						pUser->GiveItem(g_pMain->m_sBorderFinishItem4, 1);
						pUser->SendLoyaltyChange(g_pMain->m_sBorderFinishItem5);

						if (pUser->GetLevel() < 58)
							nChangeExp = int64((pUser->GetLevel() - 20) * (3000 + 100/* Temp Score */ * 1000));
						else
							nChangeExp = int64((pUser->GetLevel() + 55) * (20000 + 100/* Temp Score */ * 500));
					}

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					m_tpList.push_back(pUser->GetID());
				}
				pRoomInfo->m_ElmoradUserList.m_lock.unlock();
			}
		}
	}
#pragma endregion
	break;

	case TEMPLE_EVENT_JURAD_MOUNTAIN:
#pragma region Juraid Finish
	{
		if (nRoom != -1)
		{
			_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(nRoom);
			if (pRoomInfo == nullptr || pRoomInfo->m_bFinished == true)
				return;

			pRoomInfo->m_bFinished = true;

			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_JURAID_MOUNTAIN)
					continue;

				pEventUser->isPrizeGiven = true;

				if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
				{/*Juraid Mountain Winner Experience Lost System. Write TheThyke  Test Edilecek...*/
					int64 nExpLost = pUser->m_iExp;
					nExpLost = (nExpLost * JURAID_MOUNTATIN_WINNER_LOSTEXP) / 100;
					pUser->ExpChange(-nExpLost);
					pUser->GiveItem(g_pMain->m_sJuraidFinishItem, 2);
					pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinJuraid, 0, nullptr);
				}
				else
					pUser->GiveItem(g_pMain->m_sJuraidFinishItem1, 1);

				m_tpList.push_back(pUser->GetID());
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();

			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_JURAID_MOUNTAIN)
					continue;

				pEventUser->isPrizeGiven = true;

				if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
				{/*Juraid Mountain Winner Experience Lost System. Write TheThyke  Test Edilecek...*/
					int64 nExpLost = pUser->m_iExp;
					nExpLost = (nExpLost * JURAID_MOUNTATIN_WINNER_LOSTEXP) / 100;
					pUser->ExpChange(-nExpLost);
					pUser->GiveItem(g_pMain->m_sJuraidFinishItem, 2);
					pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinJuraid, 0, nullptr);
				}
				else
					pUser->GiveItem(g_pMain->m_sJuraidFinishItem1, 1); // black gem	

				m_tpList.push_back(pUser->GetID());
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}
		else
		{
			foreach_stlmap(itr, g_pMain->m_TempleEventJuraidRoomList)
			{
				_JURAID_ROOM_INFO* pRoomInfo = itr->second;
				if (pRoomInfo == nullptr
					|| pRoomInfo->m_bFinished == true)
					continue;

				pRoomInfo->m_bFinished = true;

				pRoomInfo->m_KarusUserList.m_lock.lock();
				foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
				{
					_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
					if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
						continue;

					CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| !pUser->isEventUser()
						|| pUser->GetZoneID() != ZONE_JURAID_MOUNTAIN)
						continue;

					pEventUser->isPrizeGiven = true;

					if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
					{/*Juraid Mountain Winner Experience Lost System. Write TheThyke  Test Edilecek...*/
						int64 nExpLost = pUser->m_iExp;
						nExpLost = (nExpLost * JURAID_MOUNTATIN_WINNER_LOSTEXP) / 100;
						pUser->ExpChange(-nExpLost);
						pUser->GiveItem(g_pMain->m_sJuraidFinishItem, 2);
						pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinJuraid, 0, nullptr);
					}
					else
						pUser->GiveItem(g_pMain->m_sJuraidFinishItem1, 1); // black gem	

					m_tpList.push_back(pUser->GetID());
				}
				pRoomInfo->m_KarusUserList.m_lock.unlock();

				pRoomInfo->m_ElmoradUserList.m_lock.lock();
				foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
				{
					_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
					if (pEventUser == nullptr || pEventUser->isPrizeGiven == true)
						continue;

					CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| !pUser->isEventUser()
						|| pUser->GetZoneID() != ZONE_JURAID_MOUNTAIN)
						continue;

					pEventUser->isPrizeGiven = true;

					if (pRoomInfo->m_bWinnerNation == pUser->GetNation())
					{/*Juraid Mountain Winner Experience Lost System. Write TheThyke  Test Edilecek...*/
						int64 nExpLost = pUser->m_iExp;
						nExpLost = (nExpLost * JURAID_MOUNTATIN_WINNER_LOSTEXP) / 100;
						pUser->ExpChange(-nExpLost);
						pUser->GiveItem(g_pMain->m_sJuraidFinishItem, 2);
						pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinJuraid, 0, nullptr);
					}
					else
						pUser->GiveItem(g_pMain->m_sJuraidFinishItem1, 1); // black gem	

					m_tpList.push_back(pUser->GetID());
				}
				pRoomInfo->m_ElmoradUserList.m_lock.unlock();
			}
		}
	}
#pragma endregion
	break;

	default:
		break;
	}

	foreach(usr, m_tpList)
	{
		CUser* pUser = GetUserPtr(*usr);

		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetEventRoom() == 0)
			continue;

		TempleEventKickOutUser(pUser);
		pUser->StateChangeServerDirect(3, ABNORMAL_NORMAL);
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventGetActiveEventTime(CUser *pUser)
void CGameServerDlg::TempleEventGetActiveEventTime(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
	result << (uint16)pTempleEvent.ActiveEvent << m_nVirtualEventRoomRemainSeconds;
	pUser->Send(&result);
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventSendActiveEventTime(CUser *pUser)
/**
* @brief	Sends the timing and number of users joined for the event.
*/
void CGameServerDlg::TempleEventSendActiveEventTime(CUser* pUser)
{
	if (pUser == nullptr || pTempleEvent.ActiveEvent == -1)
		return;

	if (!pUser->isEventUser())
	{
		TempleEventGetActiveEventTime(pUser);
		return;
	}

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT_JOIN));
	result << uint8(1) << uint16(pTempleEvent.ActiveEvent);
	pUser->Send(&result);

	switch (pTempleEvent.ActiveEvent)
	{
	case TEMPLE_EVENT_CHAOS:
		TemplEventChaosSendJoinScreenUpdate(pUser);
		break;
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		TemplEventBDWSendJoinScreenUpdate(pUser);
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		TemplEventJuraidSendJoinScreenUpdate(pUser);
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region CGameServerDlg::TempleEventKickOutUser(CUser *pUser)
void CGameServerDlg::TempleEventKickOutUser(CUser* pUser)
{
	if (pUser == nullptr
		|| !pUser->isInGame())
		return;

	uint8 nZoneID = 0;

	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
	{
		if (pUser->GetLevel() < 35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 35 && pUser->GetLevel() <= 59)
			nZoneID = ZONE_ARDREAM;
		else if (pUser->GetLevel() >= 60 && pUser->GetLevel() <= 69)
			nZoneID = ZONE_RONARK_LAND_BASE;
		else if (pUser->GetLevel() >= 70)
			nZoneID = ZONE_RONARK_LAND;
	}
	else if (pUser->GetZoneID() == ZONE_CHAOS_DUNGEON)
	{
		if (pUser->GetLevel() < 35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 35 && pUser->GetLevel() <= 59)
			nZoneID = ZONE_ARDREAM;
		else if (pUser->GetLevel() >= 60 && pUser->GetLevel() <= 69)
			nZoneID = ZONE_RONARK_LAND_BASE;
		else if (pUser->GetLevel() >= 70)
			nZoneID = ZONE_RONARK_LAND;
	}
	else if (pUser->GetZoneID() == ZONE_JURAID_MOUNTAIN)
	{
		if (pUser->GetLevel() < 35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 35 && pUser->GetLevel() <= 59)
			nZoneID = ZONE_ARDREAM;
		else if (pUser->GetLevel() >= 60 && pUser->GetLevel() <= 69)
			nZoneID = ZONE_RONARK_LAND_BASE;
		else if (pUser->GetLevel() >= 70)
			nZoneID = ZONE_RONARK_LAND;
	}


	C3DMap* ZoneInfo = g_pMain->m_ZoneArray.GetData(nZoneID);
	if (ZoneInfo == nullptr || nZoneID == 0)
		nZoneID = ZONE_MORADON;

	//Special
	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
	{
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
		{
			if (pUser->m_bHasAlterOptained)
				CMagicProcess::RemoveType4Buff(BUFF_TYPE_FRAGMENT_OF_MANES, pUser, true, true);

			pUser->m_bHasAlterOptained = false;
			pUser->BorderDefenceRemovePlayerRank();
		}
	}
	else if (pUser->GetZoneID() == ZONE_CHAOS_DUNGEON)
	{
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS)
		{
			pUser->SetMaxHp(1);
			pUser->RobChaosSkillItems();
			pUser->ChaosExpansionRemovePlayerRank();
		}
	}
	pUser->ResetTempleEventData();
	pUser->ZoneChange(nZoneID, 0.0f, 0.0f);
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventReset(uint16 bActiveEvent)
void CGameServerDlg::TempleEventReset(uint16 bActiveEvent)
{
	if (bActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
	{
		RemoveAllEventNpc(ZONE_BORDER_DEFENSE_WAR);
		foreach_stlmap(itr, m_TempleEventBDWRoomList)
		{
			if (itr->second == nullptr)
				continue;

			itr->second->Initialize();
		}
		TrashBorderDefenceWarRanking();
	}
	else if (bActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
	{
		RemoveAllEventNpc(ZONE_JURAID_MOUNTAIN);
		foreach_stlmap(itr, m_TempleEventJuraidRoomList)
		{
			if (itr->second == nullptr)
				continue;

			itr->second->Initialize();

			for (int i = 0; i < 3; i++)
			{
				CNpc* pNpc = g_pMain->GetNpcPtr(itr->second->m_sElmoBridges[i], ZONE_JURAID_MOUNTAIN);
				if (pNpc == nullptr)
					continue;

				pNpc->m_byGateOpen = 0;
			}

			for (int j = 0; j < 3; j++)
			{
				CNpc* pNpc = g_pMain->GetNpcPtr(itr->second->m_sKarusBridges[j], ZONE_JURAID_MOUNTAIN);
				if (pNpc == nullptr)
					continue;

				pNpc->m_byGateOpen = 0;
			}
		}
		pEventBridge.isBridgeSystemActive = false;
		pEventBridge.isBridgeTimerControl1 = false;
		pEventBridge.isBridgeTimerControl2 = false;
		pEventBridge.isBridgeTimerControl3 = false;
		pEventBridge.isBridgeSystemStartMinutes = 0;
	}
	else if (bActiveEvent == TEMPLE_EVENT_CHAOS)
	{
		RemoveAllEventNpc(ZONE_CHAOS_DUNGEON);
		foreach_stlmap(itr, m_TempleEventChaosRoomList)
		{
			if (itr->second == nullptr)
				continue;

			itr->second->Initialize();
		}
		TrashChaosExpansionRanking();
	}

	m_TempleEventUserMap.DeleteAllData();
	pTempleEvent.m_tLastWinnerCheck = 0;
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.ClosedTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.EventStartTimeNumber = -1;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;
	pTempleEvent.bAllowJoin = false;
	pTempleEvent.AutomaticEventOpening = true;
	pTempleEvent.EventTimerStartControl = false;
	pTempleEvent.EventTimerAttackOpenControl = false;
	pTempleEvent.EventTimerAttackCloseControl = false;
	pTempleEvent.EventTimerFinishControl = false;
	pTempleEvent.EventTimerResetControl = false;
	pTempleEvent.EventCloseMainControl = false;
	m_TempleEventLastUserOrder = 1;
}
#pragma endregion

#pragma region CGameServerDlg::TempleEventChaosSendWinnerCounter()
/**
* @brief	Sets the counter screen for the Chaos Event to zero (0).
*/
void CGameServerDlg::TempleEventChaosShowCounterScreen()
{
	if (isChaosExpansionActive())
	{
		foreach_stlmap(itr, m_TempleEventChaosRoomList)
		{
			_CHAOS_ROOM_INFO* pRoomInfo = itr->second;
			if (pRoomInfo == nullptr)
				continue;

			pRoomInfo->m_UserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_UserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetZoneID() != ZONE_CHAOS_DUNGEON)
					continue;

				//pUser->HandlePlayerRankings(Packet(WIZ_RANK, (uint8)RANK_TYPE_CHAOS_DUNGEON));
				pUser->StateChangeServerDirect(7, 0);
			}
			pRoomInfo->m_UserList.m_lock.unlock();
		}
	}
}
#pragma endregion

#pragma region 	CUser::isDevaStage()
bool CUser::isDevaStage()
{
	if (!isEventUser())
		return false;

	_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return false;

	if (GetNation() == KARUS)
	{
		if (pRoomInfo->m_iKarusMainMonsterKill >= 12 && pRoomInfo->m_iKarusSubMonsterKill >= 60)
			return true;
	}
	else
	{
		if (pRoomInfo->m_iElmoMainMonsterKill >= 12 && pRoomInfo->m_iElmoSubMonsterKill >= 60)
			return true;
	}

	return false;
}
#pragma endregion

#pragma region 	CUser::isBridgeStage1()
bool CUser::isBridgeStage1()
{
	if (!isEventUser())
		return false;

	_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return false;

	if (GetNation() == KARUS)
	{
		if (pRoomInfo->m_iKarusMainMonsterKill >= 4 && pRoomInfo->m_iKarusSubMonsterKill >= 20)
		{
			if (pRoomInfo->m_iKarusMainMonsterKill <= 8 && pRoomInfo->m_iKarusSubMonsterKill < 40)
				return true;
		}
	}
	else
	{
		if (pRoomInfo->m_iElmoMainMonsterKill >= 4 && pRoomInfo->m_iElmoSubMonsterKill >= 20)
		{
			if (pRoomInfo->m_iElmoMainMonsterKill <= 8 && pRoomInfo->m_iElmoSubMonsterKill < 40)
				return true;
		}
	}

	return false;
}
#pragma endregion

#pragma region 	CUser::isBridgeStage2()
bool CUser::isBridgeStage2()
{
	if (!isEventUser())
		return false;

	_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return false;

	if (GetNation() == KARUS)
	{
		if (pRoomInfo->m_iKarusMainMonsterKill >= 8 && pRoomInfo->m_iKarusSubMonsterKill >= 40)
		{
			if (pRoomInfo->m_iKarusMainMonsterKill <= 12 && pRoomInfo->m_iKarusSubMonsterKill < 60)
				return true;
		}
	}
	else
	{
		if (pRoomInfo->m_iElmoMainMonsterKill >= 8 && pRoomInfo->m_iElmoSubMonsterKill >= 40)
		{
			if (pRoomInfo->m_iElmoMainMonsterKill <= 12 && pRoomInfo->m_iElmoSubMonsterKill < 60)
				return true;
		}
	}

	return false;
}
#pragma endregion

#pragma endregion