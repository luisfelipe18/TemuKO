

#include "N3PMesh.h"
#include "N3PMeshInstance.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3PMesh::CN3PMesh()
{
	m_dwType |= OBJ_MESH_PROGRESSIVE;

#ifdef _USE_VERTEXBUFFER
	m_pVB = NULL;
	m_pIB = NULL;
#else
	m_pVertices = NULL;
	m_pVertices2 = NULL;
	m_pIndices = NULL;
#endif
	m_pCollapses = NULL;
	m_pAllIndexChanges = NULL;
	m_pLODCtrlValues = NULL;

	m_iNumCollapses = 0;
	m_iTotalIndexChanges = 0;
	m_iMaxNumVertices = 0; m_iMaxNumIndices = 0;
	m_iMinNumVertices = 0; m_iMinNumIndices = 0;
	m_iLODCtrlValueCount = 0;

	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);
	m_fRadius = 0.0f;
}

CN3PMesh::~CN3PMesh()
{
#ifdef _USE_VERTEXBUFFER
	if (m_pVB && m_pVB->Release() == 0) m_pVB = NULL;
	if (m_pIB && m_pIB->Release() == 0) m_pIB = NULL;
#else
	delete [] m_pVertices; m_pVertices = NULL;
	delete [] m_pVertices2; m_pVertices2 = NULL;
	delete [] m_pIndices; m_pIndices = NULL;
#endif
	delete [] m_pCollapses; m_pCollapses = NULL;
	delete [] m_pAllIndexChanges; m_pAllIndexChanges = NULL;
	delete [] m_pLODCtrlValues;	m_pLODCtrlValues = NULL;
}

void CN3PMesh::Release()
{
#ifdef _USE_VERTEXBUFFER
	if (m_pVB) { m_pVB->Release(); m_pVB = NULL;}
	if (m_pIB) { m_pIB->Release(); m_pIB = NULL;}
#else
	if (m_pVertices) { delete[] m_pVertices; m_pVertices = NULL;}
	if (m_pVertices2) { delete[] m_pVertices2; m_pVertices2 = NULL;}
	if (m_pIndices) { delete[] m_pIndices; m_pIndices = NULL;}
#endif
	if (m_pCollapses) {	delete[] m_pCollapses; m_pCollapses = NULL;}
	if (m_pAllIndexChanges)	{ delete[] m_pAllIndexChanges; m_pAllIndexChanges = NULL;}
	if (m_pLODCtrlValues) {	delete [] m_pLODCtrlValues;	m_pLODCtrlValues = NULL;}

	m_iNumCollapses = 0;
	m_iTotalIndexChanges = 0;
	m_iMaxNumVertices = 0; m_iMaxNumIndices = 0;
	m_iMinNumVertices = 0; m_iMinNumIndices = 0;
	m_iLODCtrlValueCount = 0;

	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);
	m_fRadius = 0.0f;

	CN3BaseFileAccess::Release();
}

bool CN3PMesh::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwNum;
	ReadFile(hFile, &m_iNumCollapses, sizeof(m_iNumCollapses), &dwNum, NULL);
	ReadFile(hFile, &m_iTotalIndexChanges, sizeof(m_iTotalIndexChanges), &dwNum, NULL);

	ReadFile(hFile, &m_iMaxNumVertices, sizeof(int), &dwNum, NULL);
	ReadFile(hFile, &m_iMaxNumIndices , sizeof(int), &dwNum, NULL);
	ReadFile(hFile, &m_iMinNumVertices, sizeof(int), &dwNum, NULL);
	ReadFile(hFile, &m_iMinNumIndices , sizeof(int), &dwNum, NULL);

	HRESULT hr = Create(m_iMaxNumVertices, m_iMaxNumIndices);
	__ASSERT(SUCCEEDED(hr), "Failed to create progressive mesh");
#ifdef _USE_VERTEXBUFFER
	if (m_iMaxNumVertices>0)
	{

		uint8_t* pByte;
		hr = m_pVB->Lock(0, 0, &pByte, 0);
		if (FAILED(hr)) return false;
		ReadFile(hFile, pByte, m_iMaxNumVertices*sizeof(__VertexT1), &dwNum, NULL);
		m_pVB->Unlock();
	}
	if (m_iMaxNumIndices>0)
	{

		uint8_t* pByte;
		hr = m_pIB->Lock(0, 0, &pByte, 0);
		if (FAILED(hr)) return false;
		ReadFile(hFile, pByte, m_iMaxNumIndices*sizeof(uint16_t), &dwNum, NULL);
		m_pIB->Unlock();
	}
#else
	if (m_iMaxNumVertices>0)
	{

		ReadFile(hFile, m_pVertices, m_iMaxNumVertices*sizeof(__VertexT1), &dwNum, NULL);
	}

	if (m_iMaxNumIndices>0)
	{

		ReadFile(hFile, m_pIndices, m_iMaxNumIndices*sizeof(uint16_t), &dwNum, NULL);
	}
#endif

	if (m_iNumCollapses>0)
	{
		m_pCollapses = new __EdgeCollapse[m_iNumCollapses+1];	
		ReadFile(hFile, m_pCollapses, m_iNumCollapses*sizeof(__EdgeCollapse), &dwNum, NULL);
		ZeroMemory(m_pCollapses + m_iNumCollapses, sizeof(__EdgeCollapse));	

		bool bFixed = false;
		for(int i = 0; i < m_iNumCollapses; i++)
		{
			if(m_pCollapses[i].iIndexChanges < 0)
			{
				m_pCollapses[i].iIndexChanges = 0;
				bFixed = true;
			}
		}
#ifdef _DEBUG
		if(bFixed)
			::MessageBoxA(s_hWndBase, "잘못된 Progressive Mesh 수정", m_szName.c_str(), MB_OK);
#endif
	}
	if (m_iTotalIndexChanges>0)
	{
		m_pAllIndexChanges = new int[m_iTotalIndexChanges];
		ReadFile(hFile, m_pAllIndexChanges, m_iTotalIndexChanges*sizeof(int), &dwNum, NULL);
	}

	__ASSERT(m_pLODCtrlValues == NULL && m_iLODCtrlValueCount == 0, "Invalid Level of detail control value");
	ReadFile(hFile, &m_iLODCtrlValueCount, sizeof(m_iLODCtrlValueCount), &dwNum, NULL);
	if (m_iLODCtrlValueCount>0)
	{
		m_pLODCtrlValues = new __LODCtrlValue[m_iLODCtrlValueCount];
		ReadFile(hFile, m_pLODCtrlValues, m_iLODCtrlValueCount*sizeof(__LODCtrlValue), &dwNum, NULL);
	}

	FindMinMax();

	return true;
}

void CN3PMesh::FindMinMax()
{
	if(m_iMaxNumVertices <= 0)
	{
		m_vMin.Zero();
		m_vMax.Zero();
		m_fRadius = 0;
		return;
	}

	
	m_vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(int i = 0; i < m_iMaxNumVertices; i++)
	{
		if(m_pVertices[i].x < m_vMin.x) m_vMin.x = m_pVertices[i].x;
		if(m_pVertices[i].y < m_vMin.y) m_vMin.y = m_pVertices[i].y;
		if(m_pVertices[i].z < m_vMin.z) m_vMin.z = m_pVertices[i].z;

		if(m_pVertices[i].x > m_vMax.x) m_vMax.x = m_pVertices[i].x;
		if(m_pVertices[i].y > m_vMax.y) m_vMax.y = m_pVertices[i].y;
		if(m_pVertices[i].z > m_vMax.z) m_vMax.z = m_pVertices[i].z;
	}

	
	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}

HRESULT CN3PMesh::Create(int iNumVertices, int iNumIndices)
{
	m_iMaxNumVertices = iNumVertices;
	m_iMaxNumIndices = iNumIndices;

	if (m_pVertices) {delete m_pVertices; m_pVertices = NULL;}
	if (m_pIndices) {delete m_pIndices; m_pIndices = NULL;}

	if (m_iMaxNumVertices>0)
	{
		m_pVertices = new __VertexT1[m_iMaxNumVertices];
	}
	if (m_iMaxNumIndices>0)
	{
		m_pIndices = new uint16_t[m_iMaxNumIndices];
	}

	return S_OK;
}

HRESULT CN3PMesh::GenerateSecondUV()
{

	if (m_pVertices2) {delete m_pVertices2; m_pVertices2 = NULL;}

	if (m_iMaxNumVertices>0)
	{
		m_pVertices2 = new __VertexT2[m_iMaxNumVertices];

		for(int i = 0; i < m_iMaxNumVertices; i++)
		{
			m_pVertices2[i].x = m_pVertices[i].x;
			m_pVertices2[i].y = m_pVertices[i].y;
			m_pVertices2[i].z = m_pVertices[i].z;
			m_pVertices2[i].n = m_pVertices[i].n;
			m_pVertices2[i].tu = m_pVertices[i].tu;
			m_pVertices2[i].tv = m_pVertices[i].tv;
			m_pVertices2[i].tu2 = m_pVertices[i].tu;
			m_pVertices2[i].tv2 = m_pVertices[i].tv;
		}
	}
	return S_OK;
}
