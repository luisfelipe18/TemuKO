#pragma once

class CEventCswSchedulePlayTimer : public OdbcRecordset
{
public:
	CEventCswSchedulePlayTimer(OdbcConnection* dbConnection, CswEventPlayTimerArray* pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("EVENT_CSW_PLAY_TIMER"); }
	virtual tstring GetColumns()
	{
		return _T("EventLocalID, EventZoneID, EventName, BarrackCreatedTime, DeathMatchTime, PreaparingTime,CastellanTime,OwnerDelosMonumentFinish,MonumentFinishTime");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_CSW_EVENT_PLAY_TIMER* pData = new _CSW_EVENT_PLAY_TIMER;
		_dbCommand->FetchByte(i++, pData->TableEventLocalID);
		_dbCommand->FetchUInt16(i++, pData->EventZoneID);
		_dbCommand->FetchString(i++, pData->EventName);
		_dbCommand->FetchInt32(i++, pData->BarrackCreatedTime);
		_dbCommand->FetchInt32(i++, pData->DeathMatchTime);
		_dbCommand->FetchInt32(i++, pData->PreparingTime);
		_dbCommand->FetchInt32(i++, pData->CastellanTime);
		_dbCommand->FetchInt32(i++, pData->OwnerDelosMonumentFinish);
		_dbCommand->FetchInt32(i++, pData->MonumentFinishTime);

		if (!m_pMap->PutData(pData->TableEventLocalID, pData))
			delete pData;

		return true;
	}
	CswEventPlayTimerArray* m_pMap;
};