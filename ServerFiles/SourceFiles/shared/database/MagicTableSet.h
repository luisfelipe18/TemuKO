#pragma once

class CMagicTableSet : public OdbcRecordset
{
public:
	CMagicTableSet(OdbcConnection * dbConnection, MagictableArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MAGIC"); }
	virtual tstring GetColumns() { return _T("MagicNum, BeforeAction, TargetAction, SelfEffect, FlyingEffect, TargetEffect, Moral, SkillLevel, Skill, Msp, HP, sSp, ItemGroup, UseItem, CastTime, ReCastTime, SuccessRate, Type1, Type2, Range, Etc, UseStanding"); }

	virtual bool Fetch()
	{
		_MAGIC_TABLE *pData = new _MAGIC_TABLE;

		_dbCommand->FetchUInt32(1, pData->iNum);
		_dbCommand->FetchUInt32(2, pData->nBeforeAction);
		_dbCommand->FetchByte(3, pData->bTargetAction);
		_dbCommand->FetchByte(4, pData->bSelfEffect);
		_dbCommand->FetchUInt16(5, pData->bFlyingEffect);
		_dbCommand->FetchUInt16(6, pData->iTargetEffect);
		_dbCommand->FetchByte(7, pData->bMoral);
		_dbCommand->FetchUInt16(8, pData->sSkillLevel);
		_dbCommand->FetchUInt16(9, pData->sSkill);
		_dbCommand->FetchUInt16(10, pData->sMsp);
		_dbCommand->FetchUInt16(11, pData->sHP);
		_dbCommand->FetchUInt16(12, pData->sSp);
		_dbCommand->FetchByte(13, pData->bItemGroup);
		_dbCommand->FetchUInt32(14, pData->iUseItem);
		_dbCommand->FetchByte(15, pData->bCastTime);
		_dbCommand->FetchUInt16(16, pData->sReCastTime);
		_dbCommand->FetchByte(17, pData->bSuccessRate);
		_dbCommand->FetchByte(18, pData->bType[0]);
		_dbCommand->FetchByte(19, pData->bType[1]);
		_dbCommand->FetchUInt16(20, pData->sRange);
		_dbCommand->FetchUInt16(21, pData->sEtc);
		_dbCommand->FetchByte(22, pData->sUseStanding);

		if (!m_pMap->PutData(pData->iNum, pData))
			delete pData;

		return true;
	}

	MagictableArray *m_pMap;
};