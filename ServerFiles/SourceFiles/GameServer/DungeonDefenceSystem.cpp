#include "stdafx.h"
#include "../shared/KOSocketMgr.h"

using namespace std;

#pragma region Dungeon Defence System
#pragma region void CUser::DungeonDefenceSign()
void CUser::DungeonDefenceSign()
{
	/*
	0: System Error
	1: Instance generation failed : OK
	2: You can only enter when all party members are at Moradon : OK
	3: Another instance is being created
	4: You cannot enter when one of your party members is dead : OK
	5: There is no party available : OK
	6: It is not a Full Moon Rift party : OK
	7: You cannot enter. One of your party members does not have Full Moon Rift Voucher : OK*/

	Packet x(WIZ_EVENT, uint8(TEMPLE_EVENT_DUNGEON_SIGN));
	if (!isInParty())
	{
		x << uint8(5);
		Send(&x);
		return;
	}

	if (!isPartyLeader())
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	short partyUsers[MAX_PARTY_USERS];
	for (int i = 0; i < MAX_PARTY_USERS; i++)
		partyUsers[i] = pParty->uid[i];

	uint16 PartyNemberCount = 0;
	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(partyUsers[i]);
		if (pUser == nullptr)
			continue;

		if (pUser->GetEventRoom() > 0)
		{
			x << uint8(1);
			pUser->Send(&x);
			return;
		}
		if (pUser->m_sUserPartyType == 0)
		{
			x << uint8(6);
			pUser->Send(&x);
			return;
		}
		if (!pUser->isInMoradon())
		{
			x << uint8(2);
			pUser->Send(&x);
			return;
		}
		if (pUser->isDead())
		{
			x << uint8(4);
			pUser->Send(&x);
			return;
		}
		if (!pUser->CheckExistItem(DUNGEON_DEFENCE_RIFT_ITEM, 1))
		{
			x << uint8(7);
			pUser->Send(&x);
			return;
		}

		if (pUser->isInGame() && pUser->isInParty())
			PartyNemberCount++;
	}

	if (PartyNemberCount == 0
		|| PartyNemberCount == 1)
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	uint16 LeaderEventRoom = 0;
	for (int i = 1; i <= EVENTMAXROOM; i++)
	{
		_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(i);
		if (pRoomInfo == nullptr
			|| pRoomInfo->m_DefenceisStarted == true)
			continue;

		if (pRoomInfo->m_DefenceisStarted == false)
		{
			LeaderEventRoom = pRoomInfo->m_DefenceRoomID;
			break;
		}
	}

	if (LeaderEventRoom == 0)
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(LeaderEventRoom);
	if (pRoomInfo == nullptr
		|| pRoomInfo->m_DefenceisStarted == true)
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	bool Easy = (PartyNemberCount >= 2 && PartyNemberCount <= 3);
	bool Normal = (PartyNemberCount >= 4 && PartyNemberCount <= 7);
	bool Hard = (PartyNemberCount >= 8);

	if (Easy)
	{
		pRoomInfo->m_DefenceStageID = 1;
		pRoomInfo->m_DefenceDifficulty = 1;
	}
	else if (Normal)
	{
		pRoomInfo->m_DefenceStageID = 6;
		pRoomInfo->m_DefenceDifficulty = 2;
	}
	else if (Hard)
	{
		pRoomInfo->m_DefenceStageID = 18;
		pRoomInfo->m_DefenceDifficulty = 3;
	}
	else
	{
		x << uint8(1);
		Send(&x);
		return;
	}

	pRoomInfo->m_DefenceKillCount = 0;
	pRoomInfo->m_DefenceSpawnTime = 60;
	pRoomInfo->m_DefenceRoomClose = 7200;
	pRoomInfo->m_DefenceOutTime = 0;
	pRoomInfo->m_DefenceisStarted = true;
	pRoomInfo->m_DefenceMonsterBeginnerSpawned = true;
	pRoomInfo->m_DefenceMonsterSpawned = false;
	pRoomInfo->m_DefenceisFinished = false;

	_DUNGEON_DEFENCE_ROOM_USER_LIST* pDungeonEventUser = new  _DUNGEON_DEFENCE_ROOM_USER_LIST;
	pDungeonEventUser->m_DefenceRoomID = LeaderEventRoom;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(partyUsers[i]);
		if (pUser == nullptr)
			continue;

		pDungeonEventUser->strUserID[i] = pUser->m_strUserID;
		pUser->m_bEventRoom = LeaderEventRoom;
		pUser->ZoneChange(ZONE_DUNGEON_DEFENCE, 0.f, 0.f);
		pUser->RobItem(DUNGEON_DEFENCE_RIFT_ITEM, 1);
	}

	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 199, 0, 200, 1, 0, -1, 3, -1,LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);
	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 52, 0, 124, 1, 0, -1, 3, -1, LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);
	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 211, 0, 38, 1, 0, -1, 3, -1, LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);
	g_pMain->SpawnEventNpc(31740, false, ZONE_DUNGEON_DEFENCE, 167, 0, 212, 1, 0, -1, 3, -1,LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);
	g_pMain->SpawnEventNpc(31739, false, ZONE_DUNGEON_DEFENCE, 65, 0, 134, 1, 0, -1, 3, -1, LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);
	g_pMain->SpawnEventNpc(31738, false, ZONE_DUNGEON_DEFENCE, 196, 0, 54, 1, 0, -1, 3, -1, LeaderEventRoom, 0, 1, 0, SpawnEventType::DungeonDefencSummon, LeaderEventRoom);

	if (!pRoomInfo->m_UserList.PutData(pDungeonEventUser->m_DefenceRoomID, pDungeonEventUser))
		delete pDungeonEventUser;


	/* Tekli giriþ test.*/
	/*uint16 LeaderEventRoom = 0;
	for (int i = 1; i <= EVENTMAXROOM; i++)
	{
		_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(i);
		if (pRoomInfo == nullptr
			|| pRoomInfo->m_DefenceisStarted == true)
			continue;

		if (pRoomInfo->m_DefenceisStarted == false)
		{
			LeaderEventRoom = pRoomInfo->m_DefenceRoomID;
			break;
		}
	}


	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(LeaderEventRoom);
	pRoomInfo->m_DefenceKillCount = 0;
	pRoomInfo->m_DefenceSpawnTime = 60;
	pRoomInfo->m_DefenceRoomClose = 7200;
	pRoomInfo->m_DefenceOutTime = 0;
	pRoomInfo->m_DefenceisStarted = true;
	pRoomInfo->m_DefenceMonsterBeginnerSpawned = true;
	pRoomInfo->m_DefenceMonsterSpawned = false;
	pRoomInfo->m_DefenceisFinished = false;
	pRoomInfo->m_DefenceDifficulty = 2;
	pRoomInfo->m_DefenceStageID = 6;


	_DUNGEON_DEFENCE_ROOM_USER_LIST* pDungeonEventUser = new  _DUNGEON_DEFENCE_ROOM_USER_LIST;
	pDungeonEventUser->m_DefenceRoomID = LeaderEventRoom;
	pDungeonEventUser->strUserID[1] = GetName();
	if (!pRoomInfo->m_UserList.PutData(pDungeonEventUser->m_DefenceRoomID, pDungeonEventUser))
		delete pDungeonEventUser;

	ZoneChange(ZONE_DUNGEON_DEFENCE, 0.f, 0.f);
	m_bEventRoom = LeaderEventRoom;
	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 199, 0, 200, 1, 0, -1, 3, -1, LeaderEventRoom, 0);
	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 52, 0, 124, 1, 0, -1, 3, -1, LeaderEventRoom, 0);
	g_pMain->SpawnEventNpc(31737, false, ZONE_DUNGEON_DEFENCE, 211, 0, 38, 1, 0, -1, 3, -1, LeaderEventRoom, 0);
	g_pMain->SpawnEventNpc(31740, false, ZONE_DUNGEON_DEFENCE, 167, 0, 212, 1, 0, -1, 3, -1, LeaderEventRoom, 0);
	g_pMain->SpawnEventNpc(31739, false, ZONE_DUNGEON_DEFENCE, 65, 0, 134, 1, 0, -1, 3, -1, LeaderEventRoom, 0);
	g_pMain->SpawnEventNpc(31738, false, ZONE_DUNGEON_DEFENCE, 196, 0, 54, 1, 0, -1, 3, -1, LeaderEventRoom, 0);*/
}
#pragma endregion

#pragma region void CUser::DungeonDefenceRobItemSkills()
void CUser::DungeonDefenceRobItemSkills()
{
	if (GetItemCount(MONSTER_COIN_ITEM) > 0)
		RobItem(MONSTER_COIN_ITEM, GetItemCount(MONSTER_COIN_ITEM));
}
#pragma endregion

#pragma region void CGameServerDlg::DungeonDefenceTimer()
void CGameServerDlg::DungeonDefenceTimer()
{
	if (g_pMain->m_DungeonDefenceRoomListArray.GetSize() > 0)
	{
		for (int i = 1; i <= EVENTMAXROOM; i++)
		{
			_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(i);
			if (pRoomInfo == nullptr
				|| pRoomInfo->m_DefenceisStarted == false)
				continue;

			uint16 EventRoomIndex = i;

			if (pRoomInfo->m_DefenceSpawnTime > 0)
				pRoomInfo->m_DefenceSpawnTime--;

			if (pRoomInfo->m_DefenceSpawnTime <= 0
				&& (pRoomInfo->m_DefenceMonsterBeginnerSpawned == true
					|| pRoomInfo->m_DefenceMonsterSpawned == true))
			{
				SendDungeonDefenceDetail(EventRoomIndex);
				if (pRoomInfo->m_DefenceMonsterBeginnerSpawned == true)
					pRoomInfo->m_DefenceMonsterBeginnerSpawned = false;
			}

			if (pRoomInfo->m_DefenceRoomClose > 0)
				pRoomInfo->m_DefenceRoomClose--;

			if (pRoomInfo->m_DefenceOutTime > 0)
				pRoomInfo->m_DefenceOutTime--;

			if ((pRoomInfo->m_DefenceRoomClose <= 0
				&& pRoomInfo->m_DefenceisStarted == true)
				|| (pRoomInfo->m_DefenceOutTime <= 0
					&& pRoomInfo->m_DefenceisFinished == true))
			{
				if (pRoomInfo->m_DefenceRoomClose <= 0
					&& pRoomInfo->m_DefenceisStarted == true)
					pRoomInfo->m_DefenceisStarted = false;

				if (pRoomInfo->m_DefenceOutTime <= 0
					&& pRoomInfo->m_DefenceisFinished == true)
					pRoomInfo->m_DefenceisFinished = false;

				DungeonDefenceUserisOut(EventRoomIndex);
			}
		}
	}
}
#pragma endregion

#pragma region void CGameServerDlg::DungeonDefenceUserisOut(uint16 EventRoom)
void CGameServerDlg::DungeonDefenceUserisOut(uint16 EventRoom)
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(EventRoom);
	if (pRoomInfo == nullptr)
		return;

	foreach_stlmap(itrUser, pRoomInfo->m_UserList)
	{
		_DUNGEON_DEFENCE_ROOM_USER_LIST * pDungeonUser = itrUser->second;
		if (pDungeonUser == nullptr
			|| pDungeonUser->m_DefenceRoomID != EventRoom)
			continue;

		if (pDungeonUser->m_DefenceRoomID == EventRoom)
		{
			for (int i = 0; i < MAX_PARTY_USERS; i++)
			{
				CUser * pUser = g_pMain->GetUserPtr(pDungeonUser->strUserID[i], TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| pUser->GetZoneID() != ZONE_DUNGEON_DEFENCE
					|| pUser->GetEventRoom() <= 0
					|| pUser->GetEventRoom() != EventRoom)
					continue;

				pUser->ZoneChange(ZONE_MORADON, 0.f, 0.f);
				pUser->m_bEventRoom = 0;
			}
		}
	}

	pRoomInfo->m_DefenceKillCount = 0;
	pRoomInfo->m_DefenceSpawnTime = 60;
	pRoomInfo->m_DefenceRoomClose = 7200;
	pRoomInfo->m_DefenceOutTime = 0;
	pRoomInfo->m_DefenceisStarted = false;
	pRoomInfo->m_DefenceMonsterBeginnerSpawned = false;
	pRoomInfo->m_DefenceMonsterSpawned = false;
	pRoomInfo->m_DefenceisFinished = false;
	pRoomInfo->m_DefenceStageID = 0;
	pRoomInfo->m_DefenceDifficulty = 0;
	pRoomInfo->m_UserList.DeleteData(EventRoom);

	CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(ZONE_DUNGEON_DEFENCE);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc *pNpc = TO_NPC(itr->second);
		if (pNpc == nullptr)
			continue;

		if (pNpc->isDead()
			|| pNpc->GetZoneID() != ZONE_DUNGEON_DEFENCE
			|| pNpc->GetEventRoom() != EventRoom)
			continue;

		pNpc->DeadReq();
	}
	zoneitrThread->m_arNpcArray.m_lock.unlock();
}
#pragma endregion

#pragma region void CGameServerDlg::SendDungeonDefenceDetail(uint16 EventRoom)
void CGameServerDlg::SendDungeonDefenceDetail(uint16 EventRoom)
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(EventRoom);
	if (pRoomInfo == nullptr)
		return;

	switch (pRoomInfo->m_DefenceDifficulty)
	{
	case 1:
	{
		uint8 StageID = (pRoomInfo->m_DefenceStageID);
		Packet x(WIZ_EVENT, uint8(TEMPLE_EVENT_STAGE_COUNTER));
		x << uint8(5) /*Max Stage */ << StageID; /*in Stage*/
		Send_Zone(&x, ZONE_DUNGEON_DEFENCE, nullptr, Nation::ALL, EventRoom);
	}
	break;
	case 2:
	{
		uint8 StageID = (pRoomInfo->m_DefenceStageID - 5);
		Packet x(WIZ_EVENT, uint8(TEMPLE_EVENT_STAGE_COUNTER));
		x << uint8(12) /*Max Stage */ << StageID; /*in Stage*/
		Send_Zone(&x, ZONE_DUNGEON_DEFENCE, nullptr, Nation::ALL, EventRoom);
	}
	break;
	case 3:
	{
		uint8 StageID = (pRoomInfo->m_DefenceStageID - 17);
		Packet x(WIZ_EVENT, uint8(TEMPLE_EVENT_STAGE_COUNTER));
		x << uint8(18) /*Max Stage */ << StageID; /*in Stage*/
		Send_Zone(&x, ZONE_DUNGEON_DEFENCE, nullptr, Nation::ALL, EventRoom);
	}
	break;
	default:
		printf("Dungeon Defence Invalid Difficulty. Difficulty No(%d) \n", pRoomInfo->m_DefenceDifficulty);
		break;
	}

	pRoomInfo->m_DefenceMonsterSpawned = false;
	SummonDungeonDefenceMonsters(EventRoom);
}
#pragma endregion

#pragma region void CGameServerDlg::DungeonDefenceSelectStage(uint16 EventRoom)
uint8 CGameServerDlg::DungeonDefenceSelectStage(uint16 EventRoom)
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(EventRoom);
	if (pRoomInfo == nullptr)
		return 0;

	foreach_stlmap(itr, g_pMain->m_DungeonDefenceStageListArray)
	{
		if (itr->second->sStageID == pRoomInfo->m_DefenceStageID &&
			itr->second->Difficulty == pRoomInfo->m_DefenceDifficulty)
			return itr->second->sStageID;
	}

	return 0;
}
#pragma endregion

#pragma region void CGameServerDlg::SummonDungeonDefenceMonsters(uint16 EventRoom)
void CGameServerDlg::SummonDungeonDefenceMonsters(uint16 EventRoom)
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(EventRoom);
	if (pRoomInfo == nullptr)
		return;

	foreach_stlmap(itr, g_pMain->m_DungeonDefenceMonsterListArray)
	{
		_DUNGEON_DEFENCE_MONSTER_LIST* pDrakiMonster = itr->second;
		if (pDrakiMonster == nullptr
			|| pDrakiMonster->ID != pRoomInfo->m_DefenceStageID)
			continue;

		if (pDrakiMonster->ID == pRoomInfo->m_DefenceStageID)
			g_pMain->SpawnEventNpc(pDrakiMonster->MonsterID, pDrakiMonster->isMonster == 0 ? true : false, ZONE_DUNGEON_DEFENCE, pDrakiMonster->PosX, 0, pDrakiMonster->PosZ, pDrakiMonster->sCount, pDrakiMonster->sRadiusRange, -1, pDrakiMonster->isMonster == 0 ? 3 : 0, -1, EventRoom, (uint8)pDrakiMonster->sDirection, 1, 0, SpawnEventType::DungeonDefencSummon, EventRoom);
	}
}
#pragma endregion

#pragma region void CUser::ChangeDungeonDefenceStage()
void CUser::ChangeDungeonDefenceStage()
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	uint8 StageeeID = g_pMain->DungeonDefenceSelectStage(GetEventRoom());
	pRoomInfo->m_DefenceStageID = StageeeID;

	if ((pRoomInfo->m_DefenceDifficulty == 1 && pRoomInfo->m_DefenceStageID == 5)
		|| (pRoomInfo->m_DefenceDifficulty == 2 && pRoomInfo->m_DefenceStageID == 17)
		|| (pRoomInfo->m_DefenceDifficulty == 3 && pRoomInfo->m_DefenceStageID == 35))
	{
		DungeonDefenceSendFinishTimer();
		return;
	}

	_DUNGEON_DEFENCE_MONSTER_LIST* DungeonMonster = g_pMain->m_DungeonDefenceMonsterListArray.GetData(++StageeeID);
	if (DungeonMonster == nullptr)
		return;

	pRoomInfo->m_DefenceStageID = DungeonMonster->ID;
	pRoomInfo->m_DefenceMonsterSpawned = true;

	if (pRoomInfo->m_DefenceDifficulty == 1)
	{
		if (pRoomInfo->m_DefenceStageID >= 1 && pRoomInfo->m_DefenceStageID <= 5)
			pRoomInfo->m_DefenceSpawnTime = 10;
		else
			pRoomInfo->m_DefenceSpawnTime = 10;
	}
	else if (pRoomInfo->m_DefenceDifficulty == 2)
	{
		if (pRoomInfo->m_DefenceStageID >= 6 && pRoomInfo->m_DefenceStageID <= 10)
			pRoomInfo->m_DefenceSpawnTime = 10;
		else if (pRoomInfo->m_DefenceStageID >= 11 && pRoomInfo->m_DefenceStageID <= 17)
			pRoomInfo->m_DefenceSpawnTime = 20;
		else
			pRoomInfo->m_DefenceSpawnTime = 15;
	}
	else if (pRoomInfo->m_DefenceDifficulty == 3)
	{
		if (pRoomInfo->m_DefenceStageID >= 18 && pRoomInfo->m_DefenceStageID <= 22)
			pRoomInfo->m_DefenceSpawnTime = 10;
		else if (pRoomInfo->m_DefenceStageID >= 23 && pRoomInfo->m_DefenceStageID <= 29)
			pRoomInfo->m_DefenceSpawnTime = 20;
		else if (pRoomInfo->m_DefenceStageID >= 30 && pRoomInfo->m_DefenceStageID <= 35)
			pRoomInfo->m_DefenceSpawnTime = 30;
		else
			pRoomInfo->m_DefenceSpawnTime = 20;
	}
}
#pragma endregion

#pragma region void CUser::DungeonDefenceSendFinishTimer()
void CUser::DungeonDefenceSendFinishTimer()
{
	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	pRoomInfo->m_DefenceMonsterBeginnerSpawned = false;
	pRoomInfo->m_DefenceMonsterSpawned = false;
	pRoomInfo->m_DefenceisFinished = true;
	pRoomInfo->m_DefenceOutTime = 30;

	Packet result;
	result.Initialize(WIZ_BIFROST);
	result << uint8(5) << uint16(pRoomInfo->m_DefenceOutTime);
	g_pMain->Send_Zone(&result, ZONE_DUNGEON_DEFENCE, nullptr, Nation::ALL, GetEventRoom());

	result.clear();
	result.Initialize(WIZ_SELECT_MSG);
	result << uint16(0) << uint8(7) << uint64(0) << uint8(9) << uint16(0) << uint8(0) << uint8(11) << uint16(pRoomInfo->m_DefenceOutTime) << uint16(0);
	g_pMain->Send_Zone(&result, ZONE_DUNGEON_DEFENCE, nullptr, Nation::ALL, GetEventRoom());
}
#pragma endregion

#pragma region void CNpc::DungeonDefenceProcess(CUser *pUser)
void CNpc::DungeonDefenceProcess(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	bool isZoneDungeonMonster = (GetProtoID() == 9927
		|| GetProtoID() == 9928
		|| GetProtoID() == 9929
		|| GetProtoID() == 9930
		|| GetProtoID() == 9931
		|| GetProtoID() == 9932
		|| GetProtoID() == 9933
		|| GetProtoID() == 9934
		|| GetProtoID() == 9935
		|| GetProtoID() == 9936
		|| GetProtoID() == 9937
		|| GetProtoID() == 9938
		|| GetProtoID() == 9939
		|| GetProtoID() == 9940
		|| GetProtoID() == 9941
		|| GetProtoID() == 9942
		|| GetProtoID() == 9947
		|| GetProtoID() == 9948
		|| GetProtoID() == 9949
		|| GetProtoID() == 9950
		|| GetProtoID() == 9951
		|| GetProtoID() == 9955
		|| GetProtoID() == 9956
		|| GetProtoID() == 9957
		|| GetProtoID() == 9958
		|| GetProtoID() == 9959);

	if (!isZoneDungeonMonster)
		return;

	if (pUser->GetZoneID() != ZONE_DUNGEON_DEFENCE)
		return;

	_DUNGEON_DEFENCE_ROOM_INFO* pRoomInfo = g_pMain->m_DungeonDefenceRoomListArray.GetData(pUser->GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	//full moon rift jevel
	bool FullMoonRiftJevel1 = (pRoomInfo->m_DefenceStageID >= 1 && pRoomInfo->m_DefenceStageID <= 17);
	bool FullMoonRiftJevel2 = (pRoomInfo->m_DefenceStageID >= 18 && pRoomInfo->m_DefenceStageID <= 35);

	if (FullMoonRiftJevel1)
		pUser->GiveItem(MONSTER_RIFT_JEWEL, 1);
	else if (FullMoonRiftJevel2)
		pUser->GiveItem(MONSTER_RIFT_JEWEL, 2);
	else
		pUser->GiveItem(MONSTER_RIFT_JEWEL, 1);


	if (pUser->isInParty())
	{
		//monster coin
		_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
		if (pParty == nullptr)
			return;

		short partyUsers[MAX_PARTY_USERS];
		for (int i = 0; i < MAX_PARTY_USERS; i++)
			partyUsers[i] = pParty->uid[i];

		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser * UserParty = g_pMain->GetUserPtr(partyUsers[i]);
			if (UserParty == nullptr)
				continue;

			if (pUser->GetName() == UserParty->GetName())
				UserParty->GiveItem(MONSTER_COIN_ITEM, 2);
			else
				UserParty->GiveItem(MONSTER_COIN_ITEM, 1);

			//full moon token
			if (GetProtoID() == 9931
				|| GetProtoID() == 9951
				|| GetProtoID() == 9936
				|| GetProtoID() == 9959
				|| GetProtoID() == 9971)
				UserParty->GiveItem(LUNAR_ORDER_TOKEN, 1);
		}
	}
	else
	{
		//monster coin
		pUser->GiveItem(MONSTER_COIN_ITEM, 2);

		//full moon token
		if (GetProtoID() == 9931
			|| GetProtoID() == 9951
			|| GetProtoID() == 9936
			|| GetProtoID() == 9959
			|| GetProtoID() == 9971)
			pUser->GiveItem(LUNAR_ORDER_TOKEN, 1);
	}
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSPartyDelete(Packet & pkt)
void CUser::DungeonDefencePartyBBSPartyDelete(Packet & pkt)
{
	if (m_bNeedParty == 1
		|| !isPartyLeader())
		return;

	// You don't need anymore 
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
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

	StateChangeServerDirect(2, 1); // not looking for a party
	DungeonDefenceSendPartyBBSNeeded(pkt);
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSPartyChange(Packet & pkt)
void CUser::DungeonDefencePartyBBSPartyChange(Packet &pkt)
{
	uint16 page_index = 0;
	if (!isPartyLeader())
		return;

	_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_DUNGEON_DEFENCE));
	pkt.DByte();
	pkt >> pParty->sWantedClass >> page_index >> pParty->WantedMessage;
	result << uint8(PARTY_BBS_WANTED) << uint8(1);

	_SEEKING_PARTY_USER *pPartyUser = nullptr;
	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pPartyUser = (*itr);
			pPartyUser->m_strSeekingNote = pParty->WantedMessage;
			pPartyUser->m_sClass = pParty->sWantedClass;
			break;
		}
	}

	DungeonDefenceSendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);
	Send(&result);
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSRegister(Packet & pkt)
void CUser::DungeonDefencePartyBBSRegister(Packet & pkt)
{
	_SEEKING_PARTY_USER *pUser = nullptr;
	string seeking_msg = "";
	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_DUNGEON_DEFENCE));
	pkt.SByte();
	pkt >> seeking_msg;

	result << uint8(PARTY_BBS_REGISTER) << uint8(1);
	StateChangeServerDirect(2, 2); // seeking a party

	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pUser = (*itr);
			pUser->m_strSeekingNote = seeking_msg;
			break;
		}
	}

	if (pUser == nullptr)
	{
		pUser = new _SEEKING_PARTY_USER;
		pUser->m_bNation = GetNation();
		pUser->m_strUserID = GetName();
		pUser->m_sClass = GetClass();
		pUser->m_bSeekType = 2;
		pUser->m_bZone = GetZoneID();
		pUser->m_strSeekingNote = pUser->m_strSeekingNote = seeking_msg;
		pUser->m_sLevel = GetLevel();
		pUser->m_sGetID = GetID();
		pUser->m_sPartyID = GetPartyID();
		pUser->isPartyLeader = isPartyLeader();
		pUser->m_bloginType = 2;
		g_pMain->m_SeekingPartyArray.push_back(pUser);
	}

	DungeonDefenceSendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);
	Send(&result);
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSDelete(Packet & pkt)
void CUser::DungeonDefencePartyBBSDelete(Packet & pkt)
{
	if (m_bNeedParty == 1)
		return;

	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	// You don't need anymore 
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

	StateChangeServerDirect(2, 1); // not looking for a party
	DungeonDefenceSendPartyBBSNeeded(pkt);
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSNeeded(Packet & pkt)
void CUser::DungeonDefencePartyBBSNeeded(Packet & pkt)
{
	/*uint16 page_index = 0;
	uint8 typefilter = 0, locationFilter = 0, levelFilter = 0;
	pkt >> page_index >> typefilter >> locationFilter >> levelFilter;
	DungeonDefenceSendPartyBBSNeeded(page_index,type,typefilter, locationFilter, levelFilter);*/
}
#pragma endregion

#pragma region void CUser::DungeonDefenceSendPartyBBSNeeded(Packet & pkt)
void CUser::DungeonDefenceSendPartyBBSNeeded(Packet & pkt)
{
	Packet result(WIZ_PARTY_BBS);
	uint16 page_index = 0;
	uint8 typefilter = 0, locationFilter = 0, levelFilter = 0;
	pkt >> page_index >> typefilter >> locationFilter >> levelFilter;
	uint16 start_counter = 0, BBS_Counter = 0;
	start_counter = page_index * MAX_BBS_PAGE;

	if (start_counter >= MAX_USER)
	{
		result << uint8(PARTY_TYPE_DUNGEON_DEFENCE) << uint8(PARTY_BBS_NEEDED) << uint8(0);
		Send(&result);
		return;
	}

	result << uint8(PARTY_TYPE_DUNGEON_DEFENCE) << uint8(11) << uint8(1) << page_index << uint16(BBS_Counter);

	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		_SEEKING_PARTY_USER *pUser = (*itr);
		_PARTY_GROUP *pParty = nullptr;
		uint8 PartyMembers = 0;

		if (pUser == nullptr)
			continue;

		uint16 sClass = pUser->m_sClass;
		if ((BBS_Counter < page_index * MAX_BBS_PAGE)
			|| (typefilter == 2 && pUser->isPartyLeader == 1)
			|| (typefilter == 3 && pUser->isPartyLeader == 0)
			|| (locationFilter > 0 && locationFilter != pUser->m_bZone)
			|| (pUser->m_bloginType == 0)
			|| (levelFilter > 0 && levelFilter == 1 && pUser->m_sLevel > 11)
			|| (levelFilter > 0 && levelFilter == 2 && (pUser->m_sLevel < 11 || pUser->m_sLevel > 20))
			|| (levelFilter > 0 && levelFilter == 3 && (pUser->m_sLevel < 21 || pUser->m_sLevel > 30))
			|| (levelFilter > 0 && levelFilter == 4 && (pUser->m_sLevel < 31 || pUser->m_sLevel > 40))
			|| (levelFilter > 0 && levelFilter == 5 && (pUser->m_sLevel < 41 || pUser->m_sLevel > 50))
			|| (levelFilter > 0 && levelFilter == 6 && (pUser->m_sLevel < 51 || pUser->m_sLevel > 60))
			|| (levelFilter > 0 && levelFilter == 7 && (pUser->m_sLevel < 61 || pUser->m_sLevel > 70))
			|| (levelFilter > 0 && levelFilter == 8 && (pUser->m_sLevel < 71 || pUser->m_sLevel > 80))
			|| (levelFilter > 0 && levelFilter == 9 && pUser->m_sLevel < 81))
			continue;

		BBS_Counter++;

		if (BBS_Counter >= MAX_BBS_PAGE)
			break;

		if (pUser->isPartyLeader)
		{
			pParty = g_pMain->GetPartyPtr(pUser->m_sPartyID);
			if (pParty == nullptr) //Shouldn't be hit.
				return;

			pUser->m_strSeekingNote = pParty->WantedMessage;
			PartyMembers = GetPartyMemberAmount(pParty);
			sClass = pParty->sWantedClass;
		}

		result.DByte();
		result << pUser->m_bNation << uint8(pUser->m_bSeekType)
			<< pUser->m_strUserID
			<< sClass
			<< uint16(0) << pUser->m_sLevel //Not sure what that uint16 does.
			<< uint8(pUser->isPartyLeader ? 3 : 2); //2 is player, 3 is party leader
		result.SByte();
		result << pUser->m_strSeekingNote
			<< pUser->m_bZone
			<< PartyMembers;
	}

	// You still need to fill up ten slots.
	if (BBS_Counter < MAX_BBS_PAGE)
	{
		for (int j = BBS_Counter; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	int page_number = (int)g_pMain->m_SeekingPartyArray.size() / MAX_BBS_PAGE;
	if (page_number % MAX_BBS_PAGE != 0) page_number++;
	result << page_index << page_number;
	result.put(5, BBS_Counter);
	Send(&result);
}
#pragma endregion

#pragma region void CUser::DungeonDefencePartyBBSWanted(Packet & pkt)
void CUser::DungeonDefencePartyBBSWanted(Packet & pkt)
{
	uint16 page_index = 0;
	if (!isPartyLeader())
		return;

	_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return;

	Packet result(WIZ_PARTY_BBS, uint8(PARTY_TYPE_DUNGEON_DEFENCE));
	pkt.DByte();
	pkt >> pParty->sWantedClass >> page_index >> pParty->WantedMessage;
	result << uint8(PARTY_BBS_WANTED) << uint8(1);

	_SEEKING_PARTY_USER *pPartyUser = nullptr;
	uint16 seeking_index = 0; // holds the index where we located at in the vector
	Guard lock(g_pMain->m_SeekingPartyArrayLock);
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		seeking_index++;
		if ((*itr)->m_sGetID == GetID())
		{
			pPartyUser = (*itr);
			pPartyUser->m_strSeekingNote = pParty->WantedMessage;
			pPartyUser->m_sClass = pParty->sWantedClass;
			break;
		}
	}

	if (pPartyUser == nullptr)
	{
		pPartyUser = new _SEEKING_PARTY_USER;
		pPartyUser->m_bNation = GetNation();
		pPartyUser->m_strUserID = GetName();
		pPartyUser->m_sClass = pParty->sWantedClass;
		pPartyUser->m_bSeekType = 2;
		pPartyUser->m_bZone = GetZoneID();
		pPartyUser->m_strSeekingNote = pPartyUser->m_strSeekingNote = pParty->WantedMessage;
		pPartyUser->m_sLevel = GetLevel();
		pPartyUser->m_sGetID = GetID();
		pPartyUser->m_sPartyID = GetPartyID();
		pPartyUser->isPartyLeader = isPartyLeader();
		pPartyUser->m_bloginType = 2;
		g_pMain->m_SeekingPartyArray.push_back(pPartyUser);
	}

	StateChangeServerDirect(2, 3); // Looking for party nember
	DungeonDefenceSendPartyBBSNeeded(pkt);

	uint16 counter = 0;
	foreach(itr, g_pMain->m_SeekingPartyArray)
	{
		if ((*itr) == nullptr)
			continue;

		counter++;
		if (counter < seeking_index)
			continue;

		result << (*itr)->m_strUserID << uint8((*itr)->m_sLevel) << (*itr)->m_sClass;
		if (counter >= 12)
			break;
	}

	uint16 s = counter - seeking_index;
	// You still need to fill up ten slots.
	if (s < MAX_BBS_PAGE)
	{
		for (int j = s; j < MAX_BBS_PAGE; j++)
			result << uint16(0) << uint16(0)
			<< uint16(0) << uint8(0)
			<< uint8(0) << uint8(0)
			<< uint16(0)
			<< uint8(0);
	}
	result << uint16(0) << uint8(0) << uint16(s);
	Send(&result);
}
#pragma endregion
#pragma endregion