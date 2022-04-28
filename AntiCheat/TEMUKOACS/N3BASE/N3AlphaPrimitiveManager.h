

#if !defined(AFX_N3ALPHAPRIMITIVEMANAGER_H__616E318C_49E4_4468_9719_E62E384FC48D__INCLUDED_)
#define AFX_N3ALPHAPRIMITIVEMANAGER_H__616E318C_49E4_4468_9719_E62E384FC48D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "../My_3DStruct.h"

struct __AlphaPrimitive
{
	float				fCameraDistance;	
	uint32_t				dwBlendSrc;			
	uint32_t				dwBlendDest;
	int 				nRenderFlags;		
	LPDIRECT3DTEXTURE9	lpTex;				
	uint32_t				dwFVF;				
	D3DPRIMITIVETYPE	ePrimitiveType;	
	int					nPrimitiveCount;	
	uint32_t				dwPrimitiveSize;	
	BOOL				bUseVB;				
	const void*			pwIndices;			
	int					nVertexCount;
	const void*			pVertices;			
	__Matrix44			MtxWorld;			
};

const int MAX_ALPHAPRIMITIVE_BUFFER = 1024;

class CN3AlphaPrimitiveManager
{
protected:
	int					m_nToDrawCount; 
	__AlphaPrimitive	m_Buffers[MAX_ALPHAPRIMITIVE_BUFFER]; 

public:
	int					ToDrawCount() { return m_nToDrawCount; }
	__AlphaPrimitive*	Add();

	void		Render();

	static int SortByCameraDistance(const void *pArg1, const void *pArg2); 
	
	CN3AlphaPrimitiveManager();
	virtual ~CN3AlphaPrimitiveManager();
};

#endif 
