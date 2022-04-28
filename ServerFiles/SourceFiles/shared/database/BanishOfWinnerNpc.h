#pragma once

class CBanishWinner : public OdbcRecordset
{
public:
	CBanishWinner(OdbcConnection * dbConnection, WarBanishOfWinnerArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("BANISH_OF_WINNER"); }
	virtual tstring GetColumns() { return _T("nIndex, sSid, sNationID, sZoneID, sPosX, sPosZ, sCount, sRadius, sDeadTime"); }

	virtual bool Fetch()
	{
		_BANISH_OF_WINNER *pData = new _BANISH_OF_WINNER;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sira);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->sNationID);
		_dbCommand->FetchByte(i++, pData->sZoneID);
		_dbCommand->FetchUInt16(i++, pData->sPosX);
		_dbCommand->FetchUInt16(i++, pData->sPosZ);
		_dbCommand->FetchUInt16(i++, pData->sCount);
		_dbCommand->FetchUInt16(i++, pData->sRadius);
		_dbCommand->FetchUInt16(i++, pData->sDeadTime);

		if (!m_pMap->PutData(pData->sira, pData))
			delete pData;

		return true;
	}

	WarBanishOfWinnerArray * m_pMap;
};