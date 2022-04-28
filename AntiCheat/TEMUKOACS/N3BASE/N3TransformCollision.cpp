

#include "N3TransformCollision.h"
#include "N3VMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3TransformCollision::CN3TransformCollision()
{
	m_dwType |= OBJ_TRANSFORM_COLLISION;

	m_fRadius = 0;
	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);

	m_pMeshCollision = NULL;
	m_pMeshClimb = NULL;
}

CN3TransformCollision::~CN3TransformCollision()
{
	s_MngVMesh.Delete(&m_pMeshCollision);
	s_MngVMesh.Delete(&m_pMeshClimb);
}

void CN3TransformCollision::Release()
{
	m_fRadius = 0;
	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);

	s_MngVMesh.Delete(&m_pMeshCollision);
	s_MngVMesh.Delete(&m_pMeshClimb);

	CN3Transform::Release();
}

bool CN3TransformCollision::Load(HANDLE hFile)
{
	CN3Transform::Load(hFile);

	int nL = 0;
	char szFN[512] = "";

	DWORD dwRWC;
	ReadFile(hFile, &nL, 4, &dwRWC, NULL); 
	if(nL > 0)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL; 
		m_pMeshCollision = s_MngVMesh.Get(szFN);
	}

	ReadFile(hFile, &nL, 4, &dwRWC, NULL); 
	if(nL > 0)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL; 
		m_pMeshClimb = s_MngVMesh.Get(szFN);
	}
	return true;
}

void CN3TransformCollision::CollisionMeshSet(const std::string& szFN)
{
	s_MngVMesh.Delete(&m_pMeshCollision);
	m_pMeshCollision = s_MngVMesh.Get(szFN);
	if(m_pMeshCollision) this->FindMinMax();
}

void CN3TransformCollision::ClimbMeshSet(const std::string& szFN)
{
	s_MngVMesh.Delete(&m_pMeshClimb);
	m_pMeshClimb = s_MngVMesh.Get(szFN);
	if(m_pMeshClimb) m_pMeshClimb->FindMinMax();
}

int CN3TransformCollision::CheckCollisionPrecisely(bool bIgnoreBoxCheck, int ixScreen, int iyScreen, __Vector3* pVCol, __Vector3* pVNormal)
{
	__Vector3 vPos, vDir; 
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	if(false == m_pMeshCollision->Pick(m_Matrix, vPos, vDir, pVCol, pVNormal)) return -1;
	else return 0;
}

#ifdef _DEBUG
void CN3TransformCollision::RenderCollisionMesh()
{
	if(NULL == m_pMeshCollision) return;
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_Matrix);

	m_pMeshCollision->Render(0xffff0000); 
}

void CN3TransformCollision::RenderClimbMesh()
{
	if(NULL == m_pMeshClimb) return;
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_Matrix);

	m_pMeshClimb->Render(0xff0000ff); 
}
#endif

void CN3TransformCollision::FindMinMax()
{
	m_vMin.Set(0,0,0);
	m_vMax.Set(0,0,0);
	m_fRadius = 0.0f;

	if(NULL == m_pMeshCollision || m_pMeshCollision->VertexCount() <= 0) return;

	m_vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	
	int nVC = m_pMeshCollision->VertexCount();
	__Vector3* pVs = m_pMeshCollision->Vertices();
	for(int i = 0; i < nVC; i++)
	{
		if(pVs[i].x < m_vMin.x) m_vMin.x = pVs[i].x;
		if(pVs[i].y < m_vMin.y) m_vMin.y = pVs[i].y;
		if(pVs[i].z < m_vMin.z) m_vMin.z = pVs[i].z;
		if(pVs[i].x > m_vMax.x) m_vMax.x = pVs[i].x;
		if(pVs[i].y > m_vMax.y) m_vMax.y = pVs[i].y;
		if(pVs[i].z > m_vMax.z) m_vMax.z = pVs[i].z;
	}

	
	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}
