#include "stdafx.h"
#include "DBAgent.h"

void  CUser::SendKnightCash(uint32 nCashPoint /*= 0*/)
{
	if (nCashPoint == 0)
		return;

	m_nKnightCash += nCashPoint;
	g_DBAgent.UpdateAccountKnightCash(GetAccountName(), nCashPoint);

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
	result << m_nKnightCash << m_nKnightCashBonus;
	Send(&result);
}

void CUser::GiveKnightCash(uint32 nKnightCash)
{
	if (nKnightCash <= 0)
		return;

	m_nKnightCash += nKnightCash;
	g_DBAgent.UpdateAccountKnightCash(GetAccountName(), nKnightCash);

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CASHCHANGE));
	result << m_nKnightCash << m_nKnightCashBonus;
	Send(&result);
}

void CUser::GiveNPoints(uint16 sNPointAmount)
{
	if (!isInGame())
		return;

	Packet result(WIZ_DB_NPOINTS, uint8(1));
	result << sNPointAmount;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::ReqHandleNPoints(Packet & pkt)
{
	if (!isInGame())
		return;

	uint8 bOpcode = pkt.read<uint8>();

	switch (bOpcode)
	{
	case 1:
	{
		uint16 sNPointAmount = pkt.read<uint16>();
		int8 bResult = g_DBAgent.AccountAddNPoints(GetAccountName(), sNPointAmount);
		if (bResult > 0)
		{
			DateTime time;
			std::string logstr = string_format("[%d:%d:%d] Failed to add NPoints, Account ID = %s Amount = %d!\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetAccountName().c_str(), sNPointAmount);
			FILE *fp = fopen("./Logs/GameServer.log", "a");
			if (fp != nullptr)
			{
				fwrite(logstr.c_str(), logstr.length(), 1, fp);
				fclose(fp);
			}
		}
	}
	break;
	default:
		printf("NPoints unhandled packets %d \n", bOpcode);
		TRACE("NPoints unhandled packets %d \n", bOpcode);
		break;
	}
}