

#include "N3UIList.h"
#include "N3UIString.h"
#include "N3UIScrollBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIList::CN3UIList()
{
	m_eType = UI_TYPE_LIST;

	m_iCurSel = 0;		
	m_pScrollBarRef = NULL;

	m_szFontName = "����ü";
	m_dwFontHeight = 10;
	m_bFontBold = FALSE;
	m_bFontItalic = FALSE;
	m_crFont = 0xffffffff;
}

CN3UIList::~CN3UIList()
{
}

void CN3UIList::Release()
{
	CN3UIBase::Release();

	m_ListString.clear(); 
	m_iCurSel = 0;
	m_pScrollBarRef = NULL;

	m_szFontName = "����ü";
	m_dwFontHeight = 10;
	m_bFontBold = FALSE;
	m_bFontItalic = FALSE;
	m_crFont = 0xffffffff;
}

void CN3UIList::SetFont(const std::string& szFontName, uint32_t dwHeight, BOOL bBold, BOOL bItalic)
{
	m_szFontName = szFontName;
	m_dwFontHeight = dwHeight;
	m_bFontBold = bBold;
	m_bFontItalic = bItalic;

	it_pString it = m_ListString.begin(), itEnd = m_ListString.end();
	for(int i = 0; it != itEnd; it++, i++)
	{
		(*it)->SetFont(m_szFontName, m_dwFontHeight, m_bFontBold, m_bFontItalic);
	}
	this->UpdateChildRegions();
}

void CN3UIList::SetFontColor(size_t iIndex, D3DCOLOR color)
{
	if (iIndex >= m_ListString.size())
		return;

	auto it = m_ListString.begin();
	std::advance(it, iIndex);
	(*it)->SetColor(color);
}

void CN3UIList::SetFontColor(D3DCOLOR color)
{
	m_crFont = color;
	it_pString it = m_ListString.begin(), itEnd = m_ListString.end();
	for(int i = 0; it != itEnd; it++, i++)
	{
		(*it)->SetColor(m_crFont);
	}
}

int CN3UIList::AddStrings(const std::string* pszStrings, int iStringCount)
{
	for(int i = 0; i < iStringCount; i++)
	{
		CN3UIString* pString = new CN3UIString();
		pString->Init(this);
		pString->SetFont(m_szFontName, m_dwFontHeight, m_bFontBold, m_bFontItalic);
		pString->SetColor(m_crFont);
		pString->SetString(pszStrings[i]);

		m_ListString.push_back(pString);
	}
		
	this->UpdateChildRegions();

	return m_ListString.size() - 1;
}

int	CN3UIList::AddString(const std::string& szString)
{
	CN3UIString* pString = new CN3UIString();
	pString->Init(this);
	pString->SetFont(m_szFontName, m_dwFontHeight, m_bFontBold, m_bFontItalic);
	pString->SetColor(m_crFont);
	pString->SetString(szString);

	m_ListString.push_back(pString);
	this->UpdateChildRegions();

	return m_ListString.size() - 1;
}

bool CN3UIList::InsertString(size_t iIndex, const std::string& szString)
{
	if (iIndex >= m_ListString.size())
		return false;

	CN3UIString* pString = new CN3UIString();
	pString->Init(this);
	pString->SetFont(m_szFontName, m_dwFontHeight, m_bFontBold, m_bFontItalic);
	pString->SetColor(m_crFont);
	pString->SetString(szString);

	auto it = m_ListString.begin();
	std::advance(it, iIndex);
	m_ListString.insert(it, pString);

	this->UpdateChildRegions();
	return true;
}

bool CN3UIList::DeleteString(size_t iIndex)
{
	if (iIndex >= m_ListString.size())
		return false;

	auto it = m_ListString.begin();
	std::advance(it, iIndex);
	delete (*it);
	m_ListString.erase(it);

	size_t iSC = m_ListString.size();

	if (m_pScrollBarRef)
	{
		size_t iScrollPos = m_pScrollBarRef->GetCurrentPos();
		if(iScrollPos >= iSC)
			m_pScrollBarRef->SetCurrentPos(iSC - 1);
	}

	if(m_iCurSel >= iSC) m_iCurSel = iSC - 1;

	this->UpdateChildRegions();
	return true;
}

bool CN3UIList::GetString(size_t iIndex, std::string& szString)
{
	szString.clear();

	if (iIndex >= m_ListString.size())
		return false;

	auto it = m_ListString.begin();
	std::advance(it, iIndex);

	CN3UIString* pUIString = (*it);
	szString = pUIString->GetString();

	return true;
}

bool CN3UIList::SetString(size_t iIndex, const std::string& szString)
{
	if (iIndex >= m_ListString.size())
		return false;

	auto it = m_ListString.begin();
	std::advance(it, iIndex);
	
	CN3UIString* pUIString = (*it);
	pUIString->SetString(szString);

	return false;
}

CN3UIString* CN3UIList::GetChildStrFromList(std::string str)
{
	for (std::list<CN3UIString*>::iterator it = m_ListString.begin(); it != m_ListString.end(); ++it) {
		CN3UIString* pUIString = (*it);
		if (pUIString->GetString() == str) {
			return pUIString;
		}
	}

	return NULL;

}

void CN3UIList::UpdateChildRegions()
{
	RECT rc = this->GetRegion();
	RECT rcThis = rc;
	POINT pt;
	SIZE size;
	int iScrollPos = 0;
	if(m_pScrollBarRef)
	{
		m_pScrollBarRef->GetCurrentPos();
		RECT rcTmp = m_pScrollBarRef->GetRegion();
		rc.right = rcTmp.left;
	}

	it_pString it = m_ListString.begin(), itEnd = m_ListString.end();
	for(int i = 0; it != itEnd; it++, i++)
	{
		CN3UIString* pStr = *it;
		if(i < iScrollPos)
		{
			pStr->SetVisibleWithNoSound(false);
			continue;
		}

		pt = pStr->GetPos();
		pStr->GetTextExtent(pStr->GetString(), pStr->GetString().size(), &size);
		pStr->SetPos(rc.left, rc.top);
		RECT rcTmp = { rc.left, rc.top, rc.left + (rc.right - rc.left), rc.top + size.cy };
		pStr->SetRegion(rcTmp);
		rc.top += size.cy;

		if(rc.top >= rcThis.bottom) pStr->SetVisibleWithNoSound(false);
		else pStr->SetVisibleWithNoSound(true);
	}

	if(m_pScrollBarRef)
	{
		if(rc.bottom <= rcThis.bottom)
		{
			m_pScrollBarRef->SetCurrentPos(0);
			m_pScrollBarRef->SetVisibleWithNoSound(false);
		}
		else
		{
			m_pScrollBarRef->SetVisibleWithNoSound(true);
		}
		
		m_pScrollBarRef->SetRange(0, m_ListString.size());
	}
}

int CN3UIList::GetScrollPos()
{
	if(NULL == m_pScrollBarRef) return 0;
	
	return m_pScrollBarRef->GetCurrentPos();
}

bool CN3UIList::SetScrollPos(int iScrollPos)
{
	if(NULL == m_pScrollBarRef) return false;
	
	m_pScrollBarRef->SetCurrentPos(iScrollPos);
	this->UpdateChildRegions();
	return true;
}

bool CN3UIList::Load(HANDLE hFile)
{
	bool bSuccess = CN3UIBase::Load(hFile);

	
	DWORD dwNum;
	int iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			
	__ASSERT(iStrLen>0, "No font name");
	if (iStrLen>0)
	{
		m_szFontName.assign(iStrLen, ' ');
		ReadFile(hFile, &(m_szFontName[0]), iStrLen, &dwNum, NULL);				
		ReadFile(hFile, &m_dwFontHeight, 4, &dwNum, NULL);	
		ReadFile(hFile, &m_crFont, 4, &dwNum, NULL);	
		ReadFile(hFile, &m_bFontBold, 4, &dwNum, NULL);	
		ReadFile(hFile, &m_bFontItalic, 4, &dwNum, NULL);	
	}

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pUI = *itor;
		if(pUI->UIType() == UI_TYPE_SCROLLBAR)
		{
			m_pScrollBarRef = (CN3UIScrollBar*)pUI;
		}

	}

	return bSuccess;
}

uint32_t CN3UIList::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible || UI_STATE_LIST_DISABLE == m_eState) return dwRet;

	
	if(IsIn(ptCur.x, ptCur.y) && ((dwFlags & UI_MOUSE_LBCLICK) || (dwFlags & UI_MOUSE_LBDBLCLK)) )	
	{
		RECT rc = this->GetRegion(), rcStr;
		SIZE size;

		it_pString it = m_ListString.begin(), itEnd = m_ListString.end();
		for(int i = 0; it != itEnd; it++, i++)
		{
			CN3UIString* pStr = (*it);
			if(false == pStr->IsVisible()) continue;

			pStr->GetTextExtent("1", 1, &size);
			rcStr = pStr->GetRegion();

			rc.top += size.cy;
			rc.bottom += size.cy;

			if(::PtInRect(&rcStr, ptCur))
			{
				m_iCurSel = i;
				if(dwFlags & UI_MOUSE_LBCLICK)
				{
					if(m_pParent) m_pParent->ReceiveMessage(this, UIMSG_LIST_SELCHANGE); 
					dwRet |= UIMSG_LIST_SELCHANGE;
				}
				else
				{
					if(m_pParent) m_pParent->ReceiveMessage(this, UIMSG_LIST_DBLCLK); 
					dwRet |= UIMSG_LIST_DBLCLK;
				}
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UIList::Render()
{
	CN3UIBase::Render();

	if (m_iCurSel < m_ListString.size())
	{
		auto it = m_ListString.begin();
		std::advance(it, m_iCurSel);
		CN3UIString* pStr = *it;
		if(pStr)
		{
			RECT rc = pStr->GetRegion(); 
		
			__VertexTransformedColor vLines[5];
			vLines[0].Set((float)rc.left, (float)rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
			vLines[1].Set((float)rc.right, (float)rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
			vLines[2].Set((float)rc.right, (float)rc.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
			vLines[3].Set((float)rc.left, (float)rc.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
			vLines[4] = vLines[0];
	
			DWORD dwZ, dwFog, dwAlpha, dwCOP, dwCA1, dwSrcBlend, dwDestBlend, dwVertexShader, dwAOP, dwAA1;
			CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
			CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
			CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
			CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
			CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
			CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOP);
			CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
			CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAOP);
			CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAA1);
			CN3Base::s_lpD3DDev->GetFVF(&dwVertexShader);

			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

			CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
			CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vLines, sizeof(__VertexTransformedColor));
			
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
			CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwCOP);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwCA1);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAOP);
			CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAA1);
			CN3Base::s_lpD3DDev->SetFVF(dwVertexShader);
		}
	}
}

bool CN3UIList::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (UIMSG_SCROLLBAR_POS == dwMsg)
	{
		if (pSender == m_pScrollBarRef)
		{
			this->SetScrollPos(m_pScrollBarRef->GetCurrentPos());

		}
	}

	return true;
}

void CN3UIList::ResetContent()
{
	it_pString it = m_ListString.begin(), itEnd = m_ListString.end();
	for(; it != itEnd; it++)
	{
		
		CN3UIString* pString = *it;
		delete pString;		
	}
	m_ListString.clear();
	m_iCurSel = 0;
	if(m_pScrollBarRef)
	{
		m_pScrollBarRef->SetRange(0, 0);
		m_pScrollBarRef->SetCurrentPos(0);
		m_pScrollBarRef->SetVisibleWithNoSound(false);
	}
}
