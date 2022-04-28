#include "stdafx.h"
#include "User.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

void CGameServerDlg::ArdreamEventOpen()
{
	g_pMain->OpenArdream = true;
	g_pMain->OpenCZ = false;
	KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_ARDREAM);
	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);
	DateTime time;

	/*Açýlýnca Verilecek Notice.*/
	g_pMain->LogosYolla("[Ardream Event]", string_format("Event'e giriþler açýldý, Maradon Gate'den giriþ yapabilirsiniz"), 252, 207, 25);
	SendAnnouncementWhite("The Event of Ardream is Opened..!"); // Yukardan Notice
	printf("Ardream Event is started at %02d:%02d \n", time.GetHour(), time.GetMinute());
}

void CGameServerDlg::ArdreamEventZoneClose()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSeconds = g_localTime.tm_sec;

	if (MaxLevel62Control == 0 && g_pMain->OpenArdream)
	{
		g_pMain->KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
		g_pMain->OpenArdream = false;
		g_pMain->OpenCZ = true;

		ArdreamEventGameServerSatus = false;
		ArdreamEventFinishTime = 0;

		g_pMain->LogosYolla("[Ardream Event]", string_format("Event Sona Erdi."), 132, 112, 255);
		printf("Ardream Event is Close %02d:%02d:%02d \n", nHour, nMinute, nSeconds);
	}
}

COMMAND_HANDLER(CUser::HandleOpenArdreamCommand)
{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarýný Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		return false;
		Disconnect();
	}

	if (g_pMain->ArdreamEventGameServerSatus)
	{
		g_pMain->SendHelpDescription(this, string_format("Ardream Event: Event Zaten Aktif Lutfen : %d Dakika Bekleyiniz", g_pMain->ArdreamEventFinishTime / 60));
		return false;
	}

	if (vargs.empty())
	{
		g_pMain->SendHelpDescription(this, "+opena Sure(Örnek : 30 = 30 Dakika)");
		return true;
	}

	int Sure = atoi(vargs.front().c_str());

	if (Sure < 10 || Sure > 500)
	{
		g_pMain->SendHelpDescription(this, "Ardream Event: Sure En Az 10 En Fazla 500 Dakika Olabilir");
		return true;
	}

	g_pMain->ArdreamEventFinishTime = Sure * 60;
	g_pMain->ArdreamEventGameServerSatus = true;
	g_pMain->ArdreamEventOpen();
	g_pMain->SendFormattedNotice("%s, Ardream Event Aktif Edildi | Event Suresi : %d Dakika", Nation::ALL, GetName().c_str(), g_pMain->ArdreamEventFinishTime / 60);
	//g_pMain->SendYesilNotice(this, string_format("Base Land Event: Event Aktif Edildi | Event Suresi : %d Dakika", g_pMain->BaseLandEventFinishTime));
	return true;
}

COMMAND_HANDLER(CUser::HandleCloseArdreamEventCommand)
{
	if (!isGM())
		return false;

	g_pMain->ArdreamEventZoneClose();
	return true;
}
