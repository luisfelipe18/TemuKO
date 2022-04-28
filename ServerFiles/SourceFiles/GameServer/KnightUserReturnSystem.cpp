#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;

#pragma region CDBAgent::LoadUserReturnData(CUser *pUser)
bool CDBAgent::LoadUserReturnData(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(_T("{CALL LOAD_USER_RETURN_DATA(?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt32(1, pUser->ReturnSymbolisOK);
	dbCommand->FetchInt64(2, pUser->ReturnisLogOutTime);
	dbCommand->FetchInt64(3, pUser->ReturnSymbolTime);

	int64 UpdateTime = 0;
	if (pUser->ReturnSymbolisOK <= 0)
	{
		if (pUser->ReturnisLogOutTime > 0)
		{
			UpdateTime = (int64(UNIXTIME) - pUser->ReturnisLogOutTime);
			UpdateTime = (UpdateTime / 3600);
			if (UpdateTime >= 720) //30 days
			{
				pUser->ReturnSymbolisOK = 1;
				pUser->ReturnSymbolTime = int64(UNIXTIME + 60 * 60 * 24 * 30); //symbol days +30 day
				pUser->KnightReturnGiveLetterItem();
			}
		}
	}

	return true;
}
#pragma endregion

#pragma region CDBAgent::UpdateUserReturnData(string & strCharID, CUser *pUser)
bool CDBAgent::UpdateUserReturnData(string & strCharID, CUser *pUser)
{
	if (strCharID != pUser->GetName())
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_RETURN_DATA(?,%d,%d,%d)}"), pUser->ReturnSymbolisOK, int64(UNIXTIME), pUser->ReturnSymbolTime)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CUser::KnightReturnGiveLetterItem()
void CUser::KnightReturnGiveLetterItem()
{
	_ITEM_TABLE* pTable = nullptr;
	foreach_stlmap(itr, g_pMain->m_KnightReturnLetterGiftListArray)
	{
		_ITEM_DATA pItem;
		pItem.nNum = itr->second->m_iItemNum;
		pTable = g_pMain->GetItemPtr(pItem.nNum);
		if (pTable == nullptr)
			return;

		pItem.nSerialNum = g_pMain->GenerateItemSerial();
		pItem.sCount = itr->second->sCount;
		pItem.sDuration = pTable->m_sDuration;
		g_DBAgent.SendLetter(itr->second->strSender, GetName(), itr->second->strSubject, itr->second->strMessage, 2, &pItem, 0);
	}
	ReqLetterUnread();
}
#pragma endregion

#pragma region CUser::KnightReturnLetterItemDeleted()
void CUser::KnightReturnLetterItemDeleted()
{


}
#pragma endregion