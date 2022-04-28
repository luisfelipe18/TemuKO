

#include "N3AnimKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3AnimKey::CN3AnimKey()
{
	m_eType = KEY_VECTOR3;

	m_nCount = 0; 
	m_fSamplingRate = 30.0;
	m_pDatas = NULL;
}

CN3AnimKey::~CN3AnimKey()
{
	delete [] m_pDatas; m_pDatas = NULL;
}

void CN3AnimKey::Release()
{
	m_nCount = 0; 
	m_fSamplingRate = 30.0;
	delete [] m_pDatas; m_pDatas = NULL;

	CN3Base::Release();
}

void CN3AnimKey::Alloc(int nCount, float fSamplingRate, ANIMATION_KEY_TYPE eType)
{
	if(nCount <= 0) return;
	if(KEY_VECTOR3 != eType && KEY_QUATERNION != eType) return;
	if(fSamplingRate <= 0.0f)
	{
		__ASSERT(0, "Animation Sampling Rate 는 반드시 0 보다 커야 합니다.");
		return;
	}

	if(m_pDatas != NULL)
	{
		delete [] m_pDatas; m_pDatas = NULL;
		m_nCount = 0;
	}

	m_eType = eType;
	m_nCount = nCount;
	m_fSamplingRate = fSamplingRate;

	if(KEY_VECTOR3 == m_eType)
	{
		m_pDatas = new __Vector3[nCount+1]; 
		memset(m_pDatas, 0, sizeof(__Vector3) * (nCount+1));
	}
	else if(KEY_QUATERNION == m_eType)
	{
		m_pDatas = new __Quaternion[nCount+1]; 
		memset(m_pDatas, 0, sizeof(__Quaternion) * (nCount+1));
	}
}

bool CN3AnimKey::Load(HANDLE hFile)
{
	if(m_nCount > 0) this->Release();

	DWORD dwRWC = 0;
	ReadFile(hFile, &m_nCount, 4, &dwRWC, NULL); 

	
	if(m_nCount > 0)
	{
		ReadFile(hFile, &m_eType, 4, &dwRWC, NULL); 
		ReadFile(hFile, &m_fSamplingRate, 4, &dwRWC, NULL); 

		this->Alloc(m_nCount, m_fSamplingRate, m_eType);
		if(KEY_VECTOR3 == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Vector3)*m_nCount, &dwRWC, NULL);
			__Vector3* pKeys = (__Vector3*)m_pDatas; 
			pKeys[m_nCount] = pKeys[m_nCount-1];
		}
		else if(KEY_QUATERNION == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Quaternion)*m_nCount, &dwRWC, NULL);
			__Quaternion* pKeys = (__Quaternion*)m_pDatas; 
			pKeys[m_nCount] = pKeys[m_nCount-1];
		}
	}

	return true;
}

void CN3AnimKey::Duplicate(CN3AnimKey* pSrc)
{
	if(m_nCount > 0) this->Release();

	m_nCount = pSrc->Count();

	
	if(m_nCount > 0)
	{
		m_eType = pSrc->Type();
		m_fSamplingRate = pSrc->SamplingRate();

		this->Alloc(m_nCount, m_fSamplingRate, m_eType);

		if(KEY_VECTOR3 == m_eType)
		{
			char* pData = (char*)pSrc->GetDatas();
			memcpy(m_pDatas, pData, sizeof(__Vector3)*m_nCount);
		}
		else if(KEY_QUATERNION == m_eType)
		{
			char* pData = (char*)pSrc->GetDatas();
			memcpy(m_pDatas, pData, sizeof(__Quaternion)*m_nCount);
		}
	}	
}

void CN3AnimKey::MultiplyDataVector(__Vector3 vM)
{
	if(KEY_VECTOR3 != m_eType) return;

	for(int i=0;i<m_nCount;i++)
	{
		__Vector3 vT = ((__Vector3*)m_pDatas)[i];
		vT*=vM;
		((__Vector3*)m_pDatas)[i] = vT;
	}
}
