#pragma once

#define MAX_CLAN_USERS			50
#define MIN_NATIONAL_POINTS		500
#define MIN_NP_TO_DONATE		1000
#define MAX_CLAN_NOTICE_LENGTH	128
#define MAX_USER_MEMO_LENGTH	20

class CUser;
struct _KNIGHTS_USER
{
	uint8    bLevel;
	uint8    bFame;
	uint16   sClass;
	std::string strUserName;
	std::string strMemo;
	uint32	nLoyalty;
	uint32	nDonatedNP;
	int32	nLastLogin;

	INLINE _KNIGHTS_USER() { Initialise(); }
	INLINE void Initialise()
	{
		strUserName.clear();
		strMemo.clear();
		nDonatedNP = 0;
		nLoyalty = 0;
		bLevel = 0;
		bFame = 0;
		sClass = 0;
		nLastLogin = 0;
	}
};

enum ClanTypeFlag
{
	ClanTypeNone		= 0,
	ClanTypeTraining	= 1,
	ClanTypePromoted	= 2,
	ClanTypeAccredited5	= 3,
	ClanTypeAccredited4	= 4,
	ClanTypeAccredited3	= 5,
	ClanTypeAccredited2	= 6,
	ClanTypeAccredited1	= 7,
	ClanTypeRoyal5		= 8,
	ClanTypeRoyal4		= 9,
	ClanTypeRoyal3		= 10,
	ClanTypeRoyal2		= 11,
	ClanTypeRoyal1		= 12
};

class CKnights  
{
public:
	uint16	m_sIndex;
	uint8	m_byFlag;			// 1 : Clan, 2 : Knights
	uint8	m_byNation;			// nation
	uint8	m_byGrade;
	uint8	m_byRanking;
	std::atomic<uint16>	m_sMembers;

	std::string m_strName;
	std::string m_strChief, m_strViceChief_1, m_strViceChief_2, m_strViceChief_3;
	std::string m_strClanNotice;

	uint64	m_nMoney;
	uint16	m_sDomination;
	std::atomic<uint32>	m_nPoints;
	Atomic<uint32> m_nClanPointFund; // stored in national point form
	uint16	m_sMarkVersion, m_sMarkLen;
	char	m_Image[MAX_KNIGHTS_MARK];
	uint16	m_sCape;
	uint8	m_bCapeR, m_bCapeG, m_bCapeB;
	uint16	m_sAlliance;
	uint16	m_sAllianceReq;
	uint8	m_sClanPointMethod;
	uint8	bySiegeFlag;
	uint16	nLose,nVictory;

	uint8	sPremiumInUse, sClanBuffExp, sClanBuffLoyalty;
	uint32	sPremiumTime;
	uint8	GetOnlineMembers();

	typedef CSTLMap <_KNIGHTS_USER, std::string> KnightsUserArray;
	KnightsUserArray m_arKnightsUser;

	_ITEM_DATA m_sWarehouseArray[WAREHOUSE_MAX];
	uint32 m_iBank;

	INLINE uint16 GetID() { return m_sIndex; }
	INLINE uint16 GetAllianceID() { return m_sAlliance; }
	INLINE uint16 GetRequestedAllianceID() { return m_sAllianceReq; }
	INLINE uint16 GetCapeID() { return m_sCape; }
	INLINE std::string & GetName() { return m_strName; }
	INLINE uint8 GetClanPointMethod() { return m_sClanPointMethod; }

	INLINE uint32 GetInnCoins() { return m_iBank; }
	INLINE bool hasInnCoins(uint32 amount) { return (GetInnCoins() >= amount); }

	INLINE bool isPromoted() { return m_byFlag >= ClanTypePromoted; }
	INLINE bool isInAlliance() { return m_sAlliance > NO_CLAN; }
	INLINE bool isAnyAllianceRequest() { return m_sAllianceReq > NO_CLAN; }
	INLINE bool isAllianceLeader() { return GetAllianceID() == GetID(); }

	CKnights();

	// Attach our session to the clan's list & tell clannies we logged in.
	void OnLogin(CUser *pUser);
	void OnLoginAlliance(CUser *pUser);

	void ConstructClanNoticePacket(Packet *result);
	void UpdateClanNotice(std::string & clanNotice);

	void UpdateClanFund();
	void UpdateClanGrade();

	void CheckKnightBuffSystem();
	void CheckKnightCastellanCapesBuffSystem(CUser *pUser);
	void CheckKnightCastellanCapesBuffSystem();

	bool LoadUserMemo(std::string & strUserID,std::string & strMemo);
	// Detach our session from the clan's list & tell clannies we logged off.
	void OnLogout(CUser *pUser);
	void OnLogoutAlliance(CUser *pUser);

	bool AddUser(std::string & strUserID, uint8 bFame, uint16 sClass, uint8 bLevel, int32 lastlogin, uint32 Loyalty);
	bool AddUser(CUser *pUser);

	bool RemoveUser(std::string & strUserID);
	bool RemoveUser(CUser *pUser);

	void RefundDonatedNP(uint32 nDonatedNP, CUser * pUser = nullptr, const char * strUserID = nullptr);

	void Disband(CUser *pLeader = nullptr);

	void SendUpdate();
	void SendAllianceUpdate();
	void SendChat(const char * format, ...);
	void SendChatAlliance(const char * format, ...);
	void Send(Packet *pkt);

	uint16 GetKnightsAllMembers(Packet & result, uint16 & pktSize, bool bClanLeader);

	virtual ~CKnights();
};
