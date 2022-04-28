#pragma once

class CNpcPosSet : public OdbcRecordset
{
public:
	CNpcPosSet(OdbcConnection * dbConnection, NpcPosArray * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("K_NPCPOS"); }
	virtual tstring GetColumns() { return _T("ZoneID, NpcID, ActType, RegenType, DungeonFamily, SpecialType, TrapNumber, LeftX, TopZ, RightX, BottomZ, LimitMinX, LimitMinZ, LimitMaxX, LimitMaxZ, NumNPC, RegTime, byDirection, DotCnt, path, Room"); }

	virtual bool Fetch()
	{
		_K_NPC_POS *pData = new _K_NPC_POS;

		_dbCommand->FetchByte(1, pData->bZoneID);
		_dbCommand->FetchUInt16(2, pData->sSid);
		_dbCommand->FetchByte(3, pData->bActType);
		_dbCommand->FetchByte(4, pData->bRegenType);
		_dbCommand->FetchByte(5, pData->bDungeonFamily);
		_dbCommand->FetchByte(6, pData->bSpecialType);
		_dbCommand->FetchByte(7, pData->bTrapNumber);
		_dbCommand->FetchInt32(8, pData->iLeftX);
		_dbCommand->FetchInt32(9, pData->iTopZ);
		_dbCommand->FetchInt32(10, pData->iRightX);
		_dbCommand->FetchInt32(11, pData->iBottomZ);
		_dbCommand->FetchInt32(12, pData->iLimitMinX);
		_dbCommand->FetchInt32(13, pData->iLimitMinZ);
		_dbCommand->FetchInt32(14, pData->iLimitMaxX);
		_dbCommand->FetchInt32(15, pData->iLimitMaxZ);
		_dbCommand->FetchByte(16, pData->bNumNpc);
		_dbCommand->FetchUInt16(17, pData->sRegTime);
		_dbCommand->FetchByte(18, pData->bDirection);
		_dbCommand->FetchByte(19, pData->bDotCnt);
		_dbCommand->FetchString(20, pData->szPath, sizeof(pData->szPath));
		_dbCommand->FetchUInt16(21, pData->sRoom);

		uint32 size = m_pMap->GetSize();
		if (!m_pMap->PutData(size, pData))
		{
			delete pData;
			return false;
		}

		return true;
	}

	NpcPosArray *m_pMap;
};