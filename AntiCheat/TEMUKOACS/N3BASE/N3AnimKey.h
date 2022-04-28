

#if !defined(AFX_N3AnimKey_h__INCLUDED_)
#define AFX_N3AnimKey_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Base.h"

enum ANIMATION_KEY_TYPE { 
	KEY_VECTOR3 = 0,
	KEY_QUATERNION = 1,
	KEY_UNKNOWN = 0xffffffff
}; 

class CN3AnimKey : CN3Base
{
	friend class CN3Joint;
protected:
	ANIMATION_KEY_TYPE m_eType; 
	int		m_nCount; 
	float	m_fSamplingRate; 
	void*	m_pDatas;

public:
	ANIMATION_KEY_TYPE Type() { return m_eType; } 
	void	Add(CN3AnimKey& AKSrc, int nIndexS, int nIndexE);

	bool DataGet(float fFrm, __Vector3& v)
	{
		if(KEY_VECTOR3 != m_eType) return false;
		if(m_nCount <= 0) return false;
		
		float fD = 30.0f / m_fSamplingRate;
		
		int nIndex = (int)(fFrm * (m_fSamplingRate / 30.0f));
		if(nIndex < 0 || nIndex > m_nCount) return false;
		
		float fDelta = 0;
		if(nIndex == m_nCount) { nIndex = m_nCount - 1; fDelta = 0; }
		else fDelta = (fFrm - nIndex * fD)/fD;
		
		if(0.0f != fDelta) { v = (((__Vector3*)m_pDatas)[nIndex] * (1.0f - fDelta)) + (((__Vector3*)m_pDatas)[nIndex+1] * fDelta); }
		else v = ((__Vector3*)m_pDatas)[nIndex];
		return true;
	}
	bool DataGet(float fFrm, __Quaternion& q)
	{
		if(KEY_QUATERNION != m_eType) return false;
		if(m_nCount <= 0) return false;

		float fD = 30.0f / m_fSamplingRate;

		int nIndex = (int)(fFrm * (m_fSamplingRate / 30.0f));
		if(nIndex < 0 || nIndex > m_nCount) return false;

		float fDelta = 0;
		if(nIndex == m_nCount) { nIndex = m_nCount - 1; fDelta = 0; }
		else fDelta = (fFrm - nIndex * fD)/fD;

		if(0.0f != fDelta) q.Slerp(((__Quaternion*)m_pDatas)[nIndex], ((__Quaternion*)m_pDatas)[nIndex+1], fDelta);
		else q = ((__Quaternion*)m_pDatas)[nIndex];
		return true;
	}
	
	bool Load(HANDLE hFile);
	int Count() { return m_nCount; }
	float SamplingRate() { return m_fSamplingRate; }
	void Release();
	void Alloc(int nCount, float fSamplingRate = 30.0f, ANIMATION_KEY_TYPE eType = KEY_VECTOR3);
	
	void Duplicate(CN3AnimKey* pSrc);
	void*	GetDatas() { return m_pDatas; }
	void MultiplyDataVector(__Vector3 vM);
	
	CN3AnimKey();
	virtual ~CN3AnimKey();
};

#endif 
