

#ifndef __WAVE_H_
#define __WAVE_H_

#include <Windows.h>
#include "../types.h"

#include <mmsystem.h>
#include <mmreg.h>
#include <dxerr9.h>

#define WAVEFILE_READ	1
#define WAVEFILE_WRITE	0

class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        
    HMMIO         m_hmmio;       
    MMCKINFO      m_ck;          
    MMCKINFO      m_ckRiff;      
    uint32_t      m_dwSize;      
    MMIOINFO      m_mmioinfoOut;
    uint32_t      m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    uint8_t*      m_pbData;
    uint8_t*      m_pbDataCur;
    ULONG         m_ulDataSize;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPCSTR strFileName, WAVEFORMATEX* pwfx, uint32_t dwFlags );
    HRESULT OpenFromMemory( uint8_t* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, uint32_t dwFlags );
    HRESULT Close();

    HRESULT Read( uint8_t* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, uint8_t* pbData, UINT* pnSizeWrote );

    uint32_t   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

#endif 

