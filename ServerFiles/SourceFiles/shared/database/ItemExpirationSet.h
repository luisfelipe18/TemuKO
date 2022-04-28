#pragma once

class CItemExpirationSet : public OdbcRecordset
{
public:
	CItemExpirationSet(OdbcConnection * dbConnection, ItemExpirationArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_EXPIRATION"); }
	virtual tstring GetColumns() { return _T("Num, strName, ExpirationTime"); }

	virtual bool Fetch()
	{
		_ITEM_EXPIRATION_TABLE *pData = new _ITEM_EXPIRATION_TABLE;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->m_iNum);
		_dbCommand->FetchString(i++, pData->m_sName);
		_dbCommand->FetchUInt32(i++, pData->nExpiration);
		

		if (!m_pMap->PutData(pData->m_iNum, pData))
			delete pData;

		return true;
	}

	ItemExpirationArray *m_pMap;
};