

#include "N3UIImage.h"
#include "N3UIProgress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIProgress::CN3UIProgress()
{
	m_eType = UI_TYPE_PROGRESS;

	m_pBkGndImgRef = NULL;
	m_pFrGndImgRef = NULL;
	ZeroMemory(&m_frcFrGndImgUV, sizeof(m_frcFrGndImgUV));
	m_iMaxValue = m_iMinValue = 0;
	m_iValueToReach = 0;
	m_fCurValue = 0;
	m_fChangeSpeedPerSec = 0;
	m_fTimeToDelay = 0;					
}

CN3UIProgress::~CN3UIProgress()
{
}

void CN3UIProgress::Release()
{
	CN3UIBase::Release();

	m_pBkGndImgRef = NULL;
	m_pFrGndImgRef = NULL;
	ZeroMemory(&m_frcFrGndImgUV, sizeof(m_frcFrGndImgUV));
	m_iMaxValue = m_iMinValue = 0;
	m_fCurValue = 0;
	m_fChangeSpeedPerSec = 0;
}

void CN3UIProgress::Render()
{
	if(!m_bVisible) return;
	
	CN3UIBase::Render();

	if(m_pBkGndImgRef) m_pBkGndImgRef->Render();
	if(m_pFrGndImgRef) m_pFrGndImgRef->Render();
}

void CN3UIProgress::SetCurValue(int iValue, float fTimeToDelay, float fChangeSpeedPerSec)
{
	if (iValue<m_iMinValue) iValue = m_iMinValue;
	if (iValue>m_iMaxValue) iValue = m_iMaxValue;
	if (fTimeToDelay < 0.0f) fTimeToDelay = 0.0f;
	if (fChangeSpeedPerSec < 0.0f) fChangeSpeedPerSec = 0.0f;

	if (m_iValueToReach == iValue) return;
	
	m_iValueToReach = iValue;
	m_fTimeToDelay = fTimeToDelay; 
	m_fChangeSpeedPerSec = fChangeSpeedPerSec;
	
	if(0.0f == fTimeToDelay  && 0.0f == fChangeSpeedPerSec) 
	{
		m_fCurValue = (float)iValue;
		UpdateFrGndImage();
	}
}

void CN3UIProgress::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	if (m_pBkGndImgRef) m_pBkGndImgRef->SetRegion(Rect);
	UpdateFrGndImage();
}

void CN3UIProgress::SetStyle(uint32_t dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	UpdateFrGndImage();
}

void CN3UIProgress::UpdateFrGndImage()
{
	if (NULL == m_pFrGndImgRef) return;
	int iDiff = m_iMaxValue - m_iMinValue;
	if (0 == iDiff) return;
	const float fPercentage = ((float)(m_fCurValue - m_iMinValue)) / ((float)(m_iMaxValue - m_iMinValue));

	RECT			rcRegion;
	__FLOAT_RECT	frcUVRect;
	if (m_dwStyle & UISTYLE_PROGRESS_RIGHT2LEFT)
	{
		rcRegion.right = m_rcRegion.right;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.left = m_rcRegion.right - ((int)((m_rcRegion.right - m_rcRegion.left)*fPercentage));

		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.left = m_frcFrGndImgUV.right - (m_frcFrGndImgUV.right - m_frcFrGndImgUV.left) *
			((rcRegion.right-rcRegion.left)/((float)(m_rcRegion.right - m_rcRegion.left)));
	}
	else if (m_dwStyle & UISTYLE_PROGRESS_TOP2BOTTOM)
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.right = m_rcRegion.right;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.top + ((int)((m_rcRegion.bottom - m_rcRegion.top)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.top + (m_frcFrGndImgUV.bottom - m_frcFrGndImgUV.top) *
			((rcRegion.bottom-rcRegion.top)/((float)(m_rcRegion.bottom - m_rcRegion.top)));
	}
	else if (m_dwStyle & UISTYLE_PROGRESS_BOTTOM2TOP)
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.right = m_rcRegion.right;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.top = m_rcRegion.bottom - ((int)((m_rcRegion.bottom - m_rcRegion.top)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.top = m_frcFrGndImgUV.bottom - (m_frcFrGndImgUV.bottom - m_frcFrGndImgUV.top) *
			((rcRegion.bottom-rcRegion.top)/((float)(m_rcRegion.bottom - m_rcRegion.top)));
	}
	else
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.right = m_rcRegion.left + ((int)((m_rcRegion.right - m_rcRegion.left)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.right = m_frcFrGndImgUV.left + (m_frcFrGndImgUV.right - m_frcFrGndImgUV.left)*
			((rcRegion.right-rcRegion.left)/((float)(m_rcRegion.right - m_rcRegion.left)));
	}

	m_pFrGndImgRef->SetRegion(rcRegion);
	m_pFrGndImgRef->SetUVRect(frcUVRect.left, frcUVRect.top, frcUVRect.right, frcUVRect.bottom);
}

void CN3UIProgress::SetFrGndUVFromFrGndImage()

{
	if (NULL == m_pFrGndImgRef) return;
	m_frcFrGndImgUV = *(m_pFrGndImgRef->GetUVRect());
	UpdateFrGndImage();
}

bool CN3UIProgress::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	
		int iImageType = (int)(pChild->GetReserved());
		if (iImageType == IMAGETYPE_BKGND)
		{
			m_pBkGndImgRef = (CN3UIImage*)pChild;
		}
		else if (iImageType == IMAGETYPE_FRGND)
		{
			m_pFrGndImgRef = (CN3UIImage*)pChild;
		}
		SetFrGndUVFromFrGndImage();
	}	
	return true;
}

void CN3UIProgress::Tick()
{
	CN3UIBase::Tick();

	if(m_fTimeToDelay > 0)
	{
		m_fTimeToDelay -= s_fSecPerFrm; 
		if(m_fTimeToDelay < 0) m_fTimeToDelay = 0;
		return;
	}

	if(m_fChangeSpeedPerSec <= 0) return;
	if((int)m_fCurValue == m_iValueToReach) return;
	
	if(m_fCurValue < m_iValueToReach)
	{
		m_fCurValue += m_fChangeSpeedPerSec * s_fSecPerFrm; 
		if(m_fCurValue > m_iValueToReach) m_fCurValue = (float)m_iValueToReach;
		UpdateFrGndImage();
	}
	else if(m_fCurValue > m_iValueToReach)
	{
		m_fCurValue -= m_fChangeSpeedPerSec * s_fSecPerFrm; 
		if(m_fCurValue < m_iValueToReach) m_fCurValue = (float)m_iValueToReach;
		UpdateFrGndImage();
	}
}
