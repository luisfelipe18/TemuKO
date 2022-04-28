#include "stdafx.h"

void CUser::EventTrapProcess(float x, float z, CUser * pUser)
{
	if (pUser == nullptr)
		return;

	if (pUser != nullptr)
	{
		switch (pUser->GetZoneID())
		{
		case ZONE_KROWAZ_DOMINION:
		{
			if (pUser->isGM() || isGM())
				return;

			g_pMain->m_ObjectEventArray.m_lock.lock();
			foreach_stlmap(itr, g_pMain->m_ObjectEventArray)
			{
				if (itr->second->sZoneID != pUser->GetZoneID())
					continue;

				if (itr->second->sType != OBJECT_POISONGAS)
					continue;

				float m_sNewX = itr->second->fPosX, m_sOldX = itr->second->fPosX;
				float m_sNewZ = itr->second->fPosZ, m_sOldZ = itr->second->fPosZ;

				m_sNewX -= 7, m_sOldX += 7;
				m_sNewZ += 3, m_sOldZ -= 3;

				if ((x >= m_sNewX && x <= m_sOldX) && (z >= m_sOldZ && z <= m_sNewZ))
					pUser->HpChange(-1500, pUser);
			}
			g_pMain->m_ObjectEventArray.m_lock.unlock();
		}
		break;
		case ZONE_UNDER_CASTLE:
			if (isInRangeSlow(646.0f, 236.0f, 5))
			{
				uint16 x, z;
				x = 824;
				z = 932;

				pUser->Warp(x * 10, z * 10);
			}
			break;
		default:
			break;
		}
	}
}

#pragma region CUser::UserWallCheatCheckRegion()
void CUser::UserWallCheatCheckRegion()
{
	if (isGM() || !isInGame())
		return;

	switch (GetZoneID())
	{
	case ZONE_KARUS:
	case ZONE_KARUS2:
	case ZONE_KARUS3:
	{
		if (isInRangeSlow(461.0f, 1690.0f, 2)
			|| isInRangeSlow(481.0f, 1676.0f, 2)
			|| isInRangeSlow(424.0f, 1730.0f, 2)
			|| isInRangeSlow(393.0f, 1756.0f, 2)
			|| isInRangeSlow(297.0f, 1793.0f, 10)
			|| isInRangeSlow(965.0f, 826.0f, 10)
			|| isInRangeSlow(42.0f, 905.0f, 30)
			|| isInRangeSlow(1845.0f, 1860.0f, 100)
			|| isInRangeSlow(2021.0f, 1817.0f, 70)
			|| isInRangeSlow(1915.0f, 1754.0f, 30)
			|| isInRangeSlow(1851.0f, 1694.0f, 30)
			|| isInRangeSlow(1718.0f, 1691.0f, 30)
			|| isInRangeSlow(1677.0f, 1626.0f, 15)
			|| isInRangeSlow(1632.0f, 1563.0f, 10)
			|| isInRangeSlow(1595.0f, 1494.0f, 10)
			|| isInRangeSlow(1530.0f, 1383.0f, 10)
			|| isInRangeSlow(1295.0f, 835.0f, 40))
			ZoneChange(ZONE_PRISON, 170, 146);
	}
	break;
	case ZONE_ELMORAD:
	case ZONE_ELMORAD2:
	case ZONE_ELMORAD3:
	{
		if (isInRangeSlow(2013.0f, 1153.0f, 30)
			|| isInRangeSlow(2025.0f, 479.0f, 30)
			|| isInRangeSlow(1761.0f, 264.0f, 25)
			|| isInRangeSlow(1593.0f, 375.0f, 2)
			|| isInRangeSlow(1662.0f, 419.0f, 2)
			|| isInRangeSlow(1703.0f, 379.0f, 3)
			|| isInRangeSlow(789.0f, 1250.0f, 50)
			|| isInRangeSlow(877.0f, 1203.0f, 50)
			|| isInRangeSlow(735.0f, 1149.0f, 50)
			|| isInRangeSlow(705.0f, 1059.0f, 20)
			|| isInRangeSlow(695.0f, 993.0f, 20)
			|| isInRangeSlow(707.0f, 862.0f, 5)
			|| isInRangeSlow(221.0f, 228.0f, 60)
			|| isInRangeSlow(315.0f, 331.0f, 15)
			|| isInRangeSlow(367.0f, 409.0f, 10)
			|| isInRangeSlow(419.0f, 476.0f, 10)
			|| isInRangeSlow(459.0f, 581.0f, 7)
			|| isInRangeSlow(546.0f, 678.0f, 7)
			|| isInRangeSlow(246.0f, 83.0f, 60)
			|| isInRangeSlow(169.0f, 176.0f, 20)
			|| isInRangeSlow(188.0f, 327.0f, 20)
			|| isInRangeSlow(1578.0f, 1975.0f, 40))
			ZoneChange(ZONE_PRISON, 170, 146);
	}
	break;
	case ZONE_MORADON:
	case ZONE_MORADON2:
	case ZONE_MORADON3:
	case ZONE_MORADON4:
	case ZONE_MORADON5:
	{
		if (isInRangeSlow(756.0f, 511.0f, 2)
			|| isInRangeSlow(765.0f, 511.0f, 2)
			|| isInRangeSlow(721.0f, 503.0f, 2)
			|| isInRangeSlow(960.0f, 311.0f, 40)
			|| isInRangeSlow(851.0f, 269.0f, 15)
			|| isInRangeSlow(606.0f, 33.0f, 4))
			ZoneChange(ZONE_PRISON, 170, 146);
	}
	break;
	case ZONE_RONARK_LAND:
	{
		if (isInRangeSlow(1170.0f, 1038.0f, 5)
			|| isInRangeSlow(1144.0f, 1076.0f, 5)
			|| isInRangeSlow(1132.0f, 1091.0f, 5)
			|| isInRangeSlow(1172.0f, 1007.0f, 5)
			|| isInRangeSlow(1098.0f, 699.0f, 5)
			|| isInRangeSlow(868.0f, 992.0f, 5)
			|| isInRangeSlow(865.0f, 955.0f, 5)
			|| isInRangeSlow(879.0f, 912.0f, 5)
			|| isInRangeSlow(506.0f, 1926.0f, 70)
			|| isInRangeSlow(715.0f, 1806.0f, 40)
			|| isInRangeSlow(867.0f, 1851.0f, 40)
			|| isInRangeSlow(867.0f, 1851.0f, 40)
			|| isInRangeSlow(929.0f, 1713.0f, 40)
			|| isInRangeSlow(657.0f, 1746.0f, 20)
			|| isInRangeSlow(468.0f, 1620.0f, 30)
			|| isInRangeSlow(312.0f, 1512.0f, 30)
			|| isInRangeSlow(184.0f, 1347.0f, 30))
			ZoneChange(ZONE_PRISON, 170, 146);
	}
	break;
	default:
		break;
	}

}
#pragma endregion

#pragma region CUser::OreadsZoneTerrainEvent()
void CUser::OreadsZoneTerrainEvent()
{
#pragma region Old Packet
	/*
	Packet results(WIZ_TERRAIN_EFFECTS);
	if (isInRangeSlow(326.0f, 250.0f, 35)
	|| isInRangeSlow(216.0f, 221.0f, 70)
	|| isInRangeSlow(275.0f, 284.0f, 20)
	|| isInRangeSlow(246.0f, 143.0f, 20)
	|| isInRangeSlow(218.0f, 135.0f, 20)
	|| isInRangeSlow(186.0f, 153.0f, 45)
	|| isInRangeSlow(304.0f, 316.0f, 25)
	|| isInRangeSlow(324.0f, 57.0f, 30)
	|| isInRangeSlow(295.0f, 49.0f, 30)
	|| isInRangeSlow(251.0f, 45.0f, 30)
	|| isInRangeSlow(226.0f, 55.0f, 30)
	|| isInRangeSlow(198.0f, 74.0f, 30)
	|| isInRangeSlow(185.0f, 88.0f, 15)
	|| isInRangeSlow(296.0f, 277.0f, 30))
	results << uint8(1) << uint8(4); // Terrain: Swamp- Decreases character speed.
	else if (isInRangeSlow(711.0f, 679.0f, 30)
	|| isInRangeSlow(656.0f, 622.0f, 70)
	|| isInRangeSlow(696.0f, 519.0f, 70)
	|| isInRangeSlow(611.0f, 514.0f, 70)
	|| isInRangeSlow(512.0f, 497.0f, 70)
	|| isInRangeSlow(420.0f, 465.0f, 70)
	|| isInRangeSlow(314.0f, 504.0f, 30)
	|| isInRangeSlow(393.0f, 381.0f, 40)
	|| isInRangeSlow(350.0f, 384.0f, 30)
	|| isInRangeSlow(329.0f, 449.0f, 30)
	|| isInRangeSlow(383.0f, 393.0f, 30)
	|| isInRangeSlow(448.0f, 367.0f, 40)
	|| isInRangeSlow(512.0f, 421.0f, 30)
	|| isInRangeSlow(324.0f, 347.0f, 15)
	|| isInRangeSlow(325.0f, 398.0f, 30)
	|| isInRangeSlow(422.0f, 560.0f, 30)
	|| isInRangeSlow(503.0f, 600.0f, 30)
	|| isInRangeSlow(560.0f, 657.0f, 30)
	|| isInRangeSlow(591.0f, 689.0f, 30)
	|| isInRangeSlow(655.0f, 425.0f, 30)
	|| isInRangeSlow(696.0f, 445.0f, 60)
	|| isInRangeSlow(545.0f, 253.0f, 80)
	|| isInRangeSlow(430.0f, 223.0f, 80)
	|| isInRangeSlow(544.0f, 119.0f, 40)
	|| isInRangeSlow(637.0f, 321.0f, 50)
	|| isInRangeSlow(640.0f, 357.0f, 40)
	|| isInRangeSlow(227.0f, 545.0f, 50)
	|| isInRangeSlow(163.0f, 553.0f, 40)
	|| isInRangeSlow(85.0f, 601.0f, 40)
	|| isInRangeSlow(66.0f, 680.0f, 25)
	|| isInRangeSlow(473.0f, 953.0f, 40)
	|| isInRangeSlow(464.0f, 869.0f, 40)
	|| isInRangeSlow(470.0f, 574.0f, 40)
	|| isInRangeSlow(791.0f, 465.0f, 40)
	|| isInRangeSlow(847.0f, 447.0f, 40)
	|| isInRangeSlow(930.0f, 425.0f, 40)
	|| isInRangeSlow(961.0f, 340.0f, 40)
	|| isInRangeSlow(320.0f, 542.0f, 80)
	|| isInRangeSlow(370.0f, 661.0f, 40)
	|| isInRangeSlow(421.0f, 760.0f, 40)
	|| isInRangeSlow(570.0f, 820.0f, 40))
	results << uint8(1) << uint8(1); //Terrain: Hay- Boosts Fire Magic,but weakens ýce and lightning damage.
	else if (isInRangeSlow(733.0f, 705.0f, 30)
	|| isInRangeSlow(762.0f, 744.0f, 40)
	|| isInRangeSlow(801.0f, 783.0f, 50)
	|| isInRangeSlow(740.0f, 714.0f, 25)
	|| isInRangeSlow(670.0f, 881.0f, 70)
	|| isInRangeSlow(720.0f, 938.0f, 40)
	|| isInRangeSlow(785.0f, 943.0f, 40)
	|| isInRangeSlow(835.0f, 927.0f, 40)
	|| isInRangeSlow(870.0f, 890.0f, 50)
	|| isInRangeSlow(894.0f, 841.0f, 60)
	|| isInRangeSlow(842.0f, 803.0f, 70)
	|| isInRangeSlow(771.0f, 812.0f, 40))
	results << uint8(1) << uint8(2); //Terrain: Swamp- Boosts all magic damage but decreases character speed.
	else
	results << uint8(1) << uint8(0); // empty.
	Send(&results);
	*/
#pragma endregion

	if (g_pMain->m_byBattleOpen != NATION_BATTLE
		|| GetZoneID() != ZONE_BATTLE6
		|| isGM())
	{
		Packet result(WIZ_TERRAIN_EFFECTS, uint8(0x01));
		result << uint8(0);
		Send(&result);
		return;
	}

	float PosX = GetX(), PosZ = GetZ();
	uint8 m_bSelectedTerrain = 0;

#pragma region HAY Terrain  -- Boost Flame Damage but weakens lightning and cold
	if (((PosX >= 531 && PosX <= 713) && (PosZ >= 447 && PosZ <= 690))   // Area 1
		|| ((PosX >= 375 && PosX <= 527) && (PosZ >= 392 && PosZ <= 612)) // Area 2
		|| ((PosX >= 285 && PosX <= 423) && (PosZ >= 344 && PosZ <= 569)) // Area 3
		|| ((PosX >= 263 && PosX <= 319) && (PosZ >= 461 && PosZ <= 537)) // Area 4 
		|| ((PosX >= 591 && PosX <= 669) && (PosZ >= 416 && PosZ <= 493)))// Area 5
	{
		m_bSelectedTerrain = 1;
		goto send_terrain;
	}
#pragma endregion

#pragma region Swamp Terrain -- Speed Slow all magic damage boost but spped decreases
	if (((PosX >= 619 && PosX <= 984) && (PosZ >= 714 && PosZ <= 970)))  // Area 1
	{
		m_bSelectedTerrain = 2;
		goto send_terrain;
	}
#pragma endregion

#pragma region Driedriver Terrain -- Weakens fire magic but cold and light magic boost
	if (((PosX >= 138 && PosX <= 373) && (PosZ >= 62 && PosZ <= 306)))  // Area 1
	{
		m_bSelectedTerrain = 3;
		goto send_terrain;
	}
#pragma endregion

send_terrain:
	Packet result(WIZ_TERRAIN_EFFECTS, uint8(0x01));
	result << m_bSelectedTerrain;
	Send(&result);
}
#pragma endregion
