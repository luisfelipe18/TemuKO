#pragma once

class CEventSchedulePlayTimer : public OdbcRecordset
{
public:
	CEventSchedulePlayTimer(OdbcConnection* dbConnection, RoomEventPlayTimerArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("EVENT_ROOM_PLAY_TIMER"); }
	virtual tstring GetColumns()
	{
		return _T("EventLocalID, EventZoneID, EventName, EventSignTime, EventPlayTime, EventAttackOpenTime, EventAttackCloseTime, EventFinishTime");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_ROOM_EVENT_PLAY_TIMER* pData = new _ROOM_EVENT_PLAY_TIMER;
		_dbCommand->FetchByte(i++, pData->TableEventLocalID);
		_dbCommand->FetchUInt16(i++, pData->EventZoneID);
		_dbCommand->FetchString(i++, pData->EventName);
		_dbCommand->FetchUInt32(i++, pData->EventSignTime);
		_dbCommand->FetchUInt32(i++, pData->EventPlayTime);
		_dbCommand->FetchUInt32(i++, pData->EventAttackOpenTime);
		_dbCommand->FetchUInt32(i++, pData->EventAttackCloseTime);
		_dbCommand->FetchUInt32(i++, pData->EventFinishTime);

		if (!m_pMap->PutData(pData->TableEventLocalID, pData))
			delete pData;

		return true;
	}
	RoomEventPlayTimerArray* m_pMap;
};