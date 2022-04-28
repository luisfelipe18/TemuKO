#include "stdafx.h"
#include "KingSystem.h"
#include "DBAgent.h"
#include <iostream>

uint32 CGameServerDlg::Timer_UpdateGameTime(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->UpdateGameTime();
		sleep(1 * SECOND);
	}
	return 0;
}

void CGameServerDlg::UpdateGameTime()
{
	DateTime now(&g_localTime);

	if (m_sSec != now.GetSecond())
	{
		// Check timed King events.
		foreach_stlmap(itr, m_KingSystemArray)
			itr->second->CheckKingTimer();

		uint16 m_bOnlineCount = 0, m_bMerchantCount = 0, m_bMiningCount = 0, m_bGenieCount = 0, m_bFishingCount = 0;
		std::vector<CBot*> deleted;

		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser *pUser = TO_USER(itr->second);
			if (pUser == nullptr 
				|| !pUser->isInGame())
				continue;

			if (pUser->isMerchanting())
				m_bMerchantCount++;

			if (pUser->isMining())
				m_bMiningCount++;

			if (pUser->isFishing())
				m_bFishingCount++;

			if (pUser->isGenieActive())
				m_bGenieCount++;

			m_bOnlineCount++;
		}
		m_BotArray.m_lock.lock();
		foreach_stlmap_nolock(itr, m_BotArray)
		{
			CBot* pUser = itr->second;

			if (pUser == nullptr)
				continue;

			if (!pUser->isInGame())
				continue;

			if (pUser->LastWarpTime < UNIXTIME)
				deleted.push_back(pUser);

			if ((pUser->m_bResHpType == USER_MINING
				|| pUser->m_bResHpType == USER_FLASHING)
				&& pUser->m_iGold + 15 < uint32(UNIXTIME))
			{
				Packet result(WIZ_MINING, uint8(MiningAttempt));
				uint16 resultCode = MiningResultSuccess, Random = myrand(0, 10000);
				uint16 sEffect = 0;

				if (Random > 4000
					|| pUser->m_bResHpType == USER_SITDOWN) // EXP
					sEffect = 13082; // "XP" effect
				else
					sEffect = 13081; // "Item" effect

				result << resultCode << pUser->GetID() << sEffect;
				pUser->SendToRegion(&result);
				pUser->m_iGold = uint32(UNIXTIME);
			}

			if (pUser->isMerchanting())
				m_bMerchantCount++;

			if (pUser->m_bResHpType == USER_MINING)
				m_bMiningCount++;

			if (pUser->m_bResHpType == USER_FLASHING)
				m_bFishingCount++;

			if (pUser->isGenieActive())
				m_bGenieCount++;

			m_bOnlineCount++;
		}
		m_BotArray.m_lock.unlock();
		g_pMain->m_checktime -= 1 * SECOND;

		if (g_pMain->m_checktime <= 0)
		{
			g_pMain->m_checktime = 60 * SECOND;

			std::string texts;
			texts = string_format("GA Server - v2500 - [Online :%d - Merchant :%d - Mining :%d - Fishing :%d - Genie :%d]", m_bOnlineCount, m_bMerchantCount, m_bMiningCount, m_bFishingCount, m_bGenieCount);
			SetConsoleTitle(texts.c_str());
		}

		foreach(itr, deleted) {
			if ((*itr)->isSlaveMerchant())
			{
				CUser* pSlaveUser = g_pMain->GetUserPtr((*itr)->m_bSlaveUserID);
				if (pSlaveUser != nullptr)
					pSlaveUser->GiveSlaveMerchantItems();
			}

			(*itr)->UserInOut(INOUT_OUT);
		}
	}

	// Every minute
	if (m_sMin != now.GetMinute())
	{
		m_ReloadKnightAndUserRanksMinute++;//xx

		if (m_ReloadKnightAndUserRanksMinute == RELOAD_KNIGHTS_AND_USER_RATING)
		{
			m_ReloadKnightAndUserRanksMinute = 0;
			color_c("", FOREGROUND_BLUE + FOREGROUND_BLUE + FOREGROUND_GREEN);
			printf("[Microsoft SQL Server] -> ");
			color_c("", FOREGROUND_BLUE + FOREGROUND_GREEN);
			printf(" [Loads table] -> ");
			color_c("", FOREGROUND_INTENSITY + 6);
			printf(" [ON] -> ");
			color_c("", FOREGROUND_BLUE + FOREGROUND_GREEN);
			printf(" [KNIGHTS_AND_USER_RATING]\n");
			color_c("", FOREGROUND_INTENSITY + 6);
			ReloadKnightAndUserRanks();
			//printf("[SQL Server] -> [Loads table] -> [ON] -> [Knights and User Ranking]\n");
		}

		// Player Ranking Rewards
		std::list<std::string> vargs = StrSplit(m_sPlayerRankingsRewardZones, ",");
		uint8 nZones = (uint8)vargs.size();
		if (nZones > 0)
		{
			uint8 nZoneID = 0;
			for (int i = 0; i < nZones; i++)
			{
				nZoneID = atoi(vargs.front().c_str());
				SetPlayerRankingRewards(nZoneID);
				vargs.pop_front();
			}
		}
	}

	/* BASE LAND EVENT */
	if (BaseLandEventGameServerSatus)
	{
		BaseLandEventFinishTime--;

		if (BaseLandEventFinishTime == 30 * 60 || BaseLandEventFinishTime == 28 * 60 || BaseLandEventFinishTime == 25 * 60 ||
			BaseLandEventFinishTime == 21 * 60 || BaseLandEventFinishTime == 17 * 60 || BaseLandEventFinishTime == 13 * 60 ||
			BaseLandEventFinishTime == 10 * 60 || BaseLandEventFinishTime == 5 * 60 || BaseLandEventFinishTime == 4 * 60 ||
			BaseLandEventFinishTime == 3 * 60 || BaseLandEventFinishTime == 2 * 60 || BaseLandEventFinishTime == 1 * 60)
		{
			LogosYolla("[Base Land Event]", string_format("Event %d Dakika Sonra Bitecektir", BaseLandEventFinishTime / 60), 1, 240, 1);
		}

		if (BaseLandEventFinishTime == 0)
		{
			BaseLandFinished();
		}
	}
	/*BASE LAND EVENT */

	/* ARDREAM EVENT */
	if (ArdreamEventGameServerSatus)
	{
		ArdreamEventFinishTime--;

		if (ArdreamEventFinishTime == 30 * 60 || ArdreamEventFinishTime == 28 * 60 || ArdreamEventFinishTime == 25 * 60 ||
			ArdreamEventFinishTime == 21 * 60 || ArdreamEventFinishTime == 17 * 60 || ArdreamEventFinishTime == 13 * 60 ||
			ArdreamEventFinishTime == 10 * 60 || ArdreamEventFinishTime == 5 * 60 || ArdreamEventFinishTime == 4 * 60 ||
			ArdreamEventFinishTime == 3 * 60 || ArdreamEventFinishTime == 2 * 60 || ArdreamEventFinishTime == 1 * 60)
		{
			LogosYolla("[Ardream Event]", string_format("Event %d Dakika Sonra Bitecektir", ArdreamEventFinishTime / 60), 1, 240, 1);
		}

		if (ArdreamEventFinishTime == 0)
		{
			ArdreamEventZoneClose();
		}
	}
	/*ARDREAM EVENT */

	/* OLD CZ EVENT */
	if (CzEventGameServerSatus)
	{
		CzEventFinishTime--;

		if (CzEventFinishTime == 30 * 60 || CzEventFinishTime == 28 * 60 || CzEventFinishTime == 25 * 60 ||
			CzEventFinishTime == 21 * 60 || CzEventFinishTime == 17 * 60 || CzEventFinishTime == 13 * 60 ||
			CzEventFinishTime == 10 * 60 || CzEventFinishTime == 5 * 60 || CzEventFinishTime == 4 * 60 ||
			CzEventFinishTime == 3 * 60 || CzEventFinishTime == 2 * 60 || CzEventFinishTime == 1 * 60)
		{
			LogosYolla("[Old CZ Event]", string_format("Event %d Dakika Sonra Bitecektir", CzEventFinishTime / 60), 1, 240, 1);
		}

		if (CzEventFinishTime == 0)
		{
			CZEventZoneClose();
		}
	}
	/*OLD CZ EVENT */

	// Every hour
	if (m_sHour != now.GetHour())
	{
		ResetPlayerRankings();
		UpdateWeather();
		SetGameTime();

		if (m_bSantaOrAngel)
			SendFlyingSantaOrAngel();

		Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_SAVE));
		g_pMain->AddDatabaseRequest(result);
	}

	// Every day
	if (m_sDate != now.GetDay())
	{
		_KNIGHTS_SIEGE_WARFARE *pSiegeWarFare = GetSiegeMasterKnightsPtr(Aktive);

		if (pSiegeWarFare != nullptr)
			g_DBAgent.UpdateSiegeWarfareDB(pSiegeWarFare->nMoradonTax, pSiegeWarFare->nDellosTax, pSiegeWarFare->nDungeonCharge);

		foreach_stlmap(itr, m_KingSystemArray)
			g_DBAgent.UpdateKingSystemDB(itr->second->m_byNation, itr->second->m_nNationalTreasury, itr->second->m_nTerritoryTax);
	}

	// Every month
	if (m_sMonth != now.GetMonth())
	{
		// Reset monthly NP.
		ResetLoyaltyMonthly();
	}

	/* Vanguard System Start */
	WantedEventManuelStarted++;

	if (WantedEventManuelStarted == 60) // Kaç dakikada bir seçileceði
	{
		WantedEventManuelStarted = 0;

		if (g_pMain->VanGuardSelectStatus == false)
			g_pMain->VanGuardSelectUser();

	}

	if (VanGuardSelectStatus)
	{
		if (VanGuardTime == 0)
			VanGuardFinish(3);

		VanGuardTime--;

		if (VanGuardTime == 600
			|| VanGuardTime == 540
			|| VanGuardTime == 480
			|| VanGuardTime == 420
			|| VanGuardTime == 360
			|| VanGuardTime == 300
			|| VanGuardTime == 240
			|| VanGuardTime == 180
			|| VanGuardTime == 120
			|| VanGuardTime == 60)
		{
			std::string sVanGuardMessage = string_format("Vanguard son %d dakika.", VanGuardTime / 60);
			if (!sVanGuardMessage.empty())
			{
				g_pMain->SendNotice(sVanGuardMessage.c_str(), Nation::ALL);
				//g_pMain->SendAnnouncement(sVanGuardMessage.c_str(), Nation::ALL);
			}
		}
	}

	if (g_pMain->VanGuardSelect)
		VanGuardSelectTime--;

	if (g_pMain->VanGuardSelect && VanGuardSelectTime < 1)
	{
		VanGuardSelectTime = 0;
		VanGuardSelect = false;
		VanGuardSelectUser();
	}
	/* Vanguard System End */

	/* BOSS EVENT */
	if (BossEventDurumu)
	{
		BossEventTimer--;

		if (BossEventTimer == 5 * 60 || BossEventTimer == 4 * 60 || BossEventTimer == 3 * 60 || BossEventTimer == 2 * 60 || BossEventTimer == 1 * 60)
		{
			std::string sNoticeMessage = string_format("BOSS Event Etkinliði baþlýyor. %d dakika içerisinde Ronark Land'da random olarak spawn olucaktýr.", BossEventTimer / 60);
			if (!sNoticeMessage.empty())
			{
				g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
				g_pMain->SendAnnouncement(sNoticeMessage.c_str(), Nation::ALL);
			}
		}

		if (BossEventTimer == 0)
		{
			std::string sNoticeMessage2 = string_format("BOSS Event Ronark Land Zonesinde baþlatýldý, hediyeler sizleri bekliyor. Tüm oyuncularýmýza iyi þanslar.");
			if (!sNoticeMessage2.empty())
			{
				g_pMain->SendNotice(sNoticeMessage2.c_str(), Nation::ALL);
				g_pMain->SendAnnouncement(sNoticeMessage2.c_str(), Nation::ALL);
			}

			/*g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1001, 0, (float)989);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)996, 0, (float)1017);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1024, 0, (float)1031);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1018, 0, (float)1014);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1028, 0, (float)989);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1064, 0, (float)976);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1081, 0, (float)978);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1104, 0, (float)984);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1110, 0, (float)988);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1127, 0, (float)1056);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1096, 0, (float)1090);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)995, 0, (float)1116);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1028, 0, (float)989);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1002, 0, (float)1087);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)951, 0, (float)1041);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)977, 0, (float)1041);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)974, 0, (float)1026);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1001, 0, (float)1017);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1008, 0, (float)1012);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1016, 0, (float)1001);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1020, 0, (float)976);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1015, 0, (float)959);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1057, 0, (float)910);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1022, 0, (float)911);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)985, 0, (float)926);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)992, 0, (float)953);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)987, 0, (float)965);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)999, 0, (float)978);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1089, 0, (float)103);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1076, 0, (float)107);
			g_pMain->SpawnEventNpc(3155, true, ZONE_RONARK_LAND, (float)1051, 0, (float)109);*/
			g_pMain->SpawnEventNpc(1311, true, ZONE_RONARK_LAND, (float)1110, 0, (float)988);
			g_pMain->SpawnEventNpc(1402, true, ZONE_RONARK_LAND, (float)1127, 0, (float)1056);
			g_pMain->SpawnEventNpc(2005, true, ZONE_RONARK_LAND, (float)1096, 0, (float)1090);
			g_pMain->SpawnEventNpc(2405, true, ZONE_RONARK_LAND, (float)995, 0, (float)1116);
			g_pMain->SpawnEventNpc(506, true, ZONE_RONARK_LAND, (float)1028, 0, (float)989);
			g_pMain->SpawnEventNpc(7035, true, ZONE_RONARK_LAND, (float)1002, 0, (float)1087);
			g_pMain->SpawnEventNpc(8867, true, ZONE_RONARK_LAND, (float)951, 0, (float)1041);
			g_pMain->SpawnEventNpc(1400, true, ZONE_RONARK_LAND, (float)977, 0, (float)1041);
			g_pMain->SpawnEventNpc(1205, true, ZONE_RONARK_LAND, (float)974, 0, (float)1026);
			g_pMain->SpawnEventNpc(1206, true, ZONE_RONARK_LAND, (float)1001, 0, (float)1017);
			g_pMain->SpawnEventNpc(1207, true, ZONE_RONARK_LAND, (float)1008, 0, (float)1012);
			g_pMain->SpawnEventNpc(608, true, ZONE_RONARK_LAND, (float)1016, 0, (float)1001);
			g_pMain->SpawnEventNpc(2105, true, ZONE_RONARK_LAND, (float)1020, 0, (float)976);
			g_pMain->SpawnEventNpc(9250, true, ZONE_RONARK_LAND, (float)1015, 0, (float)959);
			g_pMain->SpawnEventNpc(9560, true, ZONE_RONARK_LAND, (float)1057, 0, (float)910);
			g_pMain->SpawnEventNpc(9507, true, ZONE_RONARK_LAND, (float)1022, 0, (float)911);
			g_pMain->SpawnEventNpc(8510, true, ZONE_RONARK_LAND, (float)985, 0, (float)926);
			g_pMain->SpawnEventNpc(907, true, ZONE_RONARK_LAND, (float)992, 0, (float)953);
			g_pMain->SpawnEventNpc(908, true, ZONE_RONARK_LAND, (float)987, 0, (float)965);
			g_pMain->SpawnEventNpc(1306, true, ZONE_RONARK_LAND, (float)999, 0, (float)978);
			g_pMain->SpawnEventNpc(1725, true, ZONE_RONARK_LAND, (float)1089, 0, (float)103);
			g_pMain->SpawnEventNpc(2205, true, ZONE_RONARK_LAND, (float)1076, 0, (float)107);
			g_pMain->SpawnEventNpc(508, true, ZONE_RONARK_LAND, (float)1051, 0, (float)109);

			BossEventTimer = 0;
			BossEventDurumu = false;
			printf("[BOSS EVENT] : -> Boss Event Baslatildi.\n");
		}
	}

	//pmevent
	if ((UNIXTIME - ChatEventUnixTime) > g_pMain->ChatEventDakika * MINUTE)
	{
		ChatEventUnixTime = UNIXTIME;

		if (!m_ChatEventStatus)
		{
			UserChatEventOn = false;
			UserChatEvent();

		}
	}

	// Update the server time
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();
	m_sSec = now.GetSecond();
}

#pragma region CGameServerDlg::Timer_UpdateSessions(void * lpParam)
uint32 CGameServerDlg::Timer_UpdateSessions(void * lpParam)
{
	while (g_bRunning)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser * pUser = TO_USER(itr->second);
			if (pUser == nullptr)
				continue;

#ifndef _DEBUG // ignore timeouts in debug builds, as we'll probably be pausing it with the debugger.
			ULONGLONG timeout = KOSOCKET_TIMEOUT;

			// User has authed, but isn't in-game yet (creating a character, or is waiting for the game to load).
			if (!pUser->m_strAccountID.empty() && !pUser->isInGame())
				timeout = KOSOCKET_LOADING_TIMEOUT;

			// Disconnect timed out sessions
			if ((UNIXTIME2 - pUser->GetLastResponseTime()) >= timeout)
			{
				if (pUser->GetName().length() > 0)
					printf("[%s] disconnected due to long term inactivity. \n", pUser->GetName().c_str());

				pUser->Disconnect();
				continue;
			}
#endif
			// Update the player, and hence any skill effects while we're here.
			pUser->Update();

			if (pUser->m_bWaitingOrder && pUser->m_tOrderRemain <= UNIXTIME)
				pUser->LoadingLogin((Packet)(WIZ_LOADING_LOGIN));

			if (pUser->m_PettingOn != nullptr)
			{
				if (pUser->m_PettingOn->sAttackStart)
				{
					CNpc* pNpc = g_pMain->GetNpcPtr(pUser->m_PettingOn->sAttackTargetID, pUser->GetZoneID());
					if (pNpc == nullptr)
					{
						pUser->m_PettingOn->sAttackStart = false;
						pUser->m_PettingOn->sAttackTargetID = - 1;
						continue;
					}
					else
					{
						if (pNpc->isDead())
						{
							pUser->m_PettingOn->sAttackStart = false;
							pUser->m_PettingOn->sAttackTargetID = -1;
							continue;
						}
						CNpc* pPet = g_pMain->GetPetPtr(pUser->GetSocketID(), pUser->GetZoneID());
						if (pPet == nullptr)
						{
							pUser->m_PettingOn->sAttackStart = false;
							pUser->m_PettingOn->sAttackTargetID = -1;
							continue;
						}
						else
						{
							if (!pPet->isInRangeSlow(pNpc, RANGE_50M))
							{
								float	warp_x = pNpc->GetX() - pPet->GetX(),
									warp_z = pNpc->GetZ() - pPet->GetZ();

								// Unable to work out orientation, so we'll just fail (won't be necessary with m_sDirection).
								float	distance = sqrtf(warp_x * warp_x + warp_z * warp_z);
								if (distance == 0.0f)
									continue;

								warp_x /= distance; warp_z /= distance;
								warp_x *= 2; warp_z *= 2;
								warp_x += pNpc->GetX(); warp_z += pNpc->GetZ();

								pPet->SendMoveResult(warp_x, 0, warp_z, distance);
								continue;
							}
							uint8 bResult = ATTACK_FAIL;
							int damage = pPet->GetDamage(pNpc);

							if (damage > 0)
							{
								pNpc->HpChange(-(damage), pPet);
								if (pNpc->isDead())
									bResult = ATTACK_TARGET_DEAD;
								else
									bResult = ATTACK_SUCCESS;
							}

							Packet result(WIZ_ATTACK, uint8(LONG_ATTACK));
							result << bResult << pPet->GetID() << pNpc->GetID();
							pPet->SendToRegion(&result);
						}
					}
				}
			}
		}
		Sleep(1 * SECOND);
	}
	return 0;
}

#pragma endregion 

#pragma region CGameServerDlg::Timer_UpdateConcurrent(void * lpParam)

uint32 CGameServerDlg::Timer_UpdateConcurrent(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->ReqUpdateConcurrent();
		sleep(120 * SECOND);
	}
	return 0;
}

#pragma endregion 

#pragma region CGameServerDlg::Timer_NpcThreadHandleRequests(void * lpParam)
uint32 CGameServerDlg::Timer_NpcThreadHandleRequests(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->NpcThreadHandleRequests();
		sleep(500);
	}
	return 0;
}
#pragma endregion 

#pragma region CGameServerDlg::NpcThreadHandleRequests()
void CGameServerDlg::NpcThreadHandleRequests()
{
	foreach_stlmap(itr, g_pMain->m_arNpcThread)
	{
		CNpcThread* pNpcThread = itr->second;
		if (pNpcThread)
			pNpcThread->AddRequest(new Signal(NpcThreadSignalType::NpcHandle));
	}
}
#pragma endregion 

#pragma region CGameServerDlg::ReqUpdateConcurrent()
void CGameServerDlg::ReqUpdateConcurrent()
{
	uint32 sCount = ((uint32)m_socketMgr.GetActiveSessionMap().size() + (uint32)g_pMain->m_BotcharacterNameMap.size());

	Packet result(WIZ_ZONE_CONCURRENT);
	result << uint32(m_nServerNo)
		<< sCount;
	AddDatabaseRequest(result);
}
#pragma endregion 