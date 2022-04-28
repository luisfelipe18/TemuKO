#include "stdafx.h"

using std::string;

void CGameServerDlg::UserChatEvent()
{
	// Check.
	if (UserChatEventOn == true)
	{
		//printf("[Chat Event] : Event daha �nceden aktif edilmi�...\n");
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
	std::string SendChatEventMessage = string_format("De�erli Oyuncular, Chat Event Ba�lam��t�r,�d�l %d Knight Cash Point. Yaz�lacak Kelime: %s , �yi �anslar!.", m_ChatEventCashPoint, UserChatEventKey.c_str());

	g_pMain->SendAnnouncementWhite(SendChatEventMessage.c_str(), Nation::ALL);

	g_pMain->SendNotice(SendChatEventMessage.c_str(), Nation::ALL);

	g_pMain->SendAnnouncement(SendChatEventMessage.c_str(), Nation::ALL);

	//printf("[Chat Event] : System Started.\n");
}