

#if !defined(AFX_N3UISTATIC_H__CF0050A8_2813_40CB_B696_1CB689FDCC41__INCLUDED_)
#define AFX_N3UISTATIC_H__CF0050A8_2813_40CB_B696_1CB689FDCC41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

class CN3UIStatic : public CN3UIBase  
{
public:
	CN3UIStatic();
	virtual ~CN3UIStatic();

protected:
	CN3UIString*	m_pBuffOutRef;		
	CN3UIImage*		m_pImageBkGnd;		
	CN3SndObj*		m_pSnd_Click;	
	

public:
	virtual const std::string& GetString();
	virtual void	SetString(const std::string& szString);
	virtual void	Release();

	virtual bool	Load(HANDLE);
	virtual void	SetRegion(const RECT& Rect);
	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
};	
#endif 
