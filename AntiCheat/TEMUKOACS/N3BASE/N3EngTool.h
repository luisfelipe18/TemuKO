

#if !defined(AFX_D3DENGINETEST_H__4DE7DD27_A9BC_43C5_9D67_E99031ED38B5__INCLUDED_)
#define AFX_D3DENGINETEST_H__4DE7DD27_A9BC_43C5_9D67_E99031ED38B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Eng.h"

struct __EXPORT_OPTION
{
	char szID[8];		
	char szRemark[64];	
	
	int nNodeCount; 
	
	BOOL	bAnimationKey; 
	int		nFrmStart;	
	int		nFrmEnd;	
	float	fSamplingRate; 

	D3DCOLORVALUE dcvBackground;	
	D3DCOLORVALUE dcvAmbientLight;	

	BOOL bExportCamera;	
	BOOL bExportLight;		
	BOOL bExportGeometry;	
	BOOL bExportDummy;	
	BOOL bExportCharacter;	
	
	BOOL bExportSelectedOnly; 

	BOOL bGenerateFileName; 
	BOOL bGenerateSmoothNormal; 

	BOOL bGenerateHalfSizeTexture; 
	BOOL bGenerateCompressedTexture; 

	char szSubDir[_MAX_DIR];		
};

class CN3EngTool : public CN3Eng  
{
public:
	int					m_nGridLineCount; 
	__VertexColor*		m_pVGrids; 

	__VertexColor		m_VAxis[60]; 
	__VertexColor		m_VDir[6]; 
	__VertexTransformed m_VPreview[6];	

public:

	void GridCreate(int nWidth, int nHeight);
	void RenderTexturePreview(CN3Texture* pTex, HWND hDlgWndDiffuse, RECT* pRCSrc = NULL);
	void RenderGrid(const __Matrix44& mtxWorld);
	void RenderAxis(bool bShowDir = false);

	CN3EngTool();
	virtual ~CN3EngTool();

};

#endif 
