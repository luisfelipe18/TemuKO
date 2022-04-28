#pragma once
class CPPCardTable : public OdbcRecordset
{
public:
	CPPCardTable(OdbcConnection * dbConnection, PPCardArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("PPCARD"); }
	virtual tstring GetColumns() { return _T("FirstBox,Codes,Used,CashPoint"); }

	virtual bool Fetch()
	{
		_PPCARD * pData = new _PPCARD();
		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->FirstBox);
		_dbCommand->FetchString(i++, pData->Codes);
		_dbCommand->FetchByte(i++, pData->isUsed);
		_dbCommand->FetchUInt32(i++, pData->CashPoint);

		if (!m_pMap->PutData(pData->FirstBox, pData))
			delete pData;

		return true;
	}


	PPCardArray * m_pMap;
};