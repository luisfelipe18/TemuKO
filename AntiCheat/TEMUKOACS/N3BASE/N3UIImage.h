

#if !defined(AFX_N3UIIMAGE_H__80AE123C_5A12_4A0E_9CF0_511ED98BB896__INCLUDED_)
#define AFX_N3UIIMAGE_H__80AE123C_5A12_4A0E_9CF0_511ED98BB896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIBase.h"

class N3Texture;
class CN3UIImage : public CN3UIBase  
{
public:
	CN3UIImage();
	virtual ~CN3UIImage();

public:
	__FLOAT_RECT*			GetUVRect() {return &m_frcUVRect;}
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;			
	CN3Texture*				m_pTexRef;		
	std::string				m_szTexFN;		
	__FLOAT_RECT			m_frcUVRect;	
	D3DCOLOR				m_Color;		

	float					m_fAnimFrame;	
	int						m_iAnimCount;	
	float					m_fCurAnimFrame;	
	CN3UIImage**			m_pAnimImagesRef;	

public:
	D3DCOLOR	GetColor()	{ return m_Color; }

	CN3Texture*		GetTex() const { return m_pTexRef; }	
	void			SetTex(const std::string& szFN);		
	void			SetUVRect(float left, float top, float right, float bottom);	
	void			SetColor(D3DCOLOR color);				

	virtual void	SetRegion(const RECT& Rect);					
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);
	virtual void	Release();								
	virtual void	Tick();									
	virtual void	Render();								
	virtual void	RenderIconWrapper();
	virtual void	Init(CN3UIBase* pParent);				
	virtual bool	Load(HANDLE hFile);

	virtual void	operator = (const CN3UIImage& other);

protected:
	bool			CreateVB();								
	virtual void	SetVB();								
};

#endif 
