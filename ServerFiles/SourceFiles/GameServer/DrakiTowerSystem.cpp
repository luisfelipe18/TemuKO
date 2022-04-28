#include "stdafx.h"
#include "DBAgent.h"

#pragma region Draki Tower System
#pragma region CUser::DrakiTowerList()
void CUser::DrakiTowerList()
{
	Packet result(WIZ_EVENT, uint8(TEMPLE_DRAKI_TOWER_LIST));
	g_DBAgent.ReqDrakiTowerList(result, this);
	Send(&result);
}
#pragma endregion

#pragma region CUser::DrakiTowerTempleEnter(Packet & pkt)
void CUser::DrakiTowerTempleEnter(Packet & pkt)
{
	/*
	TEMPLE_DRAKI_TOWER_ENTER opcode.
	1.Teleporting is disabled in this zone
	2.You are in an instance dungeon.
	3.List
	4.Clear info and entrance info does not match.
	5.You have either spent all 3 entrance limitsh or do not have entrance item.
	6.Instance generation failed.
	7.You do not have the enterance item.
	8.You cannot enter right now
	9.You cannot enter while you are dead.
	10.Server internal instance index info does not match
	11.Packet instance index info does not match.
	12.No user information.
	*/

	uint32 nItemID;
	uint8 EnterDungeon;
	Packet result(WIZ_EVENT, uint8(TEMPLE_DRAKI_TOWER_ENTER));
	pkt >> nItemID >> EnterDungeon;

	if (g_pMain->isWarOpen())
	{
		result << uint32(8);
		Send(&result);
		return;
	}

	if (isDead())
	{
		result << uint32(9);
		Send(&result);
		return;
	}

	if (GetEventRoom() > 0 
		|| GetZoneID() == ZONE_DRAKI_TOWER)
	{
		result << uint32(2);
		Send(&result);
		return;
	}

	bool Dungeon = (EnterDungeon >= 1 && EnterDungeon <= 5);
	if (!Dungeon)
	{
		result << uint32(1);
		Send(&result);
		return;
	}

	if (nItemID == CERTIFIKAOFDRAKI 
		&& !CheckExistItem(CERTIFIKAOFDRAKI, 1))
	{
		result << uint32(7);
		Send(&result);
		return;
	}

	if (m_bDrakiEnteranceLimit <= 0 
		&& !CheckExistItem(CERTIFIKAOFDRAKI, 1))
	{
		result << uint32(5);
		Send(&result);
		return;
	}

	if (nItemID == CERTIFIKAOFDRAKI 
		&& CheckExistItem(CERTIFIKAOFDRAKI, 1))
		m_bDrakiEnteranceLimit = 1;

	if (m_bDrakiEnteranceLimit >= 1)
	{
		int8 results = g_DBAgent.DrakiSignCreateDatas(GetName(), this);
		if (results != 1)
		{
			result << uint32(10);
			Send(&result);
			return;
		}

		uint16 SignRoom = 0;
		for (int i = 1; i <= EVENTMAXROOM; i++)
		{
			_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_tDrakiTowerStart == true)
				continue;

			if (pRoomInfo->m_tDrakiTowerStart == false)
			{
				SignRoom = pRoomInfo->m_tDrakiRoomID;
				break;
			}
		}

		_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(SignRoom);
		if (pRoomInfo == nullptr
			|| pRoomInfo->m_tDrakiTowerStart == true)
		{
			result << uint32(6);
			Send(&result);
			return;
		}

		pRoomInfo->m_tDrakiTimer = UNIXTIME - (pRoomInfo->m_sDrakiStage == 1 ? 0 : pRoomInfo->m_sDrakiStage * 80);
		pRoomInfo->m_tDrakiTimer = UNIXTIME;
		pRoomInfo->m_tDrakiSubTimer = UNIXTIME + 300;

		if (m_bDrakiEnteranceLimit-- <= 0)
			m_bDrakiEnteranceLimit = 0;

		pRoomInfo->m_sDrakiTempStage = m_bDrakiStage;
		pRoomInfo->m_sDrakiStage = EnterDungeon;
		pRoomInfo->m_sDrakiSubStage = 1;
		pRoomInfo->m_tDrakiTowerStart = true;
		pRoomInfo->m_tDrakiRoomCloseTimer = 7200;
		m_bEventRoom = SignRoom;

		_DRAKI_TOWER_ROOM_USER_LIST* pDrakiRoomUser = new  _DRAKI_TOWER_ROOM_USER_LIST;
		pDrakiRoomUser->m_DrakiRoomID = SignRoom;
		pDrakiRoomUser->strUserID = GetName();

		if (!pRoomInfo->m_UserList.PutData(pDrakiRoomUser->m_DrakiRoomID, pDrakiRoomUser))
			delete pDrakiRoomUser;

		switch (pRoomInfo->m_sDrakiStage)
		{
		case 1:
			pRoomInfo->m_isDrakiStageChange = true;
			ZoneChange(ZONE_DRAKI_TOWER, 40, 451);
			break;
		case 2:
			ZoneChange(ZONE_DRAKI_TOWER, 78, 58);
			break;
		case 3:
			ZoneChange(ZONE_DRAKI_TOWER, 315, 439);
			break;
		case 4:
			ZoneChange(ZONE_DRAKI_TOWER, 304, 271);
			break;
		case 5:
			ZoneChange(ZONE_DRAKI_TOWER, 71, 195);
			break;
		}
		if (nItemID == CERTIFIKAOFDRAKI && CheckExistItem(CERTIFIKAOFDRAKI, 1))
			RobItem(CERTIFIKAOFDRAKI, 1);

		g_DBAgent.UpdateDrakiTowerLimitLastUpdate(GetName(), m_bDrakiEnteranceLimit);
	}
	else
	{
		result << uint32(5);
		Send(&result);
	}
}
#pragma endregion

#pragma region CUser::DrakiRiftChange(uint16 DrakiStage, uint16 DrakiSubStage)
void CUser::DrakiRiftChange(uint16 DrakiStage, uint16 DrakiSubStage)
{
	bool DrakiStateCheck = (DrakiStage >= 1 && DrakiStage <= 5);
	bool DrakiSubStateCheck = (DrakiSubStage >= 1 && DrakiStage <= 8);

	if (!DrakiStateCheck || !DrakiSubStateCheck)
		return;

	uint16 TimeLimit = 300;
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());

	if (pRoomInfo == nullptr)
		return;

	pRoomInfo->m_sDrakiStage = DrakiStage;
	pRoomInfo->m_sDrakiSubStage = DrakiSubStage;
	pRoomInfo->m_tDrakiSubTimer = UNIXTIME + TimeLimit;
	pRoomInfo->m_tDrakiTimer = uint32(UNIXTIME - pRoomInfo->m_tDrakiSpareTimer);

	Packet result(WIZ_SELECT_MSG);
	result << uint16(0)
		<< uint8(7)
		<< uint64(0)
		<< uint32(0x0A)
		<< uint8(233)
		<< uint16(TimeLimit)
		<< uint16(UNIXTIME - pRoomInfo->m_tDrakiTimer);
	Send(&result);

	result.Initialize(WIZ_EVENT);
	result << uint8(TEMPLE_DRAKI_TOWER_TIMER)
		<< uint8(233)
		<< uint8(3)
		<< uint16(pRoomInfo->m_sDrakiStage)
		<< uint16(pRoomInfo->m_sDrakiSubStage)
		<< uint32(TimeLimit)
		<< uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);
	Send(&result);

	result.Initialize(WIZ_BIFROST);
	result << uint8(5)
		<< uint16(TimeLimit);
	Send(&result);

	SummonDrakiMonsters(SelectDrakiRoom());
}
#pragma endregion

#pragma region CUser::SendDrakiTempleDetail(bool send) 
void CUser::SendDrakiTempleDetail(bool MonsterSummon)
{
	if (MonsterSummon)
	{
		uint16 TimeLimit = 300;
		_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());

		if (pRoomInfo == nullptr)
			return;

		pRoomInfo->m_tDrakiSubTimer = UNIXTIME + TimeLimit;

		Packet result(WIZ_SELECT_MSG);
		result << uint16(0)
			<< uint8(7)
			<< uint64(0)
			<< uint32(0x0A)
			<< uint8(233)
			<< uint16(TimeLimit)
			<< uint16(UNIXTIME - pRoomInfo->m_tDrakiTimer);
		Send(&result);

		result.Initialize(WIZ_EVENT);
		result << uint8(TEMPLE_DRAKI_TOWER_TIMER)
			<< uint8(233)
			<< uint8(3)
			<< uint16(pRoomInfo->m_sDrakiStage)
			<< uint16(pRoomInfo->m_sDrakiSubStage)
			<< uint32(TimeLimit)
			<< uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);
		Send(&result);

		result.Initialize(WIZ_BIFROST);
		result << uint8(5)
			<< uint16(TimeLimit);
		Send(&result);

		SummonDrakiMonsters(SelectDrakiRoom());
	}
	else
	{
		uint16 TimeLimit = 0;
		_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());

		if (pRoomInfo == nullptr)
			return;

		pRoomInfo->m_tDrakiSubTimer = UNIXTIME + 180;
		pRoomInfo->m_tDrakiSpareTimer = uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);

		if (pRoomInfo->m_sDrakiStage == 5 && pRoomInfo->m_sDrakiSubStage == 8)
		{
			if (pRoomInfo->m_isDrakiStageChange == true)
			{
				uint32 DrakiTowerFinishedTime = uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);

				if (DrakiTowerFinishedTime <= 1200)
					AchieveWarCountAdd(UserAchieveWarTypes::AchieveDrakiTowerFinished, 0, nullptr);
			}
			TimeLimit = -1;
		}
		else
			TimeLimit = 180;

		Packet result(WIZ_SELECT_MSG);
		result << uint16(0)
			<< uint8(7)
			<< uint64(0)
			<< uint32(0x0A)
			<< uint8(233)
			<< uint16(TimeLimit)
			<< uint16(UNIXTIME - pRoomInfo->m_tDrakiTimer);
		Send(&result);

		result.Initialize(WIZ_EVENT);
		result << uint8(TEMPLE_DRAKI_TOWER_TIMER)
			<< uint8(233)
			<< uint8(3)
			<< uint16(0)
			<< uint16(0)
			<< uint32(TimeLimit)
			<< uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);
		Send(&result);

		result.Initialize(WIZ_BIFROST);
		result << uint8(5)
			<< uint16(TimeLimit);
		Send(&result);

		SummonDrakiMonsters(SelectNpcDrakiRoom());
		DrakiTowerSavedUserInfo();
	}
}
#pragma endregion

#pragma region CUser::SelectDrakiRoom()
uint8 CUser::SelectDrakiRoom()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return 0;

	foreach_stlmap(itr, g_pMain->m_DrakiRoomListArray)
	{
		if (itr->second->bDrakiStage == pRoomInfo->m_sDrakiStage &&
			itr->second->bDrakiSubStage == pRoomInfo->m_sDrakiSubStage &&
			itr->second->bDrakiNpcStage == 0)
			return (uint8)itr->second->bIndex;
	}
	return 0;
}
#pragma endregion

#pragma region CUser::SelectNpcDrakiRoom()
uint8 CUser::SelectNpcDrakiRoom()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return 0;

	foreach_stlmap(itr, g_pMain->m_DrakiRoomListArray)
	{
		if (itr->second->bDrakiStage == pRoomInfo->m_sDrakiStage &&
			itr->second->bDrakiSubStage == pRoomInfo->m_sDrakiSubStage &&
			itr->second->bDrakiNpcStage == 1)
			return (uint8)itr->second->bIndex;
	}
	return 0;
}
#pragma endregion

#pragma region CUser::SummonDrakiMonsters(uint8 RoomIndex)
void CUser::SummonDrakiMonsters(uint8 RoomIndex)
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	foreach_stlmap(itr, g_pMain->m_DrakiMonsterListArray)
	{
		DRAKI_MONSTER_LIST* pDrakiMonster = itr->second;

		if (pDrakiMonster == nullptr || pDrakiMonster->bDrakiStage != RoomIndex)
			continue;

		if (pDrakiMonster->bDrakiStage == RoomIndex)
			g_pMain->SpawnEventNpc(pDrakiMonster->sMonsterID, pDrakiMonster->bMonster == 0 ? true : false, ZONE_DRAKI_TOWER, pDrakiMonster->sPosX, 0, pDrakiMonster->sPosZ, 1, 0, DRAKI_TOWER_MONSTER_KILL_TIME, pDrakiMonster->bMonster == 0 ? 3 : 0, -1, GetEventRoom(), (uint8)pDrakiMonster->sDirection, 1, 0, SpawnEventType::DrakiTowerSummon, GetEventRoom());
	}
}
#pragma endregion

#pragma region CUser::ChangeDrakiMode()
void CUser::ChangeDrakiMode()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	uint8 m_RoomID = SelectDrakiRoom();
	pRoomInfo->m_bSavedDrakiStage = m_RoomID;
	m_iDrakiTime = int(UNIXTIME - pRoomInfo->m_tDrakiTimer);

	DRAKI_ROOM_LIST* DrakiRooms = g_pMain->m_DrakiRoomListArray.GetData(++m_RoomID);
	if (DrakiRooms == nullptr)
		return;

	pRoomInfo->m_sDrakiStage = DrakiRooms->bDrakiStage;
	pRoomInfo->m_sDrakiSubStage = DrakiRooms->bDrakiSubStage;

	switch (DrakiRooms->bDrakiNpcStage)
	{
	case 0:
		SendDrakiTempleDetail(true);
		break;
	case 1:
		SendDrakiTempleDetail(false);
		break;
	}
}
#pragma endregion

#pragma region CUser::DrakiTowerSavedUserInfo()
void CUser::DrakiTowerSavedUserInfo()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());

	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_isDrakiStageChange == true)
	{
		uint32 Time = uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer);
		uint8 m_RoomID = SelectDrakiRoom();

		DRAKI_ROOM_LIST* DrakiRooms = g_pMain->m_DrakiRoomListArray.GetData(++m_RoomID);
		if (DrakiRooms == nullptr)
			return;

		m_bDrakiStage = DrakiRooms->bIndex;
		g_DBAgent.UpdateDrakiTowerData(Time, DrakiRooms->bIndex, GetName());
	}
}
#pragma endregion

#pragma region CUser::DrakiTowerNpcOut()
void CUser::DrakiTowerNpcOut()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());

	if (pRoomInfo == nullptr)
		return;

	CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(GetZoneID());

	if (zoneitrThread == nullptr)
		return;

	//kaç kere dönecek bu o zonede ne kadar npc varsa

	foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc *pNpc = TO_NPC(itr->second);

		if (pNpc == nullptr)
			continue;

		if (pNpc->isDead()
			|| pNpc->GetZoneID() != ZONE_DRAKI_TOWER
			|| pNpc->isMonster()
			|| pNpc->GetEventRoom() != GetEventRoom())
			continue;

		pNpc->DeadReq();
		pNpc->SendInOut(INOUT_OUT, pNpc->GetX(), pNpc->GetZ(), pNpc->GetY());
	}
}
#pragma endregion

#pragma region CUser::DrakiTowerKickOuts()
void CUser::DrakiTowerKickOuts()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	Packet result(WIZ_EVENT);
	result << uint8(TEMPLE_DRAKI_TOWER_OUT1)
		<< uint8(0x0C)
		<< uint8(0x04)
		<< uint8(0x00)
		<< uint8(0x14)
		<< uint16(0)
		<< uint8(0);
	Send(&result);

	pRoomInfo->m_tDrakiOutTimer = UNIXTIME + 20;
	pRoomInfo->m_bOutTimer = true;
	DrakiTowerSavedUserInfo();
}
#pragma endregion

#pragma region CUser::DrakiTowerKickTimer()
void CUser::DrakiTowerKickTimer()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_bTownOutTimer == true
		&& pRoomInfo->m_tDrakiTownOutTimer <= UNIXTIME)
	{
		pRoomInfo->m_tDrakiTimer = UNIXTIME;
		pRoomInfo->m_tDrakiSubTimer = UNIXTIME;
		pRoomInfo->m_tDrakiOutTimer = UNIXTIME;
		pRoomInfo->m_tDrakiTownOutTimer = UNIXTIME;
		pRoomInfo->m_bSavedDrakiMaxStage = 0;
		pRoomInfo->m_bSavedDrakiStage = 0;
		pRoomInfo->m_bSavedDrakiTime = 0;
		pRoomInfo->m_bSavedDrakiLimit = 0;
		pRoomInfo->m_sDrakiStage = 0;
		pRoomInfo->m_sDrakiSubStage = 0;
		pRoomInfo->m_sDrakiTempStage = 0;
		pRoomInfo->m_sDrakiTempSubStage = 0;
		pRoomInfo->m_bDrakiMonsterKill = 0;
		pRoomInfo->m_tDrakiSpareTimer = 0;
		pRoomInfo->m_tDrakiTowerStart = false;
		pRoomInfo->m_bOutTimer = false;
		pRoomInfo->m_isDrakiStageChange = false;
		pRoomInfo->m_bTownRequest = 0;
		pRoomInfo->m_bTownOutTimer = false;
		m_bEventRoom = 0;

		if (GetLevel() >= 1 && GetLevel() <= 34)
			ZoneChange(ZONE_MORADON, 0, 0);
		else
			ZoneChange(GetNation(), 0, 0);
	}

	if (pRoomInfo->m_tDrakiSubTimer == UNIXTIME)
		DrakiTowerKickOuts();

	if (pRoomInfo->m_bOutTimer == true
		&& pRoomInfo->m_tDrakiOutTimer <= UNIXTIME)
	{
		pRoomInfo->m_tDrakiTimer = UNIXTIME;
		pRoomInfo->m_tDrakiSubTimer = UNIXTIME;
		pRoomInfo->m_tDrakiOutTimer = UNIXTIME;
		pRoomInfo->m_tDrakiTownOutTimer = UNIXTIME;
		pRoomInfo->m_bSavedDrakiMaxStage = 0;
		pRoomInfo->m_bSavedDrakiStage = 0;
		pRoomInfo->m_bSavedDrakiTime = 0;
		pRoomInfo->m_bSavedDrakiLimit = 0;
		pRoomInfo->m_sDrakiStage = 0;
		pRoomInfo->m_sDrakiSubStage = 0;
		pRoomInfo->m_sDrakiTempStage = 0;
		pRoomInfo->m_sDrakiTempSubStage = 0;
		pRoomInfo->m_bDrakiMonsterKill = 0;
		pRoomInfo->m_tDrakiSpareTimer = 0;
		pRoomInfo->m_tDrakiTowerStart = false;
		pRoomInfo->m_bOutTimer = false;
		pRoomInfo->m_isDrakiStageChange = false;
		pRoomInfo->m_bTownRequest = 0;
		pRoomInfo->m_bTownOutTimer = false;
		m_bEventRoom = 0;

		if (GetLevel() >= 1 && GetLevel() <= 34)
			ZoneChange(ZONE_MORADON, 0, 0);
		else
			ZoneChange(GetNation(), 0, 0);
	}
}
#pragma endregion

#pragma region CUser::DrakiTowerTown()
void CUser::DrakiTowerTown()
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_bTownRequest == true)
		return;

	Packet result(WIZ_EVENT);
	result << uint8(TEMPLE_DRAKI_TOWER_OUT1)
		<< uint8(0x0C)
		<< uint8(0x04)
		<< uint8(0x00)
		<< uint8(0x14)
		<< uint16(0)
		<< uint8(0);
	Send(&result);

	result.Initialize(WIZ_EVENT);
	result << uint8(TEMPLE_DRAKI_TOWER_OUT2)
		<< uint8(0x0C)
		<< uint8(0x04)
		<< uint16(pRoomInfo->m_sDrakiStage)
		<< uint16(pRoomInfo->m_sDrakiSubStage)
		<< uint32(UNIXTIME - pRoomInfo->m_tDrakiTimer)
		<< uint8(1);
	Send(&result);

	pRoomInfo->m_tDrakiTownOutTimer = UNIXTIME + 20;
	pRoomInfo->m_bTownOutTimer = true;
	pRoomInfo->m_bTownRequest = true;
	DrakiTowerSavedUserInfo();
}
#pragma endregion

#pragma region CUser::DrakiTowerLimitReset()
void CGameServerDlg::DrakiTowerLimitReset()
{
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;
	int nSec = g_localTime.tm_sec;

	if (nHour == 18 && nMin == 0 && nSec == 0)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser *pUser = TO_USER(itr->second);
			if (pUser == nullptr
				|| !pUser->isInGame())
				continue;

			pUser->m_bDrakiEnteranceLimit = 3;
		}

		g_DBAgent.UpdateDaysDrakiTowerLimit();
	}
}
#pragma endregion

#pragma region CGameServerDlg::DrakiTowerRoomCloseTimer()
void CGameServerDlg::DrakiTowerRoomCloseTimer()
{
	if (m_DrakiMonsterListArray.GetSize() > 0)
	{
		for (int i = 1; i <= EVENTMAXROOM; i++)
		{
			_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_tDrakiTowerStart == false)
				continue;

			uint16 EventRoomIndex = i;

			if (pRoomInfo->m_tDrakiRoomCloseTimer > 0)
				pRoomInfo->m_tDrakiRoomCloseTimer--;

			if (pRoomInfo->m_tDrakiTowerStart == true
				&& pRoomInfo->m_tDrakiRoomCloseTimer <= 0)
				DrakiTowerRoomCloseUserisOut(EventRoomIndex);
		}
	}
}
#pragma endregion 

#pragma region CGameServerDlg::DrakiTowerRoomCloseUserisOut(uint16 EventRoom)
void CGameServerDlg::DrakiTowerRoomCloseUserisOut(uint16 EventRoom)
{
	_DRAKI_TOWER_INFO* pRoomInfo = g_pMain->m_MonsterDrakiTowerList.GetData(EventRoom);
	if (pRoomInfo == nullptr)
		return;

	foreach_stlmap(itrUser, pRoomInfo->m_UserList)
	{
		_DRAKI_TOWER_ROOM_USER_LIST * pDrakiRoomUser = itrUser->second;
		if (pDrakiRoomUser == nullptr
			|| pDrakiRoomUser->m_DrakiRoomID != EventRoom)
			continue;

		if (pDrakiRoomUser->m_DrakiRoomID == EventRoom)
		{
			CUser * pUser = g_pMain->GetUserPtr(pDrakiRoomUser->strUserID, TYPE_CHARACTER);
			if (pUser == nullptr
				|| !pUser->isInGame()
				|| pUser->GetZoneID() != ZONE_DRAKI_TOWER
				|| pUser->GetEventRoom() <= 0
				|| pUser->GetEventRoom() != EventRoom)
				continue;

			pUser->ZoneChange(ZONE_MORADON, 0.f, 0.f);
			pUser->m_bEventRoom = 0;
		}
	}


	pRoomInfo->m_tDrakiTowerStart = false;
	pRoomInfo->m_tDrakiTimer = UNIXTIME;
	pRoomInfo->m_tDrakiSubTimer = UNIXTIME;
	pRoomInfo->m_tDrakiOutTimer = UNIXTIME;
	pRoomInfo->m_tDrakiTownOutTimer = UNIXTIME;
	pRoomInfo->m_bSavedDrakiMaxStage = 0;
	pRoomInfo->m_bSavedDrakiStage = 0;
	pRoomInfo->m_bSavedDrakiTime = 0;
	pRoomInfo->m_bSavedDrakiLimit = 0;
	pRoomInfo->m_sDrakiStage = 0;
	pRoomInfo->m_sDrakiSubStage = 0;
	pRoomInfo->m_sDrakiTempStage = 0;
	pRoomInfo->m_sDrakiTempSubStage = 0;
	pRoomInfo->m_bDrakiMonsterKill = 0;
	pRoomInfo->m_tDrakiSpareTimer = 0;
	pRoomInfo->m_bOutTimer = false;
	pRoomInfo->m_isDrakiStageChange = false;
	pRoomInfo->m_bTownRequest = 0;
	pRoomInfo->m_tDrakiRoomCloseTimer = 7200;
	pRoomInfo->m_bTownOutTimer = false;
	pRoomInfo->m_UserList.DeleteData(EventRoom);

	CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(ZONE_DRAKI_TOWER);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc *pNpc = TO_NPC(itr->second);
		if (pNpc == nullptr)
			continue;

		if (pNpc->isDead()
			|| pNpc->GetZoneID() != ZONE_DRAKI_TOWER
			|| pNpc->GetEventRoom() != EventRoom)
			continue;

		pNpc->Dead();
	}
	zoneitrThread->m_arNpcArray.m_lock.unlock();
}
#pragma endregion
#pragma endregion