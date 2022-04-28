

#if !defined(AFX_N3UISCROLLBAR_H__76F849B9_9A93_4439_8FB0_A1DB922CBC73__INCLUDED_)
#define AFX_N3UISCROLLBAR_H__76F849B9_9A93_4439_8FB0_A1DB922CBC73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"
#include "N3UITrackBar.h"

class CN3UIScrollBar : public CN3UIBase 
{
public:
	CN3UIScrollBar();
	virtual ~CN3UIScrollBar();

public:
	enum eBTN_TYPE	{BTN_LEFTUP=0, BTN_RIGHTDOWN, NUM_BTN_TYPE};
protected:
	CN3UITrackBar*	m_pTrackBarRef;
	CN3UIButton*	m_pBtnRef[NUM_BTN_TYPE];

	int				m_iLineSize;		

public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(uint32_t dwStyle);
	virtual bool	ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg); 

	void			SetRange(int iMin, int iMax) {if(m_pTrackBarRef) m_pTrackBarRef->SetRange(iMin,iMax);}
	void			SetRangeMax(int iMax) {if(m_pTrackBarRef) m_pTrackBarRef->SetRangeMax(iMax);}
	void			SetRangeMin(int iMin) {if(m_pTrackBarRef) m_pTrackBarRef->SetRangeMin(iMin);}
	size_t			GetCurrentPos() const {if(m_pTrackBarRef) return m_pTrackBarRef->GetPos(); return 0;}
	void			SetCurrentPos(size_t iPos) {if(m_pTrackBarRef) m_pTrackBarRef->SetCurrentPos(iPos);}
	void			SetPageSize(int iSize) {if(m_pTrackBarRef) m_pTrackBarRef->SetPageSize(iSize);}
	int				GetPageSize() const {if(m_pTrackBarRef) return m_pTrackBarRef->GetPageSize(); return 0;}
	void			SetLineSize(int iSize) {m_iLineSize = iSize;}
	int				GetLineSize() const {return m_iLineSize;}
	int				GetMaxPos() const {if (m_pTrackBarRef) return m_pTrackBarRef->GetMaxPos(); return 0;}

};

#endif 
