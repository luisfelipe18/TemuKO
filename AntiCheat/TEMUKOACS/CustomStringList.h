#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include <list>
#include <iterator>

class CCustomString
{
public:
	std::string m_strText;
	std::string m_strColor;
	uint32_t m_iColor;
	bool m_bItalic;
	bool m_bBold;
	int m_iLineIndex;
	POINT m_pDrawPos;
	SIZE m_pSize;

public:

private:

};

typedef std::list<CCustomString*>				CustomStringList;
typedef CustomStringList::iterator				CustomStringListItor;
typedef CustomStringList::const_iterator		CustomStringListItorConst;
typedef CustomStringList::reverse_iterator		CustomStringListReverseItor;