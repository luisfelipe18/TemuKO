

#ifndef __N3FXSHAPE_H__
#define __N3FXSHAPE_H__

#pragma warning(disable : 4786)

#include "N3TransformCollision.h"
#include "N3Texture.h"
#include "N3FXPMesh.h"
#include "N3FXPMeshInstance.h"

#include <vector>

class CN3FXSPart : public CN3BaseFileAccess
{
friend class CN3FXShape;

public:
	__Material	m_Mtl;					
	__Vector3	m_vPivot;				
	__Matrix44	m_WorldMtx;				
	BOOL		m_bOutOfCameraRange;	

	float		m_fTexFPS;				
	bool		m_bTexLoop;

	CN3FXShape*	m_pRefShape;

protected:
	std::vector<CN3Texture*>	m_TexRefs;		
	CN3FXPMeshInstance			m_FXPMInst;		

	float						m_fTexIndex;	

public:
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);
	void Duplicate(CN3FXSPart* pSrc);
		
	size_t			TexCount() { return m_TexRefs.size(); }
	CN3Texture* Tex(size_t iIndex);	
	void		TexAlloc(int nCount);
	CN3Texture*	TexSet(size_t iIndex, const std::string& szFN);
	void		TexSet(size_t iIndex, CN3Texture* pTex);

	__Vector3 Min() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Min() * m_WorldMtx; else return __Vector3(0,0,0); } 
	__Vector3 Max() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Max() * m_WorldMtx; else return __Vector3(0,0,0); } 
	float	Radius() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Radius(); else return 0.0f; }

	CN3FXPMesh*	Mesh() { return m_FXPMInst.GetMesh(); }
	__VertexXyzColorT1* GetColorVertices() { return m_FXPMInst.GetVertices(); }
	void	SetColor(uint32_t dwColor = 0xffffffff) { m_FXPMInst.SetColor(dwColor); }
	bool	MeshSet(const std::string& szFN);
	void	Tick(const __Matrix44& mtxParent);
	void	Render();
	void	Release();
	
	CN3FXSPart();
	virtual ~CN3FXSPart();
};

class CN3FXShape : public CN3TransformCollision
{
public:
	std::vector<CN3FXSPart*>	m_Parts; 

	__Matrix44		m_mtxParent;
	__Matrix44		m_mtxFinalTransform;

	uint32_t			m_dwSrcBlend;
	uint32_t			m_dwDestBlend;
	BOOL			m_bAlpha;

	uint32_t			m_dwZEnable;
	uint32_t			m_dwZWrite;
	uint32_t			m_dwLight;
	uint32_t			m_dwDoubleSide;
	
public:
	void			SetPartsMtl(BOOL bAlpha, uint32_t dwSrcBlend, uint32_t dwDestBlend, uint32_t dwZEnable, uint32_t dwZWrite, uint32_t dwLight, uint32_t dwDoubleSide);
	__Vector3		CenterPos();

	void			SetCurrFrm(float fFrm);
	float			GetCurrFrm();
	float			GetWholeFrm() { return m_fFrmWhole; }

	void			FindMinMax();

	CN3FXSPart*		Part(size_t iIndex) { if (iIndex >= m_Parts.size()) return NULL; return m_Parts[iIndex]; }
	CN3FXSPart*		PartAdd() { CN3FXSPart* pPart = new CN3FXSPart(); m_Parts.push_back(pPart); return pPart; }
	size_t			PartCount() { return m_Parts.size(); }
	void			PartDelete(size_t iIndex);
	
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXShape* pSrc);

	void	Tick(float fFrm = FRAME_SELFPLAY);
	void	Render();
	void	Release();		

public:
	void SetMaxLOD();
	CN3FXShape();
	virtual ~CN3FXShape();
};

#endif 
