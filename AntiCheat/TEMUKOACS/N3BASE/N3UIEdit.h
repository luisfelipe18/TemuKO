

#if !defined(AFX_N3UIEDIT_H__91BCC181_3AA5_4CD4_8D33_06D5D96F4F26__INCLUDED_)
#define AFX_N3UIEDIT_H__91BCC181_3AA5_4CD4_8D33_06D5D96F4F26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIStatic.h"

class CN3UIEdit : public CN3UIStatic  
{

public:
	CN3UIEdit();
	virtual ~CN3UIEdit();

protected:
	class CN3Caret
	{
	public:
		CN3Caret();
		virtual ~CN3Caret();
		void	SetPos(int x, int y);
		void	MoveOffset(int iOffsetX, int iOffsetY);
		void	SetSize(int iSize);
		void	SetColor(D3DCOLOR color);
		void	Render(LPDIRECT3DDEVICE9 lpD3DDev);
		void	InitFlckering();		
		BOOL	m_bVisible;
	protected:
		int		m_iSize;				
		float	m_fFlickerTimePrev;	
		bool	m_bFlickerStatus;

		__VertexTransformedColor	m_pVB[2];	
	};

public:

	static HWND		s_hWndEdit, s_hWndParent, m_previousFocus;
	static WNDPROC	s_lpfnEditProc;
	static char		s_szBuffTmp[512];

	static void				SetImeStatus(POINT ptPos, bool bOpen);
	static BOOL				CreateEditWindow(HWND hParent, RECT rect);
	static LRESULT APIENTRY EditWndProc(HWND hWnd, uint16_t Message, WPARAM wParam, LPARAM lParam);
	static void				UpdateTextFromEditCtrl();
	static void				UpdateCaretPosFromEditCtrl();

protected:
	static CN3Caret		s_Caret;
	UINT				m_nCaretPos;		
	int					m_iCompLength;		
	size_t				m_iMaxStrLen;		
	std::string			m_szPassword;		

	CN3SndObj*			m_pSnd_Typing;		

public:
	virtual const std::string&	GetString();
	virtual void				SetString(const std::string& szString);

	virtual bool		Load(HANDLE hFile);
	virtual void		Render();
	virtual void		Release();
	virtual void		SetVisible(bool bVisible);
	virtual uint32_t		MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual BOOL		MoveOffset(int iOffsetX, int iOffsetY);		
	void				KillFocus();			
	bool				SetFocus();				
	bool				HaveFocus() const {return (this == s_pFocusedEdit);}
	void				SetCaretPos(UINT nPos);	
	void				SetMaxString(size_t iMax);		
protected:
	BOOL				IsHangulMiddleByte( const char* lpszStr, int iPos );	
};

#endif 
