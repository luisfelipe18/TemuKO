#include "stdafx.h"
#include "Map.h"

#pragma region CUser::MoveProcess(Packet & pkt)
void CUser::MoveProcess(Packet & pkt)
{
	if(!isInGame() || GetMap() == nullptr)
		return;

	if (m_bWarp || isDead()) 
		return;

	uint16 will_x, will_z, will_y;
	int16 speed = 0;
	float real_x, real_z, real_y;
	uint8 echo;
	uint16 fWillX = m_oldwillx, fWillZ = m_oldwillz;

	pkt >> will_x >> will_z >> will_y >> speed >> echo >> curX1 >> curZ1;

	if(fWillX == 0)
		fWillX = will_x;
	if(fWillZ == 0)
		fWillZ = will_z;

	if(m_sSpeed == 0 && echo == 1)
	{
		will_x = (will_x + curX1) / 2;
		will_z = (will_z + curZ1) / 2;
	}
	else if(speed)
	{
		if(GetDistance(fWillX / 10.0f, fWillZ / 10.0f, will_x / 10.0f, will_z / 10.0f) / speed > 8.0f 
			&& GetDistance(fWillX / 10.0f, fWillZ / 10.0f, will_x / 10.0f, will_z / 10.0f) / speed < 10.0f)
		{
			will_x = (will_x + curX1) / 2;
			will_z = (will_z + curZ1) / 2;
		}
		else if(GetDistance(fWillX / 10.0f, fWillZ / 10.0f, will_x / 10.0f, will_z / 10.0f) / speed >= 12.0f)
		{
			will_x = curX1;
			will_z = curZ1;
		}
	}

	real_x = will_x / 10.0f; real_z = will_z / 10.0f; real_y = will_y / 10.0f;

	m_sSpeed = speed;
	SpeedHackUser();

	m_oldwillx = will_x; m_oldwillz = will_z;

	if (!GetMap()->IsValidPosition(real_x, real_z, real_y)) 
		return;

	if (m_oldx != GetX()
		|| m_oldz != GetZ())
	{
		m_oldx = GetX();
		m_oldy = GetY();
		m_oldz = GetZ();
	}

	// TODO: Ensure this is checked properly to prevent speedhacking
	SetPosition(real_x, real_y, real_z);

	if (RegisterRegion())
	{
		g_pMain->RegionNpcInfoForMe(this);
		g_pMain->RegionUserInOutForMe(this);
		g_pMain->MerchantUserInOutForMe(this);
	}

	if (m_PettingOn)
	{
		CNpc *pPet = g_pMain->GetPetPtr(GetSocketID(), GetZoneID());
		if (pPet) {
			if ((pPet->GetState() == NPC_STANDING
				|| pPet->GetState() == NPC_MOVING)
				&& (speed == 0
					|| GetDistanceSqrt(pPet) >= 10))
			{
				float	warp_x = pPet->GetX() - GetX(),
					warp_z = pPet->GetZ() - GetZ();

				// Unable to work out orientation, so we'll just fail (won't be necessary with m_sDirection).
				float	distance = sqrtf(warp_x * warp_x + warp_z * warp_z);
				if (distance == 0.0f)
					goto fail_return;

				if (m_PettingOn->sStateChange == MODE_ATTACK) {
					if (m_PettingOn->sAttackStart)
					{
						m_PettingOn->sAttackStart = false;
						m_PettingOn->sAttackTargetID = -1;
					}
				}
				warp_x /= distance; warp_z /= distance;
				warp_x *= 2; warp_z *= 2;
				warp_x += m_oldx; warp_z += m_oldz;

				pPet->SendMoveResult(warp_x, 0, warp_z, distance);
			}
		}
	}
fail_return:
	if (m_bInvisibilityType == INVIS_DISPEL_ON_MOVE)
		CMagicProcess::RemoveStealth(this, INVIS_DISPEL_ON_MOVE);

	if (isMining())
		HandleMiningStop((Packet)(WIZ_MINING, MiningStop));

	if (isFishing())
		HandleFishingStop((Packet)(WIZ_MINING, FishingStop));

	Packet result(WIZ_MOVE);
	result << GetSocketID() << will_x << will_z << will_y << speed << echo;
	
	if (GetEventRoom() > 0)
		SendToRegion(&result, this, GetEventRoom());
	else
		SendToRegion(&result);

	GetMap()->CheckEvent(real_x, real_z, this);
	EventTrapProcess(real_x, real_z, this);
	OreadsZoneTerrainEvent();
	UserWallCheatCheckRegion();
	BDWMonumentPointProcess();
	KnightRoyaleAreaControl();
}
#pragma endregion 

