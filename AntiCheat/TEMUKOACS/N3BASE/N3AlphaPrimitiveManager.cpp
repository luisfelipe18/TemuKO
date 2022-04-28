

#include "N3AlphaPrimitiveManager.h"
#include "N3Base.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3AlphaPrimitiveManager::CN3AlphaPrimitiveManager()
{
	m_nToDrawCount = 0;
}

CN3AlphaPrimitiveManager::~CN3AlphaPrimitiveManager()
{
}

void CN3AlphaPrimitiveManager::Render()
{
	__ASSERT(CN3Base::s_lpD3DDev, "3d device pointer is NULL!!!");
	if(m_nToDrawCount <= 0) return;
	
	static __AlphaPrimitive* pBuffs[MAX_ALPHAPRIMITIVE_BUFFER];
	for(int i = 0; i < m_nToDrawCount; i++) pBuffs[i] = &(m_Buffers[i]);
	qsort(pBuffs, m_nToDrawCount, 4, SortByCameraDistance); 

	struct __RenderState
	{
		DWORD dwAlpha, dwFog, dwCull, dwLgt, dwZWrite, dwAO, dwAA1, dwAA2, dwCO, dwCA1, dwCA2, dwPointSampling;
		DWORD dwAddressU, dwAddressV;
		DWORD dwSrcBlend, dwDestBlend;
		DWORD dwZEnable;
	};
	__RenderState RS_old;		
	__RenderState RS_current;	

	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &RS_old.dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &RS_old.dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &RS_old.dwCull);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &RS_old.dwLgt);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &RS_old.dwZWrite);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &RS_old.dwZEnable);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND,  &RS_old.dwSrcBlend );
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &RS_old.dwDestBlend);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &RS_old.dwAO);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &RS_old.dwAA1);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &RS_old.dwAA2);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_COLOROP,   &RS_old.dwCO);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_COLORARG1, &RS_old.dwCA1);
	CN3Base::s_lpD3DDev->GetTextureStageState( 0, D3DTSS_COLORARG2, &RS_old.dwCA2);
	CN3Base::s_lpD3DDev->GetSamplerState( 0, D3DSAMP_MIPFILTER, &RS_old.dwPointSampling);
	CN3Base::s_lpD3DDev->GetSamplerState( 0, D3DSAMP_ADDRESSU, &RS_old.dwAddressU);
	CN3Base::s_lpD3DDev->GetSamplerState( 0, D3DSAMP_ADDRESSV, &RS_old.dwAddressV);

	RS_current = RS_old;

	if(TRUE != RS_current.dwAlpha) {RS_current.dwAlpha = TRUE; CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);}
	static __Material mtl; mtl.Init();
	CN3Base::s_lpD3DDev->SetMaterial(&mtl);

	for(int i = 0; i < m_nToDrawCount; i++)
	{

		if(pBuffs[i]->nRenderFlags & RF_NOTUSEFOG) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, RS_old.dwFog);
		if(pBuffs[i]->nRenderFlags & RF_DOUBLESIDED) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, RS_old.dwCull);
		if(pBuffs[i]->nRenderFlags & RF_NOTUSELIGHT) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, RS_old.dwLgt);
		if(pBuffs[i]->nRenderFlags & RF_NOTZWRITE) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, RS_old.dwZWrite);
		if(pBuffs[i]->nRenderFlags & RF_NOTZBUFFER) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, RS_old.dwZEnable);
		if(pBuffs[i]->nRenderFlags & RF_DIFFUSEALPHA)
		{
			if(pBuffs[i]->lpTex)
			{
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
			}
			else
			{
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			}
		}
		else
		{
			if(pBuffs[i]->lpTex)
			{
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   RS_old.dwAO );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, RS_old.dwAA1 );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, RS_old.dwAA2 );
			}
			else
			{
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   RS_old.dwAO );
				CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, RS_old.dwAA1 );
			}
		}
		if (pBuffs[i]->nRenderFlags & RF_POINTSAMPLING) CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		else CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, RS_old.dwPointSampling);
		if (pBuffs[i]->nRenderFlags & RF_UV_CLAMP)
		{
			CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		}
		else
		{
			CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, RS_old.dwAddressU );
			CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, RS_old.dwAddressV );
		}
		
		if (pBuffs[i]->dwBlendSrc != RS_current.dwSrcBlend)	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   pBuffs[i]->dwBlendSrc);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   RS_old.dwSrcBlend);
		if (pBuffs[i]->dwBlendDest != RS_current.dwDestBlend) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  pBuffs[i]->dwBlendDest);
		else CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  RS_old.dwDestBlend);

		CN3Base::s_lpD3DDev->SetFVF(pBuffs[i]->dwFVF);
		CN3Base::s_lpD3DDev->SetTexture(0, pBuffs[i]->lpTex);
		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &(pBuffs[i]->MtxWorld)); 

		if(pBuffs[i]->lpTex)
		{
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		}
		else
		{
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		}
	
		if(pBuffs[i]->pwIndices && pBuffs[i]->nPrimitiveCount > 0) 
		{
			if (pBuffs[i]->bUseVB)
			{
				CN3Base::s_lpD3DDev->SetStreamSource( 0, (LPDIRECT3DVERTEXBUFFER9)pBuffs[i]->pVertices, 0, pBuffs[i]->dwPrimitiveSize );
				CN3Base::s_lpD3DDev->SetIndices((LPDIRECT3DINDEXBUFFER9)pBuffs[i]->pwIndices);
				CN3Base::s_lpD3DDev->DrawIndexedPrimitive(	pBuffs[i]->ePrimitiveType,
												0,
												0,
												pBuffs[i]->nVertexCount,
												0,
												pBuffs[i]->nPrimitiveCount );
			}
			else
			{
				CN3Base::s_lpD3DDev->DrawIndexedPrimitiveUP(	pBuffs[i]->ePrimitiveType, 
												0,
												pBuffs[i]->nVertexCount,
												pBuffs[i]->nPrimitiveCount,
												pBuffs[i]->pwIndices,
												D3DFMT_INDEX16,
												pBuffs[i]->pVertices,
												pBuffs[i]->dwPrimitiveSize );
			}
		}
		else if(pBuffs[i]->nPrimitiveCount > 0)
		{
			if (pBuffs[i]->bUseVB)
			{
				CN3Base::s_lpD3DDev->SetStreamSource( 0, (LPDIRECT3DVERTEXBUFFER9)pBuffs[i]->pVertices, 0, pBuffs[i]->dwPrimitiveSize );
				CN3Base::s_lpD3DDev->DrawPrimitive(	pBuffs[i]->ePrimitiveType,
										0,
										pBuffs[i]->nPrimitiveCount );
			}
			else
			{
				CN3Base::s_lpD3DDev->DrawPrimitiveUP(	pBuffs[i]->ePrimitiveType, 
										pBuffs[i]->nPrimitiveCount,
										pBuffs[i]->pVertices,
										pBuffs[i]->dwPrimitiveSize );
			}
		}

#ifdef _DEBUG
		CN3Base::s_RenderInfo.nAlpha_Polygon += pBuffs[i]->nPrimitiveCount / 3; 
#endif
	}
		
	m_nToDrawCount = 0; 

	
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, RS_old.dwAlpha);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, RS_old.dwFog);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, RS_old.dwCull);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, RS_old.dwLgt);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, RS_old.dwZWrite);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, RS_old.dwZEnable);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,  RS_old.dwSrcBlend );
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, RS_old.dwDestBlend);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   RS_old.dwAO);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, RS_old.dwAA1);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, RS_old.dwAA2);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   RS_old.dwCO);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, RS_old.dwCA1);
	CN3Base::s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, RS_old.dwCA2);
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, RS_old.dwPointSampling);
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU,  RS_old.dwAddressU);
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV,  RS_old.dwAddressV);

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

}

__AlphaPrimitive* CN3AlphaPrimitiveManager::Add()
{

	if(m_nToDrawCount >= MAX_ALPHAPRIMITIVE_BUFFER) return NULL;
	m_nToDrawCount++;
	return &(m_Buffers[m_nToDrawCount-1]);
}

int CN3AlphaPrimitiveManager::SortByCameraDistance(const void *pArg1, const void *pArg2)
{
	__AlphaPrimitive *pObj1 = *((__AlphaPrimitive**)pArg1);
	__AlphaPrimitive *pObj2 = *((__AlphaPrimitive**)pArg2);

	if(pObj1->fCameraDistance > pObj2->fCameraDistance) return -1; 
	else if(pObj1->fCameraDistance < pObj2->fCameraDistance) return 1;
	else return 0;
}
