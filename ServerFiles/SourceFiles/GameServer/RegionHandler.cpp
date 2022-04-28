#include "stdafx.h"
#include "Map.h"

#pragma region CUser::AddToRegion(int16 new_region_x, int16 new_region_z)
void CUser::AddToRegion(int16 new_region_x, int16 new_region_z)
{
	if (GetRegion())
		GetRegion()->Remove(this);

	SetRegion(new_region_x, new_region_z);

	if (GetRegion())
		GetRegion()->Add(this);
}
#pragma endregion

void CUser::GetInOut(Packet & result, uint8 bType)
{
	result.Initialize(WIZ_USER_INOUT);
	result << uint16(bType) << GetID();
	if (bType != INOUT_OUT)
		GetUserInfo(result);
}

void CUser::UserInOut(uint8 bType)
{
	if (GetRegion() == nullptr)
		return;

	if (isGM())
	{
		if (bType == INOUT_IN)
		{
			if (m_bAbnormalType == ABNORMAL_INVISIBLE)
				return;
		}
	}

	Packet result;

	GetInOut(result, bType);

	if (bType == INOUT_OUT)
		GetRegion()->Remove(this);
	else
		GetRegion()->Add(this);

	if (m_bAbnormalType != ABNORMAL_INVISIBLE)
	{
		if (GetEventRoom() > 0)
			SendToRegion(&result, this, GetEventRoom());
		else
			SendToRegion(&result);
	}
}

void CUser::GmInOut(uint8 bType)
{
	Packet result;
	if (GetRegion() == nullptr)
		return;

	ResetGMVisibility();

	GmGetInOut(result, bType);
	bType == INOUT_OUT ? GetRegion()->Remove(this) : GetRegion()->Add(this);
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, this, GetEventRoom());
	else
		SendToRegion(&result);
}

void CUser::GmGetInOut(Packet& result, uint8 bType)
{
	result.Initialize(WIZ_USER_INOUT);
	result << uint16(bType) << GetID();
	GetUserInfo(result);
}