

#if !defined(AFX_N3BOARD_H__A6E96276_7491_4722_82C9_EA06561D1D41__INCLUDED_)
#define AFX_N3BOARD_H__A6E96276_7491_4722_82C9_EA06561D1D41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Transform.h"
#include <vector>
#include <string>

const uint32_t BOARD_X				= 0;
const uint32_t BOARD_Y				= 1;
const uint32_t BOARD_Z				= 2;
const uint32_t BOARD_XYZ			= 3;

class CN3Board : public CN3Transform
{
protected:
	__VertexT1 m_vRects[4];
	float		m_fTexIndex;
	std::vector<CN3Texture*> m_TexRefs; 

public:
	uint32_t		m_dwBoardType; 
	float		m_fTexFPS; 
	__Material	m_Mtl; 

public:
	void		TexSet(int index, const std::string& szFN);
	void		TexAlloc(int nCount);
	int			TexCount() { m_TexRefs.size(); }
	CN3Texture* Tex(int index) { if(m_TexRefs.empty() || index < 0 || index >= m_TexRefs.size()) return NULL; return m_TexRefs[index]; }

	void		Init(__Vector3 vPos, uint32_t dwBoardType, float fW, float fH);
	void		Tick(float fFrm);
	void		Render();

	bool		Load(HANDLE hFile);
	void		LoadFromText(const std::string& szFName);
	
	void		Release();

	CN3Board();
	virtual ~CN3Board();
};

#endif 
