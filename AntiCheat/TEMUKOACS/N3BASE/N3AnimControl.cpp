

#include "N3AnimControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3AnimControl::CN3AnimControl()
{
	m_dwType |= OBJ_ANIM_CONTROL;
}

CN3AnimControl::~CN3AnimControl()
{
}

void CN3AnimControl::Release()
{
	m_Datas.clear(); 

	CN3BaseFileAccess::Release();
}

bool CN3AnimControl::Load(HANDLE hFile)
{
	if(m_Datas.size() > 0) this->Release();

	DWORD dwRWC = 0;
	int nCount = 0;
	ReadFile(hFile, &nCount, 4, &dwRWC, NULL);

	m_Datas.clear(); 
	for(int i = 0; i < nCount; i++)
	{
		__AnimData Data;
		Data.Load(hFile);
		m_Datas.push_back(Data);
	}

	return true;
}