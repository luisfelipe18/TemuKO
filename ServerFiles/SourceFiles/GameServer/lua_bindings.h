#pragma once

LUA_FUNCTION(CheckPercent);
LUA_FUNCTION(HowmuchItem);
LUA_FUNCTION(ShowMap);
LUA_FUNCTION(CheckNation);
LUA_FUNCTION(CheckClass);
LUA_FUNCTION(CheckLevel);
LUA_FUNCTION(CheckSkillPoint);
LUA_FUNCTION(SaveEvent);
LUA_FUNCTION(CheckGiveSlot);
LUA_FUNCTION(CheckExchange);
LUA_FUNCTION(RunQuestExchange);
LUA_FUNCTION(RunMiningExchange);
LUA_FUNCTION(SearchQuest);
LUA_FUNCTION(NpcMsg);
LUA_FUNCTION(ShowEffect);
LUA_FUNCTION(ShowNpcEffect);
LUA_FUNCTION(ExistMonsterQuestSub);
LUA_FUNCTION(CountMonsterQuestSub);
LUA_FUNCTION(QuestCheckQuestFinished);
LUA_FUNCTION(QuestCheckExistEvent);
LUA_FUNCTION(CountMonsterQuestMain); 
LUA_FUNCTION(PromoteKnight);
LUA_FUNCTION(SelectMsg);
LUA_FUNCTION(CastSkill);
LUA_FUNCTION(GetName);;
LUA_FUNCTION(GetAccountName);;
LUA_FUNCTION(GetZoneID);;
LUA_FUNCTION(GetX);;
LUA_FUNCTION(GetY);
LUA_FUNCTION(GetZ);
LUA_FUNCTION(GetNation);
LUA_FUNCTION(GetLevel);
LUA_FUNCTION(GetRebirthLevel);
LUA_FUNCTION(GetClass);
LUA_FUNCTION(GetCoins);
LUA_FUNCTION(GetInnCoins);
LUA_FUNCTION(GetLoyalty);
LUA_FUNCTION(GetMonthlyLoyalty);
LUA_FUNCTION(GetManner);
LUA_FUNCTION(isWarrior);
LUA_FUNCTION(isRogue);
LUA_FUNCTION(isMage);
LUA_FUNCTION(isPriest);
LUA_FUNCTION(isPortuKurian);
LUA_FUNCTION(isInClan);
LUA_FUNCTION(isClanLeader);
LUA_FUNCTION(isInParty);
LUA_FUNCTION(isPartyLeader);
LUA_FUNCTION(isKing);
// Shortcuts for lazy people
LUA_FUNCTION(GetQuestStatus);
LUA_FUNCTION(hasCoins);
LUA_FUNCTION(hasInnCoins);
LUA_FUNCTION(hasLoyalty);
LUA_FUNCTION(hasMonthlyLoyalty);
LUA_FUNCTION(hasManner);
// Here lie the useful methods.
LUA_FUNCTION(GiveItem);
LUA_FUNCTION(GiveItemLua);
LUA_FUNCTION(RobItem);
LUA_FUNCTION(CheckExistItem);
LUA_FUNCTION(GoldGain);
LUA_FUNCTION(GoldLose);
LUA_FUNCTION(RunGiveItemExchange);
LUA_FUNCTION(ExpChange);
LUA_FUNCTION(GiveCash);
LUA_FUNCTION(ShowBulletinBoard);
LUA_FUNCTION(GiveLoyalty);
LUA_FUNCTION(RobLoyalty);
LUA_FUNCTION(NpcSay); // dialog
LUA_FUNCTION(CheckWeight);
LUA_FUNCTION(CheckSkillPoint);
LUA_FUNCTION(isRoomForItem); // FindSlotForItem();
LUA_FUNCTION(SendNameChange);
LUA_FUNCTION(SendClanNameChange);
LUA_FUNCTION(SendStatSkillDistribute);
LUA_FUNCTION(ResetSkillPoints);
LUA_FUNCTION(ResetStatPoints);
LUA_FUNCTION(PromoteUserNovice);
LUA_FUNCTION(PromoteUser);
LUA_FUNCTION(RobAllItemParty);
LUA_FUNCTION(ZoneChange);
LUA_FUNCTION(ZoneChangeParty);
LUA_FUNCTION(ZoneChangeClan);
LUA_FUNCTION(KissUser);
LUA_FUNCTION(ChangeManner);
LUA_FUNCTION(CheckClanGrade);
LUA_FUNCTION(CheckClanPoint);
LUA_FUNCTION(CheckLoyalty);
LUA_FUNCTION(CheckKnight);
LUA_FUNCTION(CheckStatPoint); 
LUA_FUNCTION(RobClanPoint);
LUA_FUNCTION(CheckBeefRoastVictory);
LUA_FUNCTION(RequestPersonalRankReward);
LUA_FUNCTION(RequestReward);
LUA_FUNCTION(RunExchange);
LUA_FUNCTION(GetMaxExchange);
LUA_FUNCTION(RunCountExchange);
LUA_FUNCTION(GetUserDailyOp);
LUA_FUNCTION(GetEventTrigger);
LUA_FUNCTION(GetPremium);
LUA_FUNCTION(CheckWarVictory);
LUA_FUNCTION(CheckMiddleStatueCapture);
LUA_FUNCTION(MoveMiddleStatue);
LUA_FUNCTION(LevelChange);
LUA_FUNCTION(GivePremium);
LUA_FUNCTION(GiveClanPremium);
LUA_FUNCTION(GivePremiumItem);
LUA_FUNCTION(GiveKnightCash);
LUA_FUNCTION(RollDice);
LUA_FUNCTION(CheckMonsterChallengeTime);
LUA_FUNCTION(CheckMonsterChallengeUserCount);
LUA_FUNCTION(CheckUnderTheCastleOpen);
LUA_FUNCTION(CheckUnderTheCastleUserCount);
LUA_FUNCTION(CheckJuraidMountainTime);
LUA_FUNCTION(GetPVPMonumentNation);
LUA_FUNCTION(NationChange);
LUA_FUNCTION(GetRace);
LUA_FUNCTION(GenderChange);
LUA_FUNCTION(JobChange);
LUA_FUNCTION(EventSoccerMember);
LUA_FUNCTION(EventSoccerStard);
LUA_FUNCTION(SpawnEventSystem);
LUA_FUNCTION(SendNationTransfer);
LUA_FUNCTION(SendRepurchaseMsg);
LUA_FUNCTION(GiveNPoints);
LUA_FUNCTION(NpcEventSystem);
LUA_FUNCTION(JoinEvent);
LUA_FUNCTION(KillNpcEvent);
LUA_FUNCTION(KingsInspectorList);
LUA_FUNCTION(isCswWinnerNembers);
LUA_FUNCTION(DelosCasttellanZoneOut);
LUA_FUNCTION(CycleSpawn);
LUA_FUNCTION(MonsterStoneQuestJoin);
LUA_FUNCTION(DrakiRiftChange);
LUA_FUNCTION(DrakiOutZone);
LUA_FUNCTION(DrakiTowerNpcOut);
LUA_FUNCTION(GetExpPercent);
LUA_FUNCTION(CheckCastleSiegeWarDeathmachRegister);
LUA_FUNCTION(CheckCastleSiegeWarDeathmacthCancelRegister);