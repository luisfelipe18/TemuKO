

#include "N3UIScrollBar.h"
#include "N3UIButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIScrollBar::CN3UIScrollBar()
{
	m_eType = UI_TYPE_SCROLLBAR;
	m_pTrackBarRef = NULL;
	ZeroMemory(m_pBtnRef, sizeof(CN3UIButton*)*NUM_BTN_TYPE);
	m_iLineSize = 1;
}

CN3UIScrollBar::~CN3UIScrollBar()
{
}

void CN3UIScrollBar::Release()
{
	CN3UIBase::Release();
	m_pTrackBarRef = NULL;
	ZeroMemory(m_pBtnRef, sizeof(CN3UIButton*)*NUM_BTN_TYPE);
	m_iLineSize = 1;
}

bool CN3UIScrollBar::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	__ASSERT(NULL == m_pTrackBarRef, "scrollbar가 초기화되어 있지 않아여");

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_TRACKBAR == pChild->UIType())
		{
			m_pTrackBarRef = (CN3UITrackBar*)pChild;
		}
		else if (UI_TYPE_BUTTON == pChild->UIType())
		{
			int iBtnType = pChild->GetReserved();
			if (iBtnType<0 || iBtnType >= NUM_BTN_TYPE) continue;
			m_pBtnRef[iBtnType] = (CN3UIButton*)pChild;
		}
	}
	return true;
}

void CN3UIScrollBar::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	

}

bool CN3UIScrollBar::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (UIMSG_TRACKBAR_POS == dwMsg)
	{
		if (m_pParent) return m_pParent->ReceiveMessage(this, UIMSG_SCROLLBAR_POS);
	}
	else if (UIMSG_BUTTON_CLICK == dwMsg)
	{
		if (m_pTrackBarRef)
		{
			if (BTN_LEFTUP == pSender->GetReserved()) m_pTrackBarRef->SetCurrentPos(m_pTrackBarRef->GetPos()-m_iLineSize);
			else if (BTN_RIGHTDOWN == pSender->GetReserved()) m_pTrackBarRef->SetCurrentPos(m_pTrackBarRef->GetPos()+m_iLineSize);

			if (m_pParent) return m_pParent->ReceiveMessage(this, UIMSG_SCROLLBAR_POS);
		}
	}
	return true;
}

void CN3UIScrollBar::SetStyle(uint32_t dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	if (UISTYLE_SCROLLBAR_HORIZONTAL == dwStyle)
	{
		if (m_pTrackBarRef) m_pTrackBarRef->SetStyle(UISTYLE_TRACKBAR_HORIZONTAL);
	}
	else
	{
		if (m_pTrackBarRef) m_pTrackBarRef->SetStyle(UISTYLE_TRACKBAR_VERTICAL);
	}
}
