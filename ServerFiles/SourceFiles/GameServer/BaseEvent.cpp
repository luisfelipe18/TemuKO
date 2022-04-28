#include "stdafx.h"
#include "User.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

void CGameServerDlg::BaseLandEventOpen()
{
	g_pMain->OpenBaseLand = true;
	g_pMain->OpenCZ = false;
	KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_RONARK_LAND_BASE);
	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);
	DateTime time;

	/*Açýlýnca Verilecek Notice.*/
	g_pMain->LogosYolla("[Base Land Event]", string_format("Base Land'a giriþler açýldý, Maradon Gate'den giriþ yapabilirsiniz"), 252, 207, 25);
	SendAnnouncementWhite("The Event of Base Land is Opened..!"); // Yukardan Notice
	printf("Base Land Event is started at %02d:%02d \n", time.GetHour(), time.GetMinute());
}

void CGameServerDlg::BaseLandFinished()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSeconds = g_localTime.tm_sec;

	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);

	g_pMain->OpenBaseLand = false;
	g_pMain->OpenCZ = true;

	BaseLandEventGameServerSatus = false;
	BaseLandEventFinishTime = 0;

	g_pMain->LogosYolla("[Base Land Event]", string_format("Event Sona Erdi."), 132, 112, 255);
	printf("Base Land Event is Close %02d:%02d:%02d \n", nHour, nMinute, nSeconds);
}

COMMAND_HANDLER(CUser::HandleBaseEvent)

{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarýný Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		return false;
		Disconnect();
	}

	if (g_pMain->BaseLandEventGameServerSatus)
	{
		g_pMain->SendHelpDescription(this, string_format("Base Land Event: Event Zaten Aktif Lutfen : %d Dakika Bekleyiniz", g_pMain->BaseLandEventFinishTime / 60));
		return false;
	}

	if (vargs.empty())
	{
		g_pMain->SendHelpDescription(this, "+openbl Sure(Örnek : 30 = 30 Dakika)");
		return true;
	}

	int Sure = atoi(vargs.front().c_str());

	if (Sure < 10 || Sure > 500)
	{
		g_pMain->SendHelpDescription(this, "Base Land Event: Sure En Az 10 En Fazla 500 Dakika Olabilir");
		return true;
	}

	g_pMain->BaseLandEventGameServerSatus = true;
	g_pMain->BaseLandEventFinishTime = Sure * 60;
	g_pMain->BaseLandEventOpen();
	g_pMain->SendFormattedNotice("%s, Base Land Event Aktif Edildi | Event Suresi : %d Dakika", Nation::ALL, GetName().c_str(), g_pMain->BaseLandEventFinishTime / 60);
	//g_pMain->SendYesilNotice(this, string_format("Base Land Event: Event Aktif Edildi | Event Suresi : %d Dakika", g_pMain->BaseLandEventFinishTime));
	return true;
}

COMMAND_HANDLER(CUser::HandleBaseLandCloseCommand)
{
	if (!isGM())
		return false;

	g_pMain->BaseLandFinished();
	return true;
}