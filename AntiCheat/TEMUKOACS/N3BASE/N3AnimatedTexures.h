

#if !defined(AFX_N3ANIMATEDTEXURES_H__8CA538E1_C4C9_45DA_9A73_D70F23C6281F__INCLUDED_)
#define AFX_N3ANIMATEDTEXURES_H__8CA538E1_C4C9_45DA_9A73_D70F23C6281F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3BaseFileAccess.h"
#include <vector>
#include "N3Texture.h"	

class CN3AnimatedTexures : public CN3BaseFileAccess
{
protected:
	std::vector<class CN3Texture*> m_TexRefs;
	float m_fTexIndex;
	float m_fTexFPS;

public:
	void Tick();
	CN3Texture* Tex(int iIndex) { if(iIndex < 0 || iIndex >= m_TexRefs.size()) return NULL; return m_TexRefs[iIndex]; }
	CN3Texture* TexCur() { return this->Tex((int)m_fTexIndex); }
	void Release();
	bool Load(HANDLE hFile);
	CN3AnimatedTexures();
	virtual ~CN3AnimatedTexures();

};

#endif 
