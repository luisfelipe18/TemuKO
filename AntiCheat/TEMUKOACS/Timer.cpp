#include "stdafx.h"
#include <time.h>
#include "Timer.h"

CTimer::CTimer(bool millisecond, float elapseInterval)
{
	m_bIsMillisecond = millisecond;
	m_fElapseInterval = elapseInterval;

	UNIXTIME = time(nullptr);
	g_localTime = *localtime(&UNIXTIME);
	UNIXTIME2 = GetTickCount64();
	lastElapse = int32(UNIXTIME);
	lastElapse2 = UNIXTIME2;
}

CTimer::~CTimer()
{
}

void CTimer::Tick()
{
	if (m_bIsMillisecond)
		UNIXTIME2 = GetTickCount64();
	else
	{
		time_t t = time(nullptr);
		if (UNIXTIME != t)
		{
			UNIXTIME = t;
			g_localTime = *localtime(&t);
		}
	}

}

bool CTimer::IsElapsedSecond()
{
	Tick();

	bool isElapsed = false;
	if (m_bIsMillisecond)
	{
		if (UNIXTIME2 - lastElapse2 >= m_fElapseInterval)
		{
			isElapsed = true;
			lastElapse2 = UNIXTIME2;
		}
	}
	else
	{
		int32 elapsed = int32(UNIXTIME) - lastElapse;
		if (elapsed >= m_fElapseInterval)
		{
			isElapsed = true;
			lastElapse = int32(UNIXTIME);
		}
	}
	return isElapsed;
}