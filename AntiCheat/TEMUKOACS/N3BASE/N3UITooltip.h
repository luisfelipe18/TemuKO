

#if !defined(AFX_N3UITOOLTIP_H__7085B857_C8EE_410D_AB27_5332D26DF01A__INCLUDED_)
#define AFX_N3UITOOLTIP_H__7085B857_C8EE_410D_AB27_5332D26DF01A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIStatic.h"

class CN3UITooltip : public CN3UIStatic
{
public:
	CN3UITooltip();
	virtual ~CN3UITooltip();

public:
protected:
	float			m_fHoverTime;	
	bool			m_bSetText;		
	POINT			m_ptCursor;		

public:
	void			SetText(const std::string& szText);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render();
	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

};

#endif 
