

#if !defined(AFX_PICK_H__4FEE8165_3BBD_4CD4_9D42_6F13B8A30603__INCLUDED_)
#define AFX_PICK_H__4FEE8165_3BBD_4CD4_9D42_6F13B8A30603__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Mesh.h"

class CPick
{
public:
	CPick();
	virtual ~CPick();

public:
	CN3Mesh m_MeshBox; 

protected:
    __Vector3		m_vPickRayDir;			
    __Vector3		m_vPickRayOrig;			

public:
	BOOL PickByBox(__Vector3 &vMin, __Vector3 &vMax, __Vector3& vIntersect);
	void			SetPickXY(long x, long y); 

	
	
	
	
	
	
	bool IntersectTriangle(__Vector3& v0, __Vector3& v1, __Vector3& v2, float& t, float& u, float& v, __Vector3* pVPos)
	{
		return ::_IntersectTriangle( m_vPickRayOrig, m_vPickRayDir, v0, v1, v2, t, u, v, pVPos );
	}
};

#endif 
