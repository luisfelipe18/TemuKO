#pragma once

typedef std::map<uint16, _ZONE_INFO *> ZoneInfoMap;

class CZoneInfoSet : public OdbcRecordset
{
public:
	CZoneInfoSet(OdbcConnection * dbConnection, ZoneInfoMap * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ZONE_INFO"); }
	virtual tstring GetColumns()
	{
		return _T("ServerNo, ZoneNo, strZoneSmdName, strZoneName,"
			"Status, ZoneType, MinLevel, MaxLevel,"
			"TradeOtherNation,TalkOtherNation,AttackOtherNation,"
			"AttackSameNation,FriendlyNpc,WarZone,ClanUpdates, "
			"Teleport,Gate,[Escape],CallingFriend,TeleportFriend,Blink,PetSpawn, "
			"ExpLost,GiveLoyalty,GuardSummon,BlinkZone, InitX, InitZ, InitY");
	}

	virtual tstring GetWhereClause() { return _T("Status = 1"); }

	virtual bool Fetch()
	{
		_ZONE_INFO * pData = new _ZONE_INFO;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->m_nServerNo);
		_dbCommand->FetchUInt16(i++, pData->m_nZoneNumber);
		_dbCommand->FetchString(i++, pData->m_MapName);
		_dbCommand->FetchString(i++, pData->m_ZoneName);
		_dbCommand->FetchByte(i++, pData->m_Status);
		_dbCommand->FetchByte(i++, pData->m_ZoneType);
		_dbCommand->FetchByte(i++, pData->m_MinLevel);
		_dbCommand->FetchByte(i++, pData->m_MaxLevel);
		_dbCommand->FetchByte(i++, pData->m_kTradeOtherNation);
		_dbCommand->FetchByte(i++, pData->m_kTalkOtherNation);
		_dbCommand->FetchByte(i++, pData->m_kAttackOtherNation);
		_dbCommand->FetchByte(i++, pData->m_kAttackSameNation);
		_dbCommand->FetchByte(i++, pData->m_kFriendlyNpc);
		_dbCommand->FetchByte(i++, pData->m_kWarZone);
		_dbCommand->FetchByte(i++, pData->m_kClanUpdates);
		_dbCommand->FetchByte(i++, pData->m_bTeleport);
		_dbCommand->FetchByte(i++, pData->m_bGate);
		_dbCommand->FetchByte(i++, pData->m_bEscape);
		_dbCommand->FetchByte(i++, pData->m_bCallingFriend);
		_dbCommand->FetchByte(i++, pData->m_bTeleportFriend);
		_dbCommand->FetchByte(i++, pData->m_bBlink);
		_dbCommand->FetchByte(i++, pData->m_bPetSpawn);
		_dbCommand->FetchByte(i++, pData->m_bExpLost);
		_dbCommand->FetchByte(i++, pData->m_bGiveLoyalty);
		_dbCommand->FetchByte(i++, pData->m_bGuardSummon);
		_dbCommand->FetchByte(i++, pData->m_bBlinkZone);

		uint32 iX = 0, iY = 0, iZ = 0;
		_dbCommand->FetchUInt32(i++, iX);
		_dbCommand->FetchUInt32(i++, iZ);
		_dbCommand->FetchUInt32(i++, iY);

		pData->m_fInitX = (float)(iX / 100.0f);
		pData->m_fInitY = (float)(iY / 100.0f);
		pData->m_fInitZ = (float)(iZ / 100.0f);

		if (pData->m_nZoneNumber > MAX_ZONE_ID)
		{
			printf("ERROR: Zone ID %d is too large. Highest zone ID can be %d.\n", pData->m_nZoneNumber, MAX_ZONE_ID);
			delete pData;
			return false;
		}

		if (m_pMap->find(pData->m_nZoneNumber) != m_pMap->end())
			delete pData;
		else
			m_pMap->insert(std::make_pair(pData->m_nZoneNumber, pData));

		return true;
	}

	ZoneInfoMap * m_pMap;
};