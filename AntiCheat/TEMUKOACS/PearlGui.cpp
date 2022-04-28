#include "PearlGui.h"
#include "Timer.h"
#include "SockEngine.h"
#include <future>

using namespace std;

HRESULT WINAPI HookCreateDevice();

#define HOOK(func,addy)	o##func = (t##func)DetourFunction((PBYTE)addy,(PBYTE)hk##func)
#define D3D_RELEASE(D3D_PTR) if( D3D_PTR ){ D3D_PTR->Release(); D3D_PTR = NULL; }
#define ES	0
#define DIP	1
#define RES 2
#define FOX 3
HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* tReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* mySendMessage) (HWND, UINT, WPARAM, LPARAM);

typedef HRESULT(WINAPI* CreateDevice_t)(IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
	DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
	IDirect3DDevice9** ppReturnedDeviceInterface);
HRESULT WINAPI D3DCreateDevice_hook(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
	DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters,
	IDirect3DDevice9 * *ppReturnedDeviceInterface);

CreateDevice_t D3DCreateDevice_orig;

PDWORD IDirect3D9_vtable = NULL;
#define CREATEDEVICE_VTI 16

DWORD VTable[3] = { 0 };
DWORD D3DEndScene;
DWORD D3DReset;
tReset oReset;
tEndScene oPresent;
tEndScene oEndScene;
WNDPROC oWndProc;
static bool sendHooked = false;
static bool reset = false;
static bool itsMe = false;

LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9  g_pIB = NULL;

void AlternativeHook(void* original, void* nova, int tamanho, BYTE * bytes_salvos)
{
	DWORD protect;
	VirtualProtect(original, tamanho, PAGE_EXECUTE_READWRITE, &protect);
	memcpy(bytes_salvos, original, tamanho);
	DWORD diferenca = ((DWORD)nova - (DWORD)original) - tamanho;
	memset(original, 0x90, tamanho);
	*(BYTE*)original = 0xE9;
	*(DWORD*)((DWORD)original + 1) = diferenca;
	VirtualProtect(original, tamanho, protect, &protect);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* Present)
{
	if (Engine->m_SettingsMgr == NULL)
		Engine->m_SettingsMgr = new CSettingsManager();

	Present->Windowed = Engine->m_SettingsMgr->m_iRealFullScreen == 1 ? false : true;
	Present->PresentationInterval = Engine->m_SettingsMgr->m_iVsync == 1 ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

	if(Engine->m_SettingsMgr->m_iVsync == 1)
		Present->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	reset = true;
	return oReset(pDevice, Present);
}

std::mutex endScene_mutex;

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!Engine->power)
		return (BYTE)0x90;

	if (!sendHooked) {
		sendHooked = true;
		Engine->m_UiMgr = new CUIManager();
		Engine->m_UiMgr->Init(pDevice);
		Engine->InitSendHook();
		Engine->InitRecvHook();
		Engine->InitSetString();
	}

	if (!reset)
	{
		if (Engine->Player.isGameStat) 
		{
			reset = true;
			if (Engine->m_UiMgr->uiTopLeft != NULL)
				Engine->m_UiMgr->uiTopLeft->UpdatePosition();
			if (Engine->m_UiMgr->uiStatPreset != NULL)
				Engine->m_UiMgr->uiStatPreset->UpdatePosition();
			if (Engine->m_UiMgr->uiSkillPreset != NULL)
				Engine->m_UiMgr->uiSkillPreset->UpdatePosition();
		}
	}

	if (!Engine->drawMode 
		|| Engine->Loading)
		return oEndScene(pDevice);

	if (Engine->uiSeedHelperPlug != NULL)
		Engine->uiSeedHelperPlug->Tick();

	if (Engine->uiPieceChangePlug != NULL)
		Engine->uiPieceChangePlug->Tick();

	if (Engine->uiTradePrice != NULL)
		Engine->uiTradePrice->Tick();

	if (Engine->m_UiMgr != NULL) {
		Engine->m_UiMgr->Tick();
		Engine->m_UiMgr->Render();
	}

	if (Engine->uiPartyPlug != NULL)
		Engine->uiPartyPlug->Tick();

	/*if (GetAsyncKeyState(VK_F1))
		Engine->StartRecording();
	else
		Engine->StopRecording();*/

	return oEndScene(pDevice);
}

LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DX_Init(DWORD* table)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, xorstr("DX"), NULL };
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindow(xorstr("DX"), NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), NULL, wc.hInstance, NULL);
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	LPDIRECT3DDEVICE9 pd3dDevice;
	itsMe = true;
	pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &pd3dDevice);
	itsMe = false;
	DWORD* pVTable = (DWORD*)pd3dDevice;
	pVTable = (DWORD*)pVTable[0];

	table[FOX] = pVTable[17];
	table[ES] = pVTable[42];
	table[DIP] = pVTable[82];
	table[RES] = pVTable[16];

	DestroyWindow(hWnd);
}

DWORD WINAPI InitGUI()
{
	while (GetModuleHandle(xorstr("d3d9.dll")) == NULL) 
		Sleep(250);

	Sleep(1000);
	HookCreateDevice();
	DX_Init(VTable);

	HOOK(EndScene, VTable[ES]);
	HOOK(Reset, VTable[RES]);
	HWND window = NULL;

	while (window == NULL)
		window = FindWindowA(NULL, xorstr("Knight OnLine Client"));

	oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
	return 0;
}

void UIMain() {
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitGUI, NULL, NULL, NULL);
}

HRESULT WINAPI HookCreateDevice()
{
	IDirect3D9* device = Direct3DCreate9(D3D_SDK_VERSION);
	if (!device)
		return D3DERR_INVALIDCALL;

	IDirect3D9_vtable = (DWORD*)*(DWORD*)device;
	device->Release();
	DWORD protectFlag;
	if (VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, &protectFlag))
	{
		*(DWORD*)&D3DCreateDevice_orig = IDirect3D9_vtable[CREATEDEVICE_VTI];
		*(DWORD*)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_hook;
		if (!VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), protectFlag, &protectFlag))
			return D3DERR_INVALIDCALL;
	}
	else 
		return D3DERR_INVALIDCALL;

	return D3D_OK;
}

HRESULT WINAPI D3DCreateDevice_hook(IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
	if (!itsMe) 
	{
		if (Engine->m_SettingsMgr == NULL) 
			Engine->m_SettingsMgr = new CSettingsManager();

		pPresentationParameters->Windowed = Engine->m_SettingsMgr->m_iRealFullScreen == 1 ? false : true;
		pPresentationParameters->PresentationInterval = Engine->m_SettingsMgr->m_iVsync == 1 ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		if (Engine->m_SettingsMgr->m_iVsync == 1)
			pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	return D3DCreateDevice_orig(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags | D3DCREATE_MULTITHREADED, pPresentationParameters, ppReturnedDeviceInterface);
}
