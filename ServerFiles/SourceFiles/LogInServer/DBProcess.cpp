#include "stdafx.h"

bool CDBProcess::Connect(string & szDSN, string & szUser, string & szPass)
{
	if (!m_dbConnection.Connect(szDSN, szUser, szPass))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	return true;
}

bool CDBProcess::LoadVersionList()
{
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("SELECT sVersion, sHistoryVersion, strFilename FROM VERSION")))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	if (dbCommand->hasData())
	{
		g_pMain->m_sLastVersion = 0;
		do
		{
			_VERSION_INFO *pVersion = new _VERSION_INFO;

			dbCommand->FetchUInt16(1, pVersion->sVersion);
			dbCommand->FetchUInt16(2, pVersion->sHistoryVersion);
			dbCommand->FetchString(3, pVersion->strFilename);

			g_pMain->m_VersionList.insert(make_pair(pVersion->strFilename, pVersion));

			if (g_pMain->m_sLastVersion < pVersion->sVersion)
				g_pMain->m_sLastVersion = pVersion->sVersion;

		} while (dbCommand->MoveNext());
	}

	return true;
}

bool CDBProcess::LoadServerList()
{
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("SELECT * FROM GAME_SERVER_LIST")))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	if(g_pMain->m_ServerList.GetSize() > 0)
		g_pMain->m_ServerList.DeleteAllData();

	if (dbCommand->hasData())
	{
		int nServerCount = 0;
		do
		{
			_SERVER_INFO* pInfo = new _SERVER_INFO();

			int i = 1;
			dbCommand->FetchInt16(i++, pInfo->sServerID);
			dbCommand->FetchInt16(i++, pInfo->sGroupID);
			dbCommand->FetchString(i++, pInfo->strServerName);
			dbCommand->FetchString(i++, pInfo->strServerIP);
			dbCommand->FetchString(i++, pInfo->strLanIP);
			dbCommand->FetchInt16(i++, pInfo->sPlayerCap);
			dbCommand->FetchInt16(i++, pInfo->sFreePlayerCap);
			dbCommand->FetchString(i++, pInfo->strKarusKingName);
			dbCommand->FetchString(i++, pInfo->strKarusNotice);
			dbCommand->FetchString(i++, pInfo->strElMoradKingName);
			dbCommand->FetchString(i++, pInfo->strElMoradNotice);		
			g_pMain->m_ServerList.PutData(pInfo->sServerID, pInfo);
			nServerCount++;

		} while (dbCommand->MoveNext());
	}

	return true;
}

bool CDBProcess::LoadUserCountList()
{
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("SELECT serverid, zone1_count, zone2_count, zone3_count FROM CONCURRENT")))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	if (dbCommand->hasData())
	{
		do
		{
			uint16 zone_1 = 0, zone_2 = 0, zone_3 = 0; uint8 serverID;
			dbCommand->FetchByte(1, serverID);
			dbCommand->FetchUInt16(2, zone_1);
			dbCommand->FetchUInt16(3, zone_2);
			dbCommand->FetchUInt16(4, zone_3);

			_SERVER_INFO* pInfo = g_pMain->m_ServerList.GetData(serverID);
			if(pInfo)
				pInfo->sUserCount = zone_1 + zone_2 + zone_3;

		} while (dbCommand->MoveNext());
	}

	return true;
}

uint16 CDBProcess::AccountLogin(string & strAccountID, string & strPasswd, std::string & OTP_Key)
{
	uint16 result = 2;
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return 6;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPasswd.c_str(), strPasswd.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_LOGIN(?, ?)}")))
		g_pMain->ReportSQLError(m_dbConnection.GetError());


	if (result == 16)
	{
		unique_ptr<OdbcCommand> dbCommand2(m_dbConnection.CreateCommand());
		if (dbCommand2.get() == nullptr)
			return 17;

		dbCommand2->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
		if (!dbCommand2->Execute(_T("SELECT OTPPassword FROM TB_USER WHERE strAccountID = ?")))
		{
			g_pMain->ReportSQLError(m_dbConnection.GetError());
			return 18;
		}

		if (dbCommand2->hasData())
		{
			do
			{
				dbCommand2->FetchString(1, OTP_Key);

			} while (dbCommand2->MoveNext());
		}
	}
	return result;
}

uint16 CDBProcess::AccountLoginOTP(string & strAccountID, string & strPasswd)
{
	uint16 result = 2;
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return 6;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPasswd.c_str(), strPasswd.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_LOGIN_OTP(?, ?)}")))
		g_pMain->ReportSQLError(m_dbConnection.GetError());

	return result;
}

int16 CDBProcess::AccountPremium(string & strAccountID)
{
	int16 sHours = -1;
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return sHours;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sHours);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_PREMIUM_V2(?)}")))
		g_pMain->ReportSQLError(m_dbConnection.GetError());

	return sHours;
}