class CGiftUserGiftAray : public OdbcRecordset
{
public:
	CGiftUserGiftAray(OdbcConnection * dbConnection, UserGiftArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KILL_RANDOM_ITEM"); }
	virtual tstring GetColumns() { return _T("Number,strName,ItemID,[Percent],Days,Coin,Count"); }

	virtual bool Fetch()
	{
		_RANDOM_ITEM *pData = new _RANDOM_ITEM;

		_dbCommand->FetchUInt16(1, pData->Number);
		_dbCommand->FetchString(2, pData->strName);
		_dbCommand->FetchUInt32(3, pData->ItemID);
		_dbCommand->FetchUInt32(4, pData->Percent);
		_dbCommand->FetchUInt32(5, pData->Days);
		_dbCommand->FetchUInt32(6, pData->Coin);
		_dbCommand->FetchUInt32(7, pData->Count);
		m_pMap->push_back(pData);
		
		return true;
	}

	UserGiftArray * m_pMap;
};