

#include "N3UIString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CN3UIString::CN3UIString()
{
	m_eType = UI_TYPE_STRING;
	m_pDFont = NULL;

	m_Color = 0xffffffff;
	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
	customInit = true;
	thisLineString = "";
	m_bOutline = false;
}

CN3UIString::~CN3UIString()
{
	delete m_pDFont;	m_pDFont = NULL;
}

void CN3UIString::Release()
{
	CN3UIBase::Release();

	m_szString = "";
	delete m_pDFont;	m_pDFont = NULL;
	m_Color = 0xffffffff;

	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
	m_NewLineIndices.clear();
}

void CN3UIString::Render()
{
	if (!m_bVisible) return;

	__ASSERT(m_pDFont, "m_pDFont is NULL");

	// Outline
	if (m_bOutline) {
		m_pDFont->DrawText((float)m_ptDrawPos.x - 1, (float)m_ptDrawPos.y, D3DCOLOR_RGBA(0, 0, 0, 255), 0);
		m_pDFont->DrawText((float)m_ptDrawPos.x + 1, (float)m_ptDrawPos.y, D3DCOLOR_RGBA(0, 0, 0, 255), 0);
		m_pDFont->DrawText((float)m_ptDrawPos.x, (float)m_ptDrawPos.y - 1, D3DCOLOR_RGBA(0, 0, 0, 255), 0);
		m_pDFont->DrawText((float)m_ptDrawPos.x, (float)m_ptDrawPos.y + 1, D3DCOLOR_RGBA(0, 0, 0, 255), 0);
	}

	m_pDFont->DrawText((float)m_ptDrawPos.x, (float)m_ptDrawPos.y, m_Color, 0);

	CN3UIBase::Render();
}

void CN3UIString::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);

	if (m_pDFont) delete m_pDFont; m_pDFont = NULL;
	m_pDFont = new CDFont("±¼¸²", 16);	
	m_pDFont->InitDeviceObjects(CN3Base::s_lpD3DDev);
	m_pDFont->RestoreDeviceObjects();
}

void CN3UIString::SetString(const std::string& szString)
{
	m_szString = szString;

	if (m_szString.empty()) m_pDFont->SetText("");
	else WordWrap();
}

void CN3UIString::SetStringAsInt(int iVal)
{
	char szBuff[32] = "";
	sprintf(szBuff, "%d", iVal);
	this->SetString(szBuff);
}

void CN3UIString::SetString_NoWordWrap(const std::string& szString)
{
	
	m_ptDrawPos.x = m_rcRegion.left;
	m_ptDrawPos.y = m_rcRegion.top;

	m_pDFont->SetText(szString.c_str());
}

void CN3UIString::SetFont(const std::string& szFontName, uint32_t dwHeight, BOOL bBold, BOOL bItalic)
{
	uint32_t dwFlag = 0;
	if (bBold) dwFlag |= D3DFONT_BOLD;
	if (bItalic) dwFlag |= D3DFONT_ITALIC;
	if (m_pDFont)
	{
		m_pDFont->SetFont(szFontName, dwHeight, dwFlag);
		WordWrap();
	}
}

void CN3UIString::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	WordWrap();
}

void CN3UIString::SetStyle(uint32_t dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	WordWrap();
}

BOOL CN3UIString::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	m_ptDrawPos.x += iOffsetX;	m_ptDrawPos.y += iOffsetY;
	return TRUE;
}

void CN3UIString::WordWrap()
{
	m_iLineCount = 0;

	if (NULL == m_pDFont) return;
	int iStrLen = m_szString.size();

	if (0 == iStrLen)
	{
		m_pDFont->SetText("");

		
		if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
		{
			if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER)	m_ptDrawPos.x = m_rcRegion.left + (m_rcRegion.right - m_rcRegion.left) / 2;
			else m_ptDrawPos.x = m_rcRegion.left;

			if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER)
			{
				
				SIZE sizeTmp = { 0, 0 };
				m_pDFont->GetTextExtent("SON", 2, &sizeTmp);
				m_ptDrawPos.y = m_rcRegion.top + ((m_rcRegion.bottom - m_rcRegion.top - sizeTmp.cy) / 2);
			}
			else m_ptDrawPos.y = m_rcRegion.top;
		}
		else
		{
			m_ptDrawPos.x = m_rcRegion.left;
			m_ptDrawPos.y = m_rcRegion.top;
		}

		return;
	}

	
	SIZE size;
	const std::string szString = GetString();
	iStrLen = szString.size();
	if (FALSE == m_pDFont->GetTextExtent(szString.c_str(), iStrLen, &size))
	{	
		m_pDFont->SetText(szString);
		return;
	}

	int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;

	if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
	{
		
		std::string szNewBuff;

		 if (size.cx <= iRegionWidth)	
		{
			szNewBuff = m_szString;

			
			if (m_dwStyle & UISTYLE_STRING_ALIGNRIGHT) m_ptDrawPos.x = m_rcRegion.right - size.cx;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER) m_ptDrawPos.x = m_rcRegion.left + ((iRegionWidth - size.cx) / 2);
			else m_ptDrawPos.x = m_rcRegion.left;
			
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom - size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight - size.cy) / 2);
			else m_ptDrawPos.y = m_rcRegion.top;
			m_iLineCount = 1;
		}
		else	
		{
			
			m_ptDrawPos.x = m_rcRegion.left;
			
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom - size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight - size.cy) / 2);
			else m_ptDrawPos.y = m_rcRegion.top;

			
			int iCount = 0;
			while (iCount < iStrLen)
			{
				if ('\n' == szString[iCount])		
				{
					break;
				}
				else if (0x80 & szString[iCount])	
				{
					BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount + 2, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx > iRegionWidth) break;
					iCount += 2;
				}
				else								
				{
					BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount + 1, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx > iRegionWidth) break;
					++iCount;
				}
			}
			
			szNewBuff = m_szString.substr(0, iCount);	
			m_iLineCount = 1;
		}
		m_pDFont->SetText(szNewBuff);

	}
	else
	{	
		m_ptDrawPos.x = m_rcRegion.left;
		m_ptDrawPos.y = m_rcRegion.top;
		m_NewLineIndices.clear();	
		
		int iCX = 0;
		int iCount = 0;

		BOOL bFlag = m_pDFont->GetTextExtent("Choi", 2, &size);
		__ASSERT(bFlag, "cannot get size of dfont");
		
		if (size.cy > iRegionHeight)
		{
			
		}

		m_iLineCount = 1;	
		m_NewLineIndices.push_back(0);	

		while (iCount < iStrLen)
		{
			if ('\n' == szString[iCount])		
			{				
				++iCount;
				iCX = 0;
				if (iCount < iStrLen - 1)
				{
					++m_iLineCount;	
					m_NewLineIndices.push_back(iCount);
				}
			}
			else
			{
				int iCC = 0;
				if (0x80 & szString[iCount])	iCC = 2;	
				else iCC = 1;	

				BOOL bFlag = m_pDFont->GetTextExtent(&(szString[iCount]), iCC, &size);
				__ASSERT(bFlag, "cannot get size of dfont");
				if ((iCX + size.cx) > iRegionWidth)	
				{
					
					iCX = 0;
					
					
					if (iCount < iStrLen - 1)
					{
						++m_iLineCount;	
						m_NewLineIndices.push_back(iCount);
					}
				}
				
				iCount += iCC;
				iCX += size.cx;
			}
		}
		SetStartLine(0);
	}
}

void CN3UIString::SetStartLine(int iLine)
{
	if ((m_dwStyle & UISTYLE_STRING_SINGLELINE) || iLine >= m_iLineCount) return;
	m_iStartLine = iLine;

	SIZE size = { 0,0 };
	BOOL bFlag = m_pDFont->GetTextExtent("AO", 2, &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	if (0 == size.cy) return;

	int iEndLine = m_iStartLine + ((m_rcRegion.bottom - m_rcRegion.top) / size.cy);
	bool bMoreLine = true;
	if (iEndLine >= m_iLineCount)
	{
		iEndLine = m_iLineCount;
		bMoreLine = false;
	}
	int i, iCC, iSize;
	std::string strNew;
	for (i = m_iStartLine; i < iEndLine - 1; ++i)
	{
		iCC = m_NewLineIndices[i + 1] - m_NewLineIndices[i];
		if (iCC > 0)
		{
			strNew += m_szString.substr(m_NewLineIndices[i], iCC);
			iSize = strNew.size();
			if ((iSize > 0) && ('\n' != strNew[iSize - 1])) strNew += "\n";
		}
	}
	
	if (bMoreLine)
	{
		if (iEndLine > 0) {
			iCC = m_NewLineIndices[iEndLine] - m_NewLineIndices[iEndLine - 1];
			if (iCC > 0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
		}
	}
	else
	{
		if (iEndLine > 0) {
			iCC = m_szString.size() - m_NewLineIndices[iEndLine - 1];
			if (iCC > 0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
		}
	}
	m_pDFont->SetText(strNew);
}

bool CN3UIString::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	DWORD dwNum;
	
	int iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			
	if (iStrLen > 0)
	{
		std::string szFontName(iStrLen, '?');
		ReadFile(hFile, &(szFontName[0]), iStrLen, &dwNum, NULL);				

		uint32_t dwFontFlags = 0, dwFontHeight = 0;
		ReadFile(hFile, &dwFontHeight, sizeof(dwFontHeight), &dwNum, NULL);	
		ReadFile(hFile, &dwFontFlags, sizeof(dwFontFlags), &dwNum, NULL);	

		SetFont(szFontName, dwFontHeight, dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);	
	}

	__ASSERT(iStrLen > 0, "No font name");

	ReadFile(hFile, &m_Color, sizeof(m_Color), &dwNum, NULL);			
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			
	if (iStrLen > 0)
	{
		std::string szString(iStrLen, '?');
		ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);				
		SetString(szString);
	}

	
	if (m_iFileFormatVersion >= N3FORMAT_VER_1264) {
		int iIdk0;
		ReadFile(hFile, &iIdk0, sizeof(int), &dwNum, NULL);		
	}

	return true;
}

void CN3UIString::operator = (const CN3UIString& other)
{
	CN3UIBase::operator = (other);

	m_ptDrawPos = other.m_ptDrawPos;		
	m_Color = other.m_Color;			

	
	uint32_t dwFontFlags = other.GetFontFlags();
	SetFont(other.GetFontName(), other.GetFontHeight(), dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);

	
	this->SetString(other.m_szString); 
}

int CN3UIString::GetStringRealWidth(int iNum)
{
	SIZE size;
	BOOL bFlag = m_pDFont->GetTextExtent("°¡", lstrlenA("°¡"), &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	
	return (size.cx * iNum) / 2;
}

int CN3UIString::GetStringRealWidth(std::string& szText)
{
	SIZE size;
	BOOL bFlag = m_pDFont->GetTextExtent(szText, szText.length(), &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	return size.cx;
}

void CN3UIString::SetStyle(uint32_t dwType, uint32_t dwStyleEx)
{
	if (dwType == UI_STR_TYPE_LINE)
	{
		uint32_t dwStyle = m_dwStyle;
		if (UISTYLE_STRING_SINGLELINE == dwStyleEx) dwStyle |= UISTYLE_STRING_SINGLELINE;
		else if (UISTYLE_STRING_MULTILINE == dwStyleEx) dwStyle &= (~UISTYLE_STRING_SINGLELINE);
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_HALIGN)
	{
		uint32_t dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNLEFT);
		dwStyle &= (~UISTYLE_STRING_ALIGNCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNRIGHT);
		if (UISTYLE_STRING_ALIGNLEFT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNLEFT;
		else if (UISTYLE_STRING_ALIGNCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNCENTER;
		else if (UISTYLE_STRING_ALIGNRIGHT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNRIGHT;
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_VALIGN)
	{
		uint32_t dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNTOP);
		dwStyle &= (~UISTYLE_STRING_ALIGNVCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNBOTTOM);
		if (UISTYLE_STRING_ALIGNTOP == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNTOP;
		else if (UISTYLE_STRING_ALIGNVCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNVCENTER;
		else if (UISTYLE_STRING_ALIGNBOTTOM == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNBOTTOM;
		SetStyle(dwStyle);
	}
}

uint32_t CN3UIString::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if (IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBCLICKED)){
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LCLICK); 
	}

	if (IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBDBLCLK)){
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LDCLICK); 
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}
