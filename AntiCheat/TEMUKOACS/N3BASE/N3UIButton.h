

#if !defined(AFX_N3UIBUTTON_H__7A7B3E89_9D17_45E8_8405_87877F3E6FF0__INCLUDED_)
#define AFX_N3UIBUTTON_H__7A7B3E89_9D17_45E8_8405_87877F3E6FF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"
class CN3UIImage;
class CN3SndObj;
class CN3UIButton : public CN3UIBase  
{
public:
	CN3UIButton();
	virtual ~CN3UIButton();

public:
	enum eBTN_STATE	{BS_NORMAL=0, BS_DOWN, BS_ON, BS_DISABLE, NUM_BTN_STATE};	
	void			SetClickRect(const RECT& Rect) {m_rcClick = Rect;}
	RECT			GetClickRect() const {return m_rcClick;}
	std::string 	m_szExtra;
	void  SetExtra(std::string extra) { m_szExtra = extra; }
	const std::string&  GetExtra() { return m_szExtra; }
protected:
	CN3UIImage*		m_ImageRef[NUM_BTN_STATE];	
	RECT			m_rcClick;					

	CN3SndObj*		m_pSnd_On;		
	CN3SndObj*		m_pSnd_Click;	

public:
	virtual bool	Load(HANDLE hFile);
	virtual void	Release();
	virtual void	SetRegion(const RECT& Rect);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);

	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();

public:
	virtual void	operator = (const CN3UIButton& other);
	void			SetSndOn(const std::string& strFileName);
	void			SetSndClick(const std::string& strFileName);

	std::string GetSndFName_On() const;
	std::string GetSndFName_Click() const;
};

#endif 

