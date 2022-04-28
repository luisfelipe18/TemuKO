#pragma once

class CAchieveMainSet : public OdbcRecordset
{
public:
	CAchieveMainSet(OdbcConnection * dbConnection, AchieveMainArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_MAIN"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, Type, TitleID, Point, ItemNum, Count, ZoneID, unknown2, AchieveType, \
				  ReqTime, strName, strDesc, byte1, byte2");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_MAIN *pData = new _ACHIEVE_MAIN;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sIndex);
		_dbCommand->FetchByte(i++, pData->Type);
		_dbCommand->FetchUInt16(i++, pData->TitleID);
		_dbCommand->FetchUInt16(i++, pData->Point);
		_dbCommand->FetchUInt32(i++, pData->ItemNum);
		_dbCommand->FetchUInt32(i++, pData->iCount);
		_dbCommand->FetchByte(i++, pData->ZoneID);
		_dbCommand->FetchByte(i++, pData->unknown2);
		_dbCommand->FetchByte(i++, pData->AchieveType);
		_dbCommand->FetchUInt16(i++, pData->ReqTime);
		_dbCommand->FetchString(i++, pData->strName);
		_dbCommand->FetchString(i++, pData->strDescription);
		_dbCommand->FetchByte(i++, pData->byte1);
		_dbCommand->FetchByte(i++, pData->byte2);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveMainArray * m_pMap;
};