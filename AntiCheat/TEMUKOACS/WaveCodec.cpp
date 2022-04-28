#include "WaveCodec.h"

#pragma comment(lib,"Msacm32.lib")

CWaveCodec::CWaveCodec()
{
    hstr = NULL;
}

CWaveCodec::~CWaveCodec()
{
    EndSeq();
}

BOOL CWaveCodec::StartSeq(LPWAVEFORMATEX lpFormat, LPWAVEFORMATEX lpFormatDst)
{
    EndSeq();
    MMRESULT mmr;
    LPWAVEFORMATEX pwfSrc;
    WAVEFORMATGSM   wfDrv;
    LPWAVEFORMATEX  pDst;

    pwfSrc = (LPWAVEFORMATEX)lpFormat;
    if (lpFormatDst)
        pDst = lpFormatDst;
    else
    {
        wfDrv.wf.cbSize = 2;
        wfDrv.wf.nAvgBytesPerSec = 1625;
        wfDrv.wf.nBlockAlign = 65;
        wfDrv.wf.nChannels = 1;
        wfDrv.wf.nSamplesPerSec = 8000;
        wfDrv.wf.wBitsPerSample = 0;
        wfDrv.wf.wFormatTag = WAVE_FORMAT_GSM610; //gsm610   
        wfDrv.reserved[0] = 64;
        wfDrv.reserved[1] = 1;
        pDst = (LPWAVEFORMATEX)&wfDrv;
    }
    mmr = acmStreamOpen(&hstr, NULL,       
        pwfSrc,           
        pDst,             
        NULL,       
        NULL,       
        0, ACM_STREAMOPENF_NONREALTIME);
    return mmr == NULL;
}

int CWaveCodec::Compress(char* pBuf, char* pDstBuf, int nLen)
{
    MMRESULT mmr;
    ACMSTREAMHEADER shdr;
    memset(&shdr, 0, sizeof(shdr));
    shdr.cbStruct = sizeof(shdr);
    shdr.pbSrc = (unsigned char*)pBuf;
    shdr.cbSrcLength = nLen;
    shdr.pbDst = (BYTE*)pDstBuf;
    shdr.cbDstLength = nLen;
    mmr = acmStreamPrepareHeader(hstr, &shdr, 0);
    if (mmr)
    {
        return 0;
    }

    mmr = acmStreamConvert(hstr, &shdr, 0);
    if (mmr)
    {
        return 0;
    }
    while ((shdr.fdwStatus & ACMSTREAMHEADER_STATUSF_DONE) == 0);
    acmStreamUnprepareHeader(hstr, &shdr, 0);
    return shdr.cbDstLengthUsed;
}

void CWaveCodec::EndSeq()
{
    if (hstr)
    {
        acmStreamClose(hstr, 0);
        hstr = NULL;
    }
}


BOOL CWaveCodec::StartDeSeq(LPWAVEFORMATEX lpFormatSrc, LPWAVEFORMATEX lpFormat)
{
    MMRESULT mmr;
    WAVEFORMATGSM   wfSrv;
    WAVEFORMATGSM wfDrv;
    WAVEFORMATEX* pSrc, * pDst;
    if (lpFormatSrc)
        pSrc = lpFormatSrc;
    else
    {
        wfSrv.wf.cbSize = 2;
        wfSrv.wf.nAvgBytesPerSec = 1625;
        wfSrv.wf.nBlockAlign = 65;
        wfSrv.wf.nChannels = 1;
        wfSrv.wf.nSamplesPerSec = 8000;
        wfSrv.wf.wBitsPerSample = 0;
        wfSrv.wf.wFormatTag = WAVE_FORMAT_GSM610; //gsm610   
        wfSrv.reserved[0] = 64;
        wfSrv.reserved[1] = 1;
        pSrc = (LPWAVEFORMATEX)&wfSrv;
    }
    if (lpFormat)
    {
        pDst = lpFormat;
    }
    else
    {
        wfDrv.wf.cbSize = 0;
        wfDrv.wf.nAvgBytesPerSec = 8000;
        wfDrv.wf.nBlockAlign = 1;
        wfDrv.wf.nChannels = 1;
        wfDrv.wf.nSamplesPerSec = 8000;
        wfDrv.wf.wBitsPerSample = 8;
        wfDrv.wf.wFormatTag = WAVE_FORMAT_PCM;
        pDst = (LPWAVEFORMATEX)&wfDrv;
    }
    mmr = acmStreamOpen(&hstr, NULL,
        pSrc,
        pDst,
        NULL,
        NULL,  
        0, ACM_STREAMOPENF_NONREALTIME);
    return mmr == NULL;
}

int CWaveCodec::DeCompress(char* pBuf, char* pDstBuf, int nLen, int nBuf)
{
    MMRESULT mmr;

    ACMSTREAMHEADER shdr;
    memset(&shdr, 0, sizeof(shdr));
    shdr.cbStruct = sizeof(shdr);
    shdr.pbSrc = (unsigned char*)pBuf;
    shdr.cbSrcLength = nLen;
    shdr.pbDst = (BYTE*)pDstBuf;
    shdr.cbDstLength = nBuf;
    mmr = acmStreamPrepareHeader(hstr, &shdr, 0);
    if (mmr)
    {
        return 0;
    }
   
    mmr = acmStreamConvert(hstr, &shdr, 0);
    if (mmr)
    {
        return 0;
    }

    return shdr.cbDstLengthUsed;
}

void CWaveCodec::Melp_StartEnc()
{
    //EsDllInit();   
}

void CWaveCodec::Melp_InitDec()
{
    //EsDllInitD();   
}
//nLen=FRAME*N   
int CWaveCodec::Melp_Compress(SPEECH* pBuf, BYTE* pDstBuf, int nLen)
{
    int nRet = 0;
    /*
    int nCodec;
    unsigned int pOut[FRAME];
    nLen/=sizeof(SPEECH);
    while(nLen>0)
    {
        nCodec=EsDll(pBuf,pOut,nLen);
        for(int i=0;i<nCodec;i++)
            pDstBuf[i]=pOut[i];

        pBuf+=FRAME;
        nLen-=FRAME;
        nRet+=nCodec;
        pDstBuf+=nCodec;
    }
    */
    return nRet;
}
//nLen=CHSIZE*N   
int CWaveCodec::Melp_DeCompress(BYTE* pBuf, SPEECH* pDstBuf, int nLen)
{
    int nRet = 0;
    /*
    int nCodec;
    unsigned int pIn[CHSIZE];
    while(nLen>0)
    {
        for(int i=0;i<CHSIZE;i++)
            pIn[i]=pBuf[i];
        nCodec=EsDllD(pIn,pDstBuf,CHSIZE);

        pBuf+=CHSIZE;
        nLen-=CHSIZE;
        nRet+=nCodec;
        pDstBuf+=nCodec;
    }*/
    return nRet * sizeof(SPEECH);
}

int CWaveCodec::G729_DeCompress(BYTE* pBuf, SPEECH* pDstBuf, int nLen, int bfi)
{
    int nRet = 0;
    while (nLen > 0)
    {
        va_g729a_decoder(pBuf, pDstBuf, bfi);

        pBuf += L_FRAME_COMPRESSED;
        nLen -= L_FRAME_COMPRESSED;
        nRet += L_FRAME;
        pDstBuf += L_FRAME;
    }
    return nRet * sizeof(SPEECH);
}

int CWaveCodec::G729_Compress(SPEECH* pBuf, BYTE* pDstBuf, int nLen)
{
    int nRet = 0;
    nLen /= sizeof(SPEECH);
    while (nLen > 0)
    {
        va_g729a_encoder(pBuf, pDstBuf);

        pBuf += L_FRAME;
        nLen -= L_FRAME;
        nRet += L_FRAME_COMPRESSED;
        pDstBuf += L_FRAME_COMPRESSED;
    }
    return nRet;
}

void CWaveCodec::G729_InitDec()
{
    va_g729a_init_decoder();
}

void CWaveCodec::G729_StartEnc()
{
    va_g729a_init_encoder();
}