

#include "N3IMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

__VertexT1 CN3IMesh::s_Vertices[MAX_IMESH_BUFFER];
__VertexT2 CN3IMesh::s_Vertices2[MAX_IMESH_BUFFER];

CN3IMesh::CN3IMesh()
{
	m_dwType |= OBJ_MESH_INDEXED;

	m_nFC = 0;

	m_pwVtxIndices = NULL; 
	m_pwUVsIndices = NULL; 

	m_nVC = m_nUVC = 0;
	m_pVertices = NULL;
	m_pfUVs = NULL;

}

CN3IMesh::~CN3IMesh()
{
	delete [] m_pVertices; m_pVertices = NULL;
	delete [] m_pfUVs; m_pfUVs = NULL;

	delete [] m_pwVtxIndices; m_pwVtxIndices = NULL; 
	delete [] m_pwUVsIndices; m_pwUVsIndices = NULL; 

}

void CN3IMesh::Release()
{
	CN3BaseFileAccess::Release();

	m_nFC = 0;
	m_nVC = 0;
	m_nUVC = 0;

	delete [] m_pVertices; m_pVertices = NULL;
	delete [] m_pfUVs; m_pfUVs = NULL;

	delete [] m_pwVtxIndices; m_pwVtxIndices = NULL; 
	delete [] m_pwUVsIndices; m_pwUVsIndices = NULL; 

	m_vMin.Zero();
	m_vMax.Zero();
}

bool CN3IMesh::Create(int nFC, int nVC, int nUVC)
{
	if(nFC <= 0 || nVC <= 0) return false; 

	if(m_nFC > 0) this->Release();

#ifdef _N3GAME
	if(nFC > 32768)
		CLogWriter::Write("CN3IMesh::Create - Too many faces. (more than 32768) (%s)", m_szFileName.c_str());
#endif

	m_nFC = nFC;
	m_nVC = nVC;
	m_pVertices = new __VertexXyzNormal[nVC]; memset(m_pVertices, 0, sizeof(__VertexXyzNormal) * nVC);
	m_pwVtxIndices = new uint16_t[nFC*3]; memset(m_pwVtxIndices, 0, 2 * nFC * 3); 
	
	if(nUVC > 0)
	{
		m_nUVC = nUVC; m_pfUVs = new float[nUVC*2]; memset(m_pfUVs, 0, 8 * nUVC); 
		m_pwUVsIndices = new uint16_t[nFC*3]; memset(m_pwUVsIndices, 0, 2 * nFC * 3); 
	}

	return true;
}

__VertexT1* CN3IMesh::BuildVertexList()
{
	if(m_nFC <= 0) return NULL;
	if(m_nFC >= MAX_IMESH_BUFFER / 3)
	{
		__ASSERT(0, "Vertex Buffer Overflow");
		return NULL;
	}

	int n = 0, nVI = 0, nUVI = 0;
	if(m_nUVC > 0)
	{
		for(int i = 0; i < m_nFC; i++)
		{
			n = i*3+0; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1] );
			
			n = i*3+1; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1] );

			n = i*3+2; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1] );
		}
	}
	else
	{
		for(int i = 0; i < m_nFC; i++)
		{
			n = i*3+0; nVI = m_pwVtxIndices[n];
			s_Vertices[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 0 );
			
			n = i*3+1; nVI = m_pwVtxIndices[n];
			s_Vertices[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 0 );

			n = i*3+2; nVI = m_pwVtxIndices[n];
			s_Vertices[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 0 );
		}
	}

	return s_Vertices;
}

__VertexT2* CN3IMesh::BuildVertexListTwoUV()
{
	if(m_nFC <= 0) return NULL;
	if(m_nFC >= MAX_IMESH_BUFFER / 3)
	{
		__ASSERT(0, "Vertex Buffer Overflow");
		return NULL;
	}

	int n = 0, nVI = 0, nUVI = 0;
	if(m_nUVC > 0)
	{
		for(int i = 0; i < m_nFC; i++)
		{
			n = i*3+0; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices2[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1],
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1]);
			
			n = i*3+1; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices2[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1],
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1]);

			n = i*3+2; nVI = m_pwVtxIndices[n]; nUVI = m_pwUVsIndices[n];

			s_Vertices2[n].Set(	m_pVertices[nVI].x, m_pVertices[nVI].y, m_pVertices[nVI].z,
								m_pVertices[nVI].n.x, m_pVertices[nVI].n.y, m_pVertices[nVI].n.z, 
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1],
								m_pfUVs[nUVI*2], m_pfUVs[nUVI*2+1]);
		}
	}
	else
	{
		for(int i = 0; i < m_nFC; i++)
		{
			n = i*3+0; nVI = m_pwVtxIndices[n];
			s_Vertices2[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 1, 0, 1 );
			
			n = i*3+1; nVI = m_pwVtxIndices[n];
			s_Vertices2[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 1, 0, 1 );

			n = i*3+2; nVI = m_pwVtxIndices[n];
			s_Vertices2[n].Set(	m_pVertices[nVI], m_pVertices[nVI].n, 0, 1, 0, 1 );
		}
	}

	return s_Vertices2;
}

void CN3IMesh::Render(bool bUseTwoUV)
{
	if(s_lpD3DDev == NULL || m_nFC <= 0) return;

	if(bUseTwoUV)
	{
		__VertexT2* pVs = this->BuildVertexListTwoUV();
		if(pVs)
		{
			s_lpD3DDev->SetFVF(FVF_VNT2);
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_nFC, pVs, sizeof(__VertexT2));
		}
	}
	else
	{
		__VertexT1* pVs = this->BuildVertexList();
		if(pVs)
		{
			s_lpD3DDev->SetFVF(FVF_VNT1);
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_nFC, pVs, sizeof(__VertexT1));
		}
	}

}

bool CN3IMesh::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);
	DWORD dwRWC = 0;
	
	int nFC = 0, nVC = 0, nUVC = 0;

	ReadFile(hFile, &nFC, 4, (DWORD *)&dwRWC, NULL);
	ReadFile(hFile, &nVC, 4, (DWORD *)&dwRWC, NULL);
	ReadFile(hFile, &nUVC, 4, (DWORD *)&dwRWC, NULL);

	if(nFC > 0 && nVC > 0)
	{
		this->Create(nFC, nVC, nUVC);
		ReadFile(hFile, m_pVertices, sizeof(__VertexXyzNormal) * nVC, (DWORD *)&dwRWC, NULL);
		ReadFile(hFile, m_pwVtxIndices, 2 * nFC * 3, (DWORD *)&dwRWC, NULL); 
	}
	else
	{
		Release();
	}
	
	if(m_nUVC > 0)
	{
		ReadFile(hFile, m_pfUVs, 8 * nUVC, (DWORD *)&dwRWC, NULL);
		ReadFile(hFile, m_pwUVsIndices, 2 * nFC * 3, (DWORD *)&dwRWC, NULL); 
	}

	this->FindMinMax(); 

	return true;
}

void CN3IMesh::FindMinMax()
{
	m_vMin.Zero();
	m_vMax.Zero();

	if(m_pVertices == NULL || m_nVC < 0) return;

	
	m_vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for(int i = 0; i < m_nVC; i++)
	{
		if(m_pVertices[i].x < m_vMin.x) m_vMin.x = m_pVertices[i].x;
		if(m_pVertices[i].y < m_vMin.y) m_vMin.y = m_pVertices[i].y;
		if(m_pVertices[i].z < m_vMin.z) m_vMin.z = m_pVertices[i].z;

		if(m_pVertices[i].x > m_vMax.x) m_vMax.x = m_pVertices[i].x;
		if(m_pVertices[i].y > m_vMax.y) m_vMax.y = m_pVertices[i].y;
		if(m_pVertices[i].z > m_vMax.z) m_vMax.z = m_pVertices[i].z;
	}
}

#ifdef _N3GAME
void CN3IMesh::TickForShadow(bool bUseTwoUV)
{
	if(s_lpD3DDev == NULL || m_nFC <= 0) 
	{
		m_pVertexT1 = NULL;
		return;
	}

	if(bUseTwoUV)
	{
		__Vector3* pVs = (__Vector3*)this->BuildVertexListTwoUV();
		if(pVs)
			m_pVertexT1 = pVs;
		else
			m_pVertexT1 = NULL;
	}
	else
	{
		__Vector3* pVs = (__Vector3*)this->BuildVertexList();
		if(pVs)
			m_pVertexT1 = pVs;
		else
			m_pVertexT1 = NULL;
	}
}

int CN3IMesh::GetVertexCount()
{
	return m_nFC;
}

__Vector3* CN3IMesh::GetVertexData()
{
	return m_pVertexT1;
}
#endif
