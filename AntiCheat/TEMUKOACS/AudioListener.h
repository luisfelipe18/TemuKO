#pragma once
#include "stdafx.h"
#include <mmsystem.h>

class AudioListener
{
	WAVEFORMATEX wave_format;
	HWAVEOUT h_waveout;
public:
	char m_buf[16][1280 + sizeof(WAVEHDR)];
	list<char*>m_buf_free;
	AudioListener();
	~AudioListener();
	bool start();
	bool stop();
	int input(unsigned char* buf);
	bool inlineis_start() { return (h_waveout != NULL); }
};
