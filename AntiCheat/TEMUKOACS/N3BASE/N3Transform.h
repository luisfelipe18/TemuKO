

#if !defined(AFX_N3Transform_h__INCLUDED_)
#define AFX_N3Transform_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3BaseFileAccess.h"
#include "N3AnimKey.h"

class CN3Transform : public CN3BaseFileAccess
{
protected:
	__Quaternion	m_qRot;
	__Vector3		m_vScale;

public:
	__Vector3		m_vPos;
	__Matrix44		m_Matrix; 

	CN3AnimKey		m_KeyPos; 
	CN3AnimKey		m_KeyRot;
	CN3AnimKey		m_KeyScale;

	float			m_fFrmWhole; 
	float			m_fFrmCur; 

public:
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	TickAnimationKey(float fFrm); 

	__Vector3		Pos() const { return m_vPos; }
	__Quaternion	Rot() const { return m_qRot; }
	__Vector3		Scale() const { return m_vScale; }

	virtual void	PosSet(const __Vector3& v) { m_vPos = v; ReCalcMatrix(); }
	virtual void	PosSet(float x, float y, float z) { m_vPos.Set(x, y, z); ReCalcMatrix(); }
	virtual void	RotSet(const __Quaternion& q) { m_qRot = q; ReCalcMatrix(); }
	virtual void	RotSet(float x, float y, float z, float w) { m_qRot.x = x, m_qRot.y = y, m_qRot.z = z, m_qRot.w = w; ReCalcMatrix(); }
	virtual void	ScaleSet(const __Vector3& v) { m_vScale = v; ReCalcMatrix(); }
	virtual void	ScaleSet(float x, float y, float z) { m_vScale.Set(x, y, z); ReCalcMatrix(); }
	virtual void	ReCalcMatrix();

	bool			Load(HANDLE hFile);

	void Release();
	CN3Transform();
	virtual ~CN3Transform();
};

#endif 
