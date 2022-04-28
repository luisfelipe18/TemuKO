#pragma once
#pragma warning(disable : 4786)

#include "N3Base.h"
#include <string>

#define N3FORMAT_VER_UNKN 0x00000000
#define N3FORMAT_VER_1068 0x00000001 
#define N3FORMAT_VER_1264 0x00000002 
#define N3FORMAT_VER_1298 0x00000004 
#define N3FORMAT_VER_2062 0x00000008 
#define N3FORMAT_VER_CURR 0x40000000 
#define N3FORMAT_VER_HERO 0x80000000 

static const int N3FORMAT_VER_DEFAULT = N3FORMAT_VER_1264;

class CN3BaseFileAccess : public CN3Base
{
protected:
	std::string		m_szFileName; 

public:
	uint32_t m_iFileFormatVersion;
	int m_iLOD; 

public:
	const std::string& FileName() const { return m_szFileName; } 
	void FileNameSet(const std::string& szFileName);

	bool LoadFromFile(); 
	virtual bool LoadFromFile(const std::string& szFileName, uint32_t iVer = N3FORMAT_VER_DEFAULT); 
	virtual bool Load(HANDLE hFile); 

	virtual bool SaveToFile(); 
	virtual bool SaveToFile(const std::string& szFileName); 
	virtual bool Save(HANDLE hFile); 

public:
	void Release();

	CN3BaseFileAccess();
	virtual ~CN3BaseFileAccess();
};
