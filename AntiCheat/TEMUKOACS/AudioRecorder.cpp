#include "AudioRecorder.h"
#include "TEMUKOACS Engine.h"
#include <cctype>

#define MAX_BUFFERS 3

bool findString(const std::string& strHaystack, const std::string& strNeedle)
{
    auto it = std::search(
        strHaystack.begin(), strHaystack.end(),
        strNeedle.begin(), strNeedle.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (it != strHaystack.end());
}

AudioRecorder::AudioRecorder()
{
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.wBitsPerSample = 16;
    wave_format.nChannels = 2;
    wave_format.nSamplesPerSec = 22050;
    wave_format.nBlockAlign = wave_format.nChannels * wave_format.wBitsPerSample / 8;
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize = sizeof(WAVEFORMATEX);
    h_wavein = NULL;
}

AudioRecorder::~AudioRecorder()
{
    Stop();
}

void AudioRecorder::Record()
{

    UINT nDevices, nC1;
    MMRESULT mRes;
    WAVEINCAPS wic = { 0 };
    int selectedDev = 0;

    bool realtekfound = false;
    string selectedDevName = xorstr("<unknown>");

    nDevices = waveInGetNumDevs();

    for (nC1 = 0; nC1 < nDevices; ++nC1)
    {
        ZeroMemory(&wic, sizeof(WAVEINCAPS));
        mRes = waveInGetDevCaps(nC1, &wic, sizeof(WAVEINCAPS));
        if (mRes == 0) {
            string dv = string(wic.szPname);
            selectedDevName = dv;
            if (Engine->m_SettingsMgr->m_micDevice == -1)
            {
                if (findString(dv, xorstr("realtek")))
                {
                    selectedDev = nC1;
                    realtekfound = true;
                    break;
                }
            }
            if (Engine->m_SettingsMgr->m_micDevice == nC1) {
                selectedDev = Engine->m_SettingsMgr->m_micDevice;
                break;
            }
        }
    }

    if (!realtekfound && Engine->m_SettingsMgr->m_micDevice == -1)
        selectedDev = nDevices - 1;
    

    MMRESULT nRet = waveInOpen(&h_wavein, selectedDev, &wave_format, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION);
    if (nRet != MMSYSERR_NOERROR)
        TEMUKOACSEngine::WriteLog(string_format(xorstr("VOICE_ERROR\tSelected device returned false. [%s]"), selectedDevName.c_str()));

    for (int i = 0; i < FRAGMENT_NUM; i++)
    {
        wh[i].lpData = new char[wave_format.nSamplesPerSec];
        wh[i].dwBufferLength = wave_format.nSamplesPerSec;
        wh[i].dwBytesRecorded = 0;
        wh[i].dwUser = NULL;
        wh[i].dwFlags = 0;
        wh[i].dwLoops = 1;
        wh[i].lpNext = NULL;
        wh[i].reserved = 0;
        waveInPrepareHeader(h_wavein, &wh[i], sizeof(WAVEHDR));
        waveInAddBuffer(h_wavein, &wh[i], sizeof(WAVEHDR));
    }

    waveInStart(h_wavein);
}

bool AudioRecorder::Stop()
{
    waveInStop(h_wavein);
    waveInReset(h_wavein);
    for (int i = 0; i < FRAGMENT_NUM; i++)
    {
        waveInUnprepareHeader(h_wavein, &wh[i], sizeof(WAVEHDR));
    }
    waveInClose(h_wavein);
    return true;
}