#include "stdafx.h"
#if 0
#pragma region CGameServerDlg::UnderTheCastleTimer()
void CGameServerDlg::UnderTheCastleTimer()
{
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;
	int nSec = g_localTime.tm_sec;

#pragma region The Under The Castle
	if (!m_bUnderTheCastleIsActive)
	{
		for (int EventIDi = 1; EventIDi <= m_EventScheduleArray.GetSize(); EventIDi++)
		{
			_EVENT_SCHEDULE* pEventStatus = m_EventScheduleArray.GetData(EventIDi);
			if (pEventStatus == nullptr
				|| pEventStatus->EventStatus == 0
				|| pEventStatus->EventType != EventType::SingleRoom)
				continue;

			std::list<std::string> nInGameEvent = StrSplit(pEventStatus->StartDays, ",");
			uint8 nInGameEventDaysSize = (uint8)nInGameEvent.size();
			if (nInGameEventDaysSize > 0)
			{
				uint8 nInGameDays = 0;
				for (int EventDayi = 0; EventDayi < nInGameEventDaysSize; EventDayi++)
				{
					nInGameDays = atoi(nInGameEvent.front().c_str());
					if (nInGameDays == nWeekDay)
					{
						for (int EventTimei = 0; EventTimei < EVENT_START_TIMES; EventTimei++)
						{
							if (pEventStatus->TimeActive[EventTimei] != 1)
								continue;

							if (pEventStatus->EventType == EventLocalType::UnderTheCastle)
							{
								if (pEventStatus->EventStartHour[EventTimei] == nHour)
								{
									m_bUnderTheCastleIsActive = true;
									m_bUnderTheCastleMonster = true;
									m_nUnderTheCastleEventTime = (180 - nMin) * MINUTE;
									Announcement(IDS_UNDER_THE_CASTLE_OPEN);
									printf("%d. call for the Knight Online %s is started at %02d:%02d:%02d \n", EventTimei, pEventStatus->EventName.c_str(), nHour, nMin, nSec);
								}
								if ((pEventStatus->EventStartHour[EventTimei] + (1)) == nHour)
								{
									m_bUnderTheCastleIsActive = true;
									m_bUnderTheCastleMonster = true;
									m_nUnderTheCastleEventTime = (120 - nMin) * MINUTE;
									printf("%d. call for the Knight Online %s is started at %02d:%02d:%02d \n", EventTimei, pEventStatus->EventName.c_str(), nHour, nMin, nSec);
								}
								if ((pEventStatus->EventStartHour[EventTimei] + (2)) == nHour)
								{
									m_bUnderTheCastleIsActive = true;
									m_bUnderTheCastleMonster = true;
									m_nUnderTheCastleEventTime = (60 - nMin) * MINUTE;
									printf("%d. call for the Knight Online %s is started at %02d:%02d:%02d \n", EventTimei, pEventStatus->EventName.c_str(), nHour, nMin, nSec);
								}
							}
						}
					}
					nInGameEvent.pop_front();
				}
			}
		}
	}
	else if (m_bUnderTheCastleIsActive)
	{
		if (m_bUnderTheCastleMonster == true)
		{
			foreach_stlmap(itr, m_MonsterUnderTheCastleArray)
				SpawnEventNpc(itr->second->sSid, itr->second->bType == 0 ? true : false, ZONE_UNDER_CASTLE, (float)itr->second->X, itr->second->Y, (float)itr->second->Z, itr->second->sCount, itr->second->bRadius, 0, 0, -1, 0, itr->second->byDirection, 1, 0, itr->second->bTrapNumber);

			m_bUnderTheCastleMonster = false;
		}
		if (m_nUnderTheCastleEventTime == 10785)
		{
			Packet result(WIZ_UTC_MOVIE, uint8(2));
			result << uint8(1) << uint16(1) << uint32(1);
			Send_Zone(&result, ZONE_UNDER_CASTLE);
		}
		else
		{
			if (m_nUnderTheCastleEventTime == 0)
			{
				foreach(itr, m_UnderTheCastleMonsterList)
					KillNpc(itr->first, ZONE_UNDER_CASTLE);

				m_bUnderTheCastleIsActive = false;
				m_bUnderTheCastleMonster = false;
				m_nUnderTheCastleUsers.clear();
				m_UnderTheCastleMonsterList.clear();

				Announcement(IDS_UNDER_THE_CASTLE_CLOSE);
				KickOutZoneUsers(ZONE_UNDER_CASTLE);
			}
		}

		if (m_nUnderTheCastleEventTime > 0)
			m_nUnderTheCastleEventTime--;
	}
#pragma endregion

	/*if (!m_bUnderTheCastleIsActive)
	{
		std::list<std::string> nUnderTheCastle = StrSplit(m_nUtcZoneOpenDays, ",");
		uint8 nUnderTheCastleDaySize = (uint8)nUnderTheCastle.size();
		if (nUnderTheCastleDaySize > 0)
		{
			uint8 nUnderTheCastleDay = 0;
			for (int i = 0; i < nUnderTheCastleDaySize; i++)
			{
				nUnderTheCastleDay = atoi(nUnderTheCastle.front().c_str());
				if (nUnderTheCastleDay == nWeekDay)
				{
					for (int x = 0; x < WAR_TIME_COUNT; x++)
					{
						if (m_nUtcZoneOpenHourStart[x] == nHour)
						{
							m_bUnderTheCastleIsActive = true;
							m_bUnderTheCastleMonster = true;
							m_nUnderTheCastleEventTime = (180 - nMin) * MINUTE;
							Announcement(IDS_UNDER_THE_CASTLE_OPEN);
						}
						if ((m_nUtcZoneOpenHourStart[x] + (1)) == nHour)
						{
							m_bUnderTheCastleIsActive = true;
							m_bUnderTheCastleMonster = true;
							m_nUnderTheCastleEventTime = (120 - nMin) * MINUTE;
						}
						if ((m_nUtcZoneOpenHourStart[x] + (2)) == nHour)
						{
							m_bUnderTheCastleIsActive = true;
							m_bUnderTheCastleMonster = true;
							m_nUnderTheCastleEventTime = (60 - nMin) * MINUTE;
						}
					}
				}
				nUnderTheCastle.pop_front();
			}
		}
	}
	else if (m_bUnderTheCastleIsActive)
	{
		if (m_bUnderTheCastleMonster == true)
		{
			foreach_stlmap(itr, m_MonsterUnderTheCastleArray)
				SpawnEventNpc(itr->second->sSid, itr->second->bType == 0 ? true : false, ZONE_UNDER_CASTLE, (float)itr->second->X, itr->second->Y, (float)itr->second->Z, itr->second->sCount, itr->second->bRadius, 0, 0, -1, 0, itr->second->byDirection, 1, 0, itr->second->bTrapNumber);

			m_bUnderTheCastleMonster = false;
		}
		if (m_nUnderTheCastleEventTime == 10785)
		{
			Packet result(WIZ_UTC_MOVIE, uint8(2));
			result << uint8(1) << uint16(1) << uint32(1);
			Send_Zone(&result, ZONE_UNDER_CASTLE);
		}
		else
		{
			if (m_nUnderTheCastleEventTime == 0)
			{
				foreach(itr, m_UnderTheCastleMonsterList)
					KillNpc(itr->first, ZONE_UNDER_CASTLE);
				
				m_bUnderTheCastleIsActive = false;
				m_bUnderTheCastleMonster = false;
				m_nUnderTheCastleUsers.clear();
				m_UnderTheCastleMonsterList.clear();

				Announcement(IDS_UNDER_THE_CASTLE_CLOSE);
				KickOutZoneUsers(ZONE_UNDER_CASTLE);
			}
		}

		if (m_nUnderTheCastleEventTime > 0)
			m_nUnderTheCastleEventTime--;
	}*/
}
#pragma endregion
#endif 0

#pragma region CNpc::UnderTheCastleProcess(CUser *pUser)
/**
* @brief	Handles the Under The Castle Process and checks if
*			the user succeeds the event.

* @param	pUser	the User that triggers the method.
*/
void CNpc::UnderTheCastleProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (GetType() == NPC_UTC_SPAWN_FAST)
		SendInOut(INOUT_OUT, GetX(), GetZ(), GetY());


	switch (GetProtoID())
	{
	case MONSTER_EMPEROR_MAMMOTH_I:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(7);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case  MONSTER_EMPEROR_MAMMOTH_II:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(8);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_EMPEROR_MAMMOTH_III:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(2);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
		g_pMain->SendItemUnderTheCastleRoomUsers(ZONE_UNDER_CASTLE, 1, GetX(), GetZ());
	}
	break;
	case MONSTER_CRESHERGIMMIC_I:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_CRESHERGIMMIC_II:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_CRESHERGIMMIC_III:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_CRESHERGIMMIC_VI:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(3);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
		g_pMain->SendItemUnderTheCastleRoomUsers(ZONE_UNDER_CASTLE, 2, GetX(), GetZ());
	}
	break;
	case MONSTER_PURIOUS_I:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_PURIOUS_II:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_PURIOUS_III:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_PURIOUS_VI:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(4);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
		g_pMain->SendItemUnderTheCastleRoomUsers(ZONE_UNDER_CASTLE, 3, GetX(), GetZ());
	}
	break;
	case MONSTER_FLUWITON_ROOM_3_I:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_FLUWITON_ROOM_3_II:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_FLUWITON_ROOM_3_III:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(5);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
		g_pMain->SendItemUnderTheCastleRoomUsers(ZONE_UNDER_CASTLE, 4, GetX(), GetZ());
	}
	break;
	case MONSTER_FLUWITON_ROOM_4_I:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_FLUWITON_ROOM_4_II:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_FLUWITON_ROOM_4_III:
	{
		Packet result(WIZ_UTC_MOVIE, uint8(2));
		result << uint8(1) << uint16(1) << uint32(6);
		g_pMain->Send_Zone(&result, ZONE_UNDER_CASTLE);
	}
	break;
	case MONSTER_FLUWITON_ROOM_4_VI:
		g_pMain->SendItemUnderTheCastleRoomUsers(ZONE_UNDER_CASTLE, 5, GetX(), GetZ());
		g_pMain->SpawnEventNpc(29197, false, ZONE_UNDER_CASTLE, (float)852, 0, (float)830, 1, 0, 0, 3, -1);
		g_pMain->SpawnEventNpc(29197, false, ZONE_UNDER_CASTLE, (float)825, 0, (float)873, 1, 0, 0, 3, -1);
		break;
	default:
		break;
	}

	_UNDER_THE_CASTLE_INFO* pRoomInfo = g_pMain->m_MonsterUnderTheCastleList.GetData(1);

	CNpc *pNpc = nullptr;

	if (pRoomInfo != nullptr)
	{
		if (GetProtoID() == MONSTER_CRESHERGIMMIC_VI)
			pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sUtcGateID[0], GetZoneID());
		else if (GetProtoID() == MONSTER_PURIOUS_VI)
			pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sUtcGateID[1], GetZoneID());
		else if (GetProtoID() == MONSTER_FLUWITON_ROOM_3_III)
			pNpc = g_pMain->GetNpcPtr(pRoomInfo->m_sUtcGateID[2], GetZoneID());

		if (pNpc != nullptr)
			pNpc->DeadReq(pUser);
	}

	g_pMain->m_UnderTheCastleMonsterList.erase(GetProtoID());
}
#pragma endregion