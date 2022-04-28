#pragma once

class CUserReturnLetterGiftItem : public OdbcRecordset
{
public:
	CUserReturnLetterGiftItem(OdbcConnection * dbConnection, KnightReturnLetterGiftListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("RETURN_IS_LETTER_ITEM"); }
	virtual tstring GetColumns()
	{
		return _T("ID, nItemID, sCount, strSender, strSubject, strMessage");
	}

	virtual bool Fetch()
	{
		_KNIGHT_RETURN_GIFT_ITEM *pData = new _KNIGHT_RETURN_GIFT_ITEM;
		int i = 1;
		_dbCommand->FetchByte(i++, pData->ID);
		_dbCommand->FetchUInt32(i++, pData->m_iItemNum);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchString(i++, pData->strMessage);
		_dbCommand->FetchString(i++, pData->strSubject);
		_dbCommand->FetchString(i++, pData->strSender);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	KnightReturnLetterGiftListArray * m_pMap;
};