

#include "N3UIImage.h"
#include "N3Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CN3UIImage::CN3UIImage()
{
	m_eType = UI_TYPE_IMAGE;

	m_pVB = NULL;
	m_pTexRef = NULL;
	m_szTexFN = "";
	m_pAnimImagesRef = NULL;

	ZeroMemory(&m_frcUVRect, sizeof(m_frcUVRect));
	m_Color = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff);
	m_fAnimFrame = 30.0f;
	m_iAnimCount = 0;
	m_fCurAnimFrame = 0.0f;
}

CN3UIImage::~CN3UIImage()
{
	if (m_pVB) { m_pVB->Release();	m_pVB = NULL; }
	s_MngTex.Delete(&m_pTexRef);
	if (m_pAnimImagesRef) { delete[] m_pAnimImagesRef; m_pAnimImagesRef = NULL; }
}

void CN3UIImage::Release()
{
	CN3UIBase::Release();
	if (m_pVB) { m_pVB->Release();	m_pVB = NULL; }
	s_MngTex.Delete(&m_pTexRef);
	m_szTexFN = "";

	ZeroMemory(&m_frcUVRect, sizeof(m_frcUVRect));
	m_Color = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff);
	m_fAnimFrame = 30.0f;
	m_iAnimCount = 0;
	m_fCurAnimFrame = 0.0f;
	if (m_pAnimImagesRef) { delete[] m_pAnimImagesRef; m_pAnimImagesRef = NULL; }
}

void CN3UIImage::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);
	if (!CreateVB())
		printf("FAIL CREATE VB\n");
}

bool CN3UIImage::CreateVB()
{
	HRESULT hr;
	if (m_pVB) { m_pVB->Release(); m_pVB = NULL; }
	hr = s_lpD3DDev->CreateVertexBuffer(4 * sizeof(__VertexTransformed), 0, FVF_TRANSFORMED, D3DPOOL_MANAGED, &m_pVB, NULL);
	return SUCCEEDED(hr);
}

void CN3UIImage::SetVB()
{
	if (UISTYLE_IMAGE_ANIMATE & m_dwStyle)	
	{
		if (m_pVB) { m_pVB->Release(); m_pVB = NULL; }
	}
	else
	{
		if (m_pVB)
		{
			__VertexTransformed* pVertices;
			m_pVB->Lock(0, 0, (void**)&pVertices, 0);

			uint32_t dwColor = 0xffffffff;
			float fRHW = 1.0f;
			
			pVertices[0].Set((float)m_rcRegion.left - 0.5f, (float)m_rcRegion.top - 0.5f, UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.left, m_frcUVRect.top);
			pVertices[1].Set((float)m_rcRegion.right - 0.5f, (float)m_rcRegion.top - 0.5f, UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.right, m_frcUVRect.top);
			pVertices[2].Set((float)m_rcRegion.right - 0.5f, (float)m_rcRegion.bottom - 0.5f, UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.right, m_frcUVRect.bottom);
			pVertices[3].Set((float)m_rcRegion.left - 0.5f, (float)m_rcRegion.bottom - 0.5f, UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.left, m_frcUVRect.bottom);

			m_pVB->Unlock();
		}
	}
}

void CN3UIImage::SetTex(const std::string& szFN)
{
	m_szTexFN = szFN;
	s_MngTex.Delete(&m_pTexRef);
	
	if (!(UISTYLE_IMAGE_ANIMATE & m_dwStyle)) m_pTexRef = s_MngTex.Get(szFN);
}

void CN3UIImage::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
	SetVB();
}

void CN3UIImage::SetUVRect(float left, float top, float right, float bottom)
{
	m_frcUVRect.left = left;	m_frcUVRect.top = top;
	m_frcUVRect.right = right;	m_frcUVRect.bottom = bottom;
	SetVB();
}

void CN3UIImage::Tick()
{
	CN3UIBase::Tick();
	if (m_iAnimCount > 0)		
	{
		m_fCurAnimFrame += (s_fSecPerFrm * m_fAnimFrame);
		while (m_fCurAnimFrame >= (float)m_iAnimCount)
		{
			m_fCurAnimFrame -= ((float)m_iAnimCount);
		}
	}
}

void CN3UIImage::Render()
{
	if (!m_bVisible) return;

	if (UISTYLE_IMAGE_ANIMATE & m_dwStyle) 
	{
		__ASSERT(m_fCurAnimFrame >= 0.0f && m_fCurAnimFrame < (float)m_iAnimCount, "animate image 가 이상작동");
		__ASSERT(m_pAnimImagesRef, "초기화 이상");
		m_pAnimImagesRef[(int)m_fCurAnimFrame]->Render();
	}
	else
	{
		if (m_pVB && m_pTexRef)
		{
			s_lpD3DDev->SetStreamSource(0, m_pVB, 0, sizeof(__VertexTransformed));
			s_lpD3DDev->SetFVF(FVF_TRANSFORMED);

			s_lpD3DDev->SetTexture(0, m_pTexRef->Get());
			s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

			s_lpD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		}

		CN3UIBase::Render();
	}
}

void CN3UIImage::RenderIconWrapper()
{
	if (!m_bVisible) return;

	if (m_pVB)
	{
		s_lpD3DDev->SetStreamSource(0, m_pVB, 0, sizeof(__VertexTransformed));
		s_lpD3DDev->SetFVF(FVF_TRANSFORMED);
		s_lpD3DDev->SetTexture(0, NULL);

		s_lpD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}

	CN3UIBase::Render();
}

BOOL CN3UIImage::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	SetVB();
	return TRUE;
}

void CN3UIImage::SetColor(D3DCOLOR color)
{
	if (m_Color == color) return;
	m_Color = color;
	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_pAnimImagesRef)
	{
		int i;
		for (i = 0; i < m_iAnimCount; ++i) m_pAnimImagesRef[i]->SetColor(color);
	}
	SetVB();
}

bool CN3UIImage::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	DWORD dwNum;
	
	__ASSERT(NULL == m_pTexRef, "load 하기 전에 초기화가 되지 않았습니다.");
	int	iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			
	char szFName[MAX_PATH] = "";
	if (iStrLen > 0)
	{
		ReadFile(hFile, szFName, iStrLen, &dwNum, NULL);		
		szFName[iStrLen] = '\0';
		this->SetTex(szFName);
	}

	ReadFile(hFile, &m_frcUVRect, sizeof(m_frcUVRect), &dwNum, NULL);	
	ReadFile(hFile, &m_fAnimFrame, sizeof(m_fAnimFrame), &dwNum, NULL);

	
	m_iAnimCount = 0; 
	for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		if (UI_TYPE_IMAGE == (*itor)->UIType()) m_iAnimCount++;
	}

	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_iAnimCount > 0)
	{
		m_pAnimImagesRef = new CN3UIImage * [m_iAnimCount];
		ZeroMemory(m_pAnimImagesRef, sizeof(CN3UIImage*) * m_iAnimCount);
		int i = 0;
		for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
		{
			if (UI_TYPE_IMAGE == (*itor)->UIType()) m_pAnimImagesRef[i] = (CN3UIImage*)(*itor);
			__ASSERT(m_pAnimImagesRef[i]->GetReserved() == (uint32_t)i, "animate Image load fail");	
			++i;
		}
	}

	SetVB();	
	return true;
}

void CN3UIImage::operator = (const CN3UIImage& other)
{
	CN3UIBase::operator = (other);

	m_Color = other.m_Color;
	m_fAnimFrame = other.m_fAnimFrame;
	m_fCurAnimFrame = other.m_fCurAnimFrame;
	m_frcUVRect = other.m_frcUVRect;
	m_iAnimCount = other.m_iAnimCount;

	if (other.m_pTexRef) m_pTexRef = s_MngTex.Get(other.m_pTexRef->FileName());
	m_szTexFN = other.m_szTexFN;

	
	m_iAnimCount = m_Children.size();	
	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_iAnimCount > 0)
	{
		m_pAnimImagesRef = new CN3UIImage * [m_iAnimCount];
		ZeroMemory(m_pAnimImagesRef, sizeof(CN3UIImage*) * m_iAnimCount);
		int i = 0;
		for (UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
		{
			__ASSERT(UI_TYPE_IMAGE == (*itor)->UIType(), "animate image child의 UI type이 image가 아니다.");
			m_pAnimImagesRef[i] = (CN3UIImage*)(*itor);
			__ASSERT(m_pAnimImagesRef[i]->GetReserved() == (uint32_t)i, "animate Image load fail");	
			++i;
		}
	}

	SetVB();	
}
