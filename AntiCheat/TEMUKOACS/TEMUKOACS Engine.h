#pragma once
#include "stdafx.h"
#include "HideCore.h"
#include "PearlAPI.h"
#include "PlayerBase.h"
#include "TableManager.h"
#include "SettingsManager.h"
#include "StringHelper.h"
#include "N3BASE/N3Base.h"
#include "N3BASE/N3UIBase.h"
#include "UIManager.h"
#include "LocalInput.h"
#include "UIState.h"
#include "UITargetBar.h"
#include "UITaskbarMain.h"
#include "UIMiniTaskbarMain.h"
#include "UIClanWindow.h"
#include "UISkillTree.h"
#include "UITradeItemDisplay.h"
#include "UIMiniMenu.h"
#include "UIGenieSub.h"
#include "UISeedHelper.h"
#include "UIPieceChange.h"
#include "UIWarp.h"
#include "UIAnvil.h"
#include "UILogin.h"
#include "UITradePrice.h"
#include "UITradeInventory.h"
#include "UITradeItemDisplaySpecial.h"
#include "MerchantManager.h"
#include "UIEventNotice.h"
#include "UITooltip.h"
#include "UIInventory.h"
#include "UIParty.h"
#include "UIHPBar.h"
#include "splash.h"

struct Handle_Thread
{
	uint32 Id;
	HANDLE handle;
	uint32 updateTime;
	DWORD startAddr;
};

typedef LONG    NTSTATUS;
typedef NTSTATUS(WINAPI* pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
typedef NTSTATUS(WINAPI* pGetProcessId)(HANDLE);
typedef NTSTATUS(WINAPI* pNtQIP)(HANDLE hProcess, ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);
#define NtCurrentProcess() (HANDLE)-1
#define ThreadBasicInformation 0

struct CLIENTS_ID
{
	unsigned long UniqueProcess;
	unsigned long UniqueThread;
};

typedef unsigned long ULONG_PTR;
typedef LONG KPRIORITY;
typedef ULONG_PTR KAFFINITY;
typedef KAFFINITY* PKAFFINITY;

typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS                ExitStatus;
	PVOID                   TebBaseAddress;
	CLIENTS_ID              ClientId;
	KAFFINITY               AffinityMask;
	KPRIORITY               Priority;
	KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

enum DEBUG_LEVEL
{
	DBG_NORMAL = 0,
	DBG_WARNING = 1,
	DBG_ERROR = 2,
	DBG_SYS = 3
};

enum GuardOpCodes
{
	VERSION = 0,
	INITIATE = 1,
	ALIVE = 2,
	DISCONNECT = 3,
	HACKTOOL = 3,
	CLIENTNAME_ERROR = 4,
	SPEED_HACK = 5,
	WALL_HACK = 6,
	DLL_INJECT = 7,
	BYPASS = 8,
	DEBUGGER = 9,
	TBL = 10,
	VIRTUAL_MACHINE = 11,
	UGLY_PROCESS = 12,
	UGLY_DRIVER = 13,
	MULTI_CLIENT = 14,
	TIMEOUT = 15,
	CLOSE = 17,
	HDW_CHECK = 18,
	THREAD = 19,
	DUMP_SCAN = 20,
	WINDOW = 21,
	THREAD_AFK = 22
};

class TEMUKOACSEngine {
public:
	struct ENGINE_SETTINGS {
		uint8 ACS_Validation;
		uint8 Button_Facebook;
		uint8 Button_Discord;
		uint8 Button_Live;
		uint8 Button_Support;
		std::string URL_Facebook;
		std::string URL_Discord;
		std::string URL_Live;
		std::string URL_KCbayi;
		uint32 KCMerchant_MinPrice;
		uint32 KCMerchant_MaxPrice;
		uint8 State_TempItemList;
		uint8 State_StatReset;
		uint8 State_SkillReset;
		uint8 State_PUS;
		ENGINE_SETTINGS() {
			ACS_Validation = 1;
			Button_Facebook = 0;
			Button_Discord = 0;
			Button_Live = 0;
			Button_Support = 0;
			URL_Facebook = "";
			URL_Discord = "";
			URL_Live = "";
			URL_KCbayi = "";
			KCMerchant_MinPrice = 0;
			KCMerchant_MaxPrice = 0;
			State_TempItemList = 0;
			State_StatReset = 0;
			State_SkillReset = 0;
			State_PUS = 0;
		}
	};
	TEMUKOACSEngine(std::string basePath);

	bool Securty();
	void OnLog(bool s_sSending, std::string logmsg, ...);
	void ShowDbgMsg(DEBUG_LEVEL type, std::string msg, ...);

	virtual ~TEMUKOACSEngine();
	static DWORD WINAPI EngineMain(TEMUKOACSEngine* e);
	static DWORD WINAPI SuspendCheck(TEMUKOACSEngine* e);
	HANDLE MainThread, ScanThread, SuspendCheckThread, LicenseThread;
	ENGINE_SETTINGS* EngineSettings;
	bool IsHWBreakpointExists();
	bool IsitaSandBox();
	User Player;
	bool Loading;
	bool power;
	void Disconnect();
	void Send(Packet* pkt);
	void Update();
	void StayAlive();
	void SendProcess(uint16 toWHO);
	bool fileExist(const char* fileName);
	void __stdcall Shutdown(std::string log);
	void SetVisible(DWORD vTable, bool type);
	void SetString(DWORD vTable, std::string str);
	void SetStringColor(DWORD vTable, DWORD color);
	std::string GetString(DWORD vTable);
	std::string GetStringFromPWEdit(DWORD vTable);
	void WriteString(DWORD value, char* vl);
	bool IsVisible(DWORD vTable);
	void GetChildByID(DWORD vTable, std::string id, DWORD& child);
	void GetBaseByChild(DWORD vTable, DWORD& base);
	DWORD rdword(DWORD ulBase, std::vector<int> offsets);
	DWORD ReadDWORD(DWORD ulBase, std::vector<int> offsets);
	DWORD ReadDWord(DWORD paddy);
	void SizeOfImage();
	bool DisableDebugPrivileges(DWORD dwPID);
	static bool AdjustSingleTokenPrivilege(HANDLE TokenHandle, LPCTSTR lpName, DWORD dwAttributes);
	int SetDebugPriv();

	// Self Module Check Methods
	PVOID AntiRevers(HMODULE dwModule);
	void HideModule(HINSTANCE hModule);
	void AntiHeaders(HINSTANCE hModule);
	void RemovePeHeader(DWORD ModuleBase);
	bool CloakDll(HMODULE hMod);
	void Checking(HMODULE hModule);
	void HideModuleFromPEB(HINSTANCE hInstance);
	void EraseHeaders(HINSTANCE hModule);
	void ScanCheck();
	HANDLE CreateUndedectedThread(void* testvoid);
	static void Scanner();
	static DWORD GetThreadEntryBase();
	static DWORD CalculateCallAddrRead(DWORD Addr);
	static int GetFunctionSize(DWORD Adress);
	static void Real_Send_Error();
	static DWORD GetThreadID();
	BOOL HardwareBreakpoints_GetThreadContext();
	int Anti_Debugging();
	bool IsInSandbox();
	bool detectSandbox();
	bool IsInsideVPC();
	DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep);
	inline int exists_regkey_value_str(HKEY hKey, char* regkey_s, char* value_s, char* lookup);
	inline int exists_regkey(HKEY hKey, char* regkey_s);
	bool CreateThreadSelf(LPTHREAD_START_ROUTINE callbackFunction, LPVOID lpParameter, uint32 id);
	void smsw();
	void IsUsingVirtualBox();
	void sidt();
	void CreateScanThreads();
	void obfuscation();
	void anti_dumper();
	bool isClientNameCorrect();
	std::string strClientName;
	void ReadClientName(std::string& strname);
	DWORD Get_proc_integrity_level();
	DWORD GetProcessIntegrityLevel();

	/*treahd handler*/
	void CheckRunTimeTBLs();
	void HeuristicClassCheckProc();
	BOOL CALLBACK HeuristicClassCheck(HWND hwnd, LPARAM lParam);
	void UserCheckThreadProc();
	void ProcessIntegrityLevelCheck();
	void HookChecker();
	void AlreadyRing3Hook(PVOID FuncAddr);
	void ClassScanProc();
	void ClassWindow(string WindowClasse, bool& isChecked);
	void InjectScanProc();

	BOOL CALLBACK HeuristicWindowCheck(HWND hwnd, LPARAM lParam);
	void HeuristicWindowCheckProc();
	// Title Scan Handler Methods
	void TileScanProc();
	bool TitleWindow(bool& isChecked, LPCSTR WindowTitle);
	void QueryCounterCountProc();
	void check_driver();
	void child_check();
	BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
	int windowCount;
	// Style Scan Handler Methods
	BOOL CALLBACK EnumWinHandle(HWND hWnd, LPARAM lparam);
	void style_check();

	// Exe Scan Handler Methods
	void TerminateProc_Func(char* ProcName, bool koexe);
	void Terminate_Scan();

	bool CheckTBLs();

	void Tbl_Scan();
	DWORD GetFileSizeOwn(char* FileName);
	bool DownloadFile(string fName, string tDir);
	string GetCurrentPath();

	void Dump_Scan();

	static inline void ThreadIdControl();
	static inline void ThreadModuleControl();
	static inline void RegisterAdressControl();
	static inline void ForceKillProcess();

	__inline static DWORD GetMemDWORD(DWORD ulBase);
	__inline static WORD GetMemWORD(DWORD ulBase);
	__inline static BYTE GetMemBYTE(DWORD ulBase);
	
	CSplash* Splash;
	HMODULE dModule;
	HANDLE n_TEMUKOACS;
	DWORD  n_dMyPid;
	void SetRegister(LPCSTR yol, LPCTSTR value, LPCSTR deger, char* tip);
	void SecurtyStard();
	void olly_crash();
	bool InstallAntiAttach();
	void Hardware_breakpointx();
	void Protection_Starter();
	DWORD ProtectProcess();

	inline void WriteInfoMessageExt(char* pMsg, DWORD dwColor);
	void WriteInfoMessage(char* pMsg, DWORD dwColor);
	void GetUiPos(DWORD vTable, POINT& pt);
	POINT GetUiPos(DWORD vTable);
	void SetState(DWORD vTable, DWORD state);
	uint32 GetState(DWORD vTable);
	void InitCallHook(DWORD hookFuncAddr, DWORD myFuncAddr);
	void SetMemArray(DWORD Adres, BYTE Deger, DWORD len);
	inline void CALLHOOK(DWORD Addr1, DWORD Addr2);
	DWORD CalculateCallAddrWrite(DWORD Addr1, DWORD Addr2);
	void SendChaoticExchange(uint16 nObjectID, uint32 nExchangeItemID);
	bool disableCameraZoom;
	void InitCameraZoom();
	void InitJmpHook(DWORD hookFuncAddr, DWORD myFuncAddr);
	inline void JMPHOOK(DWORD Addr1, DWORD Addr2);
	void InitSendHook();
	void InitRecvHook();
	inline BYTE GetByte(char* sBuf, int& index);
	inline DWORD GetDWORD(char* sBuf, int& index);
	inline int GetShort(char* sBuf, int& index);
	void SendItemAdd(uint32 itemID, uint16 count, uint32 gold, uint8 srcPos, uint8 dstPos, uint8 mode);
	void InitSetString();
	bool IsIn(DWORD vTable, int x, int y);
	LONG GetUiWidth(DWORD vTable);
	LONG GetUiHeight(DWORD vTable);
	size_t GetScrollValue(DWORD vTable);
	void SetScrollValue(DWORD vTable, int val);
	static long SelectRandomMob();
	static bool WINAPI WriteLog(std::string message);
	CPlayerBase* m_PlayerBase;
	CTableManager* tblMgr;
	CSettingsManager* m_SettingsMgr;
	std::string m_BasePath;
	std::string m_connectedIP;
	bool logState;
	bool drawMode;
	LPDIRECT3DDEVICE9 _pDevice;
	bool IsCRActive;
	CR_EVENT crEvent;
	uint32 moneyReq;
	DWORD m_zMob;
	CStringHelper* StringHelper;
	CUIManager* m_UiMgr;
	CUIStatePlug* uiState;
	CUITargetBarPlug* uiTargetBar;
	CUITaskbarMainPlug* uiTaskbarMain;
	CUIMiniTaskbarMainPlug* UIMiniTaskbarMain;
	CUIClanWindowPlug* uiClanWindowPlug;
	CUISkillTreePlug* uiSkillTreePlug;
	CUIMiniMenuPlug* uiMiniMenuPlug;
	CUIGenieSubPlug* uiGenieSubPlug;
	CUISeedHelperPlug* uiSeedHelperPlug;
	CUIPieceChangePlug* uiPieceChangePlug;
	CUIWarp* uiWarp;
	CUIAnvil* uiAnvil;
	CUILogin* uiLogin;
	CUITradePricePlug* uiTradePrice;
	CUITradeInventoryPlug* uiTradeInventory;
	CUITradeItemDiplayPlug* uiTradeItemDisplay;
	CUITradeItemDisplaySpecialPlug* uiTradeItemDisplaySpecial;
	CMerchantManager* m_MerchantMgr;
	CUIEventNoticePlug* uiEventNotice;
	CUITooltip* uiToolTip;
	CUIInventoryPlug* uiInventoryPlug;
	CUIPartyPlugin* uiPartyPlug;
	CUIHPBarPlug* uiHPBarPlug;
	// Recording
	
	bool isRecording;
	void StartRecording();
	void StopRecording();

	INLINE bool isInisRecording() { return isRecording; }

	/*Server Packet Handler*/
	void PointChange(Packet& pkt);
	void LevelChange(Packet& pkt);
	void HpChange(Packet& pkt);
	void MpChange(Packet& pkt);
	void TargetHpChange(Packet& pkt);
	void SendMYInfo(Packet& pkt);
	void GameStart(Packet& pkt);
	void ZoneChangeHandler(Packet& pkt);
	void NoticeHandler(Packet& pkt);
	void SendItemMove(Packet& pkt);
	void MerchantHandler(Packet& pkt);
	void ItemUpgradeHandler(Packet& pkt);

	/*Handle Packet*/
	void TEMUKOACSHandler(Packet& pkt);
	void StartNoviceHandler(Packet& pkt);
private:
	void InitPlayer();
	map<int, Handle_Thread> threadArray;
};

extern TEMUKOACSEngine * Engine;