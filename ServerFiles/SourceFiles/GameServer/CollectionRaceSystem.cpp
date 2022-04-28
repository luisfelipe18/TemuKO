#include "stdafx.h"

#pragma region CGameServerDlg::UpdateCollectionRaceInfoTime()
void CGameServerDlg::UpdateCollectionRaceInfoTime()
{
	uint8 Count = 0;
	printf("[CollectionRace Event] = Loading Quests \n");
	foreach_stlmap(itr, g_pMain->CollectionRaceHuntListArray)
	{
		COLLECTIONRACE_HUNT_LIST* COLLECTIONRACE_HUNT_LIST = itr->second;
		Count++;

		CollectionRaceQuestID[Count] = itr->second->QuestID;

		CollectionRaceMonsterNum1[Count] = itr->second->MonsterNum1;
		CollectionRaceMonsterNum2[Count] = itr->second->MonsterNum2;
		CollectionRaceMonsterNum3[Count] = itr->second->MonsterNum3;
		CollectionRaceMonsterNum4[Count] = itr->second->MonsterNum4;

		CollectionRaceMonsterName1[Count] = itr->second->MonsterName1;
		CollectionRaceMonsterName2[Count] = itr->second->MonsterName2;
		CollectionRaceMonsterName3[Count] = itr->second->MonsterName3;
		CollectionRaceMonsterName4[Count] = itr->second->MonsterName4;

		CollectionRaceMonsterKillCount1[Count] = itr->second->MonsterKillCount1;
		CollectionRaceMonsterKillCount2[Count] = itr->second->MonsterKillCount2;
		CollectionRaceMonsterKillCount3[Count] = itr->second->MonsterKillCount3;
		CollectionRaceMonsterKillCount4[Count] = itr->second->MonsterKillCount4;

		CollectionRaceQuestZone[Count] = itr->second->QuestZone;
	}
	CollectionRaceCounts = Count;
	printf("[CollectionRace Event] = %d Loaded Quests \n", CollectionRaceCounts);
}
#pragma endregion

#pragma region CUser::CREventProcess(uint16 MonsterNum)

void CUser::CollectionRaceProcess(uint16 MonsterNum)
{
	/*if (!CollectionRaceRegister)
		return;*/

	if (MonsterNum != CollectionRaceEventMonsterID[0]
		&& MonsterNum != CollectionRaceEventMonsterID[1]
		&& MonsterNum != CollectionRaceEventMonsterID[2]
		&& MonsterNum != CollectionRaceEventMonsterID[3])
		return;

	if (MonsterNum == CollectionRaceEventMonsterID[0] && CollectionRaceEventMonsterKillCount[0] != CollectionRaceEventCount[0] && GetZoneID() == CollectionRaceEventZone[0])
		CollectionRaceEventMonsterKillCount[0]++;
	else if (MonsterNum == CollectionRaceEventMonsterID[1]	&& CollectionRaceEventMonsterKillCount[1] != CollectionRaceEventCount[1] && GetZoneID() == CollectionRaceEventZone[1])
		CollectionRaceEventMonsterKillCount[1]++;
	else if (MonsterNum == CollectionRaceEventMonsterID[2]	&& CollectionRaceEventMonsterKillCount[2] != CollectionRaceEventCount[2] && GetZoneID() == CollectionRaceEventZone[2])
		CollectionRaceEventMonsterKillCount[2]++;
	else if (MonsterNum == CollectionRaceEventMonsterID[3]	&& CollectionRaceEventMonsterKillCount[3] != CollectionRaceEventCount[3] && GetZoneID() == CollectionRaceEventZone[3])
		CollectionRaceEventMonsterKillCount[3]++;

	if (CollectionRaceEventMonsterKillCount[0] == CollectionRaceEventCount[0]
		&& CollectionRaceEventMonsterKillCount[1] == CollectionRaceEventCount[1]
		&& CollectionRaceEventMonsterKillCount[2] == CollectionRaceEventCount[2]
		&& CollectionRaceEventMonsterKillCount[3] == CollectionRaceEventCount[3])
		CollectionRaceReward();

	else if (CollectionRaceEventMonsterKillCount[0] == CollectionRaceEventCount[0] && MonsterNum == CollectionRaceEventMonsterID[0])
		g_pMain->SendHelpDescription(this, string_format("[Cr Event] : %s Quest Completed", CollectionRaceEventMonsterName[0].c_str()));
	else if (CollectionRaceEventMonsterKillCount[1] == CollectionRaceEventCount[1] && MonsterNum == CollectionRaceEventMonsterID[1])
		g_pMain->SendHelpDescription(this, string_format("[Cr Event] : %s Quest Completed", CollectionRaceEventMonsterName[1].c_str()));
	else if (CollectionRaceEventMonsterKillCount[2] == CollectionRaceEventCount[2] && MonsterNum == CollectionRaceEventMonsterID[2])
		g_pMain->SendHelpDescription(this, string_format("[Cr Event] : %s Quest Completed", CollectionRaceEventMonsterName[2].c_str()));
	else if (CollectionRaceEventMonsterKillCount[3] == CollectionRaceEventCount[3] && MonsterNum == CollectionRaceEventMonsterID[3])
		g_pMain->SendHelpDescription(this, string_format("[Cr Event] : %s Quest Completed", CollectionRaceEventMonsterName[3].c_str()));

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CR));
	result << uint8(2) << uint16(MonsterNum);
	Send(&result);
}

#pragma endregion

#pragma region CGameServerDlg::CollectionRaceLoadProcess()
void CUser::CollectionRaceLoadProcess()
{
	//CollectionRaceRegister = true;

	CollectionRaceEventCount[0] = g_pMain->CollectionRaceMonsterKillCount1[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterName[0] = g_pMain->CollectionRaceMonsterName1[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterID[0] = g_pMain->CollectionRaceMonsterNum1[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventZone[0] = g_pMain->CollectionRaceQuestZone[g_pMain->CollectionRaceSelectedMonster];

	CollectionRaceEventCount[1] = g_pMain->CollectionRaceMonsterKillCount2[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterName[1] = g_pMain->CollectionRaceMonsterName2[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterID[1] = g_pMain->CollectionRaceMonsterNum2[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventZone[1] = g_pMain->CollectionRaceQuestZone[g_pMain->CollectionRaceSelectedMonster];

	CollectionRaceEventCount[2] = g_pMain->CollectionRaceMonsterKillCount3[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterName[2] = g_pMain->CollectionRaceMonsterName3[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterID[2] = g_pMain->CollectionRaceMonsterNum3[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventZone[2] = g_pMain->CollectionRaceQuestZone[g_pMain->CollectionRaceSelectedMonster];

	CollectionRaceEventCount[3] = g_pMain->CollectionRaceMonsterKillCount4[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterName[3] = g_pMain->CollectionRaceMonsterName4[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventMonsterID[3] = g_pMain->CollectionRaceMonsterNum4[g_pMain->CollectionRaceSelectedMonster];
	CollectionRaceEventZone[3] = g_pMain->CollectionRaceQuestZone[g_pMain->CollectionRaceSelectedMonster];

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CR));
	result << uint8(0);

	for (int i = 0; i < 4; i++)
		result << CollectionRaceEventMonsterID[i] 
		<< CollectionRaceEventCount[i] 
		<< CollectionRaceEventMonsterKillCount[i];

	for (int i = 0; i < 2; i++)
		result << g_pMain->CollectionRaceSetting->CREventItemID[i] 
		<< g_pMain->CollectionRaceSetting->CREventItemCount[i];

	result << g_pMain->CollectionRaceSetting->CREventWinCoin 
		<< g_pMain->CollectionRaceSetting->CREventWinEXP 
		<< g_pMain->CollectionRaceSetting->CREventWinNP 
		<< g_pMain->CollectionRaceSetting->CREventWinKC 
		<< uint32(g_pMain->CollectionRaceFinishTime);

	Send(&result);
}
#pragma endregion

#pragma region CGameServerDlg::CollectionRaceEndProcess()
void CUser::CollectionRaceEndProcess() 
{
	memset(&CollectionRaceEventMonsterID, 0, sizeof(CollectionRaceEventMonsterID));
	memset(&CollectionRaceEventMonsterKillCount, 0, sizeof(CollectionRaceEventMonsterKillCount));
	memset(&CollectionRaceEventCount, 0, sizeof(CollectionRaceEventCount));
	memset(&CollectionRaceEventZone, 0, sizeof(CollectionRaceEventZone));

	for (int i = 0; i < 4; i++)
		CollectionRaceEventMonsterName[i].clear();

	//CollectionRaceRegister = false;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CR));
	result << uint8(1);
	Send(&result);
}
#pragma endregion

#pragma region CGameServerDlg::CollectionRaceReward()
void CUser::CollectionRaceReward()
{
	/*if (!CollectionRaceRegister)
		return;*/

	uint8 rewardCount = 0;
	std::map<uint32, uint32> rewardMap;

	if (g_pMain->CollectionRaceSetting->CREventWinEXP > 1){
		ExpChange(g_pMain->CollectionRaceSetting->CREventWinEXP);
		rewardMap.insert(std::make_pair(ITEM_EXP + (rewardCount++), g_pMain->CollectionRaceSetting->CREventWinEXP));
	}

	if (g_pMain->CollectionRaceSetting->CREventWinCoin > 1){
		GoldGain(g_pMain->CollectionRaceSetting->CREventWinCoin);
		rewardMap.insert(std::make_pair(ITEM_GOLD + (rewardCount++), g_pMain->CollectionRaceSetting->CREventWinCoin));
	}

	if (g_pMain->CollectionRaceSetting->CREventWinNP > 1){
		SendLoyaltyChange(g_pMain->CollectionRaceSetting->CREventWinNP, false, false, false);
		rewardMap.insert(std::make_pair(ITEM_COUNT + (rewardCount++), g_pMain->CollectionRaceSetting->CREventWinNP));
	}

	if (g_pMain->CollectionRaceSetting->CREventWinKC > 1){
		SendKnightCash(g_pMain->CollectionRaceSetting->CREventWinKC);
	}

	for (int i = 0; i < 2; i++){
		if (g_pMain->CollectionRaceSetting->CREventItemID[i] > 1 && g_pMain->CollectionRaceSetting->CREventItemCount[i] > 0 && g_pMain->CollectionRaceSetting->CREventItemTimed[i] > 0){
			GiveItem(g_pMain->CollectionRaceSetting->CREventItemID[i], g_pMain->CollectionRaceSetting->CREventItemCount[i], true, g_pMain->CollectionRaceSetting->CREventItemTimed[i]);
			rewardMap.insert(std::make_pair(g_pMain->CollectionRaceSetting->CREventItemID[i] + (rewardCount++), g_pMain->CollectionRaceSetting->CREventItemCount[i]));
		}
		else if (g_pMain->CollectionRaceSetting->CREventItemID[i] > 0 && g_pMain->CollectionRaceSetting->CREventItemCount[i] > 0){
			GiveItem(g_pMain->CollectionRaceSetting->CREventItemID[i], g_pMain->CollectionRaceSetting->CREventItemCount[i]);
			rewardMap.insert(std::make_pair(g_pMain->CollectionRaceSetting->CREventItemID[i] + (rewardCount++), g_pMain->CollectionRaceSetting->CREventItemCount[i]));
		}
	}

	Packet result(WIZ_QUEST);
	result << uint8(10);

	foreach(itr, rewardMap)
		result << itr->first << itr->second;

	result << uint32(0);
	Send(&result);

	memset(&CollectionRaceEventMonsterID, 0, sizeof(CollectionRaceEventMonsterID));
	memset(&CollectionRaceEventMonsterKillCount, 0, sizeof(CollectionRaceEventMonsterKillCount));
	memset(&CollectionRaceEventCount, 0, sizeof(CollectionRaceEventCount));
	memset(&CollectionRaceEventZone, 0, sizeof(CollectionRaceEventZone));

	for (int i = 0; i < 4; i++)
		CollectionRaceEventMonsterName[i].clear();

	//CollectionRaceRegister = false;

	result.clear();
	result.Initialize(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_CR) << uint8(1);
	Send(&result);

	g_pMain->SendHelpDescription(this, string_format("[Cr Event] : Tebrikler görev tamamlandý"));
}
#pragma endregion

#pragma region CUser::HandleCollectionInfoMessage
COMMAND_HANDLER(CUser::HandleCollectionInfoMessage)
{
	/*if (!CollectionRaceRegister)
	{
		g_pMain->SendHelpDescription(this, string_format("[Collection Race] : Evente kayýtlý deðilsin"));
		return true;
	}*/

	g_pMain->SendHelpDescription(this, string_format("[Collection Race] : Event  %d  dakika sonra bitiyor", g_pMain->CollectionRaceFinishTime / 60));
	g_pMain->SendHelpDescription(this, string_format("[Collection Race] : %s : %d/%d | %s : %d/%d | %s : %d/%d | %s : %d/%d",
		CollectionRaceEventMonsterName[0].c_str(), CollectionRaceEventMonsterKillCount[0], CollectionRaceEventCount[0],
		CollectionRaceEventMonsterName[1].c_str(), CollectionRaceEventMonsterKillCount[1], CollectionRaceEventCount[1],
		CollectionRaceEventMonsterName[2].c_str(), CollectionRaceEventMonsterKillCount[2], CollectionRaceEventCount[2],
		CollectionRaceEventMonsterName[3].c_str(), CollectionRaceEventMonsterKillCount[3], CollectionRaceEventCount[3]));
	return true;
}
#pragma endregion

#pragma region CUser::HandleCollectionOpen
COMMAND_HANDLER(CUser::HandleCollectionOpen)
{
	if (!isGM())
		return true;

	if (g_pMain->CollectionRaceGameServerSatus)
	{
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Event zaten aktif : %d dakika sonra bitecek. Eventi hemen bitirmek isterseniz +endcr yazmanýz yeterli.", g_pMain->CollectionRaceFinishTime / 60));
		return true;
	}

	if (vargs.size() < 3)
	{
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : +CollectionOpen <dakika> <hedef id> <ödül id>"));
		return true;
	}

	int m_sTime = atoi(vargs.front().c_str());
	if (m_sTime < 10 
		|| m_sTime > 500)
	{
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Süre  10 ile 500 dakika arasýnda olmalý."));
		return true;
	}

	vargs.pop_front();
	uint32 m_sSelectMonster, m_sSelectReward;
	m_sSelectMonster = atoi(vargs.front().c_str());
	vargs.pop_front();
	m_sSelectReward = atoi(vargs.front().c_str());

	g_pMain->CollectionRaceSetting = g_pMain->CollectionRaceSettingsArray.GetData(m_sSelectReward);
	g_pMain->CollectionRaceHuntList = g_pMain->CollectionRaceHuntListArray.GetData(m_sSelectMonster);
	g_pMain->CollectionRaceSelectedMonster = m_sSelectMonster;

	if (g_pMain->CollectionRaceSetting == nullptr) {
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Ödül verisi bulunamadý."));
		return true;
	}
	if (g_pMain->CollectionRaceCounts < g_pMain->CollectionRaceSelectedMonster)
	{
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Görev hedefi verisi bulunamadý."));
		return true;
	}
	if (g_pMain->CollectionRaceHuntList == nullptr) {
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Hunt verisi bulunamadý."));
		return true;
	}

	g_pMain->CollectionRaceFinishTime = m_sTime * 60;

	uint16 m_sMaxCount = 0;

	for (int i = 0; i < MAX_USER; i++)
	{
		CUser* pUser = g_pMain->GetUserPtr(i);

		if (pUser == nullptr)
			continue;

		if (pUser->GetLevel() < g_pMain->CollectionRaceSetting->CREventMinLevel)
			continue;

		//if (pUser->GetZoneID() != g_pMain->m_CollectionRaceHuntList->QuestZone)
		//	continue;

		if (m_sMaxCount == g_pMain->CollectionRaceSetting->CREventMaxUser)
			continue;

		pUser->CollectionRaceLoadProcess(); m_sMaxCount++;
	}

	std::string m_CollectionRaceZone;

	if (g_pMain->CollectionRaceHuntList->QuestZone == 21)
		m_CollectionRaceZone = "Maradon";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 71)
		m_CollectionRaceZone = "Ronark Land";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 72)
		m_CollectionRaceZone = "Ardream";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 73)
		m_CollectionRaceZone = "Ronark Land Base";

	g_pMain->CollectionRaceGameServerSatus = true;

	std::string sNoticeMessage = string_format("%s CR ( Tekrarlanabilir ) Event Baþladý | Ödül : %dx %s, %dx %s, %d EXP, %d Coin, %d NP, %d KC Kazan.",
		m_CollectionRaceZone.c_str(),
		g_pMain->CollectionRaceSetting->CREventItemCount[0], g_pMain->CollectionRaceSetting->CREventstrName[0].c_str(),
		g_pMain->CollectionRaceSetting->CREventItemCount[1], g_pMain->CollectionRaceSetting->CREventstrName[1].c_str(),
		g_pMain->CollectionRaceSetting->CREventWinEXP,
		g_pMain->CollectionRaceSetting->CREventWinCoin,
		g_pMain->CollectionRaceSetting->CREventWinNP,
		g_pMain->CollectionRaceSetting->CREventWinKC);

	if (!sNoticeMessage.empty())
	{
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
		g_pMain->SendAnnouncement(sNoticeMessage.c_str(), Nation::ALL);
	}

	/*SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);
		if (pUser == nullptr)
			continue;

		//if (pUser->CollectionRaceRegister)
		//	continue;

		if (pUser->GetLevel() < g_pMain->CollectionRaceSetting->CREventMinLevel)
			continue;

		if (pUser->GetZoneID() != g_pMain->CollectionRaceHuntList->QuestZone)
			continue;

		if (m_sMaxCount == g_pMain->CollectionRaceSetting->CREventMaxUser)
			continue;

		pUser->CollectionRaceLoadProcess(); m_sMaxCount++;
	}

	if (m_sMaxCount > 0)
	{
		g_pMain->CollectionRaceGameServerSatus = true;
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Event baþladý! | Katýlýmcý : %d | Süre : %d", m_sMaxCount, g_pMain->CollectionRaceFinishTime / 60));
	}
	else
	{
		g_pMain->CollectionRaceFinishTime = 0;
		g_pMain->CollectionRaceGameServerSatus = false;
		g_pMain->SendHelpDescription(this, string_format("[CR EVENT] : Event Baþlatýlamadý.! | Katýlýmcý : %d | Süre : %d", m_sMaxCount, g_pMain->CollectionRaceFinishTime / 60));
	}*/


	return true;
}
#pragma endregion

#pragma region CUser::CollectýonClose
COMMAND_HANDLER(CUser::HandleCollectionClose)
{
	if (!isGM())
		return false;

	g_pMain->CollectionRaceFinishTime = 0;
	g_pMain->CollectionRaceGameServerSatus = false;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach(itr, sessMap)
	{
		CUser* pUser = TO_USER(itr->second);

		if (pUser == nullptr)
			continue;

		pUser->CollectionRaceEndProcess();
	}
	return true;
}
#pragma endregion

#pragma region CGameServerDlg::CollectionRaceFinish()
void CGameServerDlg::CollectionRaceFinish()
{
	if (!CollectionRaceGameServerSatus)
		return;

	std::string CollectionRaceZone;

	if (g_pMain->CollectionRaceHuntList->QuestZone == 21)
		CollectionRaceZone = "Maradon";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 71)
		CollectionRaceZone = "Ronark Land";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 72)
		CollectionRaceZone = "Ardream";
	else if (g_pMain->CollectionRaceHuntList->QuestZone == 73)
		CollectionRaceZone = "Ronark Land Base";

	g_pMain->CollectionRaceFinishTime--;

	if (g_pMain->CollectionRaceFinishTime == 3540 // 59dk
		|| g_pMain->CollectionRaceFinishTime == 1800// 30dk
		|| g_pMain->CollectionRaceFinishTime == 1500// 25dk
		|| g_pMain->CollectionRaceFinishTime == 1200// 20dk
		|| g_pMain->CollectionRaceFinishTime == 900// 15dk
		|| g_pMain->CollectionRaceFinishTime == 600// 10dk
		|| g_pMain->CollectionRaceFinishTime == 300// 5dk
		|| g_pMain->CollectionRaceFinishTime == 180// 3dk
		|| g_pMain->CollectionRaceFinishTime == 60)// 1dk
	{

		std::string sNoticeMessage = string_format("[%s CR ( Tekrarlanabilir )] CR Event devam ediyor! Son %d dakika!", CollectionRaceZone.c_str(), CollectionRaceFinishTime / 60);

		std::string sNoticeMessage2 = string_format("[%s CR ( Repeatable )] CR Event continuing! Last %d minutes!", CollectionRaceZone.c_str(), CollectionRaceFinishTime / 60);

		if (!sNoticeMessage.empty() || !sNoticeMessage2.empty())
		{
			g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);
			g_pMain->SendAnnouncement(sNoticeMessage.c_str(), Nation::ALL);

			g_pMain->SendNotice(sNoticeMessage2.c_str(), Nation::ALL);
			g_pMain->SendAnnouncement(sNoticeMessage2.c_str(), Nation::ALL);
		}
	}

	if (g_pMain->CollectionRaceFinishTime == 0)
	{
		std::string sNoticeMessage3 = string_format("%s CR ( Tekrarlanabilir ) Bomba CR Görevi Sona Erdi! Görevi tamamlayan oyuncularýmýz, ödüllerini Inventory'de bulabilir.", CollectionRaceZone.c_str());
		if (!sNoticeMessage3.empty())
		{
			g_pMain->SendNotice(sNoticeMessage3.c_str(), Nation::ALL);
			g_pMain->SendAnnouncement(sNoticeMessage3.c_str(), Nation::ALL);
		}
		g_pMain->CollectionRaceGameServerSatus = false;
		g_pMain->CollectionRaceFinishTime = 0;

		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach(itr, sessMap)
		{
			CUser* pUser = TO_USER(itr->second);

			if (pUser == nullptr)
				continue;

			/*if (pUser == nullptr
				|| !pUser->CollectionRaceRegister)
				continue;*/

			pUser->CollectionRaceEndProcess();
		}
	}
}
#pragma endregion