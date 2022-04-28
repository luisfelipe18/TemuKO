#pragma once
class CTableManager;
#include "stdafx.h"
#include "N3BASE/N3TableBase.h"
#include "Texts.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/GameDef.h"

class CTableManager
{
public:
	
	CTableManager();
	~CTableManager();
	void Init();
	__TABLE_ITEM_BASIC* getItemData(uint32 ID);
	__TABLE_ITEM_EXT* getExtData(uint32 extNum, uint32 ID);
	std::map<uint32_t, __TABLE_ITEM_BASIC>* GetItemTable();
	TABLE_MOB* getMobData(uint32 ID);
	std::map<uint32_t, TABLE_MOB>* GetMobTable();
	void GetResourceText(uint32_t dwID, std::string& szText);
private:

};