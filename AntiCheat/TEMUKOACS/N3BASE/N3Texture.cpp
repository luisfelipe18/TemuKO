

#include "N3Texture.h"

#include "../HDRReader.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CN3Texture::CN3Texture()
{
	m_dwType |= OBJ_TEXTURE;

	memset(&m_Header, 0, sizeof(m_Header));
	m_lpTexture = NULL;
	m_iLOD = 0;
}

CN3Texture::~CN3Texture()
{
	if (m_lpTexture)
	{
		int nRefCount = m_lpTexture->Release();
		if (nRefCount == 0) m_lpTexture = NULL;
	}
}

void CN3Texture::Release()
{
	if (32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32--;
	else if (64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64--;
	else if (128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128--;
	else if (256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256--;
	else if (512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512--;
	else if (512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge--;
	else s_ResrcInfo.nTexture_Loaded_OtherSize--;

	memset(&m_Header, 0, sizeof(m_Header));
	if (m_lpTexture && m_lpTexture->Release() == 0) m_lpTexture = NULL;
	m_iLOD = 0;

	CN3BaseFileAccess::Release();
}

bool CN3Texture::Create(int nWidth, int nHeight, D3DFORMAT Format, BOOL bGenerateMipMap)
{
	if (nWidth != nHeight)

		if (nWidth <= 1 || nHeight <= 1 || D3DFMT_UNKNOWN == Format) return false;

	if (m_lpTexture != NULL) this->Release();

	if (s_dwTextureCaps & TEX_CAPS_POW2) 
	{
		int nW, nH;
		for (nW = 1; nW <= nWidth; nW *= 2); nW /= 2;
		for (nH = 1; nH <= nHeight; nH *= 2); nH /= 2;

		nWidth = nW;
		nHeight = nH;
	}
	if ((s_dwTextureCaps & TEX_CAPS_SQUAREONLY) && nWidth != nHeight) 
	{
		if (nWidth > nHeight) nWidth = nHeight;
		else nHeight = nWidth;
	}
	
	memset(&m_Header, 0, sizeof(m_Header));

	
	
	int nMMC = 1;
	if (bGenerateMipMap)
	{
		nMMC = 0;
		for (int nW = nWidth, nH = nHeight; nW >= 4 && nH >= 4; nW /= 2, nH /= 2) nMMC++;
	}

	HRESULT rval = s_lpD3DDev->CreateTexture(nWidth, nHeight, nMMC, 0, Format, D3DPOOL_MANAGED, &m_lpTexture, NULL);

#ifdef _N3GAME
	if (rval == D3DERR_INVALIDCALL)
	{
		printf("N3Texture: createtexture err D3DERR_INVALIDCALL(%s)\n", m_szFileName.c_str());
		return false;
	}
	if (rval == D3DERR_OUTOFVIDEOMEMORY)
	{
		printf("N3Texture: createtexture err D3DERR_OUTOFVIDEOMEMORY(%s)\n", m_szFileName.c_str());
		return false;
	}
	if (rval == E_OUTOFMEMORY)
	{
		printf("N3Texture: createtexture err E_OUTOFMEMORY(%s)\n", m_szFileName.c_str());
		return false;
	}
#endif
	if (NULL == m_lpTexture)
	{
		__ASSERT(m_lpTexture, "Texture pointer is NULL!");
		return false;
	}

	m_Header.nWidth = nWidth;
	m_Header.nHeight = nHeight;
	m_Header.Format = Format;
	m_Header.bMipMap = bGenerateMipMap;

	if (32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32++;
	else if (64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64++;
	else if (128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128++;
	else if (256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256++;
	else if (512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512++;
	else if (512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge++;
	else s_ResrcInfo.nTexture_Loaded_OtherSize++;

	return true;
}

bool CN3Texture::LoadFromFile(const std::string& szFileName, uint32_t iVer)
{
	bool tmp = false;

	m_iFileFormatVersion = iVer;

	if (m_lpTexture != NULL) this->Release();

	this->FileNameSet(szFileName); 
	std::string szFullPath;
	if (-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//")) 
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if (NULL != s_szPath[0]) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	int nFNL = szFullPath.size();
	if (lstrcmpiA(&szFullPath[nFNL - 3], "DXT") == 0)
	{
		HANDLE hFile = ::CreateFileA(szFullPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
#ifdef _N3GAME
			if (nFNL > 63) {
				hFile = ::CreateFileA(&szFullPath[63], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			}

			
			
			
			
			

			
			
			
			
			
#endif
		}
		this->Load(hFile);
		CloseHandle(hFile);
	}
	else
	{
		D3DXIMAGE_INFO ImgInfo;
		HRESULT rval = D3DXCreateTextureFromFileExA(s_lpD3DDev,
			szFullPath.c_str(),
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED,
			D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
			D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
			0,
			&ImgInfo,
			NULL,
			&m_lpTexture);
		if (rval == D3D_OK)
		{
			D3DSURFACE_DESC sd;
			m_lpTexture->GetLevelDesc(0, &sd);

			m_Header.nWidth = sd.Width;
			m_Header.nHeight = sd.Height;
			m_Header.Format = sd.Format;
		}
		else
		{
			printf("N3Texture - Failed to load texture(%s)\n", szFullPath.c_str());
		}

		if (32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32++;
		else if (64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64++;
		else if (128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128++;
		else if (256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256++;
		else if (512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512++;
		else if (512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge++;
		else s_ResrcInfo.nTexture_Loaded_OtherSize++;
	}

	if (NULL == m_lpTexture)
	{
		this->Release();
		return false;
	}
	return true;
}

bool CN3Texture::Load(HANDLE hFile)
{
	DWORD lastAddress = 0;

	HANDLE hProcess = GetCurrentProcess();
	SRC dxt = hdrReader->GetSRC(m_szFileName);

	unsigned char* buff = dxt.buff;

	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC = 0;
	DWORD offset = hdrReader->FindPatternEx((char*)buff, dxt.sizeInBytes, "NTF");
	lastAddress = (DWORD)(&buff[0] + offset);

	__DXT_HEADER HeaderOrg;

	ReadProcessMemory(hProcess, (LPVOID)lastAddress, &HeaderOrg, sizeof(HeaderOrg), &dwRWC);
	lastAddress += dwRWC;

	
	if ('N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3]) 
	{
#ifdef _N3GAME
		printf("N3Texture Warning - Old format DXT file (%s)\n", m_szFileName.c_str());
#endif
	}

	
	bool bDXTSupport = FALSE;
	D3DFORMAT fmtNew = HeaderOrg.Format;
	if (D3DFMT_DXT1 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT1) bDXTSupport = true;
		else fmtNew = D3DFMT_A1R5G5B5;
	}
	else if (D3DFMT_DXT2 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT2) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT3 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT3) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT4 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT4) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT5) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}

	int iWCreate = HeaderOrg.nWidth, iHCreate = HeaderOrg.nHeight;
	if (fmtNew != HeaderOrg.Format) { iWCreate /= 2; iHCreate /= 2; }
	if (m_iLOD > 0 && m_iLOD <= 2 && HeaderOrg.nWidth >= 16 && HeaderOrg.nHeight >= 16) 
	{
		for (int i = 0; i < m_iLOD; i++)
		{
			iWCreate /= 2;
			iHCreate /= 2;
		}
	}
	else m_iLOD = 0; 

	int iLODPrev = m_iLOD;
	this->Create(iWCreate, iHCreate, fmtNew, HeaderOrg.bMipMap); 
	m_iLOD = iLODPrev;

	if (m_lpTexture == NULL)
	{
#ifdef _N3GAME
		printf("N3Texture error - Can't create texture (%s)\n", m_szFileName.c_str());
#endif
		return false;
	}

	D3DSURFACE_DESC sd;
	D3DLOCKED_RECT LR;
	int iMMC = m_lpTexture->GetLevelCount(); 

	
	if (D3DFMT_DXT1 == HeaderOrg.Format ||
		D3DFMT_DXT2 == HeaderOrg.Format ||
		D3DFMT_DXT3 == HeaderOrg.Format ||
		D3DFMT_DXT4 == HeaderOrg.Format ||
		D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if (TRUE == bDXTSupport) 
		{
			if (iMMC > 1)
			{
				if (m_iLOD > 0) 
				{
					size_t iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
					{
						if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; 
						else iSkipSize += iWTmp * iHTmp; 
					}
					::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 
				}

				for (int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);
					m_lpTexture->LockRect(i, &LR, NULL, NULL);

					int iTexSize = 0;
					switch (HeaderOrg.Format) {
					case D3DFMT_DXT1: {
						iTexSize = (sd.Width * sd.Height / 2);
					} break;
					case D3DFMT_DXT2: {
						printf("ERROR: D3DFMT_DXT2\n");
						system("pause");
						exit(-1);
					} break;
					case D3DFMT_DXT3: {
						iTexSize = (sd.Width * sd.Height);
					} break;
					case D3DFMT_DXT4: {
						printf("ERROR: D3DFMT_DXT4\n");
						system("pause");
						exit(-1);
					} break;
					case D3DFMT_DXT5: {
						iTexSize = (sd.Width * sd.Height * 2);
						printf("ERROR: D3DFMT_DXT5\n");
						system("pause");
						exit(-1);
					} break;
					}

					ReadProcessMemory(hProcess, (LPVOID)lastAddress, (uint8_t*)LR.pBits, iTexSize, &dwRWC);
					lastAddress += dwRWC;
					
					m_lpTexture->UnlockRect(i);
				}

				
				size_t iWTmp = HeaderOrg.nWidth / 2, iHTmp = HeaderOrg.nHeight / 2;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2) 
					::SetFilePointer(hFile, iWTmp * iHTmp * 2, 0, FILE_CURRENT); 
			}
			else 
			{
				m_lpTexture->GetLevelDesc(0, &sd);
				m_lpTexture->LockRect(0, &LR, NULL, NULL);

				int iTexSize = 0;
				switch (HeaderOrg.Format) {
				case D3DFMT_DXT1: {
					iTexSize = (sd.Width * sd.Height / 2);
				} break;
				case D3DFMT_DXT2: {
					printf("ERROR: D3DFMT_DXT2\n");
					system("pause");
					exit(-1);
				} break;
				case D3DFMT_DXT3: {
					iTexSize = (sd.Width * sd.Height);
				} break;
				case D3DFMT_DXT4: {
					printf("ERROR: D3DFMT_DXT4\n");
					system("pause");
					exit(-1);
				} break;
				case D3DFMT_DXT5: {
					iTexSize = (sd.Width * sd.Height * 2);
					printf("ERROR: D3DFMT_DXT5\n");
					system("pause");
					exit(-1);
				} break;
				}

				ReadProcessMemory(hProcess, (LPVOID)lastAddress, (uint8_t*)LR.pBits, iTexSize, &dwRWC);
				lastAddress += dwRWC;
				
				m_lpTexture->UnlockRect(0);

				
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight / 4, 0, FILE_CURRENT); 
				if (HeaderOrg.nWidth >= 1024) SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); 
			}
		}
		else 
		{
			if (iMMC > 1) 
			{
				
				size_t iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				{
					if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; 
					else iSkipSize += iWTmp * iHTmp; 
				}
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 

				
				iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2; iSkipSize = 0;
				if (m_iLOD > 0)
				{
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
						iSkipSize += iWTmp * iHTmp * 2; 
				}

				
				for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
					iSkipSize += iWTmp * iHTmp * 2;
				if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 

				for (int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);
					m_lpTexture->LockRect(i, &LR, NULL, NULL);
					int nH = sd.Height;
					for (int y = 0; y < nH; y++) {
						ReadProcessMemory(hProcess, (LPVOID)lastAddress, (uint8_t*)LR.pBits + y * LR.Pitch, 2 * sd.Width, &dwRWC);
						lastAddress += dwRWC;
					}
						
					m_lpTexture->UnlockRect(i);
				}
			}
			else 
			{
				
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize = iWTmp * iHTmp / 2; 
				else iSkipSize = iWTmp * iHTmp; 

			}
		}
	}
	else
	{
		int iPixelSize = 0;
		if (fmtNew == D3DFMT_A1R5G5B5 || fmtNew == D3DFMT_A4R4G4B4) iPixelSize = 2;
		else if (fmtNew == D3DFMT_R8G8B8) iPixelSize = 3;
		else if (fmtNew == D3DFMT_A8R8G8B8 || fmtNew == D3DFMT_X8R8G8B8) iPixelSize = 4;
		else
		{
			__ASSERT(0, "Not supported texture format");
		}

		if (iMMC > 1)
		{
			if (m_iLOD > 0) 
			{
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2) iSkipSize += iWTmp * iHTmp * iPixelSize; 
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 
			}

			
			size_t iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
			for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
			if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 

			
			for (int i = 0; i < iMMC; i++)
			{
				m_lpTexture->GetLevelDesc(i, &sd);
				m_lpTexture->LockRect(i, &LR, NULL, NULL);
				for (int y = 0; y < (int)sd.Height; y++) {
					ReadProcessMemory(hProcess, (LPVOID)lastAddress, (uint8_t*)LR.pBits + y * LR.Pitch, iPixelSize* sd.Width, &dwRWC);
					lastAddress += dwRWC;
				}
					
				m_lpTexture->UnlockRect(i);
			}
		}
		else 
		{
			
			if (HeaderOrg.nWidth >= 512 && m_Header.nWidth <= 256)
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight * iPixelSize, 0, FILE_CURRENT); 

			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->LockRect(0, &LR, NULL, NULL);
			for (int y = 0; y < (int)sd.Height; y++) {
				ReadProcessMemory(hProcess, (LPVOID)lastAddress, (uint8_t*)LR.pBits + y * LR.Pitch, iPixelSize* sd.Width, &dwRWC);
				lastAddress += dwRWC;
			}
				
			m_lpTexture->UnlockRect(0);

			if (m_Header.nWidth >= 512 && m_Header.nHeight >= 512)
				SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); 
		}
	}
	
	return true;
}

bool CN3Texture::SkipFileHandle(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	__DXT_HEADER HeaderOrg; 
	DWORD dwRWC = 0;
	ReadFile(hFile, &HeaderOrg, sizeof(HeaderOrg), &dwRWC, NULL); 
	if ('N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3]) 
	{
#ifdef _N3GAME
		printf("N3Texture Warning - Old format DXT file (%s)\n", m_szFileName.c_str());
#endif
	}

	
	if (D3DFMT_DXT1 == HeaderOrg.Format ||
		D3DFMT_DXT2 == HeaderOrg.Format ||
		D3DFMT_DXT3 == HeaderOrg.Format ||
		D3DFMT_DXT4 == HeaderOrg.Format ||
		D3DFMT_DXT5 == HeaderOrg.Format)
	{
		int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
		if (HeaderOrg.bMipMap)
		{
			
			for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
			{
				if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2;
				else iSkipSize += iWTmp * iHTmp;
			}
			
			iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2;
			for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2) 
				iSkipSize += iWTmp * iHTmp * 2; 
		}
		else 
		{
			
			if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight / 2;
			else iSkipSize += iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight;

			
			iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight * 2;
			if (HeaderOrg.nWidth >= 1024) iSkipSize += 256 * 256 * 2; 
		}

		::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 
	}
	else
	{
		int iPixelSize = 0;
		if (HeaderOrg.Format == D3DFMT_A1R5G5B5 || HeaderOrg.Format == D3DFMT_A4R4G4B4) iPixelSize = 2;
		else if (HeaderOrg.Format == D3DFMT_R8G8B8) iPixelSize = 3;
		else if (HeaderOrg.Format == D3DFMT_A8R8G8B8 || HeaderOrg.Format == D3DFMT_X8R8G8B8) iPixelSize = 4;
		else
		{
			__ASSERT(0, "Not supported texture format");
		}

		int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
		if (HeaderOrg.bMipMap)
		{
			for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
		}
		else
		{
			iSkipSize += iWTmp * iHTmp * iPixelSize;
			if (HeaderOrg.nWidth >= 512) iSkipSize += 256 * 256 * 2; 
		}

		::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); 
	}
	return true;
}

void CN3Texture::UpdateRenderInfo()
{
}
