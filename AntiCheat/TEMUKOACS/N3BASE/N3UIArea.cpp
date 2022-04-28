

#include "N3UIArea.h"
#include "N3UIEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIArea::CN3UIArea()
{
	m_eType = UI_TYPE_AREA;
	m_eAreaType = UI_AREA_TYPE_NONE;
}

CN3UIArea::~CN3UIArea()
{
}

void CN3UIArea::Release()
{
	CN3UIBase::Release();
}

void CN3UIArea::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

bool CN3UIArea::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

#ifndef _REPENT
	
	DWORD dwNum;
	int iAreaType;
	ReadFile(hFile, &iAreaType, sizeof(int), &dwNum, NULL);	
	m_eAreaType = (eUI_AREA_TYPE)iAreaType;
#endif
	return true;
}

uint32_t CN3UIArea::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	
	if(IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBCLICK) )	
	{
		m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); 
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

#ifndef _REPENT
#ifdef _N3GAME
bool CN3UIArea::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{

	return true;
}
#endif
#endif
