#pragma once

class CMonsterStoneListInformationSet : public OdbcRecordset
{
public:
	CMonsterStoneListInformationSet(OdbcConnection * dbConnection, MonsterStoneListInformationArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_STONE_RESPAWN_LIST"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, bType, sPid, ZoneID, isBoss, Family, byDirection, sCount, X, Y, Z"); }

	virtual bool Fetch()
	{
		_MONSTER_STONE_LIST_INFORMATION * pData = new _MONSTER_STONE_LIST_INFORMATION;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->bType);
		_dbCommand->FetchUInt16(i++, pData->sPid);
		_dbCommand->FetchByte(i++, pData->ZoneID);
		_dbCommand->FetchByte(i++, pData->isBoss);
		_dbCommand->FetchUInt16(i++, pData->Family);
		_dbCommand->FetchByte(i++, pData->byDirection);
		_dbCommand->FetchByte(i++, pData->sCount);
		_dbCommand->FetchUInt16(i++, pData->X);
		_dbCommand->FetchUInt16(i++, pData->Y);
		_dbCommand->FetchUInt16(i++, pData->Z);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterStoneListInformationArray *m_pMap;
};