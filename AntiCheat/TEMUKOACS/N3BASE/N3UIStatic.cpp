

#include "N3UIStatic.h"
#include "N3UIString.h"
#include "N3UIImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIStatic::CN3UIStatic()
{
	m_eType = UI_TYPE_STATIC;

	m_pBuffOutRef = NULL;
	m_pImageBkGnd = NULL;
	m_pSnd_Click = NULL;
}

CN3UIStatic::~CN3UIStatic()
{
}

void CN3UIStatic::Release()
{
	CN3UIBase::Release();
	m_pBuffOutRef = NULL;
	m_pImageBkGnd = NULL;
}

void CN3UIStatic::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

bool CN3UIStatic::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE == pChild->UIType())
		{
			m_pImageBkGnd = (CN3UIImage*)pChild;
		}
		else if (UI_TYPE_STRING == pChild->UIType())
		{
			m_pBuffOutRef = (CN3UIString*)pChild;
		}
	}
	
	
	int iSndFNLen = 0;
	DWORD dwNum;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_Click, "memory leak");
	}

	return true;
}

const std::string& CN3UIStatic::GetString()
{
	if (m_pBuffOutRef) return m_pBuffOutRef->GetString();
	else return s_szStringTmp;
}

void CN3UIStatic::SetString(const std::string& szString)
{
	if (m_pBuffOutRef) m_pBuffOutRef->SetString(szString);
}

uint32_t CN3UIStatic::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if((dwFlags & UI_MOUSE_LBCLICK) && IsIn(ptCur.x, ptCur.y))  
	{

		if(m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); 
		dwRet |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_INREGION);
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}
