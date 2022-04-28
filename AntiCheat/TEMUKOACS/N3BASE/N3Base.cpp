

#include "N3Mesh.h"
#include "N3VMesh.h"
#include "N3FXPMesh.h"
#include "N3FXShape.h"
#include "N3Base.h"
#include "mmsystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

LPDIRECT3DDEVICE9 CN3Base::s_lpD3DDev	= NULL;			
uint32_t CN3Base::s_dwTextureCaps		= 0;			
float CN3Base::s_fFrmPerSec				= 30.0f;		
float CN3Base::s_fSecPerFrm				= 1.0f/30.0f;	
SDL_Window * CN3Base::s_pWindow			= NULL;
HWND CN3Base::s_hWndBase				= NULL;			
HWND CN3Base::s_hWndPresent				= NULL;			

D3DPRESENT_PARAMETERS CN3Base::s_DevParam;	
D3DCAPS9 CN3Base::s_DevCaps;				
std::string CN3Base::s_szPath;

__CameraData CN3Base::s_CameraData;			
__ResrcInfo CN3Base::s_ResrcInfo;			
__Options CN3Base::s_Options;	
#ifdef _DEBUG
__RenderInfo CN3Base::s_RenderInfo;			
#endif

CN3Mng<CN3Texture>		CN3Base::s_MngTex; 
CN3Mng<CN3Mesh>			CN3Base::s_MngMesh; 
CN3Mng<CN3VMesh>		CN3Base::s_MngVMesh; 
CN3Mng<CN3PMesh>		CN3Base::s_MngPMesh; 
CN3Mng<CN3Joint>		CN3Base::s_MngJoint; 
CN3Mng<CN3CPartSkins>	CN3Base::s_MngSkins; 
CN3Mng<CN3AnimControl>	CN3Base::s_MngAniCtrl; 
CN3Mng<CN3FXPMesh>		CN3Base::s_MngFXPMesh; 
CN3Mng<CN3FXShape>		CN3Base::s_MngFXShape; 

CN3AlphaPrimitiveManager	CN3Base::s_AlphaMgr;  

#ifdef _N3GAME
CLogWriter g_Log; 
#endif

CN3Base::CN3Base()
{
	m_dwType = OBJ_BASE; 
	m_szName = "";
}

CN3Base::~CN3Base()
{
}

void CN3Base::Release()
{
	m_szName = "";
}

void CN3Base::ReleaseResrc()
{
	s_MngTex.Release();
	s_MngMesh.Release();
	s_MngPMesh.Release();
	s_MngVMesh.Release();
	
	s_MngJoint.Release();
	s_MngSkins.Release();
	s_MngAniCtrl.Release();

	s_MngFXPMesh.Release();
	s_MngFXShape.Release();
}

float CN3Base::TimerProcess( TIMER_COMMAND command )
{
    static BOOL     m_bTimerInitialized = FALSE;
    static BOOL     m_bUsingQPF         = FALSE;
    static LONGLONG m_llQPFTicksPerSec  = 0;

    
    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;

        
        
        LARGE_INTEGER qwTicksPerSec;
        m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
        if( m_bUsingQPF )
            m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    if( m_bUsingQPF )
    {
        static LONGLONG m_llStopTime        = 0;
        static LONGLONG m_llLastElapsedTime = 0;
        static LONGLONG m_llBaseTime        = 0;
        double fTime;
        double fElapsedTime;
        LARGE_INTEGER qwTime;
        
        
        
        if( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter( &qwTime );

        
        if( command == TIMER_GETELAPSEDTIME )
        {
            fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
            m_llLastElapsedTime = qwTime.QuadPart;
            return (FLOAT) fElapsedTime;
        }
    
        
        if( command == TIMER_GETAPPTIME )
        {
            double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;
            return (FLOAT) fAppTime;
        }
    
        
        if( command == TIMER_RESET )
        {
            m_llBaseTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        
        if( command == TIMER_START )
        {
            m_llBaseTime += qwTime.QuadPart - m_llStopTime;
            m_llStopTime = 0;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        
        if( command == TIMER_STOP )
        {
            m_llStopTime = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        
        if( command == TIMER_ADVANCE )
        {
            m_llStopTime += m_llQPFTicksPerSec/10;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
            return (FLOAT) fTime;
        }

        return -1.0f; 
    }
    else
    {
        
        static double m_fLastElapsedTime  = 0.0;
        static double m_fBaseTime         = 0.0;
        static double m_fStopTime         = 0.0;
        double fTime;
        double fElapsedTime;
        
        
        
        if( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            fTime = m_fStopTime;
        else
            fTime = timeGetTime() * 0.001;
    
        
        if( command == TIMER_GETELAPSEDTIME )
        {   
            fElapsedTime = (double) (fTime - m_fLastElapsedTime);
            m_fLastElapsedTime = fTime;
            return (FLOAT) fElapsedTime;
        }
    
        
        if( command == TIMER_GETAPPTIME )
        {
            return (FLOAT) (fTime - m_fBaseTime);
        }
    
        
        if( command == TIMER_RESET )
        {
            m_fBaseTime         = fTime;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        
        if( command == TIMER_START )
        {
            m_fBaseTime += fTime - m_fStopTime;
            m_fStopTime = 0.0f;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        
        if( command == TIMER_STOP )
        {
            m_fStopTime = fTime;
            return 0.0f;
        }
    
        
        if( command == TIMER_ADVANCE )
        {
            m_fStopTime += 0.1f;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            return (FLOAT) fTime;
        }

        return -1.0f; 
    }
}

void CN3Base::PathSet(const std::string& szPath)
{
	s_szPath = szPath;
	if(s_szPath.size() <= 0) return;

	
	CharLowerA(&(s_szPath[0])); 
	if(s_szPath.size() > 1)
	{
		
		if(s_szPath[s_szPath.size()-1] != '\\') s_szPath += '\\';
	}
}

void CN3Base::RenderLines(const __Vector3 *pvLines, int nCount, D3DCOLOR color)
{
	DWORD dwAlpha, dwFog, dwLight;
	s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	
	if(dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	if(dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	static __Material smtl;
	static bool bInit = false;
	if(false == bInit)
	{
		smtl.Init();
		bInit = true;
	}

	s_lpD3DDev->SetTexture(0, NULL);

	static __VertexColor svLines[512];

	s_lpD3DDev->SetFVF(FVF_CV);

	int nRepeat = nCount/512;
	for(int i = 0; i < nRepeat; i++)
	{
		for(int j = 0; j < 512; j++) svLines[j].Set(pvLines[i*512+j].x, pvLines[i*512+j].y, pvLines[i*512+j].z, color);
		
		s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 511, svLines, sizeof(__VertexColor));
	}
	int nPC = nCount%512;
	for(int j = 0; j < nPC+1; j++) svLines[j].Set(pvLines[nRepeat*512+j].x, pvLines[nRepeat*512+j].y, pvLines[nRepeat*512+j].z, color);
	s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, nPC, svLines, sizeof(__VertexColor)); 

	if(dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	if(dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
}

void CN3Base::RenderLines(const RECT& rc, D3DCOLOR color)
{
	static __VertexTransformedColor vLines[5];
	
	vLines[0].Set((float)rc.left, (float)rc.top, 0.9f, 1.0f, color);
	vLines[1].Set((float)rc.right, (float)rc.top, 0.9f, 1.0f, color);
	vLines[2].Set((float)rc.right, (float)rc.bottom, 0.9f, 1.0f, color);
	vLines[3].Set((float)rc.left, (float)rc.bottom, 0.9f, 1.0f, color);
	vLines[4] = vLines[0];

	DWORD dwZ, dwFog, dwAlpha, dwCOP, dwCA1, dwSrcBlend, dwDestBlend, dwVertexShader, dwAOP, dwAA1;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOP);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAOP);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAA1);
	CN3Base::s_lpD3DDev->GetFVF(&dwVertexShader);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
	CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vLines, sizeof(__VertexTransformedColor));
	
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwCOP);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwCA1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAOP);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAA1);
	CN3Base::s_lpD3DDev->SetFVF(dwVertexShader);
}

float CN3Base::TimeGet()
{
	static bool bInit = false;
	static bool bUseHWTimer = FALSE;
	static LARGE_INTEGER nTime, nFrequency;
	
	if(bInit == false)
	{
		if(TRUE == ::QueryPerformanceCounter(&nTime))
		{
			::QueryPerformanceFrequency(&nFrequency);
			bUseHWTimer = TRUE;
		}
		else 
		{
			bUseHWTimer = FALSE;
		}

		bInit = true;
	}

	if(bUseHWTimer)
	{
		::QueryPerformanceCounter(&nTime);
		return (float)((double)(nTime.QuadPart)/(double)nFrequency.QuadPart);
	}

	return (float)timeGetTime();
}
