

#if !defined(AFX_N3ANIMCONTROL_H__37E9A095_FF76_4DD5_95A2_4CA1ABC227B0__INCLUDED_)
#define AFX_N3ANIMCONTROL_H__37E9A095_FF76_4DD5_95A2_4CA1ABC227B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#pragma warning(disable : 4786)

#include "N3BaseFileAccess.h"
#include <string>
#include <vector>

typedef struct __AnimData
{
	friend CN3AnimControl;
public:
	std::string	szName;

	float	fFrmStart; 
	float	fFrmEnd; 
	float	fFrmPerSec; 

	float	fFrmPlugTraceStart;
	float	fFrmPlugTraceEnd;
	
	float	fFrmSound0;
	float	fFrmSound1;

	float	fTimeBlend; 
	int		iBlendFlags; 
	
	float	fFrmStrike0;
	float	fFrmStrike1;
	
	__AnimData()
	{
		fFrmPerSec = 30.0f; 

		fFrmStart = fFrmEnd = 0;
		fFrmPlugTraceStart = fFrmPlugTraceEnd = 0;
		fFrmSound0 = fFrmSound1 = 0;
		fTimeBlend = 0.25f; 
		iBlendFlags = 0; 
		fFrmStrike0 = fFrmStrike1 = 0;
	}
	
	void operator = (const __AnimData& other)
	{
		fFrmStart = other.fFrmStart;
		fFrmEnd = other.fFrmEnd;
		fFrmPerSec = other.fFrmPerSec;

		fFrmPlugTraceStart = other.fFrmPlugTraceStart;
		fFrmPlugTraceEnd = other.fFrmPlugTraceEnd;

		fFrmSound0 = other.fFrmSound0;
		fFrmSound1 = other.fFrmSound1;

		fTimeBlend = other.fTimeBlend;
		iBlendFlags = other.iBlendFlags; 
		
		fFrmStrike0 = other.fFrmStrike0;
		fFrmStrike1 = other.fFrmStrike1;

		szName = other.szName;
	}

	void Load(HANDLE hFile)
	{
		if(NULL == hFile || INVALID_HANDLE_VALUE == hFile) return;

		DWORD dwRWC = 0;

		int nL = 0;
		ReadFile(hFile, &nL, 4, &dwRWC, NULL); 

		ReadFile(hFile, &fFrmStart, 4, &dwRWC, NULL); 
		ReadFile(hFile, &fFrmEnd, 4, &dwRWC, NULL); 
		ReadFile(hFile, &fFrmPerSec, 4, &dwRWC, NULL); 

		ReadFile(hFile, &fFrmPlugTraceStart, 4, &dwRWC, NULL);
		ReadFile(hFile, &fFrmPlugTraceEnd, 4, &dwRWC, NULL);
		
		ReadFile(hFile, &fFrmSound0, 4, &dwRWC, NULL);
		ReadFile(hFile, &fFrmSound1, 4, &dwRWC, NULL);

		ReadFile(hFile, &fTimeBlend, 4, &dwRWC, NULL);
		ReadFile(hFile, &iBlendFlags, 4, &dwRWC, NULL); 
		
		ReadFile(hFile, &fFrmStrike0, 4, &dwRWC, NULL);
		ReadFile(hFile, &fFrmStrike1, 4, &dwRWC, NULL);

		
		szName = "";
		ReadFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0)
		{
			std::vector<char> buffer(nL+1, NULL);
			ReadFile(hFile, &buffer[0], nL, &dwRWC, NULL);
			szName = &buffer[0];
		}
	}
	void Save(HANDLE hFile)
	{
		if(NULL == hFile || INVALID_HANDLE_VALUE == hFile) return;

		DWORD dwRWC = 0;

		int nL = 0;
		WriteFile(hFile, &nL, 4, &dwRWC, NULL); 

		WriteFile(hFile, &fFrmStart, 4, &dwRWC, NULL); 
		WriteFile(hFile, &fFrmEnd, 4, &dwRWC, NULL); 
		WriteFile(hFile, &fFrmPerSec, 4, &dwRWC, NULL); 

		WriteFile(hFile, &fFrmPlugTraceStart, 4, &dwRWC, NULL);
		WriteFile(hFile, &fFrmPlugTraceEnd, 4, &dwRWC, NULL);
		
		WriteFile(hFile, &fFrmSound0, 4, &dwRWC, NULL);
		WriteFile(hFile, &fFrmSound1, 4, &dwRWC, NULL);

		WriteFile(hFile, &fTimeBlend, 4, &dwRWC, NULL);
		WriteFile(hFile, &iBlendFlags, 4, &dwRWC, NULL); 
		
		WriteFile(hFile, &fFrmStrike0, 4, &dwRWC, NULL);
		WriteFile(hFile, &fFrmStrike1, 4, &dwRWC, NULL);

		
		nL = szName.size();
		WriteFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0) WriteFile(hFile, szName.c_str(), nL, &dwRWC, NULL);
	}
} __AnimData;

typedef std::vector<__AnimData>::iterator it_Ani;

class CN3AnimControl : public CN3BaseFileAccess
{
protected:
	std::vector<__AnimData>		m_Datas; 

public:
	__AnimData* DataGet(size_t index) { if (index >= m_Datas.size()) return NULL; return &(m_Datas[index]); }
	bool Load(HANDLE hFile);
	int Count() { return m_Datas.size(); }
	void Release();
	
	CN3AnimControl();
	virtual ~CN3AnimControl();

};

#endif 
