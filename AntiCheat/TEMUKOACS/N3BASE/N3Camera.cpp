

#include "N3Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3Camera::CN3Camera()
{
	m_dwType |= OBJ_CAMERA;

	m_Data.Release();
	m_Data.vEye = m_vPos = __Vector3(15,5,-15);
	m_Data.vAt = m_vAt = __Vector3(0,0,0);
	m_Data.vUp = m_vScale = __Vector3(0,1,0);

	m_Data.fFOV = D3DXToRadian(55.0f); 
	m_Data.fNP = 0.7f;
	m_Data.fFP = 512.0f;

	m_bFogUse = TRUE;
	m_FogColor = D3DCOLOR_ARGB(255,255,255,255);

	m_fRadianX = 0.0f;
	m_bOrtho = false;
}

CN3Camera::~CN3Camera()
{
}

void CN3Camera::Release()
{
	m_Data.Release();
	m_Data.vEye = m_vPos = __Vector3(15,5,-15);
	m_Data.vAt = m_vAt = __Vector3(0,0,0);
	m_Data.vUp = m_vScale = __Vector3(0,1,0);

	m_Data.fFOV = D3DXToRadian(55.0f); 
	m_Data.fNP = 0.7f;
	m_Data.fFP = 512.0f;

	m_bFogUse = FALSE;
	m_FogColor = D3DCOLOR_ARGB(255,255,255,255);

	m_fRadianX = 0.0f;

	CN3Transform::Release();
}

bool CN3Camera::Load(HANDLE hFile)
{
	CN3Transform::Load(hFile);

	DWORD dwRWC = 0;
	ReadFile(hFile, &m_vAt, sizeof(__Vector3), &dwRWC, NULL); 
	ReadFile(hFile, &m_Data, sizeof(__CameraData), &dwRWC, NULL); 
	ReadFile(hFile, &m_bFogUse, 4, &dwRWC, NULL);
	ReadFile(hFile, &m_FogColor, 4, &dwRWC, NULL);

	return true;
}

void CN3Camera::Rotate(float fRadianX, float fRadianY)
{
	
	__Matrix44 mtx;		
	mtx.RotationY(fRadianY);

	__Vector3 v1 = m_vAt - m_vPos, v2; 
	v1 *= mtx;
	m_vScale *= mtx;

	v2.Cross(v1, m_vScale);
	__Quaternion qt;
	qt.RotationAxis(v2, fRadianX);
	mtx = qt;

	v1 *= mtx;
	m_vScale *= mtx;	

	m_vAt = m_vPos + v1;
}

void CN3Camera::Apply()
{
	s_lpD3DDev->SetTransform(D3DTS_VIEW, &m_Data.mtxView);
	s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &m_Data.mtxProjection); 
	memcpy(&(CN3Base::s_CameraData), &m_Data, sizeof(__CameraData)); 

	
	s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, m_bFogUse);
	s_lpD3DDev->SetRenderState( D3DRS_FOGCOLOR,  m_FogColor);

	s_lpD3DDev->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_EXP2);
	s_lpD3DDev->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_EXP2);

	s_lpD3DDev->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE);

	float fFogDensity = 1.0f / (0.37f*m_Data.fFP);
	float fFogStart = m_Data.fFP * 0.75f;
	float fFogEnd = m_Data.fFP;

	s_lpD3DDev->SetRenderState( D3DRS_FOGSTART,   *(uint32_t*)&fFogStart);
	s_lpD3DDev->SetRenderState( D3DRS_FOGEND,     *(uint32_t*)&fFogEnd);
	s_lpD3DDev->SetRenderState( D3DRS_FOGDENSITY, *(uint32_t*)&fFogDensity);
}

void CN3Camera::Render(float fUnitSize)
{
}

void CN3Camera::Tick(float fFrm)
{
	CN3Transform::Tick(fFrm);

	
	
	
	
	
	m_Data.vEye = m_vPos;
	m_Data.vAt  = m_vAt;
	m_Data.vUp  = m_vScale; 
	

	::D3DXMatrixLookAtLH(&m_Data.mtxView, &m_Data.vEye, &m_Data.vAt, &m_Data.vUp); 
	::D3DXMatrixInverse(&m_Data.mtxViewInverse, NULL, &m_Data.mtxView); 
	CN3Base::s_lpD3DDev->GetViewport(&m_Data.vp); 
	
	m_Data.fAspect = (float)m_Data.vp.Width / (float)m_Data.vp.Height; 
	if(m_bOrtho)
	{
		float fL = (m_Data.vAt - m_Data.vEye).Magnitude() / 2.0f;
		::D3DXMatrixOrthoLH(&m_Data.mtxProjection, fL, fL / m_Data.fAspect, m_Data.fNP * (1.0f + fL / 1000.0f), m_Data.fFP);  

	}
	else
	{
		::D3DXMatrixPerspectiveFovLH(&m_Data.mtxProjection, m_Data.fFOV, m_Data.fAspect, m_Data.fNP, m_Data.fFP); 
	}

	__Matrix44 mtx = m_Data.mtxView * m_Data.mtxProjection;
	float frustum [6][4];
	frustum[0][0] = mtx._14 - mtx._11;
	frustum[0][1] = mtx._24 - mtx._21;
	frustum[0][2] = mtx._34 - mtx._31;
	frustum[0][3] = mtx._44 - mtx._41;

	
	float t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	
	frustum[1][0] = mtx._14 + mtx._11;
	frustum[1][1] = mtx._24 + mtx._21;
	frustum[1][2] = mtx._34 + mtx._31;
	frustum[1][3] = mtx._44 + mtx._41;

	
	t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	
	frustum[2][0] = mtx._14 + mtx._12;
	frustum[2][1] = mtx._24 + mtx._22;
	frustum[2][2] = mtx._34 + mtx._32;
	frustum[2][3] = mtx._44 + mtx._42;

	
	t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	
	frustum[3][0] = mtx._14 - mtx._12;
	frustum[3][1] = mtx._24 - mtx._22;
	frustum[3][2] = mtx._34 - mtx._32;
	frustum[3][3] = mtx._44 - mtx._42;

	
	t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	
	frustum[4][0] = mtx._14 - mtx._13;
	frustum[4][1] = mtx._24 - mtx._23;
	frustum[4][2] = mtx._34 - mtx._33;
	frustum[4][3] = mtx._44 - mtx._43;

	
	t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	
	frustum[5][0] = mtx._14 + mtx._13;
	frustum[5][1] = mtx._24 + mtx._23;
	frustum[5][2] = mtx._34 + mtx._33;
	frustum[5][3] = mtx._44 + mtx._43;

	
	t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;

	memcpy(m_Data.fFrustum, frustum, sizeof(float)*6*4);
}

void CN3Camera::LookAt(const __Vector3 &vEye, __Vector3 &vAt, __Vector3 &vUp)
{
	m_vPos = vEye;
	m_vAt = vAt;
	m_vScale = vUp;
}
