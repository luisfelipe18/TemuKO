#pragma once

class CAchieveWarSet : public OdbcRecordset
{
public:
	CAchieveWarSet(OdbcConnection * dbConnection, AchieveWarArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_WAR"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, Type, sCount");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_WAR *pData = new _ACHIEVE_WAR;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sIndex);
		_dbCommand->FetchByte(i++, pData->Type);
		_dbCommand->FetchUInt32(i++, pData->Count);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveWarArray * m_pMap;
};