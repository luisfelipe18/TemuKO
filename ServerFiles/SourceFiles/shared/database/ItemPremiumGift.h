#pragma once

class CItemPremiumGift : public OdbcRecordset
{
public:
	CItemPremiumGift(OdbcConnection * dbConnection, ItemPremiumGiftArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("PREMIUM_GIFT_ITEM"); }
	virtual tstring GetColumns() { return _T("bPremiumType, iBonusItemNum, sCount, strSender, strSubject, strMessage"); }

	virtual bool Fetch()
	{
		_ITEM_PREMIUM_GIFT *pData = new _ITEM_PREMIUM_GIFT;

		int i = 1;
		uint8 bPremiumType = 0;
		_dbCommand->FetchByte(i++, bPremiumType);
		_dbCommand->FetchUInt32(i++, pData->m_iItemNum);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchString(i++, pData->strSender);
		_dbCommand->FetchString(i++, pData->strSubject);
		_dbCommand->FetchString(i++, pData->strMessage);

		m_pMap->insert(std::make_pair(bPremiumType, pData));

		return true;
	}

	ItemPremiumGiftArray *m_pMap;
};