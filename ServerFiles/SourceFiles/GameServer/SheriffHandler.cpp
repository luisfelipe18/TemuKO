#include "stdafx.h"
#include "../shared/DateTime.h"
#include "DBAgent.h"
#include <time.h>
#include <sstream>

void CUser::KingsInspectorList()
{
	Packet result(WIZ_REPORT, uint8(KingsInspector));
	Send(&result);
}

void CUser::SheriffVote(Packet & pkt)
{
	std::string strReportedName, strReason;
	uint8 opcode = pkt.read<uint8>();
	switch (opcode)
	{
	case ReportSuccess:
		{
			Packet result(WIZ_REPORT, uint8(ReportSuccess));
			uint8 ReturnValue = 1;

			pkt.SByte();
			pkt >> strReportedName >> strReason;
			
			if (strReportedName.empty() || strReason.empty() || strReason.length() > MAX_ID_REPORT)
			{
				ReturnValue = QuestionNotAnswered;
				result << ReturnValue;
				Send(&result);
				return;
			}

			auto * pTargetUser = g_pMain->GetUserPtr(strReportedName, TYPE_CHARACTER);
			if (pTargetUser == nullptr || pTargetUser->GetID() > NPC_BAND || !pTargetUser->isInGame())
			{
				ReturnValue = ReportFailed;
				result << ReturnValue;
				Send(&result);
				return;
			}
			
			if (pTargetUser->isKing() || pTargetUser->isGM())
			{
				ReturnValue = ReportFailed;
				result << ReturnValue;
				Send(&result);
				return;
			}

			if (!isKing() && !isGM())
			{
				ReturnValue = ReportFailed;
				result << ReturnValue;
				Send(&result);
				return;
			}

			auto itr = g_pMain->m_SheriffArray.find(strReportedName);
			if (itr == g_pMain->m_SheriffArray.end())
			{
				tm Days;
				std::stringstream Date;
				time_t timer = time(0);
				Days = *localtime(&timer);
				Date << Days.tm_year - 100 << "/" << 1 + Days.tm_mon << "/" << Days.tm_mday << " " 
					<< Days.tm_hour << ":" << Days.tm_min << ":" << Days.tm_sec;

				auto pSheriffReport = new _SHERIFF_STUFF();
				pSheriffReport->reportedName = strReportedName;
				pSheriffReport->ReportSheriffName = GetName();
				pSheriffReport->crime = strReason;
				pSheriffReport->m_VoteYes++;
				pSheriffReport->SheriffYesA = GetName();
				pSheriffReport->m_Date = Date.str().c_str();

				g_pMain->m_SheriffArray.insert(std::make_pair(strReportedName, pSheriffReport));
				g_DBAgent.SaveSheriffReports(pSheriffReport->reportedName, pSheriffReport->ReportSheriffName, pSheriffReport->crime, pSheriffReport->m_VoteYes, pSheriffReport->SheriffYesA, pSheriffReport->m_VoteNo, pSheriffReport->m_Date);
			}
					
			result << ReturnValue;
			Send(&result);
		}
		break;
	case ReportAgree:
		{
			Packet result(WIZ_REPORT, uint8(ReportAgree));
			uint8 ReturneeValue = ReportAgree;

			pkt.SByte();
			pkt  >> strReportedName;

			if (!isGM() && !isKing())
			{
				ReturneeValue = VotingFailed;
				result << ReturneeValue;
				Send(&result);
				return;
			}

			if (ReturneeValue == ReportAgree)
			{				
				auto itr = g_pMain->m_SheriffArray.find(strReportedName);
				if (itr != g_pMain->m_SheriffArray.end())
				{
					bool SheriffIsEmpty = false;
					std::string IsEmptySheriffName;
					IsEmptySheriffName.clear();
					auto SheriffVote = itr->second;
					
					std::string SheriffArray[] = { SheriffVote->SheriffYesA, SheriffVote->SheriffYesB, SheriffVote->SheriffYesC, SheriffVote->SheriffNoA, SheriffVote->SheriffNoB, SheriffVote->SheriffNoC };
					
					foreach_array(i, SheriffArray)
					{
						auto VotingYes = SheriffArray[i];
						if (GetName() == VotingYes)
							SheriffIsEmpty = true;
					}
					
					if (SheriffIsEmpty)
					{
						ReturneeValue = VotingFailed;
						result << ReturneeValue;
						Send(&result);
						return;
					}

					SheriffVote->m_VoteYes++;
					if (SheriffVote->SheriffYesB.empty())
					{
						SheriffVote->SheriffYesB = GetName();
						IsEmptySheriffName = SheriffVote->SheriffYesB;
					}
					else if(SheriffVote->SheriffYesC.empty())
					{
						SheriffVote->SheriffYesC = GetName();
						IsEmptySheriffName = SheriffVote->SheriffYesC;
					}
					else
					{
						ReturneeValue = VotingFailed;
						result << ReturneeValue;
						Send(&result);
						return;
					}

					g_DBAgent.UpdateSheriffTable(1,SheriffVote->reportedName,SheriffVote->m_VoteYes,SheriffVote->m_VoteNo, IsEmptySheriffName);
				}
				result << uint8(1);
				Send(&result);
			}
		}
		break;
	case ReportDisagree:
		{
			Packet result(WIZ_REPORT, uint8(ReportAgree));
			uint8 ReturneyValue = ReportDisagree;

			pkt.SByte();
			pkt  >> strReportedName;

			if (!isGM() && !isKing())
			{
				ReturneyValue = VotingFailed;
				result << ReturneyValue;
				Send(&result);
				return;
			}

			if (ReturneyValue == ReportDisagree)
			{				
				auto itr = g_pMain->m_SheriffArray.find(strReportedName);
				if (itr != g_pMain->m_SheriffArray.end())
				{
					bool SheriffIsEmpty = false;
					std::string IsEmptySheriffName;
					IsEmptySheriffName.clear();
					auto SheriffVote = itr->second;
					
					std::string SheriffArray[] = { SheriffVote->SheriffYesA, SheriffVote->SheriffYesB, SheriffVote->SheriffYesC, SheriffVote->SheriffNoA, SheriffVote->SheriffNoB, SheriffVote->SheriffNoC };
					
					foreach_array(i, SheriffArray)
					{
						auto VotingYes = SheriffArray[i];
						if (GetName() == VotingYes)
							SheriffIsEmpty = true;
					}
					
					if (SheriffIsEmpty)
					{
						ReturneyValue = VotingFailed;
						result << ReturneyValue;
						Send(&result);
						return;
					}

					SheriffVote->m_VoteNo++;
					if (SheriffVote->SheriffNoA.empty())
					{
						SheriffVote->SheriffNoA = GetName();
						IsEmptySheriffName = SheriffVote->SheriffNoA;
					}
					else if (SheriffVote->SheriffNoB.empty())
					{
						SheriffVote->SheriffNoB = GetName();
						IsEmptySheriffName = SheriffVote->SheriffNoB;
					}
					else if (SheriffVote->SheriffNoC.empty())
					{
						SheriffVote->SheriffNoC = GetName();
						IsEmptySheriffName = SheriffVote->SheriffNoC;
					}
					else
					{
						ReturneyValue = VotingFailed;
						result << ReturneyValue;
						Send(&result);
						return;
					}

					g_DBAgent.UpdateSheriffTable(2,SheriffVote->reportedName,SheriffVote->m_VoteYes,SheriffVote->m_VoteNo, IsEmptySheriffName);
				}
				result << uint8(1);
				Send(&result);
			}	
		}
		break;
	case ListOpen:
		{
		auto CurrentPage = pkt.read<uint8>();

		Packet result(WIZ_REPORT, uint8(ListOpen));
		if (g_pMain->m_SheriffArray.size() <= 0)
		{
			result << uint8(0x00) << uint8(0x01) << uint8(0x01);
			Send(&result);
			return;
		}

		uint8 counter = 0, counter2 = 0;
		uint8 showuser = CurrentPage * 8;
		uint8 closedUser = showuser - 8;

		result << counter2;
		result.SByte();
		foreach(itr, g_pMain->m_SheriffArray)
		{
			if (counter++ < closedUser)
				continue;

			auto SheriffReport = itr->second;
			result << SheriffReport->reportedName << SheriffReport->ReportSheriffName << SheriffReport->crime << uint8(0x00)
				<< SheriffReport->m_VoteYes
				<< SheriffReport->SheriffYesA << SheriffReport->SheriffYesB << SheriffReport->SheriffYesC
				<< SheriffReport->m_VoteNo
				<< SheriffReport->SheriffNoA << SheriffReport->SheriffNoB << SheriffReport->SheriffNoC << SheriffReport->m_Date;
			counter2++;

			if (counter2 >= 8)
				break;
		}

		uint8 page_number = (uint8)g_pMain->m_SheriffArray.size() / 8;
		if (page_number % 8 != 0)
			page_number++;

		result << CurrentPage << page_number;
		result.put(1, counter2);
		Send(&result);
		}
		break;
	default:
		TRACE("Unhandled Sheriff: %X\n", opcode);
		printf("Unhandled Sheriff: %X\n", opcode);
		break;
	}
}