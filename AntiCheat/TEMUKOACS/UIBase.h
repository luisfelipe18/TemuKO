#pragma once
#include "stdafx.h"
#include <list>

enum UI_MOUSE_STATE
{
	NONE = 0,
	HOVER = 1,
	PRESSED = 2
};

struct UI_TEXT
{
	uint32 id;
	std::string msg;
	D3DXVECTOR2 pos;
};

class UIBase;
class UIChild;

typedef std::list<UIChild*> UIList;
typedef UIList::iterator UIListItor;
typedef UIList::reverse_iterator UIListReverseItor;

class UIBase {
public:
	UIBase();
	void Init(LPDIRECT3DDEVICE9 Device);
	void Render();
	void Release();
	void SetSize(float width, float height);
	void SetPos(float x, float y);
	void SetPosCenter();
	void AddChild(UIChild* child);
	void RemoveChild(UIChild* child);
	void ClearChild();
	void Show();
	void Hide();
	bool isVisible() const { return m_bVisible;  }
	D3DXVECTOR2 GetSize() const { return D3DXVECTOR2(w, h); }
	D3DXVECTOR2 GetPos() const { return D3DXVECTOR2(pos.x, pos.y); }
	int GetChildrenCount() { return m_Children.size(); }
	void SetMouseState(UI_MOUSE_STATE state);
	UI_MOUSE_STATE GetMouseState();
	bool MouseIn(float x, float y, float w, float h);
	HWND _KO;
protected:
	UIList m_Children;
	bool m_bVisible;
	LPDIRECT3DDEVICE9 pDevice;
	float w, h;
	D3DXVECTOR3 pos;
	UI_MOUSE_STATE m_MouseState;
};

class UIChild : public UIBase
{
public:
	UIChild();
	void Init(const char* filename);
	void Render();
	void Release();
	void SetSize(int width, int height);
	void SetPos(float x, float y);
	void SetPosCenter();
	void Show();
	void Hide();
	void SetMessage(std::string _msg);
	bool isVisible() { return m_bVisible; }
	void SetDevice(LPDIRECT3DDEVICE9 device);
	void AddChild(UIChild* child);
	void RemoveChild(UIChild* child);
	void ClearChild();
	void SetParent(UIChild* parent);
	void SetParent(UIBase* parent);
	D3DXVECTOR2 GetSize();
	D3DXVECTOR2 GetPos();
	void SetMouseState(UI_MOUSE_STATE state);
	UI_MOUSE_STATE GetMouseState();
	void AddString(UI_TEXT text);
	void SetString(uint32 id, std::string msg);
	void RemoveString(uint32 id);
	void ClearString();
	bool isChildest() { return childest;  }
	void SetChildest(bool val) { childest = val; };
protected:
	bool childest;
	LPD3DXFONT m_font;
	UIList m_Children;
	UIChild* m_ParentChild;
	UIBase* m_Parent;
	bool m_bVisible;
	LPDIRECT3DDEVICE9 pDevice;
	IDirect3DTexture9* m_Texture;
	LPD3DXSPRITE m_Sprite;
	int w;
	int h;
	D3DXVECTOR3 pos;
	std::string msg;
	UI_MOUSE_STATE m_MouseState;
	std::list<UI_TEXT> texts;
private:
	bool LoadFromFile(const char* filename);
};