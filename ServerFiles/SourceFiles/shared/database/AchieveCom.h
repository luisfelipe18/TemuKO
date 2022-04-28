#pragma once

class CAchieveComSet : public OdbcRecordset
{
public:
	CAchieveComSet(OdbcConnection * dbConnection, AchieveComArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_COM"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, Type, Req1, Req2");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_COM *pData = new _ACHIEVE_COM;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sIndex);
		_dbCommand->FetchByte(i++, pData->Type);
		_dbCommand->FetchUInt16(i++, pData->Required1);
		_dbCommand->FetchUInt16(i++, pData->Required2);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveComArray * m_pMap;
};