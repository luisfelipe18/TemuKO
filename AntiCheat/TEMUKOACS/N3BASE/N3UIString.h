

#if !defined(AFX_N3UISTRING_H__E3559B01_72AE_4651_804D_B96D22738ED8__INCLUDED_)
#define AFX_N3UISTRING_H__E3559B01_72AE_4651_804D_B96D22738ED8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <string>
#include <vector>
#include "N3UIBase.h"
#include "DFont.h"
#include "../CustomStringList.h"

class CDFont;
class CN3UIString : public CN3UIBase
{
	friend class CN3UIEdit;

protected:
	CDFont* m_pDFont;			
	POINT			m_ptDrawPos;		
	std::string 	m_szString;			
	std::string 	m_szExtra;
	std::string		m_szExtra2;
	D3DCOLOR		m_Color;			
	int				m_iLineCount;		
	std::vector<int>	m_NewLineIndices;	
	int				m_iStartLine;		

public:
	CN3UIString();
	virtual ~CN3UIString();

	
public:
	void				SetColor(D3DCOLOR color) { m_Color = color; }
	D3DCOLOR			GetColor() const { return m_Color; }
	const std::string& GetString() { return m_szString; }
	const std::string& GetExtra() { return m_szExtra; }
	const std::string& GetExtra2() { return m_szExtra2; }
	int					GetLineCount() const { return m_iLineCount; }
	int					GetStartLine() const { return m_iStartLine; }
	int					GetStringRealWidth(int iNum);
	int					GetStringRealWidth(std::string& szText);

	virtual	uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();
	virtual void	Release();
	virtual void	Init(CN3UIBase* pParent);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);
	virtual bool	Load(HANDLE hFile);
	void			ClearOnlyStringBuffer() { m_szString = ""; }	
	void			SetStartLine(int iLine);	

	virtual void	operator = (const CN3UIString& other);
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(uint32_t dwStyle);
	virtual void	SetStyle(uint32_t dwType, uint32_t dwStyleEx);

	CustomStringList customStrings;
	std::string thisLineString;
	bool customInit;
	bool m_bOutline;

	void			ClearCustomThings() { customStrings.clear(); };
	virtual void	SetString(const std::string& szString);
	virtual void	SetStringAsInt(int iVal);
	void			SetString_NoWordWrap(const std::string& szString);	
	virtual void	SetFont(const std::string& szFontName, uint32_t dwHeight, BOOL bBold, BOOL bItalic); 
	BOOL			GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize)
	{
		if (m_pDFont) return m_pDFont->GetTextExtent(szString, iStrLen, pSize);
		return FALSE;
	}
	uint32_t				GetFontColor() const { if (m_pDFont) return m_pDFont->GetFontColor(); return 0xffffffff; }
	const std::string& GetFontName() const { if (m_pDFont) return m_pDFont->GetFontName(); return s_szStringTmp; }
	uint32_t				GetFontHeight() const { if (m_pDFont) return m_pDFont->GetFontHeight(); return 0; }
	uint32_t				GetFontFlags() const { if (m_pDFont) return m_pDFont->GetFontFlags(); return 0; }
protected:
	void				WordWrap();		
};

#endif 
