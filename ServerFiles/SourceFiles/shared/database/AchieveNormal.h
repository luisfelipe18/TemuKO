#pragma once

class CAchieveNormalSet : public OdbcRecordset
{
public:
	CAchieveNormalSet(OdbcConnection * dbConnection, AchieveNormalArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_NORMAL"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, Type, Count");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_NORMAL *pData = new _ACHIEVE_NORMAL();
		uint32 col = 1;

		_dbCommand->FetchUInt32(col++, pData->sIndex);
		_dbCommand->FetchUInt16(col++, pData->Type);
		_dbCommand->FetchUInt32(col++, pData->Count);

		
		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveNormalArray * m_pMap;
};