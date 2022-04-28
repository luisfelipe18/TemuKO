#include "stdafx.h"

void CUser::isEventSoccerMember(uint8 TeamColours, float x, float z)
{
	bool bIsNeutralZone = (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5);

	if (g_pMain->pSoccerEvent.m_SoccerRedColour > 11
		&& TeamColours == TeamColourRed)
		return;

	if (g_pMain->pSoccerEvent.m_SoccerBlueColour > 11
		&& TeamColours == TeamColourBlue)
		return;

	if (TeamColours > TeamColourRed
		|| TeamColours < TeamColourBlue)
		return;

	if (!bIsNeutralZone)
		return;

	_TEMPLE_SOCCER_EVENT_USER * pSoccerEventUser = new _TEMPLE_SOCCER_EVENT_USER;

	pSoccerEventUser->m_socketID = GetSocketID();
	pSoccerEventUser->m_teamColour = TeamColours;

	if (!g_pMain->m_TempleSoccerEventUserArray.PutData(pSoccerEventUser->m_socketID, pSoccerEventUser))
	{
		delete pSoccerEventUser;
		return;
	}

	if (TeamColours == TeamColourBlue)
		g_pMain->pSoccerEvent.m_SoccerBlueColour++;

	if (TeamColours == TeamColourRed)
		g_pMain->pSoccerEvent.m_SoccerRedColour++;

	Packet result(WIZ_MINING);
	result << uint8(16) << uint8(2) << g_pMain->pSoccerEvent.m_SoccerTime;
	Send(&result);

	if (x == 0.0f && z == 0.0f)
	{
		if (TeamColours == TeamColourBlue)
			x = 672.0f, z = 166.0f;
		else
			x = 672.0f, z = 154.0f;
	}

	ZoneChange(GetZoneID(), x, z);

	StateChangeServerDirect(11, uint32(TeamColours));
}

void CUser::isEventSoccerStard()
{
	if (g_pMain->pSoccerEvent.isSoccerAktive())
		return;

	if (!isSoccerEventUser())
		return;

	if (g_pMain->pSoccerEvent.m_SoccerBlueColour == TeamColourNone)
		return;

	if (g_pMain->pSoccerEvent.m_SoccerRedColour == TeamColourNone)
		return;

	if (g_pMain->pSoccerEvent.m_SoccerBlueColour > TeamColourNone
		&& g_pMain->pSoccerEvent.m_SoccerRedColour > TeamColourNone)
	{
		g_pMain->pSoccerEvent.m_SoccerTime = 600;
		g_pMain->pSoccerEvent.m_SoccerActive = true;
	}
}

void CUser::isEventSoccerEnd()
{
	if (!isSoccerEventUser())
		return;

	uint8 nWinnerTeam = TeamColourNone;
	Packet result(WIZ_MINING, uint8(16));

	if (g_pMain->pSoccerEvent.m_SoccerRedGool > g_pMain->pSoccerEvent.m_SoccerBlueGool)
		nWinnerTeam = TeamColourRed;
	else if (g_pMain->pSoccerEvent.m_SoccerRedGool < g_pMain->pSoccerEvent.m_SoccerBlueGool)
		nWinnerTeam = TeamColourBlue;
	else
		nWinnerTeam = TeamColourNone;

	result << uint8(4) << nWinnerTeam << g_pMain->pSoccerEvent.m_SoccerBlueGool << g_pMain->pSoccerEvent.m_SoccerRedGool;

	if (g_pMain->pSoccerEvent.m_SoccerBlueColour > TeamColourNone
		&& m_teamColour == TeamColourBlue)
		g_pMain->pSoccerEvent.m_SoccerBlueColour--;

	if (g_pMain->pSoccerEvent.m_SoccerRedColour > TeamColourNone
		&& m_teamColour == TeamColourRed)
		g_pMain->pSoccerEvent.m_SoccerRedColour--;

	StateChangeServerDirect(11, TeamColourNone);

	Send(&result);
}

bool CUser::isSoccerEventUser()
{
	_TEMPLE_SOCCER_EVENT_USER * pSoccerEvent = g_pMain->m_TempleSoccerEventUserArray.GetData(GetSocketID());

	if (pSoccerEvent != nullptr)
		return true;

	return false;
}

bool CUser::isInSoccerEvent()
{
	bool bIsNeutralZone = (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5);

	if (!bIsNeutralZone)
		return false;

	if (!isSoccerEventUser())
		return false;

	return ((GetX() > 644.0f && GetX() < 699.0f)
		&& ((GetZ() > 120.0f && GetZ() < 200.0f)));
}

uint8 CNpc::isInSoccerEvent()
{
	bool bIsNeutralZone = (GetZoneID() >= ZONE_MORADON && GetZoneID() <= ZONE_MORADON5);

	bool b_isSoccerOutside = ((GetX() > 644.0f && GetX() < 699.0f) && ((GetZ() > 120.0f && GetZ() < 200.0f)));

	bool b_isSoccerRedside = ((GetX() > 661.0f && GetX() < 681.0f) && ((GetZ() > 108.0f && GetZ() < 120.0f)));

	bool b_isSoccerBlueside = ((GetX() > 661.0f && GetX() < 681.0f) && ((GetZ() > 199.0f && GetZ() < 208.0f)));

	if (!bIsNeutralZone)
		return TeamColourMap;

	if (b_isSoccerBlueside)
		return TeamColourBlue;

	if (b_isSoccerRedside)
		return TeamColourRed;

	if (!b_isSoccerOutside)
		return TeamColourOutside;

	return TeamColourNone;
}

#pragma region CGameServerDlg::TempleSoccerEventTimer()

void CGameServerDlg::TempleSoccerEventTimer()
{
	if (pSoccerEvent.isSoccerAktive()
		&& pSoccerEvent.m_SoccerTime == 600)
	{
		Packet result(WIZ_MINING, uint8(0x10));

		foreach_stlmap(itr, g_pMain->m_TempleSoccerEventUserArray)
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr
				|| pUser->isDead()
				|| !pUser->isInGame())
				continue;

			result << uint8(0x02) << g_pMain->pSoccerEvent.m_SoccerTime;
			pUser->Send(&result);
		}
	}
	else if (pSoccerEvent.isSoccerAktive()
		&& (pSoccerEvent.m_SoccerTime > 1
			&& pSoccerEvent.m_SoccerTime < 600))
	{
		foreach_stlmap(itr, g_pMain->m_arNpcThread)
		{
			CNpcThread* pNpcThread = itr->second;

			if (pNpcThread == nullptr)
				continue;

			foreach_stlmap(btr, pNpcThread->m_arNpcArray)
			{
				CNpc * pNpc = TO_NPC(btr->second);

				if (pNpc == nullptr
					|| pNpc->isDead()
					|| pNpc->GetType() != NPC_SOCCER_BAAL)
					continue;

				if (pNpc->isInSoccerEvent() >= TeamColourBlue
					&& pNpc->isInSoccerEvent() <= TeamColourOutside)
				{
					if (pNpc->isInSoccerEvent() == TeamColourRed
						|| pNpc->isInSoccerEvent() == TeamColourBlue)
					{
						if (pNpc->isInSoccerEvent() == TeamColourRed)
							pSoccerEvent.m_SoccerBlueGool++;

						if (pNpc->isInSoccerEvent() == TeamColourBlue)
							pSoccerEvent.m_SoccerRedGool++;

						TempleSoccerEventGool(pNpc->isInSoccerEvent());
					}

					float x, z;
					pNpc->SendInOut(INOUT_OUT, pNpc->GetX(), pNpc->GetZ(), 0.0f);
					x = 672.0f; z = 160.0f;
					pNpc->SendInOut(INOUT_IN, x, z, 0.0f);
					break;
				}
				break;
			}
		}
	}
	else if (pSoccerEvent.isSoccerAktive()
		&& pSoccerEvent.m_SoccerTime == 1)
	{
		float x = 0.0f, z = 0.0f;

		foreach_stlmap(itr, g_pMain->m_TempleSoccerEventUserArray)
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr
				|| pUser->isDead()
				|| !pUser->isInGame())
				continue;

			if (pUser->m_teamColour == TeamColourBlue)
			{
				x = 639.0f;
				z = 194.0f;
			}

			if (pUser->m_teamColour == TeamColourRed)
			{
				x = 703.0f;
				z = 127.0f;
			}

			pUser->ZoneChange(pUser->GetZoneID(), x, z);
			pUser->isEventSoccerEnd();
		}

		foreach_stlmap_nolock(itr, g_pMain->m_arNpcThread)
		{
			CNpcThread* pNpcThread = itr->second;

			if (pNpcThread == nullptr)
				continue;

			foreach_stlmap_nolock(btr, pNpcThread->m_arNpcArray)
			{
				CNpc * pNpc = TO_NPC(btr->second);

				if (pNpc == nullptr
					|| pNpc->isDead()
					|| pNpc->GetType() != NPC_SOCCER_BAAL)
					continue;

				pNpc->SendInOut(INOUT_OUT, pNpc->GetX(), pNpc->GetZ(), 0.0f);
				x = 672.0f; z = 160.0f;
				pNpc->SendInOut(INOUT_IN, x, z, 0.0f);
				break;
			}
		}

		if (!pSoccerEvent.isSoccerTime())
		{
			pSoccerEvent.m_SoccerTimer = true;
			pSoccerEvent.m_SoccerTimers = 10;
		}
	}
	else if (pSoccerEvent.isSoccerAktive()
		&& pSoccerEvent.isSoccerTime()
		&& pSoccerEvent.m_SoccerTimers == 1)
	{
		TempleSoccerEventEnd();
	}

	if (pSoccerEvent.m_SoccerTime > 0)
		pSoccerEvent.m_SoccerTime--;

	if (pSoccerEvent.isSoccerTime()
		&& pSoccerEvent.m_SoccerTimers > 0)
		pSoccerEvent.m_SoccerTimers--;
}

void CGameServerDlg::TempleSoccerEventEnd()
{
	pSoccerEvent.m_SoccerActive = false;
	pSoccerEvent.m_SoccerTimer = false;
	pSoccerEvent.m_SoccerSocketID = -1;
	pSoccerEvent.m_SoccerBlueColour = 0;
	pSoccerEvent.m_SoccerBlueGool = 0;
	pSoccerEvent.m_SoccerRedGool = 0;
	pSoccerEvent.m_SoccerRedColour = 0;
	pSoccerEvent.m_SoccerTime = 0;
	pSoccerEvent.m_SoccerTimers = 0;

	if (m_TempleSoccerEventUserArray.GetSize() > 0)
		m_TempleSoccerEventUserArray.DeleteAllData();
}

void CGameServerDlg::TempleSoccerEventGool(uint8 nType)
{
	Packet result(WIZ_MINING, uint8(0x10));

	foreach_stlmap(itr, g_pMain->m_TempleSoccerEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr
			|| pUser->isDead()
			|| !pUser->isInGame())
			continue;

		result << uint8(0x01)
			<< pSoccerEvent.m_SoccerSocketID
			<< nType << nType
			<< pSoccerEvent.m_SoccerBlueGool
			<< pSoccerEvent.m_SoccerRedGool;
		pUser->Send(&result);
	}
}

#pragma endregion

void CUser::HandleSoccer(Packet & pkt)
{
	Packet result(WIZ_MINING, uint8(MiningSoccer));
	uint16 resultCode = MiningResultSuccess;

	// Are we mining already?
	if (isMining())
		resultCode = MiningResultMiningAlready;

	result << resultCode;

	// If nothing went wrong, allow the user to start mining.
	// Be sure to let everyone know we're mining.
	if (resultCode == MiningResultSuccess)
	{
		m_bMining = true;
		result << GetID();
		SendToRegion(&result);
	}
	else
	{
		Send(&result);
	}
}