

#include "N3Transform.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3Transform::CN3Transform()
{
	m_dwType |= OBJ_TRANSFORM;

	m_vPos.Set(0,0,0); 
	m_vScale.Set(1,1,1);
	m_qRot.Identity();
	m_Matrix.Identity();

	
	m_fFrmCur = 0;
	m_fFrmWhole = 0;
}

CN3Transform::~CN3Transform()
{
}

void CN3Transform::Release()
{
	m_vPos.Set(0,0,0); 
	m_vScale.Set(1,1,1);
	m_qRot.Identity();

	m_Matrix.Identity();

	
	m_fFrmCur = 0;
	m_fFrmWhole = 0;

	m_KeyPos.Release();
	m_KeyRot.Release();
	m_KeyScale.Release();

	CN3Base::Release();
}

bool CN3Transform::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC = 0;
	ReadFile(hFile, &m_vPos, sizeof(__Vector3), &dwRWC, NULL); 
	ReadFile(hFile, &m_qRot, sizeof(__Quaternion), &dwRWC, NULL);
	ReadFile(hFile, &m_vScale, sizeof(__Vector3), &dwRWC, NULL);

	
	m_KeyPos.Load(hFile);
	m_KeyRot.Load(hFile);
	m_KeyScale.Load(hFile);

	m_fFrmCur = 0;
	m_fFrmWhole = 0;

	float fFrmWhole = 0;
	
	fFrmWhole = m_KeyPos.Count() * m_KeyPos.SamplingRate() / 30.0f;
	if(fFrmWhole > m_fFrmWhole) m_fFrmWhole = fFrmWhole;
	fFrmWhole = m_KeyRot.Count() * m_KeyRot.SamplingRate() / 30.0f;
	if(fFrmWhole > m_fFrmWhole) m_fFrmWhole = fFrmWhole;
	fFrmWhole = m_KeyScale.Count() * m_KeyScale.SamplingRate() / 30.0f;
	if(fFrmWhole > m_fFrmWhole) m_fFrmWhole = fFrmWhole;

	this->ReCalcMatrix(); 

	return true;
}

void CN3Transform::Tick(float fFrm)
{
	if(FRAME_SELFPLAY == fFrm)
	{
		m_fFrmCur += s_fSecPerFrm;
		if(m_fFrmCur < 0) m_fFrmCur = 0.0f;
		if(m_fFrmCur >= m_fFrmWhole) m_fFrmCur = 0.0f;
		fFrm = m_fFrmCur;
	}
	else
	{
		m_fFrmCur = fFrm;
	}

	bool bNdeedReCalcMatrix = this->TickAnimationKey(m_fFrmCur);
	
	if(m_dwType & OBJ_JOINT) return; 

	if(bNdeedReCalcMatrix) this->ReCalcMatrix();
}

void CN3Transform::ReCalcMatrix()
{
	m_Matrix.Scale(m_vScale);
	if(m_qRot.w != 0)
	{
		static __Matrix44 mtxRot;
		D3DXMatrixRotationQuaternion(&mtxRot, &m_qRot);
		m_Matrix *= mtxRot;
	}
	m_Matrix.PosSet(m_vPos);
}

bool CN3Transform::TickAnimationKey(float fFrm)
{
	
	int nKCP = m_KeyPos.Count();
	int nKCR = m_KeyRot.Count();
	int nKCS = m_KeyScale.Count();
	if(nKCP <= 0 && nKCR <= 0 && nKCS <= 0) return false;

	bool bNeedReCalcMatrix = false;
	if(m_KeyPos.DataGet(fFrm, m_vPos) == true) bNeedReCalcMatrix = true;
	if(m_KeyRot.DataGet(fFrm, m_qRot) == true) bNeedReCalcMatrix = true;
	if(m_KeyScale.DataGet(fFrm, m_vScale) == true) bNeedReCalcMatrix = true;

	return bNeedReCalcMatrix;
}

