#include "CREvent.h"

#define ITEM_GOLD					900000000	// Coins
#define ITEM_EXP					900001000
#define ITEM_COUNT					900002000
#define ITEM_KC						980000000

CCollectionRace::CCollectionRace()
{
	m_Timer = NULL;
	m_iRemainingTime = 0;

	m_btnDetails = NULL;
	m_txtEvent = NULL;
	m_txtRemainingTime = NULL;
	m_txtCompletions = NULL;

	m_bIsStarted = false;
}

CCollectionRace::~CCollectionRace()
{
}

bool CCollectionRace::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	// Main UI
	std::string find = xorstr("btn_details");
	m_btnDetails = (CN3UIButton*)GetChildByID(find);
	find = xorstr("txt_state");
	m_txtState = (CN3UIString*)m_btnDetails->GetChildByID(find);
	find = xorstr("txt_event");
	m_txtEvent = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_remaining");
	m_txtRemainingTime = (CN3UIString*)GetChildByID(find);
	find = xorstr("txt_completions");
	m_txtCompletions = (CN3UIString*)GetChildByID(find);

	// Sub UI
	find = xorstr("group_details");
	m_gDetails = (CN3UIBase*)GetChildByID(find);

	for (int i = 0; i < 4; i++)
	{
		find = string_format(xorstr("group_hunt%d"), i + 1);
		huntGroups[i] = (CN3UIBase*)m_gDetails->GetChildByID(find);
		HuntData hData;
		find = xorstr("targetmob");
		hData.txt_target = (CN3UIString*)huntGroups[i]->GetChildByID(find);
		find = xorstr("txt_completion");
		hData.txt_completion = (CN3UIString*)huntGroups[i]->GetChildByID(find);
		find = xorstr("progress_completion");
		hData.progress_completion = (CN3UIProgress*)huntGroups[i]->GetChildByID(find);
		hunts[i] = hData;
	}

	find = xorstr("btn_rewards");
	m_btnRewards = (CN3UIButton*)m_gDetails->GetChildByID(find);

	// Regions
	find = xorstr("region_max");
	region_max = (CN3UIArea*)GetChildByID(find);
	find = xorstr("region_min");
	region_min = (CN3UIArea*)GetChildByID(find);

	region_max->SetVisible(false);
	region_min->SetVisible(false);

	m_txtEvent->SetVisible(true);
	m_txtRemainingTime->SetVisible(true);
	m_txtCompletions->SetVisible(true);
	m_btnDetails->SetVisible(true);
	m_gDetails->SetVisible(false);

	SetMoveRect(m_txtEvent->GetRegion());
	SetRegion(region_min->GetRegion());

	POINT targetBarPos;
	Engine->GetUiPos(Engine->uiTargetBar->m_dVTableAddr, targetBarPos);

	m_txtCompletions->SetString(xorstr("Completions: 0 / 4"));

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, targetBarPos.y + Engine->GetUiHeight(Engine->uiTargetBar->m_dVTableAddr) + 5);

	return true;
}

void CCollectionRace::Tick()
{
	SetVisible(Engine->IsCRActive && m_iRemainingTime > 0);

	if (IsVisible())
	{
		if (m_Timer->IsElapsedSecond())
			m_iRemainingTime--;

		uint16_t remainingMinutes = (uint16_t)ceil(m_iRemainingTime / 60);
		uint16_t remainingSeconds = m_iRemainingTime - (remainingMinutes * 60);

		std::string remainingTime;
		if (remainingMinutes < 10 && remainingSeconds < 10)
			remainingTime = string_format(xorstr("0%d : 0%d"), remainingMinutes, remainingSeconds);
		else if (remainingMinutes < 10)
			remainingTime = string_format(xorstr("0%d : %d"), remainingMinutes, remainingSeconds);
		else if (remainingSeconds < 10)
			remainingTime = string_format(xorstr("%d : 0%d"), remainingMinutes, remainingSeconds);
		else remainingTime = string_format(xorstr("%d : %d"), remainingMinutes, remainingSeconds);

		m_txtRemainingTime->SetString(string_format(xorstr("Remaining Time: %s"), remainingTime.c_str()));
	}
}

bool CCollectionRace::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_btnDetails)
			m_gDetails->SetVisible(!m_gDetails->IsVisible());
		else if (pSender == m_btnRewards)
		{
			if (Engine->m_UiMgr->uiCollectionRaceRewards == NULL)
			{
				Engine->m_UiMgr->uiCollectionRaceRewards = new CCollectionRaceRewards();
				Engine->m_UiMgr->uiCollectionRaceRewards->Init(Engine->m_UiMgr);
				Engine->m_UiMgr->uiCollectionRaceRewards->LoadFromFile(Engine->m_BasePath + xorstr("TEMUKOACS\\ui\\cr_rewards.uif"), N3FORMAT_VER_1068);
				Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiCollectionRaceRewards);
			}
			Engine->m_UiMgr->uiCollectionRaceRewards->Open();

			uint32 items[2] = { Engine->crEvent.rewards[0], Engine->crEvent.rewards[1] };
			uint32 resources[4] = { ITEM_EXP, ITEM_GOLD, ITEM_COUNT, ITEM_KC };
			uint32 counts[6] = { Engine->crEvent.rewardsCount[0], Engine->crEvent.rewardsCount[1], Engine->crEvent.rewardExp, Engine->crEvent.rewardNoah , Engine->crEvent.rewardNP , Engine->crEvent.rewardKC };

			Engine->m_UiMgr->uiCollectionRaceRewards->LoadItems(items, resources, counts);
		}
	}

	if (m_gDetails->IsVisible()) {
		m_txtState->SetString(xorstr("Collapse"));
		SetRegion(region_max->GetRegion());
	}
	else {
		m_txtState->SetString(xorstr("Expand"));
		SetRegion(region_min->GetRegion());
	}

	return true;
}

uint32_t CCollectionRace::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CCollectionRace::Open()
{
	SetVisible(true);
}

void CCollectionRace::Close()
{
	SetVisible(false);
}

bool CCollectionRace::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	return CN3UIBase::OnKeyPress(iKey);
}

void CCollectionRace::Start()
{
	m_iRemainingTime = Engine->crEvent.endTime;
	for (int i = 0; i < 4; i++)
	{
		hunts[i].nTargetID = Engine->crEvent.mobs[i];
		hunts[i].targetCount = Engine->crEvent.killTarget[i];
		hunts[i].txt_completion->SetString(string_format(xorstr("0 / %d"), hunts[i].targetCount));
		hunts[i].progress_completion->SetRange(0, hunts[i].targetCount);
		hunts[i].progress_completion->SetCurValue(0, 0.5F, 20.0F);

		TABLE_MOB* mobData = Engine->tblMgr->getMobData(hunts[i].nTargetID);
		if (mobData != nullptr)
			hunts[i].txt_target->SetString(mobData->strName);
		else if (hunts[i].nTargetID == 1)
		{
			std::string strTarget = Engine->m_PlayerBase->m_bNation == 1 ? xorstr("Hunt Elmorad") : xorstr("Hunt Karus");
			hunts[i].txt_target->SetString(strTarget);
		}
		else if (hunts[i].nTargetID == 0)
			hunts[i].txt_target->SetString(xorstr("-"));
		else
			hunts[i].txt_target->SetString(xorstr("<unknown>"));
	}
	m_Timer = new CTimer(false);
	SetVisible(true);
}

void CCollectionRace::Update()
{
	int completionCount = 0;
	for (int i = 0; i < 4; i++)
	{
		hunts[i].txt_completion->SetString(string_format(xorstr("%d / %d"), Engine->crEvent.killCount[i], hunts[i].targetCount));
		hunts[i].progress_completion->SetRange(0, Engine->crEvent.killTarget[i]);
		hunts[i].progress_completion->SetCurValue(Engine->crEvent.killCount[i]);

		if (Engine->crEvent.killCount[i] >= hunts[i].targetCount)
			completionCount++;
	}

	m_txtCompletions->SetString(string_format(xorstr("Completions: %d / 4"), completionCount));

	if (completionCount >= 4)
		Close();
}