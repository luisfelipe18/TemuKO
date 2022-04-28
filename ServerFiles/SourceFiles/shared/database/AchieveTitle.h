#pragma once

class CAchieveTitleSet : public OdbcRecordset
{
public:
	CAchieveTitleSet(OdbcConnection * dbConnection, AchieveTitleArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_TITLE"); }
	virtual tstring GetColumns() 
	{
		return _T("sIndex, strName, Str, Hp, Dex, Int, Mp, Attack, Defence, sLoyaltyBonus, sExpBonus, sShortSwordAC, sJamadarAC, sSwordAC, sBlowAC, sAxeAC ,sSpearAC, sArrowAC, sFireBonus, sIceBonus, sLightBonus, sFireResist, sIceResist, sLightResist, sMagicResist, sCurseResist, sPoisonResist");
	}

	virtual bool Fetch()
	{
		_ACHIEVE_TITLE *pData = new _ACHIEVE_TITLE;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->sIndex);
		_dbCommand->FetchString(i++, pData->strName);
		_dbCommand->FetchUInt16(i++, pData->Str);
		_dbCommand->FetchUInt16(i++, pData->Hp);
		_dbCommand->FetchUInt16(i++, pData->Dex);
		_dbCommand->FetchUInt16(i++, pData->Int);
		_dbCommand->FetchUInt16(i++, pData->Mp);
		_dbCommand->FetchUInt16(i++, pData->Attack);
		_dbCommand->FetchUInt16(i++, pData->Defence);
		_dbCommand->FetchUInt16(i++, pData->sLoyaltyBonus);
		_dbCommand->FetchUInt16(i++, pData->sExpBonus);
		_dbCommand->FetchUInt16(i++, pData->sShortSwordAC);
		_dbCommand->FetchUInt16(i++, pData->sJamadarAC);
		_dbCommand->FetchUInt16(i++, pData->sSwordAC);
		_dbCommand->FetchUInt16(i++, pData->sBlowAC);
		_dbCommand->FetchUInt16(i++, pData->sAxeAC);
		_dbCommand->FetchUInt16(i++, pData->sSpearAC);
		_dbCommand->FetchUInt16(i++, pData->sArrowAC);
		_dbCommand->FetchUInt16(i++, pData->sFireBonus);
		_dbCommand->FetchUInt16(i++, pData->sIceBonus);
		_dbCommand->FetchUInt16(i++, pData->sLightBonus);
		_dbCommand->FetchUInt16(i++, pData->sFireResist);
		_dbCommand->FetchUInt16(i++, pData->sIceResist);
		_dbCommand->FetchUInt16(i++, pData->sLightResist);
		_dbCommand->FetchUInt16(i++, pData->sMagicResist);
		_dbCommand->FetchUInt16(i++, pData->sCurseResist);
		_dbCommand->FetchUInt16(i++, pData->sPoisonResist);

		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	AchieveTitleArray * m_pMap;
};