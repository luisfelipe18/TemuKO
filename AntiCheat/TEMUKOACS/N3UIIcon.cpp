// N3UIIcon.cpp: implementation of the CN3UIIcon class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "N3BASE/N3UIIcon.h"
#include "N3BASE/N3UIWndBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3UIIcon::CN3UIIcon()
{
	CN3UIImage::CN3UIImage();
	m_eType = UI_TYPE_ICON;
}

CN3UIIcon::~CN3UIIcon()
{
}

uint32_t CN3UIIcon::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;

	if (!m_bVisible) return dwRet;
	if ((m_pParent->GetState() == UI_STATE_COMMON_NONE) || (m_pParent->GetState() == UI_STATE_ICON_MOVING))
		SetStyle(GetStyle() & (~UISTYLE_ICON_HIGHLIGHT));

	RECT rect = GetRegion();
	if (::PtInRect(&rect, ptCur) && (m_pParent->GetState() == UI_STATE_COMMON_NONE))
		SetStyle(GetStyle() | UISTYLE_ICON_HIGHLIGHT);

	rect = GetMoveRect();

	if (!::PtInRect(&rect, ptCur))		// 영역 밖이면
	{
		dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
		return dwRet;
	}

	if ((dwFlags & UI_MOUSE_LBCLICK) && !(dwFlags & UI_MOUSE_RBDOWN))  // 왼쪽버튼 눌르는 순간
	{
		m_pParent->SetState(UI_STATE_ICON_MOVING);
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DOWN_FIRST); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if ((dwFlags & UI_MOUSE_LBCLICKED) && !(dwFlags & UI_MOUSE_RBDOWN)) // 왼쪽버튼을 떼는 순간
	{
		if (m_pParent && m_pParent->GetState() == UI_STATE_ICON_MOVING) // 이전 상태가 버튼을 Down 상태이면
		{
			m_pParent->SetState(UI_STATE_COMMON_NONE);
			m_pParent->ReceiveMessage(this, UIMSG_ICON_UP); // 부모에게 버튼 클릭 통지..
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}

	if ((dwFlags & UI_MOUSE_RBCLICK) && !(dwFlags & UI_MOUSE_LBDOWN))	// 오른쪽
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RDOWN_FIRST); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if ((dwFlags & UI_MOUSE_RBCLICKED) && !(dwFlags & UI_MOUSE_LBDOWN))// 오른쪽 버튼을 떼는 순간
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RUP); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if (dwFlags & UI_MOUSE_LBDOWN)	// 왼쪽
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DOWN); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}
	/*
		else
		{
			if(m_pParent && m_pParent->GetState() == UI_STATE_ICON_MOVING) // 이전 상태가 버튼을 Down 상태이면
			{
				m_pParent->ReceiveMessage(this, UIMSG_ICON_UP); // 부모에게 버튼 클릭 통지..
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
	*/

	if (dwFlags & UI_MOUSE_LBDBLCLK)	// 왼쪽 더블 클릭
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DBLCLK); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if (dwFlags & UI_MOUSE_RBDBLCLK)	// 오른쪽 더블 클릭
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RDBLCLK); // 부모에게 버튼 클릭 통지..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UIIcon::Render()
{
	if (!IsVisible()) return;

	CN3UIImage::Render();

	if (m_dwStyle & UISTYLE_ICON_HIGHLIGHT)
	{

		RECT rc = GetRegion();

		__VertexTransformedColor vLines[5];
		vLines[0].Set((float)rc.left, (float)rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
		vLines[1].Set((float)rc.right - 1, (float)rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
		vLines[2].Set((float)rc.right - 1, (float)rc.bottom - 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
		vLines[3].Set((float)rc.left, (float)rc.bottom - 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
		vLines[4] = vLines[0];

		DWORD dwZ, dwFog, dwAlpha, dwCOP, dwCA1, dwSrcBlend, dwDestBlend, dwVertexShader, dwAOP, dwAA1;
		CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
		CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
		CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
		CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
		CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOP);
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAOP);
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAA1);
		CN3Base::s_lpD3DDev->GetFVF(&dwVertexShader);

		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
		CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vLines, sizeof(__VertexTransformedColor));

		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
		CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwCOP);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwCA1);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAOP);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAA1);
		CN3Base::s_lpD3DDev->SetFVF(dwVertexShader);
	}
}

