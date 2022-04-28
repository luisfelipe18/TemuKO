#include "stdafx.h"
#include "KingSystem.h"
#include "../shared/DateTime.h"
#include "Map.h"

#define MIN_LEVEL_VOTER 50
#define MIN_NP_VOTER 10000

#pragma region CKingSystem::CKingSystem()
/**
* @brief	The default constructor.
*/
CKingSystem::CKingSystem()
{
	m_byNation = 0;

	m_byType = ELECTION_TYPE_NO_TERM;
	m_sYear = 0;
	m_byMonth = m_byDay = m_byHour = m_byMinute = 0;

	m_byImType = 0;
	m_sImYear = 0;
	m_byImMonth = m_byImDay = m_byImHour = m_byImMinute = 0;

	m_byNoahEvent = m_byNoahEvent_Day = m_byNoahEvent_Hour = m_byNoahEvent_Minute = 0;
	m_sNoahEvent_Duration = 0;

	m_byExpEvent = m_byExpEvent_Day = m_byExpEvent_Hour = m_byExpEvent_Minute;
	m_sExpEvent_Duration = 0;

	m_nTribute = 0;
	m_nTerritoryTax = m_nNationalTreasury = 0;

	m_bSentFirstMessage = false;
	m_bElectionUnderProgress = false;
}
#pragma endregion

#pragma region CKingSystem::CheckKingTimer()
/**
* @brief	Handles timed events related to the King system.
*/
void CKingSystem::CheckKingTimer()
{
	if (m_bElectionUnderProgress == true)
		return;

	// Get the current time.
	uint8	bCurMonth = g_localTime.tm_mon + 1,
		bCurDay = g_localTime.tm_mday,
		bCurHour = g_localTime.tm_hour,
		bCurMinute = g_localTime.tm_min;

	// If there's an ongoing coin or XP event...
	if (m_byNoahEvent || m_byExpEvent)
		CheckSpecialEvent();

	switch (m_byType)
	{
	case ELECTION_TYPE_NO_TERM:
	{
		DateTime dt(m_sYear, m_byMonth, m_byDay, m_byHour, m_byMinute);

		// Nominations start two days before the election.
		dt.AddDays(-2);
		//printf("Secim 1 Gün %u Ay %u Saat %u Dakika %u \n", bCurDay, bCurMonth, bCurHour, bCurMinute);
		//printf("Secim 2 Gün %u Ay %u Saat %u Dakika %u \n", dt.GetDay(), dt.GetMonth(), dt.GetHour(), dt.GetMinute());
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			UpdateElectionStatus(ELECTION_TYPE_NOMINATION);
			g_pMain->SendFormattedResource(IDS_KING_RECOMMEND_TIME, m_byNation, false);
			ResetElectionLists();
			LoadRecommendList();
		}
	} break;

	case ELECTION_TYPE_NOMINATION:
	{
		DateTime dt(m_sYear, m_byMonth, m_byDay, m_byHour, m_byMinute);

		//Nominations last until one day and one hour before the scheduled election time.
		dt.AddDays(-1); dt.AddHours(-1);
		//printf("1 Secim 1 Gün %u Ay %u Saat %u Dakika %u \n", bCurDay, bCurMonth, bCurHour, bCurMinute);
		//printf("1 Secim 2 Gün %u Ay %u Saat %u Dakika %u \n", dt.GetDay(), dt.GetMonth(), dt.GetHour(), dt.GetMinute());
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			/*&& bCurMinute == dt.GetMinute()*/)
		{
			UpdateElectionStatus(ELECTION_TYPE_PRE_ELECTION);
			DetermineKingCandidaciesList();
			g_pMain->SendFormattedResource(IDS_KING_RECOMMEND_FINISH_TIME, m_byNation, false);
		}

		if (!(bCurMinute % 10) && !m_bSentFirstMessage)
		{
			m_bSentFirstMessage = true;
			g_pMain->SendFormattedResource(IDS_KING_PERIOD_OF_RECOMMEND_MESSAGE, m_byNation, true);
			break; // awkward, but official behaviour.
		}

		m_bSentFirstMessage = false;

	} break;

	// This state seems like it could be completely removed.
	// Leaving until the system's more complete, just in case.
	case ELECTION_TYPE_PRE_ELECTION:
	{
		DateTime dt(m_sYear, m_byMonth, m_byDay, m_byHour, m_byMinute);
		dt.AddDays(-1);
		//printf("2 Secim 1 Gün %u Ay %u Saat %u Dakika %u \n", bCurDay, bCurMonth, bCurHour, bCurMinute);
		//printf("2 Secim 2 Gün %u Ay %u Saat %u Dakika %u \n", dt.GetDay(), dt.GetMonth(), dt.GetHour(), dt.GetMinute());
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			UpdateElectionStatus(ELECTION_TYPE_ELECTION);
			g_pMain->SendFormattedResource(IDS_KING_ELECTION_TIME, m_byNation, false);
		}
	} break;

	case ELECTION_TYPE_ELECTION:
	{
		DateTime dt(m_sYear, m_byMonth, m_byDay, m_byHour, m_byMinute);

		// Elections last for a day.
		//dt.AddDays(1);
		//printf("3 Secim 1 Gün %u Ay %u Saat %u Dakika %u \n", bCurDay, bCurMonth, bCurHour, bCurMinute);
		//printf("3 Secim 2 Gün %u Ay %u Saat %u Dakika %u \n", dt.GetDay(), dt.GetMonth(), dt.GetHour(), dt.GetMinute());
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			UpdateElectionStatus(ELECTION_TYPE_TERM_ENDED);

			// Get Election results
			GetElectionResults();
			return;
		}

		if (!(bCurMinute % 10) && !m_bSentFirstMessage)
		{
			m_bSentFirstMessage = true;
			g_pMain->SendFormattedResource(IDS_KING_PERIOD_OF_ELECTION_MESSAGE, m_byNation, true);
			break; // awkward, but official behaviour.
		}
		m_bSentFirstMessage = false;
	} break;
	case ELECTION_TYPE_TERM_ENDED:
	{
		DateTime dt(m_sYear, m_byMonth, m_byDay, m_byHour, m_byMinute);

		// Elections last for a day.
		/*dt.AddDays(1);*/
		dt.AddMinutes(5);
		//printf("4 Secim 1 Gün %u Ay %u Saat %u Dakika %u \n", bCurDay, bCurMonth, bCurHour, bCurMinute);
		//printf("4 Secim 2 Gün %u Ay %u Saat %u Dakika %u \n", dt.GetDay(), dt.GetMonth(), dt.GetHour(), dt.GetMinute());

		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			AssignNewKingAndSenators();
			m_byMonth++;
			UpdateElectionStatus(ELECTION_TYPE_NO_TERM);
		}
	} break;
	}
#pragma region Impechment check
	switch (m_byImType)
	{
	case 1: // 47 hours after the impeachment time, call GetImpeachmentRequestResult()
	{
		DateTime dt(m_sImYear, m_byImMonth, m_byImDay, m_byImHour, m_byImMinute);
		dt.AddHours(47);
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			// GetImpeachmentRequestResult();
		}
	} break;

	case 2: // 2 days (48 hours) after the impeachment time, set the impeachment type to 3 
			// and send IDS_KING_IMPEACHMENT_ELECTION_MESSAGE as WAR_SYSTEM_CHAT
	{
		DateTime dt(m_sImYear, m_byImMonth, m_byImDay, m_byImHour, m_byImMinute);
		dt.AddDays(2);
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			m_byImType = 3;
			g_pMain->SendFormattedResource(IDS_KING_IMPEACHMENT_ELECTION_MESSAGE, m_byNation, false);
		}
	} break;

	case 3: // 3 days (72 hours) after the impeachment time, set the impeachment type to 4 
			// and call GetImpeachmentElectionResult()
	{
		DateTime dt(m_sImYear, m_byImMonth, m_byImDay, m_byImHour, m_byImMinute);
		dt.AddDays(3);
		if (bCurMonth == dt.GetMonth()
			&& bCurDay == dt.GetDay()
			&& bCurHour == dt.GetHour()
			&& bCurMinute == dt.GetMinute())
		{
			m_byImType = 4;
			// GetImpeachmentElectionResult();
		}
	} break;
	}
#pragma endregion 
}
#pragma endregion

#pragma region CKingSystem::AssignNewKingAndSenators()
/**
* @brief	Gets the election results.
*
*/
void CKingSystem::AssignNewKingAndSenators()
{
	m_strKingName = m_strNewKingName;

	int perc = int((float(m_KingVotes * 100) / float(m_TotalVotes)));
	if (m_TotalVotes == 0)
		perc = 100;
	else
		perc = int((float(m_KingVotes * 100) / float(m_TotalVotes)));

	// Announce the new elected king.
	g_pMain->SendFormattedResource(IDS_KING_ELECTION_RESULT_MESSAGE, m_byNation, false, perc, m_strKingName.c_str());
	return;
}
#pragma endregion

#pragma region CKingSystem::GetElectionResults()
/**
* @brief	Gets the election results.
*
*/
void CKingSystem::GetElectionResults()
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION) << uint8(KING_ELECTION_GET_VOTE_RESULTS)
		<< m_byNation << uint8(0);

	g_pMain->AddDatabaseRequest(result);
}
#pragma endregion

#pragma region CKingSystem::UpdateElectionStatus(uint8 byElectionStatus)
/**
* @brief	Updates the election status.
*
* @param	byElectionStatus	The election status.
*/
void CKingSystem::UpdateElectionStatus(uint8 byElectionStatus)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION) << uint8(KING_ELECTION_UPDATE_STATUS)
		<< m_byNation << byElectionStatus;
	m_byType = byElectionStatus;
	g_pMain->AddDatabaseRequest(result);
}
#pragma endregion

#pragma region  CKingSystem::UpdateElectionList(uint8 byElectionListType, bool bDeleteList, uint16 sClanID, std::string & strUserID, CUser * pUser /*= nullptr*/)
/**
* @brief	Updates the election list.
*
* @param	byElectionListType	Which list are we referring to?
* 								1 = ?
* 								2 = ?
* 								3 = senator list
* 								4 = voted for king list
* @param	bDeleteList		  	true to delete the list.
* @param	sClanID			  	Identifier for the clan.
* @param	strUserID		  	Identifier for the user.
* @param	pUser				The user making the request, if applicable.
*/
void CKingSystem::UpdateElectionList(uint8 byElectionListType, bool bDeleteList, uint16 sClanID, std::string & strUserID, CUser * pUser /*= nullptr*/)
{
	// byElectionListType:
	// 3 = senator
	Packet result(WIZ_KING, uint8(KING_ELECTION));

	result << uint8(KING_ELECTION) // special, looks redundant but implies these special opcodes
		<< uint8(KING_ELECTION_UPDATE_LIST) << m_byNation
		<< byElectionListType << bDeleteList
		<< sClanID << strUserID;

	g_pMain->AddDatabaseRequest(result, pUser);
}
#pragma endregion

#pragma region CKingSystem::CheckSpecialEvent()
/**
* @brief	Checks to see if a special (coin/XP) event should end.
*/
void CKingSystem::CheckSpecialEvent()
{
	// Get the current time.
	uint8	bCurDay = g_localTime.tm_mday,
		bCurHour = g_localTime.tm_hour,
		bCurMinute = g_localTime.tm_min;

	int16 sEventExpiry;

	// If there's an exp event ongoing...
	if (m_byExpEvent)
	{
		if (bCurDay == m_byExpEvent_Day)
			sEventExpiry = bCurMinute + 60 * (bCurHour - m_byExpEvent_Hour) - m_byExpEvent_Minute;
		else
			sEventExpiry = bCurMinute + 60 * (bCurHour - m_byExpEvent_Hour + 24) - m_byExpEvent_Minute;

		if (sEventExpiry > m_sExpEvent_Duration)
		{
			m_byExpEvent = 0;
			m_byExpEvent_Day = 0;
			m_byExpEvent_Hour = 0;
			m_byExpEvent_Minute = 0;
			m_sExpEvent_Duration = 0;

			// TODO: Update the KING_SYSTEM table to reset the stored event data there too.
			g_pMain->SendFormattedResource(IDS_KING_EXP_BONUS_EVENT_STOP, m_byNation, false);
		}
	}

	// If there's a coin event ongoing...
	if (m_byNoahEvent)
	{
		if (bCurDay == m_byNoahEvent_Day)
			sEventExpiry = bCurMinute + 60 * (bCurHour - m_byNoahEvent_Hour) - m_byNoahEvent_Minute;
		else
			sEventExpiry = bCurMinute + 60 * (bCurHour - m_byNoahEvent_Hour + 24) - m_byNoahEvent_Minute;

		if (sEventExpiry > m_sNoahEvent_Duration)
		{
			m_byNoahEvent = 0;
			m_byNoahEvent_Day = 0;
			m_byNoahEvent_Hour = 0;
			m_byNoahEvent_Minute = 0;
			m_sNoahEvent_Duration = 0;

			// TODO: Update the KING_SYSTEM table to reset the stored event data there too.
			g_pMain->SendFormattedResource(IDS_KING_NOAH_BONUS_EVENT_STOP, m_byNation, false);
		}
	}
}
#pragma endregion

#pragma region CKingSystem::LoadRecommendList()
/**
* @brief	Generates a list of the top 10 clan leaders eligible to nominate a King.
* 			NOTE: These players are senators.
*/
void CKingSystem::LoadRecommendList()
{
	Guard lock(m_KingSystemlock);
	m_top10ClanSet.clear();
	int count = 0;
	foreach_stlmap(itr, g_pMain->m_KnightsRatingArray[m_byNation - 1])
	{
		if (count == 10)
			break;

		CKnights *pKnights = g_pMain->GetClanPtr(itr->second->sClanID);

		if (pKnights == nullptr)
			continue;
		count++;

		_KING_ELECTION_LIST * pEntry = new _KING_ELECTION_LIST;
		pEntry->sKnights = pKnights->GetID();
		pEntry->nVotes = 0; // probably not needed as this is all handled in the procs as far as I know
		m_senatorList.insert(make_pair(pKnights->m_strChief, pEntry));

		// Add user as senator.
		UpdateElectionList(3, false, pKnights->GetID(), pKnights->m_strChief);

		// If the senator's online, we can send them an announcement
		// to tell them they can nominate a King.
		CUser * pUser = g_pMain->GetUserPtr(pKnights->m_strChief, TYPE_CHARACTER);
		if (pUser != nullptr)
		{
			Packet result;
			std::string notice;

			// %s can nominate a King
			g_pMain->GetServerResource(IDS_KING_RECOMMEND_REQUEST_MESSAGE, &notice, pKnights->m_strChief.c_str());

			// Wrap it in a "#### NOTICE : ####" block.
			g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &notice, notice.c_str());

			// Construct & send the chat/announcement packet
			ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &notice);
			pUser->Send(&result);
		}
		// add to our top 10 ranked clan set.
		m_top10ClanSet.insert(pKnights->GetID());
	}

}
#pragma endregion

#pragma region CKingSystem::DetermineKingCandidaciesList()
/**
* @brief	Generates a list of the King candidancies eligible to be a King.
* 			NOTE: These players are senators or the ones nominated by some
*				  senator.
*/
void CKingSystem::DetermineKingCandidaciesList()
{
	Guard lock(m_KingSystemlock);
	foreach(itr, m_senatorList)
		delete itr->second;
	m_senatorList.clear();
}
#pragma endregion

#pragma region CKingSystem::KingNotifyMessage(uint32 nResourceID, int byNation, ChatType chatType)
/**
* @brief	This sends the appropriate resource as a notice to the server (or to a particular
* 			user)
* 			Beyond initial reversing, this doesn't need to exist -- in fact, not even going to
* 			use it. It's just a temporary point of reference.
*
* @param	nResourceID	Identifier for the resource found in the SERVER_RESOURCE table.
* @param	byNation   	The nation to send the notice/announcement to.
* @param	chatType   	The chat type (notice/announcement).
*/
void CKingSystem::KingNotifyMessage(uint32 nResourceID, int byNation, ChatType chatType)
{
	std::string result;
	switch (nResourceID)
	{
		//	Resource ID (SERVER_RESOURCE)						// ID used internally (officially)
	case IDS_KING_RECOMMEND_TIME:							// 1 (none)
	case IDS_KING_RECOMMEND_FINISH_TIME:					// 2 (none)
	case IDS_KING_ELECTION_TIME:							// 3 (none)
	case IDS_KING_IMPEACHMENT_REQUEST_TIME:					// 4 (none)
	case IDS_KING_IMPEACHMENT_ELECTION_TIME:				// 5 (none)
	case IDS_KING_REIGN_TIME:								// 7 (none)
	case IDS_KING_KARUS_PRIZE_EVENT_MESSAGE:				// 11 (awarded %s %d coins)
	case IDS_KING_ELMO_PRIZE_EVENT_MESSAGE:					// 12 (awarded %s %d coins)
	case IDS_KING_KARUS_FUGITIVE_EVENT_MESSAGE_1:			// 13 (awarded %s %d coins -- probably inaccurate though, see below)
	case IDS_KING_ELMO_FUGITIVE_EVENT_MESSAGE_1:			// 14 (awarded %s %d coins -- probably inaccurate though, see below)
	case IDS_KING_FUGITIVE_EVENT_MESSAGE_2:					// 15 (%s killed %s and received %d coins as a reward)
	case IDS_KING_KARUS_WEATHER_FINE_EVENT:					// 16 (none)
	case IDS_KING_KARUS_WEATHER_RAIN_EVENT:					// 17 (none)
	case IDS_KING_KARUS_WEATHER_SNOW_EVENT:					// 18 (none)
	case IDS_KING_ELMO_WEATHER_FINE_EVENT:					// 19 (none)
	case IDS_KING_ELMO_WEATHER_RAIN_EVENT:					// 20 (none)
	case IDS_KING_ELMO_WEATHER_SNOW_EVENT:					// 21 (none)
	case IDS_KING_KARUS_NOAH_BONUS_EVENT:					// 22 (%d%% increased coin rate)
	case IDS_KING_KARUS_EXP_BONUS_EVENT:					// 23 (%d%% increased XP rate)
	case IDS_KING_ELMO_NOAH_BONUS_EVENT:					// 24 (%d%% increased coin rate)
	case IDS_KING_ELMO_EXP_BONUS_EVENT:						// 25 (%d%% increased XP rate)
	case IDS_KING_RECOMMEND_REQUEST_MESSAGE:				// 26 (%s can nominate a King)
	case IDS_KING_CANDIDACY_RECOMMEND_MESSAGE:				// 27 (%s has nominated %s as a King)
	case IDS_KING_PERIOD_OF_RECOMMEND_MESSAGE:				// 28 (none)
	case IDS_KING_PERIOD_OF_ELECTION_MESSAGE:				// 29 (none)
	case IDS_KING_ELECTION_RESULT_MESSAGE:					// 30 (%d%% of the vote was won by %s)
	case IDS_KING_EXP_BONUS_EVENT_STOP:						// 31 (none)
	case IDS_KING_NOAH_BONUS_EVENT_STOP:					// 32 (none)
	case IDS_KING_IMPEACHMENT_REQUEST_MESSAGE:				// 33 (none)
	case IDS_KING_IMPEACHMENT_PASS_MESSAGE:					// 34 (none)
	case IDS_KING_IMPEACHMENT_REJECT_MESSAGE:				// 35 (none)
	case IDS_KING_IMPEACHMENT_ELECTION_MESSAGE:				// 36 (none)
	case IDS_KING_IMPEACHMENT_ELECTION_YES_RESULT_MESSAGE:	// 37 (none)
	case IDS_KING_IMPEACHMENT_ELECTION_NO_RESULT_MESSAGE:	// 38 (none)
		break;
	}
}
#pragma endregion

#pragma region CKingSystem::PacketProcess(CUser * pUser, Packet & pkt)
/**
* @brief	Wrapper for the King system's packet handler.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::PacketProcess(CUser * pUser, Packet & pkt)
{
	if (pUser == nullptr)
		return;

	// ... onwards, to official-like code.
	CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(pUser->GetNation());
	if (pKingSystem != nullptr)
		pKingSystem->KingPacketProcess(pUser, pkt);
}
#pragma endregion

#pragma region CKingSystem::KingPacketProcess(CUser * pUser, Packet & pkt)
/**
* @brief	The real packet handler for the King system.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::KingPacketProcess(CUser * pUser, Packet & pkt)
{
	uint8 bOpcode = pkt.read<uint8>();
	switch (bOpcode)
	{
	case KING_ELECTION:
		ElectionSystem(pUser, pkt);
		break;

	case KING_IMPEACHMENT:
		ImpeachmentSystem(pUser, pkt);
		break;

	case KING_TAX:
		KingTaxSystem(pUser, pkt);
		break;

	case KING_EVENT:
		KingSpecialEvent(pUser, pkt);
		break;

	case KING_NATION_INTRO:
		KingNationIntro(pUser, pkt);
		break;
	default:
		TRACE("Unknown King packet %X\n", bOpcode);
		printf("Unknown King packet %X\n", bOpcode);
		break;
	}
}
#pragma endregion

#pragma region CKingSystem::ElectionSystem(CUser * pUser, Packet & pkt)
/**
* @brief	Election system.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ElectionSystem(CUser * pUser, Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case KING_ELECTION_SCHEDULE:
		ElectionScheduleConfirmation(pUser, pkt);
		break;

	case KING_ELECTION_NOMINATE:
		CandidacyRecommend(pUser, pkt);
		break;

	case KING_ELECTION_NOTICE_BOARD:
		CandidacyNoticeBoard(pUser, pkt);
		break;

	case KING_ELECTION_POLL:
		ElectionPoll(pUser, pkt);
		break;

	case KING_ELECTION_RESIGN:
		CandidacyResign(pUser, pkt);
		break;
	}
}
#pragma endregion

#pragma region CKingSystem::ElectionScheduleConfirmation(CUser * pUser, Packet & pkt)
/**
* @brief	"Check election day" button at the election NPC
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ElectionScheduleConfirmation(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION_SCHEDULE);

	switch (m_byImType)
	{
		// No impeachment, send election date.
	case 0:
	{
		// Client expects month as 1,12 (tm_mon is 0,11)
		uint8 byElectionMonth = g_localTime.tm_mon + 1;

		/* When's the next election? */
		// If we've passed the election date, we need next month's election.
		// (NOTE: this is official behaviour; it disregards the month set in the table)
		if (g_localTime.tm_mday > m_byDay)
		{
			// Next month is January? Make it so.
			++byElectionMonth;
			while (byElectionMonth > 12)
				byElectionMonth -= 12;
		}

		result << uint8(1) // election type
			<< byElectionMonth
			<< m_byDay << m_byHour << m_byMinute;
	} break;

	// Last scheduled impeachment?
	case 1:
	{
		result << uint8(3)
			<< m_byImMonth
			<< m_byImDay << m_byImHour << m_byImMinute;
	} break;

	// Next impeachment?
	case 3:
	{
		// This should not be necessary, but will leave.
		uint8 byImpeachmentMonth = m_byImMonth;
		while (byImpeachmentMonth > 12)
			m_byImMonth -= 12;

		result << uint8(2)
			<< byImpeachmentMonth
			<< m_byImDay << m_byImHour << m_byImMinute;
	} break;
	}

	pUser->Send(&result);
}
#pragma endregion

#pragma region CKingSystem::CandidacyRecommend(CUser * pUser, Packet & pkt) 
/**
* @brief	Handles King candidacy recommendations by
* 			leaders of top 10 clans.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::CandidacyRecommend(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	std::string strUserID;
	pkt.SByte();
	pkt >> strUserID;
	if (strUserID.empty() || strUserID.length() > MAX_ID_SIZE)
		return;

	result << uint8(KING_ELECTION_NOMINATE);

	// Make sure it's nomination time.
	if (m_byType != ELECTION_TYPE_NOMINATION)
	{
		result << int16(-2);
		pUser->Send(&result);
		return;
	}

	Guard lock(m_KingSystemlock);
	// Make sure the user nominating a King is a clan leader
	if (!pUser->isClanLeader()
		// ... of a top 10 clan.
		|| m_senatorList.find(pUser->GetName()) == m_senatorList.end()
		// ... and they haven't resigned their candidacy.
		|| m_resignedCandidateList.find(pUser->GetName()) != m_resignedCandidateList.end())
	{
		result << int16(-3); // You do not have authority.
		pUser->Send(&result);
		return;
	}

	foreach(itr, m_nominationList)
	{
		if (STRCASECMP(itr->second->strNominator.c_str(), pUser->GetName().c_str()) == 0)
		{
			result << int16(-4); // You can only nominate a user from the same nation.
			pUser->Send(&result);
			return;
		}
		else if (STRCASECMP(itr->second->strNominee.c_str(), strUserID.c_str()) == 0)
		{
			result << int16(-5); //This user has already been nominated.
			pUser->Send(&result);
			return;
		}
	}

	// Send request to database.
	result << strUserID;
	g_pMain->AddDatabaseRequest(result, pUser);
}
#pragma endregion

#pragma region CKingSystem::InsertNominee(std::string & strNominator, std::string & strNominee, uint16 sNomineeKnights)
/**
* @brief	Inserts the nominated candidate to the election list.
*
* @param	strNominee	The nominee.
*/
void CKingSystem::InsertNominee(std::string & strNominator, std::string & strNominee, uint16 sNomineeKnights)
{
	Guard lock(m_KingSystemlock);

	// All nominees must be senators.
	// No need to create duplicate data, so just find & reuse the same data.
	KingElectionList::iterator senatorItr = m_senatorList.find(strNominator);
	if (senatorItr == m_senatorList.end())
		return;

	// Now see if the candidate already exists in the list.
	// If they don't, no need to do anything.
	KingElectionList::iterator candidateItr = m_candidateList.find(strNominee);
	if (candidateItr != m_candidateList.end())
		return;

	// Now see if the nomination already exists in the list.
	// If they don't, no need to do anything.
	KingNominationList::iterator nominationItr = m_nominationList.find(strNominee);
	if (nominationItr != m_nominationList.end())
		return;

	// Copy the information we need from our senator list entry
	// NOTE: This is fairly dumb, so we should work this out when the system's functional and 
	// we're open to straying from official table designs.
	/*_KING_ELECTION_LIST * pEntry = new _KING_ELECTION_LIST;
	memcpy(pEntry, senatorItr->second, sizeof(_KING_ELECTION_LIST));*/
	_KING_ELECTION_LIST * pEntry = new _KING_ELECTION_LIST;
	pEntry->nVotes = 0;
	pEntry->sKnights = sNomineeKnights;
	m_candidateList.insert(make_pair(strNominee, pEntry));

	_KING_NOMINATION_LIST * pEntryNomination = new _KING_NOMINATION_LIST;
	pEntryNomination->strNominator = strNominator;
	pEntryNomination->strNominee = strNominee;
	m_nominationList.insert(make_pair(strNominee, pEntryNomination));

	delete senatorItr->second;
	m_senatorList.erase(senatorItr);

	// Let the whole nation know, who we nominated!
	g_pMain->SendFormattedResource(IDS_KING_CANDIDACY_RECOMMEND_MESSAGE, m_byNation, false, strNominator.c_str(), strNominee.c_str());

}
#pragma endregion

#pragma region CKingSystem::CandidacyNoticeBoard(CUser * pUser, Packet & pkt)
/**
* @brief	Candidacy notice board system.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::CandidacyNoticeBoard(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	uint8 opcode = pkt.read<uint8>();
	bool bSuccess = false;

	result << uint8(KING_ELECTION_NOTICE_BOARD) << opcode;

	switch (opcode)
	{
		// Write to the candidacy noticeboard
	case KING_CANDIDACY_BOARD_WRITE:
	{
		if (m_byType != ELECTION_TYPE_NOMINATION
			&& m_byType != ELECTION_TYPE_PRE_ELECTION
			&& m_byType != ELECTION_TYPE_ELECTION)
		{
			result << int16(-1);
			pUser->Send(&result);
			return;
		}

		std::string strNotice;
		pkt >> strNotice;

		// The official max length of invalid notices is 480 bytes.
		// The size of the column is 1024 bytes. Possibly this limit was
		// imposed purely for the sake of the shared memory queue?
		if (strNotice.empty() || strNotice.length() > 480)
		{
			result << int16(-2);
			pUser->Send(&result);
			return;
		}

		Guard lock(m_KingSystemlock);
		// Check to make sure the user's in the candidate list.
		KingElectionList::iterator candidateItr = m_candidateList.find(pUser->m_strUserID);
		if (candidateItr == m_candidateList.end())
		{
			result << int16(-3);
			pUser->Send(&result);
			return;
		}

		// Check to make sure the user's in the nomination list.
		KingNominationList::iterator nominationItr = m_nominationList.find(pUser->m_strUserID);
		if (nominationItr == m_nominationList.end())
		{
			result << int16(-3);
			pUser->Send(&result);
			return;
		}

		// Check to make sure the user's in the candidacy list.
		KingCandidacyNoticeBoardMap::iterator itr = m_noticeBoardMap.find(pUser->m_strUserID);
		if (itr == m_noticeBoardMap.end())
			itr = m_noticeBoardMap.insert(make_pair(pUser->m_strUserID, strNotice)).first;

		// Update the noticeboard.
		itr->second = strNotice;
		// Update the user.
		result << int16(1);
		pUser->Send(&result);

		// Update the database.
		Packet resultdb(WIZ_KING, uint8(KING_ELECTION));
		resultdb << uint8(KING_ELECTION_NOTICE_BOARD) << opcode;
		resultdb << strNotice;
		g_pMain->AddDatabaseRequest(resultdb, pUser);
	} return;

	// Read from the candidacy noticeboard
	case KING_CANDIDACY_BOARD_READ:
	{
		if (m_byType != ELECTION_TYPE_NOMINATION
			&& m_byType != ELECTION_TYPE_PRE_ELECTION
			&& m_byType != ELECTION_TYPE_ELECTION)
		{
			result << int16(-1);
			pUser->Send(&result);
			return;
		}

		pkt >> opcode;
		result << opcode;

		// List candidates
		if (opcode == 1)
		{
			Guard lock(m_KingSystemlock);
			result << int16(1) // success
				<< uint8(m_noticeBoardMap.size());

			result.SByte();
			foreach(itr, m_noticeBoardMap)
				result << itr->first;
		}
		else if (opcode == 2)
		{
			std::string strCandidate;
			pkt.SByte();
			pkt >> strCandidate;

			if (strCandidate.empty() || strCandidate.length() > MAX_ID_SIZE)
				return;

			Guard lock(m_KingSystemlock);
			// Does this candidate actually exist in the list?
			KingCandidacyNoticeBoardMap::iterator itr = m_noticeBoardMap.find(strCandidate);
			if (itr == m_noticeBoardMap.end()
				// and is the message actually set?
				|| itr->second.empty())
			{
				result << int16(-2);
			}
			else
			{
				result.DByte();
				result << int16(1) // success
					<< itr->second;
			}
		}

		pUser->Send(&result);
	} return;

	case 4:
		if (m_byType == ELECTION_TYPE_NOMINATION
			|| m_byType == ELECTION_TYPE_PRE_ELECTION
			|| m_byType == ELECTION_TYPE_ELECTION)
		{
			// TODO: Find user in (candidate list?), if not found we can break out of here and error.
			if (1 == 2)
				break;

			// 
			bSuccess = true;
		}
		break;

	case 5:
		if (m_byType == ELECTION_TYPE_NOMINATION
			|| m_byType == ELECTION_TYPE_PRE_ELECTION
			|| m_byType == ELECTION_TYPE_ELECTION)
			bSuccess = true;
		break;


	default:
		return;
	}

	result << int16(bSuccess ? 1 : -1);
	if (opcode == 4)
		result << bSuccess;

	pUser->Send(&result);
}
#pragma endregion

#pragma region CKingSystem::ElectionPoll(CUser * pUser, Packet & pkt)
/**
* @brief	Election poll.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ElectionPoll(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	uint8 opcode = pkt.read<uint8>();

	result << uint8(KING_ELECTION_POLL) << opcode;

	// Make sure player's trying to vote during the
	// election stage.
	if (m_byType != ELECTION_TYPE_ELECTION)
	{
		result << int16(-1);
		pUser->Send(&result);
		return;
	}

	Guard lock(m_KingSystemlock);
	switch (opcode)
	{
		// Show candidate list
	case 1:
	{
		uint8 count = (uint8)m_candidateList.size();
		result << uint16(1) << count;
		result.SByte();
		int counter = 1;
		foreach(itr, m_candidateList)
		{
			CKnights * pKnights = g_pMain->GetClanPtr(itr->second->sKnights);
			result << uint8(counter++) << itr->first; // candidate's name
			if (pKnights != nullptr)
				result << pKnights->m_strName; // clan name
			else
				result << uint8(0); // no clan name
		}
		pUser->Send(&result);
	} break;

	// Vote for candidate
	case 2:
	{
		std::string strCandidate;
		pkt.SByte();
		pkt >> strCandidate;
		if (strCandidate.empty() || strCandidate.length() > MAX_ID_SIZE)
			return;

		// Candidate voted for isn't in list...
		KingElectionList::iterator itr = m_candidateList.find(strCandidate);
		if (itr == m_candidateList.end())
		{
			result << int16(-2);
			pUser->Send(&result);
			return;
		}

		// User's level is too low to vote.
		if (pUser->GetLevel() < MIN_LEVEL_VOTER)
		{
			result << int16(-4);
			pUser->Send(&result);
			return;
		}

		VoteForKing(pUser->GetAccountName(), pUser->GetName(), strCandidate);

	} break;
	}
}
#pragma endregion

#pragma region CKingSystem::VoteForKing(std::string & strVoterAccountID, std::string & strVoterUserID, std::string & strNominee)
/**
* @brief	Election poll.
*
* @param	strVoterAccountID	The user  account ID sending the packet.
* @param	strVoterUserID  	The user ID sending the packet.
* @param	strNominee  		The King Candidacy
*/
void CKingSystem::VoteForKing(std::string & strVoterAccountID, std::string & strVoterUserID, std::string & strNominee)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION) // special, looks redundant but implies these special opcodes
		<< uint8(KING_ELECTION_VOTE) << m_byNation << uint8(0)
		<< strVoterAccountID << strVoterUserID << strNominee;

	g_pMain->AddDatabaseRequest(result);
}
#pragma endregion

#pragma region CKingSystem::ShowElectionList(CUser * pUser)
/**
* @brief	Election poll.
*
* @param	pUser	The user sending the packet.
*/
void CKingSystem::ShowElectionList(CUser * pUser)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION_POLL) << uint8(1);

	Guard lock(m_KingSystemlock);
	uint8 count = (uint8)m_candidateList.size();
	result << uint16(1) << count;
	result.SByte();
	int counter = 1;
	foreach(itr, m_candidateList)
	{
		CKnights * pKnights = g_pMain->GetClanPtr(itr->second->sKnights);
		result << uint8(counter++) << itr->first; // candidate's name
		if (pKnights != nullptr)
			result << pKnights->m_strName; // clan name
		else
			result << uint8(0); // no clan name
	}
	pUser->Send(&result);
}
#pragma endregion

#pragma region CKingSystem::CandidacyResign(CUser * pUser, Packet & pkt) 
/**
* @brief	Handles candidate resignations.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::CandidacyResign(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_ELECTION));
	result << uint8(KING_ELECTION_RESIGN);

	// We can only submit a resignation if we're in the nomination stage.
	if (m_byType != ELECTION_TYPE_NOMINATION)
	{
		result << int16(-2);
		pUser->Send(&result);
		return;
	}

	Guard lock(m_KingSystemlock);
	KingElectionList::iterator candidateItr = m_candidateList.find(pUser->m_strUserID);

	// Do we even exist in the candidate list?
	if (candidateItr == m_candidateList.end() || candidateItr->second == nullptr)
	{
		result << int16(-3);
		pUser->Send(&result);
		return;
	}

	KingNominationList::iterator nominationItr = m_nominationList.find(pUser->m_strUserID);

	// Do we even exist in the nomination list?
	if (nominationItr == m_nominationList.end() || nominationItr->second == nullptr)
	{
		result << int16(-3);
		pUser->Send(&result);
		return;
	}

	// Add user to our resigned candidates list so they can't be re-nominated.
	m_resignedCandidateList.insert(nominationItr->second->strNominee);

	UpdateElectionList(4, // elected King
		true, // remove ourselves from the list
		candidateItr->second->sKnights,
		pUser->m_strUserID,
		pUser);

	delete candidateItr->second;
	m_candidateList.erase(candidateItr);

	delete nominationItr->second;
	m_nominationList.erase(nominationItr);
}
#pragma endregion

#pragma region CKingSystem::ImpeachmentSystem(CUser * pUser, Packet & pkt)
/**
* @brief	Impeachment system.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ImpeachmentSystem(CUser * pUser, Packet & pkt)
{
	switch (pkt.read<uint8>())
	{
	case KING_IMPEACHMENT_REQUEST:
		ImpeachmentRequest(pUser, pkt);
		break;

	case KING_IMPEACHMENT_REQUEST_ELECT:
		ImpeachmentRequestElect(pUser, pkt);
		break;

	case KING_IMPEACHMENT_LIST:
		ImpeachmentList(pUser, pkt);
		break;

	case KING_IMPEACHMENT_ELECT:
		ImpeachmentElect(pUser, pkt);
		break;

	case KING_IMPEACHMENT_REQUEST_UI_OPEN:
		ImpeachmentRequestUiOpen(pUser, pkt);
		break;

	case KING_IMPEACHMENT_ELECTION_UI_OPEN:
		ImpeachmentElectionUiOpen(pUser, pkt);
		break;
	}
}
#pragma endregion

#pragma region Impechment System

void CKingSystem::ImpeachmentRequest(CUser * pUser, Packet & pkt)
{
}
void CKingSystem::ImpeachmentRequestElect(CUser * pUser, Packet & pkt)
{
}
void CKingSystem::ImpeachmentList(CUser * pUser, Packet & pkt)
{
}
void CKingSystem::ImpeachmentElect(CUser * pUser, Packet & pkt)
{
}

#pragma endregion

#pragma region CKingSystem::ImpeachmentRequestUiOpen(CUser * pUser, Packet & pkt) 
/**
* @brief	Attempt to open the impeachment request UI.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ImpeachmentRequestUiOpen(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_IMPEACHMENT));
	result << uint8(KING_IMPEACHMENT_REQUEST_UI_OPEN);

	// Not able to make an impeachment request right now.
	if (m_byImType != 1)
		result << int16(-1);
	// If they're not an senator...
	else if (pUser->m_bRank != 2)
		result << int16(-2);
	// Able to make an impeachment request.
	else
		result << int16(1);

	pUser->Send(&result);
}
#pragma endregion

#pragma region CKingSystem::ImpeachmentElectionUiOpen(CUser * pUser, Packet & pkt)
/**
* @brief	Attempt to open the impeachment election UI.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::ImpeachmentElectionUiOpen(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_IMPEACHMENT));

	// If it's not the impeachment's election stage, send -1 as the error code
	// otherwise, send 1 for success.
	result << uint8(KING_IMPEACHMENT_ELECTION_UI_OPEN)
		<< int16(m_byImType != 3 ? -1 : 1);

	pUser->Send(&result);
}
#pragma endregion

#pragma region CKingSystem::KingTaxSystem(CUser * pUser, Packet & pkt)
/**
* @brief	King tax system.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::KingTaxSystem(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_TAX));
	uint8 bOpcode = pkt.read<uint8>();
	result << bOpcode;

	// If you're not a King, you shouldn't have access to this command.
	if (!pUser->isKing())
	{
		result << int16(-1);
		pUser->Send(&result);
		return;
	}

	switch (bOpcode)
	{
		// Collect King's fund
	case 2:
		pUser->GoldGain(m_nTerritoryTax);
		m_nTerritoryTax = 0;
		result << m_nTerritoryTax << m_byNation;
		g_pMain->AddDatabaseRequest(result, pUser);
		break;

		// Lookup the tariff
	case 3:
	{
		C3DMap * pMap = g_pMain->GetZoneByID(m_byNation);
		if (pMap == nullptr)
			return;

		result << int16(1) << pMap->GetTariff() - 10;
		pUser->Send(&result);
	} break;

	// Update the tariff
	case 4:
	{
		C3DMap * pMap = g_pMain->GetZoneByID(m_byNation);
		uint8 byTerritoryTariff = pkt.read<uint8>();

		// Map doesn't exist, or invalid tariff amount
		if (pMap == nullptr
			|| byTerritoryTariff > 10)
		{
			result << int16(-2);
			pUser->Send(&result);
			return;
		}

		// Update the tariff
		pMap->SetTariff(byTerritoryTariff + 10);

		// Let all users in your nation know.
		result << int16(1) << byTerritoryTariff << m_byNation;
		g_pMain->Send_All(&result, nullptr, m_byNation);

		m_nTerritoryTariff = byTerritoryTariff;

		// Update the database
		g_pMain->AddDatabaseRequest(result, pUser);
	} break;

	// King's scepter / "unnecessary translation"
	case 7:
	{
		if (pUser->CheckExistItem(KING_SCEPTER))
		{
			result << int16(-1);
		}
		else if (pUser->FindSlotForItem(KING_SCEPTER) < 0)
		{
			result << int16(-2);
		}
		else
		{
			pUser->GiveItem(KING_SCEPTER);
			pUser->GiveItem(KING_SCEPTER);
			result << int16(1);
		}
		pUser->Send(&result);
	} break;
	}
}
#pragma endregion

#pragma region CKingSystem::KingSpecialEvent(CUser * pUser, Packet & pkt)
/**
* @brief	Handles commands accessible to the King.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::KingSpecialEvent(CUser * pUser, Packet & pkt)
{
	Packet result(WIZ_KING, uint8(KING_EVENT));
	uint8 opcode = pkt.read<uint8>();
	result << opcode;

	if (!pUser->isKing())
	{
		result << int16(-1);
		pUser->Send(&result);
		return;
	}

	switch (opcode)
	{
	case KING_EVENT_NOAH: // Noah event
	{
		uint8 bAmount = pkt.read<uint8>();
		if (bAmount < 1 || bAmount > 3)
			return;

		Guard lock(m_KingSystemlock);
		uint32 nCost = 30000000 * bAmount;
		if (nCost > m_nNationalTreasury)
		{
			result << int16(-3);
			pUser->Send(&result);
			return;
		}

		m_nNationalTreasury -= nCost;

		m_byNoahEvent = bAmount;
		m_byNoahEvent_Day = g_localTime.tm_mday;
		m_byNoahEvent_Hour = g_localTime.tm_hour;
		m_byNoahEvent_Minute = g_localTime.tm_min;

		m_sNoahEvent_Duration = 30; // event expires in 30 minutes

									// %d%% increased coin rate 
		g_pMain->SendFormattedResource(m_byNation == KARUS ? IDS_KING_KARUS_NOAH_BONUS_EVENT : IDS_KING_ELMO_NOAH_BONUS_EVENT,
			m_byNation, false, bAmount);

		// TODO: Update other servers via UDP
		// TODO: Update the AI server

		// Update the database
		result << m_byNation << bAmount << m_byNoahEvent_Day << m_byNoahEvent_Hour << m_byNoahEvent_Minute << m_sNoahEvent_Duration << m_nNationalTreasury;
		g_pMain->AddDatabaseRequest(result);
	} break;

	case KING_EVENT_EXP: // EXP event
	{
		uint8 bAmount = pkt.read<uint8>();
		if (bAmount != 10 && bAmount != 30 && bAmount != 50)
			return;
		Guard lock(m_KingSystemlock);
		uint32 nCost = 30000000 * bAmount;
		if (nCost > m_nNationalTreasury)
		{
			result << int16(-3);
			pUser->Send(&result);
			return;
		}

		m_nNationalTreasury -= nCost;

		m_byExpEvent = bAmount;
		m_byExpEvent_Day = g_localTime.tm_mday;
		m_byExpEvent_Hour = g_localTime.tm_hour;
		m_byExpEvent_Minute = g_localTime.tm_min;

		m_sExpEvent_Duration = 30; // event expires in 30 minutes

								   // %d%% increased coin rate 
		g_pMain->SendFormattedResource(m_byNation == KARUS ? IDS_KING_KARUS_EXP_BONUS_EVENT : IDS_KING_ELMO_EXP_BONUS_EVENT,
			m_byNation, false, bAmount);

		// TODO: Update other servers via UDP
		// TODO: Update the AI server

		// Update the database
		result << m_byNation << bAmount << m_byExpEvent_Day << m_byExpEvent_Hour << m_byExpEvent_Minute << m_sExpEvent_Duration;
		g_pMain->AddDatabaseRequest(result);
	} break;

	case KING_EVENT_PRIZE:
	{
		uint32 nCoins;
		std::string strUserID;
		pkt.SByte();
		pkt >> nCoins >> strUserID;

		// If the user submitted invalid input, chances are 
		// the coins will end up 0. We can safely ignore it.
		if (nCoins == 0)
			return;

		CUser * pTUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pTUser == nullptr	// this session check isn't official behaviour
								// as they try to handle offline users -
								// note the 'try' (it doesn't work properly)...
			|| strUserID.empty() || strUserID.length() > MAX_ID_SIZE)
		{
			result << int16(-2);
			pUser->Send(&result);
			return;
		}

		Guard lock(m_KingSystemlock);
		if (nCoins > m_nNationalTreasury)
		{
			result << int16(-4);
			pUser->Send(&result);
			return;
		}

		m_nNationalTreasury -= nCoins;
		pTUser->GoldGain(nCoins);

		// (awarded %s %d coins)
		g_pMain->SendFormattedResource(m_byNation == KARUS ? IDS_KING_KARUS_PRIZE_EVENT_MESSAGE : IDS_KING_ELMO_PRIZE_EVENT_MESSAGE,
			m_byNation, false, pTUser->m_strUserID.c_str(), nCoins);

		// TODO: Update other servers via UDP

		// Update the database
		result << m_byNation << m_nNationalTreasury << strUserID;
		g_pMain->AddDatabaseRequest(result);
	} break;

	case KING_EVENT_FUGITIVE: // not sure what this is exactly, but it seems to work pretty much the same as the /prize command
		break;

	case KING_EVENT_WEATHER: // Weather
	{
		uint8 bType, bAmount;
		pkt >> bType >> bAmount;
		if (bAmount == 0 || bAmount > 100
			|| bType == 0 || bType > WEATHER_SNOW)
			return;

		Guard lock(m_KingSystemlock);

		if (m_nNationalTreasury < 100000)
		{
			result << int16(-3);
			pUser->Send(&result);
			return;
		}

		m_nNationalTreasury -= 100000;

		g_pMain->m_byKingWeatherEvent = 1;
		g_pMain->m_byKingWeatherEvent_Day = g_localTime.tm_mday;
		g_pMain->m_byKingWeatherEvent_Hour = g_localTime.tm_hour;
		g_pMain->m_byKingWeatherEvent_Minute = g_localTime.tm_min;

		g_pMain->m_byWeather = bType;
		g_pMain->m_sWeatherAmount = bAmount;

		g_pMain->UpdateWeather();

		// TODO: Update other servers via UDP

		// Get the resource ID, which differs per nation and weather type.
		// This works because they're sequential.
		uint32 nResourceID =
			(m_byNation == KARUS
				? IDS_KING_KARUS_WEATHER_FINE_EVENT + (bType - 1)
				: IDS_KING_ELMO_WEATHER_FINE_EVENT + (bType - 1));

		g_pMain->SendFormattedResource(nResourceID, m_byNation, false);
	} break;

	case KING_EVENT_NOTICE:
	{
		std::string strMessage;
		pkt.SByte();
		pkt >> strMessage;
		if (strMessage.empty() || strMessage.length() > 256)
			return;

		result.SByte();
		result << uint8(1) << uint16(1) << strMessage;
		g_pMain->Send_All(&result, nullptr, m_byNation);

		DateTime time;
		g_pMain->WriteChatLogFile(string_format("[ KING - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), pUser->GetName().c_str(), strMessage.c_str(), pUser->GetZoneID(), uint16(pUser->GetX()), uint16(pUser->GetZ())));
	} break;
	}
}
#pragma endregion

#pragma region CKingSystem::KingNationIntro(CUser * pUser, Packet & pkt)
/**
* @brief	Handles commands accessible to the King.
*
* @param	pUser	The user sending the packet.
* @param	pkt  	The packet.
*/
void CKingSystem::KingNationIntro(CUser * pUser, Packet & pkt)
{
	if (pUser == nullptr || !pUser->isInGame() || !pUser->isKing())
		return;
	uint8 bType = pkt.read<uint8>();

	Packet result(WIZ_KING, uint8(KING_NATION_INTRO));
	result << bType;

	switch (bType)
	{
	case 1: // request change
		pUser->Send(&result);
		break;
	case 2: // update nation intro
	{
		std::string strMessage;
		pkt >> strMessage;
		result << uint8(1);
		pUser->Send(&result);

		Packet db(WIZ_KING, uint8(KING_NATION_INTRO));
		db << g_pMain->m_nServerNo << pUser->GetNation() << pUser->GetName() << strMessage;
		g_pMain->AddDatabaseRequest(db, pUser);
	}
	break;
	default:
		printf("KingNationIntro unhandled packets %d \n",bType);
		break;
	}
}

#pragma endregion

#pragma region CKingSystem::ResetElectionLists()
/**
* @brief
*/
void CKingSystem::ResetElectionLists()
{
	Guard lock(m_KingSystemlock);

	foreach(itr, m_senatorList)
		delete itr->second;
	m_senatorList.clear();

	foreach(itr, m_nominationList)
		delete itr->second;
	m_nominationList.clear();

	foreach(itr, m_candidateList)
		delete itr->second;
	m_candidateList.clear();

	m_resignedCandidateList.clear();
}
#pragma endregion

#pragma region CKingSystem::~CKingSystem()
/**
* @brief The destructor.
*/
CKingSystem::~CKingSystem()
{
	ResetElectionLists();
}
#pragma endregion