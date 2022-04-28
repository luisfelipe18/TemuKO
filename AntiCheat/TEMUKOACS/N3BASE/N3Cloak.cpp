

#include "N3Cloak.h"
#include "N3Texture.h"
#include "N3PMeshInstance.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3Cloak::CN3Cloak()
{

	m_pTex = NULL;
	m_pParticle = NULL;
	m_bpPlayerBase = NULL;
	m_nLOD = -1;
	m_pPMesh = NULL;
	m_pIndex = NULL;
	m_pVertex = NULL;
	m_fOffsetRecoveryTime = 0.0f;
	m_fPrevYaw = 0.0f;
	m_eAnchorPattern = AMP_NONE;
	m_fAnchorPreserveTime = 0.0f;
}

CN3Cloak::~CN3Cloak()
{
	CN3Cloak::Release();
}

void CN3Cloak::Release()
{
	if (m_pParticle)
		delete[] m_pParticle, m_pParticle = NULL;
	if (m_pIndex)
		delete[] m_pIndex, m_pIndex = NULL;
	if (m_pVertex)
		delete[] m_pVertex, m_pVertex = NULL;
}

void CN3Cloak::Init(CN3CPlug_Cloak *pPlugCloak)
{

	__ASSERT(m_pPMesh && m_pTex, "IN CN3Cloak, PMesh or m_pTex is null");

	SetLOD(0);

	
	m_GravityForce.x = 0.0f;
	m_GravityForce.z = 0.0005f;
	
	m_GravityForce.y = -0.0005f;

	m_Force.x = m_Force.y = m_Force.z = 0.0f;
}

void CN3Cloak::Tick(int nLOD)
{
	m_fAnchorPreserveTime -= s_fSecPerFrm;
	if (m_eAnchorPattern != AMP_NONE)
	{
		if (m_fAnchorPreserveTime < 0.0f)		
		{
			RestoreAnchorLine();
		}
	}

	TickByPlayerMotion();
	TickYaw();
	
	UpdateLocalForce();
	ApplyForce();

	m_Force.x = m_Force.y = m_Force.z = 0.0f;
}

void CN3Cloak::Render(__Matrix44 &mtx)
{
	if (m_nLOD < 0)	
		return;

	s_lpD3DDev->SetTransform( D3DTS_WORLD, &mtx);
	DWORD dwCull, dwLight;
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	
	if (dwCull != D3DCULL_NONE)
		s_lpD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

	s_lpD3DDev->SetTexture(0, m_pTex->Get());
	s_lpD3DDev->SetTexture(1, NULL);
	
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	s_lpD3DDev->SetFVF(FVF_VNT1);
	s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nVertexCount, m_nIndexCount/3, m_pIndex, D3DFMT_INDEX16 ,m_pVertex, sizeof(__VertexT1));

	
	__VertexXyzColor	Vtx[2];

	for (int i = 0 ; i< m_nGridH;i++)
	{
		for (int j=0;j<m_nGridW-1;j++)
		{
			
			int nIndex = i*m_nGridW+j;
			Vtx[0].Set(m_pParticle[nIndex].x, m_pParticle[nIndex].y, m_pParticle[nIndex].z, 0xffffffff);
			nIndex++;
			Vtx[1].Set(m_pParticle[nIndex].x, m_pParticle[nIndex].y, m_pParticle[nIndex].z, 0xffffffff);
			s_lpD3DDev->SetFVF(FVF_CV);
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, Vtx, sizeof(__VertexXyzColor));
		}
	}
	for (int i = 0 ; i< m_nGridH-1;i++)
	{
		for (int j=0;j<m_nGridW;j++)
		{
			
			int nIndex = i*m_nGridW+j;
			Vtx[0].Set(m_pParticle[nIndex].x, m_pParticle[nIndex].y, m_pParticle[nIndex].z, 0xffffffff);
			nIndex+=m_nGridW;
			Vtx[1].Set(m_pParticle[nIndex].x, m_pParticle[nIndex].y, m_pParticle[nIndex].z, 0xffffffff);
			s_lpD3DDev->SetFVF(FVF_CV);
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, Vtx, sizeof(__VertexXyzColor));
		}
	}
	
	if (dwCull != D3DCULL_NONE)
		s_lpD3DDev->SetRenderState( D3DRS_CULLMODE, dwCull);
}

#define SPRING_TOLERANCE	0.000001f
#define SPRING_COEFFICIENT		0.3f
void CN3Cloak::UpdateLocalForce()
{
	float length;

	int i, j, index, idx;
	D3DXVECTOR3 up, down, left, right;
	const float nSub = 0.2f;

	for ( i = 0; i < m_nGridH; i ++ )
	{
		for ( j = 0; j < m_nGridW; j ++ )
		{
			index = i*m_nGridW + j;			
			
			up.x = down.x = left.x = right.x = 0; 
			up.y = down.y = left.y = right.y = 0; 
			up.z = down.z = left.z = right.z = 0; 
			
			if ( j-1 >= 0 )
			{
				idx = i*m_nGridW + (j-1);
				up.x = m_pParticle[idx].x - m_pParticle[index].x; 
				up.y = m_pParticle[idx].y - m_pParticle[index].y; 
				up.z = m_pParticle[idx].z - m_pParticle[index].z;
				
				length = sqrt(up.x * up.x + up.y * up.y + up.z * up.z);

				if (length > SPRING_TOLERANCE)
				{
					length = 1.0f-nSub/length;

					up.x = (length ) * up.x;
					up.y = (length ) * up.y;
					up.z = (length ) * up.z;
				}
			}
			if ( j+1 < m_nGridW)
			{
				idx = i * m_nGridW + j+1;
				down.x = m_pParticle[idx].x - m_pParticle[index].x; 
				down.y = m_pParticle[idx].y - m_pParticle[index].y; 
				down.z = m_pParticle[idx].z - m_pParticle[index].z; 

				length = sqrt(down.x * down.x + down.y * down.y + down.z * down.z);
				if (length > SPRING_TOLERANCE)
				{				
					length = 1.0f-nSub/length;
					down.x = (length ) * down.x ;
					down.y = (length ) * down.y ;
					down.z = (length ) * down.z ;
				}
			}
			if ( i-1 >= 0 )
			{
				idx = (i-1) * m_nGridW + j;
				left.x = m_pParticle[idx].x - m_pParticle[index].x; 
				left.y = m_pParticle[idx].y - m_pParticle[index].y; 
				left.z = m_pParticle[idx].z - m_pParticle[index].z; 

				length = sqrt(left.x * left.x + left.y * left.y + left.z * left.z);
				if (length > SPRING_TOLERANCE)
				{
					length = 1.0f-nSub/length;					
					left.x = (length ) * left.x ;
					left.y = (length ) * left.y ;
					left.z = (length ) * left.z ;
				}
			}
			if ( i+1 < m_nGridH )
			{
				idx = (i+1) * m_nGridW + j;
				right.x = m_pParticle[idx].x - m_pParticle[index].x; 
				right.y = m_pParticle[idx].y - m_pParticle[index].y; 
				right.z = m_pParticle[idx].z - m_pParticle[index].z; 

				length = sqrt(right.x * right.x + right.y * right.y + right.z * right.z);
				if (length > SPRING_TOLERANCE)
				{
					length = 1.0f-nSub/length;
					right.x = (length ) * right.x ;
					right.y = (length ) * right.y ;
					right.z = (length ) * right.z ;
				}
			}
			
			m_pParticle[index].LocalForce.x = (up.x + down.x + left.x + right.x)*SPRING_COEFFICIENT;
			m_pParticle[index].LocalForce.y = (up.y + down.y + left.y + right.y)*SPRING_COEFFICIENT;
			m_pParticle[index].LocalForce.z = (up.z + down.z + left.z + right.z)*SPRING_COEFFICIENT;
		}
	}
}

void CN3Cloak::ApplyForce()
{
	const float energy_loss = 0.99f;
	__Particle *pParticle = m_pParticle;
	__VertexT1	*pVtx;
	for (int i=m_nGridW;i<m_nGridH*m_nGridW;i++)
	{
		pParticle = m_pParticle+i;
		pVtx = m_pVertex+i+m_nGridW*CLOAK_SKIP_LINE;	
		{
			

			pParticle->vx += (m_GravityForce.x + m_Force.x + pParticle->LocalForce.x) ;
			pParticle->vy += (m_GravityForce.y + m_Force.y + pParticle->LocalForce.y) ;
			pParticle->vz += (m_GravityForce.z + m_Force.z + pParticle->LocalForce.z) ;

			pParticle->x += pParticle->vx ;
			pParticle->y += pParticle->vy ;
			pParticle->z += pParticle->vz ;

			pVtx->x += pParticle->vx;
			pVtx->y += pParticle->vy;
			pVtx->z += pParticle->vz;

			
			pParticle->vx *= energy_loss;
			pParticle->vy *= energy_loss;;
			pParticle->vz *= energy_loss;			
		}
	}
}

void CN3Cloak::SetLOD(int nLevel)
{
	if (nLevel == m_nLOD)	return;

	if (m_pIndex)
		delete[] m_pIndex, m_pIndex = NULL;

	memset(m_vOffset, 0, sizeof(D3DXVECTOR3)*CLOAK_MAX_WIDTH);

	switch(nLevel)
	{
	case 0:
		{
			m_nGridW = 7;
			m_nGridH = 5;

			int nVertexCount = m_pPMesh->GetMaxNumVertices();
			int nIndexCount = m_pPMesh->GetMaxNumIndices();
			m_pVertex = new __VertexT1[nVertexCount];
			memcpy(m_pVertex, m_pPMesh->GetVertices(), sizeof(__VertexT1)*nVertexCount);
			m_pIndex = new uint16_t[nIndexCount];
			memcpy(m_pIndex, m_pPMesh->GetIndices(), sizeof(uint16_t)*nIndexCount);
			m_nVertexCount = nVertexCount;
			m_nIndexCount = nIndexCount;
			
		}
		break;
	case 1:
		{

		}
		break;
	case 2:
		break;
	case 3:
		break;
	}	

	if (m_pParticle)
		delete[] m_pParticle;
	
	m_pParticle = new __Particle[m_nGridW*m_nGridH];
	__Particle *pParticle = m_pParticle;
	int i,j;
	for (i=0;i<m_nGridH;i++)
	{
		for (j=0;j<m_nGridW;j++)
		{
			pParticle->Set(0.5f, 2.0f-j*.2f, 2.0f-i*.2f, 0.0f, 0.0f, 0.0f, 0.0f);
			pParticle++;
		}
	}

	m_nLOD = nLevel;
	
}

void CN3Cloak::ApplyOffset(D3DXVECTOR3	&vDif)
{	
	
}

void CN3Cloak::TickYaw()
{
	if (!m_bpPlayerBase)	return;

}

void CN3Cloak::TickByPlayerMotion()
{
	
}

void CN3Cloak::MoveAnchorLine(e_Cloak_AnchorMovePattern eType, float fPreserveTime)
{
	if (m_eAnchorPattern != AMP_NONE)	return;

	static float Weight[CLOAK_MAX_WIDTH] = {-0.03f, -0.02f, -0.01f, 0.0f, 0.01f, 0.02f, 0.03f};

	switch(eType)
	{
	case AMP_MOVEXZ:
		{
			float x_Offset = (rand()%2-1)*0.01f;
			float z_Offset = 0.03f;

			for(int i=0;i<m_nGridW;i++)
			{
				m_pParticle[i].x += x_Offset;
				m_pParticle[i].z += z_Offset;
				m_vOffset[i].x = x_Offset;
				m_vOffset[i].z = z_Offset;
			}
			
		}
		break;
	case AMP_MOVEXZ2:
		{
			float x_Offset = (rand()%2-1)*0.02f;
			float z_Offset = 0.04f;

			for(int i=0;i<m_nGridW;i++)
			{
				m_pParticle[i].x += x_Offset;
				m_pParticle[i].z += z_Offset;
				m_vOffset[i].x = x_Offset;
				m_vOffset[i].z = z_Offset;

			}
			
		}
		break;
	case AMP_YAWCCW:
		{
			for (int i=0;i<m_nGridW;i++)
			{
				m_pParticle[i].z += Weight[i];
				m_vOffset[i].z = Weight[i];
			}
			
		}
		break;
	case AMP_YAWCW:
		{
			for (int i=0;i<m_nGridW;i++)
			{
				m_pParticle[i].z += Weight[i];
				m_vOffset[i].z = Weight[i];
			}
			
		}	
		break;
	default:
		break;
	}

	m_fAnchorPreserveTime = fPreserveTime;
	m_eAnchorPattern = eType;
}

void CN3Cloak::RestoreAnchorLine()
{
	__ASSERT(m_eAnchorPattern != AMP_NONE, "call berserk");
	if (m_eAnchorPattern == AMP_NONE)	return;

	__Particle *pParticle = m_pParticle;
	for (int i=0;i<m_nGridW;i++)
	{	
		pParticle->x -= m_vOffset[i].x;
		pParticle->y -= m_vOffset[i].y;
		pParticle->z -= m_vOffset[i].z;
		m_vOffset[i].x = m_vOffset[i].y = m_vOffset[i].z = 0.0f;
		pParticle++;
	}

	m_eAnchorPattern = AMP_NONE;
	m_fAnchorPreserveTime = 1.0f;
	
}
