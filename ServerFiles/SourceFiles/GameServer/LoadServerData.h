#pragma once

class C3DMap;

#include "../shared/types.h"
#include "../shared/STLMap.h"
#include "GameDefine.h"
#include "Knights.h"
#include "Npc.h"
#include "NpcTable.h"
#include "NpcThread.h"

class CKingSystem;

typedef CSTLMap <C3DMap>					ZoneArray;
typedef std::map<uint8, _LEVEL_UP *>			LevelUpArray;
typedef CSTLMap <_CLASS_COEFFICIENT>		CoefficientArray;
typedef CSTLMap <_ITEM_TABLE>				ItemtableArray;
typedef CSTLMap <_ITEM_SELLTABLE>			ItemSellTableArray;
typedef CSTLMap <_ITEM_EXPIRATION_TABLE>	ItemExpirationArray;
typedef CSTLMap <_MAGIC_TABLE>				MagictableArray;
typedef CSTLMap <_MAGIC_TYPE1>				Magictype1Array;
typedef CSTLMap <_MAGIC_TYPE2>				Magictype2Array;
typedef CSTLMap <_MAGIC_TYPE3>				Magictype3Array;
typedef CSTLMap	<_MAGIC_TYPE4>				Magictype4Array;
typedef CSTLMap <_MAGIC_TYPE5>				Magictype5Array;
typedef CSTLMap <_MAGIC_TYPE6>				Magictype6Array;
typedef CSTLMap <_MAGIC_TYPE7>				Magictype7Array;
typedef CSTLMap <_MAGIC_TYPE8>				Magictype8Array; 
typedef CSTLMap <_MAGIC_TYPE9>				Magictype9Array;

// NPC Related typedefs
typedef CSTLMap <CNpcThread>				NpcThreadArray;
typedef CSTLMap <_K_NPC_POS>				NpcPosArray;
typedef CSTLMap <_K_NPC_ITEM>				NpcItemArray;
typedef CSTLMap <_K_MONSTER_ITEM>			MonsterItemArray;
typedef CSTLMap <_MAKE_ITEM_GROUP>			MakeItemGroupArray;
typedef CSTLMap <_MAKE_WEAPON>				MakeWeaponItemTableArray;
typedef CSTLMap <_MAKE_ITEM_GRADE_CODE>		MakeGradeItemTableArray;
typedef CSTLMap <_MAKE_ITEM_LARE_CODE>		MakeLareItemTableArray;
typedef CSTLMap <CNpcTable>					NpcTableArray;

typedef CSTLMap <CBot>						BotArray;

typedef CSTLMap <_PARTY_GROUP>				PartyArray;
typedef CSTLMap <CKnights>					KnightsArray;
typedef CSTLMap <_KNIGHTS_RATING>			KnightsRatingArray;
typedef CSTLMap <_KNIGHTS_ALLIANCE>			KnightsAllianceArray;
typedef CSTLMap <_ZONE_SERVERINFO>			ServerArray;
typedef CSTLMap <_KNIGHTS_CAPE>				KnightsCapeArray;
typedef CSTLMap <_START_POSITION>			StartPositionArray;
typedef	CSTLMap	<_SERVER_RESOURCE>			ServerResourceArray;
typedef	CSTLMap	<_QUEST_HELPER>				QuestHelperArray;
typedef	CSTLMap	<_QUEST_HELPER_NATION>		QuestHelperNationArray;
typedef	CSTLMap	<_QUEST_MONSTER>			QuestMonsterArray;
typedef	CSTLMap	<_RENTAL_ITEM>				RentalItemArray;
typedef CSTLMap <_ITEM_EXCHANGE>			ItemExchangeArray;
typedef CSTLMap <_ITEM_EXCHANGE_EXP>		ItemExchangeExpArray;
typedef CSTLMap <SPECIAL_PART_SEWING_EXCHANGE>	ItemSpecialExchangeArray;
typedef CSTLMap <_ITEM_EXCHANGE_CRASH>		ItemExchangeCrashArray;
typedef CSTLMap <_ITEM_UPGRADE>				ItemUpgradeArray;
typedef CSTLMap <_ITEM_OP>					ItemOpArray;
typedef CSTLMap <CKingSystem>				KingSystemArray;
typedef std::map <std::string, _SHERIFF_STUFF *>	SheriffArray;
typedef CSTLMap <_SET_ITEM>					SetItemArray;
typedef CSTLMap <_ACHIEVE_TITLE>			AchieveTitleArray;
typedef CSTLMap <_ACHIEVE_WAR>				AchieveWarArray;
typedef CSTLMap <_ACHIEVE_MAIN>				AchieveMainArray;
typedef CSTLMap <_ACHIEVE_MONSTER>			AchieveMonsterArray;
typedef CSTLMap <_ACHIEVE_NORMAL>			AchieveNormalArray;
typedef CSTLMap <_ACHIEVE_COM>				AchieveComArray;

typedef std::multimap <uint8, _ITEM_PREMIUM_GIFT*>	ItemPremiumGiftArray;
typedef std::map<std::string, _COMMANDER*>			CommanderArray;
typedef std::map<std::string, _USER_RANK *>			UserNameRankMap; 
typedef std::map<uint8, _USER_RANK *>				UserRankMap;
typedef std::vector<_QUEST_HELPER *>				QuestHelperList;
typedef std::map<uint16, QuestHelperList>			QuestNpcList;
typedef	CSTLMap	<_MONSTER_RESOURCE>					MonsterResourceArray;
typedef std::vector <_SEEKING_PARTY_USER*>			SeekingPartyArray;
typedef std::vector<_MONSTER_SUMMON_LIST>			MonsterSummonList;
typedef CSTLMap <MonsterSummonList>					MonsterSummonListArray;
typedef CSTLMap <_CHAOS_STONE_STAGE>				ChaosStoneStageArray;
typedef CSTLMap <_CHAOS_STONE_RESPAWN>				ChaosStoneRespawnCoordinateArray;
typedef CSTLMap <_CHAOS_STONE_SUMMON_LIST>			ChaosStoneSummonListArray;
typedef CSTLMap <_CHAOS_STONE_INFO>					ChaosStoneInfoArray;
typedef CSTLMap <_MONSTER_UNDER_THE_CASTLE>			MonsterUnderTheCastleArray;
typedef CSTLMap <_MONSTER_STONE_LIST_INFORMATION>	MonsterStoneListInformationArray;
typedef CSTLMap <_MONSTER_JURAID_MOUNTAIN_RESPAWN_LIST>	JuraidMountionListInformationArray;

typedef CSTLMap <_MONSTER_RESPAWN_VERIABLE_LIST>	MonsterVeriableRespawnListArray;
typedef CSTLMap <_MONSTER_RESPAWN_STABLE_LIST>		MonsterStableRespawnListArray;
typedef CSTLMap <_MONSTER_RESPAWN_STABLE_SIGN_LIST>	MonsterStableSignRespawnListArray;

typedef CSTLMap <_PREMIUM_ITEM>						PremiumItemArray;
typedef CSTLMap <_PREMIUM_ITEM_EXP>					PremiumItemExpArray;

typedef CSTLMap <_BORDER_DEFENCE_WAR_RANKING>		UserBorderDefenceWarRankingArray;
typedef CSTLMap <_CHAOS_EXPANSION_RANKING>			UserChaosExpansionRankingArray;
typedef CSTLMap <_PLAYER_KILLING_ZONE_RANKING>		UserPlayerKillingZoneRankingArray;

typedef std::map<std::string, _USER_DAILY_OP *>		UserDailyOpMap; 

typedef CSTLMap <_MINING_FISHING_ITEM>				MiningFishingItemArray;
typedef CSTLMap <_UNDER_THE_CASTLE_INFO>			MonsterUnderTheCastleList;
typedef CSTLMap <_JURAID_ROOM_INFO>					TempleEventJuraidRoomList;
typedef CSTLMap <_BDW_ROOM_INFO>					TempleEventBDWRoomList;
typedef CSTLMap <_CHAOS_ROOM_INFO>					TempleEventChaosRoomList;
typedef CSTLMap <_TEMPLE_EVENT_USER>				TempleEventUserMap;
typedef CSTLMap <_TEMPLE_SOCCER_EVENT_USER>			TempleSoccerEventUserArray;
typedef CSTLMap <_TEMPLE_QUEST_USER>				TempleMonsterStoneTeleportArray;
typedef CSTLMap <_KNIGHTS_SIEGE_WARFARE>			KnightsSiegeWarfareArray;
typedef CSTLMap <_EVENT_TRIGGER>					EventTriggerArray;
typedef CSTLMap <_MONUMENT_INFORMATION>				NationMonumentInformationArray;
typedef CSTLMap <_MONSTER_CHALLENGE>				MonsterChallengeArray;
typedef CSTLMap <_MONSTER_CHALLENGE_SUMMON_LIST>	MonsterChallengeSummonListArray;
typedef CSTLMap <_START_POSITION_RANDOM>			StartPositionRandomArray;
typedef CSTLMap <_KN_START_POSITION_RANDOM>			KnStartPositionRandomArray;
typedef CSTLMap <_USER_ITEM>						UserItemArray;
typedef CSTLMap <_OBJECT_EVENT>						ObjectEventArray;
typedef CSTLMap <_CHAT_ROOM>						ChatRoomArray;
typedef CSTLMap <_PPCARD>							PPCardArray;
typedef CSTLMap <_CHARACTER_SEAL_ITEM, uint64> 		CharacterSealItemArray;
typedef CSTLMap <_CHARACTER_SEAL_ITEM_MAPPING> 		CharacterSealItemMapping;
typedef CSTLMap <_SERVER_SETTINGS>					ServerSettingsArray;
typedef CSTLMap <_MINING_EXCHANGE>					MiningExchangeArray;
typedef CSTLMap <_DKM_MONSTER_DEAD_GIFT>			DkmMonsterDeadGiftArray;
typedef CSTLMap <_GIVE_LUA_ITEM_EXCHANGE>			LuaGiveItemExchangeArray;
typedef CSTLMap <_EVENT_SCHEDULE>					EventScheduleArray;

typedef CSTLMap <_ROOM_EVENT_PLAY_TIMER>			RoomEventPlayTimerArray;
typedef CSTLMap <_ROYALE_EVENT_PLAY_TIMER>			RoyaleEventPlayTimerArray;
typedef CSTLMap <_CSW_EVENT_PLAY_TIMER>				CswEventPlayTimerArray;

typedef CSTLMap <_DEATHMATCH_WAR_INFO>				KnightSiegeWarClanList;
typedef CSTLMap <_DEATHMATCH_BARRACK_INFO>			KnightSiegeWarBarrackClanList;
typedef CSTLMap <_CASTELLAN_WAR_INFO>				KnightSiegeWarCastellanClanList;
typedef CSTLMap <_CASTELLAN_OLD_CSW_WINNER>			KnightSiegeWarCastellanOldWinner;
typedef CSTLMap <USER_HACKTOOL_LIST>				HackToolList;
typedef CSTLMap <DRAKI_ROOM_LIST>					DrakiRoomList;
typedef CSTLMap <DRAKI_MONSTER_LIST>				DrakiMonsterList;
typedef CSTLMap <DRAKI_ROOM_RANK>					DrakiRankList;
typedef CSTLMap <_DRAKI_TOWER_INFO>					MonsterDrakiTowerList;
typedef CSTLMap <_TOURNAMENT_DATA>					ClanVsDataList;
typedef CSTLMap <_DUNGEON_DEFENCE_MONSTER_LIST>		DungeonDefenceMonsterListArray;
typedef CSTLMap <_DUNGEON_DEFENCE_ROOM_INFO>		DungeonDefenceRoomListArray;
typedef CSTLMap <_DUNGEON_DEFENCE_STAGE_LIST>		DungeonDefenceStageListArray;
typedef CSTLMap < _KNIGHT_ROYALE_INFO>				KnightRoyaleRoomInfo;
typedef CSTLMap < _KNIGHT_ROYALE_EVENT_USER>		KnightRoyaleEventUserMap;
typedef CSTLMap <_KNIGHT_ROYALE_STATS_LIST>			KnightRoyaleStatsListArray;
typedef CSTLMap <_KNIGHT_ROYALE_BEGINNER_ITEM>		KnightRoyaleBeginnerItemListArray;
typedef CSTLMap <_KNIGHT_ROYALE_TREASURE_CHEST_LIST> KnightRoyaleTreasureChestListArray;
typedef CSTLMap <_KNIGHT_RETURN_GIFT_ITEM>			KnightReturnLetterGiftListArray;
typedef CSTLMap <_KNIGHTS_CASTELLAN_CAPE>			KnightCastellanCapeArray;
typedef CSTLMap <_BANISH_OF_WINNER>					WarBanishOfWinnerArray;
typedef CSTLMap <_LOAD_QUEST_SKILL>					LoadOpenQuestArray;
typedef CSTLMap <_MONSTER_DROP_MAIN>				MonsterDropMainArray;
typedef CSTLMap <_MONSTER_GROUP_ITEM_LIST>			MonsterItemGroupItemListArray;

typedef std::vector<_RANDOM_ITEM*>					UserGiftArray;

//Pet System
typedef std::map<uint64, PET_DATA*>					PetDataSystemArray;
typedef CSTLMap <PET_INFO>							PetInfoSystemArray;
typedef CSTLMap <PET_TRANSFORM>						PetTransformSystemArray;

// Settings
typedef CSTLMap <_HOOK_SETTINGS>					SettingsArray;
typedef CSTLMap <_HOOK_PUS_ITEM>					PusItemArray;
typedef CSTLMap <COLLECTIONRACE_SETTINGS>			CollectionRaceSettingsArray;
typedef CSTLMap <COLLECTIONRACE_HUNT_LIST>			CollectionRaceHuntListArray;