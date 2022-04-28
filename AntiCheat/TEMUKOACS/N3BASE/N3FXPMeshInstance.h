

#if !defined(AFX_N3FXPMESHINSTANCE_H__19060ED0_7DDA_4279_A558_B586319324F0__INCLUDED_)
#define AFX_N3FXPMESHINSTANCE_H__19060ED0_7DDA_4279_A558_B586319324F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Base.h"
#include "N3PMesh.h"
#include "N3FXPMesh.h"

#include <string>

class CN3FXPMeshInstance   : public CN3Base
{
public:
	CN3FXPMeshInstance();
	CN3FXPMeshInstance(CN3FXPMesh *pN3FXPMesh);
	CN3FXPMeshInstance(const std::string& szFN);
	virtual ~CN3FXPMeshInstance();

protected:
	__VertexXyzColorT1* m_pColorVertices;
	uint16_t*				m_pIndices;

	
	int					m_iNumIndices;
	int					m_iNumVertices; 

	CN3PMesh::__EdgeCollapse*	m_pCollapseUpTo;	

	
	
	CN3FXPMesh*	m_pFXPMesh;

public:
	void		SetColor(uint32_t dwColor = 0xffffffff);
	BOOL		IsLOD() { if(NULL == m_pCollapseUpTo) return FALSE; return TRUE; }
	bool		Create(CN3FXPMesh* pN3FXPMesh);
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
	CN3FXPMesh*	GetMesh() {return m_pFXPMesh;};

	__VertexXyzColorT1*	GetVertices() const;
	uint16_t*				GetIndices() const { return m_pIndices;};
};

#endif 
