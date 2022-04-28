#include "stdafx.h"
#include "User.h"

void CGameServerDlg::CZEventOpen()
{
	DateTime time;
	g_pMain->OpenCZ = true;
	g_pMain->OpenArdream = false;
	KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);

	/*Açýlýnca Verilecek Notice.*/
	g_pMain->LogosYolla("[CZ Event]", string_format("CZ Event'e giriþler açýldý , Maradon Gate'den giriþ yapabilirsiniz."), 252, 207, 25);
	SendAnnouncementWhite("The Event of CZ is Opened..!"); // Yukardan Notice
	printf("CZ Event is started at %02d:%02d \n", time.GetHour(), time.GetMinute());

}

void CGameServerDlg::CZEventZoneClose()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSeconds = g_localTime.tm_sec;

	if (g_pMain->MaxLevel62Control == 0 && g_pMain->OpenCZ)
	{
		g_pMain->KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_ARDREAM);
		g_pMain->OpenCZ = false;
		g_pMain->OpenArdream = true;

		ArdreamEventGameServerSatus = false;
		ArdreamEventFinishTime = 0;

		g_pMain->LogosYolla("[CZ Event]", string_format("CZ Event bitmistir, katýlan oyuncularýmýza ödülleri gönderilmistir."), 132, 112, 255);
		printf("CZ Event is Close %02d:%02d:%02d \n", nHour, nMinute, nSeconds);

	}
}

COMMAND_HANDLER(CUser::HandleOpenCZCommand)
{
	if (!isGM())
	{
		//g_pMain->HackLogs("GM_COMMAND", string_format("%s Gm Komutlarýný Deniyor Lütfen Dikkat Ediniz", GetName().c_str()));
		return false;
		Disconnect();
	}

	if (g_pMain->CzEventGameServerSatus)
	{
		g_pMain->SendHelpDescription(this, string_format("CZ Event: Event Zaten Aktif Lutfen : %d Dakika Bekleyiniz", g_pMain->CzEventFinishTime / 60));
		return false;
	}

	if (vargs.empty())
	{
		g_pMain->SendHelpDescription(this, "+opencz Sure(Örnek : 30 = 30 Dakika)");
		return true;
	}

	int Sure = atoi(vargs.front().c_str());

	if (Sure < 10 || Sure > 500)
	{
		g_pMain->SendHelpDescription(this, "CZ Event: Sure En Az 10 En Fazla 500 Dakika Olabilir");
		return true;
	}

	g_pMain->CzEventGameServerSatus = true;
	g_pMain->CzEventFinishTime = Sure * 60;
	g_pMain->CZEventOpen();
	g_pMain->SendFormattedNotice("%s, CZ Event Aktif Edildi | Event Suresi : %d Dakika", Nation::ALL, GetName().c_str(), g_pMain->CzEventFinishTime / 60);
	//g_pMain->SendYesilNotice(this, string_format("Base Land Event: Event Aktif Edildi | Event Suresi : %d Dakika", g_pMain->BaseLandEventFinishTime));
	return true;
}

COMMAND_HANDLER(CUser::HandleCloseCZEventCommand)
{
	if (!isGM())
		return false;

	g_pMain->CZEventZoneClose();
	return true;
}