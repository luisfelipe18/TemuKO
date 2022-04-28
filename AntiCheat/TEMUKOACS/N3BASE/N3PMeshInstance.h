

#if !defined(AFX_N3PMeshInstance_h__INCLUDED_)
#define AFX_N3PMeshInstance_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Base.h"
#include "N3PMesh.h"

#include <string>

class CN3PMeshInstance : public CN3Base
{
public:
	CN3PMeshInstance();
	CN3PMeshInstance(CN3PMesh *pN3PMesh);
	CN3PMeshInstance(const std::string& szFN);
	virtual ~CN3PMeshInstance();

protected:
#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DINDEXBUFFER8	m_pIB;
#else
	uint16_t* m_pIndices;
#endif

	int m_iNumIndices, m_iNumVertices; 

	CN3PMesh::__EdgeCollapse*	m_pCollapseUpTo;	

	
	
	CN3PMesh*	m_pPMesh;	

public:
	BOOL		IsLOD() { if(NULL == m_pCollapseUpTo) return FALSE; return TRUE; }
	bool		Create(CN3PMesh* pN3Mesh);
	bool		Create(const std::string& szFN);
	void		Release();
	void		Render();
	void		RenderTwoUV();
	
	void		SetLOD(float value); 

	
	
	bool		CollapseOne();
	bool		SplitOne();

	void		SetLODByNumVertices(int iNumVertices);
	int			GetNumVertices() const {return m_iNumVertices;};
	int			GetNumIndices() const {return m_iNumIndices;};
	CN3PMesh*	GetMesh() {return m_pPMesh;};

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER8	GetVertexBuffer() const;
	LPDIRECT3DINDEXBUFFER8 GetIndexBuffer() const {return m_pIB;}
#else
	__VertexT1*	GetVertices() const;
	uint16_t*		GetIndices() const { return m_pIndices;};
#endif

#ifdef _USE_VERTEXBUFFER
	void			PartialRender(int iCount, LPDIRECT3DINDEXBUFFER8 pIB);
#else
	void			PartialRender(int iCount, uint16_t* pIndices);
#endif
	int				 GetIndexByiOrder(int iOrder);
__Vector3		GetVertexByIndex(size_t iIndex);

};

#endif 
