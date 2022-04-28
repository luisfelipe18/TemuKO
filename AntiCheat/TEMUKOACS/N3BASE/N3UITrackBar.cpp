

#include "N3UITrackBar.h"
#include "N3UIImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UITrackBar::CN3UITrackBar()
{
	m_eType = UI_TYPE_TRACKBAR;

	m_pBkGndImageRef = NULL;
	m_pThumbImageRef = NULL;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

CN3UITrackBar::~CN3UITrackBar()
{
}

void CN3UITrackBar::Release()
{
	CN3UIBase::Release();
	m_pBkGndImageRef = NULL;
	m_pThumbImageRef = NULL;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

bool CN3UITrackBar::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	
		int iImageType = (int)(pChild->GetReserved());
		if (IMAGETYPE_BKGND == iImageType)
		{
			m_pBkGndImageRef = (CN3UIImage*)pChild;
		}
		else if (IMAGETYPE_THUMB == iImageType)
		{
			m_pThumbImageRef = (CN3UIImage*)pChild;
		}
	}
	return true;
}

void CN3UITrackBar::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	if(m_pBkGndImageRef) m_pBkGndImageRef->SetRegion(m_rcRegion);	
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	int iThumbWidth = rcThumb.right - rcThumb.left;		int iThumbHeight = rcThumb.bottom - rcThumb.top;
	int iBkWidth = Rect.right - Rect.left;			int iBkHeight = Rect.bottom - Rect.top;
	if (iBkWidth<=0 || iBkHeight<=0) return;
	
	if ( iThumbWidth<=0 && iThumbHeight<=0 )
	{	
		RECT rc;
		if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
		{	
			rc.left = Rect.left;		rc.top = Rect.top + (int)(iBkHeight*0.3f);
			rc.right = Rect.right;	rc.bottom = rc.top + (int)(iBkHeight*0.3f);
		}
		else
		{	
			rc.left = Rect.left + (int)(iBkWidth*0.3f);		rc.top = Rect.top;
			rc.right = rc.left + (int)(iBkWidth*0.3f);			rc.bottom = Rect.bottom;
		}
		m_pThumbImageRef->SetRegion(rc);
	}
	UpdateThumbPos();	
}

uint32_t CN3UITrackBar::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if (false == IsIn(ptCur.x, ptCur.y))	
	{
		SetState(UI_STATE_COMMON_NONE);
		return dwRet;
	}
	dwRet |= UI_MOUSEPROC_INREGION;	

	if (UI_STATE_TRACKBAR_THUMBDRAG == m_eState)
	{
		if(dwFlags & UI_MOUSE_LBCLICKED)  
		{
			SetState(UI_STATE_COMMON_NONE);		
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (dwFlags & UI_MOUSE_LBDOWN)
		{
			
			if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)	UpDownThumbPos(ptCur.y - ptOld.y);
			else UpDownThumbPos(ptCur.x - ptOld.x);
			
			if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}
	else
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  
		{
			if (m_pThumbImageRef->IsIn(ptCur.x, ptCur.y))	
			{
				SetState(UI_STATE_TRACKBAR_THUMBDRAG);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else	
			{
				RECT rcThumb = m_pThumbImageRef->GetRegion();
				if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
				{
					if (ptCur.y <= rcThumb.top) SetCurrentPos(m_iCurPos-m_iPageSize);
					else SetCurrentPos(m_iCurPos+m_iPageSize);
				}
				else
				{
					if (ptCur.x <= rcThumb.left) SetCurrentPos(m_iCurPos-m_iPageSize);
					else SetCurrentPos(m_iCurPos+m_iPageSize);
				}
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UITrackBar::SetRange(size_t iMin, size_t iMax)
{
	if (m_iMaxPos == iMax && m_iMinPos == iMin) return;
	m_iMaxPos = iMax;		m_iMinPos = iMin;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

void CN3UITrackBar::SetCurrentPos(size_t iPos)
{
	if (iPos == m_iCurPos) return;
	m_iCurPos = iPos;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

void CN3UITrackBar::UpdateThumbPos()
{
	if (NULL == m_pThumbImageRef) return;
	float fDiff = (float)m_iMaxPos - (float)m_iMinPos;
	if (0.0f == fDiff) return;
	float fPercentage = (float)m_iCurPos/fDiff;
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
	{
		int iDY = (int)(fPercentage*((m_rcRegion.bottom - m_rcRegion.top) - (rcThumb.bottom - rcThumb.top)));
		m_pThumbImageRef->SetPos(	m_rcRegion.left, m_rcRegion.top + iDY);
	}
	else
	{
		int iDX = (int)(fPercentage*((m_rcRegion.right - m_rcRegion.left) - (rcThumb.right - rcThumb.left)));
		m_pThumbImageRef->SetPos( m_rcRegion.left + iDX, m_rcRegion.top );
	}
}

void CN3UITrackBar::UpDownThumbPos(int iDiff)
{
	if (NULL == m_pThumbImageRef) return;
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)		
	{
		int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;
		int iThumbHeight = rcThumb.bottom - rcThumb.top;
		if (0==iRegionHeight || 0==iThumbHeight) return;

		
		float fPercentage = ((rcThumb.top-m_rcRegion.top)+iDiff) / (((float)(iRegionHeight)) - iThumbHeight);

		if (fPercentage>1.0f)	
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.bottom-iThumbHeight);
			m_iCurPos = m_iMaxPos;		
		}
		else if (fPercentage<0.0f)	
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top+iDiff);
			m_iCurPos = (size_t)(m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage);
		}
	}
	else											
	{
		int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
		int iThumbWidth = rcThumb.right - rcThumb.left;
		if (0==iRegionWidth || 0==iThumbWidth) return;

		
		float fPercentage = ((rcThumb.left-m_rcRegion.left)+iDiff) / (((float)(iRegionWidth)) - iThumbWidth);

		if (fPercentage>1.0f)	
		{
			m_pThumbImageRef->SetPos(rcThumb.right-iThumbWidth, rcThumb.top);
			m_iCurPos = m_iMaxPos;
		}
		else if (fPercentage<0.0f)	
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left+iDiff, rcThumb.top);
			m_iCurPos = (size_t)(m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage);
		}
	}
}
