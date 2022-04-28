#include "stdafx.h"
#include "DBAgent.h"
using namespace std;
using std::string;

void CGameServerDlg::GameInfoNoticeTimer()
{
	if (m_GameInfo1Time > 0)
		m_GameInfo1Time--;

	if (m_GameInfo1Time <= 0)
	{
		m_GameInfo1Time = 900;
		GameInfo1Packet();
	}

	if (m_GameInfo2Time > 0)
		m_GameInfo2Time--;

	if (m_GameInfo2Time <= 0)
	{
		m_GameInfo2Time = 450;///
		GameInfo2Packet();
	}

	if (m_GameInfo3Time > 0)
		m_GameInfo3Time--;

	if (m_GameInfo3Time <= 0)
	{
		m_GameInfo3Time = 300;
		GameInfo3Packet();
	}
}

void CGameServerDlg::GameInfo1Packet()
{
	Packet result;
	std::string notice;
	GetServerResource(IDS_NOTICE_INFO_1, &notice);
	ChatPacket::Construct(&result, (uint8)WAR_SYSTEM_CHAT, &notice);
	Send_All(&result, nullptr, Nation::ALL);
}

void CGameServerDlg::GameInfo2Packet()
{
	Packet result;
	std::string notice;
	GetServerResource(IDS_NOTICE_INFO_2, &notice);
	ChatPacket::Construct(&result, (uint8)PUBLIC_CHAT, &notice);
	Send_All(&result, nullptr, Nation::ALL);
}

void CGameServerDlg::GameInfo3Packet()
{
	Packet result;
	std::string notice;
	GetServerResource(IDS_NOTICE_INFO_3, &notice);
	ChatPacket::Construct(&result, (uint8)PUBLIC_CHAT, &notice);
	Send_All(&result, nullptr, Nation::ALL);
}

void CUser::BannedWallHack()
{
	DateTime time;
	printf("[Disconnected WallHack] -> [%d:%d:%d] -> %s Disconnected by Wall[Hack]\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str());

	std::string sNoticeMessage = string_format("%s Disconnected by Wall[Hack]", GetName().c_str());
	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);

	m_bAuthority = AUTHORITY_BANNED;
	Disconnect();
}

void CUser::BannedSpeedHack()
{
	DateTime time;
	printf("[Disconnected SpeedHack] -> [%d:%d:%d] -> %s Disconnected by speed[Hack]\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str());

	std::string sNoticeMessage = string_format("%s Disconnected by Speed[Hack]", GetName().c_str());
	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);

	m_bAuthority = AUTHORITY_BANNED;
	Disconnect();
}

void CGameServerDlg::AutoOnlineCount() //yenicount
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if ((nMinute % 30 == 0 || nMinute == 0) && nSecond == 0)
	{
		uint16 count = 0;
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			if (TO_USER(itr->second)->isInGame())
				count++;
		}

		Guard lock(g_pMain->m_BotcharacterNameLock); // Count'a Botu da ekleme
		count += (uint16)g_pMain->m_BotcharacterNameMap.size(); // Count'a Botu da ekleme

		std::string sNoticeMessage = string_format("Online User Count : [%d] Thank you for being with us.", count * 5); //bilgiyi oyundaki tüm kullanýcýlara gönderir
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
	}
}

#pragma  region CUser::wallhack()
void CUser::wallhack() {

	switch (GetZoneID())
	{
	case ZONE_MORADON3:
	case ZONE_MORADON2:
	case ZONE_MORADON:
	{

		if (uint16(GetY()) >= 6500 && uint16(GetY()) <= 6553)
		{
			if ((6553 - uint16(GetY())) > 27)
				BannedWallHack();
		}
		else if (uint16(GetY()) > 21)
		{
			BannedWallHack();
		}
	}
	break;
	case ZONE_RONARK_LAND:
	{

		if (uint16(GetY()) >= 6500 && uint16(GetY()) <= 6553)
		{
			if ((6553 - uint16(GetY())) > 15)
				BannedWallHack();
		}
		else if (uint16(GetY()) > 39)
		{
			BannedWallHack();
		}
	}
	break;
	case ZONE_BATTLE:
	{

		if (uint16(GetY()) >= 6500 && uint16(GetY()) <= 6553)
		{
			if ((6553 - uint16(GetY())) > 52)
				BannedWallHack();
		}
		else if (uint16(GetY()) > 18)
		{
			BannedWallHack();
		}
	}
	break;
	/*case ZONE_BATTLE2:
	{

		if (uint16(GetY()) > 92)
		{
			BannedWallHack();
		}
	}
	break;
	case ZONE_BATTLE4:
	{

		if (uint16(GetY()) >= 6500 && uint16(GetY()) <= 6553)
		{
			if ((6553 - uint16(GetY())) > 21)
				BannedWallHack();
		}
		else if (uint16(GetY()) > 25)
		{
			BannedWallHack();
		}
	}
	break;*/
	}
}
#pragma endregion