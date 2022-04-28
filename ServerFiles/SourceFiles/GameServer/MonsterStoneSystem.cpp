#include "stdafx.h"

#pragma region CGameServerDlg::MonsterStoneProcess(Packet & pkt)
void CUser::MonsterStoneProcess(Packet & pkt)
{
	if (!isInGame())
		return;

	uint8 bZoneID; uint16 bFamilly; uint32 itemid = 0;

	pkt >> itemid;

	_ITEM_TABLE *pTable = g_pMain->GetItemPtr(itemid);

	if (pTable == nullptr)
		return;

	switch (pTable->m_iNum)
	{
	case ITEM_MONSTER_STONE:
	{
		if (CheckExistItem(ITEM_MONSTER_STONE, 1))
		{
			if (GetLevel() < 20)
				return;

			if (isQuestEventUser())
			{
				g_pMain->m_MonsterStoneTeleportListArray.DeleteData(GetSocketID());
				return;
			}
			else
			{
				_TEMPLE_QUEST_USER* pTeleportUser = new _TEMPLE_QUEST_USER();

				pTeleportUser->m_socketID = GetSocketID();
				pTeleportUser->m_zoneID = ZONE_MORADON;
				pTeleportUser->m_teletortTIME = uint32(UNIXTIME + MONSTER_STONE_TIME);

				if (!g_pMain->m_MonsterStoneTeleportListArray.PutData(pTeleportUser->m_socketID, pTeleportUser))
				{
					delete pTeleportUser;
					return;
				}
			}

			RobItem(ITEM_MONSTER_STONE, 1);

			UserInOut(INOUT_OUT);
			m_bEventRoom = ++g_pMain->m_MonsterStoneEventRoom;

			if (GetLevel() >= 20 && GetLevel() <= 29)
			{
				bZoneID = ZONE_STONE1;
				bFamilly = 1;
			}
			else if (GetLevel() >= 30 && GetLevel() <= 35)
			{
				bZoneID = ZONE_STONE1;
				bFamilly = 2;
			}
			else if (GetLevel() >= 36 && GetLevel() <= 40)
			{
				bZoneID = ZONE_STONE1;
				bFamilly = 3;
			}
			else if (GetLevel() >= 41 && GetLevel() <= 46)
			{
				bZoneID = ZONE_STONE1;
				bFamilly = 4;
			}
			else if (GetLevel() >= 47 && GetLevel() <= 55)
			{
				bZoneID = (uint8)myrand(ZONE_STONE1, ZONE_STONE2);

				if (bZoneID == ZONE_STONE1)
					bFamilly = 4;
				else
					bFamilly = 5;
			}
			else if (GetLevel() >= 56 && GetLevel() <= 60)
			{
				bZoneID = ZONE_STONE2;
				bFamilly = (uint8)myrand(6, 8);
			}
			else if (GetLevel() >= 61 && GetLevel() <= 66)
			{
				bZoneID = ZONE_STONE2;
				bFamilly = (uint8)myrand(8, 9);
			}
			else if (GetLevel() >= 67 && GetLevel() <= 70)
			{
				bFamilly = (uint8)myrand(9, 10);

				if (bFamilly == 9)
					bZoneID = ZONE_STONE2;
				else
					bZoneID = ZONE_STONE3;
			}
			else if (GetLevel() >= 71 && GetLevel() <= 74)
			{
				bZoneID = ZONE_STONE3;
				bFamilly = (uint8)myrand(10, 12);
			}
			else if (GetLevel() >= 75 && GetLevel() <= MAX_LEVEL)
			{
				bZoneID = ZONE_STONE3;
				bFamilly = 13;
			}

			foreach_stlmap(itr, g_pMain->m_MonsterStoneListInformationArray)
				if (itr->second->ZoneID == bZoneID && itr->second->Family == bFamilly)
					g_pMain->SpawnEventNpc(itr->second->sSid, itr->second->bType == 0 ? true : false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, 0, MONSTER_STONE_DEAD_TIME, 0, AWARD_EXP + GetSocketID(), GetEventRoom(), itr->second->byDirection, 1, 0);

			if (bZoneID == ZONE_STONE1)
			{
				g_pMain->SpawnEventNpc(16062, false, bZoneID, 204, 0, 201, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(12117, false, bZoneID, 204, 0, 197, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(31508, false, bZoneID, 204, 0, 193, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
			}

			if (bZoneID == ZONE_STONE2)
			{
				g_pMain->SpawnEventNpc(16062, false, bZoneID, 203, 0, 202, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(12117, false, bZoneID, 203, 0, 197, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(31508, false, bZoneID, 203, 0, 193, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
			}

			if (bZoneID == ZONE_STONE3)
			{
				g_pMain->SpawnEventNpc(16062, false, bZoneID, 204, 0, 207, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(12117, false, bZoneID, 204, 0, 200, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
				g_pMain->SpawnEventNpc(31508, false, bZoneID, 204, 0, 194, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), GetEventRoom(), 0, 0, 0);
			}

			if (bZoneID > 0)
			{
				ZoneChange(bZoneID, 0.0f, 0.0f);
				MonsterStoneTimerScreen(MONSTER_STONE_TIME);
			}
		}
	}
	break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CGameServerDlg::MonsterStoneQuestProcess(uint16 sQuestNum)
void CUser::MonsterStoneQuestProcess(uint16 sQuestNum)
{
	if (!isInGame())
		return;

	uint8 bZoneID = 0;
	uint16 bFamilly = 0;

	switch (sQuestNum)
	{
	case 702: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 50;
		break;
	case 708: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 65;
		break;
	case 714: // OK..
		bZoneID = ZONE_STONE3;
		bFamilly = 66;
		break;
	case 720: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 52;
		break;
	case 726: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 60;
		break;
	case 732: // OK..
		bZoneID = ZONE_STONE3;
		bFamilly = 61;
		break;
	case 738: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 67;
		break;
	case 744: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 68;
		break;
	case 750: // OK..
		bZoneID = ZONE_STONE3;
		bFamilly = 69;
		break;
	case 756: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 62;
		break;
	case 762: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 63;
		break;
	case 768: // OK..
		bZoneID = ZONE_STONE3;
		bFamilly = 64;
		break;
	case 781: // OK..
		bZoneID = ZONE_STONE1;
		bFamilly = 70;
		break;
	case 784: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 56;
		break;
	case 787: // OK..
		bZoneID = ZONE_STONE3;
		bFamilly = 57;
		break;
	case 790: // OK.. Not Finish Packet.
		bZoneID = ZONE_STONE1;
		bFamilly = 58;
		break;
	case 802: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 59;
		break;
	case 199: // OK..
		bZoneID = ZONE_STONE2;
		bFamilly = 71;
	default:
		break;
	}

	if (bZoneID == 0)
		return;

	foreach_stlmap(itr, g_pMain->m_MonsterStoneListInformationArray)
		if (itr->second->ZoneID == bZoneID && itr->second->Family == bFamilly)
			g_pMain->SpawnEventNpc(itr->second->sSid, itr->second->bType == 0 ? true : false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, 0, MONSTER_STONE_DEAD_TIME, 0, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, itr->second->byDirection, 1, 0);

	if (bZoneID == ZONE_STONE1)
	{
		g_pMain->SpawnEventNpc(16062, false, bZoneID, 204, 0, 201, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(12117, false, bZoneID, 204, 0, 197, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(31508, false, bZoneID, 204, 0, 193, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
	}

	if (bZoneID == ZONE_STONE2)
	{
		g_pMain->SpawnEventNpc(16062, false, bZoneID, 203, 0, 202, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(12117, false, bZoneID, 203, 0, 197, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(31508, false, bZoneID, 203, 0, 193, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
	}

	if (bZoneID == ZONE_STONE3)
	{
		g_pMain->SpawnEventNpc(16062, false, bZoneID, 204, 0, 207, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(12117, false, bZoneID, 204, 0, 200, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
		g_pMain->SpawnEventNpc(31508, false, bZoneID, 204, 0, 194, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);
	}

	if (sQuestNum == 790)
		g_pMain->SpawnEventNpc(9260, true, bZoneID, 195, 0, 197, 1, 0, MONSTER_STONE_DEAD_TIME, 3, AWARD_EXP + GetSocketID(), g_pMain->m_MonsterStoneSquadEventRoom, 0, 0, 0);

	if (isInParty())
	{
		_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());

		if (pParty == nullptr)
			return;

		short partyUsers[MAX_PARTY_USERS];

		for (int i = 0; i < MAX_PARTY_USERS; i++)
			partyUsers[i] = pParty->uid[i];

		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser * pUser = g_pMain->GetUserPtr(partyUsers[i]);
			if (pUser == nullptr)
				continue;

			if (GetEventRoom() 
				!= pUser->GetEventRoom())
				continue;

			if (pUser->isQuestEventUser())
			{
				g_pMain->m_MonsterStoneTeleportListArray.DeleteData(pUser->GetSocketID());
				return;
			}
			else
			{
				_TEMPLE_QUEST_USER* pTeleportUser = new _TEMPLE_QUEST_USER();

				pTeleportUser->m_socketID = pUser->GetSocketID();
				pTeleportUser->m_zoneID = ZONE_MORADON;
				pTeleportUser->m_teletortTIME = uint32(UNIXTIME + MONSTER_STONE_TIME);

				if (!g_pMain->m_MonsterStoneTeleportListArray.PutData(pTeleportUser->m_socketID, pTeleportUser))
				{
					delete pTeleportUser;
					return;
				}
			}

			pUser->UserInOut(INOUT_OUT);
			pUser->m_bEventRoom = g_pMain->m_MonsterStoneSquadEventRoom;

			if (bZoneID > 0)
			{
				pUser->ZoneChange(bZoneID, 0.0f, 0.0f);
				pUser->MonsterStoneQuestTimerScreen(MONSTER_STONE_TIME);
			}
		}
	}
	else
	{
		if (isQuestEventUser())
		{
			g_pMain->m_MonsterStoneTeleportListArray.DeleteData(GetSocketID());
			return;
		}
		else
		{
			_TEMPLE_QUEST_USER* pTeleportUser = new _TEMPLE_QUEST_USER();

			pTeleportUser->m_socketID = GetSocketID();
			pTeleportUser->m_zoneID = ZONE_MORADON;
			pTeleportUser->m_teletortTIME = uint32(UNIXTIME + MONSTER_STONE_TIME);

			if (!g_pMain->m_MonsterStoneTeleportListArray.PutData(pTeleportUser->m_socketID, pTeleportUser))
			{
				delete pTeleportUser;
				return;
			}
		}

		UserInOut(INOUT_OUT);
		m_bEventRoom = g_pMain->m_MonsterStoneSquadEventRoom;

		if (bZoneID > 0)
		{
			ZoneChange(bZoneID, 0.0f, 0.0f);
			MonsterStoneQuestTimerScreen(MONSTER_STONE_TIME);
		}
	}
	++g_pMain->m_MonsterStoneSquadEventRoom;
}
#pragma endregion

#pragma region CGameServerDlg::TempleMonsterStoneTimer()
void CGameServerDlg::TempleMonsterStoneTimer()
{
	foreach_stlmap(itr, g_pMain->m_MonsterStoneTeleportListArray)
	{
		_TEMPLE_QUEST_USER* pTeleport = itr->second;
		if (pTeleport == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(pTeleport->m_socketID);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (pTeleport->m_teletortTIME > 0 && pTeleport->m_teletortTIME < UNIXTIME)
		{
			pTeleport->m_teletortTIME = 0;
			pUser->m_bEventRoom = 0;
			pUser->ZoneChange(pTeleport->m_zoneID, 0.0f, 0.0f);
			break;
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::MonsterStoneExitProcess()
void CUser::MonsterStoneExitProcess()
{
	if (isQuestEventUser())
		g_pMain->m_MonsterStoneTeleportListArray.DeleteData(GetSocketID());

	g_pMain->KillNpc(AWARD_EXP + GetSocketID(), GetZoneID(), this);
	m_bEventRoom = 0;
}
#pragma endregion

#pragma region CNpc::MonsterStoneKillProcess(CUser *pUser)

/**
* @brief	Handles the Monster Stone Kill Process and checks if
*			the user succeeds the event.

* @param	pUser	the User that triggers the method.
*/
void CNpc::MonsterStoneKillProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (GetProtoID() == 8718
		|| GetProtoID() == 8728
		|| GetProtoID() == 8746
		|| GetProtoID() == 8770
		|| GetProtoID() == 8708
		|| GetProtoID() == 8737
		|| GetProtoID() == 8754
		|| GetProtoID() == 8784
		|| GetProtoID() == 8792
		|| GetProtoID() == 8762
		|| GetProtoID() == 8776
		|| GetProtoID() == 8800
		|| GetProtoID() == 8809
		|| GetProtoID() == 8728
		|| GetProtoID() == 9164
		|| GetProtoID() == 9165
		|| GetProtoID() == 9176
		|| GetProtoID() == 9177
		|| GetProtoID() == 9188
		|| GetProtoID() == 9189
		|| GetProtoID() == 9127
		|| GetProtoID() == 9128
		|| GetProtoID() == 9139
		|| GetProtoID() == 9140
		|| GetProtoID() == 9151
		|| GetProtoID() == 9152
		|| GetProtoID() == 9262
		|| GetProtoID() == 9264
		|| GetProtoID() == 9266
		|| GetProtoID() == 9270		
		|| GetProtoID() == 7008)
	{
		Packet result(WIZ_EVENT);
		result << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(0x11) << uint8(0x00)
			<< uint8(0x65) << uint8(0x14) << uint32(0x00);

		pUser->Send(&result);

		result.Initialize(WIZ_QUEST);
		result << uint8(2) << uint16(209) << uint8(0);

		pUser->Send(&result);

		_TEMPLE_QUEST_USER * pEventUser = g_pMain->m_MonsterStoneTeleportListArray.GetData(pUser->GetSocketID());

		if (pEventUser)
		{
			g_pMain->KillNpc(AWARD_EXP + pUser->GetSocketID(), GetZoneID());
			pEventUser->m_teletortTIME = uint32(UNIXTIME + MONSTER_STONE_FINISH_TIME);
			return;
		}
		pUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
	}
}
#pragma endregion

#pragma region CUser::MonsterStoneTimerScreen(uint16 Time)
void CUser::MonsterStoneTimerScreen(uint16 Time)
{
	bool bIsNeutralZone = (GetZoneID() >= ZONE_STONE1 && GetZoneID() <= ZONE_STONE3);

	if (!bIsNeutralZone || !isInGame())
		return;

	Packet result;

	result.Initialize(WIZ_BIFROST);
	result << uint8(MONSTER_SQUARD) << uint16(Time);
	Send(&result);

	result.clear();
	result.Initialize(WIZ_SELECT_MSG);
	result << uint16(0) << uint8(7) << uint64(0) << uint8(9) << uint16(0) << uint8(0) << uint8(11) << uint16(Time) << uint16(0);
	Send(&result);
}
#pragma endregion

#pragma region CUser::MonsterStoneQuestTimerScreen(uint16 Time)
void CUser::MonsterStoneQuestTimerScreen(uint16 Time)
{
	bool bIsNeutralZone = (GetZoneID() >= ZONE_STONE1 && GetZoneID() <= ZONE_STONE3);

	if (!bIsNeutralZone || !isInGame())
		return;

	Packet result;
	result.Initialize(WIZ_BIFROST);
	result << uint8(MONSTER_SQUARD) << uint16(Time);
	Send(&result);

	result.clear();
	result.Initialize(WIZ_SELECT_MSG);
	result << uint16(0) << uint8(7) << uint64(0) << uint8(9) << uint16(0) << uint8(0) << uint8(8) << uint16(Time) << uint16(0);
	Send(&result);
}
#pragma endregion

#pragma region CUser::isQuestEventUser()
bool CUser::isQuestEventUser()
{
	_TEMPLE_QUEST_USER * pEventUser = g_pMain->m_MonsterStoneTeleportListArray.GetData(GetSocketID());

	if (pEventUser != nullptr)
		return true;

	return false;
}
#pragma endregion