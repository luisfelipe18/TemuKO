#include "stdafx.h"
#include "DBAgent.h"
#include "md5.h"

using namespace std;
#define HOOK_ACTIVE 1
#define HOOK_VERSION	2
#define HOOK_ALIVE_TIMEOUT 3600
#define HOOK_SUPPORT_CHEK 10

struct _TempsItems
{
	uint32 nItemID;
	uint32 time;
	uint8 pos;
};

uint32 s_SwiftSkills[] = { 208002 , 108010 , 207010 , 208010 , 500265 , 107725 , 108725 , 207725 , 208725 , 490230 , 490336, 101002, 102002, 105002, 106002, 107002, 108002, 201002 , 202002, 205002, 206002, 207002, 208002, 490223, 490334, 500316 };

uint32 s_HpSkills[] = { 111606, 491009, 500102, 491011 , 500103 , 500304 , 500315 , 500054 , 112606 , 211606 , 212606 , 111615 , 112615 , 211615 , 212615 , 111624 , 112624 , 211624 , 212624 , 111633 , 112633 , 211633 , 212633 , 111642 , 112642 , 211642 , 212642 , 111654 , 112654 , 211654 , 212654 , 500054 , 500354 , 111655 , 112655 , 211655 , 212655 , 111656 , 112656 , 211656 , 212656 , 111657 , 112657 , 211657 , 212657 , 112670 , 212670 , 112672 , 212672 , 112675 , 212675 };

uint32 s_AcSkills[] = { 491006, 491007, 500029, 500030, 500055, 500056, 111603, 112603, 211603, 212603, 111612, 112612, 211612, 212612, 111621, 112621, 211621, 212621, 111630, 112630, 211630, 212630, 111639, 112639, 211639, 212639,	111651 , 112651 , 211651 , 212651 , 111660 , 112660 , 211660 , 212660 , 112673 , 212673 , 112674 , 212674 };

uint32 s_ResisSkills[] = { 111609, 112609, 211609, 212609, 111627, 112627, 211627, 212627, 111636, 112636, 211636, 212636, 111645, 112645, 211645, 212645 };

uint32 s_ResSkills[] = { 111503, 112503, 211503, 212503, 111512, 112512, 211512, 212512, 111521, 112521, 211521, 212521, 111530, 112530, 211530, 212530, 111539, 112539, 211539 , 212539 , 111548 , 112548 , 211548 , 212548 , 112570 , 212570 , 112575 , 212575 , 112580 , 212580 };

#pragma region CUser::Handle(Packet & pkt)
/**
* @brief	Handles the  Guard packets coming from the client and
*			responds respectively.
*
* @param	pkt	The packet to be handled.
*/
void CUser::Handle(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();
	// If we're not authed yet, forced us to before we can do anything else.
	// NOTE: We're checking the account ID store here because it's only set on successful auth,
	// at which time the other account ID will be cleared out (yes, it's messy -- need to clean it up).
	if (m_strAccountID.empty())
	{
		switch (command)
		{
		case WIZ_HOOK_ALIVE:
		case WIZ_HOOK_COM:
			break;
		default:
			printf("[SID=%d] Hook System packet (%X) prior to account login\n", GetSocketID(), command);
			break;
		}
	}
	else if (!m_bSelectedCharacter)
	{
		switch (command)
		{
		case WIZ_HOOK_COM:
			HOOK_General(pkt);
			break;
		case WIZ_HOOK_ALIVE:
			HOOK_StayAlive(pkt);
			break;
		default:
			printf("[SID=%d] Hook System packet (%X) prior to selecting character\n", GetSocketID(), command);
			break;
		}
	}

	if (!isInGame())
	{
		switch (command)
		{
		case WIZ_HOOK_COM:
			HOOK_General(pkt);
			break;
		case WIZ_HOOK_ALIVE:
			HOOK_StayAlive(pkt);
			break;
		case WIZ_HOOK_AUTHINFO:
			HOOK_AuthInfo(pkt);
			break;
		case WIZ_HOOK_PROCINFO:
			//WIZ_HOOK_ProcInfo(pkt);
			break;
		case WIZ_HOOK_LOG:
			HOOK_LogHandler(pkt);
			break;
		case WIZ_HOOK_UIINFO:
			HOOK_UIRequest(pkt);
			break;
		case WIZ_HOOK_PUS:
			HOOK_PowerUpStoreRequest(pkt);
			break;
		case WIZ_HOOK_RESET:
			HOOK_AllSkillStatRestard(pkt);
			break;
		case WIZ_HOOK_DROP_REQUEST:
			HOOK_DropRequest(pkt);
			break;
		case WIZ_HOOK_CLANBANK:
			//WIZ_HOOK_ClanBank(pkt);
			break;
		case WIZ_HOOK_USERINFO:
			HOOK_ReqUserInfo(pkt);
			break;
		case WIZ_HOOK_LOOT_SETTINS:
			//WIZ_HOOK_SaveLootSettings(pkt);
			break;
		case WIZ_HOOK_CHAOTIC_EXCHANGE:
			HOOK_ChaoticExchange(pkt);
			break;
		case WIZ_HOOK_MERCHANT:
			HOOK_MerchantHandler(pkt);
			break;
		case WIZ_HOOK_USERDATA:
			HOOK_ReqUserData(pkt);
			break;
		case WIZ_HOOK_TEMPITEMS:
			HOOK_SendTempItems(pkt);
			break;
		case WIZ_HOOK_CR:
			break;
		case WIZ_HOOK_SUPPORT:
			HOOK_Support(pkt);
			break;
		case WIZ_HOOK_MERCHANTLIST:
			HOOK_ReqMerchantList(pkt);
			break;
		case WIZ_HOOK_LIFESKILL:
			HOOK_HandleLifeSkill(pkt);
			break;
		default:
			printf("[SID=%d] Hook System Unknown packet !isInGame() %X\n", GetSocketID(), command);
			break;
		}
	}
	else if (isInGame())
	{
		switch (command)
		{
		case WIZ_HOOK_COM:
			HOOK_General(pkt);
			break;
		case WIZ_HOOK_ALIVE:
			HOOK_StayAlive(pkt);
			break;
		case WIZ_HOOK_AUTHINFO:
			HOOK_AuthInfo(pkt);
			break;
		case WIZ_HOOK_PROCINFO:
			//WIZ_HOOK_ProcInfo(pkt);
			break;
		case WIZ_HOOK_LOG:
			HOOK_LogHandler(pkt);
			break;
		case WIZ_HOOK_UIINFO:
			HOOK_UIRequest(pkt);
			break;
		case WIZ_HOOK_PUS:
			HOOK_PowerUpStoreRequest(pkt);
			break;
		case WIZ_HOOK_RESET:
			HOOK_AllSkillStatRestard(pkt);
			break;
		case WIZ_HOOK_DROP_REQUEST:
			HOOK_DropRequest(pkt);
			break;
		case WIZ_HOOK_CLANBANK:
			//WIZ_HOOK_ClanBank(pkt);
			break;
		case WIZ_HOOK_USERINFO:
			HOOK_ReqUserInfo(pkt);
			break;
		case WIZ_HOOK_LOOT_SETTINS:
			//WIZ_HOOK_SaveLootSettings(pkt);
			break;
		case WIZ_HOOK_CHAOTIC_EXCHANGE:
			HOOK_ChaoticExchange(pkt);
			break;
		case WIZ_HOOK_MERCHANT:
			HOOK_MerchantHandler(pkt);
			break;
		case WIZ_HOOK_USERDATA:
			HOOK_ReqUserData(pkt);
			break;
		case WIZ_HOOK_TEMPITEMS:
			HOOK_SendTempItems(pkt);
			break;
		case WIZ_HOOK_CR:
			break;
		case WIZ_HOOK_SUPPORT:
			HOOK_Support(pkt);
			break;
		case WIZ_HOOK_MERCHANTLIST:
			HOOK_ReqMerchantList(pkt);
			break;
		case WIZ_HOOK_LIFESKILL:
			HOOK_HandleLifeSkill(pkt);
			break;
		case WIZ_HOOK_MONSTER_INFO:
			MonsterInfoHandler(pkt);
			break;
		case WIZ_HOOK_EVENT:
			TempleProcess(pkt);
			break;
		case WIZ_HOOK_VOICE:
			HOOK_VoiceHandler(pkt);
			break;
		default:
			printf("[SID=%d] Hook System Unknown packet isInGame() %X\n", GetSocketID(), command);
			break;
		}
	}
}
#pragma endregion

#pragma region CUser::HOOK_VoiceHandler(Packet& pkt) 
void CUser::HOOK_VoiceHandler(Packet& pkt)
{
	if (!isInParty())
		return;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_VOICE));
	result << GetID() << uint64(0);
	Send(&result);

	result.clear();
	result.Initialize(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_VOICE) << GetID();
	result.append(pkt.contents(), pkt.size());

	CUser* pPartyUser;
	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty)
	{
		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
			if (pPartyUser)
			{
				if (GetID() == pPartyUser->GetID())
					continue;

				pPartyUser->Send(&result);
			}
		}
	}
}
#pragma endregion

#pragma region CUser::HOOK_SendTempItems(Packet& pkt) 
void CUser::HOOK_SendTempItems(Packet& pkt) 
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_TEMPITEMS));
	vector<_TempsItems> items;

	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		if (m_sItemArray[i].nNum)
		{
			if (m_sItemArray[i].nExpirationTime <= 0)
				continue;

			_TempsItems item;
			item.nItemID = m_sItemArray[i].nNum;
			item.pos = i >= 51 && i <= 51 + MBAG_TOTAL ? 1 : 0;
			item.time = m_sItemArray[i].nExpirationTime;
			items.push_back(item);
		}
	}

	for (int i = 0; i < WAREHOUSE_MAX; i++)
	{
		if (m_sWarehouseArray[i].nNum)
		{
			if (m_sWarehouseArray[i].nExpirationTime <= 0)
				continue;

			_TempsItems item;
			item.nItemID = m_sWarehouseArray[i].nNum;
			item.pos = 2;
			item.time = m_sWarehouseArray[i].nExpirationTime;
			items.push_back(item);
		}
	}

	result << uint8(items.size());
	for (auto itr : items)
		result << itr.nItemID << itr.pos << itr.time;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_HpInfo()
void CUser::HOOK_HpInfo()
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_HPINFO));
	result << uint16(m_MaxHP) << m_sHp << uint16(m_MaxMP) << m_sMp;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_Support(Packet& pkt)
void CUser::HOOK_ReqMerchantList(Packet& pkt)
{
	uint8 subCode;
	pkt >> subCode;
	switch (subCode)
	{
	case 0: 
	{
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MERCHANTLIST));
		uint32 merchantCount = 0;
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		std::vector<uint32> MerchantOfficalListen;
		foreach(itr, sessMap)
		{
			CUser* pUser = TO_USER(itr->second);
			if (pUser == nullptr)
				continue;

			if (!pUser->isInGame()
				|| !pUser->isMerchanting())
				continue;

			for (int j = 0; j < MAX_MERCH_ITEMS; j++) {
				_MERCH_DATA* pMerch = &pUser->m_arMerchantItems[j];

				if (pMerch->nNum == 0
					|| pMerch->nPrice == 0)
					continue;

				merchantCount++;
			}
			MerchantOfficalListen.push_back(pUser->GetSocketID());
		}
		result << merchantCount;

		foreach(itr, MerchantOfficalListen)
		{
			CUser* pUser = g_pMain->GetUserPtr(*itr);
			if (pUser == nullptr)
				continue;

			if (!pUser->isInGame()
				|| !pUser->isMerchanting())
				continue;

			for (int j = 0; j < MAX_MERCH_ITEMS; j++) {
				_MERCH_DATA* pMerch = &pUser->m_arMerchantItems[j];

				if (pMerch->nNum == 0 || pMerch->nPrice == 0)
					continue;

				result << pUser->GetSocketID()
					<< uint32(pUser->GetID())
					<< pUser->GetName()
					<< uint8((pUser->isSellingMerchant() ? uint8(0) : uint8(1)))
					<< pMerch->nNum
					<< pMerch->sCount
					<< pMerch->nPrice
					<< pMerch->isKC
					<< pUser->GetX()
					<< pUser->GetY()
					<< pUser->GetZ();
			}
		}
		Send(&result);
	}break;
	case 1:
	{
		string s_sTragetID;
		pkt >> s_sTragetID;

		CUser* pUser = g_pMain->GetUserPtr(s_sTragetID, NameType::TYPE_CHARACTER);
		if (pUser == nullptr)
			return;

		if (!pUser->isMerchanting())
			return;

		ZoneChange(pUser->GetZoneID(), pUser->GetX(), pUser->GetZ());
	}break;
	case 2:
	{
		string s_sTragetID;
		pkt >> s_sTragetID;

		CUser* pUser = g_pMain->GetUserPtr(s_sTragetID, NameType::TYPE_CHARACTER);
		if (pUser == nullptr)
			return;

		Packet result;
		string msg = "";
		ChatPacket::Construct(&result, PRIVATE_CHAT, &msg, &pUser->GetName());
		Send(&result);
	}break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::HOOK_Support(Packet& pkt)
void CUser::HOOK_Support(Packet& pkt)
{
	if (HOOK_LASTSUPPORT == 0)
		HOOK_LASTSUPPORT = UNIXTIME;

	if (UNIXTIME - HOOK_LASTSUPPORT < HOOK_SUPPORT_CHEK)
		return;

	uint8 subCode;
	pkt >> subCode;
	string subject, message;
	pkt >> subject >> message;
	if (subject.size() > 25 || message.size() > 40)
		return;

	DateTime time;
	g_pMain->WriteCheatLogFile(string_format("[Log Support - %d:%d:%d] %s, %s, %s, %s, %s, %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetAccountName().c_str(), GetName().c_str(), GetRemoteIP().c_str(), subject.c_str(), message.c_str(), subCode == 0 ? "Bug" : "Bot & Koxp"));
	HOOK_LASTSUPPORT = UNIXTIME;
}
#pragma endregion

#pragma region CUser::HOOK_ReqUserInfo(Packet& pkt)
void CUser::HOOK_ReqUserInfo(Packet& pkt)
{
	uint32 moneyReq = 0;
	if (GetPremium() == WAR_Premium)
		moneyReq = 0;
	else
	{
		moneyReq = (int)pow((GetLevel() * 2.0f), 3.4f);
		if (GetLevel() < 30)
			moneyReq = (int)(moneyReq * 0.4f);
		else if (GetLevel() >= 60)
			moneyReq = (int)(moneyReq * 1.5f);

		if ((g_pMain->m_sDiscount == 1
			&& g_pMain->m_byOldVictory == GetNation())
			|| g_pMain->m_sDiscount == 2)
			moneyReq /= 2;
	}
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_USERINFO));
	result << uint32(m_nKnightCash) << uint32(m_nKnightCashBonus) 
		<< uint16(m_sDaggerR) << uint16(m_sAxeR) << uint16(m_sSwordR) 
		<< uint16(m_sMaceR) << uint16(m_sSpearR) << uint16(m_sBowR) 
		<< uint16(m_sJamadarR) << uint32(moneyReq);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_HandleLifeSkill(Packet& pkt)
void CUser::HOOK_HandleLifeSkill(Packet& pkt)
{
	HOOK_SendLifeSkills();
}

void CUser::HOOK_SendLifeSkills()
{
	LF_VEC2 m_War = GetLifeSkillLevel(LIFE_SKILL::WAR);
	LF_VEC2 m_Hunting = GetLifeSkillLevel(LIFE_SKILL::HUNTING);
	LF_VEC2 m_Smithery = GetLifeSkillLevel(LIFE_SKILL::SMITHERY);
	LF_VEC2 m_Karma = GetLifeSkillLevel(LIFE_SKILL::KARMA);

	Packet result(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_LIFESKILL)
		<< m_War.LVL << m_War.PER
		<< m_Hunting.LVL << m_Hunting.PER
		<< m_Smithery.LVL << m_Smithery.PER
		<< m_Karma.LVL << m_Karma.PER;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_StayAlive(Packet& pkt)
void CUser::HOOK_StayAlive(Packet& pkt)
{
	std::string public_key = md5("0x" + std::to_string(HOOK_VERSION) + "52824" + m_strAccountID);
	std::string uPublic_key;
	pkt >> uPublic_key;
	if (&uPublic_key != nullptr) {
		if (public_key != uPublic_key) {
			//printf("%s Version mismatch. Please update your game or reinstall.", m_strAccountID.c_str());
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
		else {
			HOOK_LASTCHECK = UNIXTIME;
			Packet result(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_USERDATA)
				<< GetSocketID() 
				<< GetName() 
				<< GetClass() 
				<< GetRace() 
				<< GetLevel() 
				<< GetNation()
				<< uint16(m_bStats[0])
				<< uint16(m_bStats[1])
				<< uint16(m_bStats[2])
				<< uint16(m_bStats[3])
				<< uint16(m_bStats[4])
				<< g_pMain->Settings->ACS_Validation
				<< g_pMain->Settings->Button_Facebook
				<< g_pMain->Settings->Button_Discord
				<< g_pMain->Settings->Button_Live
				<< g_pMain->Settings->Button_Support
				<< g_pMain->Settings->URL_Facebook
				<< g_pMain->Settings->URL_Discord 
				<< g_pMain->Settings->URL_Live 
				<< g_pMain->Settings->URL_KCBayi
				<< g_pMain->Settings->KCMerchant_MinPrice
				<< g_pMain->Settings->KCMerchant_MaxPrice 
				<< g_pMain->Settings->State_StatReset 
				<< g_pMain->Settings->State_SkillReset
				<< g_pMain->Settings->State_TempItemList
				<< g_pMain->Settings->State_PUS;
			Send(&result);
		}
	}
	else {
		UserDataSaveToAgent();
		Disconnect();
		return;
	}
}
#pragma endregion

#pragma region CUser::HOOK_UIRequest(Packet& pkt)
void CUser::HOOK_ReqUserData(Packet& pkt)
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_USERDATA));
	result << GetSocketID()
		<< GetName() 
		<< GetClass()
		<< GetRace() 
		<< GetLevel()
		<< GetNation()
		<< uint16(m_bStats[0]) 
		<< uint16(m_bStats[1]) 
		<< uint16(m_bStats[2]) 
		<< uint16(m_bStats[3]) 
		<< uint16(m_bStats[4])
		<< g_pMain->Settings->ACS_Validation
		<< g_pMain->Settings->Button_Facebook
		<< g_pMain->Settings->Button_Discord
		<< g_pMain->Settings->Button_Live
		<< g_pMain->Settings->Button_Support
		<< g_pMain->Settings->URL_Facebook
		<< g_pMain->Settings->URL_Discord 
		<< g_pMain->Settings->URL_Live
		<< g_pMain->Settings->URL_KCBayi
		<< g_pMain->Settings->KCMerchant_MinPrice
		<< g_pMain->Settings->KCMerchant_MaxPrice
		<< g_pMain->Settings->State_StatReset
		<< g_pMain->Settings->State_SkillReset
		<< g_pMain->Settings->State_TempItemList
		<< g_pMain->Settings->State_PUS;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_UIRequest(Packet& pkt)
void CUser::HOOK_UIRequest(Packet& pkt)
{
	int moneyReq = 0;
	if (GetPremium() == WAR_Premium)
		moneyReq = 0;
	else
	{
		moneyReq = (int)pow((GetLevel() * 2.0f), 3.4f);
		if (GetLevel() < 30)
			moneyReq = (int)(moneyReq * 0.4f);
		else if (GetLevel() >= 60)
			moneyReq = (int)(moneyReq * 1.5f);

		if ((g_pMain->m_sDiscount == 1
			&& g_pMain->m_byOldVictory == GetNation())
			|| g_pMain->m_sDiscount == 2)
			moneyReq /= 2;
	}
	Packet result(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_UIINFO)
		<< uint32(m_nKnightCash) 
		<< uint32(m_nKnightCashBonus) 
		<< uint16(m_sDaggerR) 
		<< uint16(m_sAxeR) 
		<< uint16(m_sSwordR) 
		<< uint16(m_sMaceR) 
		<< uint16(m_sSpearR)
		<< uint16(m_sBowR) 
		<< uint16(m_sJamadarR)
		<< uint32(moneyReq)
		<< GetSocketID() 
		<< GetName() 
		<< GetClass() 
		<< GetRace() 
		<< GetLevel() 
		<< GetNation()
		<< m_bStats[0] 
		<< m_bStats[1] 
		<< m_bStats[2] 
		<< m_bStats[3] 
		<< m_bStats[4];
	Send(&result);

	result.clear();
	result.Initialize(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_USERDATA) 
		<< GetSocketID()
		<< GetName()
		<< GetClass() 
		<< GetRace() 
		<< GetLevel() 
		<< GetNation()
		<< uint16(m_bStats[0])
		<< uint16(m_bStats[1])
		<< uint16(m_bStats[2])
		<< uint16(m_bStats[3])
		<< uint16(m_bStats[4])
		<< g_pMain->Settings->ACS_Validation 
		<< g_pMain->Settings->Button_Facebook
		<< g_pMain->Settings->Button_Discord
		<< g_pMain->Settings->Button_Live
		<< g_pMain->Settings->Button_Support 
		<< g_pMain->Settings->URL_Facebook
		<< g_pMain->Settings->URL_Discord
		<< g_pMain->Settings->URL_Live 
		<< g_pMain->Settings->URL_KCBayi
		<< g_pMain->Settings->KCMerchant_MinPrice
		<< g_pMain->Settings->KCMerchant_MaxPrice
		<< g_pMain->Settings->State_StatReset
		<< g_pMain->Settings->State_SkillReset
		<< g_pMain->Settings->State_TempItemList
		<< g_pMain->Settings->State_PUS;
	Send(&result);

	HOOK_PowerUpStoreRequest((Packet)(NULL));

	result.clear();
	result.Initialize(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_TEMPITEMS);
	HOOK_SendTempItems(result);
}
#pragma region

#pragma region CUser::HOOK_AuthInfo(Packet& pkt)
void CUser::HOOK_General(Packet& pkt)
{
	Packet result;
	uint8 sOpCode = pkt.read<uint8>();
	DateTime time;
	switch (sOpCode)
	{
	case 22:
	{
		uint32 mackey;
		string hardwareid;
		std::string s_strAccountID;
		pkt >> s_strAccountID >> mackey >> hardwareid;
		if (m_strAccountID != s_strAccountID)
		{
			UserDataSaveToAgent();
			Disconnect();
			return;
		}

		uint8 HardwareIDCheck = g_DBAgent.UserHWIDCheck(s_strAccountID, hardwareid, mackey);
		if (HardwareIDCheck == 9 
			|| HardwareIDCheck == 4)
		{
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
		else if (HardwareIDCheck == 3 
			|| HardwareIDCheck == 6)
		{
			result.clear();
			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_MESSAGE) << "Error" << "This account is banned.";
			Send(&result);
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
		else if (HardwareIDCheck == 2)
		{
			//printf("HWID Success: %s - %s\n", GetRemoteIP().c_str(), GetAccountName().c_str());
			//m_mackey = mackey;
		}
		else
		{
			result.clear();
			result.Initialize(WIZ_HOOK_GUARD);
			result << uint8(WIZ_HOOK_MESSAGE) << "Error" << "An unknown error has been occured. Please contact with administrators.";
			Send(&result);
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
	}break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::HOOK_AuthInfo(Packet& pkt)
void CUser::HOOK_AuthInfo(Packet& pkt) {
	uint8 u_auth;
	pkt >> u_auth;
	if (&u_auth != nullptr) {
		if (u_auth == AUTHORITY_GAME_MASTER && !isGM()) {
			string graphicCards, processor, ip;
			pkt >> graphicCards >> processor;
			ip = GetRemoteIP();
			DateTime time;
			g_pMain->WriteCheatLogFile(string_format("[Log Detection - %d:%d:%d] %s, %s, %s, %s, %s, %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetAccountName().c_str(), GetName().c_str(), "Wallhack", graphicCards.c_str(), processor.c_str(), ip.c_str()));
			g_pMain->SendFormattedNotice("%s is currently disconnect due to editing memory.", Nation::ALL, GetName().c_str());
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
	}
	else {
		UserDataSaveToAgent();
		Disconnect();
		return;	
	}
}
#pragma endregion

#pragma region CUser::HOOK_CashLose(uint32 cash)
bool CUser::HOOK_CashLose(uint32 cash)
{
	if (m_nKnightCash < cash)
		return false;

	m_nKnightCash -= cash;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
	result << m_nKnightCash << m_nKnightCashBonus;
	Send(&result);

	return true;
}
#pragma endregion

#pragma region CUser::HOOK_CashGain(uint32 cash)
void CUser::HOOK_CashGain(uint32 cash)
{
	m_nKnightCash += cash;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
	result << m_nKnightCash << m_nKnightCashBonus;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_MerchantHandler(Packet& pkt)
void CUser::HOOK_MerchantHandler(Packet& pkt)
{
	uint8 subCode;
	pkt >> subCode;

	if (!isSellingMerchantingPreparing() 
		|| isBuyingMerchantingPreparing())
		return;

	switch (subCode)
	{
	case MERCHANT_ITEM_ADD:
		HOOK_MerchantItemAdd(pkt);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::HOOK_MerchantItemAdd(Packet& pkt)
void CUser::HOOK_MerchantItemAdd(Packet& pkt)
{
	if (!isSellingMerchantingPreparing() 
		|| isBuyingMerchantingPreparing())
		return;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_ADD));
	uint32 nGold, nItemID;
	uint16 sCount;
	uint8 bSrcPos, // It sends the "actual" inventory slot (SLOT_MAX -> INVENTORY_MAX-SLOT_MAX), so need to allow for it. 
		bDstPos,
		bMode, // Might be a flag for normal / "premium" merchant mode, once skills are implemented take another look at this.
		isKC;

	pkt >> nItemID >> sCount >> nGold >> bSrcPos >> bDstPos >> bMode >> isKC;
	// TODO: Implement the possible error codes for these various error cases.
	if (bSrcPos >= HAVE_MAX
		|| bDstPos >= MAX_MERCH_ITEMS)
		return;

	_ITEM_TABLE* pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| nItemID >= ITEM_NO_TRADE_MIN && nItemID <= ITEM_NO_TRADE_MAX // Cannot be traded, sold or stored.
		|| pTable->m_bRace == RACE_UNTRADEABLE // Cannot be traded or sold.
		|| (pTable->m_bKind == 255 && sCount != 1)) // Cannot be traded or sold.
		goto fail_return;

	bSrcPos += SLOT_MAX;
	_ITEM_DATA* pSrcItem = GetItem(bSrcPos);
	if (pSrcItem == nullptr
		|| pSrcItem->nNum != nItemID
		|| pSrcItem->sCount == 0
		|| sCount == 0
		|| pSrcItem->sCount < sCount
		|| pSrcItem->isRented()
		|| pSrcItem->isSealed()
		|| pSrcItem->isBound()
		|| pSrcItem->isDuplicate()
		|| pSrcItem->isExpirationTime())
		goto fail_return;

	_MERCH_DATA* pMerch = &m_arMerchantItems[bDstPos];

	if (pMerch == nullptr)
		return;

	pMerch->nNum = nItemID;
	pMerch->nPrice = nGold;
	pMerch->sCount = sCount; // Selling Count
	if (pTable->m_bKind == 255)
		pMerch->bCount = sCount; // Original Count ( INVENTORY )
	else
		pMerch->bCount = pSrcItem->sCount; // Original Count ( INVENTORY )

	pMerch->sDuration = pSrcItem->sDuration;
	pMerch->nSerialNum = pSrcItem->nSerialNum; // NOTE: Stackable items will have an issue with this.
	pMerch->bOriginalSlot = bSrcPos;
	pMerch->isKC = isKC;
	pMerch->IsSoldOut = false;

	// Take the user's item.
	memset(pSrcItem, 0, sizeof(_ITEM_DATA));

	result << uint16(1)
		<< nItemID << sCount << pMerch->sDuration << nGold
		<< bSrcPos << bDstPos << isKC;
	Send(&result);

fail_return:
	result << uint16(0)
		<< nItemID << sCount << (uint16)bSrcPos + bDstPos << nGold
		<< bSrcPos << bDstPos << isKC;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_ChaoticExchange(Packet& pkt)
void CUser::HOOK_ChaoticExchange(Packet& pkt)
{
	enum ResultOpCodes
	{
		Failed = 0,
		Success = 1
	};

	enum ResultMessages
	{
		EffectNone = 0, // No effect
		EffectRed = 1, // There will be better days.
		EffectGreen = 2, // Don't be too disappointed. You're luck isn't that bad.
		EffectWhite = 3 // It must be your lucky day.
	};

	uint16 nObjectID = 0;
	uint32 nExchangeItemID = 0;
	ResultOpCodes resultOpCode = Failed;
	ResultMessages resultMessage = EffectNone;
	uint32 nTotalPercent = 0;
	uint32 bRandArray[10000];
	memset(&bRandArray, 0, sizeof(bRandArray));
	int offset = 0;
	_ITEM_EXCHANGE* pExchange;
	std::vector<uint32> ExchangeIndexList;
	uint8 sItemSlot = 0;
	uint8 sExchangeItemSlot = 0;
	uint8 bStockInn = 0;
	uint8 bSell = 0;
	uint32 transactionPrice = 0; uint32 nItemID = 0; uint32 bRandSlot = 0;
	pkt >> nObjectID >> nExchangeItemID >> bStockInn >> bSell;

	if (bStockInn < 0 
		|| bStockInn > 1 
		|| bSell < 0 
		|| bSell > 1)
		goto fail_return;

	CNpc* pNpc = g_pMain->GetNpcPtr(nObjectID, GetZoneID());
	if (pNpc == nullptr
		|| (pNpc->GetType() != NPC_CHAOTIC_GENERATOR && pNpc->GetType() != NPC_CHAOTIC_GENERATOR2)
		|| isTrading()
		|| isMerchanting()
		|| isMining()
		|| (UNIXTIME2 - m_iLastExchangeTime < 400))
		goto fail_return;

	m_iLastExchangeTime = UNIXTIME2;
	sExchangeItemSlot = FindSlotForItem(nExchangeItemID, 1) - SLOT_MAX;
	if (g_pMain->m_ItemExchangeArray.GetSize() > 0)
	{
		foreach_stlmap(itr, g_pMain->m_ItemExchangeArray)
		{
			if (itr->second->nOriginItemNum[0] == nExchangeItemID)
				ExchangeIndexList.push_back(itr->second->nIndex);
			else
				continue;
		}
	}

	foreach(itr, ExchangeIndexList)
	{
		pExchange = g_pMain->m_ItemExchangeArray.GetData(*itr);

		if (pExchange == nullptr
			|| !CheckExchange(*itr)
			|| pExchange->bRandomFlag >= 101
			|| !CheckExistItemAnd(pExchange->nOriginItemNum[0], pExchange->sOriginItemCount[0], 0, 0, 0, 0, 0, 0, 0, 0))
			continue;

		if (offset >= 10000)
			break;

		for (int i = 0; i < int(pExchange->sExchangeItemCount[0] / 5); i++)
		{
			if (i + offset >= 10000)
				break;

			bRandArray[offset + i] = pExchange->nExchangeItemNum[0];
		}

		offset += int(pExchange->sExchangeItemCount[0] / 5);
	}

	if (offset >= 10000)
		offset = 10000;

	bRandSlot = myrand(0, offset);
	nItemID = bRandArray[bRandSlot];

	_ITEM_TABLE* pItem = g_pMain->m_ItemtableArray.GetData(nItemID);
	if (pItem == nullptr)
		goto fail_return;

	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++)
	{
		if (GetItem(i)->nNum == 0 && ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
			break;
	}

	if (bFreeSlots <= 1)
		goto fail_return;

	int SlotForItem = FindSlotForItem(pItem->m_iNum, 1);
	if (SlotForItem == -1)
		goto fail_return;

	sItemSlot = GetEmptySlot() - SLOT_MAX;
	RobItem(pExchange->nOriginItemNum[0], 1);

	if (bStockInn == 1 && !bSell)
		GiveWerehouseItem(nItemID, 1, false, false);
	else if (bStockInn == 0 && !bSell)
		GiveItem(nItemID, 1);
	else 
	{
		_ITEM_TABLE* pItemTables = g_pMain->GetItemPtr(nItemID);
		if (pItemTables == nullptr)
			goto fail_return;

		if (pItemTables->m_ItemType == 4) 
		{
			if (bStockInn == 1)
				GiveWerehouseItem(nItemID, 1, false, false);
			else
				GiveItem(nItemID, 1);
		}
		else 
		{
			if (pItemTables->m_iSellPrice != SellTypeFullPrice)
				transactionPrice += ((pItemTables->m_iBuyPrice / (GetPremiumProperty(PremiumItemSellPercent) > 0 ? 4 : 6))); // /6 is normal, /4 for prem/discount
			else
				transactionPrice += pItemTables->m_iBuyPrice;

			if (GetCoins() + transactionPrice > COIN_MAX)
				goto fail_return;
		}
	}

	if (pItem->m_ItemType == 4)
		resultMessage = EffectWhite;
	else if (pItem->m_ItemType == 5 
		|| pItem->m_ItemType == 11 
		|| pItem->m_ItemType == 12)
		resultMessage = EffectGreen;
	else
		resultMessage = EffectRed;

	resultOpCode = Success;
fail_return:
	Packet result(WIZ_ITEM_UPGRADE);
	result << (uint8)ITEM_BIFROST_EXCHANGE 
		<< (uint8)resultOpCode 
		<< nItemID 
		<< sItemSlot 
		<< nExchangeItemID 
		<< sExchangeItemSlot 
		<< (uint8)resultMessage;

	Send(&result);

	result.clear();
	result.SetOpcode(WIZ_OBJECT_EVENT);
	result << (uint8)OBJECT_ARTIFACT << (uint8)resultMessage << nObjectID;

	if (resultOpCode != Failed)
	{
		if (GetEventRoom() > 0)
			SendToRegion(&result, nullptr, GetEventRoom());
		else
			SendToRegion(&result);
	}

	_ITEM_TABLE* pItemTables = g_pMain->GetItemPtr(nItemID);

	if (resultOpCode != Failed && pItemTables != nullptr) 
	{
		if (bStockInn == 1)
		{
			if ((bSell 
				&& pItemTables->m_ItemType == 4) 
				|| !bSell)
				g_pMain->SendHelpDescription(this, string_format("%s has been stored.", pItemTables->m_sName.c_str()));
		}

		if (bSell 
			&& pItemTables->m_ItemType != 4 
			&& transactionPrice > 0)
			GoldGain(transactionPrice);
	}

	if (resultOpCode == Failed) 
	{
		result.clear();
		result.Initialize(WIZ_HOOK_GUARD);
		result << uint8(WIZ_HOOK_CHAOTIC_RESULT);
		Send(&result);
	}
}
#pragma endregion

#pragma region CUser::HOOK_Main()
void CUser::HOOK_Main()
{
	if (!isInGame() 
		|| !m_bSelectedCharacter)
		return;

	if (HOOK_TICKCOUNT == 0)
		HOOK_LASTCHECK = UNIXTIME;
	if (UNIXTIME - HOOK_LASTCHECK > HOOK_ALIVE_TIMEOUT) {
		if (HOOK_ACTIVE == 1) {
			//printf("%s Couldn't connect to ACS server.\n", m_strAccountID.c_str());
			g_pMain->SendHelpDescription(this, "Couldn't connect to ACS server.");
			UserDataSaveToAgent();
			Disconnect();
			return;
		}
	}
	else {
		HOOK_TICKCOUNT++;
	}
}
#pragma endregion

#pragma region CUser::MonsterInfoHandler(Packet & pkt)
void CUser::MonsterInfoHandler(Packet& pkt)
{
	uint32 MonsterID;
	pkt >> MonsterID;
	CNpcTable* pMob = g_pMain->m_arMonTable.GetData(MonsterID);
	if (pMob == nullptr)
		return;

	Packet result(WIZ_HOOK_GUARD);
	result << uint8(WIZ_HOOK_MONSTER_INFO) << pMob->m_sLevel << pMob->m_MaxHP << pMob->m_sDamage << pMob->m_sDefense;
	Send(&result);
}
#pragma endregion

#pragma region CUser::MonsterInfoHandler()
void CUser::HOOK_PowerUpStore()
{
	uint32 sItemCount = g_pMain->PusItemArray.GetSize();
	if (sItemCount == 0)
		return;

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_PUS));
	result << uint32(sItemCount);
	foreach_stlmap(itr, g_pMain->PusItemArray)
		result << uint32(itr->second->sItemID) << uint32(itr->second->sPrice) << uint8(itr->second->sType) << uint32(itr->second->sBuyCount);
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_PowerUpStoreRequest(Packet& pkt)
void CUser::HOOK_PowerUpStoreRequest(Packet& pkt)
{
	uint8 sCode;
	pkt >> sCode;
	switch (sCode)
	{
	case 0:
		HOOK_PowerUpStore();
		break;
	case 2:
		HOOK_PowerUpStorePurchase(pkt);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::HOOK_ItemNotice(uint8 type, uint32 nItemID)
void CUser::HOOK_ItemNotice(uint8 type, uint32 nItemID)
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_AUTODROP));
	result << type << nItemID;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_SendMessageBox(string title, string message)
void CUser::HOOK_SendMessageBox(string title, string message)
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MESSAGE));
	result << title << message;
	Send(&result);
}
#pragma endregion

#pragma region CUser::HOOK_PowerUpStorePurchase(Packet& pkt)
void CUser::HOOK_PowerUpStorePurchase(Packet& pkt)
{
	if (isMerchanting()
		|| isTrading()
		|| isDead()
		|| isBanned())
	{
		if (isMerchanting())
			HOOK_SendMessageBox("Failed", "You can't use power-up-store while merchanting.");
		if (isTrading())
			HOOK_SendMessageBox("Failed", "You can't use power-up-store while trading.");
		return;
	}
	uint32 sItemID;
	pkt >> sItemID;

	DateTime time;

	_HOOK_PUS_ITEM* pPowerUpStore = g_pMain->PusItemArray.GetData(sItemID);
	if (pPowerUpStore == nullptr)
		return;

	if (m_nKnightCash >= pPowerUpStore->sPrice) 
	{
		_ITEM_TABLE* pItemTables = g_pMain->m_ItemtableArray.GetData(pPowerUpStore->sItemID);
		if (pItemTables == nullptr)
			return;

		int SlotForItem = FindSlotForItem(pPowerUpStore->sItemID, pPowerUpStore->sBuyCount);
		if (SlotForItem == -1)
		{
			HOOK_SendMessageBox("Congratulations", "No Inventory Space");
			return;
		}

		m_nKnightCash -= pPowerUpStore->sPrice;
		g_DBAgent.UpdateKnightCash(this);
		GiveItem(pPowerUpStore->sItemID, pPowerUpStore->sBuyCount);
		HOOK_SendMessageBox("Congratulations", string_format("You have purchased %s for %d Knight Cash.", pItemTables->m_sName.c_str(), pPowerUpStore->sPrice));
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
		result << uint32(m_nKnightCash) << uint32(m_nKnightCashBonus);
		Send(&result);

		// Hook Pus Log System
		g_pMain->WriteHookPusSystemLog(string_format("[Hook Pus System - %d:%d:%d ] = %s | You have purchased %s.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pItemTables->m_sName.c_str()));
	}
	else if (m_nKnightCashBonus >= pPowerUpStore->sPrice) 
	{
		_ITEM_TABLE* pItemTables = g_pMain->m_ItemtableArray.GetData(pPowerUpStore->sPrice);
		if (pItemTables == nullptr)
			return;

		int SlotForItem = FindSlotForItem(pPowerUpStore->sItemID, pPowerUpStore->sBuyCount);
		if (SlotForItem == -1)
		{
			HOOK_SendMessageBox("Congratulations", "No Inventory Space");
			return;
		}

		m_nKnightCashBonus -= pPowerUpStore->sPrice;
		GiveItem(pPowerUpStore->sItemID, pPowerUpStore->sBuyCount);
		HOOK_SendMessageBox("Congratulations", string_format("You have purchased %s for %d Knight Cash.", pItemTables->m_sName.c_str(), pPowerUpStore->sPrice));
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
		result << uint32(m_nKnightCash) << uint32(m_nKnightCashBonus);
		Send(&result);

		// Hook Pus Log System
		g_pMain->WriteHookPusSystemLog(string_format("[Hook Pus System - %d:%d:%d ] = %s | You have purchased %s.\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pItemTables->m_sName.c_str()));
	}
}
#pragma endregion

#pragma region CUser::HOOK_AllSkillStatRestard(Packet & pkt)
void CUser::HOOK_AllSkillStatRestard(Packet& pkt)
{
	uint8 sSubCode;
	pkt >> sSubCode;
	if (isMerchanting()
		|| isTrading()
		|| isStoreOpen()
		|| isFishing()
		|| isMining()
		|| isDead()
		|| !isInGame())
		return;

	if (GetZoneID() == ZONE_KNIGHT_ROYALE
		|| GetZoneID() == ZONE_PRISON
		|| GetZoneID() == ZONE_CHAOS_DUNGEON
		|| GetZoneID() == ZONE_DUNGEON_DEFENCE)
		return;

	switch (sSubCode)
	{
	case 1:
	{
		uint8 point[4];
		for (int i = 0; i < 4; i++)
			pkt >> point[i];

		uint8 ceilPoint[4];

		if (isWarrior()
			|| isPortuKurian())
		{
			ceilPoint[0] = 80;
			ceilPoint[1] = 80;
			ceilPoint[2] = 83;
		}
		else if (isRogue())
		{
			ceilPoint[0] = 80;
			ceilPoint[1] = 80;
			ceilPoint[2] = 80;
		}
		else if (isMage())
		{
			ceilPoint[0] = 80;
			ceilPoint[1] = 80;
			ceilPoint[2] = 80;
		}
		else if (isPriest())
		{
			ceilPoint[0] = 80;
			ceilPoint[1] = 80;
			ceilPoint[2] = 80;
		}

		if (isMastered())
			ceilPoint[3] = 23;
		else
			ceilPoint[3] = 0;

		for (int i = 0; i < 4; i++) {
			if (point[i] > ceilPoint[i])
				return;
		}

		uint16 usedPoints = 0;
		uint16 maxPoints = (GetLevel() - 9) * 2;

		for (int i = 0; i < 4; i++)
			usedPoints += point[i];

		if (usedPoints > maxPoints)
			return;

		Packet result(WIZ_CLASS_CHANGE, uint8(ALL_SKILLPT_CHANGE));
		int index = 0, skill_point = 0, money = 0, temp_value = 0, old_money = 0;
		uint8 type = 0;

		temp_value = (int)pow((GetLevel() * 2.0f), 3.4f);
		if (GetLevel() < 30)
			temp_value = (int)(temp_value * 0.4f);
		else if (GetLevel() >= 60)
			temp_value = (int)(temp_value * 1.5f);

		// If global discounts are enabled 
		if (g_pMain->m_sDiscount == 2 // or war discounts are enabled
			|| (g_pMain->m_sDiscount == 1 && g_pMain->m_byOldVictory == m_bNation))
			temp_value /= 2;

		// Level too low.
		if (GetLevel() < 10)
			goto fail_return_skill;

		if (GetPremium() == WAR_Premium
			|| GetPremium() == Royal_Premium)
			temp_value = 0;

		// Get total skill points
		for (int i = 1; i < 9; i++)
			skill_point += m_bstrSkill[i];

		// If we don't have any skill points, there's no point resetting now is there.
		if (skill_point <= 0)
		{
			type = 2;
			goto fail_return_skill;
		}

		// Not enough money.
		if (!GoldLose(temp_value, false))
			goto fail_return_skill;

		// Reset skill points.
		m_bstrSkill[0] = maxPoints - usedPoints;
		for (int i = 1; i < 9; i++)
			m_bstrSkill[i] = 0;

		for (int i = 0; i < 4; i++)
			m_bstrSkill[i + 5] = point[i];

		result << uint8(1) << GetCoins() << m_bstrSkill[0];
		Send(&result);

		ResetWindows();
		InitType4();
		RecastSavedMagic();
		SendMyInfo();
		SetUserAbility();
		g_pMain->RegionUserInOutForMe(this);
		g_pMain->RegionNpcInfoForMe(this);

		return;
	fail_return_skill:
		result << type << temp_value;
		Send(&result);
	}break;
	case 2: {
		uint8 point[5];
		for (int i = 0; i < 5; i++)
			pkt >> point[i];

		uint8 basePoint[5];
		basePoint[0] = 50;
		basePoint[1] = 60;
		basePoint[2] = 60;
		basePoint[3] = 50;
		basePoint[4] = 50;

		if (isPriest())
			basePoint[4] += 20;
		else if (isWarrior() 
			|| isPortuKurian())
		{
			basePoint[0] += 15;
			basePoint[1] += 5;
		}
		else if (isMage())
		{
			basePoint[2] += 10;
			basePoint[4] += 20;
			basePoint[1] -= 10;
		}
		else if (isRogue())
		{
			basePoint[0] += 10;
			basePoint[2] += 10;
		}
	
		uint16 totalMinus = 0;

		for (int i = 0; i < 5; i++)
		{
			if (point[i] < basePoint[i])
				point[i] = basePoint[i];
			totalMinus += point[i] - basePoint[i];
		}

		uint16 freePoint = 10 + (GetLevel() - 1) * 3;

		if (GetLevel() > 60)
			freePoint += 2 * (GetLevel() - 60);

		if (totalMinus > freePoint)
			return;

		freePoint -= totalMinus;

		Packet result(WIZ_CLASS_CHANGE, uint8(ALL_POINT_CHANGE));
		int temp_money;
		uint16 statTotal;

		uint16 byStr, bySta, byDex, byInt, byCha;
		uint8 bResult = 0;

		bool bIsFree = false;

		if (GetLevel() > g_pMain->m_byMaxLevel)
			goto fail_return;

		if (GetPremium() == WAR_Premium
			|| GetPremium() == Royal_Premium)
			bIsFree = true;

		temp_money = (int)pow((GetLevel() * 2.0f), 3.4f);
		if (GetLevel() < 30)
			temp_money = (int)(temp_money * 0.4f);
		else if (GetLevel() >= 60)
			temp_money = (int)(temp_money * 1.5f);

		if ((g_pMain->m_sDiscount == 1 && g_pMain->m_byOldVictory == GetNation())
			|| g_pMain->m_sDiscount == 2)
			temp_money /= 2;

		for (int i = 0; i < SLOT_MAX; i++)
		{
			if (m_sItemArray[i].nNum) {
				bResult = 4;
				goto fail_return;
			}
		}

		bool allDefault = true;

		for (int j = 0; j < 5; j++)
			if (point[j] != basePoint[j])
				allDefault = false;

		if (allDefault 
			&& GetStatTotal() == 290)
		{
			bResult = 2;
			goto fail_return;
		}

		if (GetPremium() == WAR_Premium
			|| GetPremium() == Royal_Premium)
			temp_money = 0;

		if (!bIsFree && !GoldLose(temp_money, false))
			goto fail_return;

		SetStat(STAT_STR, point[0]);
		SetStat(STAT_STA, point[1]);
		SetStat(STAT_DEX, point[2]);
		SetStat(STAT_CHA, point[3]);
		SetStat(STAT_INT, point[4]);

		statTotal = GetStatTotal();
		SetUserAbility();

		byStr = GetStat(STAT_STR);
		bySta = GetStat(STAT_STA),
		byDex = GetStat(STAT_DEX);
		byInt = GetStat(STAT_INT),
		byCha = GetStat(STAT_CHA);

		m_sPoints = freePoint;

		result << uint8(1)
			<< GetCoins()
			<< byStr << bySta << byDex << byInt << byCha
			<< m_MaxHp << m_MaxMp << m_sTotalHit << m_sMaxWeight << m_sPoints;
		Send(&result);
		return;

	fail_return:
		result << bResult << temp_money;
		Send(&result);

	}break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::HOOK_LogHandler(Packet & pkt)
void CUser::HOOK_LogHandler(Packet& pkt)
{
	string log, graphicCards, processor, ip;
	pkt >> log >> graphicCards >> processor;
	ip = GetRemoteIP();

	if (&log == nullptr)
		return;

	DateTime time;
	g_pMain->WriteCheatLogFile(string_format("[Log Support - %d:%d:%d] %s, %s, %s, %s, %s, %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetAccountName().c_str(), GetName().c_str(), ip.c_str(), log.c_str(), graphicCards.c_str(), processor.c_str()));
}
#pragma endregion

#pragma region CUser::MonsterInfoHandler(Packet & pkt)
void CUser::HOOK_DropRequest(Packet& pkt)
{
	uint8 command;
	Packet result;
	pkt >> command;
	switch (command)
	{
	case 1:
	{
		uint32 sTraget;
		pkt >> sTraget;
		CNpc* pNpc = g_pMain->GetNpcPtr(sTraget, GetZoneID());
		if (pNpc == nullptr)
			return;

		_K_MONSTER_ITEM* pMonsterItem = g_pMain->m_MonsterItemArray.GetData(pNpc->GetProtoID());
		if (pMonsterItem == nullptr)
			return;

		result.clear();
		result.Initialize(WIZ_HOOK_GUARD);
		result << uint8(WIZ_HOOK_DROP_REQUEST) << command << pNpc->GetProtoID();
		for (int j = 0; j < 5; j++)
			result << uint32(pMonsterItem->iItem[j]) << uint16(pMonsterItem->sPercent[j]);
		result << uint32(0) << uint16(0);
		Send(&result);
	}break;
	case 2:
	{
		uint32 sGropID;
		uint8 sCount = 0;
		pkt >> sGropID;
		_MAKE_ITEM_GROUP* pItemGrops = g_pMain->m_MakeItemGroupArray.GetData(sGropID);
		if (pItemGrops == nullptr)
			return;

		result.clear();
		result.Initialize(WIZ_HOOK_GUARD);
		result << uint8(WIZ_HOOK_DROP_REQUEST) 
			<< command 
			<< uint8(30);

		for (uint32 itemID : pItemGrops->iItems)
		{
			if (sCount > 29)
				continue;

			result << itemID;
			sCount++;
		}
		Send(&result);
	}break;
	case 3:
	{
		uint16 sProtoID;
		pkt >> sProtoID;

		_K_MONSTER_ITEM* pMonsterItems = g_pMain->m_MonsterItemArray.GetData(sProtoID);
		if (pMonsterItems == nullptr)
			return;

		result.clear();
		result.Initialize(WIZ_HOOK_GUARD);
		result << uint8(WIZ_HOOK_DROP_REQUEST) << uint8(1) << sProtoID;
		for (int i = 0; i < 5; i++)
			result << uint32(pMonsterItems->iItem[i]) << uint16(pMonsterItems->sPercent[i]);
		result << uint32(0) << uint16(0);
		Send(&result);

	}break;
	default:
		break;
	}
}
#pragma endregion

#pragma region CUser::LifeSkillHuntProcess(uint16 MonsterNum)
void CUser::LifeSkillHuntProcess(uint16 MonsterNum)
{
	CNpc* pNpc = g_pMain->GetNpcPtr(MonsterNum, GetZoneID());
	if (pNpc == nullptr)
		return;

	if (GetLevel() > pNpc->GetLevel())
	{
		if (GetLevel() - pNpc->GetLevel() > 60)
			return;
	}

	int exp = 1;
	if (pNpc->GetLevel() >= 110)
		exp = 10;

	if (isInParty())
	{
		CUser* pPartyUser;
		_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());

		if (pParty == nullptr)
			return;

		for (int i = 0; i < MAX_PARTY_USERS; i++) {
			pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
			if (pPartyUser == nullptr)
				continue;

			if (!pPartyUser->isInGame()
				|| pPartyUser->isDead()
				|| pPartyUser->isFishing()
				|| pPartyUser->isMining()
				|| pPartyUser->isStoreOpen()
				|| pPartyUser->isMerchanting()
				|| pPartyUser->isTrading()
				|| !pPartyUser->isInRange(this, RANGE_50M))
				continue;

			pPartyUser->m_sLifeSkills.HuntingExp += exp;
			pPartyUser->HOOK_SendLifeSkills();		
		}
	}
	if (!isInParty())
	{
		if (!isInGame()
			|| isDead()
			|| isFishing()
			|| isMining()
			|| isStoreOpen()
			|| isMerchanting()
			|| isTrading())
			return;

		m_sLifeSkills.HuntingExp += exp;
		HOOK_SendLifeSkills();
	}
}
#pragma endregion

#pragma region CUser::HOOK_SendPartyProcess()
void CUser::HOOK_SendPartyProcess()
{
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_PARTY_PROCESS));
	uint8 m_Count = 0;
	result << uint8(1) << m_Count;
	_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(GetPartyID());

	if (pParty == nullptr)
		return;

	for (int32 j = 0; j < MAX_PARTY_USERS; j++)
	{
		uint8 s_swift = 0;uint8 s_hpbuff = 0;uint8 s_acbuff = 0;uint8 s_resisbuff = 0;uint8 s_resbuff = 0;

		CUser* pPartyUser = g_pMain->GetUserPtr(pParty->uid[j]);
		if (pPartyUser == nullptr)
			continue;

		result << pPartyUser->GetID() << short(pPartyUser->GetMaxHealth()) << short(pPartyUser->GetHealth()) << pPartyUser->GetLevel();

		Guard lock(pPartyUser->m_buffLock);

		foreach(itr, pPartyUser->m_buffMap)
		{
			for (int i = 0; i < 26; i++)
				if (itr->second.m_nSkillID == s_SwiftSkills[i])
					s_swift = 1;

			for (int i = 0; i < 51; i++)
				if (itr->second.m_nSkillID == s_HpSkills[i])
					s_hpbuff = 1;

			for (int i = 0; i < 38; i++)
				if (itr->second.m_nSkillID == s_AcSkills[i])
					s_acbuff = 1;

			for (int i = 0; i < 16; i++)
				if (itr->second.m_nSkillID == s_ResisSkills[i])
					s_resisbuff = 1;

			for (int i = 0; i < 30; i++)
				if (itr->second.m_nSkillID == s_ResSkills[i])
					s_resbuff = 1;
		}

		result << s_swift << s_hpbuff << s_acbuff << s_resisbuff << s_resbuff;
		m_Count++;
	}
	if (m_Count < 2)
		result.put(0, 0);

	result.put(2, m_Count);
	Send(&result);
}
#pragma endregion

#pragma region CUser::SpeedHackTime(Packet & pkt)
void CUser::SpeedHackTime(Packet & pkt)
{
	if (!isInGame() || isGM())
		return;

	DateTime time;
	float nSpeed = 45.0f;

	if (GetFame() == COMMAND_CAPTAIN 
		|| isRogue())
		nSpeed = 90.0f;
	else if (isWarrior() 
		|| isMage() 
		|| isPriest())
		nSpeed = 67.0f;
	else if (isPortuKurian())
		nSpeed = 67.0f;

	nSpeed += 15.0f; // Tolerance...

	float nRange = (pow(GetX() - m_LastX, 2.0f) + pow(GetZ() - m_LastZ, 2.0f)) / 100.0f;

	if (nRange >= nSpeed)
	{
		m_SpeedHackTrial++;

		if (m_SpeedHackTrial >= 3)
			g_pMain->WriteCheatLogFile(string_format("SpeedHack - %d:%d:%d || %s is Warp to Last Position. Range = %d, Speed = %d\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), (int)nRange, (int)nSpeed));

		if (m_SpeedHackTrial >= 10)
			m_SpeedHackTrial = 0;
	}
	else
	{
		m_SpeedHackTrial = 0;
		m_LastX = GetX();
		m_LastZ = GetZ();
	}

#if 0 // temporarily disabled
	uint8 b_first;
	float servertime = 0.0f, clienttime = 0.0f, client_gap = 0.0f, server_gap = 0.0f;

	pkt >> b_first >> clienttime;

	if (b_first) {
		m_fSpeedHackClientTime = clienttime;
		m_fSpeedHackServerTime = TimeGet();
	}
	else {
		servertime = TimeGet();

		server_gap = servertime - m_fSpeedHackServerTime;
		client_gap = clienttime - m_fSpeedHackClientTime;

		if (client_gap - server_gap > 10.0f) {
			TRACE("%s SpeedHack User Checked By Server Time\n", m_id);
			Close();
		}
		else if (client_gap - server_gap < 0.0f) {
			m_fSpeedHackClientTime = clienttime;
			m_fSpeedHackServerTime = TimeGet();
		}
	}
#endif
}
#pragma endregion

#pragma region CUser::ProgramCheckProcess(Packet & pkt)
void CUser::ProgramCheckProcess(Packet& pkt)
{
	uint8 sOpCode;
	std::string sStrUserID, sProgramInfo;
	pkt >> sOpCode;

	if (sOpCode == 1)
	{
		if (!isGM())
			return;

		pkt >> sStrUserID;
		g_pMain->sGameMasterSocketID = -1;

		CUser* pUser = g_pMain->GetUserPtr(sStrUserID, TYPE_CHARACTER);

		if (pUser == nullptr
			|| !pUser->isInGame())
			return;

		g_pMain->sGameMasterSocketID = GetSocketID();
		Packet result(WIZ_PROGRAMCHECK, uint8(0x02));
		result << uint8(0x01);
		pUser->Send(&result);
	}
	else
	{
		pkt >> sProgramInfo;
		CUser* pUser = g_pMain->GetUserPtr(g_pMain->sGameMasterSocketID);

		if (pUser == nullptr
			|| !pUser->isInGame())
			return;

		g_pMain->SendHelpDescription(pUser, sProgramInfo.c_str());
	}
}
#pragma endregion

