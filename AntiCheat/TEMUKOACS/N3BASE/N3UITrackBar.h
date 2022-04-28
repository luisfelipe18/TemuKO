

#if !defined(AFX_N3UITRACKBAR_H__EB496B74_468B_4D7B_89A9_D3A5A1A3E538__INCLUDED_)
#define AFX_N3UITRACKBAR_H__EB496B74_468B_4D7B_89A9_D3A5A1A3E538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

class CN3UITrackBar : public CN3UIBase  
{
public:
	CN3UITrackBar();
	virtual ~CN3UITrackBar();

public:
	enum eIMAGE_TYPE{IMAGETYPE_BKGND=0, IMAGETYPE_THUMB, NUM_IMAGETYPE};
protected:
	CN3UIImage*		m_pBkGndImageRef;		
	CN3UIImage*		m_pThumbImageRef;		

	size_t			m_iMaxPos;									
	size_t			m_iMinPos;									
	size_t 			m_iCurPos;									
	int				m_iPageSize;								

public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);
	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);

	void			SetRange(size_t iMin, size_t iMax);
	void			SetRangeMax(size_t iMax) {SetRange(m_iMinPos, iMax);}
	void			SetRangeMin(size_t iMin) {SetRange(iMin, m_iMaxPos);}
	void			SetCurrentPos(size_t iPos);
	size_t			GetPos() const {return m_iCurPos;}
	void			SetPageSize(int iSize) {m_iPageSize = iSize;}
	int				GetPageSize() const {return m_iPageSize;}
	size_t			GetMaxPos() const {return m_iMaxPos;}
	size_t			GetMinPos() const {return m_iMinPos;}
protected:
	void			UpdateThumbPos();							
	void			UpDownThumbPos(int iDiff);					
};

#endif 
