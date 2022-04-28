

#include "N3Joint.h"
#include "N3Mesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3Joint::CN3Joint()
{
	m_dwType |= OBJ_JOINT;
	
	m_qOrient.Identity();		
	m_pParent = NULL;
}

CN3Joint::~CN3Joint()
{
	for(it_Joint it = m_Children.begin(), itEnd = m_Children.end(); it != itEnd; it++)
	{
		delete *it;
	}
	m_Children.clear();
}

void CN3Joint::Release()
{
	m_qOrient.Identity();		
	m_KeyOrient.Release();		
	
	for(it_Joint it = m_Children.begin(), itEnd = m_Children.end(); it != itEnd; it++)
	{
		delete *it;
	}
	m_Children.clear();
	m_pParent = NULL;

	CN3Transform::Release();
}

bool CN3Joint::Load(HANDLE hFile)
{
	CN3Transform::Load(hFile);

	DWORD dwRWC = 0;
	int nL = 0;

	m_KeyOrient.Load(hFile); 

	int nCC = 0;
	ReadFile(hFile, &nCC, 4, &dwRWC, NULL);
	for(int i = 0; i < nCC; i++)
	{
		CN3Joint* pChild = new CN3Joint();
		this->ChildAdd(pChild);

		pChild->Load(hFile);
	}

	return true;
}

void CN3Joint::ChildAdd(CN3Joint *pChild)
{
	__ASSERT(pChild, "Child joint pointer is NULL!");

	CN3Joint* pChild2 = NULL;
	it_Joint it = m_Children.begin();
	int iSize = m_Children.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pChild2 = *it;
		if(pChild2 == pChild) return;
	}

	m_Children.push_back(pChild);
	pChild->ParentSet(this);
}

void CN3Joint::ChildDelete(CN3Joint *pChild)
{
	if(NULL == pChild) return;
	it_Joint it = m_Children.begin(), itEnd = m_Children.end();
	int iSize = m_Children.size();
	for(; it != itEnd; )
	{
		if(*it == pChild) 
		{
			it = m_Children.erase(it);
			pChild->ParentSet(NULL);
			break;
		}
		else
		{
			it++;
		}
	}
}

void CN3Joint::ParentSet(CN3Joint *pParent)
{
	if(pParent == m_pParent) return;

	m_pParent = pParent;
	if(pParent) pParent->ChildAdd(this);

	m_iFileFormatVersion = pParent->m_iFileFormatVersion;
}

void CN3Joint::NodeCount(int &nCount)
{
	nCount++;
	
	CN3Joint* pChild = NULL;
	it_Joint it = m_Children.begin();
	int iSize = m_Children.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pChild = *it;

		__ASSERT(pChild, "Child joint pointer is NULL!");
		pChild->NodeCount(nCount);
	}
}

BOOL CN3Joint::FindPointerByID(int nID, CN3Joint *&pJoint)
{
	static int stnID = 0;
	if(pJoint == NULL) stnID = 0;

	pJoint = this;
	if(nID == stnID) return TRUE;
	stnID++;

	CN3Joint* pChild = NULL;
	it_Joint it = m_Children.begin();
	int iSize = m_Children.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pChild = *it;

		__ASSERT(pChild, "Child joint pointer is NULL!");
		if(TRUE == pChild->FindPointerByID(nID, pJoint)) return TRUE;
	}
	
	return FALSE;
}

void CN3Joint::Tick(float fFrm)
{
	m_KeyPos.DataGet(fFrm, m_vPos);
	m_KeyRot.DataGet(fFrm, m_qRot);
	m_KeyScale.DataGet(fFrm, m_vScale);
	m_KeyOrient.DataGet(fFrm, m_qOrient);

	CN3Joint::ReCalcMatrix();

	CN3Joint* pChild = NULL;
	it_Joint it = m_Children.begin();
	int iSize = m_Children.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pChild = *it;

		__ASSERT(pChild, "Child joint pointer is NULL!");
		pChild->Tick(fFrm);
	}
}

bool CN3Joint::TickAnimationKey(float fFrm)
{
	bool bNeedReCalcMatrix = CN3Transform::TickAnimationKey(fFrm);
	if(m_KeyOrient.DataGet(fFrm, m_qOrient)) bNeedReCalcMatrix = true;

	return bNeedReCalcMatrix;
}

void CN3Joint::ReCalcMatrix()
{
	static __Matrix44 mtx;
	static __Quaternion qt;

	if(m_KeyOrient.Count() > 0) 
	{
		qt = m_qRot * m_qOrient;
		::D3DXMatrixRotationQuaternion(&m_Matrix, &qt); 
	}
	else
	{
		::D3DXMatrixRotationQuaternion(&m_Matrix, &m_qRot); 
	}

	if(1.0f != m_vScale.x || 1.0f != m_vScale.y || 1.0f != m_vScale.z) 
	{
		mtx.Scale(m_vScale); 
		m_Matrix *= mtx;
	}
	m_Matrix.PosSet(m_vPos);

	if(m_pParent) m_Matrix *= m_pParent->m_Matrix; 
}

void CN3Joint::MatricesGet(__Matrix44 *pMtxs, int &nJointIndex)
{
	memcpy(&(pMtxs[nJointIndex]), &m_Matrix, sizeof(__Matrix44));
	nJointIndex++;

	CN3Joint* pChild = NULL;
	it_Joint it = m_Children.begin();
	int iSize = m_Children.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pChild = *it;
		pChild->MatricesGet(pMtxs, nJointIndex);
	}
}

void CN3Joint::ReCalcMatrixBlended(float fFrm0, float fFrm1, float fWeight0)
{

	static __Vector3 v1, v2;
	static __Quaternion qt1, qt2;
	int nKC = 0, nFrm0 = (int)fFrm0, nFrm1 = (int)fFrm1;
	float fWeight1 = 1.0f - fWeight0;

	bool bHaveKey1 = m_KeyPos.DataGet(fFrm0, v1);
	bool bHaveKey2 = m_KeyPos.DataGet(fFrm1, v2);
	if(bHaveKey1 && bHaveKey2) 
		m_vPos = (v1 * fWeight0) + (v2 * fWeight1);

	bHaveKey1 = m_KeyRot.DataGet(fFrm0, qt1);
	bHaveKey2 = m_KeyRot.DataGet(fFrm1, qt2);
	if(bHaveKey1 && bHaveKey2) 
		m_qRot.Slerp(qt1, qt2, fWeight1);

	bHaveKey1 = m_KeyScale.DataGet(fFrm0, v1);
	bHaveKey1 = m_KeyScale.DataGet(fFrm1, v2);
	if(bHaveKey1 && bHaveKey2) 
		m_vScale = (v1 * fWeight0) + (v2 * fWeight1);

	bHaveKey1 = m_KeyOrient.DataGet(fFrm0, qt1);
	bHaveKey1 = m_KeyOrient.DataGet(fFrm1, qt2);
	if(bHaveKey1 && bHaveKey2) 
		m_qOrient.Slerp(qt1, qt2, fWeight1);

	this->ReCalcMatrix(); 
}
