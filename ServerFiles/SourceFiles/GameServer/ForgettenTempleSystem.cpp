#include "stdafx.h"

void CGameServerDlg::ForgettenTempleEventTimer()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if (!m_bForgettenTempleIsActive)
	{
		foreach_stlmap(itr, m_MonsterChallengeArray)
		{
			if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 55 && nSecond == 0)
			{
				if (nHour + 1 == itr->second->bStartTime1)
					m_nForgettenTempleStartHour = itr->second->bStartTime1;
				else if (nHour + 1 == itr->second->bStartTime2)
					m_nForgettenTempleStartHour = itr->second->bStartTime2;
				else if (nHour + 1 == itr->second->bStartTime3)
					m_nForgettenTempleStartHour = itr->second->bStartTime3;

				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;

				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 59 && nSecond == 40)
			{
				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;
				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1
				|| nHour == itr->second->bStartTime2
				|| nHour == itr->second->bStartTime3) && nMinute == 0)
			{
				if (itr->second->bLevelMin == 35)
					m_nForgettenTempleChallengeTime = 1;
				else if (itr->second->bLevelMin == 46)
					m_nForgettenTempleChallengeTime = 2;
				else if (itr->second->bLevelMin == 60)
					m_nForgettenTempleChallengeTime = 3;
			}
		}

		if (m_nForgettenTempleChallengeTime > 0)
		{
			m_bForgettenTempleIsActive = true;
			m_nForgettenTempleStartTime = int32(UNIXTIME);

			foreach_stlmap(itr, m_MonsterChallengeSummonListArray)
				if (itr->second->bLevel == m_nForgettenTempleChallengeTime)
					m_nForgettenTempleLastStage++;

			if (m_nForgettenTempleLastStage>56)
				m_nForgettenTempleLastStage = 56;

			Announcement(IDS_MONSTER_CHALLENGE_OPEN);
		}
	}
	else if (m_bForgettenTempleIsActive)
	{
		if (int32(UNIXTIME) - m_nForgettenTempleStartTime == 300 /* 5 minutes */
			&& !m_bForgettenTempleSummonMonsters)
		{
			m_bForgettenTempleSummonMonsters = true;
			m_nForgettenTempleCurrentStage = 1;
			m_nForgettenTempleLastSummonTime = int32(UNIXTIME) + 30;
			Announcement(IDS_MONSTER_CHALLENGE_START);
		}
		else if (m_bForgettenTempleSummonMonsters)
		{
			foreach_stlmap(itr, m_MonsterChallengeSummonListArray)
			{
				if (m_nForgettenTempleChallengeTime != itr->second->bLevel
					|| int32(UNIXTIME) - m_nForgettenTempleLastSummonTime < 45
					|| itr->second->bStage != m_nForgettenTempleCurrentStage
					&& m_nForgettenTempleCurrentStage != 57)
					continue;

				if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage)
					m_bForgettenTempleSummonMonsters = false;
				else
				{
					SpawnEventNpc(itr->second->sSid, true, ZONE_FORGOTTEN_TEMPLE, (float)itr->second->sPosX, 0, (float)itr->second->sPosZ, itr->second->sCount, itr->second->bRange);
					m_nForgettenTempleCurrentStage++;
					m_nForgettenTempleLastSummonTime = int32(UNIXTIME);
				}
			}
		}
		else
		{
			if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage && !m_nForgettenTempleBanishFlag)
			{
				if (m_ForgettenTempleMonsterList.size() == 0)
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 200;
					Announcement(IDS_MONSTER_CHALLENGE_VICTORY);
					switch (m_nForgettenTempleChallengeTime)
					{
					case 1:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, RED_TREASURE_CHEST);
						break;
					case 2:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, LUNAR_ORDER_TOKEN);
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, GREEN_TREASURE_CHEST);
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, FORGETTEN_TEMPLE_COIN1);
						break;
					case 3:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, LUNAR_ORDER_TOKEN);
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, BLUE_TREASURE_CHEST);
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, FORGETTEN_TEMPLE_COIN2);
						break;
					}
				}
				else if (int32(UNIXTIME) - m_nForgettenTempleStartTime >= (HOUR - (10 * MINUTE)))
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 0;
				}
			}
		}

		if (m_nForgettenTempleBanishFlag)
		{
			m_nForgettenTempleBanishTime++;

			if (m_nForgettenTempleBanishTime == 30 /* 30 seconds */)
				Announcement(IDS_MONSTER_CHALLENGE_CLOSE);
			else if (m_nForgettenTempleBanishTime == 230 /* 5 minutes */)
			{
				m_nForgettenTempleBanishFlag = false;
				m_nForgettenTempleBanishTime = 0;

				m_nForgettenTempleUsers.clear();
				m_bForgettenTempleIsActive = false;
				m_nForgettenTempleStartHour = 0;
				m_nForgettenTempleLevelMin = 0;
				m_nForgettenTempleLevelMax = 0;
				m_nForgettenTempleStartTime = 0;
				m_nForgettenTempleChallengeTime = 0;
				m_bForgettenTempleSummonMonsters = false;
				m_nForgettenTempleCurrentStage = 0;
				m_nForgettenTempleLastStage = 0;

				foreach(itr, m_ForgettenTempleMonsterList)
					KillNpc(itr->first, ZONE_FORGOTTEN_TEMPLE);

				m_ForgettenTempleMonsterList.clear();
				m_nForgettenTempleLastSummonTime = 0;

				KickOutZoneUsers(ZONE_FORGOTTEN_TEMPLE);
			}
		}
	}
}