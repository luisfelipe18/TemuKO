

#if !defined(AFX_N3UIPROGRESS_H__09307DB0_2F95_4205_AF1D_0262F1CE0E24__INCLUDED_)
#define AFX_N3UIPROGRESS_H__09307DB0_2F95_4205_AF1D_0262F1CE0E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

class CN3UIImage;
class CN3UIProgress : public CN3UIBase  
{
public:
	CN3UIProgress();
	virtual ~CN3UIProgress();

public:
	enum eIMAGE_TYPE{ IMAGETYPE_BKGND=0, IMAGETYPE_FRGND, NUM_IMAGETYPE};
	int				GetMax() const {return m_iMaxValue;}
	int				GetMin() const {return m_iMinValue;}
	int				GetCurValue() const {return (int)m_fCurValue;}

	void			SetStepValue(int iValue) {m_iStepValue = iValue;}
	void			StepIt() {SetCurValue((int)m_fCurValue + m_iStepValue);}
protected:
	CN3UIImage*		m_pBkGndImgRef;
	CN3UIImage*		m_pFrGndImgRef;

	__FLOAT_RECT	m_frcFrGndImgUV;				

	int				m_iMaxValue;					
	int				m_iMinValue;					
	float			m_fCurValue;					
	float			m_fChangeSpeedPerSec;			
	int				m_iValueToReach;				
	float			m_fTimeToDelay;					
	
	int				m_iStepValue;					

public:
	virtual void	Tick();
	virtual void	Render();
	virtual void	Release();
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(uint32_t dwStyle);
	virtual bool	Load(HANDLE hFile);

	void			SetMaxValue(int value) { m_iMaxValue = value; };
	void			SetCurValue(int iValue, float fTimeToDelay = 0, float fChangeSpeedPerSec = 0);
	void			SetCurValue_Smoothly(int iValue, float fPercentPerSec){SetCurValue(iValue, 0, fPercentPerSec*(m_iMaxValue-m_iMinValue));}	
	void			SetRange(int iMin, int iMax) {m_iMinValue = iMin; m_iMaxValue = iMax; UpdateFrGndImage();}
	void			SetFrGndUVFromFrGndImage();	
protected:
	void			UpdateFrGndImage();	
};

#endif 

