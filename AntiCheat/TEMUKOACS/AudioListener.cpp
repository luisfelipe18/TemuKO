#include "AudioListener.h"
#include <mutex>

AudioListener::AudioListener()
{
	wave_format.wFormatTag = WAVE_FORMAT_PCM;
	wave_format.wBitsPerSample = 16;
	wave_format.nChannels = 2;
	wave_format.nSamplesPerSec = 22050;
	wave_format.nBlockAlign = wave_format.nChannels * wave_format.wBitsPerSample / 8;
	wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
	wave_format.cbSize = sizeof(WAVEFORMATEX);
	h_waveout = NULL;
}

AudioListener::~AudioListener(){
	stop();
}

std::mutex input_mutex;

int AudioListener::input(unsigned char* buf)
{
	const std::lock_guard<std::mutex> lock(input_mutex);

	size_t nSize = m_buf_free.size();
	if (nSize == 0)
		return 0;

	WAVEHDR* p_wavehdr = (WAVEHDR*)m_buf_free.front();
	m_buf_free.pop_front();

	p_wavehdr->lpData = reinterpret_cast <CHAR*>(buf);
	p_wavehdr->dwBufferLength = wave_format.nSamplesPerSec;

	waveOutPrepareHeader(h_waveout, p_wavehdr, sizeof(WAVEHDR));
	waveOutWrite(h_waveout, p_wavehdr, sizeof(WAVEHDR));
	return 0;
}

bool AudioListener::start()
{
	if (h_waveout != NULL)
		return true;

	MMRESULT nRet = waveOutOpen(&h_waveout, WAVE_MAPPER, &wave_format, NULL, (DWORD)this, CALLBACK_NULL);
	if (nRet != MMSYSERR_NOERROR)
		return false;

	for (int i = 0; i < 16; ++i)
	{
		WAVEHDR* p_wavehdr = (WAVEHDR*)m_buf[i];
		p_wavehdr->dwBufferLength = wave_format.nAvgBytesPerSec;
		p_wavehdr->lpData = m_buf[i] + sizeof(WAVEHDR);
		p_wavehdr->dwFlags = 0;
		waveOutPrepareHeader(h_waveout, p_wavehdr, sizeof(WAVEHDR));
		m_buf_free.push_back((char*)p_wavehdr);
	}
	return true;
}

bool AudioListener::stop()
{
	if (h_waveout != NULL)
	{
		while (m_buf_free.size() != 16)
			Sleep(80);

		m_buf_free.clear();
		waveOutReset(h_waveout);

		for (int i = 0; i < 16; ++i)
		{
			WAVEHDR* p_wavehdr = (WAVEHDR*)m_buf[i];
			waveOutUnprepareHeader(h_waveout, p_wavehdr, sizeof(WAVEHDR));
		}

		waveOutClose(h_waveout);
		h_waveout = NULL;
	}
	return true;
}