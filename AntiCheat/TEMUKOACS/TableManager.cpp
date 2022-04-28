#include "TableManager.h"

CTableManager::CTableManager()
{
}

CTableManager::~CTableManager()
{
}

typedef struct __TABLE_TEXT
{
	uint32_t m_iID;
	std::string m_strText;
} TABLE_TEXT;

static CN3TableBase<TABLE_ITEM_BASIC> tbl_item;
static CN3TableBase<TABLE_MOB> mobs;
static CN3TableBase<TABLE_TEXT> texts;
static CN3TableBase<TABLE_ITEM_EXT> ext[45];

void LoadItemExtsTbl()
{
	for (int i = 0; i < 45; i++)
	{
		string szFN = string_format(xorstr("TEMUKOACS\\data\\exts\\Item_Ext_%d_us.tbl"), i);
		ext[i].LoadFromFile(szFN.c_str());
	}
}

void LoadItemOrgTbl()
{
	string szFN = xorstr("TEMUKOACS\\data\\items.tbl");
	tbl_item.LoadFromFile(szFN.c_str());
}

void LoadMobTbl()
{
	string szFN = xorstr("TEMUKOACS\\data\\mobs.tbl");
	mobs.LoadFromFile(szFN.c_str());
}

void LoadTextsTbl()
{
	string szFN = xorstr("TEMUKOACS\\data\\texts.tbl");
	texts.LoadFromFile(szFN.c_str());
}

void CTableManager::Init()
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LoadItemExtsTbl, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LoadItemOrgTbl, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LoadMobTbl, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LoadTextsTbl, NULL, NULL, NULL);
}

uint32 getItemOriginIDExAS(uint32 sID)
{
	return std::atoi((std::to_string(sID).substr(0, 6) + xorstr("000")).c_str());
}

TABLE_ITEM_BASIC* CTableManager::getItemData(uint32 ID) {
	TABLE_ITEM_BASIC* d = tbl_item.Find(getItemOriginIDExAS(ID));
	return (d == NULL ? nullptr : d);
}

std::map<uint32_t, TABLE_ITEM_BASIC>* CTableManager::GetItemTable() {
	return tbl_item.GetTable();
}

TABLE_MOB* CTableManager::getMobData(uint32 ID) {
	TABLE_MOB* d = mobs.Find(ID);
	return (d == NULL ? nullptr : d);
}

TABLE_ITEM_EXT* CTableManager::getExtData(uint32 extNum, uint32 ID) {
	auto found = ext[extNum].Find(ID % 1000);
	if (found != NULL)
		return found;
	
	std::map <uint32_t, TABLE_ITEM_EXT>* d = ext[extNum].GetTable();

	for (std::map<uint32_t, TABLE_ITEM_EXT>::iterator itr = d->begin(), itr_end = d->end(); itr != itr_end; ++itr) {
		if (itr->second.baseItemID == ID) {
			return &itr->second;
		}
	}
	return nullptr;
}

std::map<uint32_t, TABLE_MOB>* CTableManager::GetMobTable() {
	return mobs.GetTable();
}

void CTableManager::GetResourceText(uint32_t dwID, std::string& szText)
{
	TABLE_TEXT* txt = texts.Find(dwID);
	Engine->StringHelper->Replace(txt->m_strText, (char)25, (char)39);
	szText = txt->m_strText;
}