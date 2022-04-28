#pragma once

class CNpcPosSet : public OdbcRecordset
{
public:
	CNpcPosSet(OdbcConnection * dbConnection, K_NpcPosSetArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("K_NPCPOS"); }
	virtual tstring GetColumns() { return _T("ZoneID, NpcID, ActType, RegenType, DungeonFamily, SpecialType, TrapNumber, LeftX, TopZ, RightX, BottomZ, LimitMinX, LimitMinZ, LimitMaxX, LimitMaxZ, NumNPC, RegTime, byDirection, DotCnt, path"); }

	virtual bool Fetch()
	{
		_K_NPCPOS * pData = new _K_NPCPOS;
		int i = 1;

		_dbCommand->FetchByte(i++, pData->bZoneID);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchByte(i++, pData->bActType);
		_dbCommand->FetchByte(i++, pData->bRegenType);
		_dbCommand->FetchByte(i++, pData->bDungeonFamily);
		_dbCommand->FetchByte(i++, pData->bSpecialType);
		_dbCommand->FetchByte(i++, pData->bTrapNumber);
		_dbCommand->FetchInt32(i++, pData->iLeftX);
		_dbCommand->FetchInt32(i++, pData->iTopZ);
		_dbCommand->FetchInt32(i++, pData->iRightX);
		_dbCommand->FetchInt32(i++, pData->iBottomZ);
		_dbCommand->FetchInt32(i++, pData->iLimitMinX);
		_dbCommand->FetchInt32(i++, pData->iLimitMinZ);
		_dbCommand->FetchInt32(i++, pData->iLimitMaxX);
		_dbCommand->FetchInt32(i++, pData->iLimitMaxZ);
		_dbCommand->FetchByte(i++, pData->bNumNpc);
		_dbCommand->FetchUInt16(i++, pData->sRegTime);
		_dbCommand->FetchByte(i++, pData->bDirection);
		_dbCommand->FetchByte(i++, pData->bDotCnt);
		_dbCommand->FetchString(i++, pData->szPath, sizeof(pData->szPath));

		pData->nIndex = m_pMap->GetSize();

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;

		if (pData->bActType > 100)
			pData->bActType = -100;

		return true;
	}
	K_NpcPosSetArray *m_pMap;
};