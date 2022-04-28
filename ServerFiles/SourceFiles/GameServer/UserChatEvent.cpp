#include "stdafx.h"

using std::string;

void CGameServerDlg::UserChatEvent()
{
	// Check.
	if (UserChatEventOn == true)
	{
		//printf("[Chat Event] : Event daha önceden aktif edilmiþ...\n");
		return;
	}

	// Event Start!
	if (UserChatEventOn == false)
	{
		UserChatEventOn = true;
	}

	static const string RandomKeyList = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand((int)time(NULL));
	string RandomKey = "";

	for (int i = 0; i < 10; i++) {
		RandomKey += RandomKeyList[rand() % RandomKeyList.size()];
	}

	UserChatEventKey = RandomKey.c_str();
	//printf("[Chat Event] : Random Key Degeri = %s\n", UserChatEventKey.c_str());
	std::string SendChatEventMessage = string_format("Deðerli Oyuncular, Chat Event Baþlamýþtýr,Ödül %d Knight Cash Point. Yazýlacak Kelime: %s , Ýyi Þanslar!.", m_ChatEventCashPoint, UserChatEventKey.c_str());

	g_pMain->SendAnnouncementWhite(SendChatEventMessage.c_str(), Nation::ALL);

	g_pMain->SendNotice(SendChatEventMessage.c_str(), Nation::ALL);

	g_pMain->SendAnnouncement(SendChatEventMessage.c_str(), Nation::ALL);

	//printf("[Chat Event] : System Started.\n");
}