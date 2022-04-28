// N3TableBase.h: interface for the CN3TableBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_)
#define AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>

template <typename Type> class CN3TableBase
{
public:
	CN3TableBase();
	virtual ~CN3TableBase();

	// Attributes
protected:
	enum DATA_TYPE { DT_NONE, DT_CHAR, DT_BYTE, DT_SHORT, DT_WORD, DT_INT, DT_DWORD, DT_STRING, DT_FLOAT, DT_DOUBLE };

	typename std::vector<DATA_TYPE> m_DataTypes;	// ½ÇÁ¦ »ç¿ëµÇ´Â Á¤º¸ÀÇ µ¥ÀÌÅ¸ Å¸ÀÔ
	typename std::map<uint32_t, Type> m_Datas; // ½ÇÁ¦ »ç¿ëµÇ´Â Á¤º¸

// Operations
public:
	void	Release();
	std::map<uint32_t, Type> * GetTable()
	{
		return &m_Datas;
	}
	Type* Find(uint32_t dwID) // ID·Î data Ã£±â
	{
		auto it = m_Datas.find(dwID);
		if (it == m_Datas.end()) return NULL; // Ã£±â¿¡ ½ÇÆĞ Çß´Ù!~!!
		else return &(it->second);
	}
	size_t	GetSize() { return m_Datas.size(); }
	Type* GetIndexedData(size_t index)	//index·Î Ã£±â..
	{
		if (m_Datas.empty()) return NULL;
		if (index >= m_Datas.size()) return NULL;

		auto it = m_Datas.begin();
		std::advance(it, index);
		return &(it->second);
	}
	bool	IDToIndex(uint32_t dwID, size_t* index) // ÇØ´ç IDÀÇ Index ¸®ÅÏ..	Skill¿¡¼­ ¾´´Ù..
	{
		auto it = m_Datas.find(dwID);
		if (it == m_Datas.end())
			return false; // Ã£±â¿¡ ½ÇÆĞ Çß´Ù!~!!

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

// cppÆÄÀÏ¿¡ ÀÖÀ¸´Ï±î link¿¡·¯°¡ ³­´Ù. ¿Ö ±×·²±î?

template <class Type>
CN3TableBase<Type>::CN3TableBase()
{
}

template <class Type>
CN3TableBase<Type>::~CN3TableBase()
{
	Release();
}

template <class Type>
void CN3TableBase<Type>::Release()
{
	m_DataTypes.clear(); // data type ÀúÀåÇÑ°Í Áö¿ì±â
	m_Datas.clear(); // row µ¥ÀÌÅ¸ Áö¿ì±â
}

// ÆÄÀÏ¿¡ µ¥ÀÌÅ¸ Å¸ÀÔº°·Î ¾²±â..
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
			if (iTemp < -127 || iTemp > 128) return FALSE; // ¹üÀ§°¡ ¹ş¾î³µ¾î~
			cWrite = (char)iTemp;
		}
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

		WriteFile(hFile, &cWrite, sizeof(cWrite), &dwNum, NULL);
	}
	break;
	case DT_BYTE:
	{
		uint8_t byteWrite;
		if (isdigit(lpszData[0]))
		{
			int iTemp = atoi(lpszData);
			if (iTemp < 0 || iTemp > 255) return FALSE; // ¹üÀ§°¡ ¹ş¾î³µ¾î~
			byteWrite = (uint8_t)iTemp;
		}
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

		WriteFile(hFile, &byteWrite, sizeof(byteWrite), &dwNum, NULL);
	}
	break;
	case DT_SHORT:
	{
		int16_t iWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0])
		{
			int iTemp = atoi(lpszData);
			if (iTemp < -32767 || iTemp > 32768) return FALSE; // ¹üÀ§°¡ ¹ş¾î³µ¾î~
			iWrite = (int16_t)iTemp;
		}
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_WORD:
	{
		uint16_t iWrite;
		if (isdigit(lpszData[0]))
		{
			int iTemp = atoi(lpszData);
			if (iTemp < 0 || iTemp > 65535) return FALSE; // ¹üÀ§°¡ ¹ş¾î³µ¾î~
			iWrite = (int16_t)iTemp;
		}
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_INT:
	{
		int iWrite;
		if (isdigit(lpszData[0]) || '-' == lpszData[0])	iWrite = atoi(lpszData);
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

		WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
	}
	break;
	case DT_DWORD:
	{
		uint32_t iWrite;
		if (isdigit(lpszData[0]))	iWrite = strtoul(lpszData, NULL, 10);
		else return FALSE;		// ¹®ÀÚ´Â ¾ÈµÇ~!

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
		else return FALSE;	// ¹®ÀÚ´Â ¾ÈµÇ~!
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
BOOL CN3TableBase<Type>::Load(HANDLE hFile)
{
	Release();

	// data(column) ÀÇ ±¸Á¶°¡ ¾î¶»°Ô µÇ¾î ÀÖ´ÂÁö ÀĞ±â
	DWORD dwNum;
	int i, j, iDataTypeCount = 0;
	ReadFile(hFile, &iDataTypeCount, 4, &dwNum, NULL);			// (¿¢¼¿¿¡¼­ column ¼ö)

	std::vector<int> offsets;
	__ASSERT(iDataTypeCount > 0, "Data Type ÀÌ 0 ÀÌÇÏÀÔ´Ï´Ù.");
	if (iDataTypeCount > 0)
	{
		m_DataTypes.insert(m_DataTypes.begin(), iDataTypeCount, DT_NONE);
		ReadFile(hFile, &(m_DataTypes[0]), sizeof(DATA_TYPE) * iDataTypeCount, &dwNum, NULL);	// °¢°¢ÀÇ column¿¡ ÇØ´çÇÏ´Â data type

		if (FALSE == MakeOffsetTable(offsets))
		{
			__ASSERT(0, "can't make offset table");
			return FALSE;	// structureº¯¼ö¿¡ ´ëÇÑ offset table ¸¸µé¾îÁÖ±â
		}

		int iSize = offsets[iDataTypeCount];	// MakeOffstTable ÇÔ¼ö¿¡¼­ ¸®ÅÏµÇ´Â °ªÁß m_iDataTypeCount¹øÂ°¿¡ ÀÌ ÇÔ¼öÀÇ ½ÇÁ¦ »çÀÌÁî°¡ µé¾îÀÖ´Ù.
		if (sizeof(Type) != iSize ||		// ÀüÃ¼ typeÀÇ Å©±â¿Í ½ÇÁ¦ ±¸Á¶Ã¼ÀÇ Å©±â¿Í ´Ù¸£°Å³ª 
			DT_DWORD != m_DataTypes[0])	// ¸Ç Ã³À½ÀÇ µ¥ÀÌÅ¸°¡ DT_DWORDÇüÀÌ ¾Æ´Ò¶§(¸ÇÃ³À½Àº °íÀ¯ÇÑ IDÀÌ¹Ç·Î)
		{
			m_DataTypes.clear();
			__ASSERT(0, "DataType is mismatch or DataSize is incorrect!!");
			return FALSE;
		}
	}

	// row °¡ ¸îÁÙÀÎÁö ÀĞ±â
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

		__ASSERT(pt.second, "CN3TableBase<Type> : Key Áßº¹ °æ°í.");
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

// structure´Â 4¹ÙÀÌÆ® Á¤·ÄÇÏ¿©¼­ ¸Ş¸ğ¸®¸¦ Àâ´Â´Ù. µû¶ó¼­ ¾Æ·¡ ÇÔ¼ö°¡ ÇÊ¿äÇÏ´Ù.
// ¾Æ·¡ ÇÔ¼ö·Î OffsetTableÀ» ¸¸µé¾î ¾´ ÈÄ¿¡´Â ¸¸µå½Ã ¸®ÅÏ°ªÀ» delete [] ¸¦ ÇØÁÖ¾î¾ß ÇÑ´Ù.
template <class Type>
BOOL CN3TableBase<Type>::MakeOffsetTable(std::vector<int>& offsets)
{
	if (m_DataTypes.empty()) return false;

	int i, iDataTypeCount = m_DataTypes.size();
	offsets.clear();
	offsets.resize(iDataTypeCount + 1);	// +1À» ÇÑ ÀÌÀ¯´Â ¸Ç ¸¶Áö¸· °ª¿¡ TypeÀÇ ½ÇÁ¦ »çÀÌÁî¸¦ ³Ö±â À§ÇØ¼­
	offsets[0] = 0;
	int iPrevDataSize = SizeOf(m_DataTypes[0]);
	for (i = 1; i < iDataTypeCount; ++i)
	{
		int iCurDataSize = SizeOf(m_DataTypes[i]);
		if (1 == iCurDataSize % 4)	// ÇöÀç µ¥ÀÌÅÍ°¡ 1¹ÙÀÌÆ®¸é ±×³É ÀÌÀü µ¥ÀÌÅÍ°¡ ¸î¹ÙÀÌÆ®µç »ó°ü ¾ø´Ù.
		{
			offsets[i] = offsets[i - 1] + iPrevDataSize;
		}
		else if (2 == iCurDataSize % 4) // ÇöÀç µ¥ÀÌÅÍ°¡ 2¹ÙÀÌÆ®¸é Â¦¼ö¹øÁö¿¡ À§Ä¡ÇØ¾ß ÇÑ´Ù.
		{
			if (0 == ((offsets[i - 1] + iPrevDataSize) % 2))
				offsets[i] = offsets[i - 1] + iPrevDataSize;
			else
				offsets[i] = offsets[i - 1] + iPrevDataSize + 1;
		}
		else if (0 == iCurDataSize % 4) // ÇöÀç µ¥ÀÌÅÍ°¡ 4¹ÙÀÌÆ®¸é 4ÀÇ ¹è¼ö¹øÁö¿¡ À§Ä¡ÇØ¾ß ÇÑ´Ù.
		{
			if (0 == ((offsets[i - 1] + iPrevDataSize) % 4))
				offsets[i] = offsets[i - 1] + iPrevDataSize;
			else
				offsets[i] = ((int)(offsets[i - 1] + iPrevDataSize + 3) / 4) * 4;	// 4ÀÇ ¹è¼ö·Î ¸¸µé±â
		}
		else __ASSERT(0, "");
		iPrevDataSize = iCurDataSize;
	}

	// ¸Ç ¸¶Áö¸· °ª¿¡ TypeÀÇ ½ÇÁ¦ »çÀÌÁî¸¦ ³ÖÀÚ.
	offsets[iDataTypeCount] = ((int)(offsets[iDataTypeCount - 1] + iPrevDataSize + 3) / 4) * 4;	// 4ÀÇ ¹è¼ö·Î ¸¸µé±â

	return true;
}

#endif // !defined(AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_)
