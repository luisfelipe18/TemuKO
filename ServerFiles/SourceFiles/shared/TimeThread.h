#pragma once

void StartTimeThread();
void CleanupTimeThread();

uint32 THREADCALL TimeThread(void * lpParam);

extern time_t UNIXTIME;
extern ULONGLONG UNIXTIME2;
extern tm g_localTime;