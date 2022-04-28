

#if !defined(AFX_N3UI_H__5F1578F8_8476_4647_8C53_E22A5F6184FE__INCLUDED_)
#define AFX_N3UI_H__5F1578F8_8476_4647_8C53_E22A5F6184FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#pragma warning(disable : 4786)

#include "N3BaseFileAccess.h"
#include "N3UIDef.h"
#include <list>
#include <set>
#include <string>

class CN3UIBase;

typedef std::list<CN3UIBase*>			UIList;
typedef UIList::iterator				UIListItor;
typedef UIList::const_iterator			UIListItorConst;
typedef UIList::reverse_iterator		UIListReverseItor;

class CN3UIImage;
class CN3UIString;
class CN3UIButton;
class CN3UIStatic;
class CN3UIProgress;
class CN3UITrackBar;
class CN3UIScrollBar;
class CN3UIEdit;
class CN3UITooltip;
class CN3UIArea;
class CN3SndObj;
class CN3UIList;

class CN3UIBase : public CN3BaseFileAccess
{
public:
	std::string m_szID;			
	std::string	m_szToolTip;	
	void		SetID(LPSTR pszID) {m_szID = pszID;}
	const std::string GetID() const {return m_szID;}
	void		SetTooltipText(LPSTR pszTT) {m_szToolTip = pszTT;}

	static CN3UITooltip*	s_pTooltipCtrl;		
	
	CN3UIBase*	m_pChildUI;		
	CN3UIBase*	m_pParentUI;
	bool m_bNeedToRestore;
	bool m_bIsThisFocused;

protected:
	static std::string	s_szStringTmp;		

	int			m_iChildID;
	CN3UIBase*	m_pParent;		
	UIList		m_Children;		
	eUI_TYPE	m_eType;		
	eUI_STATE	m_eState;		
	uint32_t		m_dwStyle;		
	uint32_t		m_dwReserved;	

	RECT		m_rcRegion;		
	RECT		m_rcMovable;	

	bool		m_bVisible;		
	CN3SndObj*	m_pSnd_OpenUI;	
	CN3SndObj*	m_pSnd_CloseUI;	

	static CN3UIEdit*		s_pFocusedEdit;		
	
public:
	CN3UIBase();
	virtual ~CN3UIBase();

public:
	void			PrintChildIDs(void);
	eUI_TYPE		UIType() const { return m_eType; }
	eUI_STATE 		GetState() const { return m_eState; }
	bool			IsVisible()	const { return m_bVisible; }
	RECT			GetRegion() const { return m_rcRegion; }
	void			SetMoveRect(const RECT& Rect) { m_rcMovable = Rect; }
	RECT			GetMoveRect() { return m_rcMovable; }
	void			SetReserved(uint32_t dwReserved) {m_dwReserved = dwReserved;}
	uint32_t			GetReserved() const {return m_dwReserved;}
	CN3UIBase*		GetParent() const {return m_pParent;}
	static CN3UIEdit*	GetFocusedEdit() {return s_pFocusedEdit;}
	static CN3UITooltip*	GetTooltipCtrl() {return s_pTooltipCtrl;}
	uint32_t			GetStyle()	{return m_dwStyle;}

	void			SetUIType(eUI_TYPE eUIType) { m_eType = eUIType; }	

public:	
	bool			IsIn(int x, int y);
	void			AddChild(CN3UIBase* pChild) { m_Children.push_front(pChild); }
	void			RemoveChild(CN3UIBase* pChild); 
	void			SetParent(CN3UIBase* pParent);	
	int				GetWidth() { return m_rcRegion.right - m_rcRegion.left; }
	int				GetHeight() { return m_rcRegion.bottom - m_rcRegion.top; }
	POINT			GetPos() const;
	virtual void	SetPos(int x, int y);	
	void			SetPosCenter();	
	CN3UIBase*		GetChildByID(const std::string& szID);
	UIList			GetChildren() { return m_Children; }

	virtual void	SetRegion(const RECT& pRect) { m_rcRegion = pRect; }	
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);	
	virtual void	SetSize(int iWidth, int iHeight);	
	virtual void	SetState(eUI_STATE eState) { m_eState = eState; }
	virtual void	SetStyle(uint32_t dwStyle) {m_dwStyle = dwStyle;}	
	virtual void	SetVisible(bool bVisible);

	virtual void	SetVisibleWithNoSound(bool bVisible, bool bWork = false, bool bReFocus = false);

	virtual void	CallBackProc(int iID, uint32_t dwFlag);
	virtual void	ShowWindow(int iID = -1, CN3UIBase* pParent = NULL);
	virtual bool	Load(HANDLE hFile);
	virtual bool	ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg); 
	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Tick();
	virtual void	Render();
	virtual void	Release(); 
	virtual void	Init(CN3UIBase* pParent);
	virtual bool	OnKeyPress(int iKey) { return false; }
	virtual bool	OnKeyPressed(int iKey) { return false; }

	static	bool	EnableTooltip(const std::string& szFN);	
	static	void	DestroyTooltip();	

	int				GetChildrenCount() { return m_Children.size(); }
	CN3UIBase*		GetChildByIndex(size_t iIndex)
	{
		if (iIndex >= m_Children.size()) return NULL;
		auto it = m_Children.begin();
		std::advance(it, iIndex);
		return *it;
	}
	virtual void	operator = (const CN3UIBase& other);
};

#endif 
