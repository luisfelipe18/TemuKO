#pragma once

#include <vector>
#include <map>

#define __ASSERT(expr, expMessage) \
if(!(expr)) {\
	printf("ERROR-> %s\n%s: %d\n\n", expMessage, __FILE__, __LINE__);\
}

template <typename Type> class CN3TableBase
{
public:
	CN3TableBase();
	virtual ~CN3TableBase();

	
protected:
	enum DATA_TYPE { DT_NONE, DT_CHAR, DT_BYTE, DT_SHORT, DT_WORD, DT_INT, DT_DWORD, DT_STRING, DT_FLOAT, DT_DOUBLE };

	typename std::vector<DATA_TYPE> m_DataTypes;	
	typename std::map<uint32_t, Type> m_Datas; 

public:
	string fl;
	void	Release();
	std::map<uint32_t, Type>* GetTable()
	{
		return &m_Datas;
	}
	Type* Find(uint32_t dwID) 
	{
		auto it = m_Datas.find(dwID);
		if (it == m_Datas.end()) return NULL; 
		else return &(it->second);
	}
	size_t	GetSize() { return m_Datas.size(); }
	Type* GetIndexedData(size_t index)	
	{
		if (m_Datas.empty()) return NULL;
		if (index >= m_Datas.size()) return NULL;

		auto it = m_Datas.begin();
		std::advance(it, index);
		return &(it->second);
	}
	bool	IDToIndex(uint32_t dwID, size_t* index) 
	{
		auto it = m_Datas.find(dwID);
		if (it == m_Datas.end())
			return false; 

		auto itSkill = m_Datas.begin();
		size_t iSize = m_Datas.size();
		for (size_t i = 0; i < iSize; i++, itSkill++)
		{
			if (itSkill == it)
			{
				*index = i;
				return true;
			}
		}

		return false;
	}
	BOOL	LoadFromFile(const std::string& szFN);
protected:
	BOOL	Load(HANDLE hFile);
	BOOL	WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData);
	BOOL	ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData);

	int		SizeOf(DATA_TYPE DataType) const;
	BOOL	MakeOffsetTable(std::vector<int>& offsets);
};

template <class Type>
CN3TableBase<Type>::CN3TableBase()
{
	fl = "";
}

template <class Type>
CN3TableBase<Type>::~CN3TableBase()
{
	Release();
}

template <class Type>
void CN3TableBase<Type>::Release()
{
	m_DataTypes.clear(); 
	m_Datas.clear(); 
}

template <class Type>
BOOL CN3TableBase<Type>::WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData)
{
	DWORD dwNum;
	switch (DataType)
	{
	case DT_CHAR:
	{
		char cWrite;
		if (isdigit(lpszData[0]))
		{
			int iTemp = atoi(lpszData);
			if (iTemp < -127 || iTemp > 128) return FALSE; 
			cWrite = (char)iTemp;
		}
		else return FALSE;		

		WriteFile(hFile, &cWrite, sizeof(cWrite), &dwNum, NULL);
	}
	break;
	case DT_BYTE:
	{
		uint8_t byteWrite;
		if (isdigit(lpszData[0]))
		{
			int iTemp = atoi(lpszData);
			if (iTemp < 0 || iTemp > 255) return FALSE; 
			byteWrite = (uint8_t)iTemp;
		}
		else return FALSE;		

		WriteFile(hFile, &byteWrite, sizeof(byteWrite), &dwNum, NULL);
	}
	break;
	case DT_SHORT:
	{
		int16_t iWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0])
		{
			int iTemp = atoi(lpszData);
			if (iTemp < -32767 || iTemp > 32768) return FALSE; 
			iWrite = (int16_t)iTemp;
		}
		else return FALSE;		

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_WORD:
	{
		uint16_t iWrite;
		if (isdigit(lpszData[0]))
		{
			int iTemp = atoi(lpszData);
			if (iTemp < 0 || iTemp > 65535) return FALSE; 
			iWrite = (int16_t)iTemp;
		}
		else return FALSE;		

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_INT:
	{
		int iWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0])	iWrite = atoi(lpszData);
		else return FALSE;		

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_DWORD:
	{
		uint32_t iWrite;
		if (isdigit(lpszData[0]))	iWrite = strtoul(lpszData, NULL, 10);
		else return FALSE;		

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_STRING:
	{
		std::string& szString = *((std::string*)lpszData);
		int iStrLen = szString.size();
		WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);
		if (iStrLen > 0) WriteFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
	}
	break;
	case DT_FLOAT:
	{
		float fWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
			'.' == lpszData[0])	fWrite = (float)atof(lpszData);
		else return FALSE;	
		WriteFile(hFile, &fWrite, sizeof(fWrite), &dwNum, NULL);
	}
	break;
	case DT_DOUBLE:
	{
		double dWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
			'.' == lpszData[0])	dWrite = atof(lpszData);
		WriteFile(hFile, &dWrite, sizeof(dWrite), &dwNum, NULL);
	}
	break;

	case DT_NONE:
	default:
		__ASSERT(0, "");
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData)
{
	DWORD dwNum;
	switch (DataType)
	{
	case DT_CHAR:
	{
		ReadFile(hFile, pData, sizeof(char), &dwNum, NULL);
	}
	break;
	case DT_BYTE:
	{
		ReadFile(hFile, pData, sizeof(uint8_t), &dwNum, NULL);
	}
	break;
	case DT_SHORT:
	{
		ReadFile(hFile, pData, sizeof(int16_t), &dwNum, NULL);
	}
	break;
	case DT_WORD:
	{
		ReadFile(hFile, pData, sizeof(uint16_t), &dwNum, NULL);
	}
	break;
	case DT_INT:
	{
		ReadFile(hFile, pData, sizeof(int), &dwNum, NULL);
	}
	break;
	case DT_DWORD:
	{
		ReadFile(hFile, pData, sizeof(uint32_t), &dwNum, NULL);
	}
	break;
	case DT_STRING:
	{
		std::string& szString = *((std::string*)pData);

		int iStrLen = 0;
		ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);

		szString = "";
		if (iStrLen > 0)
		{
			szString.assign(iStrLen, ' ');
			ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
		}
	}
	break;
	case DT_FLOAT:
	{
		ReadFile(hFile, pData, sizeof(float), &dwNum, NULL);
	}
	break;
	case DT_DOUBLE:
	{
		ReadFile(hFile, pData, sizeof(double), &dwNum, NULL);
	}
	break;

	case DT_NONE:
	default:
		__ASSERT(0, "");
		return FALSE;
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::LoadFromFile(const std::string& szFN)
{
	if (szFN.empty()) return FALSE;

	fl = szFN;

	HANDLE hFile = ::CreateFileA(szFN.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("N3TableBase - Can't open file(read) File Handle error (%s)", szFN.c_str());
		return FALSE;
	}

	std::string szFNTmp = szFN + ".tmp";
	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if (dwSizeLow <= 0)
	{
		CloseHandle(hFile);
		::remove(szFNTmp.c_str()); 
		return FALSE;
	}

	
	uint8_t* pDatas = new uint8_t[dwSizeLow];
	DWORD dwRWC = 0;
	::ReadFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL); 
	CloseHandle(hFile); 

	uint16_t key_r = 0x0816;
	uint16_t key_c1 = 0x6081;
	uint16_t key_c2 = 0x1608;

	
	
	
	
	
	
	

	
	
	
	
	
	
	

		
	for (uint32_t i = 0; i < dwSizeLow; i++)
	{
		uint8_t byData = (pDatas[i] ^ (key_r >> 8));
		key_r = (pDatas[i] + key_r) * key_c1 + key_c2;
		pDatas[i] = byData;
	}

	
	hFile = ::CreateFileA(szFNTmp.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	::WriteFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL); 
	CloseHandle(hFile); 
	delete[] pDatas; pDatas = NULL;

	hFile = ::CreateFileA(szFNTmp.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 

	BOOL bResult = Load(hFile);

	CloseHandle(hFile);

	if (FALSE == bResult)
	{
		printf("N3TableBase - incorrect table (%s)", szFN.c_str());
	}

	
	::remove(szFNTmp.c_str());

	return bResult;
}

template <class Type>
BOOL CN3TableBase<Type>::Load(HANDLE hFile)
{
	Release();

	
	DWORD dwNum;
	int i, j, iDataTypeCount = 0;
	ReadFile(hFile, &iDataTypeCount, 4, &dwNum, NULL);			

	std::vector<int> offsets;
	__ASSERT(iDataTypeCount > 0, string_format("Data Type is 0 or less: %s\n", fl.c_str()).c_str());
	if (iDataTypeCount > 0)
	{
		m_DataTypes.insert(m_DataTypes.begin(), iDataTypeCount, DT_NONE);
		ReadFile(hFile, &(m_DataTypes[0]), sizeof(DATA_TYPE) * iDataTypeCount, &dwNum, NULL);	

		if (FALSE == MakeOffsetTable(offsets))
		{
			__ASSERT(0, "can't make offset table");
			return FALSE;	
		}

		int iSize = offsets[iDataTypeCount];	
		if (sizeof(Type) != iSize ||		
			DT_DWORD != m_DataTypes[0])	
		{
			m_DataTypes.clear();
			__ASSERT(0, "DataType is mismatch or DataSize is incorrect!!");
			return FALSE;
		}
	}

	
	int iRC;
	ReadFile(hFile, &iRC, sizeof(iRC), &dwNum, NULL);
	Type Data;
	for (i = 0; i < iRC; ++i)
	{
		for (j = 0; j < iDataTypeCount; ++j)
		{
			ReadData(hFile, m_DataTypes[j], (char*)(&Data) + offsets[j]);
		}

		uint32_t dwKey = *((uint32_t*)(&Data));
		auto pt = m_Datas.insert(std::make_pair(dwKey, Data));

		__ASSERT(pt.second, "CN3TableBase<Type> : Key 중복 경고.");
	}
	return TRUE;
}

template <class Type>
int CN3TableBase<Type>::SizeOf(DATA_TYPE DataType) const
{
	switch (DataType)
	{
	case DT_CHAR:
		return sizeof(char);
	case DT_BYTE:
		return sizeof(uint8_t);
	case DT_SHORT:
		return sizeof(int16_t);
	case DT_WORD:
		return sizeof(uint16_t);
	case DT_INT:
		return sizeof(int);
	case DT_DWORD:
		return sizeof(uint32_t);
	case DT_STRING:
		return sizeof(std::string);
	case DT_FLOAT:
		return sizeof(float);
	case DT_DOUBLE:
		return sizeof(double);
	}
	__ASSERT(0, "");
	return 0;
}

template <class Type>
BOOL CN3TableBase<Type>::MakeOffsetTable(std::vector<int>& offsets)
{
	if (m_DataTypes.empty()) return false;

	int i, iDataTypeCount = m_DataTypes.size();
	offsets.clear();
	offsets.resize(iDataTypeCount + 1);	
	offsets[0] = 0;
	int iPrevDataSize = SizeOf(m_DataTypes[0]);
	for (i = 1; i < iDataTypeCount; ++i)
	{
		int iCurDataSize = SizeOf(m_DataTypes[i]);
		if (1 == iCurDataSize % 4)	
		{
			offsets[i] = offsets[i - 1] + iPrevDataSize;
		}
		else if (2 == iCurDataSize % 4) 
		{
			if (0 == ((offsets[i - 1] + iPrevDataSize) % 2))
				offsets[i] = offsets[i - 1] + iPrevDataSize;
			else
				offsets[i] = offsets[i - 1] + iPrevDataSize + 1;
		}
		else if (0 == iCurDataSize % 4) 
		{
			if (0 == ((offsets[i - 1] + iPrevDataSize) % 4))
				offsets[i] = offsets[i - 1] + iPrevDataSize;
			else
				offsets[i] = ((int)(offsets[i - 1] + iPrevDataSize + 3) / 4) * 4;	
		}
		else __ASSERT(0, "");
		iPrevDataSize = iCurDataSize;
	}

	
	offsets[iDataTypeCount] = ((int)(offsets[iDataTypeCount - 1] + iPrevDataSize + 3) / 4) * 4;	

	return true;
}
