

#if !defined(AFX_N3VMesh_h__INCLUDED_)
#define AFX_N3VMesh_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3BaseFileAccess.h"

class CN3IMesh;

class CN3VMesh : public CN3BaseFileAccess
{
protected:
	__Vector3*	m_pVertices; 
	int			m_nVC; 

	uint16_t*		m_pwIndices; 
	int			m_nIC; 

	__Vector3	m_vMin;
	__Vector3	m_vMax;
	float		m_fRadius; 

public:
	__Vector3	Min() { return m_vMin; }
	__Vector3	Max() { return m_vMax; }
	bool		Pick(const __Matrix44& MtxWorld, const __Vector3& vPos, const __Vector3& vDir, __Vector3* pVCol = NULL, __Vector3* pVNormal = NULL);
	bool		CheckCollision(const __Matrix44& MtxWorld, const __Vector3& v0, const __Vector3& v1, __Vector3* pVCol = NULL, __Vector3* pVNormal = NULL); 
	float		Radius() { return m_fRadius; }
	void		FindMinMax();
	void		Render(D3DCOLOR crLine);
	void		CreateVertices(int nVC);
	void		CreateIndex(int nIC);
	void		CreateCube(const __Vector3& vMin, const __Vector3& vMax);
	
	int			VertexCount() { return m_nVC; }
	__Vector3*	Vertices() { return m_pVertices; }

	int			IndexCount() { return m_nIC; }
	uint16_t*		Indices() { return m_pwIndices; }

	bool		Load(HANDLE hFile);
	
	void		Release();

	CN3VMesh();
	virtual ~CN3VMesh();

	int				GetColIndexByiOrder(int iOrder);
__Vector3	  GetColVertexByIndex(int iIndex); 
	void			PartialColRender(int iCount, int* piIndices);
	void			PartialGetCollision(int iIndex, __Vector3& vec);

};

#endif 
