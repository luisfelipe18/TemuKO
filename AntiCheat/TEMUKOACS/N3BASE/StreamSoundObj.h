

#ifndef __STREAMSOUNDOBJ_H_
#define __STREAMSOUNDOBJ_H_

#include "N3SndObj.h"

class CN3SndEng;

class CStreamSoundObj : public CN3SndObj
{
	private:
		int					m_PlayTime;		
		int					m_FinalByte;
				
		DSBUFFERDESC		m_dsbd;
		uint32_t				m_BufferSize;
		uint32_t				m_BlockSize;	
		
		int					m_CurrState;	
		int					m_PrevState;	
		uint32_t				m_CurrPos;		
		int					m_PastTime;		

		
		WAVEFORMATEX		m_WaveFormat;
		uint32_t				m_WaveSize;
		LPSTR				m_pWaveMem;
		HGLOBAL				m_hWaveMem;
		MMCKINFO			mmCkInfoRIFF;
		MMCKINFO			mmCkInfoChunk;
		HMMIO				hMMIO;
		
	public:
		BOOL Create(CN3SndEng* pEng);

		
		void Tick();		
		void Release();
		
		CStreamSoundObj();
		virtual ~CStreamSoundObj();
				
	private:
		void RealPlay();
		
		

		BOOL LoadWave(LPCSTR pFileName);
		BOOL WriteBuffer();
		BOOL InitWriteBuffer();
		void Reset();
};

#endif 
