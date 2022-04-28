

#include "WaveFile.h"

CWaveFile::CWaveFile()
{
    m_pwfx    = NULL;
    m_hmmio   = NULL;
    m_dwSize  = 0;
    m_bIsReadingFromMemory = FALSE;
}

CWaveFile::~CWaveFile()
{
    Close();

    if( !m_bIsReadingFromMemory )
	{
		if(m_pwfx)
		{
			delete[] m_pwfx;
			m_pwfx = NULL;
		}
	}
}

HRESULT CWaveFile::Open( LPCSTR strFileName, WAVEFORMATEX* pwfx, uint32_t dwFlags )
{
    HRESULT hr;

    m_dwFlags = dwFlags;
    m_bIsReadingFromMemory = FALSE;

    if( m_dwFlags == WAVEFILE_READ )
    {
        if( strFileName == NULL )
            return E_INVALIDARG;
		if(m_pwfx)
		{
			delete[] m_pwfx;
			m_pwfx = NULL;
		}

        m_hmmio = mmioOpenA( (LPSTR)strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

        if( NULL == m_hmmio )
        {
            HRSRC   hResInfo;
            HGLOBAL hResData;
            uint32_t   dwSize;
            VOID*   pvRes;

            
            if( NULL == ( hResInfo = FindResourceA( NULL, strFileName, ("WAVE") ) ) )
            {
                if( NULL == ( hResInfo = FindResourceA( NULL, strFileName, ("WAV") ) ) )
                    return E_FAIL;
            }

            if( NULL == ( hResData = LoadResource( NULL, hResInfo ) ) )
                return E_FAIL;

            if( 0 == ( dwSize = SizeofResource( NULL, hResInfo ) ) ) 
                return E_FAIL;

            if( NULL == ( pvRes = LockResource( hResData ) ) )
                return E_FAIL;

            CHAR* pData = new CHAR[ dwSize ];
            memcpy( pData, pvRes, dwSize );

            MMIOINFO mmioInfo;
            ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
            mmioInfo.fccIOProc = FOURCC_MEM;
            mmioInfo.cchBuffer = dwSize;
            mmioInfo.pchBuffer = (CHAR*) pData;

            m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
        }

        if( FAILED( hr = ReadMMIO() ) )
        {
            
            mmioClose( m_hmmio, 0 );
            return hr;
        }

        if( FAILED( hr = ResetFile() ) )
            return hr;

        
        m_dwSize = m_ck.cksize;
    }
    else
    {
        m_hmmio = mmioOpenA( (LPSTR)strFileName, NULL, MMIO_ALLOCBUF  | 
                                                  MMIO_READWRITE | 
                                                  MMIO_CREATE );
        if( NULL == m_hmmio )
            return E_FAIL;

        if( FAILED( hr = WriteMMIO( pwfx ) ) )
        {
            mmioClose( m_hmmio, 0 );
            return hr;
        }
                        
        if( FAILED( hr = ResetFile() ) )
            return hr;
    }

    return hr;
}

HRESULT CWaveFile::OpenFromMemory( uint8_t* pbData, ULONG ulDataSize, 
                                   WAVEFORMATEX* pwfx, uint32_t dwFlags )
{
    m_pwfx       = pwfx;
    m_ulDataSize = ulDataSize;
    m_pbData     = pbData;
    m_pbDataCur  = m_pbData;
    m_bIsReadingFromMemory = TRUE;
    
    if( dwFlags != WAVEFILE_READ )
        return E_NOTIMPL;       
    
    return S_OK;
}

HRESULT CWaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           
    PCMWAVEFORMAT   pcmWaveFormat;  

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return E_FAIL;

    
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return E_FAIL; 

    
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return E_FAIL;

    
    
       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
           return E_FAIL;

    
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return E_FAIL;

    
    
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
            return E_FAIL;

        
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        
        uint16_t cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(uint16_t)) != sizeof(uint16_t) )
            return E_FAIL;

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return E_FAIL;

        
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        
        if( mmioRead( m_hmmio, (CHAR*)(((uint8_t*)&(m_pwfx->cbSize))+sizeof(uint16_t)),
                      cbExtraBytes ) != cbExtraBytes )
        {
			if(m_pwfx)
			{
				delete m_pwfx;
				m_pwfx = NULL;
			}
            return E_FAIL;
        }
    }

    
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
		if(m_pwfx)
		{
			delete m_pwfx;
			m_pwfx = NULL;
		}
        return E_FAIL;
    }

    return S_OK;
}

uint32_t CWaveFile::GetSize()
{
    return m_dwSize;
}

HRESULT CWaveFile::ResetFile()
{
    if( m_bIsReadingFromMemory )
    {
        m_pbDataCur = m_pbData;
    }
    else 
    {
        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( m_dwFlags == WAVEFILE_READ )
        {
            
			if (-1 == mmioSeek(m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
				SEEK_SET))
				return S_FALSE;

            
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
			if (0 != mmioDescend(m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK))
				return S_FALSE;
        }
        else
        {
            
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            m_ck.cksize = 0;

			if (0 != mmioCreateChunk(m_hmmio, &m_ck, 0))
				return S_FALSE;

			if (0 != mmioGetInfo(m_hmmio, &m_mmioinfoOut, 0))
				return S_FALSE;
        }
    }
    
    return S_OK;
}

HRESULT CWaveFile::Read( uint8_t* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    if( m_bIsReadingFromMemory )
    {
        if( m_pbDataCur == NULL )
            return CO_E_NOTINITIALIZED;
        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( (uint8_t*)(m_pbDataCur + dwSizeToRead) > 
            (uint8_t*)(m_pbData + m_ulDataSize) )
        {
            dwSizeToRead = m_ulDataSize - (uint32_t)(m_pbDataCur - m_pbData);
        }
        
        CopyMemory( pBuffer, m_pbDataCur, dwSizeToRead );
        
        if( pdwSizeRead != NULL )
            *pdwSizeRead = dwSizeToRead;

        return S_OK;
    }
    else 
    {
        MMIOINFO mmioinfoIn; 

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;
        if( pBuffer == NULL || pdwSizeRead == NULL )
            return E_INVALIDARG;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

		if (0 != mmioGetInfo(m_hmmio, &mmioinfoIn, 0))
			return S_FALSE;
                
        UINT cbDataIn = dwSizeToRead;
        if( cbDataIn > m_ck.cksize ) 
            cbDataIn = m_ck.cksize;       

        m_ck.cksize -= cbDataIn;
    
        for( uint32_t cT = 0; cT < cbDataIn; cT++ )
        {
            
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
				if (0 != mmioAdvance(m_hmmio, &mmioinfoIn, MMIO_READ))
					return S_FALSE;

				if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
					return S_FALSE;
            }

            
            *((uint8_t*)pBuffer+cT) = *((uint8_t*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
        }

		if (0 != mmioSetInfo(m_hmmio, &mmioinfoIn, 0))
			return S_FALSE;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = cbDataIn;

        return S_OK;
    }
}

HRESULT CWaveFile::Close()
{
    if( m_dwFlags == WAVEFILE_READ )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }
    else
    {
        m_mmioinfoOut.dwFlags |= MMIO_DIRTY;

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

		if (0 != mmioSetInfo(m_hmmio, &m_mmioinfoOut, 0))
			return S_FALSE;
    
        
        
		if (0 != mmioAscend(m_hmmio, &m_ck, 0))
			return S_FALSE;
    
        
		if (0 != mmioAscend(m_hmmio, &m_ckRiff, 0))
			return S_FALSE;
        
        mmioSeek( m_hmmio, 0, SEEK_SET );

		if (0 != (INT)mmioDescend(m_hmmio, &m_ckRiff, NULL, 0))
			return S_FALSE;
    
        m_ck.ckid = mmioFOURCC('f', 'a', 'c', 't');

        if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) ) 
        {
            uint32_t dwSamples = 0;
            mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(uint32_t) );
            mmioAscend( m_hmmio, &m_ck, 0 ); 
        }
    
        
        
		if (0 != mmioAscend(m_hmmio, &m_ckRiff, 0))
			return S_FALSE;
    
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}

HRESULT CWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
{
    uint32_t    dwFactChunk; 
    MMCKINFO ckOut1;
    
    dwFactChunk = (uint32_t)-1;

    
    m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');       
    m_ckRiff.cksize = 0;

	if (0 != mmioCreateChunk(m_hmmio, &m_ckRiff, MMIO_CREATERIFF))
		return S_FALSE;
    
    
    
    
    
    m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
    m_ck.cksize = sizeof(PCMWAVEFORMAT);   

	if (0 != mmioCreateChunk(m_hmmio, &m_ck, 0))
		return S_FALSE;
    
    
    if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
    {
		if (mmioWrite(m_hmmio, (HPSTR)pwfxDest,
			sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
			return S_FALSE;
    }   
    else 
    {
        
		if ((UINT)mmioWrite(m_hmmio, (HPSTR)pwfxDest,
			sizeof(*pwfxDest) + pwfxDest->cbSize) !=
			(sizeof(*pwfxDest) + pwfxDest->cbSize))
			return S_FALSE;
    }  
    
    
	if (0 != mmioAscend(m_hmmio, &m_ck, 0))
		return S_FALSE;
    
    
    
    ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
    ckOut1.cksize = 0;

	if (0 != mmioCreateChunk(m_hmmio, &ckOut1, 0))
		return S_FALSE;
    
	if (mmioWrite(m_hmmio, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) !=
		sizeof(dwFactChunk))
		return S_FALSE;
    
    
	if (0 != mmioAscend(m_hmmio, &ckOut1, 0))
		return S_FALSE;
       
    return S_OK;
}

HRESULT CWaveFile::Write( UINT nSizeToWrite, uint8_t* pbSrcData, UINT* pnSizeWrote )
{
    UINT cT;

    if( m_bIsReadingFromMemory )
        return E_NOTIMPL;
    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pnSizeWrote == NULL || pbSrcData == NULL )
        return E_INVALIDARG;

    *pnSizeWrote = 0;
    
    for( cT = 0; cT < nSizeToWrite; cT++ )
    {       
        if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
        {
            m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
			if (0 != mmioAdvance(m_hmmio, &m_mmioinfoOut, MMIO_WRITE))
				return S_FALSE;
        }

        *((uint8_t*)m_mmioinfoOut.pchNext) = *((uint8_t*)pbSrcData+cT);
        (uint8_t*)m_mmioinfoOut.pchNext++;

        (*pnSizeWrote)++;
    }

    return S_OK;
}

