#pragma once
typedef short SPEECH;
#define FRAME 180           /* speech frame size */ 
#define CHSIZE 9 

extern "C"
{
#include "va_g729a.h" 
}

#include "stdafx.h"
#include <mmsystem.h> 
#include <mmreg.h> 
#include <Msacm.h> 

#pragma comment(lib,"va_g729")

typedef struct
{
	WAVEFORMATEX wf;
	BYTE reserved[2];
}
WAVEFORMATGSM;

class CWaveCodec
{
public:
	int G729_DeCompress(BYTE* pBuf, SPEECH* pDstBuf, int nLen, int bfi);
	int G729_Compress(SPEECH* pBuf, BYTE* pDstBuf, int nLen);
	void G729_InitDec();
	void G729_StartEnc();

	int Melp_DeCompress(BYTE* pBuf, SPEECH* pDstBuf, int nLen);
	int Melp_Compress(SPEECH* pBuf, BYTE* pDstBuf, int nLen);
	void Melp_InitDec();
	void Melp_StartEnc();
	int DeCompress(char* pBuf, char* pDstBuf, int nLen, int nBuf);
	BOOL StartDeSeq(LPWAVEFORMATEX lpFormatSrc = NULL, LPWAVEFORMATEX lpFormat = NULL);
	void EndSeq();
	int Compress(char* pBuf, char* pDstBuf, int nLen);
	BOOL StartSeq(LPWAVEFORMATEX lpFormat, LPWAVEFORMATEX lpFormatDst);
	CWaveCodec();
	virtual ~CWaveCodec();
private:
	HACMSTREAM hstr;
};