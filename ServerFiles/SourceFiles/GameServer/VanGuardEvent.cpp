#include "stdafx.h"
#include "DBAgent.h"

void CUser::VanGuard(Packet & pkt)
{
	uint8 VanGuardPacket;
	pkt >> VanGuardPacket;

	if (!iszonevanguar())
		return;

	if (VanGuardPacket == 1)
	{
		Packet result(WIZ_VANGUARD, uint8(0x02));
		result << uint8(0x01) << uint8(0x01) << GetName();
		g_pMain->Send_Zone(&result, ZONE_RONARK_LAND);

		//g_pMain->SendHelpDescription(this, "[Wanted] : You are the Vanguard !");
		VanGuardStatus = true;
		g_pMain->VanGuardSelect = false;
		g_pMain->VanGuardSelectTime = 0;
		g_pMain->VanGuardSelectStatus = true;
		g_pMain->VanGuardTime = uint16(600);
		g_pMain->VanGuardUserName = GetName();


		Packet SexyPandaVan(WIZ_VANGUARD, uint8(0x02));
		SexyPandaVan << uint8(0x02) << uint8(0x01) << uint8(0x00) << uint16(GetX()) << uint16(GetZ()) << GetName();
		g_pMain->Send_Zone(&SexyPandaVan, ZONE_RONARK_LAND);

	}
	else
	{
		printf("VanGuard Unknow %d\n", VanGuardPacket);
		return;
	}
}

void CGameServerDlg::VanGuardFinish(int OpCode)
{
	CUser *pUser = g_pMain->GetUserPtr(g_pMain->VanGuardUserName, TYPE_CHARACTER);

	if (pUser == nullptr)
	{
		//printf("[Wanted Event] = Karakter Bulunamadi\n");
		g_pMain->VanGuardSelect = false;
		g_pMain->VanGuardSelectTime = 0;
		g_pMain->VanGuardSelectStatus = false;
		g_pMain->VanGuardTime = 0;
		g_pMain->VanGuardUserName.clear();
		return;
	}

	//clear vanguard in teh view of user
	Packet result(WIZ_VANGUARD, uint8(0x03));
	result << uint8(0x03) << uint8(0x02);
	g_pMain->Send_Zone(&result, ZONE_RONARK_LAND);

	switch (OpCode)
	{

	case 1: // Oyundan Cýktý

		g_pMain->LogosYolla_Zone("Wanted Event", string_format("%s Karakterini Oyundan Ayrýldý Wanted Event Sona Erdi!", pUser->GetName().c_str()), 0, 204, 102, ZONE_RONARK_LAND);
		g_pMain->VanGuardSelect = false;
		g_pMain->VanGuardSelectTime = 0;
		g_pMain->VanGuardSelectStatus = false;
		g_pMain->VanGuardTime = 0;
		g_pMain->VanGuardUserName.clear();
		pUser->VanGuardStatus = false;
		//printf("[Wanted Event] = %s Oyundan Ayrildi Event Iptal Edildi\n", pUser->GetName().c_str());
		break;

	case 2: // Zoneden Çýktý
		g_pMain->LogosYolla_Zone("Wanted User", string_format("%s Karakteri Zoneden Ayrýldý Wanted Event Sona Erdi!", pUser->GetName().c_str()), 0, 204, 102, ZONE_RONARK_LAND);
		g_pMain->VanGuardSelect = false;
		g_pMain->VanGuardSelectTime = 0;
		g_pMain->VanGuardSelectStatus = false;
		g_pMain->VanGuardTime = 0;
		g_pMain->VanGuardUserName.clear();
		pUser->VanGuardStatus = false;
		g_pMain->SendHelpDescription(pUser, "[Wanted Event] : Zoneden Ayrýldýnýz Eventi Kaybettiniz");
		//printf("[Wanted Event] = %s Zoneden Ayrildi Event Iptal Edildi\n", pUser->GetName().c_str());
		break;

	case 3: // Kazandý
		//printf("[Wanted Event] = %s Eventi Kazandi\n", pUser->GetName().c_str());
		g_pMain->SendHelpDescription(pUser, "[Wanted Event] : Tebrikler Kazandýnýz");
		g_pMain->LogosYolla_Zone("Wanted Event", string_format("%s Karakteri 10 Dakika Boyunca Hayatta Kaldý Eventi Kazandý Tebrikler!", pUser->GetName().c_str()), 0, 204, 102, pUser->GetZoneID());
		g_pMain->VanGuardSelect = false;
		g_pMain->VanGuardSelectTime = 0;
		g_pMain->VanGuardSelectStatus = false;
		g_pMain->VanGuardTime = 0;
		g_pMain->VanGuardUserName.clear();
		pUser->VanGuardStatus = false;

		if (g_pMain->WantedEventSystemWinItem > 1)
			pUser->GiveItem(g_pMain->WantedEventSystemWinItem, g_pMain->WantedEventSystemWinItemCount, true, g_pMain->WantedEventSystemWinItemDays);

		if (g_pMain->WantedEventSystemWinNP > 1)
			pUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNP, false, false, false);

		if (g_pMain->WantedEventSystemWinKC > 1)
			pUser->GiveKnightCash(WantedEventSystemWinKC);

		if (g_pMain->WantedEventSystemWinNationNP > 1)
		{
			SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
			foreach(itr, sessMap)
			{
				CUser* OdulUser = TO_USER(itr->second);

				if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != OdulUser->GetNation() || !OdulUser->isInPKZone())
					continue;

				OdulUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNationNP, false, false, false);
			}
			g_pMain->LogosYolla_Zone("Wanted Event", string_format("%s Karakteri Irkýna %d National Point Kazandýrdý Tebrikler!", pUser->GetName().c_str(), g_pMain->WantedEventSystemWinNationNP), 0, 204, 102, ZONE_RONARK_LAND);
		}


		/*for (int i = 0; i < MAX_USER; i++)
		{

			CUser * OdulUser = GetUserPtr(i);

			if (OdulUser == nullptr || !OdulUser->isInGame() || OdulUser->GetNation() != pUser->GetNation() || !pUser->isInPKZone())
				continue;

			OdulUser->SendLoyaltyChange(g_pMain->WantedEventSystemWinNationNP, false, false, false);
		}
		g_pMain->LogosYolla_Zone("Wanted Event", string_format("%s Karakteri Irkýna %d National Point Kazandýrdý Tebrikler!", pUser->GetName().c_str(), g_pMain->WantedEventSystemWinNationNP), 0, 204, 102, ZONE_RONARK_LAND);
	}*/
		break;

	default:
		break;
	}

	if (pUser)
	{
		Packet result(WIZ_VANGUARD, uint8(0x02));
		result << uint8(0x01) << uint8(0x00) << pUser->GetName();
		g_pMain->Send_Zone(&result, pUser->GetZoneID());
	}

}

void CGameServerDlg::VanGuardSelectUser()
{
	uint16 OnlinePlayer = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);

		if (pUser == nullptr)
			continue;

		if (!pUser->iszonevanguar() || !pUser->isInGame())
			continue;

		OnlinePlayer++;
	}

	/*for (uint16 i = 0; i < MAX_USER; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(i);

		if (pUser == nullptr)
			continue;

		if (!pUser->iszonevanguar() || !pUser->isInGame())
			continue;

		OnlinePlayer++;
	}*/

	uint32 UserRandom = myrand(1, OnlinePlayer);

	uint16 Count2 = 0;

	SessionMap sessMap2 = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap2)
	{
		CUser* pUser = TO_USER(itr->second);

		if (pUser == nullptr)
			continue;

		if (!pUser->iszonevanguar() || !pUser->isInGame())
			continue;

		Count2++;


		if (Count2 == UserRandom)
		{
			Packet result(WIZ_VANGUARD, uint8(0x01));
			result << uint8(0x01) << uint8(0x01);;
			pUser->Send(&result);
			VanGuardSelectTime = 20;
			VanGuardSelect = true;
			break;
		}
	}

	/*for (uint16 i = 0; i < MAX_USER; i++)
	{
		CUser * pUser = g_pMain->GetUserPtr(i);

		if (pUser == nullptr)
			continue;

		if (!pUser->iszonevanguar() || !pUser->isInGame())
			continue;

		Count2++;


		if (Count2 == UserRandom)
		{
			Packet result(WIZ_VANGUARD, uint8(0x01));
			result << uint8(0x01) << uint8(0x01);;
			pUser->Send(&result);
			VanGuardSelectTime = 20;
			VanGuardSelect = true;
			break;
		}

	}*/

}

void CUser::VanGuardZone()
{
	if (VanGuardStatus && !g_pMain->VanGuardUserName.empty())
	{
		if (GetSPosX() > 12990 && GetSPosX() < 14740 && GetSPosZ() > 10200 && GetSPosZ() < 11400 && GetZoneID() == ZONE_RONARK_LAND && g_pMain->VanGuardUserName == GetName())
			ZoneChange(ZONE_RONARK_LAND, 1017.0f, 1000.0f);

		else if (GetSPosX() > 5120 && GetSPosX() < 7170 && GetSPosZ() > 8470 && GetSPosZ() < 9670 && GetZoneID() == ZONE_RONARK_LAND && g_pMain->VanGuardUserName == GetName())
			ZoneChange(ZONE_RONARK_LAND, 1017.0f, 1000.0f);

	}
}