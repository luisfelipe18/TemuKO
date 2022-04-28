#pragma once

class CUser;
class CGameServerDlg;
class CKnights;
class Packet;

class CKnightsManager  
{
public:

	static void KnightsVsLoginList(CUser* pUser);
	static void CreateKnights(CUser* pUser, Packet & pkt);
	static void KnightsJoin(CUser* pUser, Packet & pkt);
	static void KnightsJoinReq(CUser* pUser, Packet & pkt);
	static void KnightsWithdraw(CUser* pUser, Packet & pkt);
	static void KnightsRemove(CUser* pUser, Packet & pkt);
	static void KnightsDestroy(CUser* pUser);
	static void KnightsAdmit(CUser* pUser, Packet & pkt);
	static void KnightsReject(CUser* pUser, Packet & pkt);
	static void KnightsPunish(CUser* pUser, Packet & pkt);
	static void KnightsChief(CUser* pUser, Packet & pkt);
	static void KnightsViceChief(CUser* pUser, Packet & pkt);
	static void KnightsOfficer(CUser* pUser, Packet & pkt);
	static void KnightsAllMember(CUser* pUser);
	static void KnightsCurrentMember(CUser* pUser, Packet & pkt);
	static void KnightsAllList(CUser* pUser, Packet & pkt);
	static void KnightsRegisterSymbol(CUser* pUser, Packet & pkt);
	static void KnightsRequestSymbolVersion(CUser* pUser, Packet & pkt);
	static void KnighsRequestSymbols(CUser* pUser, Packet & pkt);
	static void KnightsGetSymbol(CUser* pUser, uint16 sClanID);
	static void KnightsHandoverList(CUser* pUser, Packet & pkt);
	static void KnightsHandoverReq(CUser* pUser, Packet & pkt);
	static void KnightsPointMethodModify(CUser* pUser, Packet & pkt);
	static bool CheckAlliance(CKnights * pMainClan, CKnights * pTargetClan);
	static void KnightsAllianceProcess(CUser* pUser, Packet & pkt, uint8 opcode);
	static void KnightsAllianceCreate(CUser* pUser, Packet & pkt);
	static void KnightsAllianceRequest(CUser* pUser, Packet & pkt);
	static void KnightsAllianceInsert(CUser* pUser, Packet & pkt);
	static void KnightsAllianceRemove(CUser* pUser, Packet & pkt);
	static void KnightsAlliancePunish(CUser* pUser, Packet & pkt);
	static void KnightsAllianceList(CUser* pUser, Packet & pkt);
	static void ListTop10Clans(CUser *pUser);
	static void DonateNPReq(CUser *pUser, Packet & pkt);
	static void DonateNP(CUser *pUser, Packet & pkt);
	static void DonationList(CUser *pUser, Packet & pkt);

	static void ClanNoticeUpdateProcess(CUser *pUser, Packet & pkt);
	static void UserMemoUpdateProcess(CUser *pUser, Packet & pkt);

	static bool AddKnightsUser(int index, std::string & strUserID, std::string & strMemo, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, int32 Loyalty);
	static void AddUserDonatedNP(int index, std::string & strUserID, uint32 nDonatedNP, bool isAddClanFund = false);
	static bool RemoveKnightsUser(int index, std::string & strUserID);
	static bool LoadKnightsIndex(int index);
	static bool LoadAllKnights();
	static void UpdateKnightsGrade(uint16 sClanID, uint8 byFlag);
	static void UpdateClanPoint(uint16 sClanID, int32 nChangeAmount);

	// database requests go here
	static void ReqKnightsPacket(CUser* pUser, Packet & pkt);
	static void ReqKnightsSave(Packet & pkt);
	static void ReqCreateKnights(CUser* pUser, Packet & pkt);
	static void ReqKnightsJoin(CUser* pUser, Packet & pkt);
	static void ReqKnightsLeave(CUser* pUser, Packet & pkt);
	static void ReqKnightsRemove(CUser* pUser, Packet & pkt);
	static void ReqKnightsAdmit(CUser* pUser, Packet & pkt);
	static void ReqKnightsReject(CUser* pUser, Packet & pkt);
	static void ReqKnightsChief(CUser* pUser, Packet & pkt);
	static void ReqKnightsViceChief(CUser* pUser, Packet & pkt);
	static void ReqKnightsOfficer(CUser* pUser, Packet & pkt);
	static void ReqKnightsPunish(CUser* pUser, Packet & pkt);
	static void ReqKnightsHandover(CUser* pUser, Packet & pkt);
	static void ReqKnightsDestroy(CUser* pUser, Packet & pkt);
	static void ReqKnightsAllMember(CUser *pUser, Packet & pkt);
	static void ReqKnightsRegisterSymbol(CUser *pUser, Packet & pkt);
	static void ReqKnightsGradeUpdate(Packet & pkt);
	static void ReqKnightsDonateNP(CUser *pUser, Packet & pkt);
	static void ReqKnightsRefundNP(Packet & pkt);
	static void ReqKnightsUpdateNP(Packet & pkt);
	static void ReqKnightsPointMethodChange(CUser* pUser, Packet & pkt);
	static void ReqKnightsClanNoticeUpdate(Packet & pkt);
	static void ReqKnightsUserMemoUpdate(Packet & pkt);
	static void ReqKnightsCapeUpdate(Packet & pkt);

	static void ReqKnightsAllianceCreate(CUser *pUser, Packet & pkt);
	static void ReqKnightsAllianceInsert(CUser *pUser, Packet & pkt);
	static void ReqKnightsAllianceRemove(CUser *pUser, Packet & pkt);
	static void ReqKnightsAlliancePunish(CUser *pUser, Packet & pkt);
	static void ReqKnightsAllianceNoticeUpdate(Packet & pkt);
	static void RecvKnightsAllList(Packet & pkt);

	static int GetKnightsIndex( int nation );
	static bool IsAvailableName( const char* strname);	
	static void PacketProcess(CUser* pUser, Packet & pkt);	
};

enum KNIGHT_ALLY_TYPES
{
	ALLY_CREATE = 44,
	ALLY_INSERT = 46,
	ALLY_REMOVE = 47,
	ALLY_PUNISH = 48,
	ALLY_DESTROY = 49,
};