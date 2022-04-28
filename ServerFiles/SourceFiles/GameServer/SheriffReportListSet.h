#pragma once

class CSheriffReportList: public OdbcRecordset
{
public:
	CSheriffReportList(OdbcConnection * dbConnection, SheriffArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("SHERIFF_REPORTS"); }
	virtual tstring GetColumns()  { return _T("ReportedName, ReportedSheriffName, Crime, Yes, SheriffYesA, SheriffYesB, SheriffYesC, No, SheriffNoA, SheriffNoB, SheriffNoC, ReportDateTime"); } 
	
	virtual bool Fetch()
	{
		int i = 1;

		auto * pData = new _SHERIFF_STUFF();

		_dbCommand->FetchString(i++, pData->reportedName);
		_dbCommand->FetchString(i++, pData->ReportSheriffName);
		_dbCommand->FetchString(i++, pData->crime);
		_dbCommand->FetchByte(i++, pData->m_VoteYes);
		_dbCommand->FetchString(i++, pData->SheriffYesA);
		_dbCommand->FetchString(i++, pData->SheriffYesB);
		_dbCommand->FetchString(i++, pData->SheriffYesC);
		_dbCommand->FetchByte(i++, pData->m_VoteNo);
		_dbCommand->FetchString(i++, pData->SheriffNoA);
		_dbCommand->FetchString(i++, pData->SheriffNoB);
		_dbCommand->FetchString(i++, pData->SheriffNoC);
		_dbCommand->FetchString(i++, pData->m_Date);

		SheriffArray::iterator itr = g_pMain->m_SheriffArray.find(pData->reportedName);
		if (itr == g_pMain->m_SheriffArray.end())
			m_pMap->insert(std::make_pair(pData->reportedName, pData));

		return true;
	}

	SheriffArray * m_pMap;
};