

#if !defined(AFX_N3IMesh_h__INCLUDED_)
#define AFX_N3IMesh_h__INCLUDED_

#include "N3BaseFileAccess.h"

const int MAX_IMESH_BUFFER = 10000;

class CN3IMesh : public CN3BaseFileAccess
{
protected:
	static __VertexT1	s_Vertices[MAX_IMESH_BUFFER];
	static __VertexT2	s_Vertices2[MAX_IMESH_BUFFER];

	int					m_nFC; 

	int					m_nVC; 
	uint16_t*				m_pwVtxIndices; 
	int					m_nUVC; 
	uint16_t*				m_pwUVsIndices; 

	float*				m_pfUVs; 

	__Vector3			m_vMin, m_vMax; 

public:
	__Vector3			Min() { return m_vMin; }
	__Vector3			Max()	{ return m_vMax; }
	void				FindMinMax();
	void				Render(bool bUseTwoUV = false);
	__VertexT1*			BuildVertexList(); 
	__VertexT2*			BuildVertexListTwoUV(); 
	
	int					FaceCount() { return m_nFC; }
	int					VertexCount() { return m_nVC; }

	__VertexXyzNormal*	m_pVertices; 

	__VertexXyzNormal*	Vertices() { return m_pVertices; }
	uint16_t*				VertexInices() { return m_pwVtxIndices; }
	int					UVCount() { return m_nUVC; } 

	virtual bool		Create(int nFC, int nVC, int nUVC); 
	bool				Load(HANDLE hFile);
	
	void Release();
	CN3IMesh();
	virtual ~CN3IMesh();

#ifdef _N3GAME
	void				TickForShadow(bool bUseTwoUV = false);
	int					GetVertexCount();
	__Vector3*			GetVertexData();

private:	
	__Vector3*			m_pVertexT1;
#endif

};

#endif 
