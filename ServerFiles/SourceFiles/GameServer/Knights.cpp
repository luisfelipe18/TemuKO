#include "stdafx.h"
#include "Knights.h"
#include "KingSystem.h"

using std::string;

#pragma region CKnights::CKnights()
CKnights::CKnights()
{
	m_sIndex = 0;
	m_byFlag = ClanTypeNone;
	m_byNation = 0;
	m_byGrade = 5;
	m_byRanking = 0;
	m_sMembers = 1;
	memset(&m_Image, 0, sizeof(m_Image));
	m_nMoney = 0;
	m_sDomination = 0;
	m_nPoints = 0;
	m_nClanPointFund = 0;
	m_sCape = -1;
	m_sAlliance = 0;
	m_sAllianceReq = 0;
	m_sMarkLen = 0;
	m_sMarkVersion = 0;
	m_bCapeR = m_bCapeG = m_bCapeB = 0;
	m_sClanPointMethod = 0;
	sPremiumInUse = sPremiumTime = sClanBuffExp = sClanBuffLoyalty = m_iBank = NO_PREMIUM;
	memset(m_sWarehouseArray, 0x00, sizeof(m_sWarehouseArray));
}

#pragma endregion 

#pragma region CKnights::CheckKnightCastellanCapesBuffSystem(CUser *pUser)
void CKnights::CheckKnightCastellanCapesBuffSystem(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	bool sNotice = false;
	
	uint16 sAc = g_pMain->m_sCapeAC;
	uint8 sAtk = g_pMain->m_sCapeAtk;
	uint8 sNp = g_pMain->m_sCapeNp;
	uint16 sHp = g_pMain->m_sCapeHp;

	Packet result(WIZ_NOTICE);
	result.DByte();

	_KNIGHTS_CASTELLAN_CAPE* pCastellanCapeInfo = g_pMain->m_KnightCastellanCapeArray.GetData(pUser->GetClanID());

	if (pCastellanCapeInfo != nullptr) 
	{
		if (pCastellanCapeInfo->sRemainingTime >= (uint64)UNIXTIME)
		{
			if (!pUser->m_bSuperiorCapeFlag) 
			{
				pUser->m_sACPercent = pUser->m_sACPercent * (100 + sAc) / 100;
				pUser->m_bAttackAmount = pUser->m_bAttackAmount * (100 + sAtk) / 100;
				pUser->m_bSkillNPBonus += sNp;
				pUser->m_sMaxHPAmount = sHp * (pUser->GetMaxHealth() - pUser->m_sMaxHPAmount) / 100;
				pUser->SetUserAbility();
				pUser->m_bSuperiorCapeFlag = true;
				sNotice = true;
			}
		}
	}
	std::string header = string_format("Defanse Buff %%%d|Damage Buff %%%d|NP Buff +%d|HP Buff %%%d", sAc, sAtk, sNp, sHp);
	result << uint8(4) << uint8(sNotice == true ? 1 : 2) << "Superior Cape Bonus" << header;
	pUser->Send(&result);
}
#pragma endregion

#pragma region CKnights::CheckKnightCastellanCapesBuffSystem()
void CKnights::CheckKnightCastellanCapesBuffSystem()
{
	bool sNotice = false;

	uint16 sAc = g_pMain->m_sCapeAC;
	uint8 sAtk = g_pMain->m_sCapeAtk;
	uint8 sNp = g_pMain->m_sCapeNp;
	uint16 sHp = g_pMain->m_sCapeHp;

	_KNIGHTS_CASTELLAN_CAPE* pCastellanCapeInfo = g_pMain->m_KnightCastellanCapeArray.GetData(GetID());

	if (pCastellanCapeInfo != nullptr)
	{
		if (pCastellanCapeInfo->sRemainingTime >= (uint64)UNIXTIME)
		{
			sNotice = true;

			std::string header = string_format("Defanse Buff %%%d|Damage Buff %%%d|NP Buff +%d|HP Buff %%%d", sAc, sAtk, sNp, sHp);
			Packet result(WIZ_NOTICE);
			result.DByte();
			result << uint8(4) << uint8(sNotice == true ? 1 : 2) << "Superior Cape Bonus" << header;

			foreach_stlmap(i, m_arKnightsUser)
			{
				_KNIGHTS_USER* p = i->second;

				if (p == nullptr)
					continue;

				CUser* pUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
				if (pUser == nullptr
					|| !pUser->isInGame()
					|| pUser->m_bSuperiorCapeFlag)
					continue;

				pUser->m_sACPercent = pUser->m_sACPercent * (100 + sAc) / 100;
				pUser->m_bAttackAmount = pUser->m_bAttackAmount * (100 + sAtk) / 100;
				pUser->m_bSkillNPBonus += sNp;
				pUser->m_sMaxHPAmount = sHp * (pUser->GetMaxHealth() - pUser->m_sMaxHPAmount) / 100;
				pUser->m_bSuperiorCapeFlag = true;
				pUser->SetUserAbility();

				pUser->Send(&result);
			}
		}
	}
}
#pragma endregion

#pragma region CKnights::CheckKnightBuffSystem()
void CKnights::CheckKnightBuffSystem()
{
	uint8 Members = GetOnlineMembers();

	if (Members < 5)
		sClanBuffExp = sClanBuffLoyalty = 0;
	else if (Members >= 5 && Members < 10)
		sClanBuffExp = sClanBuffLoyalty = 5;
	else if (Members >= 10 && Members < 20)
		sClanBuffExp = sClanBuffLoyalty = 10;
	else if (Members >= 15 && Members < 20)
		sClanBuffExp = sClanBuffLoyalty = 20;
	else if (Members >= 20 && Members < 25)
		sClanBuffExp = sClanBuffLoyalty = 30;
	else if (Members >= 25 && Members < 30)
		sClanBuffExp = sClanBuffLoyalty = 40;
	else if (Members >= 30)
		sClanBuffExp = sClanBuffLoyalty = 50;

	std::string header = string_format("Exp Buff %%%d|Loyalty Buff +%d", sClanBuffExp, sClanBuffLoyalty);
	Packet result(WIZ_NOTICE);
	result.DByte();
	result << uint8(4) << uint8(sClanBuffExp > 0 ? 1 : 2) << "CLAN BUFF" << header;
	Send(&result);
}

uint8 CKnights::GetOnlineMembers()
{
	uint8 OnlinePlayer = 0;

	foreach_stlmap(i, m_arKnightsUser)
	{
		_KNIGHTS_USER* p = i->second;

		if (p == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		OnlinePlayer++;
	}

	if (OnlinePlayer == 0)
		return 0;

	return OnlinePlayer;
}
#pragma endregion

#pragma region CKnights::OnLogin(CUser *pUser)

void CKnights::OnLogin(CUser *pUser)
{
	if(pUser == nullptr)
		return;

	std:: string userid= pUser->GetName();
	STRTOUPPER(userid);
	_KNIGHTS_USER *pKnightUser = m_arKnightsUser.GetData(userid);
	if(pKnightUser == nullptr)
		return;

	Packet result;
	pKnightUser->bLevel = pUser->m_bLevel;
	pKnightUser->sClass = pUser->m_sClass;
	pKnightUser->nLoyalty = pUser->m_iLoyalty;
	pUser->m_pKnightsUser = pKnightUser;

	// Send login notice
	result.Initialize(WIZ_KNIGHTS_PROCESS);
	result << uint8(KNIGHTS_USER_ONLINE);
	result.SByte();
	result << pUser->GetName();
	Send(&result);

	// Construct the clan notice packet to send to the logged in player
	if (!m_strClanNotice.empty())
	{
		ConstructClanNoticePacket(&result);
		pUser->Send(&result);
	}
}


#pragma endregion 

#pragma region CKnights::OnLoginAlliance(CUser *pUser)

void CKnights::OnLoginAlliance(CUser *pUser)
{
	if(pUser == nullptr)
		return;

	std:: string userid= pUser->GetName();
	STRTOUPPER(userid);
	_KNIGHTS_USER *pKnightUser = m_arKnightsUser.GetData(userid);
	if(pKnightUser == nullptr)
		return;

	Packet result;
	pKnightUser->bLevel = pUser->m_bLevel;
	pKnightUser->sClass = pUser->m_sClass;
	pKnightUser->nLoyalty = pUser->m_iLoyalty;
	pUser->m_pKnightsUser = pKnightUser;

	// Construct the clan notice packet to send to the logged in player
	if (!m_strClanNotice.empty())
	{
		ConstructClanNoticePacket(&result);
		pUser->Send(&result);
	}
}

#pragma endregion 

#pragma region CKnights::ConstructClanNoticePacket(Packet *result)

void CKnights::ConstructClanNoticePacket(Packet *result)
{
	if(!m_strClanNotice.empty())
	{
		string clan = "Clan Notice";
		result->Initialize(WIZ_NOTICE);
		result->DByte();
		*result	<< uint8(4)			// type
			<< uint8(1)				// total blocks
			<< clan			// header
			<< m_strClanNotice;
	}
	else
	{
		string clan = "Clan Notice";
		result->Initialize(WIZ_NOTICE);
		result->DByte();
		*result	<< uint8(4)		// type
			<< uint8(2)			// total blocks
			<< clan;			// header
	}
}

#pragma endregion 

#pragma region CKnights::UpdateClanNotice(std::string & clanNotice)

/**
* @brief	Updates this clan's notice with clanNotice
* 			and informs logged in clan members.
*
* @param	clanNotice	The clan notice.
*/
void CKnights::UpdateClanNotice(std::string & clanNotice)
{
	if (clanNotice.length() > MAX_CLAN_NOTICE_LENGTH)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_UPDATENOTICE));;
	// Tell the database to update the clan notice.
	result << GetID() << clanNotice;
	g_pMain->AddDatabaseRequest(result);
}


#pragma endregion 

#pragma region CKnights::UpdateClanFund()

/**
* @brief	Sends a request to update the clan's fund in the database.
*/
void CKnights::UpdateClanFund()
{
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_UPDATE_FUND));
	result << GetID() << uint32(m_nClanPointFund);
	g_pMain->AddDatabaseRequest(result);
}

#pragma endregion 

#pragma region CKnights::UpdateClanGrade()

/**
* @brief	Sends a request to update the clan's fund in the database.
*/
void CKnights::UpdateClanGrade()
{
	uint32 mtotal = 0;
	foreach_stlmap (i, m_arKnightsUser)
	{
		_KNIGHTS_USER *p = i->second;

		if(p == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		if(pUser != nullptr && pUser->isInGame())
			p->nLoyalty = pUser->m_iLoyalty;

		mtotal += p->nLoyalty;
	}

	m_nPoints = mtotal;
	m_byGrade = g_pMain->GetKnightsGrade(m_nPoints);
}

#pragma endregion 

#pragma region CKnights::OnLogout(CUser *pUser) 

void CKnights::OnLogout(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_USER_OFFLINE));
	result.SByte();
	result << pUser->GetName();
	Send(&result);
}

#pragma endregion 

#pragma region CKnights::OnLogoutAlliance(CUser *pUser)

void CKnights::OnLogoutAlliance(CUser *pUser)
{
	return;

	Packet logoutNotice;
	// TODO: Shift this to SERVER_RESOURCE
	std::string buffer = string_format("%s is offline.", pUser->GetName().c_str()); 
	ChatPacket::Construct(&logoutNotice, ALLIANCE_CHAT, &buffer);
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	
	if(pKnights == nullptr)
		Send(&logoutNotice);
	else
		g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &logoutNotice);
}

#pragma endregion 

#pragma region CKnights::AddUser(std::string & strUserID, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, uint32 Loyalty)

bool CKnights::AddUser(std::string & strUserID, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, uint32 Loyalty)
{
	if(m_arKnightsUser.GetSize() >= MAX_CLAN_USERS)
		return false;

	std:: string userid= strUserID;
	STRTOUPPER(userid);
	if(m_arKnightsUser.GetData(userid) != nullptr)
		return false;

	_KNIGHTS_USER * pKnightUser = new _KNIGHTS_USER();

	pKnightUser->strUserName = strUserID;
	pKnightUser->bLevel = bLevel;
	pKnightUser->sClass = sClass;
	pKnightUser->nLastLogin = lastlogin;
	pKnightUser->bFame = bFame;
	pKnightUser->nLoyalty = Loyalty;

	CUser* pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser != nullptr)
		pUser->m_pKnightsUser = pKnightUser;

	m_arKnightsUser.PutData(userid, pKnightUser);

	return true;
}

#pragma endregion 

#pragma region CKnights::AddUser(CUser *pUser)

bool CKnights::AddUser(CUser *pUser)
{
	if (pUser == nullptr
		|| !AddUser(pUser->GetName(), TRAINEE, pUser->GetClass(), pUser->GetLevel(), uint32(UNIXTIME), pUser->m_iLoyalty))
		return false;

	pUser->SetClanID(m_sIndex);
	pUser->m_bFame = TRAINEE;
	m_sMembers++;

	CheckKnightBuffSystem();
	CheckKnightCastellanCapesBuffSystem(pUser);
	pUser->SendClanPremiumInfo();

	return true;
}

#pragma endregion 

#pragma region CKnights::RemoveUser(std::string & strUserID)

/**
* @brief	Removes the specified user from the clan array.
*
* @param	strUserID	Identifier for the user.
*
* @return	.
*/
bool CKnights::RemoveUser(std::string & strUserID)
{
	std:: string userid= strUserID;
	STRTOUPPER(userid);
	_KNIGHTS_USER *pKnightUser = m_arKnightsUser.GetData(userid);
	if(pKnightUser == nullptr)
		return false;

	// If they're not logged in (note: logged in users being removed have their NP refunded in the other handler)
	// but they've donated NP, ensure they're refunded for the next time they login.
	if (pKnightUser->nDonatedNP > 0)
		RefundDonatedNP(pKnightUser->nDonatedNP, nullptr, pKnightUser->strUserName.c_str());

	if (STRCASECMP(m_strViceChief_1.c_str(), strUserID.c_str()) == 0)
		m_strViceChief_1 = "";
	else if (STRCASECMP(m_strViceChief_2.c_str(), strUserID.c_str()) == 0)
		m_strViceChief_2 = "";
	else if (STRCASECMP(m_strViceChief_3.c_str(), strUserID.c_str()) == 0)
		m_strViceChief_3 = "";

	m_arKnightsUser.DeleteData(userid);

	if(m_sMembers > 1)
		m_sMembers--;
	return true;
}


#pragma endregion 

#pragma region CKnights::RemoveUser(CUser *pUser)

/**
* @brief	Removes the specified user from this clan.
*
* @param	pUser	The user.
*/
bool CKnights::RemoveUser(CUser *pUser)
{
	if (pUser == nullptr
		|| pUser->m_pKnightsUser == nullptr)
		return false;

	// Ensure users are refunded when they leave/are removed from a clan.
	// NOTE: If we bring back multiserver setups, this is going to cause synchronisation issues.
	uint32 nDonatedNP = pUser->m_pKnightsUser->nDonatedNP;
	if (nDonatedNP > 0)
		RefundDonatedNP(nDonatedNP, pUser);


	if (STRCASECMP(m_strViceChief_1.c_str(), pUser->GetName().c_str()) == 0)
		m_strViceChief_1 = "";
	else if (STRCASECMP(m_strViceChief_2.c_str(), pUser->GetName().c_str()) == 0)
		m_strViceChief_2 = "";
	else if (STRCASECMP(m_strViceChief_3.c_str(), pUser->GetName().c_str()) == 0)
		m_strViceChief_3 = "";

	pUser->SetClanID(NO_CLAN);
	pUser->m_bFame = 0;
	pUser->m_pKnightsUser = nullptr;

	std:: string userid= pUser->GetName();
	STRTOUPPER(userid);
	m_arKnightsUser.DeleteData(userid);

	if(m_sMembers > 1)
		m_sMembers--;

	if (!pUser->isClanLeader())
		pUser->SendClanUserStatusUpdate();

	CheckKnightBuffSystem();
	CheckKnightCastellanCapesBuffSystem(pUser);
	pUser->SendClanPremiumInfo();

	return true;
}

#pragma endregion 

#pragma region CKnights::LoadUserMemo(std::string & strUserID, std::string & strMemo)

bool CKnights::LoadUserMemo(std::string & strUserID, std::string & strMemo)
{
	std:: string userid= strUserID;
	STRTOUPPER(userid);
	_KNIGHTS_USER * pKnightUser = m_arKnightsUser.GetData(userid);
	if(pKnightUser == nullptr)
		return false;

	pKnightUser->strMemo = strMemo;
	return true;
}

#pragma endregion 

#pragma region CKnights::RefundDonatedNP(uint32 nDonatedNP, CUser * pUser /*= nullptr*/, const char * strUserID /*= nullptr*/)

/**
* @brief	Refunds 30% of the user's donated NP.
* 			If the user has the item "CONT Recovery", refund ALL of the user's 
* 			donated NP.
*
* @param	nDonatedNP	The donated NP.
* @param	pUser	  	The user's session, when refunding the user in-game.
* 						Set to nullptr to indicate the use of the character's name
* 						and consequently a database update instead.
* @param	strUserID 	Logged out character's name. 
* 						Used to refund logged out characters' national points 
* 						when pUser is set to nullptr.
*/
void CKnights::RefundDonatedNP(uint32 nDonatedNP, CUser * pUser /*= nullptr*/, const char * strUserID /*= nullptr*/)
{
	uint32 m_totalDonationChange = nDonatedNP;

	if(m_byFlag >= ClanTypeAccredited5)
	{
		int counter = 0;
		while(m_totalDonationChange != 0)
		{
			if(m_nClanPointFund < m_totalDonationChange)
			{
				switch (m_byFlag)
				{
				case ClanTypeAccredited5:
					m_nClanPointFund = 0;
					m_totalDonationChange = 0;
					break;
				case ClanTypeAccredited4: // 7000 * 36 = 252000
					m_byFlag = ClanTypeFlag::ClanTypeAccredited5;
					m_nClanPointFund += 252000;  
					break;
				case ClanTypeAccredited3: // 10000 * 36 = 360000
					m_byFlag = ClanTypeFlag::ClanTypeAccredited4;
					m_nClanPointFund += 360000;   
					break;
				case ClanTypeAccredited2: // 15000 * 36 = 540000
					m_byFlag = ClanTypeFlag::ClanTypeAccredited3;
					m_nClanPointFund += 540000; 
					break;
				case ClanTypeAccredited1: // 20000 * 36 = 720000
					m_byFlag = ClanTypeFlag::ClanTypeAccredited2;
					m_nClanPointFund += 720000; 
					break;
				case ClanTypeRoyal5: // 25000 * 36 = 900000
					m_byFlag = ClanTypeFlag::ClanTypeAccredited1;
					m_nClanPointFund += 900000; 
					break;
				case ClanTypeRoyal4: // 30000 * 36 = 1080000
					m_byFlag = ClanTypeFlag::ClanTypeRoyal5;
					m_nClanPointFund += 1080000; 
					break;
				case ClanTypeRoyal3: // 35000 * 36 = 1260000
					m_byFlag = ClanTypeFlag::ClanTypeRoyal4;
					m_nClanPointFund += 1260000; 
					break;
				case ClanTypeRoyal2: // 40000 * 36 = 1440000
					m_byFlag = ClanTypeFlag::ClanTypeRoyal3;
					m_nClanPointFund += 1440000; 
					break;
				case ClanTypeRoyal1: // 45000 * 36 = 1620000
					m_byFlag = ClanTypeFlag::ClanTypeRoyal2;
					m_nClanPointFund += 1620000; 
					break;
				default:
					TRACE("A clan with unknow clantype IDNum = %d ClanPoints = %d", GetID(), (uint32)m_nClanPointFund);
					printf("A clan with unknow clantype IDNum = %d ClanPoints = %d", GetID(), (uint32)m_nClanPointFund);
					break;
				}

				if(m_nClanPointFund < m_totalDonationChange)
				{
					m_totalDonationChange -= m_nClanPointFund;
					m_nClanPointFund = 0;
				}
				else
				{
					m_nClanPointFund -= m_totalDonationChange;
					m_totalDonationChange = 0;
					break;
				}
			}
			else
			{
				m_nClanPointFund -= m_totalDonationChange;
				m_totalDonationChange = 0;
				break;
			}
		}
	}
	else
	{
		if(m_nClanPointFund < nDonatedNP)
			m_nClanPointFund = 0;
		else
			m_nClanPointFund -= nDonatedNP;
	}

	_KNIGHTS_CAPE *pCape = g_pMain->m_KnightsCapeArray.GetData(m_sCape);
	if (pCape != nullptr)
	{
		if ((pCape->byGrade && m_byGrade > pCape->byGrade)
			// not sure if this should use another error, need to confirm
				|| m_byFlag < pCape->byRanking)
		{
			m_sCape = 0; m_bCapeR = 0;  m_bCapeG = 0; m_bCapeB = 0;

			// Now save (we don't particularly care whether it was able to do so or not).
			Packet db(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_UPDATE_CAPE));
			db	<< GetID() << GetCapeID()
				<< m_bCapeR << m_bCapeG << m_bCapeB;
			g_pMain->AddDatabaseRequest(db);
			SendUpdate();
		}
	}

	// If the player's logged in, just adjust their national points in-game.
	if (pUser != nullptr)
	{
		// Refund 30% of NP unless the user has the item "CONT Recovery".
		// In this case, ALL of the donated NP will be refunded.
		if (!pUser->RobItem(ITEM_CONT_RECOVERY)
			&& !pUser->RobItem(ITEM_CONT_RESTORE_CERT))
		{
			nDonatedNP = (nDonatedNP * 30) / 100;
		}



		if (pUser->m_iLoyalty + nDonatedNP > LOYALTY_MAX)
			pUser->m_iLoyalty = LOYALTY_MAX;
		else
			pUser->m_iLoyalty += nDonatedNP;

		Packet result1(WIZ_LOYALTY_CHANGE, uint8(LOYALTY_NATIONAL_POINTS));
		result1 << pUser->m_iLoyalty << pUser->m_iLoyaltyMonthly
			<< uint32(0) // Clan donations(? Donations made by this user? For the clan overall?)
			<< uint32(0); // Premium NP(? Additional NP gained?)

		pUser->Send(&result1);
		return;
	}

	nDonatedNP = (nDonatedNP * 30) / 100;

	// For logged out players, we must update the player's national points in the database.
	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_REFUND_POINTS));
	result << strUserID << nDonatedNP;
	g_pMain->AddDatabaseRequest(result);
}

#pragma endregion 

#pragma region CKnights::Disband(CUser *pLeader /*= nullptr*/)

void CKnights::Disband(CUser *pLeader /*= nullptr*/)
{
	string clanNotice;
	g_pMain->GetServerResource(m_byFlag == ClanTypeTraining ? IDS_CLAN_DESTROY : IDS_KNIGHTS_DESTROY, 
		&clanNotice, m_strName.c_str());
	SendChat(clanNotice.c_str());

	std::vector<std::string> m_temparKnightsUser;

	m_arKnightsUser.m_lock.lock();
	foreach_stlmap_nolock (i, m_arKnightsUser)
	{
		m_temparKnightsUser.push_back(i->second->strUserName);
	}
	m_arKnightsUser.m_lock.unlock();

	foreach (i, m_temparKnightsUser)
	{
		CUser* pUser = g_pMain->GetUserPtr((*i), TYPE_CHARACTER);
		// If the user's logged in, handle the player data removal in-game.
		// It will be saved to the database when they log out.
		if (pUser != nullptr)
			RemoveUser(pUser);
		else
			RemoveUser((*i));
	}


	Packet result(WIZ_KNIGHTS_PROCESS, uint8(KNIGHTS_DESTROY));
	result << uint8(1);
	pLeader->Send(&result);

	g_pMain->m_KnightsArray.DeleteData(m_sIndex);
}

#pragma endregion 


#pragma region CKnights::SendChat(const char * format, ...)
void CKnights::SendChat(const char * format, ...)
{
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	Packet result;
	ChatPacket::Construct(&result, KNIGHTS_CHAT, buffer);
	Send(&result);
}

#pragma endregion 

#pragma region CKnights::SendChatAlliance(const char * format, ...)

void CKnights::SendChatAlliance(const char * format, ...)
{
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	Packet result;
	ChatPacket::Construct(&result, ALLIANCE_CHAT, buffer);
	Send(&result);
}

#pragma endregion 

#pragma region CKnights::Send(Packet *pkt)

void CKnights::Send(Packet *pkt)
{
	foreach_stlmap(user, m_arKnightsUser)
	{
		_KNIGHTS_USER *p = user->second;
		
		if(p == nullptr)
			continue;

		CUser* pUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);
		
		if(pUser)
			pUser->Send(pkt);
	}
}

#pragma endregion 

#pragma region CKnights::GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader)
/**
*
*/
uint16 CKnights::GetKnightsAllMembers(Packet & result, uint16 & pktSize, bool bClanLeader)
{
	uint16 count = 0;
	result.DByte();
	foreach_stlmap (i, m_arKnightsUser)
	{
		_KNIGHTS_USER *p = i->second;
		
		if(p == nullptr)
			continue;

		CUser* pTUser = g_pMain->GetUserPtr(p->strUserName, TYPE_CHARACTER);

		if (pTUser != nullptr)
		{
			result << pTUser->GetName() << pTUser->GetFame() << uint8(0) 
				<< pTUser->GetLevel() << pTUser->m_sClass << uint8(1) << p->strMemo
				<< uint32(0);
		}
		else 
		{// normally just clan leaders see this, but we can be generous now.
			result << p->strUserName << uint8(p->bFame) << uint8(0) 
				<< uint8(p->bLevel) << uint16(p->sClass) << uint8(0)
				<< p->strMemo << (int32(UNIXTIME) - p->nLastLogin) / 3600;
		}
		count++;
	}

	return count;
}

#pragma endregion 

#pragma region CKnights::~CKnights()

CKnights::~CKnights()
{
}

#pragma endregion 



