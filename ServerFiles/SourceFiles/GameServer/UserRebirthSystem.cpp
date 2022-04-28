#include "stdafx.h"
/** xsafeguard.com
* Game Security System
*/
#pragma region CUserRebirthSystemActivity()

void CUser::RebirthSystemActivity()
{
	CNpc* RebirthNpcControl = nullptr;

	RebirthNpcControl = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());

	if (RebirthNpcControl == nullptr)
		return;

	if (GetLevel() != MAX_LEVEL)
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : Reb Level ve Exp experience you must have."));
		return;
	}

	if (RebirthNpcControl->GetProtoID() != 19005)
		return;

	if (m_iExp != m_iMaxExp && GetRebirthLevel() == 0)
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : Reb Level :%d ve % %llu experience you must have.", GetRebirthLevel(), m_iMaxExp));
		return;
	}
	else if (m_iExp != m_iMaxExp)
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : %llu experience you must have.", m_iMaxExp));
		return;
	}

	if (GetRebirthLevel() >= 10)
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : Congratulations : %d Rebirth Leveliniz Son", GetRebirthLevel()));
		return;
	}

	if (GetCoins() < uint32(100000000))
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : %d Noah'a you must have", GetCoins()));
		return;
	}

	if (GetLoyalty() < uint32(10000))
	{
		g_pMain->SendHelpDescription(this, string_format("[Tarot Reader] Mackin : %d Nation Point'e you must have.", GetLoyalty()));
		return;
	}

	if (CheckExistItem(900579000, 1))
	{
		g_pMain->SendHelpDescription(this, "[Tarot Reader] Mackin : Qualification of Rebirth you can't retrieve your stuff again");
		return;
	}

	GoldLose(100000000, true);
	SendLoyaltyChange(-10000, false, false, false);
	GiveItem(900579000, 1, true);

	g_pMain->SendHelpDescription(this, "[Tarot Reader] Mackin : Qualification of Rebirth Was delivered");
}

#pragma endregion
