#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;
extern CDBAgent g_DBAgent;

#pragma region New Castle Siege Warfare System
#pragma region CGameServerDlg::CastleSiegeWarMainTimer()
void CGameServerDlg::CastleSiegeWarMainTimer()
{
	if (!isCswActive())
		return;

	switch (pCswEvent.Status)
	{
	case CswOperationStatus::BarrackCreated:
		if (!pCswEvent.BarrackCreateTimerControl)
		{
			int32 RemainingTime = pCswEvent.CswTime - (uint32)UNIXTIME;
			if (RemainingTime)
			{
				if (RemainingTime == 5 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeployBarrack, 5);
				else if (RemainingTime == 4 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeployBarrack, 4);
				else if (RemainingTime == 3 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeployBarrack, 3);
				else if (RemainingTime == 2 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeployBarrack, 2);
				else if (RemainingTime == 1 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeployBarrack, 1);
			}
			else
			{
				if (!RemainingTime)
				{
					pCswEvent.BarrackCreateTimerControl = true;
					CastleSiegeWarDeatchmatchOpen();
				}
			}
		}
		break;
	case CswOperationStatus::DeathMatch:
		if (!pCswEvent.DeathMatchTimerControl)
		{
			int32 RemainingTime = pCswEvent.CswTime - (uint32)UNIXTIME;
			if (RemainingTime)
			{
				if (RemainingTime == 29 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 29);
				else if (RemainingTime == 25 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 25);
				else if (RemainingTime == 20 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 20);
				else if (RemainingTime == 15 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 15);
				else if (RemainingTime == 10 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 10);
				else if (RemainingTime == 5 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 5);
				else if (RemainingTime == 4 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 4);
				else if (RemainingTime == 3 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 3);
				else if (RemainingTime == 2 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 2);
				else if (RemainingTime == 1 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeDeathMatch, 1);
			}
			else
			{
				if (!RemainingTime)
				{
					pCswEvent.DeathMatchTimerControl = true;
					CastleSiegeDeatchmatchClose();
				}
			}
		}
		break;
	case CswOperationStatus::Preparation:
		if (!pCswEvent.PreparationTimerControl)
		{
			int32 RemainingTime = pCswEvent.CswTime - (uint32)UNIXTIME;
			if (RemainingTime)
			{
				if (RemainingTime == 10 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 10);
				else if (RemainingTime == 5 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 5);
				else if (RemainingTime == 4 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 4);
				else if (RemainingTime == 3 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 3);
				else if (RemainingTime == 2 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 2);
				else if (RemainingTime == 1 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticePreparation, 1);
			}
			else
			{
				if (!RemainingTime)
				{
					pCswEvent.PreparationTimerControl = true;
					CastleSiegeCastellanWarOpen();
				}
			}
		}
		break;
	case CswOperationStatus::CastellanWar:
		if (!pCswEvent.CastellanWarTimerControl)
		{
			int32 RemainingTime = pCswEvent.CswTime - (uint32)UNIXTIME;
			if (RemainingTime)
			{
				if (RemainingTime == 29 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 29);
				else if (RemainingTime == 25 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 25);
				else if (RemainingTime == 20 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 20);
				else if (RemainingTime == 15 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 15);
				else if (RemainingTime == 10 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 10);
				else if (RemainingTime == 5 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 5);
				else if (RemainingTime == 4 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 4);
				else if (RemainingTime == 3 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 3);
				else if (RemainingTime == 2 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 2);
				else if (RemainingTime == 1 * MINUTE)
					CswRemainingTimeNotice(CswNoticeStatus::NoticeCastellan, 1);
			}

			if (pCswEvent.isMonumentActive)
			{
				int32 RemainingMonumentTime = pCswEvent.MonumentTime - (uint32)UNIXTIME;
				if (RemainingMonumentTime)
				{
					if (RemainingMonumentTime == 12 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 12);
					else if (RemainingMonumentTime == 10 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 10);
					else if (RemainingMonumentTime == 5 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 5);
					else if (RemainingMonumentTime == 4 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 4);
					else if (RemainingMonumentTime == 3 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 3);
					else if (RemainingMonumentTime == 2 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 2);
					else if (RemainingMonumentTime == 1 * MINUTE)
						CswRemainingTimeNotice(CswNoticeStatus::NoticeMonumentKilled, 1);
				}
				else
				{
					if (!RemainingMonumentTime)
						pCswEvent.isMonumentFinish = true;
				}
			}

			if (!RemainingTime || pCswEvent.isMonumentFinish)
			{
				pCswEvent.CastellanWarTimerControl = true;
				CastleSiegeWarGenelClose();
			}
		}
		break;
	}
}
#pragma endregion

#pragma region CGameServerDlg::CswRemainingTimeNotice(uint8 NoticeType,int32 Time)
void CGameServerDlg::CswRemainingTimeNotice(uint8 NoticeType, int32 Time)
{
	Packet pkt;
	string notice;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		switch (NoticeType)
		{
		case CswNoticeStatus::NoticeDeployBarrack:
			g_pMain->GetServerResource(IDS_SIEGE_WAR_DEPLOY_BARRACK, &notice, Time);

			// Wrap it in a "#### NOTICE : ####" block.
			g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

			// Construct & send the chat/announcement packet
			ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
			pUser->Send(&pkt);
			break;
		case CswNoticeStatus::NoticeDeathMatch:
			if (pUser->GetZoneID() == ZONE_DELOS)
			{
				g_pMain->GetServerResource(IDS_SIEGE_WAR_DEATHMATCH_REM_TIME, &notice, Time);

				// Wrap it in a "#### NOTICE : ####" block.
				g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

				// Construct & send the chat/announcement packet
				ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
				pUser->Send(&pkt);
			}
			break;
		case CswNoticeStatus::NoticePreparation:
			g_pMain->GetServerResource(IDS_SIEGE_WAR_PREPARING_REM_TIME, &notice, Time);

			// Wrap it in a "#### NOTICE : ####" block.
			g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

			// Construct & send the chat/announcement packet
			ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
			pUser->Send(&pkt);
			break;
		case CswNoticeStatus::NoticeCastellan:
			if (pUser->GetZoneID() == ZONE_DELOS)
			{
				g_pMain->GetServerResource(IDS_SIEGE_WAR_CASTELLAN_WAR_START, &notice, Time);

				// Wrap it in a "#### NOTICE : ####" block.
				g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

				// Construct & send the chat/announcement packet
				ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
				pUser->Send(&pkt);
			}
			break;
		case CswNoticeStatus::NoticeMonumentKilled:
			if (pUser->GetZoneID() == ZONE_DELOS)
			{
				_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
				if (pKnightSiegeWar != nullptr)
				{
					CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
					g_pMain->GetServerResource(IDS_SIEGE_WAR_MONUMENT_DEAD, &notice, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), Time);
					// Wrap it in a "#### NOTICE : ####" block.
					g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

					// Construct & send the chat/announcement packet
					ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
					pUser->Send(&pkt);
				}
			}
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::CswUserToolUserNotice(uint8 NoticeType)
void CUser::CswUserToolUserNotice(uint8 NoticeType)
{
	_CSW_EVENT_PLAY_TIMER* pPlayTimer = g_pMain->m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr)
		return;

	Packet pkt;
	string notice;

	switch (NoticeType)
	{
		//Barrack
	case CswNoticeStatus::NoticeDeployBarrack:
		g_pMain->GetServerResource(IDS_SIEGE_WAR_DEPLOY_BARRACK, &notice, pPlayTimer->BarrackCreatedTime);

		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
		Send(&pkt);
		break;
		//DeathMatch
	case CswNoticeStatus::NoticeDeathMatch:
		g_pMain->GetServerResource(IDS_SIEGE_WAR_DEATHMATCH_START, &notice);

		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
		Send(&pkt);
		break;
		//Preparation
	case CswNoticeStatus::NoticePreparation:
		g_pMain->GetServerResource(IDS_SIEGE_WAR_PREPARING_START, &notice);

		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
		Send(&pkt);
		break;
		//Castellan
	case CswNoticeStatus::NoticeCastellan:
		g_pMain->GetServerResource(IDS_SIEGE_WAR_CASTELLAN_WAR_START, &notice, pPlayTimer->CastellanTime);

		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
		Send(&pkt);
		break;
		//Monument
	case CswNoticeStatus::NoticeMonumentKilled:
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiegeWar != nullptr)
		{
			CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
			g_pMain->GetServerResource(IDS_SIEGE_WAR_MONUMENT_DEAD, &notice, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pPlayTimer->MonumentFinishTime);
			// Wrap it in a "#### NOTICE : ####" block.
			g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

			// Construct & send the chat/announcement packet
			ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
			Send(&pkt);
		}
	}
	break;
	//Finish
	case CswNoticeStatus::NoticeCswFinish:
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiegeWar != nullptr)
		{
			CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
			if (pKnights != nullptr)
			{
				g_pMain->GetServerResource(IDS_SIEGE_WAR_VICTORY, &notice, pKnights->GetName().c_str());
				// Wrap it in a "#### NOTICE : ####" block.
				g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

				// Construct & send the chat/announcement packet
				ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
				Send(&pkt);
				break;
			}
		}
		g_pMain->GetServerResource(IDS_SIEGE_WAR_END, &notice);
		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&pkt, WAR_SYSTEM_CHAT, &notice);
		Send(&pkt);
	}
	break;
	}
}
#pragma endregion

#pragma region CUser::CswUserToolUserFlag()
void CUser::CswUserToolUserFlag()
{
	Packet result(WIZ_SIEGE, uint8(2));

	if (g_pMain->isCswActive())
	{
		int32 CswTime = 0, MonumentTime = 0;

		if (g_pMain->pCswEvent.CswTime != 0) { CswTime = g_pMain->pCswEvent.CswTime - (uint32)UNIXTIME; }
		if (CswTime <= 0) { CswTime = 0; }

		if (g_pMain->isCastellanWarActive() && g_pMain->pCswEvent.isMonumentActive)
		{
			if (g_pMain->pCswEvent.MonumentTime != 0)
				MonumentTime = g_pMain->pCswEvent.MonumentTime - (uint32)UNIXTIME;

			if (MonumentTime <= 0)
				MonumentTime = 0;
		}

		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege != nullptr)
		{
			CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnight != nullptr)
			{
				result << g_pMain->pCswEvent.MonumentType
					<< pKnight->GetID()
					<< pKnight->m_sMarkVersion
					<< pKnight->m_byFlag
					<< pKnight->m_byGrade
					<< MonumentTime
					<< int16(CswTime);

				result.SByte();
				result << pKnight->GetName();
				Send(&result);
				return;
			}
		}

		string ClanName = "";
		result << uint8(g_pMain->pCswEvent.MonumentType)
			<< uint16(0)
			<< uint32(0)
			<< MonumentTime
			<< int16(CswTime);

		result.SByte();
		result << ClanName;
		Send(&result);
	}
	else
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege != nullptr)
		{
			CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnight != nullptr)
			{
				result << g_pMain->pCswEvent.MonumentType
					<< pKnight->GetID()
					<< pKnight->m_sMarkVersion
					<< pKnight->m_byFlag
					<< pKnight->m_byGrade;
				Send(&result);
			}
		}
	}
}
#pragma endregion

#pragma region CUser::CswUserToolTownUser()
void CUser::CswUserToolTownUser()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege != nullptr)
	{
		CKnights* pKnightUser = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
		if (pKnightUser != nullptr && pKnightUser->GetID() == GetClanID())
			return;
	}
	Home();
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarUserTools(bool Notice, uint8 NoticeType, bool Flag, bool KickOutUser,bool Town)
void CGameServerDlg::CastleSiegeWarUserTools(bool Notice, uint8 NoticeType, bool Flag, bool KickOutUser, bool Town)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (Notice)
		{
			switch (NoticeType)
			{
			case CswNoticeStatus::NoticeDeployBarrack:
				pUser->CswUserToolUserNotice(CswNoticeStatus::NoticeDeployBarrack);
				break;
			case CswNoticeStatus::NoticeDeathMatch:
				if (pUser->GetZoneID() == ZONE_DELOS)
					pUser->CswUserToolUserNotice(CswNoticeStatus::NoticeDeathMatch);
				break;
			case CswNoticeStatus::NoticePreparation:
				pUser->CswUserToolUserNotice(CswNoticeStatus::NoticePreparation);
				break;
			case CswNoticeStatus::NoticeCastellan:
				if (pUser->GetZoneID() == ZONE_DELOS)
					pUser->CswUserToolUserNotice(CswNoticeStatus::NoticeCastellan);
				break;
			case CswNoticeStatus::NoticeMonumentKilled:
				if (pUser->GetZoneID() == ZONE_DELOS)
					pUser->CswUserToolUserNotice(CswNoticeStatus::NoticeMonumentKilled);
				break;
			case CswNoticeStatus::NoticeCswFinish:
				pUser->CswUserToolUserNotice(CswNoticeStatus::NoticeCswFinish);
				break;
			}
		}

		if (Flag)
		{
			if (pUser->GetZoneID() == ZONE_DELOS)
				pUser->CswUserToolUserFlag();
		}

		if (!Town && KickOutUser)
		{
			if (pCswEvent.Status == CswOperationStatus::DeathMatch)
			{
				_DEATHMATCH_BARRACK_INFO* pBarakaInfo = m_KnightSiegeWarBarrackList.GetData(pUser->GetClanID());
				if (pBarakaInfo == nullptr && pUser->GetZoneID() == ZONE_DELOS)
					pUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
			}
			else
			{
				if (pUser->GetZoneID() == ZONE_DELOS
					|| pUser->GetZoneID() == ZONE_HELL_ABYSS
					|| pUser->GetZoneID() == ZONE_DESPERATION_ABYSS)
					pUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
			}
		}

		if (!KickOutUser && Town)
		{
			if (pUser->GetZoneID() == ZONE_DELOS)
				pUser->CswUserToolTownUser();
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeDeathmatchBarrackCreated()
void CGameServerDlg::CastleSiegeDeathmatchBarrackCreated()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege == nullptr)
		return;

	_CSW_EVENT_PLAY_TIMER* pPlayTimer = m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr)
		return;

	m_KnightSiegeWarBarrackList.DeleteAllData();
	g_DBAgent.KnightsSiegeWarBarrackResetTable();

	m_KnightSiegeWarClanList.DeleteAllData();
	g_DBAgent.LoadKnightSiegeEnteredClan();

	if (m_KnightSiegeWarClanList.GetSize() <= 0)
		return;

	if (m_KnightSiegeWarClanList.GetSize() == 1)
	{
		pCswEvent.Started = true;
		CastleSiegeDeatchmatchClose();
		return;
	}

	CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
	if (pKnight != nullptr)
	{
		_CASTELLAN_OLD_CSW_WINNER* pData = new _CASTELLAN_OLD_CSW_WINNER;
		pData->m_oldClanID = pKnight->GetID();
		if (!g_pMain->m_KnightSiegeWarCastellanOldWinner.PutData(pData->m_oldClanID, pData))
			delete pData;
	}

	pCswEvent.Started = true;
	pCswEvent.Status = CswOperationStatus::BarrackCreated;
	pCswEvent.MonumentType = 1;
	pCswEvent.CswTime = (uint32)UNIXTIME + (pPlayTimer->BarrackCreatedTime * MINUTE);
	CastleSiegeWarUserTools(true, CswNoticeStatus::NoticeDeployBarrack, false, true, false);
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarDeatchmatchOpen()
void CGameServerDlg::CastleSiegeWarDeatchmatchOpen()
{
	_CSW_EVENT_PLAY_TIMER* pPlayTimer = m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr)
	{
		CastleSiegeWarGenelClose();
		return;
	}

	if (m_KnightSiegeWarBarrackList.GetSize() <= 0)
	{
		CastleSiegeWarGenelClose();
		return;
	}

	if (m_KnightSiegeWarBarrackList.GetSize() == 1)
	{
		CastleSiegeDeatchmatchClose();
		return;
	}

	m_byBattleOpen = SIEGE_BATTLE;
	m_byOldBattleOpen = SIEGE_BATTLE;

	pCswEvent.Status = CswOperationStatus::DeathMatch;
	pCswEvent.CswTime = (uint32)UNIXTIME + (pPlayTimer->DeathMatchTime * MINUTE);

	CastleSiegeWarUserTools(true, CswNoticeStatus::NoticeDeathMatch, true, true, false);
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeDeatchmatchClose()
void CGameServerDlg::CastleSiegeDeatchmatchClose()
{
	_CSW_EVENT_PLAY_TIMER* pPlayTimer = m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr)
	{
		CastleSiegeWarGenelClose();
		return;
	}

	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;
	CastleSiegeWarDeathMatchWinnerCheck();
	m_KnightSiegeWarCastellanClanList.DeleteAllData();
	g_DBAgent.LoadKnightSiegeCastellanEnteredClan();

	pCswEvent.Status = CswOperationStatus::Preparation;
	pCswEvent.CswTime = (uint32)UNIXTIME + (pPlayTimer->PreparingTime * MINUTE);
	CastleSiegeWarUserTools(true, CswNoticeStatus::NoticePreparation, true, true, false);
	CastleSiegeWarAutoKilledBarrack();

	C3DMap* pMap = g_pMain->GetZoneByID(ZONE_DELOS);
	if (pMap)
		pMap->UpdateDelosDuringCSW(true, ZoneAbilitySiege1);
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeCastellanWarOpen()
void CGameServerDlg::CastleSiegeCastellanWarOpen()
{
	_CSW_EVENT_PLAY_TIMER* pPlayTimer = m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr)
	{
		CastleSiegeWarGenelClose();
		return;
	}

	m_byBattleOpen = SIEGE_BATTLE;
	m_byOldBattleOpen = SIEGE_BATTLE;
	pCswEvent.Status = CswOperationStatus::CastellanWar;
	pCswEvent.CswTime = (uint32)UNIXTIME + (pPlayTimer->CastellanTime * MINUTE);

	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege != nullptr)
	{
		CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
		if (pKnight != nullptr)
		{
			pCswEvent.MonumentTime = (uint32)UNIXTIME + (pPlayTimer->OwnerDelosMonumentFinish * MINUTE);
			pCswEvent.isMonumentActive = true;
		}
	}
	CastleSiegeWarUserTools(true, CswNoticeStatus::NoticeCastellan, true, false, true);
}
#pragma endregion

#pragma region CNpc::CastleSiegeWarMonumentKilled(CUser *pUser)
void CNpc::CastleSiegeWarMonumentKilled(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
	{
		if (g_pMain->m_byBattleOpen == SIEGE_BATTLE && pUser->isInClan())
		{
			_CASTELLAN_WAR_INFO* pData = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(pUser->GetClanID());
			if (pData == nullptr)
			{
				_CASTELLAN_OLD_CSW_WINNER* pOldClan = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(pUser->GetClanID());
				if (pOldClan == nullptr)
					return;
			}

			CKnights* pClan = g_pMain->GetClanPtr(pUser->GetClanID());
			if (pClan == nullptr)
				return;

			if (pClan->m_byFlag >= ClanTypePromoted && pClan->m_byGrade >= 1)
				CastleSiegeWarMonumentProcess(pUser);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarFragSaved()
void CGameServerDlg::CastleSiegeWarFragSaved()
{
	if (pCswEvent.Started == true)
	{
		foreach_stlmap_nolock(itr, m_KnightSiegeWarClanList)
		{
			if (itr->second == nullptr)
				continue;

			g_DBAgent.SaveKnightSiegeWarRank(itr->second->m_tClanID, itr->second->m_tTotalKills, itr->second->m_tBarracksKills, itr->second->m_tRemainBarracks);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarGameStartController()
void CGameServerDlg::CastleSiegeWarGameStartController()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege != nullptr)
	{
		if (pKnightSiege->sMasterKnights != 0)
		{
			int8 testing = g_DBAgent.KnightSiegeWarGameStartController(pKnightSiege->sMasterKnights);
			if (testing == 1)
			{
				pKnightSiege->sMasterKnights = 0;
				g_pMain->UpdateSiege(pKnightSiege->sCastleIndex, pKnightSiege->sMasterKnights, pKnightSiege->bySiegeType, pKnightSiege->byWarDay, pKnightSiege->byWarTime, pKnightSiege->byWarMinute);
			}
		}
	}
}
#pragma endregion

#pragma region CDBAgent::KnightSiegeWarGameStartController()
int8 CDBAgent::KnightSiegeWarGameStartController(uint16 ClanID)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL CASTLE_SIEGE_WAR_CONTROL(%d)}"), ClanID)))
	{
		ReportSQLError(m_GameDB->GetError());
		return bRet;
	}

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::SaveKnightSiegeWarRank(uint16 ClanID, uint16 TotalKils, uint8 BarrackKills, uint8 RemainBarrack)
bool CDBAgent::SaveKnightSiegeWarRank(uint16 ClanID, uint16 TotalKils, uint8 BarrackKills, uint8 RemainBarrack)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	int8 bRet = 0; // generic error

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL SAVE_KNIGHT_SIEGE_RANK(%d,%d,%d,%d)}"),
		ClanID, TotalKils, BarrackKills, RemainBarrack)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return false;
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarAutoKilledBarrack()
void CGameServerDlg::CastleSiegeWarAutoKilledBarrack()
{
	CNpcThread* zoneitrThread = g_pMain->m_arNpcThread.GetData(ZONE_DELOS);
	if (zoneitrThread == nullptr)
		return;

	zoneitrThread->m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock(itr, zoneitrThread->m_arNpcArray)
	{
		CNpc* pNpc = TO_NPC(itr->second);
		if (pNpc == nullptr)
			continue;

		if (pNpc->isDead()
			|| pNpc->GetZoneID() != ZONE_DELOS
			|| pNpc->isMonster()
			|| pNpc->GetProtoID() != 511)
			continue;

		pNpc->DeadReq();
	}
	zoneitrThread->m_arNpcArray.m_lock.unlock();
}
#pragma endregion

#pragma region CNpc::CastleSiegeWarMonumentProcess(CUser * pUser)
void CNpc::CastleSiegeWarMonumentProcess(CUser* pUser)
{
	_CSW_EVENT_PLAY_TIMER* pPlayTimer = g_pMain->m_CswEventPlayTimerArray.GetData(EventLocalID::CastleSiegeWar);
	if (pPlayTimer == nullptr
		|| pUser == nullptr)
		return;

	_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiegeWar != nullptr)
	{
		pKnightSiegeWar->sMasterKnights = pUser->GetClanID();
		g_pMain->UpdateSiege(pKnightSiegeWar->sCastleIndex, pKnightSiegeWar->sMasterKnights, pKnightSiegeWar->bySiegeType, pKnightSiegeWar->byWarDay, pKnightSiegeWar->byWarTime, pKnightSiegeWar->byWarMinute);
	}

	g_pMain->pCswEvent.isMonumentActive = true;
	g_pMain->pCswEvent.MonumentTime = (uint32)UNIXTIME + (pPlayTimer->MonumentFinishTime * MINUTE);
	g_pMain->pCswEvent.MonumentType = 2;

	g_pMain->CastleSiegeWarUserTools(true, CswNoticeStatus::NoticeMonumentKilled, true, false, true);
	g_pMain->ResetAllEventObject(ZONE_DELOS);
}
#pragma endregion

#pragma region CUser::CastleSiegeWarFlag()
void CUser::CastleSiegeWarFlag()
{
	Packet result(WIZ_SIEGE, uint8(2));
	if (g_pMain->isCswActive())
	{
		int32 CswTime = 0, MonumentTime = 0;

		if (g_pMain->pCswEvent.CswTime != 0)
		{
			CswTime = g_pMain->pCswEvent.CswTime - (uint32)UNIXTIME;
			if (CswTime <= 0)
				CswTime = 0;
		}

		if (g_pMain->isCastellanWarActive() && g_pMain->pCswEvent.isMonumentActive)
		{
			if (g_pMain->pCswEvent.MonumentTime != 0)
			{
				MonumentTime = g_pMain->pCswEvent.MonumentTime - (uint32)UNIXTIME;
				if (MonumentTime <= 0)
					MonumentTime = 0;
			}
		}

		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege != nullptr)
		{
			CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnight != nullptr)
			{
				result << g_pMain->pCswEvent.MonumentType
					<< pKnight->GetID()
					<< pKnight->m_sMarkVersion
					<< pKnight->m_byFlag
					<< pKnight->m_byGrade
					<< MonumentTime
					<< int16(CswTime);

				result.SByte();
				result << pKnight->GetName();
				Send(&result);
				return;
			}
		}

		string ClanName = "";
		result << uint8(g_pMain->pCswEvent.MonumentType)
			<< uint16(0)
			<< uint32(0)
			<< MonumentTime
			<< int16(CswTime);

		result.SByte();
		result << ClanName;
		Send(&result);
	}
	else
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege != nullptr)
		{
			CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnight != nullptr)
			{
				result << g_pMain->pCswEvent.MonumentType
					<< pKnight->GetID()
					<< pKnight->m_sMarkVersion
					<< pKnight->m_byFlag
					<< pKnight->m_byGrade;
				Send(&result);
			}
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarFinishWinnerCheck()
void CGameServerDlg::CastleSiegeWarFinishWinnerCheck()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege == nullptr)
		return;

	CKnights* pKnights = GetClanPtr(pKnightSiege->sMasterKnights);
	if (pKnights == nullptr)
		return;

	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != ZONE_DELOS)
			continue;

		pUser->SetZoneAbilityChange(ZONE_DELOS);
		if (pUser->GetClanID() == pKnights->GetID())
		{
			pUser->AchieveWarCountAdd(UserAchieveWarTypes::AchieveWinCSW, 0, nullptr);
			pUser->GiveNPoints((uint16)g_pMain->m_sCastleSiegeWarDeathMatchWinner1);
			pUser->SendLoyaltyChange(g_pMain->m_sCastleSiegeWarDeathMatchWinner);
		}

		if (pUser->GetClanID() != pKnights->GetID())
		{
			pUser->SendLoyaltyChange(g_pMain->m_sCastleSiegeWarWinner2);
			pUser->GiveNPoints((uint16)g_pMain->m_sCastleSiegeWarWinner3);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarDeathMatchWinnerCheck()
void CGameServerDlg::CastleSiegeWarDeathMatchWinnerCheck()
{
	uint8 WinnerCount = 0;
	std::vector<_DEATHMATCH_WAR_INFO> WinnerClanRank;
	foreach_stlmap(itr, g_pMain->m_KnightSiegeWarClanList)
	{
		if (itr->second == nullptr)
			continue;

		WinnerCount++;
		if (WinnerCount >= 6)
			break;

		WinnerClanRank.push_back(*itr->second);
	}

	std::sort(WinnerClanRank.begin(), WinnerClanRank.end(),
		[](_DEATHMATCH_WAR_INFO const& a, _DEATHMATCH_WAR_INFO const& b)
	{ return a.m_tBarracksKills == b.m_tBarracksKills ? a.m_tTotalKills > b.m_tTotalKills : a.m_tBarracksKills > b.m_tBarracksKills; });

	uint8 WinnerClanRankRewar = 0;
	if ((int32)WinnerClanRank.size() > 0)
	{
		foreach(itr, WinnerClanRank)
		{
			_DEATHMATCH_WAR_INFO* pRankInfo = &(*itr);
			if (pRankInfo == nullptr)
				continue;

			WinnerClanRankRewar++;
			CastleSiegeWarDeathMatchWinnerReward(pRankInfo->m_tClanID, WinnerClanRankRewar);
		}
	}
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarDeathMatchWinnerReward(uint16 WinnerClanID, uint8 WinnerRankID)
void CGameServerDlg::CastleSiegeWarDeathMatchWinnerReward(uint16 WinnerClanID, uint8 WinnerRankID)
{
	if (WinnerClanID <= 0 || WinnerRankID <= 0 || WinnerRankID >= 6)
		return;

	CKnights* pWinnerClan = nullptr;
	g_pMain->m_KnightsArray.m_lock.lock();
	foreach_stlmap_nolock(itr, g_pMain->m_KnightsArray)
	{
		if (itr->second == nullptr)
			continue;

		if (itr->second->GetID() == WinnerClanID)
			pWinnerClan = itr->second;
	}
	g_pMain->m_KnightsArray.m_lock.unlock();

	pWinnerClan->m_arKnightsUser.m_lock.lock();
	foreach_stlmap_nolock(itr, pWinnerClan->m_arKnightsUser)
	{
		_KNIGHTS_USER* pFirstClanKnightUser = itr->second;
		if (pFirstClanKnightUser == nullptr)
			continue;

		CUser* pWinnerClanUser = g_pMain->GetUserPtr(pFirstClanKnightUser->strUserName, TYPE_CHARACTER);
		if (pWinnerClanUser == nullptr)
			continue;

		if (!pWinnerClanUser->isInGame())
			continue;

		if (WinnerRankID == 1)
		{
			pWinnerClanUser->GiveItem(g_pMain->m_sCastleSiegeWarDeathMatchWinner, 1); //New Cape Voucher (7 Günlük)
			pWinnerClanUser->GiveItem(g_pMain->m_sCastleSiegeWarDeathMatchWinner1, 1); //Siege War Reward Chest
		}
		else if (WinnerRankID >= 2 
			&& WinnerRankID <= 5)
			pWinnerClanUser->GiveItem(g_pMain->m_sCastleSiegeWarDeathMatchWinner2, 1); //Siege War Reward Chest

		pWinnerClanUser->SendLoyaltyChange(g_pMain->m_sCastleSiegeWarDeathMatchWinner3);
	}
	pWinnerClan->m_arKnightsUser.m_lock.unlock();
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarGenelClose()
void CGameServerDlg::CastleSiegeWarGenelClose()
{
	CastleSiegeWarFinishWinnerCheck();
	CastleSiegeWarReset();
	CastleSiegeWarUserTools(true, CswNoticeStatus::NoticeCswFinish, true, false, true);

	g_DBAgent.CastleSiegeWarRegisterReset();
	m_KnightSiegeWarCastellanOldWinner.DeleteAllData();
	ResetAllEventObject(ZONE_DELOS);

	C3DMap* pMap = GetZoneByID(ZONE_DELOS);
	if (pMap)
		pMap->UpdateDelosDuringCSW(false);
}
#pragma endregion

#pragma region CUser::CastleSiegeWarDeathmacthRegister()
uint8 CUser::CastleSiegeWarDeathmacthRegister()
{
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;
	int nSec = g_localTime.tm_sec;

	if (!isClanLeader() 
		&& !isClanAssistant())
		return 2;

	CKnights* pClan = g_pMain->GetClanPtr(GetClanID());
	if (pClan == nullptr)
		return 3;

	if (pClan->m_byGrade > 3
		|| pClan->m_byFlag < ClanTypePromoted)
	{
		if (pClan->m_byFlag >= ClanTypeAccredited5)
			goto fail_return;

		return 3;
	}

fail_return:
	uint8 result = g_DBAgent.KnightsSiegeWarRegisterChecking(GetClanID(), GetName());
	if (result == 4)
		return 4;

	if (result == 5)
		return 5;

	uint8 result2 = g_DBAgent.KnightsSiegeWarRegisterSuccess(GetClanID(), GetName());
	if (result2 != 1)
		return 2;

	return 1;
}
#pragma endregion

#pragma region CUser::CastleSiegeWarDeathmacthCancelRegister()
uint8 CUser::CastleSiegeWarDeathmacthCancelRegister()
{
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;
	int nSec = g_localTime.tm_sec;

	if (!isClanLeader())
		return 2;

	uint8 result = g_DBAgent.KnightsSiegeWarCancelChecking(GetClanID(), GetName());
	if (result == 3)
		return 3;

	uint8 result2 = g_DBAgent.KnightsSiegeWarCamcelSuccess(GetClanID(), GetName());
	if (result2 != 1)
		return 2;

	return 1;
}
#pragma endregion

#pragma region CUser::CastleSiegeWarRegisterClanShow()
void CUser::CastleSiegeWarRegisterClanShow()
{
	if (!isInGame())
		return;

	Packet result(WIZ_SIEGE, uint8(5));
	result << uint8(1);
	g_DBAgent.LoadKnightSiegeWarRegister(result);
	Send(&result);
}
#pragma endregion

#pragma region CUser::CastleSiegeWarClanRankShow()
void CUser::CastleSiegeWarClanRankShow()
{
	if (!isInGame())
		return;

	Packet result(WIZ_SIEGE, uint8(5));
	result << uint8(2);

	uint8 sReaminCount = 0;
	uint16 sCount = 0;
	size_t wpos = result.wpos();
	result << sCount;
	result.DByte();

	std::vector<_DEATHMATCH_WAR_INFO> ClanRankingSorted;
	foreach_stlmap(itr, g_pMain->m_KnightSiegeWarClanList)
	{
		if (itr->second == nullptr)
			continue;

		ClanRankingSorted.push_back(*itr->second);
	}

	std::sort(ClanRankingSorted.begin(), ClanRankingSorted.end(),
		[](_DEATHMATCH_WAR_INFO const& a, _DEATHMATCH_WAR_INFO const& b)
	{ return a.m_tBarracksKills == b.m_tBarracksKills ? a.m_tTotalKills > b.m_tTotalKills : a.m_tBarracksKills > b.m_tBarracksKills; });

	if ((int32)ClanRankingSorted.size() > 0)
	{
		foreach(itr, ClanRankingSorted)
		{
			_DEATHMATCH_WAR_INFO* pRankInfo = &(*itr);
			if (pRankInfo == nullptr)
				continue;

			sReaminCount += pRankInfo->m_tRemainBarracks;
			result << pRankInfo->m_tClanID
				<< pRankInfo->m_tMark
				<< pRankInfo->m_tClanName
				<< pRankInfo->m_tBarracksKills
				<< pRankInfo->m_tTotalKills
				<< pRankInfo->m_tRemainBarracks
				<< sReaminCount;
			sCount++;
		}
	}
	result.put(wpos, sCount);
	Send(&result);
}
#pragma endregion

#pragma region CDBAgent::LoadKnightSiegeWarRegister()
void CDBAgent::LoadKnightSiegeWarRegister(Packet& pkt)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL LOAD_KNIGHTS_SIEGE_WAR_REGISTER}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	uint8 sCount = 0;
	size_t wpos = pkt.wpos();
	pkt << sCount;
	pkt.DByte();
	do
	{
		uint16 sBaseClanNum;
		int16 sMarkVersion;
		string sBaseClanCommander, strKnightsName;

		dbCommand->FetchString(1, sBaseClanCommander);
		dbCommand->FetchString(2, strKnightsName);
		dbCommand->FetchInt16(3, sMarkVersion);
		dbCommand->FetchUInt16(4, sBaseClanNum);

		pkt << sBaseClanNum
			<< sMarkVersion
			<< strKnightsName
			<< sBaseClanCommander;
		sCount++;

	} while (dbCommand->MoveNext());

	pkt.put(wpos, sCount);
}
#pragma endregion

#pragma region CDBAgent::LoadKnightSiegeCastellanEnteredClan()
bool CDBAgent::LoadKnightSiegeCastellanEnteredClan()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_KNIGHTS_SIEGE_WAR_RANK_RESULT}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		_CASTELLAN_WAR_INFO* pData = new _CASTELLAN_WAR_INFO;
		dbCommand->FetchUInt16(1, pData->m_tClanID);
		dbCommand->FetchUInt16(2, pData->m_tTotalKills);
		dbCommand->FetchByte(3, pData->m_tBarracksKills);
		dbCommand->FetchByte(4, pData->m_tRemainBarracks);

		if (!g_pMain->m_KnightSiegeWarCastellanClanList.PutData(pData->m_tClanID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadKnightSiegeCastellanRankTable()
bool CDBAgent::LoadKnightSiegeCastellanRankTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_CASTELLAN_ENTERED_CLAN}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return true;
	do
	{
		_CASTELLAN_WAR_INFO* pData = new _CASTELLAN_WAR_INFO;
		dbCommand->FetchUInt16(1, pData->m_tClanID);
		dbCommand->FetchUInt16(2, pData->m_tTotalKills);
		dbCommand->FetchByte(3, pData->m_tBarracksKills);
		dbCommand->FetchByte(4, pData->m_tRemainBarracks);

		if (!g_pMain->m_KnightSiegeWarCastellanClanList.PutData(pData->m_tClanID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::CastleSiegeWarRegisterReset()
bool CDBAgent::CastleSiegeWarRegisterReset()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL KNIGHTS_SIEGE_WAR_DEATMATCH_REGISTER_RESET}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	return true;
}
#pragma endregion

#pragma region CDBAgent::LoadKnightSiegeWarRankTable()
bool CDBAgent::LoadKnightSiegeWarRankTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_SIEGE_RANK}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return true;

	do
	{
		_DEATHMATCH_WAR_INFO* pData = new _DEATHMATCH_WAR_INFO;
		dbCommand->FetchUInt16(1, pData->m_tTotalKills);
		dbCommand->FetchByte(2, pData->m_tBarracksKills);
		dbCommand->FetchByte(3, pData->m_tRemainBarracks);
		dbCommand->FetchString(4, pData->m_tClanName);
		dbCommand->FetchInt16(5, pData->m_tMark);
		dbCommand->FetchUInt16(6, pData->m_tClanID);

		if (!g_pMain->m_KnightSiegeWarClanList.PutData(pData->m_tClanID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarRegisterChecking(uint16 sClanID, std::string & strUserID)
uint8 CDBAgent::KnightsSiegeWarRegisterChecking(uint16 sClanID, std::string& strUserID)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SIEGE_WAR_DEATMATCH_REGISTER_CHECK(%d, ?)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarRegisterSuccess(uint16 sClanID, std::string & strUserID)
uint8 CDBAgent::KnightsSiegeWarRegisterSuccess(uint16 sClanID, std::string& strUserID)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SIEGE_WAR_DEATMATCH_REGISTER(%d, ?)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarCancelChecking(uint16 sClanID, std::string & strUserID)
uint8 CDBAgent::KnightsSiegeWarCancelChecking(uint16 sClanID, std::string& strUserID)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SIEGE_WAR_DEATMATCH_CANCEL_CHECK(%d, ?)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarCamcelSuccess(uint16 sClanID, std::string & strUserID)
uint8 CDBAgent::KnightsSiegeWarCamcelSuccess(uint16 sClanID, std::string& strUserID)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SIEGE_WAR_DEATMATCH_CANCEL(%d, ?)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::LoadKnightSiegeEnteredClan()
bool CDBAgent::LoadKnightSiegeEnteredClan()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL LOAD_KNIGHTS_SIEGE_WAR_START_RANK_REGISTER}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	do
	{
		_DEATHMATCH_WAR_INFO* pData = new _DEATHMATCH_WAR_INFO;
		dbCommand->FetchUInt16(1, pData->m_tTotalKills);
		dbCommand->FetchByte(2, pData->m_tBarracksKills);
		dbCommand->FetchByte(3, pData->m_tRemainBarracks);
		dbCommand->FetchString(4, pData->m_tClanName);
		dbCommand->FetchInt16(5, pData->m_tMark);
		dbCommand->FetchUInt16(6, pData->m_tClanID);

		if (!g_pMain->m_KnightSiegeWarClanList.PutData(pData->m_tClanID, pData))
			delete pData;

	} while (dbCommand->MoveNext());

	return true;
}
#pragma endregion

#pragma region CUser::UpateCSWTotalKillCounts()
void CUser::UpateCSWTotalKillCounts()
{
	_DEATHMATCH_WAR_INFO* pClanInfo = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
	if (pClanInfo == nullptr)
		return;

	if (pClanInfo->m_tRemainBarracks <= 0)
		return;

	pClanInfo->m_tTotalKills++;
}
#pragma endregion

#pragma region CUser::UpateCSWTotalBarracksKillCounts()
void CUser::UpateCSWTotalBarracksKillCounts()
{
	_DEATHMATCH_WAR_INFO* pClanInfo = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
	if (pClanInfo == nullptr)
		return;

	if (pClanInfo->m_tRemainBarracks <= 0)
		return;

	pClanInfo->m_tBarracksKills++;
}
#pragma endregion

#pragma region CGameServerDlg::UpdateCSWRemainBarracks()
void CGameServerDlg::UpdateCSWRemainBarracks(uint16 sClanID)
{
	_DEATHMATCH_WAR_INFO* pClanInfo = g_pMain->m_KnightSiegeWarClanList.GetData(sClanID);
	if (pClanInfo == nullptr)
		return;

	if (pClanInfo->m_tRemainBarracks <= 0)
		return;

	pClanInfo->m_tRemainBarracks--;

	if (pClanInfo->m_tRemainBarracks <= 0)
		pClanInfo->m_tRemainBarracks = 0;
}
#pragma endregion

#pragma region CUser::CastleSiegeWarBaseCreate()
void CUser::CastleSiegeWarBaseCreate()
{
	/*
	-1: You cannot set up your base at the current moment.
	-2: This is not a Castle Siege War zone.
	-3: You cannot set up your base here.
	*/
	Packet result(WIZ_SIEGE);
	bool BaseNotCreated = false;

	if (GetZoneID() != ZONE_DELOS)
	{
		result << uint8(1) << uint8(1) << uint16(-2);
		Send(&result);
		return;
	}

	if (g_pMain->pCswEvent.Status != CswOperationStatus::BarrackCreated)
	{
		result << uint8(1) << uint8(1) << uint16(-1);
		Send(&result);
		return;
	}

	if (!isClanLeader())
	{
		result << uint8(1) << uint8(1) << uint16(-1);
		Send(&result);
		return;
	}

	_DEATHMATCH_WAR_INFO* pData3 = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
	if (pData3 == nullptr)
	{
		result << uint8(1) << uint8(1) << uint16(-1);
		Send(&result);
		return;
	}

	bool GenisAlan = (GetX() >= 323.0f && GetX() <= 711
		&& GetZ() >= 360 && GetZ() <= 615);

	bool ObjectAlan = (isInRangeSlow(650.0f, 563.0f, 15)
		|| isInRangeSlow(543.0f, 614.0f, 15)
		|| isInRangeSlow(649.0f, 565.0f, 15)
		|| isInRangeSlow(572.0f, 481.0f, 15)
		|| isInRangeSlow(557.0f, 467.0f, 15)
		|| isInRangeSlow(479.0f, 438.0f, 15)
		|| isInRangeSlow(694.0f, 465.0f, 20)
		|| isInRangeSlow(436.0f, 378.0f, 15)
		|| isInRangeSlow(405.0f, 420.0f, 15)
		|| isInRangeSlow(355.0f, 500.0f, 10)
		|| isInRangeSlow(324.0f, 527.0f, 15)
		|| isInRangeSlow(323.0f, 606.0f, 10)
		|| isInRangeSlow(353.0f, 566.0f, 15)
		|| isInRangeSlow(373.0f, 572.0f, 10)
		|| isInRangeSlow(385.0f, 597.0f, 10));

	if (!GenisAlan)
	{
		result << uint8(1) << uint8(1) << uint16(-3);
		Send(&result);
		return;
	}

	if (ObjectAlan)
	{
		result << uint8(1) << uint8(1) << uint16(-3);
		Send(&result);
		return;
	}

	if (g_pMain->m_KnightSiegeWarBarrackList.GetSize() > 0)
	{
		g_pMain->m_KnightSiegeWarBarrackList.m_lock.lock();
		foreach_stlmap(itr, g_pMain->m_KnightSiegeWarBarrackList)
		{
			_DEATHMATCH_BARRACK_INFO* pData2 = g_pMain->m_KnightSiegeWarBarrackList.GetData(itr->second->m_tBaseClanID);
			if (pData2 != nullptr)
			{
				uint16 PosX = pData2->m_tBaseX;
				uint16 PosZ = pData2->m_tBaseZ;

				if (isInRangeSlow(PosX, PosZ, 10))
					BaseNotCreated = true;
			}
		}
		g_pMain->m_KnightSiegeWarBarrackList.m_lock.unlock();
	}

	if (BaseNotCreated)
	{
		BaseNotCreated = false;
		result << uint8(1) << uint8(1) << uint16(-3);
		Send(&result);
		return;
	}

	_DEATHMATCH_BARRACK_INFO* pData4 = g_pMain->m_KnightSiegeWarBarrackList.GetData(GetClanID());
	if (pData4 != nullptr)
	{
		result << uint8(1) << uint8(1) << uint16(-1);
		Send(&result);
		return;
	}
	else
	{
		uint8 CreatedResultInsert = g_DBAgent.KnightsSiegeWarBarrackCreated(1, GetClanID(), GetSPosX(), GetSPosZ());
		if (CreatedResultInsert != 1)
		{
			result << uint8(1) << uint8(1) << uint16(-3);
			Send(&result);
			return;
		}

		_DEATHMATCH_BARRACK_INFO* pData = new _DEATHMATCH_BARRACK_INFO;
		pData->m_tBaseClanID = GetClanID();
		pData->m_tBaseX = (int)m_curx;
		pData->m_tBaseZ = (int)m_curz;

		if (!g_pMain->m_KnightSiegeWarBarrackList.PutData(pData->m_tBaseClanID, pData))
		{
			delete pData;
			uint8 CreatedResultDelete = g_DBAgent.KnightsSiegeWarBarrackCreated(2, GetClanID(), GetSPosX(), GetSPosZ());
			if (CreatedResultDelete != 1)
			{
				result << uint8(1) << uint8(1) << uint16(-3);
				Send(&result);
				return;
			}
		}

		_DEATHMATCH_WAR_INFO* pDeathmatchinfo = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
		if (pDeathmatchinfo != nullptr)
		{
			uint8 CreatedResultUpdate = g_DBAgent.KnightsSiegeWarBarrackCreated(3, GetClanID(), GetSPosX(), GetSPosZ());

			if (CreatedResultUpdate != 1)
			{
				result << uint8(1) << uint8(1) << uint16(-3);
				Send(&result);
				delete pData;
				return;
			}

			pDeathmatchinfo->m_tRemainBarracks = 1;
		}

		g_pMain->SpawnEventNpc(511, false, GetZoneID(), GetX(), GetY(), GetZ(), 1, 2, 0, 0, GetSocketID(), 0, 0, 4, 0, 0, 0);
	}
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarBarrackCreated(uint16 sClanID, uint16 X, uint16 Z)
uint8 CDBAgent::KnightsSiegeWarBarrackCreated(uint8 Type, uint16 sClanID, uint16 X, uint16 Z)
{
	int8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL KNIGHTS_SIEGE_WAR_BARRACK_INSERT(%d, %d, %d, %d)}"), Type, sClanID, X, Z)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}
#pragma endregion

#pragma region CDBAgent::KnightsSiegeWarBarrackResetTable()
bool CDBAgent::KnightsSiegeWarBarrackResetTable()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("{CALL KNIGHTS_SIEGE_WAR_BARRACK_RESET}")))
		ReportSQLError(m_GameDB->GetError());

	return true;
}
#pragma endregion

#pragma region CNpc::CastleSiegeWarBarrackKilled(CUser *pUser)
void CNpc::CastleSiegeWarBarrackKilled(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	switch (GetProtoID())
	{
	case 511:
	{
		if (!pUser->isInClan())
			return;

		_DEATHMATCH_WAR_INFO* pClanInfo = g_pMain->m_KnightSiegeWarClanList.GetData(pUser->GetClanID());
		if (pClanInfo == nullptr)
			return;

		if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
			pUser->UpateCSWTotalBarracksKillCounts();

		CKnights* pKnights = g_pMain->GetClanPtr(m_oBarrackID);
		if (pKnights == nullptr)
			return;

		_DEATHMATCH_BARRACK_INFO* pBarrakInfo = g_pMain->m_KnightSiegeWarBarrackList.GetData(pKnights->GetID());
		if (pBarrakInfo == nullptr)
			return;

		g_pMain->UpdateCSWRemainBarracks(pKnights->GetID());

		foreach_stlmap(itr, pKnights->m_arKnightsUser)
		{
			_KNIGHTS_USER* p = itr->second;
			if (p == nullptr)
				continue;

			CUser* pTUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
			if (pTUser == nullptr
				|| !pTUser->isInGame()
				|| pTUser->GetZoneID() != ZONE_DELOS)
				continue;

			pTUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
		}

		if (g_pMain->m_KnightSiegeWarBarrackList.GetData(pBarrakInfo->m_tBaseClanID))
			g_pMain->m_KnightSiegeWarBarrackList.DeleteData(pBarrakInfo->m_tBaseClanID);


		_DEATHMATCH_WAR_INFO* pClanInfo2 = g_pMain->m_KnightSiegeWarClanList.GetData(pKnights->GetID());
		if (pClanInfo2 == nullptr)
			return;

		CKnights* pKnights2 = g_pMain->GetClanPtr(pUser->GetClanID());
		if (pKnights2 == nullptr)
			return;

		Packet result;
		string notice;

		// %s knighttage's barrack has been destroyed by %s of %s knightage
		g_pMain->GetServerResource(IDS_BARRACK_DEATH_INFO, &notice, pKnights->GetName().c_str(), pKnights2->GetName().c_str(), pUser->GetName().c_str());

		// Wrap it in a "#### NOTICE : ####" block.
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

		// Construct & send the chat/announcement packet
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
		g_pMain->Send_All(&result, nullptr, Nation::ALL, ZONE_DELOS);

		//ölen barakayý sildir veritabaný
		if (g_pMain->m_KnightSiegeWarBarrackList.GetSize() <= 1)
		{
			g_pMain->m_KnightSiegeWarBarrackList.DeleteAllData();
			g_pMain->KickOutZoneUsers(ZONE_DELOS, ZONE_MORADON);
			//bütün barakalarý sildir veritabaný
		}
	}break;
	}
}
#pragma endregion

#pragma region CUser::DelosCasttellanZoneOut()
void CUser::DelosCasttellanZoneOut()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiege == nullptr)
		return;

	CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
	if (pKnight == nullptr)
		return;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr)
			continue;

		CKnights* pTKnight = g_pMain->GetClanPtr(pUser->GetClanID());

		if (pUser->GetZoneID() != ZONE_DELOS_CASTELLAN)
			continue;

		if (pTKnight == nullptr)
			continue;

		if (pKnight->GetID() == pUser->GetClanID())
			continue;

		if (pTKnight->GetAllianceID() == pKnightSiege->sMasterKnights)
			continue;

		pUser->ZoneChange(ZONE_MORADON, 0.0f, 0.0f);
	}
}
#pragma endregion

#pragma region CUser::CanEnterDelos()
bool CUser::CanEnterDelos()
{
	switch (g_pMain->pCswEvent.Status)
	{
	case CswOperationStatus::BarrackCreated:
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege == nullptr)
			return false;

		_DEATHMATCH_WAR_INFO* pKnightList = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
		if (pKnightList == nullptr)
			return false;

		CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
		if (pKnights != nullptr)
		{
			if (GetClanID() == pKnights->GetID())
				return false;
		}
	}
	break;
	case CswOperationStatus::DeathMatch:
	{
		_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
		if (pKnightSiege == nullptr)
			return false;

		_DEATHMATCH_BARRACK_INFO* pData4 = g_pMain->m_KnightSiegeWarBarrackList.GetData(GetClanID());
		if (pData4 == nullptr)
			return false;

		_DEATHMATCH_WAR_INFO* pKnightList = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
		if (pKnightList == nullptr)
			return false;

		CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
		if (pKnights != nullptr)
		{
			if (GetClanID() == pKnights->GetID())
				return false;
		}
	}
	break;
	case CswOperationStatus::Preparation:
	{
		_CASTELLAN_WAR_INFO* pKnightList2 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
		if (pKnightList2 == nullptr)
		{
			_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			if (pKnightSiege == nullptr)
				return false;

			CKnights* pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);
			if (pKnights != nullptr)
			{
				if (GetClanID() != pKnights->GetID())
					return false;
			}
		}
	}
	break;
	case CswOperationStatus::CastellanWar:
	{
		_CASTELLAN_WAR_INFO* pKnightList2 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
		if (pKnightList2 == nullptr)
		{
			_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
			if (pKnightSiege == nullptr)
				return false;

			_CASTELLAN_OLD_CSW_WINNER* pOldClan = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(GetClanID());
			if (pOldClan == nullptr)
				return false;
		}
	}
	break;
	}

	if (g_pMain->isCswActive())
	{
		CKnights* pClan = g_pMain->GetClanPtr(GetClanID());
		if (pClan == nullptr)
			return false;

		if (pClan->m_byGrade > 3
			|| pClan->m_byFlag < ClanTypePromoted)
		{
			if (pClan->m_byFlag >= ClanTypeAccredited5)
				goto fail_return;

			return false;
		}
	}

fail_return:
	if (GetLoyalty() <= 0)
		return false;

	return true;
}
#pragma endregion

#pragma region CUser::SiegeWarFareProcess(Packet & pkt)
void CUser::SiegeWarFareProcess(Packet& pkt)
{
	uint8 opcode, type;
	uint16 tarrif;
	pkt >> opcode >> type >> tarrif;

	_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiegeWarFare == nullptr)
		return;

	CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiegeWarFare->sMasterKnights);

	Packet result(WIZ_SIEGE);
	switch (opcode)
	{
	case 1:
	{
		result << opcode << type;
		switch (type)
		{
		case 1:
			CastleSiegeWarBaseCreate();
			break;
		default:
			printf("Siege Npc Base Created unhandlec packets %d \n", type);
			TRACE("Siege Npc Base Created unhandlec packets %d \n", type);
			break;
		}
	}
	break;
	case 5:
		result << opcode << type;
		switch (type)
		{
		case 1:
			CastleSiegeWarRegisterClanShow();
			break;
		case 2:
			CastleSiegeWarClanRankShow();
			break;
		default:
			printf("Siege Rank unhandlec packets %d \n", type);
			TRACE("Siege Rank Created unhandlec packets %d \n", type);
			break;
		}
		break;
	case 3: //moradon npc
	{
		result << opcode << type;
		switch (type)
		{
		case 2:
			result << pKnightSiegeWarFare->sCastleIndex
				<< uint16(pKnightSiegeWarFare->bySiegeType)
				<< pKnightSiegeWarFare->byWarDay
				<< pKnightSiegeWarFare->byWarTime
				<< pKnightSiegeWarFare->byWarMinute;
			Send(&result);
			break;
		case 4:
			if (pKnight == nullptr)
				return;

			result.SByte();
			result
				<< pKnightSiegeWarFare->sCastleIndex
				<< uint8(1)
				<< pKnight->GetName()
				<< pKnight->m_byNation
				<< pKnight->m_sMembers
				<< pKnightSiegeWarFare->byWarRequestDay
				<< pKnightSiegeWarFare->byWarRequestTime
				<< pKnightSiegeWarFare->byWarRequestMinute;
			Send(&result);
			break;
		case 5:
			if (pKnight == nullptr)
				return;

			result.SByte();
			result
				<< pKnightSiegeWarFare->sCastleIndex
				<< pKnightSiegeWarFare->bySiegeType
				<< pKnight->GetName()
				<< pKnight->m_byNation
				<< pKnight->m_sMembers;
			Send(&result);
			break;

		default:
			printf("Siege Npc Maradon unhandled packets %d \n", type);
			TRACE("Siege Npc Maradon unhandled packets %d \n", type);
			break;
		}
	}break;

	case 4: //delos npc // Fixed by TheThyke
	{
		result << opcode << type;
		switch (type)
		{
		case 2: // get all coins from the fund
			if (isKing())
			{
				if (pKnightSiegeWarFare->nDellosTax + pKnightSiegeWarFare->nMoradonTax + GetCoins() > COIN_MAX)
				{
					result.Initialize(WIZ_QUEST);
					result << uint8(13) << uint8(2);
					Send(&result);
					return;
				}
				else
				{
					uint32 gold = 0;
					gold = pKnightSiegeWarFare->nDellosTax + pKnightSiegeWarFare->nMoradonTax;
					GoldGain(gold, true);
					pKnightSiegeWarFare->nDellosTax = 0;
					pKnightSiegeWarFare->nMoradonTax = 0;
				}
			}
			else
			{
				if (pKnightSiegeWarFare->nDungeonCharge + GetCoins() > COIN_MAX)
				{
					result.Initialize(WIZ_QUEST);
					result << uint8(13) << uint8(2);
					Send(&result);
					return;
				}
				else
				{
					pKnightSiegeWarFare->nDungeonCharge;
					GoldGain(pKnightSiegeWarFare->nDungeonCharge, true);
					pKnightSiegeWarFare->nDungeonCharge = 0;
				}
			}
			// Tell database to update
			g_pMain->UpdateSiegeTax(0, 0);
			break;
		case 3:
			if (isKing())
				return;

			result << pKnightSiegeWarFare->sCastleIndex
				<< pKnightSiegeWarFare->sMoradonTariff
				<< pKnightSiegeWarFare->sDellosTariff
				<< pKnightSiegeWarFare->nDungeonCharge;
			Send(&result);
			break;
		case 4:
		{
			if (tarrif > 20 || isKing())
				return;

			pKnightSiegeWarFare->sMoradonTariff = tarrif;

			C3DMap* pMap = g_pMain->GetZoneByID(ZONE_MORADON);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sMoradonTariff));

			pMap = g_pMain->GetZoneByID(ZONE_MORADON2);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sMoradonTariff));

			pMap = g_pMain->GetZoneByID(ZONE_MORADON3);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sMoradonTariff));

			pMap = g_pMain->GetZoneByID(ZONE_MORADON4);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sMoradonTariff));

			pMap = g_pMain->GetZoneByID(ZONE_MORADON5);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sMoradonTariff));

			result << uint16(1) << tarrif << uint8(ZONE_MORADON);
			g_pMain->Send_All(&result);

			// Tell database to update
			g_pMain->UpdateSiegeTax(ZONE_MORADON, pKnightSiegeWarFare->sMoradonTariff);
		}
		break;
		case 5:
		{
			if (tarrif > 20 || isKing())
				return;

			pKnightSiegeWarFare->sDellosTariff = tarrif;

			C3DMap* pMap = g_pMain->GetZoneByID(ZONE_DELOS);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sDellosTariff));

			pMap = g_pMain->GetZoneByID(ZONE_DESPERATION_ABYSS);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sDellosTariff));

			pMap = g_pMain->GetZoneByID(ZONE_HELL_ABYSS);
			if (pMap != nullptr)
				pMap->SetTariff(uint8(pKnightSiegeWarFare->sDellosTariff));

			result << uint16(1) << tarrif << uint8(ZONE_DELOS);
			g_pMain->Send_All(&result);

			// Tell database to update
			g_pMain->UpdateSiegeTax(ZONE_DELOS, pKnightSiegeWarFare->sDellosTariff);
		}
		break;
		default:
			printf("Siege Npc Dels unhandled packets %d \n", type);
			TRACE("Siege Npc Dels unhandled packets %d \n", type);
			break;
		}
	}
	break;
	default:
		printf("Siege Npc - unhandled packets type: %d opcode: %d\n", type, opcode);
		TRACE("Siege Npc - unhandled packets type: %d opcode: %d\n", type, opcode);
		break;
	}
}
#pragma endregion

#pragma region CUser::isCswWinnerNembers()
bool CUser::isCswWinnerNembers()
{
	_KNIGHTS_SIEGE_WARFARE* pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
	if (pKnightSiegeWar == nullptr)
		return false;

	if (pKnightSiegeWar->sMasterKnights == 0)
		return false;

	CKnights* pKnight = g_pMain->GetClanPtr(pKnightSiegeWar->sMasterKnights);
	if (pKnight == nullptr)
		return false;

	CKnights* pTKnight = g_pMain->GetClanPtr(GetClanID());
	if (pTKnight == nullptr)
		return false;

	if (pKnight->GetID() == GetClanID())
	{
		ZoneChange(ZONE_DELOS_CASTELLAN, 458.0f, 113.0f);
		return false;
	}

	if (pTKnight->GetAllianceID() == pKnightSiegeWar->sMasterKnights)
	{
		ZoneChange(ZONE_DELOS_CASTELLAN, 458.0f, 113.0f);
		return false;
	}

	return false;
}
#pragma endregion

#pragma region CGameServerDlg::CastleSiegeWarReset()
void CGameServerDlg::CastleSiegeWarReset()
{
	pCswEvent.Started = false;
	pCswEvent.isMonumentFinish = false;
	pCswEvent.isMonumentActive = false;
	pCswEvent.BarrackCreateTimerControl = false;
	pCswEvent.DeathMatchTimerControl = false;
	pCswEvent.PreparationTimerControl = false;
	pCswEvent.CastellanWarTimerControl = false;
	pCswEvent.Status = CswOperationStatus::NotOperation;
	pCswEvent.CswTime = 0;
	pCswEvent.MonumentTime = 0;
	pCswEvent.MonumentType = 0;
	pCswEvent.BarrackNoticeTime = 0;
	pCswEvent.DeathMatchNoticeTime = 0;
	pCswEvent.PreparationNoticeTime = 0;
	pCswEvent.CastellanWarNoticeTime = 0;
	pCswEvent.MonumentNoticeTime = 0;

	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;
}
#pragma endregion
#pragma endregion