#pragma once
class CTimer;
#include "stdafx.h"

class CTimer
{
public:
	CTimer(bool millisecond = false, float elapseInterval = 1.0f);
	~CTimer();

	bool m_bIsMillisecond;
	float m_fElapseInterval;

	time_t UNIXTIME;
	ULONGLONG UNIXTIME2;
	tm g_localTime;
	int32 lastElapse;
	ULONGLONG lastElapse2;

	void Tick();
	bool IsElapsedSecond();
private:

};