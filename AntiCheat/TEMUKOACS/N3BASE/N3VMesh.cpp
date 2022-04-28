

#include "N3VMesh.h"
#include "N3IMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3VMesh::CN3VMesh()
{
	m_dwType |= OBJ_MESH_VECTOR3;
	
	m_pVertices = NULL; 
	m_pwIndices = NULL; 

	m_nVC = 0;
	m_nIC = 0;

	m_vMin.Zero();
	m_vMax.Zero();
	m_fRadius = 0.0f; 
}

CN3VMesh::~CN3VMesh()
{
	if(m_nVC > 32768) ::GlobalFree((HGLOBAL)(m_pVertices));
	else delete [] m_pVertices;
	delete [] m_pwIndices; m_pwIndices = NULL;
}

void CN3VMesh::Release()
{
	if(m_nVC > 32768) ::GlobalFree((HGLOBAL)(m_pVertices));
	else delete [] m_pVertices;
	m_pVertices = NULL;
	m_nVC = 0;

	delete [] m_pwIndices; m_pwIndices = NULL;
	m_nIC = 0;

	m_vMin.Zero();
	m_vMax.Zero();
	m_fRadius = 0.0f; 
}

bool CN3VMesh::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC = 0;

	int nVC;
	ReadFile(hFile, &nVC, 4, &dwRWC, NULL); 
	if(nVC > 0)
	{
		this->CreateVertices(nVC); 
		ReadFile(hFile, m_pVertices, nVC * sizeof(__Vector3), &dwRWC, NULL);
	}

	int nIC;
	ReadFile(hFile, &nIC, 4, &dwRWC, NULL); 
	if(nIC > 0)
	{
		this->CreateIndex(nIC); 
		ReadFile(hFile, m_pwIndices, nIC * 2, &dwRWC, NULL);
	}

	this->FindMinMax(); 

	return true;
}

void CN3VMesh::CreateVertices(int nVC)
{
	if(nVC <= 0) return;

	if(m_nVC > 32768) ::GlobalFree((HGLOBAL)(m_pVertices));
	else delete [] m_pVertices;

	m_pVertices = NULL;

	if(nVC > 32768)
	{
		m_pVertices = (__Vector3*)(::GlobalAlloc(GMEM_FIXED, nVC * sizeof(__Vector3)));
	}
	else
	{
		m_pVertices = new __Vector3[nVC];
	}

	memset(m_pVertices, 0, nVC * sizeof(__Vector3)); 
	m_nVC = nVC;
}

void CN3VMesh::CreateIndex(int nIC)
{
	__ASSERT(nIC > 0, "Index count is less than 0");
	
#ifdef _N3GAME
	if(nIC > 32768)
		CLogWriter::Write("N3VMesh creation warning (%s) - Too many Indices.", m_szFileName.c_str());
#endif

	delete [] m_pwIndices;
	m_pwIndices = new uint16_t[nIC];
	memset(m_pwIndices, 0, nIC * 2); 
	m_nIC = nIC;
}

void CN3VMesh::CreateCube(const __Vector3 &vMin, const __Vector3 &vMax)
{
	this->CreateVertices(8);
	this->CreateIndex(36);

	m_pVertices[0].Set(vMin.x, vMax.y, vMin.z); 
	m_pVertices[1].Set(vMax.x, vMax.y, vMin.z); 
	m_pVertices[2].Set(vMax.x, vMin.y, vMin.z); 
	m_pVertices[3].Set(vMin.x, vMin.y, vMin.z); 

	m_pVertices[4].Set(vMax.x, vMax.y, vMax.z); 
	m_pVertices[5].Set(vMin.x, vMax.y, vMax.z); 
	m_pVertices[6].Set(vMin.x, vMin.y, vMax.z); 
	m_pVertices[7].Set(vMax.x, vMin.y, vMax.z); 

	m_pwIndices[0] = 0; m_pwIndices[1] = 1; m_pwIndices[2] = 2; 
	m_pwIndices[3] = 0; m_pwIndices[4] = 2; m_pwIndices[5] = 3; 
	m_pwIndices[6] = 1; m_pwIndices[7] = 4; m_pwIndices[8] = 7;
	m_pwIndices[9] = 1; m_pwIndices[10] = 7; m_pwIndices[11] = 2;
	m_pwIndices[12] = 4; m_pwIndices[13] = 5; m_pwIndices[14] = 6;
	m_pwIndices[15] = 4; m_pwIndices[16] = 6; m_pwIndices[17] = 7;
	m_pwIndices[18] = 5; m_pwIndices[19] = 0; m_pwIndices[20] = 3;
	m_pwIndices[21] = 5; m_pwIndices[22] = 3; m_pwIndices[23] = 6;
	m_pwIndices[24] = 5; m_pwIndices[25] = 4; m_pwIndices[26] = 1;
	m_pwIndices[27] = 5; m_pwIndices[28] = 1; m_pwIndices[29] = 0;
	m_pwIndices[30] = 3; m_pwIndices[31] = 2; m_pwIndices[32] = 7;
	m_pwIndices[33] = 3; m_pwIndices[34] = 7; m_pwIndices[35] = 6;

	this->FindMinMax(); 
}

#ifdef _DEBUG
void CN3VMesh::Render(D3DCOLOR crLine)
{
	if(m_nVC <= 0) return;

	DWORD dwLight, dwShade;
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	s_lpD3DDev->GetRenderState(D3DRS_FILLMODE, &dwShade);
	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	if(D3DFILL_WIREFRAME != dwShade) s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	s_lpD3DDev->SetTexture(0, NULL);
	s_lpD3DDev->SetFVF(FVF_CV);

	__VertexColor vTs[3];
	if(m_nIC)
	{
		int nFC = m_nIC / 3;
		for(int i = 0; i < nFC; i++)
		{
			vTs[0].Set(m_pVertices[m_pwIndices[i*3+0]], crLine);
			vTs[1].Set(m_pVertices[m_pwIndices[i*3+1]], crLine);
			vTs[2].Set(m_pVertices[m_pwIndices[i*3+2]], crLine);

			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vTs, sizeof(__VertexColor));
		}
	}
	else
	{
		int nFC = m_nVC / 3;
		for(int i = 0; i < nFC; i++)
		{
			vTs[0].Set(m_pVertices[i*3+0], crLine);
			vTs[1].Set(m_pVertices[i*3+1], crLine);
			vTs[2].Set(m_pVertices[i*3+2], crLine);

			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vTs, sizeof(__VertexColor));
		}
	}

	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
	if(D3DFILL_WIREFRAME != dwShade) s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, dwShade);
}
#endif 

void CN3VMesh::FindMinMax()
{
	m_vMin.Zero();
	m_vMax.Zero();
	m_fRadius = 0;

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

	
	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}

bool CN3VMesh::CheckCollision(const __Matrix44& MtxWorld, const __Vector3& v0, const __Vector3& v1, __Vector3* pVCol, __Vector3* pVNormal)
{
	if(m_nVC <= 0) return false;

	static __Vector3 vPos0, vPos1, vDir, vColTmp;
	static __Matrix44 mtxWI, mtxWIRot, mtxRot;
	static float fT, fU, fV, fDistTmp, fDistClosest;
	fDistClosest = FLT_MAX;

	D3DXMatrixInverse(&mtxWI, NULL, &MtxWorld); 
	mtxWIRot = mtxWI;
	mtxWIRot.PosSet(0,0,0);
	mtxRot = MtxWorld;
	mtxRot.PosSet(0,0,0);

	vPos0 = v0 * mtxWI;
	vPos1 = v1 * mtxWI;
	vDir = vPos1 - vPos0;

	int nFC = 0;
	if(m_nIC > 0) nFC = m_nIC / 3;
	else nFC = m_nVC / 3;

	int nCI0, nCI1, nCI2; 
	for(int i = 0; i < nFC; i++)
	{
		if(m_nIC > 0) { nCI0 = m_pwIndices[i*3+0]; nCI1 = m_pwIndices[i*3+1]; nCI2 = m_pwIndices[i*3+2]; }
		else { nCI0 = i*3; nCI1 = i*3+1; nCI2 = i*3+2; }

		if(false == ::_IntersectTriangle(vPos0, vDir, m_pVertices[nCI0], m_pVertices[nCI1], m_pVertices[nCI2], fT, fU, fV, &vColTmp)) continue; 
		if(false == ::_IntersectTriangle(vPos1, vDir, m_pVertices[nCI0], m_pVertices[nCI1], m_pVertices[nCI2])) 
		{
			fDistTmp = (vPos0 - vColTmp).Magnitude(); 
			if(fDistTmp < fDistClosest) 
			{
				fDistClosest = fDistTmp;
				
				if(pVCol) *pVCol = vColTmp * MtxWorld;

				
				if(pVNormal)
				{
					(*pVNormal).Cross(m_pVertices[nCI1] - m_pVertices[nCI0], m_pVertices[nCI2] - m_pVertices[nCI1]);
					(*pVNormal) *= mtxRot;
					(*pVNormal).Normalize();
				}
			}
		}
	}
	if(fDistClosest != FLT_MAX) return true;

	
	__Vector3 tmpNormal;
	float d;
	for(int i = 0; i < nFC; i++)
	{
		if(m_nIC > 0) { nCI0 = m_pwIndices[i*3+0]; nCI1 = m_pwIndices[i*3+1]; nCI2 = m_pwIndices[i*3+2]; }
		else { nCI0 = i*3; nCI1 = i*3+1; nCI2 = i*3+2; }

		tmpNormal.Cross(m_pVertices[nCI1] - m_pVertices[nCI0], m_pVertices[nCI2] - m_pVertices[nCI1]);
		d = -(tmpNormal.x*m_pVertices[nCI0].x) - (tmpNormal.y*m_pVertices[nCI0].y) - (tmpNormal.z*m_pVertices[nCI0].z);
		if( ((tmpNormal.x*vPos0.x)+(tmpNormal.y*vPos0.y)+(tmpNormal.z*vPos0.z)+d) > 0) return false;
	}

	
	return true;
}

bool CN3VMesh::Pick(const __Matrix44& MtxWorld, const __Vector3& vPos, const __Vector3& vDir, __Vector3* pVCol, __Vector3* pVNormal)
{
	if(m_nVC <= 0) return false;

	static __Matrix44 mtxWI, mtxWIRot, mtxRot;
	D3DXMatrixInverse(&mtxWI, NULL, &MtxWorld); 
	mtxWIRot = mtxWI;
	mtxWIRot.PosSet(0,0,0);
	mtxRot = MtxWorld;
	mtxRot.PosSet(0,0,0);
	__Vector3 vPos2 = vPos * mtxWI;
	__Vector3 vDir2 = vDir * mtxWIRot;

	int nFC = 0;
	if(m_nIC > 0) nFC = m_nIC / 3;
	else nFC = m_nVC / 3;

	int nCI0, nCI1, nCI2; 
	for(int i = 0; i < nFC; i++)
	{
		if(m_nIC > 0) { nCI0 = m_pwIndices[i*3+0]; nCI1 = m_pwIndices[i*3+1]; nCI2 = m_pwIndices[i*3+2]; }
		else { nCI0 = i*3; nCI1 = i*3+1; nCI2 = i*3+2; }

		if(false == ::_IntersectTriangle(vPos2, vDir2, m_pVertices[nCI0], m_pVertices[nCI1], m_pVertices[nCI2])) continue;

		
		if(pVCol)
		{
			float fT, fU, fV;
			::_IntersectTriangle(vPos2, vDir2, m_pVertices[nCI0], m_pVertices[nCI1], m_pVertices[nCI2], fT, fU, fV, pVCol);
			(*pVCol) *= MtxWorld;
		}

		
		if(pVNormal)
		{
			(*pVNormal).Cross(m_pVertices[nCI1] - m_pVertices[nCI0], m_pVertices[nCI2] - m_pVertices[nCI1]);
			(*pVNormal) *= mtxRot;
			(*pVNormal).Normalize();
		}

		return true;
	}

	return false;
}

int	CN3VMesh::GetColIndexByiOrder(int iOrder)
{
	if(m_nIC)
	{
		return m_pwIndices[iOrder];
	}
	else
	{
		return iOrder;
	}
}

__Vector3 CN3VMesh::GetColVertexByIndex(int iIndex)
{
	return m_pVertices[iIndex];
}

void CN3VMesh::PartialColRender(int iCount, int* piIndices)
{
	if(m_nVC <= 0) return;

	DWORD dwLight, dwShade;
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	s_lpD3DDev->GetRenderState(D3DRS_FILLMODE, &dwShade);
	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	if(D3DFILL_WIREFRAME != dwShade) s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	s_lpD3DDev->SetTexture(0, NULL);
	s_lpD3DDev->SetFVF(FVF_CV);

	__VertexColor vTs[3];
	if(iCount)
	{
		int nFC = iCount / 3;
		for(int i = 0; i < nFC; i++)
		{
			vTs[0].Set(m_pVertices[piIndices[i*3+0]], 0xffffffff);
			vTs[1].Set(m_pVertices[piIndices[i*3+1]], 0xffffffff);
			vTs[2].Set(m_pVertices[piIndices[i*3+2]], 0xffffffff);

			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vTs, sizeof(__VertexColor));
		}
	}

	if(dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
	if(D3DFILL_WIREFRAME != dwShade) s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, dwShade);
}

void CN3VMesh::PartialGetCollision(int iIndex, __Vector3& vec)
{
	if(m_nVC <= 0) return;

	vec = m_pVertices[iIndex];
}
