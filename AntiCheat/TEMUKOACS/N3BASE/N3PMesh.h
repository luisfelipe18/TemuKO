

#if !defined(AFX_N3PMesh_h__INCLUDED_)
#define AFX_N3PMesh_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3BaseFileAccess.h"

class CN3PMesh : public CN3BaseFileAccess
{
friend class CN3FXPMeshInstance;
friend class CN3PMeshInstance;
friend class CN3PMeshCreate;
friend class CPMeshEditor;			

public:
	struct  __LODCtrlValue
	{
		float	fDist;
		int		iNumVertices;
	};

	
	struct __EdgeCollapse
	{
	
		int NumIndicesToLose, NumIndicesToChange, NumVerticesToLose;
		
		int iIndexChanges;	
		int CollapseTo;
		bool	bShouldCollapse;	
	};

protected:

	
	int m_iNumCollapses;			
	int m_iTotalIndexChanges;
	int *m_pAllIndexChanges; 

	
	

	__EdgeCollapse	*m_pCollapses;

	
#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	LPDIRECT3DINDEXBUFFER8	m_pIB;
#else
	__VertexT1*		m_pVertices;
	__VertexT2*		m_pVertices2;
	uint16_t*			m_pIndices;
#endif
	int m_iMaxNumVertices, m_iMaxNumIndices;
	int m_iMinNumVertices, m_iMinNumIndices;

	__Vector3	m_vMin, m_vMax; 
	float		m_fRadius; 

	
	int					m_iLODCtrlValueCount;
	__LODCtrlValue*		m_pLODCtrlValues;

public:

	__Vector3 Min() { return m_vMin; } 
	__Vector3 Max() { return m_vMax; } 

	void Release();
	HRESULT Create(int iNumVertices, int iNumIndices);
	HRESULT GenerateSecondUV();
	
	bool Load(HANDLE hFile);
	void FindMinMax();

	float Radius() { return m_fRadius; }
	int	GetMinNumVertices() const {return m_iMinNumVertices;};
	int	GetMaxNumVertices() const {return m_iMaxNumVertices;};
	int GetMaxNumIndices() const {return m_iMaxNumIndices;};
	int GetMinNumIndices() const {return m_iMinNumIndices;};

	__VertexT1*		GetVertices() const { return m_pVertices; };
	__VertexT2*		GetVertices2() const { return m_pVertices2; };
	uint16_t*			GetIndices() const { return m_pIndices;};		

	CN3PMesh();
	virtual ~CN3PMesh();
};

#endif 
