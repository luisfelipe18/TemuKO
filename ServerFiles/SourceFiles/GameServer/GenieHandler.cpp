#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::unique_ptr;
extern CDBAgent g_DBAgent;

#pragma region CUser::HandleGenie(Packet & pkt)
void CUser::HandleGenie(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();

	switch (command)
	{
	case GenieInfoRequest:
		GenieNonAttackProgress(pkt);
		break;
	case GenieUpdateRequest:
		GenieAttackProgress(pkt);
		break;
	default:
		printf("Genie OpCode %u \n", command);
		TRACE("Genie OpCode %u \n", command);
		break;
	}
}
#pragma endregion

#pragma region CUser::GenieNonAttackProgress(Packet & pkt)
void CUser::GenieNonAttackProgress(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();

	switch (command)
	{
	case GenieUseSpiringPotion:
		GenieUseGenieSpirint(pkt);
		break;
	case GenieLoadOptions:
		HandleGenieLoadOptions();
		break;
	case GenielSaveOptions:
		HandleGenieSaveOptions(pkt);
		break;
	case GenieStartHandle:
		GenieStart();
		break;
	case GenieStopHandle:
		GenieStop();
		break;
	default:
		TRACE("GenieNonAttackProgress Unknow Attack Handle %d\n", command);
		printf("GenieNonAttackProgress Unknow Attack Handle %d\n", command);
		break;
	}

}
#pragma endregion

#pragma region CUser::HandleGenieLoadOptions()
void CUser::HandleGenieLoadOptions()
{
	Packet result(WIZ_GENIE, uint8(GenieInfoRequest));
	result << uint8(GenieLoadOptions);

	for (int i = 0; i < sizeof(m_GenieOptions); i++)
		result << uint8(*(uint8 *)(m_GenieOptions + i));
	Send(&result);
}
#pragma endregion

#pragma region CUser::HandleGenieSaveOptions(Packet & pkt)
void CUser::HandleGenieSaveOptions(Packet & pkt)
{
	for (int i = 0; i < sizeof(m_GenieOptions); i++)
		*(uint8 *)(m_GenieOptions + i) = pkt.read<uint8>();
}
#pragma endregion

#pragma region CUser::GenieAttackProgress(Packet & pkt)
void CUser::GenieAttackProgress(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();

	if (m_GenieTime == 0)
	{
		SendGenieStop(true);
		return;
	}

	switch (command)
	{
	case GenieMove:
		MoveProcess(pkt);
		break;
	case GenieRotate:
		Rotate(pkt);
		break;
	case GenieMainAttack:
		Attack(pkt);
		break;
	case GenieMagic:
		CMagicProcess::MagicPacket(pkt, this);
		break;
	default:
		TRACE("Genie Unknow Attack Handle %d\n", command);
		printf("Genie Unknow Attack Handle %d\n", command);
		break;
	}
}
#pragma endregion

#pragma region CUser::GenieStart()
void CUser::GenieStart()
{
	if (m_GenieTime > 0)
	{
		Packet result(WIZ_GENIE, uint8(GenieStatusActive));
		result << uint8(4) << uint16(1) << GetGenieTime();
		m_bGenieStatus = true;
		UserInOut(INOUT_IN);
		Send(&result);
	}
	else
		m_bGenieStatus = GenieStatusInactive;

	SendGenieStart(true);
}
#pragma endregion

#pragma region CUser::SendGenieStart(bool isToRegion /* = false */)
void CUser::SendGenieStart(bool isToRegion /* = false */)
{
	Packet result(WIZ_GENIE, uint8(GenieInfoRequest));
	result << uint8(GenieStartHandle) << uint16(1) << uint16(m_GenieTime);
	Send(&result);

	Packet result2(WIZ_GENIE, uint8(GenieInfoRequest));
	result2 << uint8(GenieActivated) << uint16(GetID()) << uint8(1);

	if (isToRegion)
		SendToRegion(&result2, nullptr, GetEventRoom());
	else
		Send(&result2);

	if (m_bGenieStatus == GenieStatusInactive)
		GenieStop();
}
#pragma endregion

#pragma region CUser::GenieStop()
void CUser::GenieStop()
{
	if (m_bGenieStatus == GenieStatusInactive)
		return;

	Packet result(WIZ_GENIE, uint8(1));
	result << uint8(5) << uint16(1) << GetGenieTime();
	m_bGenieStatus = false;
	UserInOut(INOUT_IN);
	SendGenieStop(true);

	Send(&result);
}
#pragma endregion

void CUser::SendGenieStop(bool isToRegion /* = false */)
{
	Packet result(WIZ_GENIE, uint8(GenieInfoRequest));
	if (m_GenieTime > 0)
		result << uint8(GenieStopHandle) << uint16(1) << uint16(m_GenieTime);
	else
		result << uint8(GenieStopHandle) << uint16(1) << uint16(m_GenieTime);

	Send(&result);

	Packet result2(WIZ_GENIE, uint8(GenieInfoRequest));
	result2 << uint8(GenieActivated) << uint16(GetID()) << uint8(0);

	if (isToRegion)
		SendToRegion(&result2, nullptr, GetEventRoom());
	else
		Send(&result2);
}

#pragma region CUser::UpdateGenieTime(uint16 m_sTime)
void CUser::UpdateGenieTime(uint16 m_sTime)
{
	Packet result(WIZ_GENIE, uint8(GenieInfoRequest));
	result << uint8(GenieRemainingTime) << m_sTime;
	Send(&result);

	if (m_sTime == GenieStatusInactive)
		GenieStop();
}
#pragma endregion

#pragma region CUser::GenieUseGenieSpirint()
void CUser::GenieUseGenieSpirint(Packet & pkt)
{
	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing())
		return;

	uint32 nItemID;
	uint16 GenieItem;
	pkt >> nItemID;

	_ITEM_TABLE* ItemTable = g_pMain->GetItemPtr(nItemID);
	if (ItemTable == nullptr)
		return;

	GenieItem = GetItemCount(nItemID);

	if (!CheckExistItem(nItemID))
		return;

	if (nItemID != 810305000 
		&& nItemID != 810378000 
		&& nItemID != 900772000)
		return;

	if (RobItem(nItemID))
		m_GenieTime = 120;

	if (m_sFirstUsingGenie <= 0)
		m_sFirstUsingGenie = 1;

	Packet result(WIZ_GENIE, uint8(GenieUseSpiringPotion));
	result << uint8(GenieUseSpiringPotion) << GetGenieTime();
	Send(&result);
}
#pragma endregion

bool CDBAgent::UpdateGenieData(string & strCharID, CUser *pUser)
{
	if (strCharID != pUser->GetName())
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)pUser->m_GenieOptions, sizeof(pUser->m_GenieOptions), SQL_BINARY);
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_GENIE_DATA(?, ?, %d, %d)}"), pUser->m_GenieTime, pUser->m_sFirstUsingGenie)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

bool CDBAgent::LoadGenieData(string & strCharID, CUser *pUser)
{
	if (pUser == nullptr
		|| strCharID != pUser->GetName())
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_GENIE_DATA(?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	int field = 1;
	dbCommand->FetchUInt16(field++, pUser->m_GenieTime);
	dbCommand->FetchBinary(field++, (char *)pUser->m_GenieOptions, sizeof(pUser->m_GenieOptions));
	dbCommand->FetchByte(field++, pUser->m_sFirstUsingGenie);

	return true;
}
#pragma endregion