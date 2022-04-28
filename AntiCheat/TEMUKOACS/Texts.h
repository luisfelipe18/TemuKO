#pragma once
class CTexts;
#include "stdafx.h"

class CTexts
{
public:
	uint32_t m_iID;
	std::string m_strText;

public:
	CTexts(uint32_t id, std::string text);
	~CTexts();

private:

};

typedef std::list<CTexts*>				TextsList;
typedef TextsList::iterator				TextsListItor;
typedef TextsList::const_iterator		TextsListItorConst;
typedef TextsList::reverse_iterator		TextsListReverseItor;