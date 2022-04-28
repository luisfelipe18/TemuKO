#pragma once

class CJuraidMountionListInformationSet : public OdbcRecordset
{
public:
	CJuraidMountionListInformationSet(OdbcConnection * dbConnection, JuraidMountionListInformationArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_JURAID_MOUNTAIN_RESPAWN_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, bType, sPid, ZoneID, Family, sCount, X, Y, Z, byDirection, bRadius"); }

	virtual bool Fetch()
	{
		_MONSTER_JURAID_MOUNTAIN_RESPAWN_LIST * pData = new _MONSTER_JURAID_MOUNTAIN_RESPAWN_LIST;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->bType);
		_dbCommand->FetchUInt16(i++, pData->sPid);
		_dbCommand->FetchByte(i++, pData->ZoneID);
		_dbCommand->FetchUInt16(i++, pData->Family);
		_dbCommand->FetchByte(i++, pData->sCount);
		_dbCommand->FetchUInt16(i++, pData->X);
		_dbCommand->FetchUInt16(i++, pData->Y);
		_dbCommand->FetchUInt16(i++, pData->Z);
		_dbCommand->FetchByte(i++, pData->byDirection);
		_dbCommand->FetchByte(i++, pData->bRadius);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	JuraidMountionListInformationArray *m_pMap;
};