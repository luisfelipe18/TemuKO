

#if !defined(AFX_N3Camera_h__INCLUDED_)
#define AFX_N3Camera_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Transform.h"

class CN3Camera : public CN3Transform
{
public:
	__Vector3			m_vAt;
	__CameraData		m_Data;
	BOOL				m_bFogUse;
	D3DCOLOR			m_FogColor;

	float				m_fRadianX;
	bool				m_bOrtho;

public:
	void		Rotate(float fRadianX, float fRadianY);
	void		LookAt(const __Vector3& vEye, __Vector3& vAt, __Vector3& vUp);
	__Vector3	EyePos() { return m_vPos; }
	__Vector3	AtPos() { return m_vAt; }
	__Vector3	UpVector() { return m_vScale; } 
	__Vector3	Dir() {__Vector3 vDir = m_vAt - m_vPos; vDir.Normalize(); return vDir;} 

	void		EyePosSet(const __Vector3& v) { m_vPos = v; }
	void		EyePosSet(float x, float y, float z) { m_vPos.Set(x, y, z); }
	void		AtPosSet(const __Vector3& v) { m_vAt = v; } 
	void		AtPosSet(float x, float y, float z) { m_vAt.Set(x, y, z); }
	void		UpVectorSet(const __Vector3& v) { m_vScale = v; } 
	void		UpVectorSet(float x, float y, float z) { m_vScale.Set(x, y, z); } 

	bool		Load(HANDLE hFile);

	void		Release();
	CN3Camera();
	virtual ~CN3Camera();

	void		Tick(float fFrm = FRAME_SELFPLAY); 
	void		Render(float fUnitSize = 1.0f);
	void		Apply(); 
};

#endif 
