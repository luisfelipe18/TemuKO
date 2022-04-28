#include "stdafx.h"

#pragma region CGameServerDlg::ClanTournamentTimer()
void CGameServerDlg::ClanTournamentTimer()
{
	_TOURNAMENT_DATA* TournamentAndream = g_pMain->m_ClanVsDataList.GetData(77);
	if (TournamentAndream != nullptr)
	{
		if (TournamentAndream->aTournamentisStarted == true)
		{
			if (TournamentAndream->aTournamentisFinished == false)
			{
				if (TournamentAndream->aTournamentTimer == 0)
				{
					if (TournamentAndream->aTournamentScoreBoard[0] > TournamentAndream->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentAndream->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 1, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentAndream->aTournamentScoreBoard[1] > TournamentAndream->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentAndream->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 1, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentAndream->aTournamentScoreBoard[0] == TournamentAndream->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 1);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentAndream->aTournamentOutTimer = UNIXTIME + 300;
					TournamentAndream->aTournamentisStarted = false;
				}
			}

			if (TournamentAndream->aTournamentOutTimer == UNIXTIME)
				TournamentAndream->aTournamentisFinished = true;

			if (TournamentAndream->aTournamentOutTimer <= UNIXTIME
				&& TournamentAndream->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentAndream->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentAndream->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentAndream->aTournamentScoreBoard[0], TournamentAndream->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_CLAN_WAR_ARDREAM, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(77);
			}
		}

		if (TournamentAndream->aTournamentTimer > 0)
			TournamentAndream->aTournamentTimer--;
	}

	_TOURNAMENT_DATA* TournamentRonarkLand = g_pMain->m_ClanVsDataList.GetData(78);
	if (TournamentRonarkLand != nullptr)
	{
		if (TournamentRonarkLand->aTournamentisStarted == true)
		{
			if (TournamentRonarkLand->aTournamentisFinished == false)
			{
				if (TournamentRonarkLand->aTournamentTimer == 0)
				{
					if (TournamentRonarkLand->aTournamentScoreBoard[0] > TournamentRonarkLand->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentRonarkLand->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 2, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentRonarkLand->aTournamentScoreBoard[1] > TournamentRonarkLand->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentRonarkLand->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 2, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentRonarkLand->aTournamentScoreBoard[0] == TournamentRonarkLand->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 2);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentRonarkLand->aTournamentOutTimer = UNIXTIME + 300;
					TournamentRonarkLand->aTournamentisStarted = false;
				}
			}

			if (TournamentRonarkLand->aTournamentOutTimer == UNIXTIME)
				TournamentRonarkLand->aTournamentisFinished = true;

			if (TournamentRonarkLand->aTournamentOutTimer <= UNIXTIME
				&& TournamentRonarkLand->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentRonarkLand->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentRonarkLand->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentRonarkLand->aTournamentScoreBoard[0], TournamentRonarkLand->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_CLAN_WAR_RONARK, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(78);
			}
		}

		if (TournamentRonarkLand->aTournamentTimer > 0)
			TournamentRonarkLand->aTournamentTimer--;
	}

	_TOURNAMENT_DATA* TournamentParty1 = g_pMain->m_ClanVsDataList.GetData(96);
	if (TournamentParty1 != nullptr)
	{
		if (TournamentParty1->aTournamentisStarted == true)
		{
			if (TournamentParty1->aTournamentisFinished == false)
			{
				if (TournamentParty1->aTournamentTimer == 0)
				{
					if (TournamentParty1->aTournamentScoreBoard[0] > TournamentParty1->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty1->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 1, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty1->aTournamentScoreBoard[1] > TournamentParty1->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty1->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 1, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty1->aTournamentScoreBoard[0] == TournamentParty1->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 1);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentParty1->aTournamentOutTimer = UNIXTIME + 300;
					TournamentParty1->aTournamentisStarted = false;
				}
			}

			if (TournamentParty1->aTournamentOutTimer == UNIXTIME)
				TournamentParty1->aTournamentisFinished = true;

			if (TournamentParty1->aTournamentOutTimer <= UNIXTIME
				&& TournamentParty1->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty1->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty1->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentParty1->aTournamentScoreBoard[0], TournamentParty1->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_PARTY_VS_1, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(96);
			}
		}

		if (TournamentParty1->aTournamentTimer > 0)
			TournamentParty1->aTournamentTimer--;
	}

	_TOURNAMENT_DATA* TournamentParty2 = g_pMain->m_ClanVsDataList.GetData(97);
	if (TournamentParty2 != nullptr)
	{
		if (TournamentParty2->aTournamentisStarted == true)
		{
			if (TournamentParty2->aTournamentisFinished == false)
			{
				if (TournamentParty2->aTournamentTimer == 0)
				{
					if (TournamentParty2->aTournamentScoreBoard[0] > TournamentParty2->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty2->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 2, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty2->aTournamentScoreBoard[1] > TournamentParty2->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty2->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 2, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty2->aTournamentScoreBoard[0] == TournamentParty2->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 2);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentParty2->aTournamentOutTimer = UNIXTIME + 300;
					TournamentParty2->aTournamentisStarted = false;
				}
			}

			if (TournamentParty2->aTournamentOutTimer == UNIXTIME)
				TournamentParty2->aTournamentisFinished = true;

			if (TournamentParty2->aTournamentOutTimer <= UNIXTIME
				&& TournamentParty2->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty2->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty2->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentParty2->aTournamentScoreBoard[0], TournamentParty2->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_PARTY_VS_2, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(97);
			}
		}

		if (TournamentParty2->aTournamentTimer > 0)
			TournamentParty2->aTournamentTimer--;
	}

	_TOURNAMENT_DATA* TournamentParty3 = g_pMain->m_ClanVsDataList.GetData(98);
	if (TournamentParty3 != nullptr)
	{
		if (TournamentParty3->aTournamentisStarted == true)
		{
			if (TournamentParty3->aTournamentisFinished == false)
			{
				if (TournamentParty3->aTournamentTimer == 0)
				{
					if (TournamentParty3->aTournamentScoreBoard[0] > TournamentParty3->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty3->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 3, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty3->aTournamentScoreBoard[1] > TournamentParty3->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty3->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 3, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty3->aTournamentScoreBoard[0] == TournamentParty3->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 3);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentParty3->aTournamentOutTimer = UNIXTIME + 300;
					TournamentParty3->aTournamentisStarted = false;
				}
			}

			if (TournamentParty3->aTournamentOutTimer == UNIXTIME)
				TournamentParty3->aTournamentisFinished = true;

			if (TournamentParty3->aTournamentOutTimer <= UNIXTIME
				&& TournamentParty3->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty3->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty3->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentParty3->aTournamentScoreBoard[0], TournamentParty3->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_PARTY_VS_3, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(98);
			}
		}

		if (TournamentParty3->aTournamentTimer > 0)
			TournamentParty3->aTournamentTimer--;
	}

	_TOURNAMENT_DATA* TournamentParty4 = g_pMain->m_ClanVsDataList.GetData(99);
	if (TournamentParty4 != nullptr)
	{
		if (TournamentParty4->aTournamentisStarted == true)
		{
			if (TournamentParty4->aTournamentisFinished == false)
			{
				if (TournamentParty4->aTournamentTimer == 0)
				{
					if (TournamentParty4->aTournamentScoreBoard[0] > TournamentParty4->aTournamentScoreBoard[1])
					{
						CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty4->aTournamentClanNum[0]);/*Red Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 4, pRedClan == nullptr ? "null" : pRedClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty4->aTournamentScoreBoard[1] > TournamentParty4->aTournamentScoreBoard[0])
					{
						CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty4->aTournamentClanNum[1]);/*Blue Clan*/
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_NOTICE, &notice, 4, pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str());
						ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
						Send_All(&result, nullptr, Nation::ALL);
					}
					else if (TournamentParty4->aTournamentScoreBoard[0] == TournamentParty4->aTournamentScoreBoard[1])
					{
						Packet result;
						std::string notice;
						GetServerResource(IDS_CLAN_WAR_DRAW_NOTICE, &notice, 4);
						ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
						Send_All(&result);
					}
					TournamentParty4->aTournamentOutTimer = UNIXTIME + 300;
					TournamentParty4->aTournamentisStarted = false;
				}
			}

			if (TournamentParty4->aTournamentOutTimer == UNIXTIME)
				TournamentParty4->aTournamentisFinished = true;

			if (TournamentParty4->aTournamentOutTimer <= UNIXTIME
				&& TournamentParty4->aTournamentisFinished == true)
			{
				DateTime time;
				CKnights *pRedClan = g_pMain->GetClanPtr(TournamentParty4->aTournamentClanNum[0]);/*Red Clan*/
				CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentParty4->aTournamentClanNum[1]);/*Blue Clan*/
				WriteTournamentLogs(string_format("[Tournament Finish Time - %d:%d:%d] Red Clan Name = %s : %s Blue Clan Name (Red Board %d : %d Blue Board)\n",
					time.GetHour(), time.GetMinute(), time.GetSecond(), pRedClan == nullptr ? "null" : pRedClan->GetName().c_str(), pBlueClan == nullptr ? "null" : pBlueClan->GetName().c_str(), TournamentParty4->aTournamentScoreBoard[0], TournamentParty4->aTournamentScoreBoard[1]));

				KickOutZoneUsers(ZONE_PARTY_VS_4, ZONE_MORADON);
				m_ClanVsDataList.DeleteData(99);
			}
		}

		if (TournamentParty4->aTournamentTimer > 0)
			TournamentParty4->aTournamentTimer--;
	}
}
#pragma endregion

#pragma region CGameServerDlg::UpdateClanTournamentScoreBoard(CUser* pUser)
void CGameServerDlg::UpdateClanTournamentScoreBoard(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	bool TournamentTrueZone = (pUser->GetZoneID() == 77
		|| pUser->GetZoneID() == 78
		|| pUser->GetZoneID() == 96
		|| pUser->GetZoneID() == 97
		|| pUser->GetZoneID() == 98
		|| pUser->GetZoneID() == 99);

	if (!TournamentTrueZone)
		return;

	CKnights *pUserClan = g_pMain->GetClanPtr(pUser->GetClanID());
	_TOURNAMENT_DATA* TournamentInfo = g_pMain->m_ClanVsDataList.GetData(pUser->GetZoneID());
	if (pUserClan == nullptr
		|| TournamentInfo == nullptr
		|| (pUserClan->GetID() != TournamentInfo->aTournamentClanNum[0]
			&& pUserClan->GetID() != TournamentInfo->aTournamentClanNum[1]))
	{
		pUser->NativeZoneReturn();
		pUser->UserDataSaveToAgent();
		pUser->Disconnect();
		return;
	}

	if (TournamentInfo != nullptr)
	{
		if (TournamentInfo->aTournamentisStarted == true)
		{
			if (TournamentInfo->aTournamentTimer != 0)
			{
				if (pUser->GetClanID() == TournamentInfo->aTournamentClanNum[0])
					TournamentInfo->aTournamentScoreBoard[0]++;
				else if (pUser->GetClanID() == TournamentInfo->aTournamentClanNum[1])
					TournamentInfo->aTournamentScoreBoard[1]++;

				Packet TournamentClanPacket(WIZ_BATTLE_EVENT);
				TournamentClanPacket << uint8(0x12)
					<< uint8(2)/*2:Board1,3:Board2*/
					<< TournamentInfo->aTournamentScoreBoard[0]/*Red Clan Score Board */
					<< TournamentInfo->aTournamentScoreBoard[1]/*Blue Clan Score Board */
					<< TournamentInfo->aTournamentTimer/*Timer*/
					<< TournamentInfo->aTournamentMonumentKilled;/*Monument Killed Advantage*/
				g_pMain->Send_Zone(&TournamentClanPacket, TournamentInfo->aTournamentZoneID);

				TournamentClanPacket.clear();
				TournamentClanPacket.Initialize(WIZ_BIFROST);
				TournamentClanPacket << uint8(5) << uint16(TournamentInfo->aTournamentTimer);
				g_pMain->Send_Zone(&TournamentClanPacket, TournamentInfo->aTournamentZoneID);
			}
		}
	}
}
#pragma endregion

#pragma region void TournamentMonumentKillProcess(CUser* pUser)
void CNpc::TournamentMonumentKillProcess(CUser* pUser)
{
	if (pUser == nullptr)
		return;

	bool TournamentTrueZone = (pUser->GetZoneID() == 77
		|| pUser->GetZoneID() == 78
		|| pUser->GetZoneID() == 96
		|| pUser->GetZoneID() == 97
		|| pUser->GetZoneID() == 98
		|| pUser->GetZoneID() == 99);

	if (!TournamentTrueZone)
		return;

	CKnights *pUserClan = g_pMain->GetClanPtr(pUser->GetClanID());
	_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(pUser->GetZoneID());
	if (pUserClan == nullptr
		|| TournamentClanInfo == nullptr
		|| (pUserClan->GetID() != TournamentClanInfo->aTournamentClanNum[0]
			&& pUserClan->GetID() != TournamentClanInfo->aTournamentClanNum[1]))
	{
		pUser->NativeZoneReturn();
		pUser->UserDataSaveToAgent();
		pUser->Disconnect();
		return;
	}

	if (TournamentClanInfo != nullptr)
	{
		if (TournamentClanInfo->aTournamentisStarted == true)
		{
			if (TournamentClanInfo->aTournamentTimer != 0)
			{
				if (TournamentClanInfo->aTournamentScoreBoard[0] != TournamentClanInfo->aTournamentScoreBoard[1])
				{
					if (pUser->GetClanID() == TournamentClanInfo->aTournamentClanNum[0])
					{
						if (TournamentClanInfo->aTournamentClanNum[0] < TournamentClanInfo->aTournamentClanNum[1])
						{
							uint16 RedClanScore = 0, BlueClanScore = 0, HalfScore = 0, AddScore = 0;

							RedClanScore = TournamentClanInfo->aTournamentScoreBoard[0];
							BlueClanScore = TournamentClanInfo->aTournamentScoreBoard[1];

							HalfScore = (BlueClanScore - RedClanScore);
							AddScore = (HalfScore / 2);
							TournamentClanInfo->aTournamentScoreBoard[0] += AddScore;

							Packet TournamentClanPacket(WIZ_BATTLE_EVENT);
							TournamentClanPacket << uint8(0x12)
								<< uint8(2)/*2:Board1,3:Board2*/
								<< TournamentClanInfo->aTournamentScoreBoard[0]/*Red Clan Score Board */
								<< TournamentClanInfo->aTournamentScoreBoard[1]/*Blue Clan Score Board */
								<< TournamentClanInfo->aTournamentTimer/*Timer*/
								<< TournamentClanInfo->aTournamentMonumentKilled;/*Monument Killed Advantage*/
							g_pMain->Send_Zone(&TournamentClanPacket, TournamentClanInfo->aTournamentZoneID);

							TournamentClanPacket.clear();
							TournamentClanPacket.Initialize(WIZ_BIFROST);
							TournamentClanPacket << uint8(5) << uint16(TournamentClanInfo->aTournamentTimer);
							g_pMain->Send_Zone(&TournamentClanPacket, TournamentClanInfo->aTournamentZoneID);
						}
					}
					else if (pUser->GetClanID() == TournamentClanInfo->aTournamentClanNum[1])
					{
						if (TournamentClanInfo->aTournamentClanNum[1] < TournamentClanInfo->aTournamentClanNum[0])
						{
							uint16 RedClanScore = 0, BlueClanScore = 0, HalfScore = 0, AddScore = 0;

							RedClanScore = TournamentClanInfo->aTournamentScoreBoard[0];
							BlueClanScore = TournamentClanInfo->aTournamentScoreBoard[1];

							HalfScore = (RedClanScore - BlueClanScore);
							AddScore = (HalfScore / 2);
							TournamentClanInfo->aTournamentScoreBoard[1] += AddScore;

							Packet TournamentClanPacket(WIZ_BATTLE_EVENT);
							TournamentClanPacket << uint8(0x12)
								<< uint8(2)/*2:Board1,3:Board2*/
								<< TournamentClanInfo->aTournamentScoreBoard[0]/*Red Clan Score Board */
								<< TournamentClanInfo->aTournamentScoreBoard[1]/*Blue Clan Score Board */
								<< TournamentClanInfo->aTournamentTimer/*Timer*/
								<< TournamentClanInfo->aTournamentMonumentKilled;/*Monument Killed Advantage*/
							g_pMain->Send_Zone(&TournamentClanPacket, TournamentClanInfo->aTournamentZoneID);

							TournamentClanPacket.clear();
							TournamentClanPacket.Initialize(WIZ_BIFROST);
							TournamentClanPacket << uint8(5) << uint16(TournamentClanInfo->aTournamentTimer);
							g_pMain->Send_Zone(&TournamentClanPacket, TournamentClanInfo->aTournamentZoneID);
						}
					}
				}
			}
		}
	}
}
#pragma endregion

#pragma region void CUser::TournamentSendTimer()
void CUser::TournamentSendTimer()
{
	bool TournamentTrueZone = (GetZoneID() == 77
		|| GetZoneID() == 78
		|| GetZoneID() == 96
		|| GetZoneID() == 97
		|| GetZoneID() == 98
		|| GetZoneID() == 99);

	if (!TournamentTrueZone)
		return;

	_TOURNAMENT_DATA* TournamentClanInfo = g_pMain->m_ClanVsDataList.GetData(GetZoneID());
	if (TournamentClanInfo != nullptr)
	{
		if (TournamentClanInfo->aTournamentisStarted == true)
		{
			CKnights *pRedClan = g_pMain->GetClanPtr(TournamentClanInfo->aTournamentClanNum[0]);/*Red Clan*/
			CKnights *pBlueClan = g_pMain->GetClanPtr(TournamentClanInfo->aTournamentClanNum[1]);/*Blue Clan*/
			if (pRedClan != nullptr && pBlueClan != nullptr)
			{
				Packet TournamentClanPacket;
				TournamentClanPacket.Initialize(WIZ_BIFROST);
				TournamentClanPacket << uint8(5) << uint16(TournamentClanInfo->aTournamentTimer);
				Send(&TournamentClanPacket);

				TournamentClanPacket.clear();
				TournamentClanPacket.Initialize(WIZ_BATTLE_EVENT);
				TournamentClanPacket << uint8(0x12)
					<< uint8(2)/*2:Board1,3:Board2*/
					<< TournamentClanInfo->aTournamentScoreBoard[0]/*Red Clan Score Board */
					<< TournamentClanInfo->aTournamentScoreBoard[1]/*Blue Clan Score Board */
					<< TournamentClanInfo->aTournamentTimer/*Timer*/
					<< TournamentClanInfo->aTournamentMonumentKilled;/*Monument Killed Advantage*/
				Send(&TournamentClanPacket);
			}
		}
	}
}
#pragma endregion