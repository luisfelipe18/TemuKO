

#include "N3UIButton.h"
#include "N3UIImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif
#include <vector>

CN3UIButton::CN3UIButton()
{
	m_eType = UI_TYPE_BUTTON;

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));
	m_pSnd_On = NULL;
	m_pSnd_Click = NULL;
}

CN3UIButton::~CN3UIButton()
{

}

void CN3UIButton::Release()
{
	CN3UIBase::Release();

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));

}

void CN3UIButton::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	SetClickRect(Rect);
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

BOOL CN3UIButton::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	
	m_rcClick.left += iOffsetX;		m_rcClick.top += iOffsetY;
	m_rcClick.right += iOffsetX;	m_rcClick.bottom += iOffsetY;	
	return TRUE;
}

void CN3UIButton::Render()
{
 	if(!m_bVisible) return;

	switch(m_eState)
	{
	case UI_STATE_BUTTON_NORMAL:
		{
			if (m_ImageRef[BS_NORMAL]) m_ImageRef[BS_NORMAL]->Render();
		}
		break;
	case UI_STATE_BUTTON_DOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKDOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKUP:
		{
			if (m_ImageRef[BS_DOWN]) m_ImageRef[BS_DOWN]->Render();
		}
		break;
	case UI_STATE_BUTTON_ON:
		{
			if (m_ImageRef[BS_ON]) m_ImageRef[BS_ON]->Render();
		}
		break;
	case UI_STATE_BUTTON_DISABLE:
		{
			if (m_ImageRef[BS_DISABLE]) m_ImageRef[BS_DISABLE]->Render();
		}
	}

	int i = 0;
	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		for(i = 0; i < NUM_BTN_STATE; i++) 
			if(pChild == m_ImageRef[i]) break;
		if(i >= NUM_BTN_STATE) pChild->Render(); 
	}
}

uint32_t CN3UIButton::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if(false == IsIn(ptCur.x, ptCur.y))	
	{
		if (false == IsIn(ptOld.x, ptOld.y))	return dwRet; 
		dwRet |= UI_MOUSEPROC_PREVINREGION;	

		if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	

		if(UISTYLE_BTN_NORMAL & m_dwStyle) 
		{
			SetState(UI_STATE_BUTTON_NORMAL);	
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) 
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	
				SetState(UI_STATE_BUTTON_DOWN);	
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	
				UI_STATE_BUTTON_ON == m_eState)	
				SetState(UI_STATE_BUTTON_NORMAL);	
		}
		return dwRet; 
	}
	dwRet |= UI_MOUSEPROC_INREGION;	

	if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	

	
	if (FALSE == PtInRect(&m_rcClick, ptCur))
	{
		if(UISTYLE_BTN_NORMAL & m_dwStyle) 
		{
			SetState(UI_STATE_BUTTON_NORMAL);	
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) 
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	
				SetState(UI_STATE_BUTTON_DOWN);	
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	
				UI_STATE_BUTTON_ON == m_eState)	
				SetState(UI_STATE_BUTTON_NORMAL);	
		}
		return dwRet;
	}

	
	
	if(UISTYLE_BTN_NORMAL & m_dwStyle)
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  
		{
			SetState(UI_STATE_BUTTON_DOWN); 
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED) 
		{
			if(m_pParent && UI_STATE_BUTTON_DOWN == m_eState) 
			{
				SetState(UI_STATE_BUTTON_ON); 
				m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); 
			}
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	
		{
			SetState(UI_STATE_BUTTON_ON); 
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			
			
		}
	}

	
	else if(UISTYLE_BTN_CHECK & m_dwStyle) 
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  
		{
			if (UI_STATE_BUTTON_NORMAL == m_eState || UI_STATE_BUTTON_ON == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKDOWN); 
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if(UI_STATE_BUTTON_DOWN == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKUP); 
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED)  
		{
			if(UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState) 
			{
				SetState(UI_STATE_BUTTON_DOWN); 
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); 
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState) 
			{
				SetState(UI_STATE_BUTTON_ON); 
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); 
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	
		{
			SetState(UI_STATE_BUTTON_ON); 
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			
			
		}

	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CN3UIButton::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	DWORD dwNum;
	ReadFile(hFile, &m_rcClick, sizeof(m_rcClick), &dwNum, NULL);	

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	
		int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}

	
	int iSndFNLen = 0;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_On, "memory leak");
	}

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_Click, "memory leak");
	}

	return true;
}

void CN3UIButton::operator = (const CN3UIButton& other)
{
	CN3UIBase::operator = (other);

	m_rcClick = other.m_rcClick;			
	SetSndOn(other.GetSndFName_On());		
	SetSndClick(other.GetSndFName_Click());	

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	
		int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}
}

void CN3UIButton::SetSndOn(const std::string& strFileName)
{
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	

	SetCurrentDirectoryA(tmpBase.PathGet().c_str());
}

void CN3UIButton::SetSndClick(const std::string& strFileName)
{
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	

	SetCurrentDirectoryA(tmpBase.PathGet().c_str());
}

std::string CN3UIButton::GetSndFName_On() const
{
	return std::string("");
}

std::string CN3UIButton::GetSndFName_Click() const
{
	return std::string("");
}

