#pragma once
class CVoiceSettings;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIEdit.h"

class CVoiceSettings : public CN3UIBase
{
	CN3UIButton* btn_close, * btn_micstate, * btn_mute_all, * btn_mute_listening, * btn_mute_speaking;
	CN3UIEdit* edit_hotkey;
public:
	CVoiceSettings();
	~CVoiceSettings();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	uint32_t MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	bool OnKeyPress(int iKey);
	void Close();
	void Open();
	void LoadSettings();
	void SaveSettings();
};