#include "stdafx.h"
#include "UIImageTooltipDlg.h"

#pragma once

const int UIITEM_TYPE_ONLYONE = 0;
const int UIITEM_TYPE_COUNTABLE = 1;
const int UITEIM_TYPE_GOLD = 2;
const int UIITEM_TYPE_COUNTABLE_SMALL = 3;
const int UIITEM_TYPE_SOMOONE = 4;

const int UIITEM_TYPE_SONGPYUN_ID_MIN = 490000;

const int UIITEM_COUNT_MANY = 9999;
const int UIITEM_COUNT_FEW = 500;

#define IDS_BIND_POINT_FAILED           1001
#define IDS_BIND_POINT_REQUEST_FAIL     1002
#define IDS_BIND_POINT_SUCCESS          1003
#define IDS_CHAT_SELECT_TARGET_FAIL     1101
#define IDS_CHAT_SELECT_TARGET_SUCCESS  1102
#define IDS_CHR_SELECT_FMT_INFO         1201
#define IDS_CHR_SELECT_HINT             1202
#define IDS_CLASS_CHANGE_ALREADY        1301
#define IDS_CLASS_CHANGE_NOT_YET        1302
#define IDS_CLASS_CHANGE_SUCCESS        1303
#define IDS_CLASS_KINDOF_WARRIOR        1304
#define IDS_CLASS_KINDOF_ROGUE          1305
#define IDS_CLASS_KINDOF_WIZARD         1306
#define IDS_CLASS_KINDOF_PRIEST         1307
#define IDS_CLASS_KINDOF_ATTACK_WARRIOR 1308
#define IDS_CLASS_KINDOF_DEFEND_WARRIOR 1309
#define IDS_CLASS_KINDOF_ARCHER         1310
#define IDS_CLASS_KINDOF_ASSASSIN       1311
#define IDS_CLASS_KINDOF_ATTACK_WIZARD  1312
#define IDS_CLASS_KINDOF_PET_WIZARD     1313
#define IDS_CLASS_KINDOF_HEAL_PRIEST    1314
#define IDS_CLASS_KINDOF_CURSE_PRIEST   1315
#define IDS_CLASS_EL_ASSASIN            1401
#define IDS_CLASS_EL_BLADE              1402
#define IDS_CLASS_EL_CLERIC             1403
#define IDS_CLASS_EL_DRUID              1404
#define IDS_CLASS_EL_ENCHANTER          1405
#define IDS_CLASS_EL_MAGE               1406
#define IDS_CLASS_EL_PROTECTOR          1407
#define IDS_CLASS_EL_RANGER             1408
#define IDS_CLASS_KA_BERSERKER          1409
#define IDS_CLASS_KA_DARKPRIEST         1410
#define IDS_CLASS_KA_GUARDIAN           1411
#define IDS_CLASS_KA_HUNTER             1412
#define IDS_CLASS_KA_NECROMANCER        1413
#define IDS_CLASS_KA_PENETRATOR         1414
#define IDS_CLASS_KA_SHAMAN             1415
#define IDS_CLASS_KA_SORCERER           1416
#define IDS_CLASS_PRIEST                1417
#define IDS_CLASS_ROGUE                 1418
#define IDS_CLASS_UNKNOWN               1419
#define IDS_CLASS_WARRIOR               1420
#define IDS_CLASS_WIZARD                1421
#define IDS_CONFIRM_DELETE_CHR          1501
#define IDS_CONFIRM_EXIT_GAME           1502
#define IDS_CONNECTION_CLOSED           1601
#define IDS_CONNECT_IP                  1602
#define IDS_COUNTABLE_ITEM_BUY_FAIL     1701
#define IDS_MANY_COUNTABLE_ITEM_BUY_FAIL 1701
#define IDS_COUNTABLE_ITEM_BUY_NOT_ENOUGH_MONEY 1702
#define IDS_COUNTABLE_ITEM_GET_MANY     1703
#define IDS_MANY_COUNTABLE_ITEM_GET_MANY 1703
#define IDS_COUNTABLE_ITEM_TOO_MAMY     1704
#define IDS_MANY_COUNTABLE_ITEM_TOO_MAMY 1704
#define IDS_SMALL_COUNTABLE_ITEM_BUY_FAIL 1705
#define IDS_SMALL_COUNTABLE_ITEM_GET_MANY 1706
#define IDS_SMALL_COUNTABLE_ITEM_TOO_MAMY 1707
#define IDS_ITEM_TOOMANY_OR_HEAVY       1708
#define IDS_DOOR_CLOSED                 1801
#define IDS_DOOR_OPENED                 1802
#define IDS_ERR_CHARACTER_CREATE        1901
#define IDS_ERR_DB_CREATE               1902
#define IDS_ERR_GOTO_TOWN_OUT_OF_HP     1903
#define IDS_ERR_INVALID_CLASS           1904
#define IDS_ERR_INVALID_NAME            1905
#define IDS_ERR_INVALID_NAME_HAS_SPECIAL_LETTER 1906
#define IDS_ERR_INVALID_NATION_RACE     1907
#define IDS_ERR_INVALID_RACE            1908
#define IDS_ERR_KNIGHTS_CREATE_FAILED_NAME_EMPTY 1909
#define IDS_ERR_NOT_SUPPORTED_RACE      1910
#define IDS_ERR_NO_MORE_CHARACTER       1911
#define IDS_ERR_OVERLAPPED_ID           1912
#define IDS_ERR_REMAIN_BONUS_POINT      1913
#define IDS_ERR_REQUEST_OBJECT_EVENT_SO_FAR 1914
#define IDS_ERR_UNKNOWN                 1915
#define IDS_EXIT                        2001
#define IDS_FMT_CONCURRENT_USER_COUNT   2101
#define IDS_FONT_BALLOON                2201
#define IDS_FONT_ID                     2202
#define IDS_FONT_INFO                   2203
#define IDS_INV_ITEM_FULL               2301
#define IDS_ITEM_ATTRIB_CRAFT           2401
#define IDS_ITEM_ATTRIB_GENERAL         2402
#define IDS_ITEM_ATTRIB_LAIR            2403
#define IDS_ITEM_ATTRIB_MAGIC           2404
#define IDS_ITEM_ATTRIB_UNIQUE          2405
#define IDS_ITEM_ATTRIB_UPGRADE         2406
#define IDS_ITEM_CLASS_AMULET           2501
#define IDS_ITEM_CLASS_ARMOR_MAGE       2502
#define IDS_ITEM_CLASS_ARMOR_PRIEST     2503
#define IDS_ITEM_CLASS_ARMOR_ROGUE      2504
#define IDS_ITEM_CLASS_ARMOR_WARRIOR    2505
#define IDS_ITEM_CLASS_ARROW            2506
#define IDS_ITEM_CLASS_AXE              2507
#define IDS_ITEM_CLASS_AXE_2H           2508
#define IDS_ITEM_CLASS_BELT             2509
#define IDS_ITEM_CLASS_BOW              2510
#define IDS_ITEM_CLASS_BOW_CROSS        2511
#define IDS_ITEM_CLASS_BOW_LONG         2512
#define IDS_ITEM_CLASS_CHARM            2513
#define IDS_ITEM_CLASS_DAGGER           2514
#define IDS_ITEM_CLASS_EARRING          2515
#define IDS_ITEM_CLASS_ETC              2516
#define IDS_ITEM_CLASS_JAVELIN          2517
#define IDS_ITEM_CLASS_JEWEL            2518
#define IDS_ITEM_CLASS_LAUNCHER         2519
#define IDS_ITEM_CLASS_MACE             2520
#define IDS_ITEM_CLASS_MACE_2H          2521
#define IDS_ITEM_CLASS_POLEARM          2522
#define IDS_ITEM_CLASS_POTION           2523
#define IDS_ITEM_CLASS_RING             2524
#define IDS_ITEM_CLASS_SCROLL           2525
#define IDS_ITEM_CLASS_SHIELD           2526
#define IDS_ITEM_CLASS_SPEAR            2527
#define IDS_ITEM_CLASS_STAFF            2528
#define IDS_ITEM_CLASS_SWORD            2529
#define IDS_ITEM_CLASS_SWORD_2H         2530
#define IDS_ITEM_WEIGHT_OVERFLOW        2601
#define IDS_KNIGHTS_ADMIT_FAILED        2701
#define IDS_KNIGHTS_ADMIT_SUCCESS       2702
#define IDS_KNIGHTS_APPOINT_CHIEF_FAILED 2703
#define IDS_KNIGHTS_APPOINT_CHIEF_SUCCESS 2704
#define IDS_KNIGHTS_APPOINT_OFFICER_FAILED 2705
#define IDS_KNIGHTS_APPOINT_OFFICER_SUCCESS 2706
#define IDS_KNIGHTS_APPOINT_VICECHIEF_FAILED 2707
#define IDS_KNIGHTS_APPOINT_VICECHIEF_SUCCESS 2708
#define IDS_KNIGHTS_CREATE_FAILED       2709
#define IDS_KNIGHTS_CREATE_SUCCESS      2710
#define IDS_KNIGHTS_DESTROY_CONFIRM     2711
#define IDS_KNIGHTS_DESTROY_FAILED      2712
#define IDS_KNIGHTS_DESTROY_SUCCESS     2713
#define IDS_KNIGHTS_DUTY_CHIEF          2714
#define IDS_KNIGHTS_DUTY_KNIGHT         2715
#define IDS_KNIGHTS_DUTY_OFFICER        2716
#define IDS_KNIGHTS_DUTY_PUNISH         2717
#define IDS_KNIGHTS_DUTY_TRAINEE        2718
#define IDS_KNIGHTS_DUTY_UNKNOWN        2719
#define IDS_KNIGHTS_DUTY_VICECHIEF      2720
#define IDS_KNIGHTS_JOIN_FAILED         2721
#define IDS_KNIGHTS_JOIN_SUCCESS        2722
#define IDS_KNIGHTS_PUNISH_FAILED       2723
#define IDS_KNIGHTS_PUNISH_SUCCESS      2724
#define IDS_KNIGHTS_REJECT_FAILED       2725
#define IDS_KNIGHTS_REJECT_SUCCESS      2726
#define IDS_KNIGHTS_REMOVE_MEMBER_FAILED 2727
#define IDS_KNIGHTS_REMOVE_MEMBER_SUCCESS 2728
#define IDS_KNIGHTS_WITHDRAW_CONFIRM    2729
#define IDS_KNIGHTS_WITHROW_FAILED      2730
#define IDS_KNIGHTS_WITHROW_SUCCESS     2731
#define IDS_LEVER_ACTIVATE              2801
#define IDS_LEVER_DEACTIVATE            2802
#define IDS_LOGIN_FAILED                2901
#define IDS_LOGIN_ERR_ALREADY_CONNECTED_ACCOUNT 2902
#define IDS_NOACCOUNT_RETRY_MGAMEID     2903
#define IDS_NO_MGAME_ACCOUNT            2904
#define IDS_FMT_CONNECT_ERROR           2905
#define IDS_FMT_GAME_SERVER_LOGIN_ERROR 2906
#define IDS_SERVER_CONNECT_FAIL         2907
#define IDS_CURRENT_SERVER_ERROR        2908
#define IDS_CONNECT_FAIL                2909
#define IDS_MSG_ATTACK_DISABLE          3001
#define IDS_MSG_ATTACK_START            3002
#define IDS_MSG_ATTACK_STOP             3003
#define IDS_MSG_CASTING_FAIL_LACK_MP    3004
#define IDS_MSG_CASTING_FAIL_LOW_MP     3005
#define IDS_MSG_FAIL_LOW_SP             3006
#define IDS_MSG_FMT_EXP_GET             3007
#define IDS_MSG_FMT_EXP_LOST            3008
#define IDS_MSG_FMT_HP_LOST             3009
#define IDS_MSG_FMT_HP_RECOVER          3010
#define IDS_MSG_FMT_MP_RECOVER          3011
#define IDS_MSG_FMT_MP_USE              3012
#define IDS_MSG_FMT_SP_RECOVER          3013
#define IDS_MSG_FMT_SP_USE              3014
#define IDS_MSG_FMT_TARGET_ATTACK_FAILED 3015
#define IDS_MSG_FMT_TARGET_HP_LOST      3016
#define IDS_MSG_FMT_TARGET_HP_RECOVER   3017
#define IDS_MSG_VALID_CLASSNRACE_INVALID_RACE 3018
#define IDS_MSG_VALID_CLASSNRACE_LOW_CHA 3019
#define IDS_MSG_VALID_CLASSNRACE_LOW_DEX 3020
#define IDS_MSG_VALID_CLASSNRACE_LOW_INT 3021
#define IDS_MSG_VALID_CLASSNRACE_LOW_LEVEL 3022
#define IDS_MSG_VALID_CLASSNRACE_LOW_POWER 3023
#define IDS_MSG_VALID_CLASSNRACE_LOW_RANK 3024
#define IDS_MSG_VALID_CLASSNRACE_LOW_STR 3025
#define IDS_MSG_VALID_CLASSNRACE_LOW_TITLE 3026
#define IDS_MSG_CONCURRENT_USER_OVERFLOW 3027
#define IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS 3028
#define IDS_NATION_ELMORAD              3101
#define IDS_NATION_KARUS                3102
#define IDS_NATION_UNKNOWN              3103
#define IDS_NEWCHR_MAP                  3201
#define IDS_NEWCHR_DEX                  3202
#define IDS_NEWCHR_EL_BABA              3203
#define IDS_NEWCHR_EL_FEMALE            3204
#define IDS_NEWCHR_EL_MAGE              3205
#define IDS_NEWCHR_EL_MALE              3206
#define IDS_NEWCHR_EL_PRIEST            3207
#define IDS_NEWCHR_EL_ROGUE             3208
#define IDS_NEWCHR_EL_WARRIOR           3209
#define IDS_NEWCHR_INT                  3210
#define IDS_NEWCHR_KA_ARKTUAREK         3211
#define IDS_NEWCHR_KA_MAGE              3212
#define IDS_NEWCHR_KA_PRIEST            3213
#define IDS_NEWCHR_KA_PURITUAREK        3214
#define IDS_NEWCHR_KA_ROGUE             3215
#define IDS_NEWCHR_KA_TUAREK            3216
#define IDS_NEWCHR_KA_WARRIOR           3217
#define IDS_NEWCHR_KA_WRINKLETUAREK     3218
#define IDS_NEWCHR_POW                  3219
#define IDS_NEWCHR_STA                  3220
#define IDS_NPCEVENT_TITLE_REPAIR       3301
#define IDS_NPC_EVENT_TITLE_TRADE       3302
#define IDS_PARTY_CONFIRM_DESTROY       3401
#define IDS_PARTY_CONFIRM_DISCHARGE     3402
#define IDS_PARTY_CONFIRM_LEAVE         3403
#define IDS_PARTY_DESTROY               3404
#define IDS_PARTY_INSERT                3405
#define IDS_PARTY_INSERT_ERR            3406
#define IDS_PARTY_INSERT_ERR_INVALID_NATION 3407
#define IDS_PARTY_INSERT_ERR_LEVEL_DIFFERENCE 3408
#define IDS_PARTY_INSERT_ERR_REJECTED   3409
#define IDS_PARTY_INSERT_FAILED         3410
#define IDS_PARTY_INVITE                3411
#define IDS_PARTY_INVITE_FAILED         3412
#define IDS_PARTY_ITEM_GET              3413
#define IDS_PARTY_LEAVE                 3414
#define IDS_PARTY_PERMIT                3415
#define IDS_PERSONAL_TRADE_FMT_WAIT     3501
#define IDS_PERSONAL_TRADE_PERMIT       3502
#define IDS_PERSONAL_TRADE_REQUEST      3503
#define IDS_PER_TRADEING_OTHER          3504
#define IDS_RACE_ALL                    3601
#define IDS_RACE_EL_BABARIAN            3602
#define IDS_RACE_EL_MAN                 3603
#define IDS_RACE_EL_WOMEN               3604
#define IDS_RACE_KA_ARKTUAREK           3605
#define IDS_RACE_KA_PURITUAREK          3606
#define IDS_RACE_KA_TUAREK              3607
#define IDS_RACE_KA_WRINKLETUAREK       3608
#define IDS_RACE_UNKNOWN                3609
#define IDS_REGENERATION                3701
#define IDS_REPAIR_LACK_GOLD            3801
#define IDS_REQUEST_BINDPOINT           3901
#define IDS_SKILL_ATTACK_FAIL_SOFAR     4001
#define IDS_SKILL_FAIL_CASTING          4002
#define IDS_SKILL_FAIL_DIFFURENTCLASS   4003
#define IDS_SKILL_FAIL_EFFECTING        4004
#define IDS_SKILL_FAIL_INVALID_ITEM     4005
#define IDS_SKILL_FAIL_LACK_HP          4006
#define IDS_SKILL_FAIL_LACK_ITEM        4007
#define IDS_SKILL_FAIL_LACK_SP          4008
#define IDS_SKILL_FAIL_SOFAR            4009
#define IDS_SKILL_HEALING_FAIL_SOFAR    4010
#define IDS_SKILL_FAIL_OBJECT_BLOCK     4010
#define IDS_SKILL_POINT_BEFORE_CLASS_CHANGE 4101
#define IDS_SKILL_POINT_EXTRA_NOT_EXIST 4102
#define IDS_SKILL_POINT_NOT_YET         4103
#define IDS_SKILL_SUCCESS_HEALING_FROM  4201
#define IDS_SKILL_SUCCESS_HEALING_TO    4202
#define IDS_SKILL_TOOLTIP_DOUBLE        4301
#define IDS_SKILL_TOOLTIP_NEED_ITEM_DUAL 4302
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID1 4303
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID10 4304
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID11 4305
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID12 4306
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID13 4307
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID14 4308
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID15 4309
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID16 4310
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID17 4311
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID2 4312
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID3 4313
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID4 4314
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID5 4315
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID6 4316
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID7 4317
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID8 4318
#define IDS_SKILL_TOOLTIP_NEED_ITEM_ID9 4319
#define IDS_SKILL_TOOLTIP_NO_MANA       4320
#define IDS_SKILL_TOOLTIP_USE_ITEM_EXIST 4321
#define IDS_SKILL_TOOLTIP_USE_ITEM_NO   4322
#define IDS_SKILL_TOOLTIP_USE_MANA      4323
#define IDS_SKILL_UP_INVALID            4401
#define IDS_SKILL_USE                   4402
#define IDS_TOOLTIP_ATTACK              4501
#define IDS_TOOLTIP_ATTACKINT_FAST      4502
#define IDS_TOOLTIP_ATTACKINT_NORMAL    4503
#define IDS_TOOLTIP_ATTACKINT_SLOW      4504
#define IDS_TOOLTIP_ATTACKINT_VERYFAST  4505
#define IDS_TOOLTIP_ATTACKINT_VERYSLOW  4506
#define IDS_TOOLTIP_ATTACKRANGE         4507
#define IDS_TOOLTIP_ATTRMAGIC1          4508
#define IDS_TOOLTIP_ATTRMAGIC2          4509
#define IDS_TOOLTIP_ATTRMAGIC3          4510
#define IDS_TOOLTIP_ATTRMAGIC4          4511
#define IDS_TOOLTIP_ATTRMAGIC5          4512
#define IDS_TOOLTIP_ATTRMAGIC6          4513
#define IDS_TOOLTIP_ATTRMAGIC7          4514
#define IDS_TOOLTIP_AVOIDRATE_OVER      4515
#define IDS_TOOLTIP_AVOIDRATE_UNDER     4516
#define IDS_TOOLTIP_BONUSMAGICATTACK    4517
#define IDS_TOOLTIP_BONUSDEX            4518
#define IDS_TOOLTIP_BONUSHP             4519
#define IDS_TOOLTIP_BONUSINT            4520
#define IDS_TOOLTIP_BONUSSTR            4521
#define IDS_TOOLTIP_BONUSWIZ            4522
#define IDS_TOOLTIP_BUY_PRICE           4523
#define IDS_TOOLTIP_CUR_DURABILITY      4524
#define IDS_TOOLTIP_DAMAGE              4525
#define IDS_TOOLTIP_DEFENSE             4526
#define IDS_TOOLTIP_DEFENSE_RATE_ARROW  4527
#define IDS_TOOLTIP_DEFENSE_RATE_AXE    4528
#define IDS_TOOLTIP_DEFENSE_RATE_BLOW   4529
#define IDS_TOOLTIP_DEFENSE_RATE_DAGGER 4530
#define IDS_TOOLTIP_DEFENSE_RATE_SPEAR  4531
#define IDS_TOOLTIP_DEFENSE_RATE_SWORD  4532
#define IDS_TOOLTIP_GOLD                4533
#define IDS_TOOLTIP_HITRATE_OVER        4534
#define IDS_TOOLTIP_HITRATE_UNDER       4535
#define IDS_TOOLTIP_MAX_DURABILITY      4536
#define IDS_TOOLTIP_NEEDMAGICATTACK     4537
#define IDS_TOOLTIP_NEEDDEXTERITY       4538
#define IDS_TOOLTIP_NEEDHEALTH          4539
#define IDS_TOOLTIP_NEEDINTELLI         4540
#define IDS_TOOLTIP_NEEDLEVEL           4541
#define IDS_TOOLTIP_NEEDRANK            4542
#define IDS_TOOLTIP_NEEDSTAMINA         4543
#define IDS_TOOLTIP_NEEDSTRENGTH        4544
#define IDS_TOOLTIP_NEEDTITLE           4545
#define IDS_TOOLTIP_REGISTCURSE         4546
#define IDS_TOOLTIP_REGISTELEC          4547
#define IDS_TOOLTIP_REGISTFIRE          4548
#define IDS_TOOLTIP_REGISTICE           4549
#define IDS_TOOLTIP_REGISTMAGIC         4550
#define IDS_TOOLTIP_REGISTPOISON        4551
#define IDS_TOOLTIP_SELL_PRICE          4552
#define IDS_TOOLTIP_WEIGHT              4553
#define IDS_TOOLTIP_REDUCE				4554
#define IDS_TOOLTIP_REPEL_PHYSICAL		4555
#define IDS_TOOLTIP_RENTAL_TIME			4556
#define IDS_TOOLTIP_NEEDLEVEL_RANGE		4558
#define IDS_TOOLTIP_NEEDTITLE2			4559
#define IDS_TOOLTIP_GRADE				4560
#define IDS_TOOLTIP_UNIQUE				4561
#define IDS_TOOLTIP_LOW_CLASS			4562
#define IDS_TOOLTIP_MIDDLE_CLASS		4563
#define IDS_TOOLTIP_HIGH_CLASS			4564
#define IDS_TRY_SPEED_HACKING           4601
#define IDS_URL_JOIN                    4701
#define IDS_VERSION_CONFIRM             4801
#define IDS_TOOLTIP_BONUSSTA            4802
#define IDS_VERSION_CONFIRM_TW          4803
#define ID_DEBUG0                       4901
#define IDS_HELP_TIP1                   5001
#define IDS_HELP_TIP2                   5002
#define IDS_HELP_TIP3                   5003
#define IDS_HELP_TIP4                   5004
#define IDS_HELP_TIP5                   5005
#define IDS_HELP_TIP6                   5006
#define IDS_HELP_TIP7                   5007
#define IDS_HELP_TIP8                   5008
#define IDS_HELP_TIP9                   5009
#define IDS_HELP_TIP10                  5010
#define IDS_HELP_TIP11                  5011
#define IDS_HELP_TIP12                  5012
#define IDS_HELP_TIP13                  5013
#define IDS_HELP_TIP14                  5014
#define IDS_HELP_TIP15                  5015
#define IDS_HELP_TIP16                  5016
#define IDS_HELP_TIP17                  5017
#define IDS_HELP_TIP18                  5018
#define IDS_HELP_TIP19                  5019
#define IDS_HELP_TIP20                  5020
#define IDS_HELP_TIP21                  5021
#define IDS_HELP_TIP22                  5022
#define IDS_HELP_TIP23                  5023
#define IDS_HELP_TIP24                  5024
#define IDS_HELP_TIP25                  5025
#define IDS_HELP_TIP26                  5026
#define IDS_HELP_TIP27                  5027
#define IDS_HELP_TIP28                  5028
#define IDS_HELP_TIP29                  5029
#define IDS_HELP_TIP30                  5030
#define IDS_HELP_TIP31                  5031
#define IDS_HELP_TIP32                  5032
#define IDS_HELP_TIP33                  5033
#define IDS_HELP_TIP_ALL                5034
#define IDS_EDIT_BOX_GOLD               6033
#define IDS_EDIT_BOX_COUNT              6034
#define IDS_SKILL_INFO_BASE             6035
#define IDS_SKILL_INFO_BLADE0           6036
#define IDS_SKILL_INFO_BLADE1           6037
#define IDS_SKILL_INFO_BLADE2           6038
#define IDS_SKILL_INFO_BLADE3           6039
#define IDS_SKILL_INFO_RANGER0          6044
#define IDS_SKILL_INFO_RANGER1          6045
#define IDS_SKILL_INFO_RANGER2          6046
#define IDS_SKILL_INFO_RANGER3          6047
#define IDS_SKILL_INFO_MAGE0            6052
#define IDS_SKILL_INFO_MAGE1            6053
#define IDS_SKILL_INFO_MAGE2            6054
#define IDS_SKILL_INFO_MAGE3            6055
#define IDS_SKILL_INFO_CLERIC0          6060
#define IDS_SKILL_INFO_CLERIC1          6061
#define IDS_SKILL_INFO_CLERIC2          6062
#define IDS_SKILL_INFO_CLERIC3          6063
#define IDS_SKILL_INFO_SHAMAN3          6064
#define IDS_ERR_CHARACTER_SELECT        6066
#define IDS_DROPPED_NOAH_GET            6067
#define IDS_DURABILITY_EXOAST           6068
#define IDS_ITEM_GET_BY_RULE            6069
#define IDS_OTHER_PER_TRADE_NO          6070
#define IDS_PER_TRADE_FAIL              6071
#define IDS_OTHER_PER_TRADE_CANCEL      6072
#define IDS_ITEM_KIND_BRONS             6073
#define IDS_ITEM_KIND_SILVER            6074
#define IDS_ITEM_KIND_GOLDEN            6075
#define IDS_ITEM_KIND_PLATINUM          6076
#define IDS_ITEM_KIND_CRIMSON           6077
#define IDS_ITEM_KIND_LUNA              6078
#define IDS_ITEM_KIND_SOLAR             6079
#define IDS_ITEM_KIND_ANCIENT           6080
#define IDS_ITEM_KIND_MISTIQ            6081
#define IDS_TOOLTIP_NOAH                6082
#define IDS_TOOLTIP_REPAIR_PRICE        6084
#define IDS_TOOLTIP_CANNOT              6085
#define IDS_POINTINIT_NOT_ENOUGH_NOAH   6086
#define IDS_POINTINIT_ALREADY           6087
#define IDS_NOAH_CHANGE_GET             6088
#define IDS_NOAH_CHANGE_LOST            6089
#define IDS_NOAH_CHANGE_SPEND           6099
#define IDS_LOYALTY_CHANGE_GET			6100
#define IDS_MANNER_CHANGE_GET			6101
#define IDS_BEGINNER_HELPER_30			6102
#define IDS_LADDER_CHANGE_GET			6103
#define IDS_LOYALTY_CHANGE_LOST			6104
#define IDS_LADDER_CHANGE_LOST			6105
#define IDS_MSG_HASITEMINSLOT           6112
#define IDS_ERR_REQUEST_NPC_EVENT_SO_FAR 6116
#define IDS_WRONG_PASSWORD              6120
#define IDS_WANT_PARTY_MEMBER           6123
#define IDS_SETTING_KARUS_SCREEN        6124
#define IDS_SETTING_ELMORAD_SCREEN      6125
#define IDS_PRIVATE_CMD_CAT				7800
#define IDS_TRADE_CMD_CAT				7801
#define IDS_PARTY_CMD_CAT				7802	
#define IDS_CLAN_CMD_CAT				7803
#define IDS_KNIGHTS_CMD_CAT				7804
#define IDS_GUARDIAN_MON_CMD_CAT		7805
#define IDS_KING_CMD_CAT				7806
#define IDS_GM_CMD_CAT					7807
#define IDS_PRIVATE_TIP					7900
#define IDS_TRADE_TIP					7901
#define IDS_PARTY_TIP					7902
#define IDS_CLAN_TIP					7903
#define IDS_KNIGHTS_TIP					7904
#define IDS_GUARDIAN_MON_TIP			7905
#define IDS_KING_TIP					7906
#define IDS_GM_TIP						7906
#define IDS_PRIVATE_PM_CMD				8000
#define IDS_PRIVATE_TWN_CMD				8001
#define IDS_PRIVATE_EXIT_CMD			8002
#define IDS_PRIVATE_GREET_CMD			8003
#define IDS_PRIVATE_GREET2_CMD			8004
#define IDS_PRIVATE_GREET3_CMD			8005
#define IDS_PRIVATE_PROVOKE_CMD			8006
#define IDS_PRIVATE_PROVOKE2_CMD		8007
#define IDS_PRIVATE_PROVOKE3_CMD		8008
#define IDS_PRIVATE_SAVE_CMD			8009
#define IDS_PRIVATE_BATTLE_CMD			8010
#define IDS_ITEM_RECEIVED				7613
#define IDS_TRADE_COIN_RECV				7682
#define IDS_TRADE_COIN_PAID				7683
#define IDS_CMD_WHISPER                 8000
#define IDS_CMD_TOWN                    8001
#define IDS_CMD_EXIT                    8002
#define IDS_CMD_GREETING                8003
#define IDS_CMD_GREETING2               8004
#define IDS_CMD_GREETING3               8005
#define IDS_CMD_PROVOKE                 8006
#define IDS_CMD_PROVOKE2                8007
#define IDS_CMD_PROVOKE3                8008
#define IDS_CMD_GAME_SAVE               8009
#define IDS_CMD_RECOMMEND               8010
#define IDS_CMD_INDIVIDUAL_BATTLE       8011
#define IDS_CMD_SIT_STAND		        8012
#define IDS_CMD_WALK_RUN		        8013
#define IDS_CMD_LOCATION		        8014
#define IDS_CMD_TRADE                   8200
#define IDS_CMD_FORBIDTRADE             8201
#define IDS_CMD_PERMITTRADE             8202
#define IDS_CMD_MERCHANT	            8203
#define IDS_CMD_PARTY                   8400
#define IDS_CMD_LEAVEPARTY              8401
#define IDS_CMD_RECRUITPARTY            8402
#define IDS_CMD_FORBIDPARTY             8403
#define IDS_CMD_PERMITPARTY             8404
#define IDS_CMD_JOINCLAN                8600
#define IDS_CMD_WITHDRAWCLAN            8601
#define IDS_CMD_FIRECLAN                8602
#define IDS_CMD_COMMAND					8603
#define IDS_CMD_CLAN_WAR				8604
#define IDS_CMD_SURRENDER				8605
#define IDS_CMD_APPOINTVICECHIEF        8606
#define IDS_CMD_CLAN_CHAT		        8607
#define IDS_CMD_CLAN_BATTLE		        8608
#define IDS_CMD_CONFEDERACY				8800
#define IDS_CMD_BAN_KNIGHTS				8801
#define IDS_CMD_QUIT_KNIGHTS			8802
#define IDS_CMD_BASE					8803
#define IDS_CMD_DECLARATION				8804
#define IDS_CMD_VISIBLE                 9000
#define IDS_CMD_INVISIBLE               9001
#define IDS_CMD_CLEAN                   9002
#define IDS_CMD_RAINING                 9003
#define IDS_CMD_SNOWING                 9004
#define IDS_CMD_TIME                    9005
#define IDS_CMD_CU_COUNT                9006
#define IDS_CMD_NOTICE                  9007
#define IDS_CMD_ARREST                  9008
#define IDS_CMD_FORBIDCONNECT           9009
#define IDS_CMD_FORBIDCHAT              9010
#define IDS_CMD_PERMITCHAT              9011
#define IDS_CMD_NOTICEALL				9012
#define IDS_CMD_CUTOFF					9013
#define IDS_CMD_VIEW					9014
#define IDS_CMD_UNVIEW					9015
#define IDS_CMD_FORBIDUSER				9016
#define IDS_CMD_SUMMONUSER				9017
#define IDS_CMD_ATTACKDISABLE			9018
#define IDS_CMD_ATTACKENABLE			9019
#define IDS_CMD_PLC						9020
#define IDS_PARTY_BBS_REGISTER          6300
#define IDS_PARTY_BBS_REGISTER_CANCEL   6301
#define IDS_TRADE_BBS_SELL_REGISTER     6302
#define IDS_TRADE_BBS_PER_TRADE         6303
#define IDS_TRADE_BBS_BUY_REGISTER      6304
#define IDS_TRADE_BBS_FAIL1             6305
#define IDS_TRADE_BBS_FAIL2             6306
#define IDS_TRADE_BBS_FAIL3             6307
#define IDS_TRADE_BBS_FAIL4             6308
#define IDS_TRADE_BBS_FAIL5             6309
#define IDS_TRADE_BBS_FAIL6             6310
#define IDS_OTHER_PER_TRADE_ID_NO       6311
#define IDS_CLAN_DENY_LOWLEVEL          6500
#define IDS_CLAN_DENY_LOWGOLD           6501
#define IDS_CLAN_DENY_INVALIDDAY        6502
#define IDS_CLAN_DENY_ALREADYJOINED     6503
#define IDS_CLAN_DENY_UNKNOWN           6504
#define IDS_CLAN_MAKE_SUCCESS           6505
#define IDS_CLAN_WARNING_COST           6506
#define IDS_CLAN_INPUT_NAME             6507
#define IDS_CLAN_REINPUT_NAME           6508
#define IDS_CLAN_WITHDRAW_SUCCESS       6509
#define IDS_CLAN_WITHDRAW_FAIL          6510
#define IDS_CLAN_JOIN_SUCCESS           6511
#define IDS_CLAN_JOIN_FAIL_CLAN_FULL    6512
#define IDS_CLAN_JOIN_FAIL_NONE_CLAN    6513
#define IDS_CLAN_JOIN_FAIL_INVALIDRIGHT 6514
#define IDS_CLAN_JOIN_FAIL_OTHER_CLAN_USER 6515
#define IDS_CLAN_JOIN_FAIL_ENEMY_USER   6516
#define IDS_CLAN_JOIN_FAIL_DEAD_USER    6517
#define IDS_CLAN_JOIN_FAIL_NONE_USER    6518
#define IDS_CLAN_COMMON_FAIL_ME         6519
#define IDS_CLAN_COMMON_FAIL_NOTJOINED  6520
#define IDS_CLAN_JOIN_REQ               6521
#define IDS_CLAN_JOIN_REJECT            6522
#define IDS_CLAN_COMMON_FAIL_BATTLEZONE 6523
#define IDS_CONFIRM_EXECUTE_OPTION      6524
#define IDS_REQUEST_GAME_SAVE           6525
#define IDS_DELAY_GAME_SAVE             6526
#define IDS_CLAN_DENY_INVALID_SERVER    6527
#define IDS_DEAD_RETURN_TOWN            6600
#define IDS_DEAD_REVIVAL                6601
#define IDS_DEAD_LACK_LIFE_STONE        6602
#define IDS_DEAD_REVIVAL_MESSAGE        6603
#define IDS_DEAD_LOW_LEVEL              6604
#define IDS_INVEN_WEIGHT                6605

CUIImageTooltipDlg::CUIImageTooltipDlg() : m_CYellow(D3DCOLOR_RGBA(255, 255, 0, 255)),
m_CBlue(D3DCOLOR_RGBA(128, 128, 255, 255)),
m_CGold(D3DCOLOR_RGBA(220, 199, 124, 255)),
m_CIvory(D3DCOLOR_RGBA(200, 124, 199, 255)),
m_CGreen(D3DCOLOR_RGBA(128, 255, 0, 255)),
m_CWhite(D3DCOLOR_RGBA(255, 255, 255, 255)),
m_CRed(D3DCOLOR_RGBA(255, 60, 60, 255)),
m_CCyan(D3DCOLOR_RGBA(0, 255, 204, 255)),
m_CKrowazRed(D3DCOLOR_RGBA(210, 0, 0, 255)),
m_CPink(D3DCOLOR_RGBA(247, 129, 190, 255)),
m_COrange(D3DCOLOR_RGBA(255, 165, 0, 255))
{
	m_iPosXBack = 0;
	m_iPosYBack = 0;
	m_spItemBack = NULL;
	m_pImg = NULL;
}

CUIImageTooltipDlg::~CUIImageTooltipDlg()
{
	m_Parent = NULL;
	Release();
}

void CUIImageTooltipDlg::Release()
{
	CN3UIBase::Release();
}

bool CUIImageTooltipDlg::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	std::string str;
	char	cstr[4];

	for (int i = 0; i < MAX_TOOLTIP_COUNT; i++)
	{
		str = ("string_");
		sprintf(cstr, ("%d"), i);	str += cstr;
		m_pStr[i] = (CN3UIString*)GetChildByID(str);
	}

	std::string mins = ("mins");
	m_pImg = (CN3UIImage*)GetChildByID(mins);
	SetVisible(false);

	return true;
}

void CUIImageTooltipDlg::Tick()
{
	if (m_Parent != Engine->m_UiMgr->m_FocusedUI)
		Engine->m_UiMgr->m_pUITooltipDlg->SetVisible(false);
}

void CUIImageTooltipDlg::DisplayTooltipsDisable()
{
	m_spItemBack = NULL;
	if (IsVisible()) SetVisible(false);
}

bool CUIImageTooltipDlg::SetTooltipTextColor(int iMyValue, int iTooltipValue)
{
	if (iMyValue >= iTooltipValue)
		return true;
	return false;
}

bool CUIImageTooltipDlg::SetTooltipTextColor(e_Race eMyValue, e_Race eTooltipValue)
{
	if (eMyValue == eTooltipValue)
		return true;
	return false;
}

bool CUIImageTooltipDlg::SetTooltipTextColor(e_Class eMyValue, e_Class eTooltipValue)
{
	if (eMyValue == eTooltipValue)
		return true;
	return false;
}

void CUIImageTooltipDlg::SetPosSomething(int xpos, int ypos, int iNum)
{
	int iWidth = 0;

	int iPadding = 8;

	for (int i = 0; i < iNum + 1; i++)
	{
		if (m_pstdstr[i].empty())	continue;
		int currentWidth = m_pStr[0]->GetStringRealWidth(m_pstdstr[i]);
		if (currentWidth > iWidth)
			iWidth = currentWidth;
	}

	int iHeight = m_pStr[iNum]->GetRegion().bottom - m_pStr[0]->GetRegion().top;

	iWidth += (int)ceil(iPadding * 2);
	iHeight += (int)ceil(iPadding * 1.5);

	RECT rect, rect2;

	int iRight, iTop, iBottom, iX, iY;

	RECT screen = Engine->m_UiMgr->GetScreenRect();
	iRight = screen.right;
	iTop = 0;
	iBottom = screen.bottom;

	if ((xpos + 26 + iWidth) < iRight)
	{
		rect.left = xpos + 26;
		rect.right = rect.left + iWidth;
		iX = xpos + 26;
	}
	else
	{
		rect.left = xpos - iWidth;
		rect.right = xpos;
		iX = xpos - iWidth;
	}

	if ((ypos - iHeight) > iTop)
	{
		rect.top = ypos - iHeight; rect.bottom = ypos;
		iY = ypos - iHeight;
	}
	else
	{
		if ((ypos + iHeight) < iBottom)
		{
			rect.top = ypos; rect.bottom = ypos + iHeight;
			iY = ypos;
		}
		else
		{
			rect.top = iBottom - iHeight; rect.bottom = iBottom;
			iY = rect.top;
		}
	}

	SetPos(iX, iY);
	SetSize(iWidth, iHeight);

	for (int i = iNum + 1; i < MAX_TOOLTIP_COUNT; i++)
	{
		m_pStr[i]->SetString("");
		m_pStr[i]->ClearCustomThings();
	}

	for (int i = 0; i < iNum + 1; i++)
	{
		if (!m_pStr[i])	continue;

		// add padding to rects
		rect2 = m_pStr[i]->GetRegion();
		rect2.left = rect.left + iPadding;
		rect2.right = rect.right - iPadding;
		m_pStr[i]->SetRegion(rect2);
		m_pStr[i]->SetString(m_pstdstr[i]);
	}

	m_pImg->SetRegion(rect);

	m_iPosXBack = xpos;
	m_iPosYBack = ypos;
}

bool ParseColor(const char* s, DWORD* col) {
	*col = 0;

	if (s[0] != '#' || s[1] != '#') {
		return false;
	}
	else {
		for (int i = 0; i < 8; ++i) {
			*col *= 16;
			char c = s[i + 2];
			if (c >= '0' && c <= '9') {
				*col += c - '0';
			}
			else if (c >= 'A' && c <= 'F') {
				*col += c - 'A' + 10;
			}
			else if (c >= 'a' && c <= 'f') {
				*col += c - 'a' + 10;
			}
			else {
				return false;
			}
		}
		return true;
	}
}

bool CUIImageTooltipDlg::GetTextByItemClass(e_ItemClass eItemClass, std::string& szText)
{
	switch (eItemClass)
	{
	case ITEM_CLASS_DAGGER:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_DAGGER, szText);
		break; // 단검(dagger)
	case ITEM_CLASS_SWORD:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_SWORD, szText);
		break; // 한손검(onehandsword)
	case ITEM_CLASS_SWORD_2H:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_SWORD_2H, szText);
		break; // 3 : 양손검(twohandsword)
	case ITEM_CLASS_AXE:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_AXE, szText);
		break; // 한손도끼(onehandaxe)
	case ITEM_CLASS_AXE_2H:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_AXE_2H, szText);
		break; // 두손도끼(twohandaxe)
	case ITEM_CLASS_MACE:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_MACE, szText);
		break; // 한손타격무기(mace)
	case ITEM_CLASS_MACE_2H:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_MACE_2H, szText);
		break; // 두손타격무기(twohandmace)
	case ITEM_CLASS_SPEAR:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_SPEAR, szText);
		break; // 창(spear)
	case ITEM_CLASS_POLEARM:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_POLEARM, szText);
		break; // 폴암(polearm)
	case ITEM_CLASS_SHIELD:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_SHIELD, szText);
		break; // 쉴드(shield)
	case ITEM_CLASS_BOW:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_BOW, szText);
		break; //  쇼트보우(Shortbow)
	case ITEM_CLASS_BOW_CROSS:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_BOW_CROSS, szText);
		break; // 크로스보우(crossbow)
	case ITEM_CLASS_BOW_LONG:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_BOW_LONG, szText);
		break; // 롱보우(longbow)
	case ITEM_CLASS_EARRING:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_EARRING, szText);
		break; // 귀걸이
	case ITEM_CLASS_AMULET:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_AMULET, szText);
		break; // 목걸이
	case ITEM_CLASS_RING:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_RING, szText);
		break; // 반지
	case ITEM_CLASS_BELT:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_BELT, szText);
		break; // 허리띠
	case ITEM_CLASS_CHARM:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_CHARM, szText);
		break; //인벤토리에 지니고 있는 아이템
	case ITEM_CLASS_JEWEL:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_JEWEL, szText);
		break; //보석종류
	case ITEM_CLASS_POTION:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_POTION, szText);
		break; // 물약
	case ITEM_CLASS_SCROLL:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_SCROLL, szText);
		break; // 스크롤
	case ITEM_CLASS_LAUNCHER:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_LAUNCHER, szText);
		break;
	case ITEM_CLASS_STAFF:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_STAFF, szText);
		break; // 지팡이(staff)
	case ITEM_CLASS_ARROW:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_ARROW, szText);
		break; // 화살(Arrow)
	case ITEM_CLASS_JAVELIN:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_JAVELIN, szText);
		break; // 투창
	case ITEM_CLASS_ARMOR_WARRIOR:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_ARMOR_WARRIOR, szText);
		break; // 전사 방어구
	case ITEM_CLASS_ARMOR_ROGUE:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_ARMOR_ROGUE, szText);
		break; // 로그 방어구
	case ITEM_CLASS_ARMOR_MAGE:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_ARMOR_MAGE, szText);
		break; // 마법사 방어구
	case ITEM_CLASS_ARMOR_PRIEST:
		Engine->tblMgr->GetResourceText(IDS_ITEM_CLASS_ARMOR_PRIEST, szText);
		break; // 사제 방어구
	default:
		return false;
	}

	return true;
}

bool CUIImageTooltipDlg::GetTextByRace(e_Race eRace, std::string& szText)
{
	switch (eRace)
	{
	case RACE_EL_BABARIAN:
		Engine->tblMgr->GetResourceText(IDS_RACE_EL_BABARIAN, szText);
		break;
	case RACE_EL_MAN:
		Engine->tblMgr->GetResourceText(IDS_RACE_EL_MAN, szText);
		break;
	case RACE_EL_WOMEN:
		Engine->tblMgr->GetResourceText(IDS_RACE_EL_WOMEN, szText);
		break;
	case RACE_KA_ARKTUAREK:
		Engine->tblMgr->GetResourceText(IDS_RACE_KA_ARKTUAREK, szText);
		break;
	case RACE_KA_TUAREK:
		Engine->tblMgr->GetResourceText(IDS_RACE_KA_TUAREK, szText);
		break;
	case RACE_KA_WRINKLETUAREK:
		Engine->tblMgr->GetResourceText(IDS_RACE_KA_WRINKLETUAREK, szText);
		break;
	case RACE_KA_PURITUAREK:
		Engine->tblMgr->GetResourceText(IDS_RACE_KA_PURITUAREK, szText);
		break;
	default:
		Engine->tblMgr->GetResourceText(IDS_NATION_UNKNOWN, szText);
		return false;
	}
	return true;
}

bool CUIImageTooltipDlg::GetTextByClass(e_Class eClass, std::string& szText)
{
	switch (eClass)
	{
	case CLASS_KINDOF_WARRIOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_WARRIOR, szText);
		break;
	case CLASS_KINDOF_ROGUE:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_ROGUE, szText);
		break;
	case CLASS_KINDOF_WIZARD:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_WIZARD, szText);
		break;
	case CLASS_KINDOF_PRIEST:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_PRIEST, szText);
		break;
	case CLASS_KINDOF_ATTACK_WARRIOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_ATTACK_WARRIOR, szText);
		break;
	case CLASS_KINDOF_DEFEND_WARRIOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_DEFEND_WARRIOR, szText);
		break;
	case CLASS_KINDOF_ARCHER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_ARCHER, szText);
		break;
	case CLASS_KINDOF_ASSASSIN:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_ASSASSIN, szText);
		break;
	case CLASS_KINDOF_ATTACK_WIZARD:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_ATTACK_WIZARD, szText);
		break;
	case CLASS_KINDOF_PET_WIZARD:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_PET_WIZARD, szText);
		break;
	case CLASS_KINDOF_HEAL_PRIEST:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_HEAL_PRIEST, szText);
		break;
	case CLASS_KINDOF_CURSE_PRIEST:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KINDOF_CURSE_PRIEST, szText);
		break;
	case CLASS_EL_WARRIOR:
	case CLASS_KA_WARRIOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_WARRIOR, szText);
		break;
	case CLASS_EL_ROGUE:
	case CLASS_KA_ROGUE:
		Engine->tblMgr->GetResourceText(IDS_CLASS_ROGUE, szText);
		break;
	case CLASS_EL_WIZARD:
	case CLASS_KA_WIZARD:
		Engine->tblMgr->GetResourceText(IDS_CLASS_WIZARD, szText);
		break;
	case CLASS_EL_PRIEST:
	case CLASS_KA_PRIEST:
		Engine->tblMgr->GetResourceText(IDS_CLASS_PRIEST, szText);
		break;
	case CLASS_KA_BERSERKER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_BERSERKER, szText);
		break;
	case CLASS_KA_GUARDIAN:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_GUARDIAN, szText);
		break;
	case CLASS_KA_HUNTER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_HUNTER, szText);
		break;
	case CLASS_KA_PENETRATOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_PENETRATOR, szText);
		break;
	case CLASS_KA_SORCERER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_SORCERER, szText);
		break;
	case CLASS_KA_NECROMANCER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_NECROMANCER, szText);
		break;
	case CLASS_KA_SHAMAN:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_SHAMAN, szText);
		break;
	case CLASS_KA_DARKPRIEST:
		Engine->tblMgr->GetResourceText(IDS_CLASS_KA_DARKPRIEST, szText);
		break;
	case CLASS_EL_BLADE:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_BLADE, szText);
		break;
	case CLASS_EL_PROTECTOR:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_PROTECTOR, szText);
		break;
	case CLASS_EL_RANGER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_RANGER, szText);
		break;
	case CLASS_EL_ASSASIN:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_ASSASIN, szText);
		break;
	case CLASS_EL_MAGE:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_MAGE, szText);
		break;
	case CLASS_EL_ENCHANTER:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_ENCHANTER, szText);
		break;
	case CLASS_EL_CLERIC:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_CLERIC, szText);
		break;
	case CLASS_EL_DRUID:
		Engine->tblMgr->GetResourceText(IDS_CLASS_EL_DRUID, szText);
		break;
	default:
		szText = xorstr("Unknown Class");
		return false;
	}

	return true;
}

int	CUIImageTooltipDlg::CalcTooltipStringNumAndWrite(__IconItemSkill* spItem, bool bPrice, bool bBuy)
{
	int iIndex = 0;
	std::string szStr;
	if (Engine->m_PlayerBase == NULL)
		Engine->m_PlayerBase = new CPlayerBase();
	//__InfoPlayerMySelf*	pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);
	CPlayerBase* pPlayerBase = Engine->m_PlayerBase != NULL ? Engine->m_PlayerBase : new CPlayerBase();
	

	if ((!m_spItemBack) || (m_spItemBack->pItemBasic->Num != spItem->pItemBasic->Num) ||
		//(m_spItemBack->pItemExt->dwID != spItem->pItemExt->dwID) ||
		(m_spItemBack->iDurability != spItem->iDurability))
	{
		for (int i = 0; i < MAX_TOOLTIP_COUNT; i++)
		{
			m_pStr[i]->ClearCustomThings();
			m_pstdstr[i] = "";
		}

		std::string szString;
		char szBuff[64] = "";

		if (m_pStr[iIndex])
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);

			if (spItem->pItemBasic->strName != m_pStr[iIndex]->GetString())
			{
				ItemAttrib eTA = (ItemAttrib)(spItem->pItemExt->byMagicOrRare);
				switch (eTA)
				{
				case ITEM_ATTR_NORMAL:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				case ITEM_ATTR_MAGIC:
					m_pStr[iIndex]->SetColor(m_CBlue);
					break;
				case ITEM_ATTR_RARE:
					m_pStr[iIndex]->SetColor(m_CYellow);
					break;
				case ITEM_ATTR_CRAFT:
					m_pStr[iIndex]->SetColor(m_CGreen);
					break;
				case ITEM_ATTR_UNIQUE:
					m_pStr[iIndex]->SetColor(m_CGold);
					break;
				case ITEM_ATTR_UPGRADE:
					m_pStr[iIndex]->SetColor(m_CIvory);
					break;
				case ITEM_ATTR_COSPRE:
					m_pStr[iIndex]->SetColor(m_CCyan);
					break;
				case ITEM_ATTR_MINERVA:
					m_pStr[iIndex]->SetColor(m_CKrowazRed);
					break;
				case ITEM_ATTR_REBIRTH_UPGRADE:
					m_pStr[iIndex]->SetColor(m_CPink);
					break;
				case ITEM_ATTR_REBIRTH_UNIQUE:
					m_pStr[iIndex]->SetColor(m_COrange);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				}
				
				m_pStr[iIndex]->SetFont(xorstr("Verdana"), m_pStr[iIndex+1]->GetFontHeight() + 1, TRUE, FALSE);
				std::string strtemp = "";

				if ((e_ItemAttrib)(spItem->pItemExt->byMagicOrRare) != ITEM_ATTRIB_UNIQUE)
				{
					if (spItem->pItemExt->extensionID % 10 != 0)
					{
						char szExtID[20];
						sprintf(szExtID, "(+%d)", spItem->pItemExt->extensionID % 10);
						strtemp = szExtID;
					}

					m_pstdstr[iIndex] = spItem->pItemBasic->strName + strtemp;
				}
				else
				{
					m_pstdstr[iIndex] = spItem->pItemExt->szHeader;
				}

			}
		}
		iIndex++;

		if ((spItem->pItemBasic->isCountable != UIITEM_TYPE_COUNTABLE) && (spItem->pItemBasic->isCountable != UIITEM_TYPE_COUNTABLE_SMALL))
		{
			e_ItemClass eIC = (e_ItemClass)(spItem->pItemBasic->Kind);
			if (GetTextByItemClass(eIC, szString)) {
				m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
				m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
				m_pStr[iIndex]->SetColor(m_CWhite);
				m_pstdstr[iIndex] = szString;
				iIndex++;
			}
		}

		if (m_pStr[iIndex])
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);

			if (spItem->pItemBasic->strName != m_pStr[iIndex]->GetString())
			{
				ItemAttrib eTA = (ItemAttrib)(spItem->pItemExt->byMagicOrRare);
				switch (eTA)
				{
				case ITEM_ATTR_MAGIC:
					m_pStr[iIndex]->SetColor(m_CBlue);
					m_pstdstr[iIndex] = xorstr("(Magic Item)");
					iIndex++;
					break;
				case ITEM_ATTR_RARE:
					m_pStr[iIndex]->SetColor(m_CYellow);
					m_pstdstr[iIndex] = xorstr("(Rare Item)");
					iIndex++;
					break;
				case ITEM_ATTR_CRAFT:
					m_pStr[iIndex]->SetColor(m_CGreen);
					m_pstdstr[iIndex] = xorstr("(Craft Item)");
					iIndex++;
					break;
				case ITEM_ATTR_UNIQUE:
					m_pStr[iIndex]->SetColor(m_CGold);
					m_pstdstr[iIndex] = xorstr("(Unique Item)");
					iIndex++;
					break;
				case ITEM_ATTR_UPGRADE:
					m_pStr[iIndex]->SetColor(m_CIvory);
					m_pstdstr[iIndex] = xorstr("(Upgrade Item)");
					iIndex++;
					break;
				case ITEM_ATTR_COSPRE:
					m_pStr[iIndex]->SetColor(m_CCyan);
					m_pstdstr[iIndex] = xorstr("(Cospre Item)");
					iIndex++;
					break;
				case ITEM_ATTR_MINERVA:
					m_pStr[iIndex]->SetColor(m_CKrowazRed);
					m_pstdstr[iIndex] = xorstr("(Minerva Item)");
					iIndex++;
					break;
				case ITEM_ATTR_REBIRTH_UPGRADE:
					m_pStr[iIndex]->SetColor(m_CPink);
					m_pstdstr[iIndex] = xorstr("(Rebirth Item)");
					iIndex++;
					break;
				case ITEM_ATTR_REBIRTH_UNIQUE:
					m_pStr[iIndex]->SetColor(m_COrange);
					m_pstdstr[iIndex] = xorstr("(Rebirth Unique Item)");
					iIndex++;
					break;
				default:
					break;
				}
			}
			
		}
		

		if ((int)spItem->pItemBasic->Class != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
			e_Class eClass = (e_Class)spItem->pItemBasic->Class;
			GetTextByClass(eClass, szString); // 아이템을 찰수 있는 종족에 따른 문자열 만들기.

			switch (eClass)
			{
			case CLASS_KINDOF_WARRIOR:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_WARRIOR:
				case CLASS_KA_BERSERKER:
				case CLASS_KA_GUARDIAN:
				case CLASS_EL_WARRIOR:
				case CLASS_EL_BLADE:
				case CLASS_EL_PROTECTOR:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_ROGUE:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_ROGUE:
				case CLASS_KA_HUNTER:
				case CLASS_KA_PENETRATOR:
				case CLASS_EL_ROGUE:
				case CLASS_EL_RANGER:
				case CLASS_EL_ASSASIN:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_WIZARD:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_WIZARD:
				case CLASS_KA_SORCERER:
				case CLASS_KA_NECROMANCER:
				case CLASS_EL_WIZARD:
				case CLASS_EL_MAGE:
				case CLASS_EL_ENCHANTER:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_PRIEST:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_PRIEST:
				case CLASS_KA_SHAMAN:
				case CLASS_KA_DARKPRIEST:
				case CLASS_EL_PRIEST:
				case CLASS_EL_CLERIC:
				case CLASS_EL_DRUID:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_ATTACK_WARRIOR:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_BERSERKER:
				case CLASS_EL_BLADE:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_DEFEND_WARRIOR:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_GUARDIAN:
				case CLASS_EL_PROTECTOR:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_ARCHER:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_HUNTER:
				case CLASS_EL_RANGER:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_ASSASSIN:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_PENETRATOR:
				case CLASS_EL_ASSASIN:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_ATTACK_WIZARD:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_SORCERER:
				case CLASS_EL_MAGE:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_PET_WIZARD:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_NECROMANCER:
				case CLASS_EL_ENCHANTER:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_HEAL_PRIEST:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_SHAMAN:
				case CLASS_EL_CLERIC:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			case CLASS_KINDOF_CURSE_PRIEST:
				switch (Engine->m_PlayerBase->m_sClass)
				{
				case CLASS_KA_DARKPRIEST:
				case CLASS_EL_DRUID:
					m_pStr[iIndex]->SetColor(m_CWhite);
					break;
				default:
					m_pStr[iIndex]->SetColor(m_CRed);
					break;
				}
				break;

			default:
				if (SetTooltipTextColor(Engine->m_PlayerBase->m_sClass, eClass))
					m_pStr[iIndex]->SetColor(m_CWhite);
				else
					m_pStr[iIndex]->SetColor(m_CRed);
				break;
			}
			m_pstdstr[iIndex] = szString;
			iIndex++;
		}

		if(!(szString.length() > 0))
			iIndex++;

		if ((int)spItem->pItemBasic->Class != 0)
		{
			if (m_pStr[iIndex-1]->GetColor() != m_CWhite)
			{
				m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
				m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
				m_pStr[iIndex]->SetColor(m_CRed);
				m_pstdstr[iIndex] = xorstr("(Unable to equip)");
				iIndex++;
			}
		}

		if (spItem->pItemBasic->Attack + spItem->pItemExt->siDamage != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DAMAGE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->Attack + spItem->pItemExt->siDamage);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemBasic->Delay * (float)((float)spItem->pItemExt->siAttackIntervalPercentage / 100.0f) != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			float fValue = spItem->pItemBasic->Delay * (float)((float)spItem->pItemExt->siAttackIntervalPercentage / 100.0f);

			if ((1 <= fValue) && (fValue <= 89))
				Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKINT_VERYFAST, szStr);
			else if ((90 <= fValue) && (fValue <= 110))
				Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKINT_FAST, szStr);
			else if ((111 <= fValue) && (fValue <= 130))
				Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKINT_NORMAL, szStr);
			else if ((131 <= fValue) && (fValue <= 150))
				Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKINT_SLOW, szStr);
			else
				Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKINT_VERYSLOW, szStr);
			sprintf(szBuff, szStr.c_str());
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		// 공격시간 감소 없어짐..

		if (spItem->pItemBasic->Range != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTACKRANGE, szStr);
			sprintf(szBuff, szStr.c_str(), (float)spItem->pItemBasic->Range / 10.0f);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siHitRate != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_HITRATE_OVER, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siHitRate);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siEvationRate != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_AVOIDRATE_OVER, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siEvationRate);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemBasic->Weight != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_WEIGHT, szStr);
			sprintf(szBuff, szStr.c_str(), (spItem->pItemBasic->Weight * 0.1f));
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemBasic->Duration + spItem->pItemExt->siMaxDurability != 1)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_MAX_DURABILITY, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->Duration + spItem->pItemExt->siMaxDurability);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemBasic->Duration + spItem->pItemExt->siMaxDurability != 1)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_CUR_DURABILITY, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->Duration + spItem->pItemExt->siMaxDurability);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = string(szBuff) + " (100%)";
			iIndex++;
		}

		if (spItem->pItemBasic->AC + spItem->pItemExt->siDefense != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->AC + spItem->pItemExt->siDefense);
			m_pStr[iIndex]->SetColor(m_CWhite);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateDagger != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_DAGGER, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateDagger);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateSword != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_SWORD, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateSword);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateBlow != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_BLOW, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateBlow);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateAxe != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_AXE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateAxe);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateSpear != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_SPEAR, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateSpear);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siDefenseRateArrow != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_DEFENSE_RATE_ARROW, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siDefenseRateArrow);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byDamageFire != 0)	// 화염속성
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC1, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byDamageFire);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byDamageIce != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC2, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byDamageIce);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byDamageThuner != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC4, szStr); // IDS_TOOLTIP_ATTRMAGIC3
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byDamageThuner);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byDamagePoison != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC3, szStr); // IDS_TOOLTIP_ATTRMAGIC4
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byDamagePoison);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byStillHP != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC4, szStr); // IDS_TOOLTIP_ATTRMAGIC5
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byStillHP);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byDamageMP != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC6, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byDamageMP);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->byStillMP != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_ATTRMAGIC7, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->byStillMP);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusStr != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSSTR, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusStr);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusSta != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSSTA, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusSta);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusHP != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSHP, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusHP);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusDex != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSDEX, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusDex);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusMSP != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSWIZ, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusMSP);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusInt != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSINT, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusInt);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siBonusMagicAttak != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BONUSMAGICATTACK, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siBonusMagicAttak);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistFire != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTFIRE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistFire);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistIce != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTICE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistIce);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistElec != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTELEC, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistElec);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistMagic != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTMAGIC, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistMagic);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistPoison != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTPOISON, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistPoison);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemExt->siRegistCurse != 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REGISTCURSE, szStr);
			sprintf(szBuff, szStr.c_str(), spItem->pItemExt->siRegistCurse);
			m_pStr[iIndex]->SetColor(m_CGreen);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if ( /*(spItem->pItemBasic->byAttachPoint == ITEM_LIMITED_EXHAUST) &&*/ spItem->pItemBasic->ReqLevelMin + spItem->pItemExt->siNeedLevel > 1)
		{
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDLEVEL, szStr);
			if (SetTooltipTextColor(Engine->m_PlayerBase->m_iLevel, spItem->pItemBasic->ReqLevelMin + spItem->pItemExt->siNeedLevel))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqLevelMin + spItem->pItemExt->siNeedLevel);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		/*if (spItem->pItemBasic->byNeedRank + spItem->pItemExt->siNeedRank > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDRANK, szStr);
			if (SetTooltipTextColor(pInfoExt->iRank, spItem->pItemBasic->byNeedRank + spItem->pItemExt->siNeedRank))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);
			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->byNeedRank + spItem->pItemExt->siNeedRank);
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (spItem->pItemBasic->byNeedTitle + spItem->pItemExt->siNeedTitle > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDTITLE, szStr);
			if (SetTooltipTextColor(pInfoExt->iTitle, spItem->pItemBasic->byNeedTitle + spItem->pItemExt->siNeedTitle))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);
			sprintf(szBuff, szStr.c_str(), std::to_string(spItem->pItemBasic->byNeedTitle + spItem->pItemExt->siNeedTitle).c_str());
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}*/

		int iNeedValue;
		iNeedValue = spItem->pItemBasic->ReqStr;
		if (iNeedValue != 0)
			iNeedValue += spItem->pItemExt->siNeedStrength;
		if (iNeedValue > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDSTRENGTH, szStr);
			if (SetTooltipTextColor(pPlayerBase->m_iStr, spItem->pItemBasic->ReqStr + spItem->pItemExt->siNeedStrength))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);

			std::string szReduce;
			if (spItem->pItemExt->siNeedStrength < 0)
			{
				if (spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE_REVERSE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE_REVERSE)
					Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REDUCE, szReduce);
			}

			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqStr + spItem->pItemExt->siNeedStrength, szReduce.c_str());
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		iNeedValue = spItem->pItemBasic->ReqHp;
		if (iNeedValue != 0)
			iNeedValue += spItem->pItemExt->siNeedStamina;
		if (iNeedValue > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDSTAMINA, szStr);
			if (SetTooltipTextColor(pPlayerBase->m_iHp, spItem->pItemBasic->ReqHp + spItem->pItemExt->siNeedStamina))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);

			std::string szReduce;
			if (spItem->pItemExt->siNeedStamina < 0)
			{
				if (spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE_REVERSE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE_REVERSE)
					Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REDUCE, szReduce);
			}

			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqHp + spItem->pItemExt->siNeedStamina, szReduce.c_str());

			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		iNeedValue = spItem->pItemBasic->ReqDex;
		if (iNeedValue != 0)
			iNeedValue += spItem->pItemExt->siNeedDexterity;
		if (iNeedValue > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDDEXTERITY, szStr);
			if (SetTooltipTextColor(pPlayerBase->m_iDex, spItem->pItemBasic->ReqDex + spItem->pItemExt->siNeedDexterity))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);

			std::string szReduce;
			if (spItem->pItemExt->siNeedDexterity < 0)
			{
				if (spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE_REVERSE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE_REVERSE)
					Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REDUCE, szReduce);
			}

			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqDex + spItem->pItemExt->siNeedDexterity, szReduce.c_str());

			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		iNeedValue = spItem->pItemBasic->ReqInt;
		if (iNeedValue != 0)
			iNeedValue += spItem->pItemExt->siNeedInteli;
		if (iNeedValue > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDINTELLI, szStr);
			if (SetTooltipTextColor(pPlayerBase->m_iInt, spItem->pItemBasic->ReqInt + spItem->pItemExt->siNeedInteli))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);

			std::string szReduce;
			if (spItem->pItemExt->siNeedInteli < 0)
			{
				if (spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE_REVERSE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE_REVERSE)
					Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REDUCE, szReduce);
			}

			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqInt + spItem->pItemExt->siNeedInteli, szReduce.c_str());
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		iNeedValue = spItem->pItemBasic->ReqMp;
		if (iNeedValue != 0)
			iNeedValue += spItem->pItemExt->siNeedMagicAttack;
		if (iNeedValue > 0)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_NEEDMAGICATTACK, szStr);
			if (SetTooltipTextColor(pPlayerBase->m_iMp, spItem->pItemBasic->ReqMp + spItem->pItemExt->siNeedMagicAttack))
				m_pStr[iIndex]->SetColor(m_CWhite);
			else
				m_pStr[iIndex]->SetColor(m_CRed);

			std::string szReduce;
			if (spItem->pItemExt->siNeedMagicAttack < 0)
			{
				if (spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UNIQUE_REVERSE
					|| spItem->pItemExt->byMagicOrRare == ITEM_ATTRIB_UPGRADE_REVERSE)
					Engine->tblMgr->GetResourceText(IDS_TOOLTIP_REDUCE, szReduce);
			}

			sprintf(szBuff, szStr.c_str(), spItem->pItemBasic->ReqMp + spItem->pItemExt->siNeedMagicAttack, szReduce.c_str()); //(Reduce)
			m_pstdstr[iIndex] = szBuff;
			iIndex++;
		}

		if (m_pStr[iIndex])
		{
			if (spItem->pItemBasic->strName != m_pStr[iIndex]->GetString())
			{
				ItemAttrib eTA = (ItemAttrib)(spItem->pItemExt->byMagicOrRare);
				switch (eTA)
				{
				case ITEM_ATTR_UNIQUE:
					m_pStr[iIndex]->SetStyle(UI_STR_TYPE_LINE, UISTYLE_STRING_SINGLELINE);
					m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
					m_pStr[iIndex]->SetColor(m_CGold);
					m_pstdstr[iIndex] = xorstr("Unique");
					iIndex++;
					break;
				}
			}

		}
		if (bPrice && Engine->m_PlayerBase != NULL)
		{
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_BUY_PRICE, szStr);
			if (SetTooltipTextColor(Engine->m_PlayerBase->m_iGold, spItem->pItemBasic->repairPrice * (spItem->pItemExt->siPriceMultiply > 0 ? spItem->pItemExt->siPriceMultiply : 1)))
				m_pStr[iIndex]->SetColor(D3DCOLOR_ARGB(255, 186, 255, 201));
			else
				m_pStr[iIndex]->SetColor(m_CRed);
			sprintf(szBuff, szStr.c_str(), Engine->StringHelper->NumberFormat(spItem->pItemBasic->repairPrice * (spItem->pItemExt->siPriceMultiply > 0 ? spItem->pItemExt->siPriceMultiply : 1)).c_str());
			m_pstdstr[iIndex] = szBuff;
			iIndex++;

			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNLEFT);
			Engine->tblMgr->GetResourceText(IDS_TOOLTIP_SELL_PRICE, szStr);
			m_pStr[iIndex]->SetColor(D3DCOLOR_ARGB(255, 115, 255, 243));
			sprintf(szBuff, szStr.c_str(), Engine->StringHelper->NumberFormat(spItem->pItemBasic->repairPrice * (spItem->pItemExt->siPriceMultiply > 0 ? spItem->pItemExt->siPriceMultiply : 1) / 6).c_str());
			m_pstdstr[iIndex] = szBuff;

		}
		else iIndex--;

		if (spItem->pItemBasic->Kind == 255 
			|| spItem->pItemBasic->UNK0 == 1
			|| spItem->pItemBasic->SellingGroup == 247)
		{
			iIndex++;
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
			m_pStr[iIndex]->SetColor(D3DCOLOR_RGBA(255, 23, 23, 255));
			m_pstdstr[iIndex] = xorstr("Can be traded after 72 hours");
		}

		if (spItem->pItemBasic->Description != "" && spItem->pItemBasic->Description != " ")
		{
			iIndex++;
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "<font color=@#", "##");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "@>", "FF");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "</font>", "##FFFFFFFF");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "<b>", "");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "</b>", "");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "<u>", "");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "</u>", "");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "<i>", "");
			Engine->StringHelper->Replace(spItem->pItemBasic->Description, "</i>", "");

			std::vector<std::string> out;
			Engine->StringHelper->Split(spItem->pItemBasic->Description, '|', out);
			if (out.size() > 1)
			{
				bool star = false;
				for (size_t i = 0; i < out.size(); i++)
				{
					iIndex++;
					if (!star)
					{
						m_pstdstr[iIndex] = xorstr("*");
						star = true;
					}
					m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
					
					m_pstdstr[iIndex] += out[i];

					DWORD color = 0;
					if (ParseColor(out[i].c_str(), &color)) {
						m_pStr[iIndex]->SetColor(D3DCOLOR(color));
						// Remove the color define
						while (m_pstdstr[iIndex].find("##") != std::string::npos) {
							for (int i = 0; i < m_pstdstr[iIndex].length(); i++)
							{
								if (m_pstdstr[iIndex][i] == '#' && m_pstdstr[iIndex][i + 1] == '#')
								{
									m_pstdstr[iIndex].erase(i, 10);
									break;
								}
							}
						}
					}
					else
						m_pStr[iIndex]->SetColor(m_CWhite);
				}
				m_pstdstr[iIndex] += xorstr("*");
			}
			else
			{
				iIndex++;
				m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
				
				m_pstdstr[iIndex] = xorstr("*") + spItem->pItemBasic->Description + xorstr("*");

				DWORD color = 0;
				if (ParseColor(spItem->pItemBasic->Description.c_str(), &color)) {
					m_pStr[iIndex]->SetColor(D3DCOLOR(color));
					// Remove the color define
					while (m_pstdstr[iIndex].find("##") != std::string::npos) {
						for (int i = 0; i < m_pstdstr[iIndex].length(); i++)
						{
							if (m_pstdstr[iIndex][i] == '#' && m_pstdstr[iIndex][i + 1] == '#')
							{
								m_pstdstr[iIndex].erase(i, 10);
								break;
							}
						}
					}
				}
				else
					m_pStr[iIndex]->SetColor(m_CWhite);
			}
		}
		if (spItem->pItemBasic->Class > 0 && spItem->pItemBasic->Duration > 0 && spItem->pItemBasic->SellingGroup > 0 && spItem->pItemBasic->Slot > 0)
		{
			iIndex++;
			m_pStr[iIndex]->SetStyle(UI_STR_TYPE_HALIGN, UISTYLE_STRING_ALIGNCENTER);
			m_pStr[iIndex]->SetColor(D3DCOLOR_RGBA(128, 255, 0, 255));
			m_pstdstr[iIndex] = xorstr("[Enable Comprasion by pressing the 'Ctrl' Key.]");
		}
	}
	else
	{
		iIndex = m_spItemBack->index;
	}

	if (spItem->index == 0)
		spItem->index = iIndex;

	m_spItemBack = spItem;

	return spItem->index;
}

int iNum = 1;

void CUIImageTooltipDlg::DisplayTooltipsEnable(int xpos, int ypos, __IconItemSkill* spItem, CN3UIBase* parent, bool tick, bool bPrice, bool bBuy)
{
	m_Parent = parent;

	if (!spItem) return;

	if (!IsVisible()) SetVisible(true);

	if (tick)
	{
		if (m_spItemBack == NULL || m_spItemBack->pItemBasic->Num != spItem->pItemBasic->Num)
		{
			iNum = CalcTooltipStringNumAndWrite(spItem, bPrice, bBuy);
		}
		SetPosSomething(xpos, ypos, iNum);
	}
	else {
		if (m_spItemBack == NULL || m_spItemBack->pItemBasic->Num != spItem->pItemBasic->Num)
		{
			int iNum = CalcTooltipStringNumAndWrite(spItem, bPrice, bBuy);
			SetPosSomething(xpos, ypos, iNum);
		}
	}
}