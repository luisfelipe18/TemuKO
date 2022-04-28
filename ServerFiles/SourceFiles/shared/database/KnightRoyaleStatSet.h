#pragma once

class CKnightRoyaleStatstSet : public OdbcRecordset
{
public:
	CKnightRoyaleStatstSet(OdbcConnection * dbConnection, KnightRoyaleStatsListArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("KNIGHT_ROYALE_STATS"); }
	virtual tstring GetColumns()
	{
		return _T("sLevel, sRequiredExperience, sTotalHit, sTotalAc, iMaxHp, iMaxMp, sFireR, sColdR, sLightningR, sMagicR, sDiseaseR, sPoisonR");
	}

	virtual bool Fetch()
	{
		_KNIGHT_ROYALE_STATS_LIST *pData = new _KNIGHT_ROYALE_STATS_LIST;

		int i = 1;
		_dbCommand->FetchByte(i++, pData->KnightRoyaleStatsLevel);
		_dbCommand->FetchUInt32(i++, pData->KnightRoyaleStatsRequiredExperience);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsTotalHit);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsTotalAc);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatiMaxHp);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatiMaxMp);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsFireR);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsColdR);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsLightningR);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsMagicR);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsDiseaseR);
		_dbCommand->FetchUInt16(i++, pData->KnightRoyaleStatsPoisonR);

		if (!m_pMap->PutData(pData->KnightRoyaleStatsLevel, pData))
			delete pData;

		return true;
	}

	KnightRoyaleStatsListArray * m_pMap;
};