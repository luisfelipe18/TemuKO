#pragma once
#include <list>
#include "types.h"
#include <string>
#include "CustomStringList.h"
using namespace std;

class CItemOrg
{
public:
	uint32_t m_iItemID;
	uint8_t m_iExtID;
	std::string m_strName;
	std::string m_strDesc;
	uint32_t m_iIconID;
	uint32_t m_iDxtID;
	uint8_t m_iKind;
	uint8_t m_iSlot;
	uint8_t m_iClass;
	int16_t m_iAttack;
	int16_t m_iDelay;
	int16_t m_iRange;
	int16_t m_iWeight;
	int16_t m_iDuration;
	int m_iRepairPrice;
	int m_iSellingPrice;
	int16_t m_iAC;
	uint8_t m_iIsCountable;
	char m_iReqLevelMin;
	char m_iReqLevelMax;
	uint8_t m_iReqStr;
	uint8_t m_iReqHp;
	uint8_t m_iReqDex;
	uint8_t m_iReqInt;
	uint8_t m_iReqMp;
	uint8_t m_iSellingGroup;
	uint8_t m_iItemGrade;
	CustomStringList customStrings;
public:
	CItemOrg();
	~CItemOrg();
	void ParseDescription();
	void ParseDescriptionExt();
	uint32_t GetColor(std::string color);

private:

};

typedef std::list<CItemOrg*>				ItemOrgList;
typedef ItemOrgList::iterator				ItemOrgListItor;
typedef ItemOrgList::const_iterator			ItemOrgListItorConst;
typedef ItemOrgList::reverse_iterator		ItemOrgListReverseItor;