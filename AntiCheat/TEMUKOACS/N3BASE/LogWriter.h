

#if !defined(AFX_LOGWRITER_H__3299921C_04C3_40D6_848B_52F0CE5C9352__INCLUDED_)
#define AFX_LOGWRITER_H__3299921C_04C3_40D6_848B_52F0CE5C9352__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <string>

class CLogWriter  
{
protected:

	static std::string s_szFileName;

public:
	static void Open(const std::string& szFN);
	static void Close();
	static void Write(const char* lpszFormat, ...);
	
	CLogWriter();
	virtual ~CLogWriter();

};

#endif 
