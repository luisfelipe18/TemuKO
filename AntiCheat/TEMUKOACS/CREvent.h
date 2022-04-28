#pragma once
class CCollectionRace;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "Timer.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"

class CCollectionRace : public CN3UIBase
{
	struct HuntData
	{
		uint32 nTargetID;
		uint16 currentCount;
		uint16 targetCount;
		CN3UIString* txt_target;
		CN3UIString* txt_completion;
		CN3UIProgress* progress_completion;
		HuntData()
		{
			nTargetID = 0;
			currentCount = 0;
			targetCount = 1;
			txt_target = NULL;
			txt_completion = NULL;
			progress_completion = NULL;
		}
	};

	uint16 m_iRemainingTime;

	CN3UIString* m_txtEvent;
	CN3UIString* m_txtRemainingTime;
	CN3UIString* m_txtCompletions;
	CN3UIButton* m_btnDetails;
	CN3UIString* m_txtState;

	CN3UIBase* m_gDetails;
	CN3UIBase* m_txtEventDetails;
	CN3UIButton* m_btnRewards;
	CN3UIBase* huntGroups[4];

	CN3UIArea* region_max;
	CN3UIArea* region_min;

	HuntData hunts[4];

	CTimer* m_Timer;
	bool m_bIsStarted;
public:
	CCollectionRace();
	~CCollectionRace();
	bool Load(HANDLE hFile);
	void Tick();
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Open();
	void Close();
	void Start();
	void Update();
};