#pragma once

class CCharacterSealSet : public OdbcRecordset
{
public:
	CCharacterSealSet(OdbcConnection * dbConnection, CharacterSealItemArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("USER_CHARACTER_SEAL_ITEMS"); }
	virtual tstring GetColumns() { return _T("strUserID, strAccountID, nItemSerial"); }

	virtual bool Fetch()
	{
		std::string strCharID, strAccountID;
		uint64 nItemSerial = 0;

		_dbCommand->FetchString(1, strCharID);
		_dbCommand->FetchString(2, strAccountID);
		_dbCommand->FetchUInt64(3, nItemSerial);

		_CHARACTER_SEAL_ITEM * pData = m_pMap->GetData(nItemSerial);
		if (pData == nullptr)
		{
			pData = new _CHARACTER_SEAL_ITEM;
			pData->nSerialNum = nItemSerial;
			pData->m_strUserID = strCharID;
			pData->strAccountID = strAccountID;

			if (!m_pMap->PutData(pData->nSerialNum, pData))
				delete pData;
		}

		return true;
	}

	CharacterSealItemArray *m_pMap;
};