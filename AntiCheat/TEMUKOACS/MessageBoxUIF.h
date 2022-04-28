#pragma once
class CUIMessageBox;
#include "stdafx.h"
#include "TEMUKOACS Engine.h"
#include "Timer.h"
#include "N3BASE/N3UIBase.h"
#include "N3BASE/N3UIString.h"
#include "N3BASE/N3UIButton.h"
#include "N3BASE/N3UIEdit.h"

class CUIMessageBox : public CN3UIBase
{
public:
	MsgBoxTypes m_Type;
	ParentTypes m_ParentType;

	CN3UIString* m_txtTitle;
	CN3UIString* m_txtMessage;

	CN3UIEdit* m_editTextbox;

	CN3UIButton* m_btnNo;
	CN3UIButton* m_btnYes;
	CN3UIButton* m_btnOk;

public:
	CUIMessageBox(MsgBoxTypes type, ParentTypes pluginType = ParentTypes::PARENT_NONE);
	~CUIMessageBox();
	bool Load(HANDLE hFile);
	void Update(MsgBoxTypes type, ParentTypes parentType);
	void SetTitle(std::string title);
	void SetMessage(std::string msg);
	void SetVisibleTextbox(bool bVisible);
	bool ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	bool OnKeyPress(int iKey);
	void Close();

private:

};