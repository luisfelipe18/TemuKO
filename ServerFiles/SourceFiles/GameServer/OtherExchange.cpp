#include "stdafx.h"

void CUser::OtherExchangeSystem()
{
	CNpc * pNpc = g_pMain->GetNpcPtr(GetTargetID(), GetZoneID());
	if (pNpc == nullptr)
		return;

	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| isFishing())
		return;


}