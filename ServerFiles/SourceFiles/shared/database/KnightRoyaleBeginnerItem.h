#pragma once

class CKnightRoyaleBeginnerItem : public OdbcRecordset
{
public:
	CKnightRoyaleBeginnerItem(OdbcConnection * dbConnection, KnightRoyaleBeginnerItemListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KNIGHT_ROYALE_BEGINNER_ITEM"); }
	virtual tstring GetColumns()
	{
		return _T("ID, ItemPos, nItemID, nCount, nDurability, nFlag");
	}

	virtual bool Fetch()
	{
		_KNIGHT_ROYALE_BEGINNER_ITEM *pData = new _KNIGHT_ROYALE_BEGINNER_ITEM;

		int i = 1;
		_dbCommand->FetchByte(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->ItemPos);
		_dbCommand->FetchUInt32(i++, pData->nItemID);
		_dbCommand->FetchByte(i++, pData->nCount);
		_dbCommand->FetchUInt16(i++, pData->nDurability);
		_dbCommand->FetchByte(i++, pData->nFlag);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	KnightRoyaleBeginnerItemListArray * m_pMap;
};