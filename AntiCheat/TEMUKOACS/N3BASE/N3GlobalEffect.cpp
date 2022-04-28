

#include "N3GlobalEffect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3GlobalEffect::CN3GlobalEffect()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_bActive = FALSE;
	m_iVC = 0;
	m_iIC = 0;
	m_fFadeTime = 0.0f; 
	m_fFadeTimeCur = 0.0f; 
	m_iFadeMode = 0;
}

CN3GlobalEffect::~CN3GlobalEffect()
{
	if (m_pVB) { m_pVB->Release(); m_pVB = NULL;}
	if (m_pIB) { m_pIB->Release(); m_pIB = NULL;}
}

void CN3GlobalEffect::Release()
{
	if (m_pVB) { m_pVB->Release(); m_pVB = NULL;}
	if (m_pIB) { m_pIB->Release(); m_pIB = NULL;}
	m_bActive = FALSE;
	m_iVC = 0;
	m_iIC = 0;
	m_fFadeTime = 0.0f; 
	m_fFadeTimeCur = 0.0f; 
	m_iFadeMode = 0;

	CN3Transform::Release();
}

void CN3GlobalEffect::Tick()
{
	CN3Transform::Tick(-1);

	if(m_iFadeMode && m_fFadeTime > 0) 
	{
		m_fFadeTimeCur += s_fSecPerFrm;
		if(m_fFadeTimeCur > m_fFadeTime) m_fFadeTimeCur = m_fFadeTime;
	}
}

void CN3GlobalEffect::Render(__Vector3& vPos)
{
	PosSet(vPos);
}

void CN3GlobalEffect::FadeSet(float fTimeToFade, bool bFadeIn)
{
	m_fFadeTime = fTimeToFade;
	m_fFadeTimeCur = 0;
	if(bFadeIn) m_iFadeMode = 1;
	else m_iFadeMode = -1;
}
