#include "stdafx.h"
#include "MagicInstance.h"

#pragma region CGameServerDlg::BDWMonumentAltarTimer()
void CGameServerDlg::BDWMonumentAltarTimer()
{
	if (isBorderDefenceWarActive())
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(i);
			if (pRoomInfo == nullptr)
				continue;

			if (pRoomInfo->m_bFinished
				|| !pRoomInfo->m_tAltarSpawn)
				continue;

			if (pRoomInfo->m_tAltarSpawnTimed > 0)
			{
				if (pRoomInfo->m_tAltarSpawnTimed <= UNIXTIME
					&& pRoomInfo->m_tAltarSpawn)
				{
					pRoomInfo->m_tAltarSpawnTimed = 0;
					BDWMonumentAltarRespawn();
				}
			}
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::BDWMonumentAltarRespawn()
void CGameServerDlg::BDWMonumentAltarRespawn()
{
	if (isBorderDefenceWarActive())
	{
		for (int i = 1; i <= MAX_TEMPLE_EVENT_ROOM; i++)
		{
			_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(i);
			if (pRoomInfo == nullptr)
				continue;

			if (pRoomInfo->m_bFinished
				|| !pRoomInfo->m_tAltarSpawn)
				continue;

			if (pRoomInfo->m_tAltarSpawn
				&& !pRoomInfo->m_bFinished)
			{
				Packet result(WIZ_EVENT);
				result << uint8(2) << uint8(2);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, i);
				pRoomInfo->m_tAltarSpawn = false;

				CNpcThread* zoneitrThread = m_arNpcThread.GetData(ZONE_BORDER_DEFENSE_WAR);
				if (zoneitrThread == nullptr)
					return;

				foreach_stlmap(itr, zoneitrThread->m_arNpcArray)
				{
					CNpc* pNpc = TO_NPC(itr->second);
					if (pNpc == nullptr)
						continue;

					if (!pNpc->isDead()
						|| pNpc->GetEventRoom() != i
						|| pNpc->GetProtoID() != ALTAR_OF_MANES)
						continue;

					pNpc->m_bForceReset = true;

					if (pNpc->GetNpcState() == NPC_LIVE) // if its not dead then just full its hp.
						pNpc->HpChange(pNpc->GetMaxHealth(), nullptr);
				}

				//g_pMain->SpawnEventNpc(ALTAR_OF_MANES, true, ZONE_BORDER_DEFENSE_WAR, 127, 0, 131, 1, 0, 0, 0, -1, i, 136);
			}
		}
	}
}
#pragma endregion

#pragma region CUser::BDWMonumentPointProcess()
void CUser::BDWMonumentPointProcess()
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		if (m_bHasAlterOptained)
		{
			bool isInPointRange = false;

			if (GetNation() == KARUS)
			{
				if (GetX() >= 28.0f
					&& GetX() <= 35.0f
					&& GetZ() >= 128.0f
					&& GetZ() <= 135.0f)
					isInPointRange = true;
			}

			if (GetNation() == ELMORAD)
			{
				if (GetX() >= 220.0f
					&& GetX() <= 227.0f
					&& GetZ() >= 127.0f
					&& GetZ() <= 135.0f)
					isInPointRange = true;
			}

			if (isInPointRange)
			{
				uint16 AltarSpawnTimed = 60;
				Packet result(WIZ_EVENT);
				result << uint8(TEMPLE_EVENT_ALTAR_TIMER) << AltarSpawnTimed;
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

				pRoomInfo->m_tAltarSpawnTimed = UNIXTIME + AltarSpawnTimed;
				pRoomInfo->m_tAltarSpawn = true;
				BDWAltarScreenAndPlayerPointChange();

				CMagicProcess::RemoveType4Buff(BUFF_TYPE_FRAGMENT_OF_MANES, this, true, true);
				m_bHasAlterOptained = false;

			}
		}
	}
}
#pragma endregion

#pragma region CUser::BDWUserHasObtainedGameLoqOut()
void CUser::BDWUserHasObtainedGameLoqOut()
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		if (m_bHasAlterOptained == true)
		{
			uint16 AltarSpawnTimed = 60;
			Packet result(WIZ_EVENT);
			result << uint8(TEMPLE_EVENT_ALTAR_TIMER) << AltarSpawnTimed;
			g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

			pRoomInfo->m_tAltarSpawnTimed = UNIXTIME + AltarSpawnTimed;
			pRoomInfo->m_tAltarSpawn = true;
			m_bHasAlterOptained = false;
		}
	}
}
#pragma endregion

#pragma region CUser::BDWAltarScreenAndPlayerPointChange()
void CUser::BDWAltarScreenAndPlayerPointChange()
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_FinishPacketControl)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		uint8 nation = 0;
		GetNation() == Nation::ELMORAD ? nation = 2 : nation = 1;

		if (GetNation() == KARUS)
		{
			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				if (pEventUser->isLoqOut)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetNation() != KARUS)
					continue;

				// Update the kill count of the nation of that room Altar System.
				pRoomInfo->m_KarusScoreBoard += 10;
				pUser->m_BorderDefenceWarUserPoint += 10;
				pUser->UpdateBorderDefenceWarRank();
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();
		}
		else
		{
			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				if (pEventUser->isLoqOut)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetNation() != ELMORAD)
					continue;

				// Update the kill count of the nation of that room Altar System.Party'de ki oyuncu sayýsýna baðlý olarak arttýr.
				pRoomInfo->m_ElmoScoreBoard += 10;
				pUser->m_BorderDefenceWarUserPoint += 10;
				pUser->UpdateBorderDefenceWarRank();
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}

		// Send the new score to the room users.
		Packet result(WIZ_EVENT);
		result << uint8(TEMPLE_SCREEN) << uint32(pRoomInfo->m_KarusScoreBoard) << uint32(pRoomInfo->m_ElmoScoreBoard);
		g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

		//Flag Total Point.
		nation == Nation::ELMORAD ? pRoomInfo->m_iElmoMonuCount += 1 : pRoomInfo->m_iKarusMonuCount += 1;

		if (pRoomInfo->m_bWinnerNation == 0)
		{
			pRoomInfo->m_bWinnerNation = 0;

			if (pRoomInfo->m_ElmoScoreBoard >= 300)
			{
				g_pMain->pTempleEvent.EventCloseMainControl = true; pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;
				pRoomInfo->m_bWinnerNation = ELMORAD; pRoomInfo->m_tAltarSpawn = false; pRoomInfo->m_tAltarSpawnTimed = 0;

				result.clear();
				result.Initialize(WIZ_EVENT);
				result << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_BORDER_DEFENCE_WAR) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
			}
			else if (pRoomInfo->m_KarusScoreBoard >= 300)
			{
				g_pMain->pTempleEvent.EventCloseMainControl = true; pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;
				pRoomInfo->m_bWinnerNation = KARUS; pRoomInfo->m_tAltarSpawn = false; pRoomInfo->m_tAltarSpawnTimed = 0;

				result.clear();
				result.Initialize(WIZ_EVENT);
				result << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_BORDER_DEFENCE_WAR) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
			}
		}
	}
}
#pragma endregion

#pragma region CUser::BDWDeath()
void CUser::BDWDeath()
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		if (m_bHasAlterOptained)
		{
			uint16 AltarSpawnTimed = 60;
			m_bHasAlterOptained = false;
			pRoomInfo->m_tAltarSpawnTimed = UNIXTIME + AltarSpawnTimed;
			pRoomInfo->m_tAltarSpawn = true;

			Packet result(WIZ_EVENT);
			result << uint8(TEMPLE_EVENT_ALTAR_TIMER) << AltarSpawnTimed;
			g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
		}
	}
}
#pragma endregion

#pragma region CNpc::BDWMonumentAltarSystem(CUser *pUser)
void CNpc::BDWMonumentAltarSystem(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	if (!g_pMain->pTempleEvent.isActive
		|| GetZoneID() != ZONE_BORDER_DEFENSE_WAR
		|| GetEventRoom() <= 0
		|| pUser->GetEventRoom() != GetEventRoom())
		return;

	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(pUser->GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		pUser->m_bHasAlterOptained = true;
		MagicInstance instance;
		instance.sCasterID = pUser->GetID();
		instance.sTargetID = pUser->GetID();
		instance.nSkillID = 492063;
		instance.Run();

		Packet result(WIZ_EVENT);
		result.SByte();
		result << uint8(TEMPLE_EVENT_ALTAR_FLAG)
			<< pUser->GetName()
			<< pUser->GetNation();
		g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, pUser->GetEventRoom());
	}
}
#pragma endregion

#pragma region CUser::BDWUpdateRoomKillCount()
void CUser::BDWUpdateRoomKillCount()
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_FinishPacketControl)
		return;

	if (g_pMain->isBorderDefenceWarActive())
	{
		uint8 nation = 0;
		GetNation() == Nation::ELMORAD ? nation = 2 : nation = 1;

		if (GetNation() == KARUS)
		{
			pRoomInfo->m_KarusUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_KarusUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				if (pEventUser->isLoqOut)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetNation() != KARUS)
					continue;

				// Update the kill count of the nation of that room Altar System.
				pRoomInfo->m_KarusScoreBoard += 1;
			}
			pRoomInfo->m_KarusUserList.m_lock.unlock();
		}
		else
		{
			pRoomInfo->m_ElmoradUserList.m_lock.lock();
			foreach_stlmap_nolock(itrUser, pRoomInfo->m_ElmoradUserList)
			{
				_TEMPLE_STARTED_EVENT_USER* pEventUser = itrUser->second;
				if (pEventUser == nullptr)
					continue;

				if (pEventUser->isLoqOut)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(pEventUser->strUserID, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| !pUser->isEventUser()
					|| pUser->GetNation() != ELMORAD)
					continue;

				// Update the kill count of the nation of that room Altar System.
				pRoomInfo->m_ElmoScoreBoard += 1;
			}
			pRoomInfo->m_ElmoradUserList.m_lock.unlock();
		}

		nation == Nation::ELMORAD ? pRoomInfo->m_iElmoradKillCount += 1 : pRoomInfo->m_iKarusKillCount += 1;
		m_BorderDefenceWarUserPoint += 1;
		UpdateBorderDefenceWarRank();

		// Send the new score to the room users.
		Packet result(WIZ_EVENT);
		result << uint8(TEMPLE_SCREEN) << uint32(pRoomInfo->m_KarusScoreBoard) << uint32(pRoomInfo->m_ElmoScoreBoard);
		g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

		if (pRoomInfo->m_bWinnerNation == 0)
		{
			pRoomInfo->m_bWinnerNation = 0;

			if (pRoomInfo->m_ElmoScoreBoard >= 300)
			{
				g_pMain->pTempleEvent.EventCloseMainControl = true; pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;
				pRoomInfo->m_bWinnerNation = ELMORAD; pRoomInfo->m_tAltarSpawn = false; pRoomInfo->m_tAltarSpawnTimed = 0;

				result.clear();
				result.Initialize(WIZ_EVENT);
				result << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_BORDER_DEFENCE_WAR) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
			}
			else if (pRoomInfo->m_KarusScoreBoard >= 300)
			{
				g_pMain->pTempleEvent.EventCloseMainControl = true; pRoomInfo->m_FinishPacketControl = true; pRoomInfo->m_tFinishTimeCounter = (uint32)UNIXTIME + 20;
				pRoomInfo->m_bWinnerNation = KARUS; pRoomInfo->m_tAltarSpawn = false; pRoomInfo->m_tAltarSpawnTimed = 0;

				result.clear();
				result.Initialize(WIZ_EVENT);
				result << uint8(TEMPLE_EVENT_FINISH) << uint16(TEMPLE_EVENT_BORDER_DEFENCE_WAR) << uint8(pRoomInfo->m_bWinnerNation) << uint32(20);
				g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
			}
		}
	}
}
#pragma endregion

#pragma region CUser::JRUpdateRoomKillCount()
void CUser::JRUpdateRoomKillCount()
{
	_JURAID_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventJuraidRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_FinishPacketControl)
		return;

	if (g_pMain->isJuraidMountainActive())
	{
		uint8 nation = 0;
		GetNation() == Nation::ELMORAD ? nation = 2 : nation = 1;

		nation == Nation::ELMORAD ? pRoomInfo->m_iElmoradKillCount++ : pRoomInfo->m_iKarusKillCount++;

		// Send the new score to the room users.
		Packet result(WIZ_EVENT);
		result << uint8(TEMPLE_SCREEN) << uint32(pRoomInfo->m_iKarusKillCount) << uint32(pRoomInfo->m_iElmoradKillCount);
		g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_JURAID_MOUNTAIN, true, GetEventRoom());
	}

}

#if 0
#pragma region CUser::HandleBDWCapture(Packet & pkt)
/**
* @brief BDW Monument Capture.
*/
void CUser::HandleBDWCapture(Packet& pkt)
{
	_BDW_ROOM_INFO* pRoomInfo = g_pMain->m_TempleEventBDWRoomList.GetData(GetEventRoom());
	if (pRoomInfo == nullptr)
		return;

	if (pRoomInfo->m_bWinnerNation != 0
		|| pRoomInfo->m_tFinishTimeCounter > 0)
		return;

	pRoomInfo->m_bMonumentHolderNation = GetNation();
	pRoomInfo->m_tMonumentTimeCounter = uint32(UNIXTIME) + 360; // 360 seconds will start to be counted down
	pRoomInfo->m_tFinishTimeCounter = 0;

	Packet result(WIZ_CAPTURE, uint8(0x05));
	result << GetNation() << GetName();
	g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

	result.clear();
	result.Initialize(WIZ_CAPTURE);
	result << uint8(0x04) << uint8(GetNation()) << uint16(360);
	g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());

	if (isInParty())
	{
		_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());

		if (pParty == nullptr)
			return;

		short partyUsers[MAX_PARTY_USERS];

		for (int i = 0; i < MAX_PARTY_USERS; i++)
			partyUsers[i] = pParty->uid[i];

		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser* pUser = g_pMain->GetUserPtr(partyUsers[i]);
			if (pUser == nullptr)
				continue;

			if (GetEventRoom() != pUser->GetEventRoom())
				continue;

			pUser->m_iLoyaltyDaily += 1;
			pUser->UpdatePlayerRank();
		}
	}

	if (!isInParty())
	{
		m_iLoyaltyDaily += 1;
		UpdatePlayerRank();
	}

	uint8 nation = 0;
	GetNation() == Nation::ELMORAD ? nation = 2 : nation = 1;
	nation == Nation::ELMORAD ? pRoomInfo->m_iElmoradKillCount++ : pRoomInfo->m_iKarusKillCount++;

	// Send the new score to the room users.
	result.clear();
	result.Initialize(WIZ_EVENT);
	result << uint8(TEMPLE_SCREEN) << uint32(pRoomInfo->m_iKarusKillCount) << uint32(pRoomInfo->m_iElmoradKillCount);
	g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_BORDER_DEFENSE_WAR, true, GetEventRoom());
}
#pragma endregion
#endif