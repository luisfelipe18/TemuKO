#include "TEMUKOACS Engine.h"
#include "md5.h"
#include "PearlGui.h"
#include <thread>
#include <future>
#include "encrypt.h"
#include "HideCore.h"
#include "HDRReader.h"
#include <ostream>
#include "LicenseEngine.h"
// < Voice Chat >
#include "AudioRecorder.h"
#include "AudioListener.h"
#include "WaveCodec.h"
// </ Voice Chat >

AudioRecorder VoiceRecorder;
AudioListener VoiceListener;

CWaveCodec wv;

extern bool HideThread(HANDLE hThread);
extern void Shutdown(string message = "");
extern string WtoString(WCHAR s[]);
extern string strToLower(string str);

vector<ProcessInfo> processTMP;
string tmpGraphics = xorstr("<unknown>");
string tmpProcessor = xorstr("<unknown>");

typedef struct
{
private:
	void* pVoid;
public:
	unsigned int Size;
	BYTE* Data;
} RECV_DATA;

typedef int (WINAPI* MyOldRecv) (SOCKET, uint8*, int, int);
typedef int (WSAAPI* MyRecv) (SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int (WINAPI* MySend) (SOCKET, char*, int, int);
typedef int (WINAPI* MyConnect) (SOCKET, const sockaddr*, int);
typedef int (WSAAPI* MyWSAConnect) (SOCKET, const sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
typedef int (WSAAPI* MyWSAStartup) (WORD, LPWSADATA);
typedef int (WINAPI* MyTerminateProcess) (HANDLE hProcess, UINT uExitCode);
typedef int (WINAPI* MyExitProcess) (UINT uExitCode);
typedef int (WINAPI* MyLoadTBL) (const std::string& szFN);

MyOldRecv OrigOldRecv = NULL;
MyRecv OrigRecv = NULL;
MySend OrigSend = NULL;
MyConnect OrigConnect = NULL;
MyWSAConnect OrigWSAConnect = NULL;
MyWSAStartup OrigWSAStartup = NULL;
MyTerminateProcess OrigTerminateProcess = NULL;
MyExitProcess OrigExitProcess = NULL;
MyLoadTBL OrigLoadTBL = NULL;

bool m_bAllowAlive = true;
bool m_bGameStard = false;
bool m_bAlive = false;
bool m_bGameConnected = false;
bool ShieldExitCheck = false;

INLINE bool isIngame() { return m_bGameConnected; }
INLINE bool isInHookGame() { return m_bAllowAlive; }
INLINE bool isInAlive() { return m_bAlive; }
INLINE bool isInGameStard() { return m_bGameStard; }
INLINE bool isInShieldExit() { return ShieldExitCheck; }

HANDLE thisProc = NULL;
DWORD KO_ADR = 0x0;
const DWORD KO_WH = 0x6C0;
const DWORD KO_PTR_PKT = 0x00E47844;
const DWORD KO_SND_FNC = 0x00497E40;
const DWORD KO_ACC = 0x00C957B8;

DWORD GameThreadID;
DWORD GameThreadEntryBase;
DWORD MyThreadID;
DWORD MyThreadEntryBase;

void LM_Send(Packet* pkt);
void __stdcall LM_Shutdown(std::string log, std::string graphicCards = tmpGraphics, std::string processor = tmpProcessor);
void LM_StayAlive();
void LM_SendProcess(uint16 toWHO);

std::mutex log_lock;
struct tm* timeInfo;

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

bool WINAPI TEMUKOACSEngine::WriteLog(std::string message)
{
	const std::lock_guard<std::mutex> lock(log_lock);

	// < time info >
	time_t rawtime;
	time(&rawtime);
	timeInfo = localtime(&rawtime);
	// </ time info >

	if (!dirExists(xorstr("TEMUKOACS"))) {
		if (!CreateDirectoryA(xorstr("TEMUKOACS"), NULL))
			return false;
	}

	ofstream logFile;
	logFile.open(xorstr("TEMUKOACS\\engine_log.txt"));
	logFile << xorstr("[") << std::to_string(timeInfo->tm_hour).c_str() << xorstr(":") << std::to_string(timeInfo->tm_min).c_str() << xorstr("]\t") << message.c_str() << endl;
	logFile.close();

	return true;
}

std::string forbiddenModules[] = { /*xorstr("dbk64"), xorstr("dbk32"),*/ xorstr("pchunter"), xorstr("hacker"), xorstr("PROCEXP152"), xorstr("BlackBoneDrv10"), xorstr("since"), xorstr("ntice"), xorstr("winice"), xorstr("syser"), xorstr("77fba431") };

DWORD WINAPI DriverScan(LPVOID lParam)
{
	VIRTUALIZER_START
		while (true) {
			Sleep(3000);
			LPVOID drivers[ARRAY_SIZE];
			DWORD cbNeeded;
			int cDrivers, i;
			WCHAR szDriver[ARRAY_SIZE];
			if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
			{
				cDrivers = cbNeeded / sizeof(drivers[0]);
				for (i = 0; i < cDrivers; i++)
				{
					if (GetDeviceDriverBaseNameW(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
					{
						string driverName = strToLower(WtoString(szDriver));
						for (string fbDriver : forbiddenModules) {
							if (driverName.find(strToLower(fbDriver)) != std::string::npos) {
								string s1 = xorstr("An 3rd party tools has been detected on the system: %s\n");
								string s2 = xorstr("If you don't use any hacking stuff, ");
								string s3 = xorstr("please restart computer and try again.");
								Shutdown(string_format(s1 + s2 + s3, fbDriver.c_str()));
							}
						}
					}
				}
			}
			Engine->StayAlive();
		}
	VIRTUALIZER_END
}

DWORD WINAPI TEMUKOACSEngine::SuspendCheck(TEMUKOACSEngine* e)
{
	Sleep(1000);
	DWORD TimeTest1 = 0, TimeTest2 = 0;
	while (true)
	{
		TimeTest1 = TimeTest2;
		TimeTest2 = GetTickCount();
		if (TimeTest1 != 0)
		{
			Sleep(1000);
			if ((TimeTest2 - TimeTest1) > 5000)
				e->Shutdown(xorstr("Time travel has been detected."));
		}
		if (WaitForSingleObject(e->MainThread, 1) == WAIT_OBJECT_0)
			e->Shutdown(xorstr("All the pieces of the game can't be working together."));
	}
}

DWORD WINAPI TEMUKOACSEngine::EngineMain(TEMUKOACSEngine* e)
{
	VIRTUALIZER_START
		e->LicenseThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LicenseEngine, e, NULL, NULL);
		e->ScanThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DriverScan, NULL, NULL, NULL);
		e->SuspendCheckThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SuspendCheck, e, NULL, NULL);
		HideCore::HideModuleLinks(GetModuleHandle(NULL));
		HideCore::HideModuleLinks(GetModuleHandleA(xorstr("KnightOnLine.exe")));
		while (true)
		{
			e->Update();

			if (e->IsitaSandBox())
				e->Shutdown(xorstr("Sandbox"));

			if (WaitForSingleObject(e->ScanThread, 1) == WAIT_OBJECT_0 
				|| WaitForSingleObject(e->SuspendCheckThread, 1) == WAIT_OBJECT_0
				/*|| WaitForSingleObject(e->LicenseThread, 1) == WAIT_OBJECT_0*/)
				Engine->Shutdown(xorstr("All the pieces of the game can't work together."));

			Sleep(15000);
		}
	VIRTUALIZER_END
	return TRUE;
}

void TEMUKOACSEngine::Update() {
	if (isInAlive()) 
	{
		if (thisProc == NULL) 
			thisProc = GetCurrentProcess();
		if (KO_ADR == 0x0) 
			KO_ADR = *(DWORD*)KO_PTR_CHR;
		if (isInGameStard()) 
		{
			uint8 auth;
			auth = *(uint8*)(KO_ADR + KO_WH);
			if (this->Player.Authority != auth) {
				if (auth == USER || auth == GAMEMASTER || auth == BANNED) {
					tmpGraphics = "";
					for (string gpu : Player.GPU)
						tmpGraphics += xorstr(" | ") + gpu;
					this->Player.Authority = (UserAuthority)auth;
					Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_AUTHINFO));
					result << auth << tmpGraphics << Player.CPU;
					this->Send(&result);
				}
			}
			if (m_PlayerBase == NULL)
				m_PlayerBase = new CPlayerBase();

			m_PlayerBase->UpdateFromMemory();
		}
	}
}

DWORD Read4Bytes(DWORD paddy) {
	return *(int*)paddy;
}

void __stdcall LM_Shutdown(std::string log, std::string graphicCards, std::string processor) {
	if (isInAlive() && isInGameStard()) {
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_LOG));
		result << log << graphicCards << processor;
		LM_Send(&result);
	}
	Shutdown(log);
}

void TEMUKOACSEngine::Shutdown(std::string log) {
	tmpGraphics = "";
	for (string gpu : Player.GPU)
		tmpGraphics += xorstr(" | ") + gpu;
	LM_Shutdown(log, tmpGraphics, Player.CPU);
}

void LM_Send(Packet* pkt) {
	if (isInAlive() && Engine->power) {
		uint8 opcode = pkt->GetOpcode();
		uint8* out_stream = nullptr;
		uint16 len = (uint16)(pkt->size() + 1);

		out_stream = new uint8[len];
		out_stream[0] = pkt->GetOpcode();

		if (pkt->size() > 0)
			memcpy(&out_stream[1], pkt->contents(), pkt->size());

		BYTE* ptrPacket = out_stream;
		SIZE_T tsize = len;

		__asm
		{
			mov ecx, KO_PTR_PKT
			mov ecx, DWORD ptr ds : [ecx]
			push tsize
			push ptrPacket
			call KO_SND_FNC
		}

		delete[] out_stream;
	}
}

std::string getHWID() {
	HW_PROFILE_INFO hwProfileInfo;
	GetCurrentHwProfile(&hwProfileInfo);
	string hwidWString = hwProfileInfo.szHwProfileGuid;
	string hwid(hwidWString.begin(), hwidWString.end());
	return hwid;
}

void GetMacHash(uint16& MACOffset1, uint16& MACOffset2);
uint16 GetCPUHash();

uint16 GetVolumeHash()
{
	DWORD SerialNum = 0;
	GetVolumeInformationA(xorstr("C:\\"), NULL, 0, &SerialNum, NULL, NULL, NULL, 0);
	uint16 nHash = (uint16)((SerialNum + (SerialNum >> 16)) & 0xFFFF);
	return nHash;
}

int64 GetHardwareID()
{
	uint16 MACData1, MACData2 = 0;
	GetMacHash(MACData1, MACData2);
	return _atoi64(string_format(xorstr("%d%d%d%d"), MACData1, MACData2, GetCPUHash(), GetVolumeHash()).c_str());
}

uint16 HashMacAddress(PIP_ADAPTER_INFO info)
{
	uint16 nHash = 0;
	for (uint32 i = 0; i < info->AddressLength; i++)
		nHash += (info->Address[i] << ((i & 1) * 8));
	return nHash;
}

void GetMacHash(uint16& MACOffset1, uint16& MACOffset2)
{
	IP_ADAPTER_INFO AdapterInfo[32];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return;

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	MACOffset1 = HashMacAddress(pAdapterInfo);
	if (pAdapterInfo->Next)
		MACOffset2 = HashMacAddress(pAdapterInfo->Next);

	if (MACOffset1 > MACOffset2)
	{
		uint16 TempOffset = MACOffset2;
		MACOffset2 = MACOffset1;
		MACOffset1 = TempOffset;
	}
}

uint16 GetCPUHash()
{
	int CPUInfo[4] = { 0, 0, 0, 0 };
	__cpuid(CPUInfo, 0);
	uint16 nHash = 0;
	uint16* nPointer = (uint16*)(&CPUInfo[0]);
	for (uint16 i = 0; i < 8; i++)
		nHash += nPointer[i];

	return nHash;
}

bool isInHookSystem = false;

void HookStardSystem()
{
	Engine->InitCameraZoom();
	Engine->m_PlayerBase = new CPlayerBase();
	Engine->m_SettingsMgr = new CSettingsManager();
	Engine->m_SettingsMgr->Init();
	//Plugins
	Engine->uiState = new CUIStatePlug();
	Engine->uiTargetBar = new CUITargetBarPlug();
	Engine->uiTaskbarMain = new CUITaskbarMainPlug();
	Engine->UIMiniTaskbarMain = new CUIMiniTaskbarMainPlug();
	Engine->uiClanWindowPlug = new CUIClanWindowPlug();
	Engine->uiSkillTreePlug = new CUISkillTreePlug();
	Engine->uiMiniMenuPlug = new CUIMiniMenuPlug();
	Engine->uiGenieSubPlug = new CUIGenieSubPlug();
	Engine->uiSeedHelperPlug = new CUISeedHelperPlug();
	Engine->uiPieceChangePlug = new CUIPieceChangePlug();
	Engine->uiTradePrice = new CUITradePricePlug();
	Engine->uiTradeInventory = new CUITradeInventoryPlug();
	Engine->uiTradeItemDisplay = new CUITradeItemDiplayPlug();
	Engine->uiTradeItemDisplaySpecial = new CUITradeItemDisplaySpecialPlug();
	Engine->m_MerchantMgr = new CMerchantManager();
	Engine->uiAnvil = new CUIAnvil();
	Engine->uiToolTip = new CUITooltip();
	//Engine->uiInventoryPlug = new CUIInventoryPlug();//Windows 7 Giriþ Sorunu
	Engine->uiPartyPlug = new CUIPartyPlugin();
	Engine->uiHPBarPlug = new CUIHPBarPlug();
	//Engine->uiEventNotice = new CUIEventNoticePlug();
	isInHookSystem = true;
}

void __cdecl HandlePacket(Packet pkt) 
{
	if (!isInAlive()
		|| !Engine->power)
		return;

	uint8 sOpCode = pkt.GetOpcode();
	switch (sOpCode)
	{
	case WIZ_POINT_CHANGE:
		Engine->PointChange(pkt);
		break;
	case WIZ_LEVEL_CHANGE:
		Engine->LevelChange(pkt);
		break;
	case WIZ_HP_CHANGE:
		Engine->HpChange(pkt);
		break;
	case WIZ_MSP_CHANGE:
		Engine->MpChange(pkt);
		break;
	case WIZ_TARGET_HP:
		Engine->TargetHpChange(pkt);
		break;
	case WIZ_MYINFO:
		Engine->SendMYInfo(pkt);
		break;
	case WIZ_GAMESTART:
		Engine->GameStart(pkt);
		break;
	case WIZ_ZONE_CHANGE:
		Engine->ZoneChangeHandler(pkt);
		break;
	case WIZ_NOTICE:
		Engine->NoticeHandler(pkt);
		break;
	case WIZ_ITEM_MOVE:
		Engine->SendItemMove(pkt);
		break;
	case WIZ_MERCHANT:
		Engine->MerchantHandler(pkt);
		break;
	case WIZ_ITEM_UPGRADE:
		Engine->ItemUpgradeHandler(pkt);
		break;
	case WIZ_HOOK_GUARD:
		Engine->TEMUKOACSHandler(pkt);
		break;
	default:
		break;
	}
	Engine->m_PlayerBase->UpdateGold();
}

std::mutex recv_mutex;

void RecvMake(RECV_DATA* pRecv)
{
	const std::lock_guard<std::mutex> lock(recv_mutex);

	Packet pkt;

	unsigned int length = pRecv->Size;
	if (length > 0)
		length--;

	pkt = Packet(pRecv->Data[0]);
	if (length > 0)
	{
		pkt.resize(length);
		memcpy((void*)pkt.contents(), &pRecv->Data[1], length);
	}

	HandlePacket(pkt);
}

bool WINAPI hkRECV(RECV_DATA* pRecv, void* pParam)
{
	if (pRecv->Size < 1)
		return true;

	if (pRecv->Data[0] == WIZ_WAREHOUSE)
		Engine->Player.m_iClanBank = false;

	if (pRecv->Data[0] == WIZ_CLANWAREHOUSE)
	{
		pRecv->Data[0] = WIZ_WAREHOUSE;

		if (!Engine->Player.isClanBank())
			Engine->Player.m_iClanBank = true;
	}

	__asm
	{
		MOV ECX, DWORD PTR DS : [00E47878h]
		PUSH pParam
		PUSH pRecv
		MOV EAX, KO_RECV_FUNC
		CALL EAX
	}

	async(RecvMake, pRecv).get();
}

void TEMUKOACSEngine::InitRecvHook()
{
	*(DWORD*)KO_RECV_PTR = (DWORD)hkRECV;
}

/* Connection Gain or Lose */

int WINAPI ConnectDetour(SOCKET s, const sockaddr* name, int namelen) {
	struct sockaddr_in* addr_in = (struct sockaddr_in*)name;
	char* _s = inet_ntoa(addr_in->sin_addr);
	string hostAddress = string(_s);
	uint16_t port;
	port = htons(addr_in->sin_port);

	if (port == 15001)
		m_bGameConnected = true;

	Engine->m_connectedIP = hostAddress;

	if (hostAddress != SERVER_IP) {
		if (Engine->m_UiMgr != NULL)
			Engine->m_UiMgr->ShowMessageBox(xorstr("Connection Refused"), xorstr("Incompatibility detected between server and client."), Ok, PARENT_LOGIN);

		return WSAECONNREFUSED;
	}
	m_bAlive = true;
	return OrigConnect(s, name, namelen);
}

int WSAAPI WSAStartupDetour(WORD wVersionRequired, LPWSADATA lpWSAData) {
	m_bAlive = true;
	return OrigWSAStartup(wVersionRequired, lpWSAData);
}
int WSAAPI WSAConnectDetour(SOCKET s, const sockaddr* name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS) {
	struct sockaddr_in* addr_in = (struct sockaddr_in*)name;
	char* _s = inet_ntoa(addr_in->sin_addr);
	string hostAddress = string(_s);
	Engine->m_connectedIP = hostAddress;

	if (hostAddress != SERVER_IP) {
		if (Engine->m_UiMgr != NULL)
			Engine->m_UiMgr->ShowMessageBox(xorstr("Connection Refused"), xorstr("Incompatibility detected between server and client."), Ok, PARENT_LOGIN);

		return WSAECONNREFUSED;
	}
	m_bAlive = true;
	return OrigWSAConnect(s, name, namelen, lpCalleeData, lpCalleeData, lpSQOS, lpGQOS);
}

/* ----------------------- */

void TEMUKOACSEngine::InitJmpHook(DWORD hookFuncAddr, DWORD myFuncAddr)
{
	SetMemArray(hookFuncAddr, 0x90, 6);
	JMPHOOK(hookFuncAddr, myFuncAddr);
}

inline void TEMUKOACSEngine::JMPHOOK(DWORD Addr1, DWORD Addr2)
{
	BYTE jmp[] = { 0xE9,0,0,0,0 };
	DWORD diff = CalculateCallAddrWrite(Addr2, Addr1);

	memcpy(jmp + 1, &diff, 4);
	WriteProcessMemory(HANDLE(-1), (LPVOID)Addr1, jmp, 5, 0);
}

void SendHWID()
{
	m_bAlive = true;
	char AccName[25];
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)KO_ACC, AccName, sizeof(AccName), NULL);
	string m_strAccountID = string(AccName);
	uint16 MACData1, MACData2 = 0;
	GetMacHash(MACData1, MACData2);
	int64 UserHardwareID = GetHardwareID();
	char hwid[255];
	string res = md5(to_string(UserHardwareID) + string(hwid));
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_COM));
	result << uint8(22) << m_strAccountID << uint32(MACData1 + MACData2) << res;
	Engine->Send(&result);
}

// Packet simulation
#define MAX_CRC_BYTE 10000
#define MAX_PACKET_SIZE	(1024 * 100)

char* sPacket;
uint8_t PacketMain = 0, PacketCounter = 0;
int32_t pSiz = 0, Send_Index = 0, ReturnAdress = 0;
DWORD HookAddres = 0, PushAdress = 0, BackToAdress = 0, SendErrorRetnAddr = 0x00499539;
uint8 header = 0, subCode = 0;
uint32 nPrice, nItemID;
uint16 sCount;
uint8 bSrcPos, bDstPos, bMode;

uint16 nObjectID = 0;
uint32 nExchangeItemID = 0;

__declspec(naked) void Real_Send() {

	__asm
	{
		MOV EAX, [ESP]
		MOV ReturnAdress, EAX
		MOV EAX, [ESP + 4]
		MOV sPacket, EAX
		MOV AL, BYTE PTR DS : [EAX]
		MOV PacketMain, AL
		MOV EAX, [ESP + 8]
		MOV pSiz, EAX
	}

	_asm pushad
	_asm pushfd

	Engine->ThreadIdControl();
	Engine->ThreadModuleControl();
	Engine->RegisterAdressControl();

	if (PacketMain == WIZ_SEL_CHAR)
	{
		SendHWID();
	} 
	else if (PacketMain == WIZ_MERCHANT)
	{
		Send_Index = 0;

		header = Engine->GetByte(sPacket, Send_Index);
		subCode = Engine->GetByte(sPacket, Send_Index);

		if (subCode == MERCHANT_ITEM_ADD) // item add to merchant
		{
			nItemID = Engine->GetDWORD(sPacket, Send_Index);
			sCount = Engine->GetShort(sPacket, Send_Index);
			nPrice = Engine->GetDWORD(sPacket, Send_Index);
			bSrcPos = Engine->GetByte(sPacket, Send_Index);
			bDstPos = Engine->GetByte(sPacket, Send_Index);
			bMode = Engine->GetByte(sPacket, Send_Index);

			Engine->SendItemAdd(nItemID, sCount, nPrice, bSrcPos, bDstPos, bMode);
		}
	}
	else if (PacketMain == WIZ_LOGOUT) 
	{
		if (Engine->m_UiMgr != NULL)
			Engine->m_UiMgr->Release();

		Engine->Player.logOut = true;
		TerminateProcess(GetCurrentProcess(), 0);
	}
	else if (PacketMain == WIZ_GAMESTART 
		|| PacketMain == WIZ_MYINFO 
		|| PacketMain == WIZ_NOTICE) 
	{
		if (!isInGameStard())
			m_bGameStard = true;
	}
	else if (PacketMain == WIZ_ZONE_CHANGE) 
	{
		header = Engine->GetByte(sPacket, Send_Index);
		subCode = Engine->GetByte(sPacket, Send_Index);

		if (subCode == 3)
			Engine->Player.isTeleporting = true;
		else if (subCode == 2)
			Engine->Player.isTeleporting = false;
	}
	else if (PacketMain == WIZ_ITEM_UPGRADE)
	{
		if (Engine->uiPieceChangePlug != NULL)
		{
			if (Engine->uiPieceChangePlug->m_bAuto) 
			{
				Send_Index = 0;

				header = Engine->GetByte(sPacket, Send_Index);
				subCode = Engine->GetByte(sPacket, Send_Index);
				if (subCode == 5) // chaotic kýrdýrma
				{
					nObjectID = Engine->GetShort(sPacket, Send_Index);
					nExchangeItemID = Engine->GetDWORD(sPacket, Send_Index);

					Engine->uiPieceChangePlug->m_nObjectID = nObjectID;
					Engine->uiPieceChangePlug->m_nExchangeItemID = nExchangeItemID;
					Engine->uiPieceChangePlug->m_bAutoExchangeStarted = true;

					Engine->SendChaoticExchange(nObjectID, nExchangeItemID);
				}
			}
		}
	}
	else if (PacketMain == WIZ_WAREHOUSE) 
	{
		if (Engine->Player.isClanBank())
		{
			sPacket[0] = WIZ_CLANWAREHOUSE;
			Engine->Player.m_iClanBank = false;
		}
	}

	_asm popfd
	_asm popad

	_asm
	{
		PUSH - 1
		PUSH PushAdress
		JMP BackToAdress
	}
}

DWORD WINAPI GetThreadIDWithHandle(HANDLE hThread)
{
	VIRTUALIZER_START

		NTSTATUS ntStatus;
	HANDLE hDupHandle;
	_THREAD_BASIC_INFORMATION ThreadBasicInfo;

	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
	if (NtQueryInformationThread == NULL)
		return 0;

	ntStatus = NtQueryInformationThread(hThread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), NULL);
	CloseHandle(hDupHandle);

	if (ntStatus != 0)
		return 0;

	VIRTUALIZER_END
		return ThreadBasicInfo.ClientId.UniqueThread;
}

inline void TEMUKOACSEngine::ThreadIdControl()
{
	VIRTUALIZER_START

	if (GameThreadID != GetThreadID() && MyThreadID != GetThreadID())
	{
		/*ExitProcess(0);
		ForceKillProcess();
		exit(0);
		ShieldExitCheck = true;*/
	}

	if (GameThreadEntryBase != GetThreadEntryBase() && MyThreadEntryBase != GetThreadEntryBase())
	{
		/*ExitProcess(0);
		ForceKillProcess();
		exit(0);
		ShieldExitCheck = true;*/
	}

	VIRTUALIZER_END
}

inline void TEMUKOACSEngine::ThreadModuleControl()
{
	VIRTUALIZER_START
	if (GetThreadIDWithHandle(GetCurrentThread()) != GameThreadID && GetThreadIDWithHandle(GetCurrentThread()) != MyThreadID)
	{
		/*ExitProcess(0);
		ForceKillProcess();
		exit(0);
		ShieldExitCheck = true;*/
	}
	VIRTUALIZER_END
}
DWORD _eax = 0, _ecx = 0, _edx = 0, _ebx = 0, _esp = 0, _ebp = 0, _esi = 0, _edi = 0;
inline void TEMUKOACSEngine::RegisterAdressControl()
{
	VIRTUALIZER_START
		_asm
	{
		mov _eax, eax
		mov _ecx, ecx
		mov _edx, edx
		mov _ebx, ebx
		mov _esp, esp
		mov _ebp, ebp
		mov _esi, esi
		mov _edi, edi
	}

	if (_eax == RealSend - 19 
		|| _ecx == RealSend - 19
		|| _edx == RealSend - 19
		|| _ebx == RealSend - 19 
		|| _esp == RealSend - 19 
		|| _ebp == RealSend - 19 
		|| _esi == RealSend - 19 
		|| _edi == RealSend - 19)
	{
		/*ExitProcess(0);
		ForceKillProcess();
		exit(0);
		ShieldExitCheck = true;*/
	}
	VIRTUALIZER_END
}

inline void TEMUKOACSEngine::ForceKillProcess()
{
	VIRTUALIZER_START
		__asm
	{
		CALL GetCurrentProcess_echo
		PUSH 0
		PUSH EAX
		CALL ZwTerminateProcess_echo
		JMP continue_echo

		GetCurrentProcess_echo :
		OR EAX, 0xFFFFFFFF
			RETN

			ZwTerminateProcess_echo :
		MOV EAX, 0x2A
			CALL DWORD PTR FS : [0xC0]
			RETN 0x8

			continue_echo :
	}
	VIRTUALIZER_END
}

void TEMUKOACSEngine::SendItemAdd(uint32 itemID, uint16 count, uint32 gold, uint8 srcPos, uint8 dstPos, uint8 mode)
{
	uint8 isKC = Engine->uiTradePrice->m_bIsKC ? 1 : 0;
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_MERCHANT));
	result << ((uint8)MERCHANT_ITEM_ADD) << itemID << count << gold << srcPos << dstPos << mode << isKC;
	Send(&result);
	m_MerchantMgr->SetRecentItemAddReq(result);
}

inline int TEMUKOACSEngine::GetShort(char* sBuf, int& index)
{
	index += 2;
	return *(short*)(sBuf + index - 2);
};

inline DWORD TEMUKOACSEngine::GetDWORD(char* sBuf, int& index)
{
	index += 4;
	return *(DWORD*)(sBuf + index - 4);
};

inline BYTE TEMUKOACSEngine::GetByte(char* sBuf, int& index)
{
	int t_index = index;
	index++;
	return (BYTE)(*(sBuf + t_index));
};

const DWORD KO_SND = 0x00497E40;

void TEMUKOACSEngine::InitSendHook()
{
	HookAddres = KO_SND;
	PushAdress = *(DWORD*)(HookAddres + 4);
	BackToAdress = KO_SND + 7;
	InitJmpHook(KO_SND, (DWORD)Real_Send);
}

int WINAPI SendDetour(SOCKET s, char* buf, int len, int flags) {
	Packet pkt;
	uint16 header;
	uint16 length;
	uint16 footer;
	memcpy(&header, buf, 2);
	memcpy(&length, buf + 2, 2);
	memcpy(&footer, buf + 4 + length, 2);
	uint8* in_stream = new uint8[length];
	memcpy(in_stream, buf + 4, length);

	if (length > 0)
		length--;

	pkt = Packet(in_stream[0], (size_t)length);
	if (length > 0)
	{
		pkt.resize(length);
		memcpy((void*)pkt.contents(), &in_stream[1], length);
	}

	delete[]in_stream;

	uint8 cmd = pkt.GetOpcode();

	return OrigSend(s, buf, len, flags);
}

string strToLower(string str) {
	for (auto& c : str) c = tolower(c);
	return str;
}

string WtoString(WCHAR s[]) {
	wstring ws(s);
	string ret(ws.begin(), ws.end());
	return ret;
}

int currentID = 0;
vector<string> activeWindows;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (currentID != dwProcessId) return TRUE;
	char String[255];
	if (!hWnd)
		return TRUE;
	if (!::IsWindowVisible(hWnd))
		return TRUE;
	if (!SendMessage(hWnd, WM_GETTEXT, sizeof(String), (LPARAM)String))
		return TRUE;
	if (IsWindowVisible(hWnd))
	{
		char wnd_title[256];
		GetWindowTextA(hWnd, wnd_title, sizeof(wnd_title));
		activeWindows.push_back(string((wnd_title)));
	}
	return TRUE;
}

bool inArray(vector<string> arr, string obj) {
	for (string i : arr) {
		if (obj == i) return true;
	}
	return false;
}

void TEMUKOACSEngine::InitPlayer() {
	if (thisProc == NULL) thisProc = GetCurrentProcess();
	if (KO_ADR == 0x0) ReadProcessMemory(thisProc, (LPCVOID)KO_PTR_CHR, &KO_ADR, sizeof(DWORD), 0);
	ofstream logFile;
	if (!dirExists(xorstr("TEMUKOACS"))) {
		CreateDirectoryA(xorstr("TEMUKOACS"), NULL);
	}
	logFile.open(xorstr("TEMUKOACS\\init_log.txt"));
	Player.Nick = "";
	Player.Level = 0;
	Player.RebLevel = 0;
	Player.NationPoint = 0;
	Player.KnightCash = 0;
	Player.KnightCashBonus = 0;
	Player.ddAc = 0;
	Player.axeAc = 0;
	Player.swordAc = 0;
	Player.maceAc = 0;
	Player.arrowAc = 0;
	Player.spearAc = 0;
	Player.isTeleporting = false;
	Player.isGameStat = false;
	Player.m_iClanBank = false;
	Player.logOut = false;

	strClientName.clear();
	windowCount = 0;
	n_TEMUKOACS = GetCurrentProcess();
	n_dMyPid = GetCurrentProcessId();

	ReadProcessMemory(thisProc, (LPVOID)(KO_ADR + KO_OFF_ZONE), &Player.zone, sizeof(Player.zone), 0);
	this->Player.Authority = USER;
	uint16 MACData1, MACData2 = 0;
	GetMacHash(MACData1, MACData2);
	this->Player.MAC = uint32(MACData1 + MACData2);
	//Init GPU info
	DISPLAY_DEVICE DevInfo;
	DevInfo.cb = sizeof(DISPLAY_DEVICE);
	DWORD iDevNum = 0;
	logFile << xorstr("-- TEMUKOACS Initializing --") << endl;
	while (EnumDisplayDevices(NULL, iDevNum, &DevInfo, 0))
	{
		if (inArray(this->Player.GPU, DevInfo.DeviceString)) {
			iDevNum++;
			continue;
		}
		this->Player.GPU.push_back(DevInfo.DeviceString);
		iDevNum++;
		logFile << xorstr("------ GPU: ") << DevInfo.DeviceString << endl;
	}
	tmpGraphics = "";
	for (string gpu : Player.GPU)
		tmpGraphics += xorstr(" | ")+ gpu;
	//Init processor info
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];
	char CPUBrandString[0x40] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}
	this->Player.CPU = string(CPUBrandString) + xorstr(" | ") + to_string(siSysInfo.dwNumberOfProcessors) + xorstr(" Core(s)");
	logFile << xorstr("------ CPU: ") << this->Player.CPU.c_str() << endl;
	tmpProcessor = Player.CPU;
	//Init hwid info
	this->Player.HWID = GetHardwareID();
	//Init screen info
	ScreenInfo* screen = new ScreenInfo();
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	screen->height = desktop.bottom;
	screen->width = desktop.right;
	this->Player.Screen = screen;
	//Init processes
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		cProcesses = cbNeeded / sizeof(DWORD);
		for (i = 0; i < cProcesses; i++)
		{
			if (aProcesses[i] != 0) {
				char szProcessName[MAX_PATH];
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
				if (NULL != hProcess)
				{
					HMODULE hMod;
					DWORD cbNeeded;
					if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
					{
						GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
						ProcessInfo procInfo;
						procInfo.id = aProcesses[i];
						procInfo.name = szProcessName;
						currentID = aProcesses[i];
						activeWindows.clear();
						EnumWindows(EnumWindowsProc, NULL);
						for (string windowName : activeWindows) {
							procInfo.windows.push_back(windowName);
						}
						this->Player.Processes.push_back(procInfo);
					}
				}
			}
		}
	}
	logFile.close();

	processTMP = Player.Processes;
	OrigConnect = (MyConnect)DetourFunction((PBYTE)connect, (PBYTE)ConnectDetour);
	OrigWSAConnect = (MyWSAConnect)DetourFunction((PBYTE)WSAConnect, (PBYTE)WSAConnectDetour);
	OrigWSAStartup = (MyWSAStartup)DetourFunction((PBYTE)WSAStartup, (PBYTE)WSAStartupDetour);

	MainThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)EngineMain, this, NULL, NULL);
}

void LM_SendProcess(uint16 toWHO) {
	processTMP.clear();
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		cProcesses = cbNeeded / sizeof(DWORD);
		for (i = 0; i < cProcesses; i++)
		{
			if (aProcesses[i] != 0) {
				char szProcessName[MAX_PATH];
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
				if (NULL != hProcess)
				{
					HMODULE hMod;
					DWORD cbNeeded;
					if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
					{
						GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
						ProcessInfo procInfo;
						procInfo.id = aProcesses[i];
						procInfo.name = szProcessName;
						currentID = aProcesses[i];
						activeWindows.clear();
						EnumWindows(EnumWindowsProc, NULL);
						for (string windowName : activeWindows) {
							procInfo.windows.push_back(windowName);
						}
						processTMP.push_back(procInfo);
					}
				}
			}
		}
	}

	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_PROCINFO));
	result << uint16(toWHO) << uint32(processTMP.size());

	for (ProcessInfo proc : processTMP) {
		result << int(proc.id) << string(proc.name) << int(proc.windows.size());
		for (string window : proc.windows)
			result << string(window);
	}
	LM_Send(&result);
}

void TEMUKOACSEngine::SendProcess(uint16 toWHO) {
	LM_SendProcess(toWHO);
}

void TEMUKOACSEngine::Disconnect() {
	m_bAllowAlive = false;
}

void TEMUKOACSEngine::Send(Packet* pkt) {
	LM_Send(pkt);
}

void LM_StayAlive() {
	if (isInAlive() 
		&& isInHookGame() 
		&& isIngame()) 
	{
		Sleep(60 * SECOND);
		char AccName[25];
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)KO_ACC, AccName, sizeof(AccName), NULL);
		string m_strAccountID = string(AccName);
		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_ALIVE));
		string public_key = md5(xorstr("0x") + std::to_string(PL_VERSION) + xorstr("52824") + m_strAccountID);
		result << public_key;
		Engine->Send(&result);
	}
}

void TEMUKOACSEngine::StayAlive() {
	LM_StayAlive();
}

int WINAPI hTerminateProcess(HANDLE hProcess, UINT uExitCode) {
	if (hProcess == GetCurrentProcess())
		if (Engine->m_UiMgr != NULL)
			Engine->m_UiMgr->Release();
	return OrigTerminateProcess(hProcess, uExitCode);
}

int WINAPI hExitProcess(UINT uExitCode) {
	if (Engine->m_UiMgr != NULL)
		Engine->m_UiMgr->Release();
	return OrigExitProcess(uExitCode);
}

TEMUKOACSEngine::~TEMUKOACSEngine() {
	if (Splash)
		delete Splash;
}

//Uif Hook

DWORD rdwordExt(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(DWORD)))
		return(*(DWORD*)(ulBase));

	return 0;
}

DWORD TEMUKOACSEngine::rdword(DWORD ulBase, std::vector<int> offsets)
{
	DWORD ibase = rdwordExt(ulBase);
	for (size_t i = 0; i < offsets.size() - 1; i++)
	{
		int offset = offsets[i];
		ibase += offset;
		int ibase1 = ibase;
		ibase = rdwordExt(ibase);
	}

	return ibase;
}

DWORD TEMUKOACSEngine::ReadDWORD(DWORD ulBase, std::vector<int> offsets)
{
	DWORD ibase;
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)ulBase, &ibase, sizeof(ibase), NULL);
	for (size_t i = 0; i < offsets.size() - 1; i++)
	{
		int offset = offsets[i];
		ibase += offset;
		int ibase1 = ibase;
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)ibase, &ibase, sizeof(ibase), NULL);
	}
	return ibase;
}

DWORD TEMUKOACSEngine::ReadDWord(DWORD paddy)
{
	DWORD retval;
	__asm
	{
		mov ebx, [paddy]
		xor eax, eax
		mov eax, DWORD PTR DS : [ebx]
		mov retval, eax
	}
	return retval;
}

std::string m_strReplaceString = "";
DWORD m_dvTable = 0x0;

void __declspec(naked) SetStringAsm()
{
	_asm
	{
		MOV ECX, m_dvTable
		MOV EAX, OFFSET m_strReplaceString
		PUSH EAX
		CALL KO_SET_STRING_FUNC
		RET
	}
}

void TEMUKOACSEngine::SetString(DWORD vTable, std::string str)
{
	m_dvTable = vTable;
	m_strReplaceString = str;
	SetStringAsm();
}

const   DWORD   KO_SET_SCROLL_VALUE_FUNC = 0x4296B0;
DWORD m_dvBase = 0x0;
int m_iVal = 0;

void __declspec(naked) SetScrollValueAsm()
{
	_asm
	{
		MOV ECX, m_dvBase
		MOV ESI, m_dvTable
		MOV EAX, m_iVal
		PUSH EAX
		CALL KO_SET_SCROLL_VALUE_FUNC
		RET
	}
}

void TEMUKOACSEngine::SetScrollValue(DWORD vTable, int val)
{
	m_dvBase = *(DWORD*)(vTable + 0x124);
	m_dvTable = vTable;
	m_iVal = val;
	SetScrollValueAsm();
}

void TEMUKOACSEngine::WriteString(DWORD value, char* vl)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)value, (LPVOID)vl, sizeof(vl), 0);
}

std::string TEMUKOACSEngine::GetString(DWORD vTable)
{
	char* buff;
	DWORD value = vTable + 308;

	buff = (char*)malloc(100);
	ReadProcessMemory(GetCurrentProcess(), (void*)value, (LPVOID)buff, 100, 0);

	return std::string(buff);
}

std::string TEMUKOACSEngine::GetStringFromPWEdit(DWORD vTable)
{
	char* buff;
	DWORD value = vTable + 140;

	buff = (char*)malloc(100);
	ReadProcessMemory(GetCurrentProcess(), (void*)value, (LPVOID)buff, 100, 0);

	return std::string(buff);
}

DWORD m_dChild = 0x0;
std::string m_strGetChildID;

void __declspec(naked) GetChildByIDAsm()
{
	_asm
	{
		MOV ECX, [m_dvTable]
		MOV EAX, OFFSET m_strGetChildID
		PUSH EAX
		CALL KO_GET_CHILD_BY_ID_FUNC
		MOV m_dChild, EAX
		RET
	}
}

void TEMUKOACSEngine::GetChildByID(DWORD vTable, std::string id, DWORD& child)
{
	m_dvTable = vTable;
	m_dChild = child;
	m_strGetChildID = id;
	GetChildByIDAsm();
	if (m_dChild == 0)
	{
		child = NULL;
		return;
	}
	child = m_dChild;
}

void TEMUKOACSEngine::GetBaseByChild(DWORD vTable, DWORD& base)
{
	base = *(DWORD*)(vTable + 0xBC);
}

DWORD m_dBool = 0;

void __declspec(naked) SetVisibleAsm()
{
	_asm
	{
		MOV ECX, m_dvTable
		MOV EAX, m_dBool
		PUSH EAX
		CALL KO_SET_VISIBLE_FUNC
		RET
	}
}

void TEMUKOACSEngine::SetVisible(DWORD vTable, bool type)
{
	if (vTable == 0x0) return;
	m_dvTable = vTable;
	m_dBool = type ? 1 : 0;
	SetVisibleAsm();
}

bool TEMUKOACSEngine::IsVisible(DWORD vTable)
{
	bool isVisible = *(BYTE*)(vTable + 252) == 1 ? true : false;
	return isVisible;
}

TEMUKOACSEngine::TEMUKOACSEngine(std::string basePath) {
	power = true;
	m_BasePath = basePath;
	ScanThread = NULL;
	IsCRActive = false;
	Loading = false;
	disableCameraZoom = false;
	tblMgr = NULL;
	moneyReq = 0;
	m_zMob = 0;
	StringHelper = NULL;
	m_PlayerBase = NULL;
	m_UiMgr = NULL;
	uiState = NULL;
	uiTargetBar = NULL;
	uiTaskbarMain = NULL;
	UIMiniTaskbarMain = NULL;
	uiClanWindowPlug = NULL;
	uiSkillTreePlug = NULL;
	uiMiniMenuPlug = NULL;
	uiGenieSubPlug = NULL;
	m_SettingsMgr = NULL;
	uiSeedHelperPlug = NULL;
	uiPieceChangePlug = NULL;
	uiLogin = NULL;
	uiWarp = NULL;
	uiTradePrice = NULL;
	uiTradeInventory = NULL;
	uiTradeItemDisplay = NULL;
	uiTradeItemDisplaySpecial = NULL;
	uiAnvil = NULL;
	m_SettingsMgr = NULL;
	m_MerchantMgr = NULL;
	uiEventNotice = NULL;
	uiToolTip = NULL;
	uiInventoryPlug = NULL;
	uiPartyPlug = NULL;
	uiHPBarPlug = NULL;
	_pDevice = NULL;
	logState = true;
	drawMode = true;
	m_connectedIP = "";
	EngineSettings = new ENGINE_SETTINGS();
	InitPlayer();
	tblMgr->Init();
}

inline void TEMUKOACSEngine::WriteInfoMessageExt(char* pMsg, DWORD dwColor)
{
	int iMsgLen = strlen(pMsg);
	char* pParam = new char[iMsgLen + 33];
	DWORD	dwParamAddr = (DWORD)pParam;

	memset(pParam, 0, iMsgLen + 33);
	memcpy(pParam + 32, pMsg, iMsgLen);

	*(int*)(pParam + 20) = iMsgLen;
	*(DWORD*)(pParam + 4) = (DWORD)pParam + 32;
	*(DWORD*)(pParam + 24) = 0x1F;

	__asm
	{
		MOV ECX, DWORD PTR DS : [0xE47878]
		MOV ECX, DWORD PTR DS : [ecx + 0x1C8]

		push dwColor
		push dwParamAddr
		mov  eax, KO_ADD_INFO_MSG_FUNC
		call eax
	}

	delete[] pParam;
}

void TEMUKOACSEngine::WriteInfoMessage(char* pMsg, DWORD dwColor)
{
	WriteInfoMessageExt(pMsg, dwColor);
}

POINT TEMUKOACSEngine::GetUiPos(DWORD vTable)
{
	uintptr_t** ptrVtable = (uintptr_t**)vTable;

	POINT pt;
	pt.x = (LONG)ptrVtable[55];
	pt.y = (LONG)ptrVtable[56];
	return pt;
}

void TEMUKOACSEngine::GetUiPos(DWORD vTable, POINT& pt)
{
	uintptr_t** ptrVtable = (uintptr_t**)vTable;
	pt.x = (LONG)ptrVtable[55];
	pt.y = (LONG)ptrVtable[56];
}

DWORD m_iParam1;
DWORD m_iParam2;
DWORD m_iParam3;

void __declspec(naked) SetStateAsm()
{
	_asm
	{
		MOV ECX, m_dvTable
		MOV EAX, m_iParam1
		PUSH EAX
		CALL KO_UI_SET_STATE_FUNC
		RET
	}
}

void TEMUKOACSEngine::SetState(DWORD vTable, DWORD state)
{
	m_dvTable = vTable;
	m_iParam1 = state;
	SetStateAsm();
}

uint32 TEMUKOACSEngine::GetState(DWORD vTable)
{
	return Read4Bytes(vTable + 0xD0);
}

void SetMemBYTE(DWORD Adres, BYTE Deger)
{
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)Adres, &Deger, 1, NULL);
}

void TEMUKOACSEngine::SetMemArray(DWORD Adres, BYTE Deger, DWORD len)
{
	for (DWORD i = 0; i < len; i++)
		SetMemBYTE(Adres + i, Deger);
}

void TEMUKOACSEngine::InitCallHook(DWORD hookFuncAddr, DWORD myFuncAddr)
{
	SetMemArray(hookFuncAddr, 0x90, 5);
	CALLHOOK(hookFuncAddr, myFuncAddr);
}

DWORD TEMUKOACSEngine::CalculateCallAddrWrite(DWORD Addr1, DWORD Addr2)
{
	return Addr1 - Addr2 - 5;
}

inline void TEMUKOACSEngine::CALLHOOK(DWORD Addr1, DWORD Addr2)
{
	BYTE call[] = { 0xE8,0,0,0,0 };
	DWORD diff = CalculateCallAddrWrite(Addr2, Addr1);

	memcpy(call + 1, &diff, 4);
	WriteProcessMemory(HANDLE(-1), (LPVOID)Addr1, call, 5, 0);
}

void __stdcall CameraZoom_Hook(float fDelta)
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	if (Engine->disableCameraZoom)
		return;

	__asm
	{
		MOV ECX, thisPtr
		PUSH fDelta
		MOV EAX, KO_CAMERA_ZOOM_FUNC
		CALL EAX
	}
}

void TEMUKOACSEngine::InitCameraZoom()
{
	InitCallHook(KO_CAMERA_ZOOM_CALL_ADDR, (DWORD)CameraZoom_Hook);
}

void TEMUKOACSEngine::SendChaoticExchange(uint16 nObjectID, uint32 nExchangeItemID)
{
	uint8 bStockInn = uiPieceChangePlug->m_bStock ? 1 : 0;
	uint8 bSell = uiPieceChangePlug->m_bSell ? 1 : 0;
	Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_CHAOTIC_EXCHANGE));
	result << nObjectID << nExchangeItemID << bStockInn << bSell;
	Send(&result);
}

const	DWORD	KO_SET_STRING_COLOR_FUNC = 0x0040F710;
const	DWORD	KO_SET_STRING_COLOR_RET_ADDR = 0x0040F716;
void __declspec(naked) SetStringColorAsm()
{
	_asm
	{
		MOV ECX, m_dvTable
		MOV EAX, m_iParam1
		PUSH EAX
		CALL KO_SET_STRING_COLOR_FUNC
		RET
	}
}

void TEMUKOACSEngine::SetStringColor(DWORD vTable, DWORD color)
{
	m_dvTable = vTable;
	m_iParam1 = color;
	SetStringColorAsm();
}

std::string purchasingPriceSearch = xorstr("purchasing price");

string loadingArray[] = { xorstr("Allocating Terrain..."), xorstr("Loading "), xorstr("Loading Effect Data..."), xorstr("Loading Terrain Patch Data..."), xorstr("Loading Lightmap Data..."), xorstr("Loading colormap"),
xorstr("Loading Objects..."), xorstr("Loading Character Data..."), xorstr("Loading Information") };

void __stdcall SetString_Hook(const std::string& szString)
{
	DWORD thisPtr;
	__asm
	{
		MOV thisPtr, ECX
	}

	m_strReplaceString = szString;

	if (Engine->StringHelper == NULL)
		Engine->StringHelper = new CStringHelper();

	if (Engine->StringHelper->IsContains(m_strReplaceString, xorstr("Loading Information[")))
	{
		if (Engine->Loading)
			Engine->Loading = false;

		if (!isInHookSystem)
			HookStardSystem();
	}

	if (m_strReplaceString == xorstr("Allocating Terrain..."))
		Engine->Loading = true;

	if (Engine->m_MerchantMgr != NULL) 
	{
		if (Engine->m_MerchantMgr->NeedToCheckTooltip() 
			|| Engine->m_MerchantMgr->NeedToCheckDisplayTooltip())
		{
			if (Engine->StringHelper->IsContains(m_strReplaceString, purchasingPriceSearch))
			{
				if (Engine->m_MerchantMgr->UpdateTooltipString(m_strReplaceString)) // kc ise renk deðiþtir
					Engine->SetStringColor(thisPtr, 0xff7a70);
			}
		}
	}

	//printf("Hook String %s\n", m_strReplaceString.c_str());
	_asm
	{
		MOV ECX, thisPtr
		MOV EAX, OFFSET m_strReplaceString

		PUSH EAX
		CALL KO_SET_STRING_FUNC
	}
}
void TEMUKOACSEngine::InitSetString()
{
	*(DWORD*)KO_SET_STRING_PTR = (DWORD)SetString_Hook;
}

void __declspec(naked) IsInAsm()
{
	_asm
	{
		MOV ECX, [m_dvTable]
		MOV EAX, m_iParam2
		PUSH EAX
		MOV EAX, m_iParam1
		PUSH EAX
		CALL KO_UIBASE_IS_IN_FUNC
		MOV m_dBool, EAX
		RET
	}
}

bool TEMUKOACSEngine::IsIn(DWORD vTable, int x, int y)
{
	m_dvTable = vTable;
	m_iParam1 = y;
	m_iParam2 = x;
	IsInAsm();

	if (m_dBool == 0x00000001)
		return true;
	return false;
}

bool TEMUKOACSEngine::fileExist(const char* fileName)
{
	std::ifstream infile(string(m_BasePath + fileName).c_str());
	return infile.good();
}

LONG TEMUKOACSEngine::GetUiWidth(DWORD vTable)
{
	uintptr_t** ptrVtable = (uintptr_t**)vTable;
	LONG right = (LONG)ptrVtable[57];
	LONG left = (LONG)ptrVtable[55];

	return right - left;
}

LONG TEMUKOACSEngine::GetUiHeight(DWORD vTable)
{
	uintptr_t** ptrVtable = (uintptr_t**)vTable;
	LONG bottom = (LONG)ptrVtable[58];
	LONG top = (LONG)ptrVtable[56];

	return bottom - top;
}

size_t TEMUKOACSEngine::GetScrollValue(DWORD vTable)
{
	DWORD ECX = *(DWORD*)(vTable + 0x124);
	return *(uint32*)(ECX + 0x134);
}

bool TEMUKOACSEngine::IsitaSandBox()
{
	unsigned char bBuffering;
	unsigned long aCreateProcesses = (unsigned long)GetProcAddress(GetModuleHandleA(xorstr("KERNEL32.dll")), xorstr("CreateProcessA"));

	ReadProcessMemory(GetCurrentProcess(), (void*)aCreateProcesses, &bBuffering, 1, 0);

	if (bBuffering == 0xE9)
		return 1;
	else
		return 0;

	return false;
}

bool TEMUKOACSEngine::IsHWBreakpointExists()
{
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT));
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	HANDLE hThread = GetCurrentThread();
	if (GetThreadContext(hThread, &ctx) == 0)
		return false;

	if ((ctx.Dr0) || (ctx.Dr1) || (ctx.Dr2) || (ctx.Dr3))
		return true;
	else
		return false;

	return false;
}

DWORD KO_ADR_CHR = 0x0;
DWORD KO_ADR_DLG = 0x0;

BYTE ReadByte(DWORD vTable)
{
	return *(byte*)vTable;
}

long ReadLong(DWORD vTable)
{
	return *(long*)vTable;
}

long TEMUKOACSEngine::SelectRandomMob()
{
	if (KO_ADR_CHR == 0x0 
		|| KO_ADR_DLG == 0x0) 
	{
		KO_ADR_CHR = *(DWORD*)KO_PTR_CHR;
		KO_ADR_DLG = *(DWORD*)KO_DLG;
	}

	long EBP, ESI, EAX, FEnd, Tick, base_addr, LID, LBase;
	EBP = ReadLong(ReadLong(KO_FLDB) + 0x34);
	FEnd = ReadLong(ReadLong(EBP + 4) + 4);
	ESI = ReadLong(EBP);
	Tick = GetTickCount();
	do {
		base_addr = ReadLong(ESI + 0x10);
		if (base_addr == 0) return 0;
		if (ReadLong(base_addr + KO_OFF_NATION) == 0 & ReadByte(base_addr + 0x2A0) != 10)
		{
			LID = ReadLong(base_addr + KO_OFF_ID);
			LBase = (base_addr);
		}
		EAX = ReadLong(ESI + 8);
		if (ReadLong(ESI + 8) != FEnd)
		{
			do {
				EAX = ReadLong(EAX);
			} while (ReadLong(EAX) != FEnd & GetTickCount() - Tick < 150);
			ESI = EAX;
		}
		else {
			EAX = ReadLong(ESI + 4);
			do {
				ESI = EAX;
				EAX = ReadLong(EAX + 4);
			} while (ESI == Read4Bytes(EAX + 8) & GetTickCount() - Tick < 150);
			if (ReadLong(ESI + 8) != EAX) {
				ESI = EAX;
			}
		}
	} while (ESI != EBP & GetTickCount() - Tick < 150);
	if (LBase == 0) return 0;
	return LID;
}
// Recording
void TEMUKOACSEngine::StartRecording()
{
	return;

	if (!isInAlive()
		|| !isInHookGame()
		|| !isIngame()
		|| isInisRecording()
		|| Engine->uiPartyPlug == NULL 
		|| !Engine->uiPartyPlug->isInParty())
		return;

	if (Engine->m_SettingsMgr == NULL)
	{
		Engine->m_SettingsMgr = new CSettingsManager();
		Engine->m_SettingsMgr->Init();
	}

	if (Engine->m_SettingsMgr->m_micState != 1)
		return;

	if(isInisRecording())
		return;

	isRecording = true;
	VoiceRecorder.Record();
}

void TEMUKOACSEngine::StartNoviceHandler(Packet& pkt)
{
	return;
	if (Engine->m_SettingsMgr == NULL)
	{
		Engine->m_SettingsMgr = new CSettingsManager();
		Engine->m_SettingsMgr->Init();
	}

	uint16 uid;
	uint64 len;
	pkt >> uid >> len;

	if (len == 0) {
		if (Engine->uiPartyPlug != NULL)
			Engine->uiPartyPlug->UserSpeaking(uid, true);
		return;
	}

	if (Engine->m_SettingsMgr->m_muteAll == 1
		|| (Engine->isInisRecording() && Engine->m_SettingsMgr->m_muteSpeaking == 1))
		return;

	unsigned char* buff = new unsigned char[len];
	memcpy(&buff[0], &pkt.contents()[sizeof(uint16) + sizeof(uint64)], len);

	wv.G729_InitDec();

	short* dBuff = new short[22050];

	wv.G729_DeCompress(buff, dBuff, len, 0);

	if (Engine->uiPartyPlug != NULL)
		Engine->uiPartyPlug->UserSpeaking(uid, true);

	VoiceListener.start();
	VoiceListener.input((unsigned char*)dBuff);
}

void TEMUKOACSEngine::StopRecording(){
	isRecording = false;
}

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	return;
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;

	if (WIM_DATA == uMsg)
	{
		BYTE* compressed = new BYTE[pwh->dwBytesRecorded];

		wv.G729_StartEnc();

		int len = wv.G729_Compress(reinterpret_cast <short*>(pwh->lpData), compressed, pwh->dwBytesRecorded);

		Packet result(WIZ_HOOK_GUARD, uint8(WIZ_HOOK_VOICE));
		result << uint64(len);

		result.append(&compressed[1], len);

		Engine->Send(&result);

		if(Engine->isInisRecording())
			waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));
	}
}

__declspec(naked) DWORD TEMUKOACSEngine::GetThreadID()
{
	VIRTUALIZER_START
		_asm
	{
		MOV EAX, DWORD PTR FS : [0x18]
		MOV EAX, [EAX + 0x24]
		RETN
	}
	VIRTUALIZER_END
}

__inline DWORD TEMUKOACSEngine::GetMemDWORD(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(DWORD)))
	{
		return(*(DWORD*)(ulBase));
	}
	return 0;
}

__inline WORD TEMUKOACSEngine::GetMemWORD(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(WORD)))
	{
		return(*(WORD*)(ulBase));
	}
	return 0;
}

__inline BYTE TEMUKOACSEngine::GetMemBYTE(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(BYTE)))
	{
		return(*(BYTE*)(ulBase));
	}

	return 0;
}

__inline float GetMemFLOAT(DWORD ulBase)
{
	if (!IsBadReadPtr((VOID*)ulBase, sizeof(DWORD)))
	{
		return(*(float*)(ulBase));
	}

	return 0;
}

__declspec(naked) DWORD TEMUKOACSEngine::GetThreadEntryBase()
{
	VIRTUALIZER_START
		_asm
	{
		MOV EAX, DWORD PTR FS : [0x18]
		RETN
	}
	VIRTUALIZER_END
}

DWORD TEMUKOACSEngine::CalculateCallAddrRead(DWORD Addr){
	return rdwordExt(Addr) + Addr + 5;
}

int TEMUKOACSEngine::GetFunctionSize(DWORD Adress)
{
	for (int i = 0; i < MAX_CRC_BYTE; i++)
		if (GetMemBYTE(Adress + i) == 0xCC && GetMemDWORD(Adress + i + 1) == 0xCCCCCCCC)
		{
			return i - sizeof(DWORD);
		}
	return 0;
}

void TEMUKOACSEngine::ScanCheck()
{
	HANDLE h1 = CreateUndedectedThread(Scanner);
	HideThread(h1);
}

HANDLE TEMUKOACSEngine::CreateUndedectedThread(void* testvoid)
{
	CONTEXT a;
	HANDLE hThread;
	memset(&a, 0, sizeof(CONTEXT));
	DWORD randaddr = (DWORD)(GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlLoadString")) + 0x430;
	hThread = CreateThread(0, 0, LPTHREAD_START_ROUTINE(randaddr), 0, CREATE_SUSPENDED, 0);
	a.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &a);
	a.Eax = (DWORD)testvoid;
	SetThreadContext(hThread, &a);
	ResumeThread(hThread);
	CloseHandle(hThread);
	return hThread;
}

void TEMUKOACSEngine::Scanner()
{
	VIRTUALIZER_START

	MyThreadID = GetCurrentThreadId();
	MyThreadEntryBase = GetThreadEntryBase();

	DWORD Hook_Send = reinterpret_cast<unsigned int>(Real_Send);
	Hook_Send = CalculateCallAddrRead(Hook_Send + 1) - 1;

	int Hook_SendSize = GetFunctionSize(Hook_Send);

	VIRTUALIZER_END

	while (true)
	{
		VIRTUALIZER_START
		char class_name[80]; memset(class_name, 0x00, 80);
		char title[80]; memset(title, 0x00, 80);
		BOOL HWNDCheck = FALSE;
		DWORD pid = 0;
		HWND hwnd = GetForegroundWindow();
		GetClassName(hwnd, class_name, sizeof(class_name));
		GetWindowText(hwnd, title, sizeof(title));
		GetWindowThreadProcessId(hwnd, &pid);

		if (isInShieldExit())
		{
			/*ExitProcess(0);
			ForceKillProcess();
			exit(0);*/
		}

		VIRTUALIZER_END
		Sleep(100);
	}
}

__declspec(naked) void TEMUKOACSEngine::Real_Send_Error()
{
	VIRTUALIZER_START
		if (isInGameStard())
			PacketCounter--;
	VIRTUALIZER_END

		_asm CALL DWORD PTR DS : [0xB7B1D4]
		_asm JMP SendErrorRetnAddr
}

void TEMUKOACSEngine::Checking(HMODULE hModule)
{
	VIRTUALIZER_START

	char szAppDirectoryPath[MAX_PATH] = "";
	n_TEMUKOACS = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	dModule = hModule;
	GetModuleFileName(dModule, szAppDirectoryPath, MAX_PATH);

	RealSend = KO_SND + 7;
	//RealRecv = KO_RECV;

	DWORD HookAddres = KO_SND;
	PushAdress = *(DWORD*)(HookAddres + 4);
	BackToAdress = KO_SND + 7;

	DWORD Hook_Send = reinterpret_cast<unsigned int>(Real_Send);
	DWORD Hook_Recv = reinterpret_cast<unsigned int>(hkRECV);
	DWORD Hook_Send_Error = reinterpret_cast<unsigned int>(Real_Send_Error);

	SetMemArray(KO_SND, 0x90, 6);
	JMPHOOK(KO_SND, Hook_Send);

	SetMemArray(0x00499533, 0x90, 6);
	JMPHOOK(0x00499533, Hook_Send_Error);

	GameThreadID = GetThreadID();
	GameThreadEntryBase = GetThreadEntryBase();

	typedef NTSTATUS(NTAPI* pNtSetInformationThread)(UUID __RPC_FAR* Uuid);

	HMODULE hntdll = NULL;
	hntdll = GetModuleHandleA("Rpcrt4.dll");
	if (!hntdll)
		return;

	// Get NtSetInformationThread
	pNtSetInformationThread NtSIT = (pNtSetInformationThread)GetProcAddress(hntdll, "UuidCreateSequential");

	VIRTUALIZER_END
}