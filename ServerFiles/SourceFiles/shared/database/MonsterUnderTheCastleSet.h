#pragma once

class CMonsterUnderTheCastleSet : public OdbcRecordset
{
public:
	CMonsterUnderTheCastleSet(OdbcConnection * dbConnection, MonsterUnderTheCastleArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_UNDER_THE_CASTLE"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, bType, TrapNumber, X, Y, Z, byDirection, sCount, bRadius"); }

	virtual bool Fetch()
	{
		_MONSTER_UNDER_THE_CASTLE * pData = new _MONSTER_UNDER_THE_CASTLE;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->bType);
		_dbCommand->FetchByte(i++, pData->bTrapNumber);
		_dbCommand->FetchUInt16(i++, pData->X);
		_dbCommand->FetchUInt16(i++, pData->Y);
		_dbCommand->FetchUInt16(i++, pData->Z);
		_dbCommand->FetchByte(i++, pData->byDirection);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchByte(i++, pData->bRadius);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterUnderTheCastleArray *m_pMap;
};