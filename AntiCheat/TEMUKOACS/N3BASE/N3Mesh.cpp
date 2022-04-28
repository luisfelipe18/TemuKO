

#include "N3Mesh.h"
#include "N3IMesh.h"
#include "N3PMesh.h"
#include "N3PMeshInstance.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3Mesh::CN3Mesh()
{
	m_dwType |= OBJ_MESH;

	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);

	m_nVC = 0;
	m_nIC = 0;

	m_pVertices = NULL; 
	m_psnIndices = NULL; 
}

CN3Mesh::~CN3Mesh()
{
	delete [] m_pVertices; m_pVertices = NULL;
	delete [] m_psnIndices; m_psnIndices = NULL;
}

void CN3Mesh::Release()
{
	this->ReleaseVertices();
	this->ReleaseIndices();

	CN3BaseFileAccess::Release();
}

void CN3Mesh::ReleaseVertices()
{
	delete [] m_pVertices; m_pVertices = NULL;
	
	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);

	m_nVC = 0;
}

void CN3Mesh::ReleaseIndices()
{
	delete [] m_psnIndices; m_psnIndices = NULL;
	m_nIC = 0;
}

bool CN3Mesh::Load(HANDLE hFile)
{
	if(m_pVertices != NULL || m_psnIndices != NULL) this->Release();

	DWORD dwRWC = 0;

	ReadFile(hFile, &m_nVC, 4, &dwRWC, NULL); 
	if(m_nVC > 0)
	{
		this->Create(m_nVC, 0); 
		ReadFile(hFile, m_pVertices, m_nVC * sizeof(__VertexT1), &dwRWC, NULL);
		this->FindMinMax(); 
	}
	
	ReadFile(hFile, &m_nIC, 4, &dwRWC, NULL); 
	if(m_nIC > 0)
	{
		this->Create(0, m_nIC); 
		ReadFile(hFile, m_psnIndices, m_nIC * 2, &dwRWC, NULL);
	}

	return true;
}

void CN3Mesh::Create(int nVC, int nIC)
{
	if(nVC > 0) 
	{
#ifdef _N3GAME
		if(nVC > 32768)
			CLogWriter::Write("CN3IMesh::Create - Too many vertices. (more than 32768) (%s)", m_szFileName.c_str());
#endif
		if(m_pVertices) this->ReleaseVertices();
		m_pVertices = new __VertexT1[nVC]; memset(m_pVertices, 0, nVC * sizeof(__VertexT1)); 
		m_nVC = nVC;
	}

	if(nIC > 0) 
	{
#ifdef _N3GAME
		if(nIC > 32768)
			CLogWriter::Write("CN3IMesh::Create - Too many indices. (more than 32768) (%s)", m_szFileName.c_str());
#endif
		if(m_psnIndices) this->ReleaseIndices();
		m_psnIndices = new uint16_t[nIC]; memset(m_psnIndices, 0, nIC * 2); 
		m_nIC = nIC;
	}
}

void CN3Mesh::FindMinMax()
{
	m_vMin.Zero();
	m_vMax.Zero();

	if(m_nVC <= 0) return;

	
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

void CN3Mesh::Create_Cube(__Vector3 &vMin, __Vector3 &vMax)
{
	this->Create(36, 0);

	__Vector3 vPs[6];
	__Vector3 vN;
	float fTUVs[6][2] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };

	int i = 0;

	
	vN.Set(0,0,-1);
	vPs[0].Set(vMin.x, vMax.y, vMin.z); vPs[1].Set(vMax.x, vMax.y, vMin.z); vPs[2].Set(vMax.x, vMin.y, vMin.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMin.x, vMin.y, vMin.z);
	for(i = 0; i < 6; i++) m_pVertices[0+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	
	vN.Set(1, 0, 0);
	vPs[0].Set(vMax.x, vMax.y, vMin.z); vPs[1].Set(vMax.x, vMax.y, vMax.z); vPs[2].Set(vMax.x, vMin.y, vMax.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMax.x, vMin.y, vMin.z);
	for(i = 0; i < 6; i++) m_pVertices[6+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	
	vN.Set(0, 0, 1);
	vPs[0].Set(vMax.x, vMax.y, vMax.z); vPs[1].Set(vMin.x, vMax.y, vMax.z); vPs[2].Set(vMin.x, vMin.y, vMax.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMax.x, vMin.y, vMax.z);
	for(i = 0; i < 6; i++) m_pVertices[12+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	
	vN.Set(-1, 0, 0);
	vPs[0].Set(vMin.x, vMax.y, vMax.z); vPs[1].Set(vMin.x, vMax.y, vMin.z); vPs[2].Set(vMin.x, vMin.y, vMin.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMin.x, vMin.y, vMax.z);
	for(i = 0; i < 6; i++) m_pVertices[18+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	
	vN.Set(0, 1, 0);
	vPs[0].Set(vMin.x, vMax.y, vMax.z); vPs[1].Set(vMax.x, vMax.y, vMax.z); vPs[2].Set(vMax.x, vMax.y, vMin.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMin.x, vMax.y, vMin.z);
	for(i = 0; i < 6; i++) m_pVertices[24+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	
	vN.Set(0, -1, 0);
	vPs[0].Set(vMin.x, vMin.y, vMin.z); vPs[1].Set(vMax.x, vMin.y, vMin.z); vPs[2].Set(vMax.x, vMin.y, vMax.z);
	vPs[3] = vPs[0]; vPs[4] = vPs[2]; vPs[5].Set(vMin.x, vMin.y, vMax.z);
	for(i = 0; i < 6; i++) m_pVertices[30+i].Set(vPs[i], vN, fTUVs[i][0], fTUVs[i][1]);

	this->FindMinMax();
}

bool CN3Mesh::Import(CN3PMesh* pPMesh)
{
	if(NULL == pPMesh) return false;
	int iNumIndices = pPMesh->GetMaxNumIndices();
	if (0 >= iNumIndices) return false;
	Release();	

	
	CN3PMeshInstance PMeshInstance;
	PMeshInstance.Create(pPMesh);
	PMeshInstance.SetLODByNumVertices(pPMesh->GetMaxNumVertices());

	
	Create(PMeshInstance.GetNumVertices(), PMeshInstance.GetNumIndices());

	
	__VertexT1* pVertices = PMeshInstance.GetVertices();
	uint16_t* pIndices = PMeshInstance.GetIndices();
	memcpy(m_pVertices, pVertices, sizeof(__VertexT1)*m_nVC);
	memcpy(m_psnIndices, pIndices, sizeof(uint16_t)*m_nIC);

	m_szName = pPMesh->m_szName; 
	return true;
}
