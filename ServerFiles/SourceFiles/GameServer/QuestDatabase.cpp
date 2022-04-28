#include "stdafx.h"
#include "../shared/database/OdbcConnection.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;


bool CDBAgent::LoadQuestData(std::string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (pUser == nullptr
		|| strCharID.length() > MAX_ID_SIZE
		|| pUser->m_questMap.GetSize() > 0)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_USER_QUEST_DATA(?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	char strQuest[QUEST_ARRAY_SIZE] = { 0 };

	uint16 sQuestCount = 0, sQuestDataCount = 0;
	int field = 1;

	dbCommand->FetchUInt16(field++, sQuestCount);
	dbCommand->FetchBinary(field++, strQuest, sizeof(strQuest));

	// Convert the old quest storage format to the new one.
	if (sQuestCount > QUEST_LIMIT)
		sQuestCount = QUEST_LIMIT;

	for (int i = 0, index = 0; i < sQuestCount; i++, index += 7)
	{
		_USER_QUEST_INFO *pUserQuest = new _USER_QUEST_INFO();
		uint16	sQuestID = *(uint16 *)(strQuest + index);
		pUserQuest->QuestState = *(uint8  *)(strQuest + index + 2);
		pUserQuest->m_bKillCounts[0] = *(uint8  *)(strQuest + index + 3);
		pUserQuest->m_bKillCounts[1] = *(uint8  *)(strQuest + index + 4);
		pUserQuest->m_bKillCounts[2] = *(uint8  *)(strQuest + index + 5);
		pUserQuest->m_bKillCounts[3] = *(uint8  *)(strQuest + index + 6);

		_QUEST_HELPER_NATION * pQuestNation = g_pMain->m_QuestHelperNationArray.GetData(sQuestID);
		if (pQuestNation != nullptr
			&& pQuestNation->bNation != Nation::NONE
			&& pQuestNation->bNation != pUser->GetNation())
		{
			if (pUser->GetNation() == Nation::KARUS)
			{
				sQuestID--;
			}
			else if (pUser->GetNation() == Nation::ELMORAD)
			{
				sQuestID++;
			}
		}

		if (!pUser->m_questMap.PutData(sQuestID, pUserQuest))
			return false;
	}

	// Start the Seed quest if it doesn't already exist.
	if (pUser->CheckExistEvent(STARTER_SEED_QUEST, 0))
		pUser->SaveEvent(STARTER_SEED_QUEST, 1);

	return true;

}

bool CDBAgent::UpdateQuestData(std::string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (strCharID != pUser->GetName())
		return false;

	char strQuest[QUEST_ARRAY_SIZE];
	memset(strQuest, 0, sizeof(strQuest));
	int index = 0;
	foreach_stlmap(itr, pUser->m_questMap)
	{
		*(uint16 *)(strQuest + index) = itr->first;
		*(uint8  *)(strQuest + index + 2) = itr->second->QuestState;
		*(uint8  *)(strQuest + index + 3) = itr->second->m_bKillCounts[0];
		*(uint8  *)(strQuest + index + 4) = itr->second->m_bKillCounts[1];
		*(uint8  *)(strQuest + index + 5) = itr->second->m_bKillCounts[2];
		*(uint8  *)(strQuest + index + 6) = itr->second->m_bKillCounts[3];
		index += 7;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strQuest, sizeof(strQuest), SQL_BINARY);

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_QUEST_DATA(?, ?, %d)}"), pUser->m_questMap.GetSize())))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}