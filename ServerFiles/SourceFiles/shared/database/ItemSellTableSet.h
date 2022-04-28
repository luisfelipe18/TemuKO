#pragma once

class CItemSellTableSet : public OdbcRecordset
{
public:
	CItemSellTableSet(OdbcConnection * dbConnection, ItemSellTableArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_SELLTABLE"); }
	virtual tstring GetColumns() { return _T("iSellingGroup, Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9, Item10,\
											 Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19, Item20, Item21,\
											 Item22, Item23, Item24"); }

	virtual bool Fetch()
	{
		int itemID[24];
		int iSellingGroup = 0;
		memset(itemID, 0 , sizeof(itemID));

		_dbCommand->FetchInt32(1, iSellingGroup);

		for(int i = 0; i < 24; i++)
			_dbCommand->FetchInt32(i+2, itemID[i]);

		_ITEM_SELLTABLE* itr = m_pMap->GetData(iSellingGroup);

		if(itr == nullptr)
		{
			_ITEM_SELLTABLE *pData = new _ITEM_SELLTABLE;

			pData->iSellingGroup = iSellingGroup;

			if (!m_pMap->PutData(pData->iSellingGroup, pData))
				delete pData;
		}

		itr = m_pMap->GetData(iSellingGroup);

		if(itr == nullptr)
			return false;

		for(int i = 0; i < 24; i++)
		{
			if(iSellingGroup == 0)
				break;

			if(itemID[i] == 0)
				continue;
			
			itr->ItemIDs.push_back(itemID[i]);
		}

		return true;
	}
	ItemSellTableArray *m_pMap;
};