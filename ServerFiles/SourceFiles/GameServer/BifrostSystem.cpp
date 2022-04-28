#include "stdafx.h"

#pragma region Bifrost System
#pragma region CGameServerDlg::Timer_BifrostTime(void * lpParam)
uint32 CGameServerDlg::Timer_BifrostTime(void * lpParam)
{
	while (g_bRunning)
	{
		if (g_pMain->m_BifrostVictory == 0)
		{
			g_pMain->m_sBifrostRemainingTime -= 60;

			if (g_pMain->m_sBifrostRemainingTime == 0 && !g_pMain->m_sBifrostWarStart)
			{
				g_pMain->m_BifrostVictory = 0;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_START, Nation::ALL, false);

				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;

				g_pMain->m_sBifrostWarStart = true;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostMonumentAttackTime;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
			}
			else if (g_pMain->m_sBifrostRemainingTime == 0 && g_pMain->m_sBifrostWarStart)
			{
				g_pMain->m_sBifrostWarStart = false;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;
			}
		}
		else if (g_pMain->m_BifrostVictory != 0)
		{
			g_pMain->m_sBifrostTime -= 60;
			g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;

			if (g_pMain->m_sBifrostTime <= 90 * MINUTE && !g_pMain->m_sBifrostVictoryNoticeAll)
			{
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_DRAW, Nation::ALL, false);
				g_pMain->m_sBifrostVictoryNoticeAll = true;
			}

			if (g_pMain->m_sBifrostTime == 0)
			{
				g_pMain->m_BifrostVictory = 0;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;

				g_pMain->m_sBifrostWarStart = false;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
				g_pMain->KickOutZoneUsers(ZONE_BIFROST, ZONE_RONARK_LAND);
			}
			else if (g_pMain->m_sBifrostTime == g_pMain->m_xJoinOtherNationBifrostTime)
			{
				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;
			}
		}

		sleep(60 * SECOND);
	}
	return 0;
}
#pragma endregion

#pragma region CUser::BifrostProcess(CUser * pUser)
void CUser::BifrostProcess(CUser * pUser)
{
	if (pUser == nullptr)
		return;

	if (g_pMain->m_BifrostVictory == 0 && g_pMain->m_bAttackBifrostMonument)
	{
		g_pMain->m_sBifrostTime = g_pMain->m_xBifrostTime;
		g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;
		g_pMain->m_BifrostVictory = pUser->GetNation();
		g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL, false);
		g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);

		if (g_pMain->m_bAttackBifrostMonument)
			g_pMain->m_bAttackBifrostMonument = false;
	}
	else if (g_pMain->m_BifrostVictory == 1 || g_pMain->m_BifrostVictory == 2)
	{
		if (pUser->GetNation() != g_pMain->m_BifrostVictory && g_pMain->m_bAttackBifrostMonument)
		{
			g_pMain->m_BifrostVictory = 3;
			g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL, false);

			if (g_pMain->m_bAttackBifrostMonument)
				g_pMain->m_bAttackBifrostMonument = false;
		}
	}
}
#pragma endregion
#pragma endregion