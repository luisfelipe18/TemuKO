#include "stdafx.h"
#include "MagicInstance.h"

bool CNpc::isMoral2Checking(Unit* pTarget, _MAGIC_TABLE* pSkill)
{
	if (pTarget == nullptr
		|| pSkill == nullptr
		|| GetEventRoom() != pTarget->GetEventRoom()
		|| !isMonster())
		return true;

	if (pSkill->bMoral == MORAL_FRIEND_WITHME)
	{
		if (TO_USER(pTarget)->isPriest())
		{
			if (pSkill->iNum == 111554 ||
				pSkill->iNum == 112554 ||
				pSkill->iNum == 211554 ||
				pSkill->iNum == 212554)
			{
				_ITEM_TABLE* pTable = TO_USER(pTarget)->GetItemPrototype(TO_USER(pTarget)->GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND);
				if (pTable != nullptr)
				{
					if (pTable->isDarkKnightMace())
					{
						return false;
					}
				}
			}
		}
		if (isHealer())
			return false;
	}

	return true;
}


bool CUser::isMoral2Checking(Unit* pTarget, _MAGIC_TABLE* pSkill)
{
	if (pTarget == nullptr
		|| pSkill == nullptr
		|| GetEventRoom() != pTarget->GetEventRoom())
		return true;

	// For non-player hostility checks, refer to the appropriate override.
	if (!pTarget->isPlayer())
		return pTarget->isMoral2Checking(this, pSkill);

	if (!isInArena() && GetZoneID() != ZONE_DELOS)
	{
		if (GetNation() != pTarget->GetNation()
			&& TO_USER(pTarget)->isInOwnSafetyArea())
			return true;
	}

	if (pSkill->bMoral == MORAL_FRIEND_WITHME)
	{
		if (isPriest())
		{
			if (pSkill->iNum == 111545 ||
				pSkill->iNum == 112545 ||
				pSkill->iNum == 211545 ||
				pSkill->iNum == 212545)
			{
				_ITEM_TABLE* pTable = GetItemPrototype(GetZoneID() == ZONE_KNIGHT_ROYALE ? KNIGHT_ROYAL_RIGHTHAND : RIGHTHAND);
				if (pTable != nullptr)
				{
					if (pTable->isDarkKnightMace())
					{
						if (isInArena() && TO_USER(pTarget)->isInArena())
						{
							if (isInMeleeArena() && TO_USER(pTarget)->isInMeleeArena())
							{
								return false;
							}
							else if (isInPartyArena() && TO_USER(pTarget)->isInPartyArena())
							{
								if (isInParty() && GetPartyID() != TO_USER(pTarget)->GetPartyID())
									return false;

								if (!isInParty())
									return false;
							}
							else if (GetZoneID() == ZONE_ARENA)
							{
								if (isInClanArena() && TO_USER(pTarget)->isInClanArena())
								{
									if (!isInClan() || !TO_USER(pTarget)->isInClan())
										return true;
								}

								if (isInClanArena() && !TO_USER(pTarget)->isInClanArena())
									return true;

								if (!isInClanArena() && TO_USER(pTarget)->isInClanArena())
									return true;

								if (isInEnemySafetyArea() && !TO_USER(pTarget)->isInEnemySafetyArea())
									return true;

								if (!isInEnemySafetyArea() && TO_USER(pTarget)->isInEnemySafetyArea())
									return true;

								return false;
							}
						}

						if (GetZoneID() == ZONE_DELOS)
						{
							CKnights* pKnightsSource = g_pMain->GetClanPtr(GetClanID());
							CKnights* pKnightsTarget = g_pMain->GetClanPtr(TO_USER(pTarget)->GetClanID());

							if (g_pMain->pCswEvent.Started)
							{
								if (!isInClan() || !TO_USER(pTarget)->isInClan())
									return true;

								if (pKnightsSource != nullptr && pKnightsTarget != nullptr)
								{
									if (isInEnemySafetyArea()
										|| isInOwnSafetyArea()
										|| TO_USER(pTarget)->isInEnemySafetyArea()
										|| TO_USER(pTarget)->isInOwnSafetyArea())
									{
										if (pKnightsSource->GetID() == pKnightsTarget->GetID())
											return false;
										else
											return true;
									}

									if (g_pMain->pCswEvent.Status == CswOperationStatus::DeathMatch)
									{
										_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
										_DEATHMATCH_WAR_INFO* pDeathmatchSource = g_pMain->m_KnightSiegeWarClanList.GetData(GetClanID());
										_DEATHMATCH_WAR_INFO* pDeathmatchTarget = g_pMain->m_KnightSiegeWarClanList.GetData(TO_USER(pTarget)->GetClanID());
										if (pKnightSiege != nullptr)
										{
											if (pDeathmatchSource != nullptr && pDeathmatchTarget != nullptr)
												return false;
										}
									}
									else if (g_pMain->pCswEvent.Status == CswOperationStatus::CastellanWar)
									{
										_KNIGHTS_SIEGE_WARFARE* pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(Aktive);
										if (pKnightSiege != nullptr)
										{
											_CASTELLAN_OLD_CSW_WINNER* pOldWinnerSource = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(GetClanID());
											if (pOldWinnerSource != nullptr)
											{
												_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(TO_USER(pTarget)->GetClanID());
												if (pInfo1 != nullptr && (GetClanID() != TO_USER(pTarget)->GetClanID()))
													return false;
											}
											else
											{
												if (pKnightSiege->sMasterKnights == GetClanID())
												{
													_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(TO_USER(pTarget)->GetClanID());
													if (pInfo1 != nullptr && (GetClanID() != TO_USER(pTarget)->GetClanID()))
														return false;
												}
											}

											_CASTELLAN_WAR_INFO* pInfo1 = g_pMain->m_KnightSiegeWarCastellanClanList.GetData(GetClanID());
											if (pInfo1 != nullptr && GetClanID() != pKnightSiege->sMasterKnights)
											{
												_CASTELLAN_OLD_CSW_WINNER* pOldWinnerTarget = g_pMain->m_KnightSiegeWarCastellanOldWinner.GetData(TO_USER(pTarget)->GetClanID());
												if (pOldWinnerTarget != nullptr)
													return false;
												else
												{
													if (TO_USER(pTarget)->GetClanID() == pKnightSiege->sMasterKnights)
														return false;
												}
											}
										}
									}
								}
							}
							if (pKnightsSource != nullptr && pKnightsTarget != nullptr)
							{
								if (pKnightsSource->GetID() == pKnightsTarget->GetID())
									return false;
							}

							return true;
						}

						if (GetNation() != pTarget->GetNation())
						{
							if (isInPVPZone())
								return false;

							if ((GetZoneID() == ZONE_DESPERATION_ABYSS
								|| GetZoneID() == ZONE_HELL_ABYSS
								|| GetZoneID() == ZONE_DRAGON_CAVE))
								return false;
						}
					}
				}
			}
		}

		// Players can attack other players in the Moradon arena.
		if (isInArena() && TO_USER(pTarget)->isInArena())
		{
			if (isInPartyArena())
			{
				if (isInParty() && GetPartyID() == TO_USER(pTarget)->GetPartyID())
					return false;
				else
					return true;
			}
			else if (isInMeleeArena() && TO_USER(pTarget)->isInMeleeArena())
				return true;

			// Players can attack other players in the arena.
			if (GetZoneID() == ZONE_ARENA)
			{
				if (isInClanArena() && TO_USER(pTarget)->isInClanArena())
				{
					if (isInClan() && TO_USER(pTarget)->isInClan()
						&& GetClanID() == TO_USER(pTarget)->GetClanID())
						return false;
				}

				if (isInEnemySafetyArea() && TO_USER(pTarget)->isInEnemySafetyArea())
					return false;

				return true;
			}
		}

		if (GetZoneID() == ZONE_DELOS)
		{
			CKnights* pKnightsSource = g_pMain->GetClanPtr(GetClanID());
			CKnights* pKnightsTarget = g_pMain->GetClanPtr(TO_USER(pTarget)->GetClanID());
			if (pKnightsSource != nullptr && pKnightsTarget != nullptr)
			{
				if (pKnightsSource->GetID() == pKnightsTarget->GetID())
					return false;
			}

			return true;
		}

		if (GetNation() != pTarget->GetNation())
		{
			if (isInPVPZone())
				return true;

			if ((GetZoneID() == ZONE_DESPERATION_ABYSS
				|| GetZoneID() == ZONE_HELL_ABYSS
				|| GetZoneID() == ZONE_DRAGON_CAVE))
				return true;
		}
	}
	return false;
}

bool CNpc::isMoral7Checking(Unit* pTarget, _MAGIC_TABLE* pSkill)
{

	return false;
}

bool CUser::isMoral7Checking(Unit* pTarget, _MAGIC_TABLE* pSkill)
{
	// For non-player hostility checks, refer to the appropriate override.
	if (!pTarget->isPlayer())
		return pTarget->isMoral7Checking(this, pSkill);

	return false;
}