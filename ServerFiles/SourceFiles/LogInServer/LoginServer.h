#pragma once

#include "../shared/KOSocketMgr.h"
#include "../shared/STLMap.h"

typedef std::map <std::string, _VERSION_INFO *> VersionInfoList;
typedef CSTLMap<_SERVER_INFO>	ServerInfoList;

class LoginSession;
class LoginServer
{
	friend class CDBProcess;
public:
	LoginServer();

	INLINE short GetVersion() { return m_sLastVersion; };
	INLINE std::string & GetFTPUrl() { return m_strFtpUrl; };
	INLINE std::string & GetFTPPath() { return m_strFilePath; };

	INLINE News * GetNews() { return &m_news; };

	INLINE VersionInfoList* GetPatchList() { return &m_VersionList; };

	bool Startup();

	static uint32 THREADCALL Timer_UpdateUserCount(void * lpParam);
	static uint32 THREADCALL Timer_UpdateKingNotice(void * lpParam);
	void GetServerList(Packet & result);

	~LoginServer();

	void AddDatabaseRequest(Packet & pkt);

	std::map<std::string, uint32> LoginTrials;
	KOSocketMgr<LoginSession> m_socketMgr[10];
	Condition* s_hEvent;

private:
	void UpdateServerList();
	void GetInfoFromIni();
	void WriteLogFile(std::string & logMessage);
	void ReportSQLError(OdbcError *pError);

	std::string m_strFtpUrl, m_strFilePath;
	std::string m_ODBCName, m_ODBCLogin, m_ODBCPwd;
	short	m_sLastVersion;

	void Yaz(char* yazi, WORD color);
	void color_c(char* buff, WORD color);
	void SendConsoleFAIL();
	void SendConsoleOK();

	uint32 m_LoginServerPort;

	VersionInfoList						m_VersionList;
	ServerInfoList						m_ServerList;
	HardwareInformation					g_HardwareInformation;
	News m_news;

	RWLock m_patchListLock;
	Packet m_serverListPacket;
	std::recursive_mutex m_lock, m_serverListLock;
	std::vector<int64> m_HardwareIDArray;
	std::vector<std::string> m_sHardwareIpArray;

	FILE *m_fpLoginServer;
public:
	CDBProcess	m_DBProcess;
	void WriteUserLogFile(std::string & logMessage);

	FILE *m_fpUser;
};

extern LoginServer * g_pMain;
