

#if !defined(AFX_N3FXPMESH_H__18E8E609_322F_4F0F_839B_0411986DA76E__INCLUDED_)
#define AFX_N3FXPMESH_H__18E8E609_322F_4F0F_839B_0411986DA76E__INCLUDED_

#include "N3BaseFileAccess.h"
#include "N3PMesh.h"

class CN3FXPMesh : public CN3PMesh
{
friend class CN3FXPMeshInstance;

private:
	__VertexXyzColorT1*		m_pColorVertices;

public:
	void					Render();
	void					Release();
	void					FindMinMax();
	bool					Load(HANDLE hFile);
	void					SetColor(uint32_t dwColor = 0xffffffff);
	HRESULT					Create(int iNumVertices, int iNumIndices);
	void					operator=(const CN3FXPMesh& fxPMesh);
	__VertexXyzColorT1*		GetColorVertices() { return m_pColorVertices; }

public:
	CN3FXPMesh();
	virtual ~CN3FXPMesh();
};

#endif
