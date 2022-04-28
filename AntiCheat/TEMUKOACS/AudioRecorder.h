#pragma once
#include "stdafx.h"
#include <mmsystem.h>

#define FRAGMENT_NUM 1

class AudioRecorder
{
private:
    WAVEFORMATEX wave_format;
    HWAVEIN h_wavein;
    WAVEHDR wh[FRAGMENT_NUM];
public:
    AudioRecorder();
    ~AudioRecorder();
    char m_buf[16][1280 + sizeof(WAVEHDR)];
    void Record();
    bool Stop();
};

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);