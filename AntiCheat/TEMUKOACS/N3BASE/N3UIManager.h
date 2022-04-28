

#if !defined(AFX_N3UIMANAGER_H__1EED48B7_90A3_4242_99BD_ABC12040CDDE__INCLUDED_)
#define AFX_N3UIMANAGER_H__1EED48B7_90A3_4242_99BD_ABC12040CDDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

class CN3UIManager : public CN3UIBase  
{
protected:
	struct __RenderStateForUI
	{
		uint32_t dwZEnable, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwFog;
		uint32_t dwMagFilter, dwMinFilter, dwMipFilter;
#ifdef _DEBUG
		BOOL	bSet;	
#endif
	};
	
	static __RenderStateForUI s_sRSFU; 

	uint32_t	m_dwMouseFlagsCur;
public:
	CN3UIManager();
	virtual ~CN3UIManager();

public:
	static void	RenderStateRestore();
	static void	RenderStateSet();

	void	Release();
	void	Tick();
	uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	uint32_t	GetCurMouseFlags() { return m_dwMouseFlagsCur; }
	void	Render();
	void	ReorderChildList();
};

#endif 
