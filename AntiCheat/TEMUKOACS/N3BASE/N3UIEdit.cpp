#include "N3UIEdit.h"
#include "N3UIString.h"
#include "N3UIImage.h"
#include "DFont.h"
#include "N3UIStatic.h"

#include <imm.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

const float CARET_FLICKERING_TIME = 0.4f;

HWND CN3UIEdit::s_hWndEdit = NULL;
HWND CN3UIEdit::s_hWndParent = NULL;
HWND CN3UIEdit::m_previousFocus = NULL;
WNDPROC	CN3UIEdit::s_lpfnEditProc = NULL;
char CN3UIEdit::s_szBuffTmp[512] = "";

CN3UIEdit::CN3Caret::CN3Caret()
{
	m_pVB[0].Set(0,0,UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xffff0000);
	m_pVB[1].Set(0,10,UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xffff0000);
	m_bVisible = FALSE;
	m_fFlickerTimePrev = CN3Base::TimeGet();	
	m_bFlickerStatus = true;

}
CN3UIEdit::CN3Caret::~CN3Caret()
{
}
void CN3UIEdit::CN3Caret::SetPos(int x, int y)
{
	m_pVB[0].x = (float)x; m_pVB[0].y = (float)y;
	m_pVB[1].x = (float)x; m_pVB[1].y = (float)y+m_iSize;
	this->InitFlckering();
}
void CN3UIEdit::CN3Caret::MoveOffset(int iOffsetX, int iOffsetY)
{
	m_pVB[0].x += iOffsetX; m_pVB[0].y += iOffsetY; 
	m_pVB[1].x = m_pVB[0].x; m_pVB[1].y = m_pVB[0].y+m_iSize;
	this->InitFlckering();
}
void CN3UIEdit::CN3Caret::SetSize(int iSize)
{
	m_iSize = iSize;
	m_pVB[1].y = m_pVB[0].y+m_iSize;
}
void CN3UIEdit::CN3Caret::SetColor(D3DCOLOR color)
{
	m_pVB[0].color = color;
	m_pVB[1].color = color;
}
void CN3UIEdit::CN3Caret::Render(LPDIRECT3DDEVICE9	lpD3DDev)
{
	if (FALSE == m_bVisible) return;

	
	float fTime = CN3Base::TimeGet();
	if(fTime - m_fFlickerTimePrev > CARET_FLICKERING_TIME)
	{
		m_bFlickerStatus = !m_bFlickerStatus;
		m_fFlickerTimePrev = fTime;
	}
	if(!m_bFlickerStatus) return;

	__ASSERT(lpD3DDev, "DIRECT3DDEVICE8 is null");
	lpD3DDev->SetTexture(0, NULL);

	lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
	lpD3DDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, m_pVB, sizeof(m_pVB[0]));
}
void CN3UIEdit::CN3Caret::InitFlckering()
{
	m_fFlickerTimePrev = CN3Base::TimeGet();	
	m_bFlickerStatus = true;
}

BOOL CN3UIEdit::CreateEditWindow(HWND hParent, RECT rect)
{
	if(NULL == hParent) return FALSE;
	if(s_hWndEdit) return FALSE;

	s_hWndParent = hParent;
	s_hWndEdit = CreateWindowA("EDIT", "EditWindow", WS_CHILD|WS_TABSTOP|ES_LEFT|ES_WANTRETURN, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, hParent, NULL, NULL, NULL);
	s_lpfnEditProc = (WNDPROC)SetWindowLong(s_hWndEdit, GWL_WNDPROC, (uint32_t)(CN3UIEdit::EditWndProc));

	
	::SendMessage(s_hWndEdit, EM_LIMITTEXT, 0, 0);

	
	HFONT hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
	::SendMessage(s_hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	::SendMessage(s_hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	
	HDC hDC = GetDC(s_hWndEdit);
	SetBkMode(hDC, TRANSPARENT);
	SetROP2(hDC, R2_XORPEN);
	ReleaseDC(s_hWndEdit, hDC);

	return TRUE;
}

LRESULT APIENTRY CN3UIEdit::EditWndProc(HWND hWnd, uint16_t Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (s_pFocusedEdit && s_pFocusedEdit->GetParent())
			{
				s_pFocusedEdit->GetParent()->ReceiveMessage(s_pFocusedEdit, UIMSG_EDIT_ESCAPE);
				s_pFocusedEdit->KillFocus();
			}
			return 0;
		}
		if (wParam == VK_RETURN)
		{
			if(s_pFocusedEdit && s_pFocusedEdit->GetParent())
			{
				s_pFocusedEdit->GetParent()->ReceiveMessage(s_pFocusedEdit, UIMSG_EDIT_RETURN);
			}
			return 0;
		}
		if (wParam == VK_TAB) {
			if (s_pFocusedEdit && s_pFocusedEdit->GetParent())
			{
				s_pFocusedEdit->GetParent()->ReceiveMessage(s_pFocusedEdit, UIMSG_EDIT_TAB);
			}
			return 0;
		}
		(CallWindowProc(s_lpfnEditProc, hWnd, Message, wParam, lParam));
		if(s_pFocusedEdit) CN3UIEdit::UpdateCaretPosFromEditCtrl();
		return 0;
		

    case WM_CHAR:
		if(s_pFocusedEdit) CN3UIEdit::UpdateCaretPosFromEditCtrl();
		if(wParam==VK_RETURN) return 0;
		if(wParam==VK_TAB) return 0;
		break;
	case WM_INPUTLANGCHANGE:
		{
			POINT ptPos;
			ptPos.x = 0;
			ptPos.y = 0;
			SetImeStatus(ptPos,true);
		}
		break;
	case WM_IME_ENDCOMPOSITION:
		{
			POINT ptPos;
			ptPos.x = -1000;
			ptPos.y = -1000;
			SetImeStatus(ptPos,false);
		}
		break;
	case WM_IME_STARTCOMPOSITION:
		{
			POINT ptPos;
			ptPos.x = 0;
			ptPos.y = 0;
			SetImeStatus(ptPos,true);
		}
		break;
    } 

	return (CallWindowProc(s_lpfnEditProc, hWnd, Message, wParam, lParam));
}

CN3UIEdit::CN3Caret CN3UIEdit::s_Caret;

CN3UIEdit::CN3UIEdit()
{
	m_eType = UI_TYPE_EDIT;
	m_nCaretPos = 0;
	m_iCompLength = 0;
	m_iMaxStrLen = 0x7fffffff;
	KillFocus();
	m_pSnd_Typing = NULL;
	m_previousFocus = NULL;
}

CN3UIEdit::~CN3UIEdit()
{
	KillFocus();
}

void CN3UIEdit::Release()
{
	CN3UIBase::Release();
	m_nCaretPos = 0;
	m_iCompLength = 0;
	m_iMaxStrLen = 0x7fffffff;
	KillFocus();
	m_szPassword = "";
}

void CN3UIEdit::Render()
{
	if (!m_bVisible) return;

	CN3UIStatic::Render();
	if (HaveFocus())
	{
		s_Caret.Render(s_lpD3DDev);	
	}
}

void CN3UIEdit::SetVisible(bool bVisible)
{
	CN3UIBase::SetVisible(bVisible);

	if (false == bVisible && true == m_bVisible)	
	{
		KillFocus();
	}
}

void CN3UIEdit::KillFocus()
{
	if (HaveFocus())
	{
		s_pFocusedEdit = NULL;
		s_Caret.m_bVisible = FALSE;

		if(s_hWndEdit)
		{
			::SetWindowTextA(s_hWndEdit, "");
			if(m_previousFocus == NULL)
				::SetFocus(s_hWndParent);
			else
				::SetFocus(m_previousFocus);
		}
	}
}

bool CN3UIEdit::SetFocus()
{
	if (m_previousFocus == NULL)
		m_previousFocus = ::GetFocus();

	if (NULL != s_pFocusedEdit) s_pFocusedEdit->KillFocus();	
	s_pFocusedEdit = this;				

	SIZE size;
	if (m_pBuffOutRef && m_pBuffOutRef->GetTextExtent("SON",2,&size))
	{
		s_Caret.SetSize(size.cy);
		s_Caret.SetColor(m_pBuffOutRef->GetFontColor());
		
	}

	s_Caret.m_bVisible = TRUE;
	s_Caret.InitFlckering();
	
	if(s_hWndEdit)
	{
		::SetFocus(s_hWndEdit);

		RECT rcEdit = GetRegion();
		int iX		= rcEdit.left;
		int iY		= rcEdit.top;
		int iH		= rcEdit.bottom - rcEdit.top;
		int iW		= rcEdit.right - rcEdit.left;
		::MoveWindow(s_hWndEdit, iX, iY, iW, iH, false);
		
		if(UISTYLE_EDIT_PASSWORD & m_dwStyle)
		{
			::SetWindowTextA(s_hWndEdit, m_szPassword.c_str());
		}
		else
		{
			if(m_pBuffOutRef) ::SetWindowTextA(s_hWndEdit, m_pBuffOutRef->GetString().c_str());
			else ::SetWindowTextA(s_hWndEdit, "");
		}
	}

	CN3UIEdit::UpdateCaretPosFromEditCtrl();

	return true;
}

uint32_t CN3UIEdit::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;

	if (!m_bVisible) return dwRet;

	if(dwFlags & UI_MOUSE_LBCLICK && IsIn(ptCur.x, ptCur.y))
	{
		SetFocus();	
		dwRet |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_INREGION);
		return dwRet;
	} else if(dwFlags & UI_MOUSE_LBCLICK) KillFocus();

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UIEdit::SetCaretPos(UINT nPos)
{
	if (nPos > m_iMaxStrLen) nPos = m_iMaxStrLen;	
	m_nCaretPos = nPos;

	const std::string& szBuff = m_pBuffOutRef->GetString();
	__ASSERT(szBuff.empty() || -1 == szBuff.find('\n'), "multiline edit");	
	SIZE size = {0,0};
	if (!szBuff.empty() && m_pBuffOutRef ) m_pBuffOutRef->GetTextExtent(szBuff, m_nCaretPos, &size) ;

	int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	if (size.cx > iRegionWidth) size.cx = iRegionWidth;
	s_Caret.SetPos(m_pBuffOutRef->m_ptDrawPos.x + size.cx, m_pBuffOutRef->m_ptDrawPos.y);
}

void CN3UIEdit::SetMaxString(size_t iMax)		
{
	if (iMax <= 0) {__ASSERT(0, "ÃÖ´ë ±Û¾¾ ¼ö¸¦ 0º¸´Ù Å©°Ô Á¤ÇØÁÖ¼¼¿ä"); return;}
	m_iMaxStrLen = iMax;

	if (NULL == m_pBuffOutRef) return;

	const std::string szBuff = GetString();
	if ( m_iMaxStrLen >= szBuff.size()) return;

	
	SetString(szBuff);
}

BOOL CN3UIEdit::IsHangulMiddleByte( const char* lpszStr, int iPos )
{
    if( !lpszStr ) return FALSE;
    if (iPos <= 0) return FALSE;
    int nLength = lstrlenA(lpszStr);
    if (iPos >= nLength) return FALSE;
    if (!(lpszStr[iPos] & 0x80))
        return FALSE;
    register BOOL bMiddle = FALSE;
    register int i;

    for (i=0; i<iPos && i <nLength; i++)
    {
        if (lpszStr[i] & 0x80)
            bMiddle = !bMiddle;
    }
    return bMiddle;
}

const std::string& CN3UIEdit::GetString()
{
	if (UISTYLE_EDIT_PASSWORD & m_dwStyle) return m_szPassword;
	return CN3UIStatic::GetString();
}

void CN3UIEdit::SetString(const std::string& szString)
{
	if (NULL == m_pBuffOutRef) return;
	if (szString.size() > m_iMaxStrLen)
	{
		std::string szNewBuff(m_iMaxStrLen, '?');

		if (IsHangulMiddleByte(szString.c_str(), m_iMaxStrLen))
		{
			szNewBuff = szString.substr(0, m_iMaxStrLen-1);	
			if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
			{
				int iNewBuffLen = szNewBuff.size();
				m_szPassword = szNewBuff;

				szNewBuff.assign(m_iMaxStrLen-1, '*');
				__ASSERT(NULL == szNewBuff[m_iMaxStrLen-1],"±ÛÀÚ¼ö°¡ ´Ù¸£´Ù.");
			}
			m_pBuffOutRef->SetString(szNewBuff);
		}
		else
		{
			szNewBuff = szString.substr(0, m_iMaxStrLen);	
			if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
			{
				int iNewBuffLen = szNewBuff.size();
				m_szPassword = szNewBuff;

				szNewBuff.assign(m_iMaxStrLen, '*');
				__ASSERT(NULL == szNewBuff[m_iMaxStrLen],"±ÛÀÚ¼ö°¡ ´Ù¸£´Ù.");
			}
			m_pBuffOutRef->SetString(szNewBuff);
		}
	}
	else
	{
		if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
		{
			m_szPassword = szString;
			if (!szString.empty())
			{
				std::string szNewBuff(szString.size(), '*');
				m_pBuffOutRef->SetString(szNewBuff);
			}
			else m_pBuffOutRef->SetString("");
		}
		else
		{
			m_pBuffOutRef->SetString(szString);
		}
	}

	const std::string& szTempStr = m_pBuffOutRef->GetString();
	size_t iStrLen = szTempStr.size();
	if (m_nCaretPos > iStrLen) SetCaretPos(iStrLen);
}

BOOL CN3UIEdit::MoveOffset(int iOffsetX, int iOffsetY)		
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;

	if (HaveFocus()) s_Caret.MoveOffset(iOffsetX, iOffsetY);
	return TRUE;
}

bool CN3UIEdit::Load(HANDLE hFile)
{
	if (false == CN3UIStatic::Load(hFile)) return false;

	
	int iSndFNLen = 0;
	DWORD dwNum;

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_Typing, "memory leak");
	}

	return true;
}

void CN3UIEdit::UpdateTextFromEditCtrl()
{
	if(NULL == s_pFocusedEdit || NULL == s_hWndEdit) return;

	::GetWindowTextA(s_hWndEdit, s_szBuffTmp, 512);
	s_pFocusedEdit->SetString(s_szBuffTmp);
}

void CN3UIEdit::UpdateCaretPosFromEditCtrl()
{
	if(NULL == s_pFocusedEdit || NULL == s_hWndEdit) return;

	int iTmp = ::SendMessage(s_hWndEdit, EM_GETSEL, 0, 0);
	int iCaret = LOWORD(iTmp);
	int iCTmp2 = HIWORD(iTmp);
	s_pFocusedEdit->SetCaretPos(iCaret);
}

void CN3UIEdit::SetImeStatus(POINT ptPos, bool bOpen)
{
	HKL hHKL = GetKeyboardLayout(0);
	if(ImmIsIME(hHKL))
	{
		HIMC hImc = ImmGetContext(s_hWndEdit);
		if(bOpen)
		{
			SendMessage(s_hWndEdit, WM_IME_NOTIFY, IMN_OPENSTATUSWINDOW, 0);
			ImmSetStatusWindowPos(hImc,&ptPos);
		}
		else
		{
			SendMessage(s_hWndEdit, WM_IME_NOTIFY, IMN_CLOSESTATUSWINDOW, 0);
		}
		ImmReleaseContext( s_hWndEdit, hImc );
	}
}

