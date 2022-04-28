#include "stdafx.h"
#include <sstream>
#include "../shared/Ini.h"
#include "../shared/DateTime.h"

extern bool g_bRunning;
std::vector<Thread *> g_timerThreads;

LoginServer::LoginServer() : m_sLastVersion(__VERSION), m_fpLoginServer(nullptr)
{
}

bool LoginServer::Startup()
{
	DateTime time;

	//color_c("", FOREGROUND_BLUE + FOREGROUND_GREEN);// mavi
	color_c("", FOREGROUND_INTENSITY + 6); // sari

	// Server Start
	printf("Server started on %04d-%02d-%02d at %02d:%02d\n\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
	
	// License System..
	m_HardwareIDArray.push_back(230662953965053);
	m_HardwareIDArray.push_back(4563295485398);
	m_HardwareIDArray.push_back(287021517914848);//Yenis!04.
	m_HardwareIDArray.push_back(620371517914848);
	m_HardwareIDArray.push_back(3748943933151791338);
	m_HardwareIDArray.push_back(9223372036854775807);
	m_HardwareIDArray.push_back(260732953912417);
	m_HardwareIDArray.push_back(260732953912417);
	m_HardwareIDArray.push_back(99442953912417);
	m_HardwareIDArray.push_back(254922953945256);
	m_HardwareIDArray.push_back(311982953912417); // HardWareID LICENCIA

	/*
	 *Del mismo modo, escribimos la dirección IP existente y los números de
	 * serie del disco duro para 2 licencias en Bursa en dos líneas y obtenemos una compilación.
	*/
	m_sHardwareIpArray.push_back("127.0.0.1"); // IP LICENCIA
	if (!g_HardwareInformation.IsValidHardwareID(m_HardwareIDArray))
	{
		printf("Lisans Number: %lld\n", g_HardwareInformation.GetHardwareID());
		return false;
	}

	// License System..

	color_c("", FOREGROUND_INTENSITY + FOREGROUND_GREEN); // ye�il
	//color_c("", FOREGROUND_INTENSITY + FOREGROUND_GREEN + FOREGROUND_BLUE); // a��k mavi
	printf("### -> License Successfuly\n\n");
	/*License System End*/
	color_c("", FOREGROUND_BLUE + FOREGROUND_BLUE + FOREGROUND_GREEN);
	printf("Connected to database server\n");
	printf("Version in database: %d\n\n", GetVersion());
	color_c("", FOREGROUND_INTENSITY + 6);

	GetInfoFromIni();

	CreateDirectory("Logs",NULL);

	m_fpLoginServer = fopen("./Logs/LoginServer.log", "a");
	if (m_fpLoginServer == nullptr)
	{
		printf("ERROR: Unable to open log file.\n");
		return false;
	}

	m_fpUser = fopen(string_format("./Logs/Login_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpUser == nullptr)
	{
		printf("ERROR: Unable to open user log file.\n");
		return false;
	}

	if (!m_DBProcess.Connect(m_ODBCName, m_ODBCLogin, m_ODBCPwd)) 
	{
		printf("ERROR: Unable to connect to the database using the details configured.\n");
		return false;
	}

	printf("Database connection is established successfully.\n");

	if (!m_DBProcess.LoadVersionList())
	{
		printf("ERROR: Unable to load the version list.\n");
		return false;
	}

	printf("Latest version in database: %d\n", GetVersion());
	if (!m_DBProcess.LoadServerList())
	{
		printf("ERROR: Unable to load the server list.\n");
		return false;
	}
	InitPacketHandlers();

	for(int i = 0; i < 10 ; i++)
	{
		if (!m_socketMgr[i].Listen((uint16) m_LoginServerPort + i, MAX_USER / 5))
		{
			printf("ERROR: Failed to listen on server port.\n");
			return false;
		}

		m_socketMgr[i].RunServer();
	}


	g_timerThreads.push_back(new Thread(Timer_UpdateUserCount));
	g_timerThreads.push_back(new Thread(Timer_UpdateKingNotice));
	return true;
}

uint32 LoginServer::Timer_UpdateUserCount(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->UpdateServerList();
		sleep(60 * SECOND);
	}
	return 0;
}

uint32 LoginServer::Timer_UpdateKingNotice(void * lpParam)
{
	while (g_bRunning)
	{
		sleep(600 * SECOND);
		g_pMain->m_DBProcess.LoadServerList();	
	}
	return 0;
}

void LoginServer::GetServerList(Packet & result)
{
	Guard lock(m_serverListLock);
	result.append(m_serverListPacket.contents(), m_serverListPacket.size());
}

void LoginServer::UpdateServerList()
{
	// Update the user counts first
	m_DBProcess.LoadUserCountList();

	Guard lock(m_serverListLock);
	Packet & result = m_serverListPacket;

	result.clear();
	result << uint8(m_ServerList.GetSize());
	foreach_stlmap (itr, m_ServerList) 
	{		
		_SERVER_INFO *pServer = itr->second;
		if(pServer == nullptr)
			continue;

		result << pServer->strLanIP;
		result << pServer->strServerIP;
		result << pServer->strServerName;

		if (pServer->sUserCount <= pServer->sPlayerCap)
			result << pServer->sUserCount;
		else
			result << int16(-1);

		result << pServer->sServerID << pServer->sGroupID;
		result << pServer->sPlayerCap << pServer->sFreePlayerCap;
		result << uint8(0); 

		// we read all this stuff from ini, TODO: make this more versatile.
		result	<< pServer->strKarusKingName << pServer->strKarusNotice 
			<< pServer->strElMoradKingName << pServer->strElMoradNotice;
	}
}

void LoginServer::GetInfoFromIni()
{
	CIni ini(CONF_LOGIN_SERVER);

	ini.GetString("DOWNLOAD", "URL", "ftp.yoursite.net", m_strFtpUrl, false);
	ini.GetString("DOWNLOAD", "PATH", "/", m_strFilePath, false);

	ini.GetString("ODBC", "DSN", "KO_MAIN", m_ODBCName, false);
	ini.GetString("ODBC", "UID", "username", m_ODBCLogin, false);
	ini.GetString("ODBC", "PWD", "password", m_ODBCPwd, false);

	m_LoginServerPort = ini.GetInt("SETTINGS","PORT", 15100);

	int nServerCount = ini.GetInt("GAME_SERVER_LIST", "COUNT", 1);
	if (nServerCount <= 0) 
		nServerCount = 1;

	char key[20]; 

	// Read news from INI (max 3 blocks)
#define BOX_START '#' << uint8(0) << '\n'
#define BOX_END '#'

	m_news.Size = 0;
	std::stringstream ss;
	for (int i = 0; i < 3; i++)
	{
		string title, message;

		_snprintf(key, sizeof(key), "TITLE_%02d", i);
		ini.GetString("NEWS", key, "", title);
		if (title.empty())
			continue;

		_snprintf(key, sizeof(key), "MESSAGE_%02d", i);
		ini.GetString("NEWS", key, "", message);
		if (message.empty())
			continue;

		size_t oldPos = 0, pos = 0;
		ss << title << BOX_START << message << BOX_END;
	}

	m_news.Size = ss.str().size();
	if (m_news.Size)
		memcpy(&m_news.Content, ss.str().c_str(), m_news.Size);
}

void LoginServer::WriteLogFile(string & logMessage)
{
	Guard lock(m_lock);
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpLoginServer);
	fflush(m_fpLoginServer);
}

void LoginServer::WriteUserLogFile(string & logMessage)
{
	Guard lock(m_lock);
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpUser);
	fflush(m_fpUser);
}

void LoginServer::ReportSQLError(OdbcError *pError)
{
	if (pError == nullptr)
		return;

	// This is *very* temporary.
	string errorMessage = string_format(_T("ODBC error occurred.\r\nSource: %s\r\nError: %s\r\nDescription: %s\n"),
		pError->Source.c_str(), pError->ExtendedErrorMessage.c_str(), pError->ErrorMessage.c_str());

	TRACE("%s", errorMessage.c_str());
	WriteLogFile(errorMessage);
	delete pError;
}

LoginServer::~LoginServer() 
{
	printf("Waiting for timer threads to exit...");
	foreach (itr, g_timerThreads)
	{
		(*itr)->waitForExit();
		delete (*itr);
	}
	printf(" exited.\n");
	Sleep(1 * SECOND);

	m_ServerList.DeleteAllData();

	foreach (itr, m_VersionList)
		delete itr->second;
	m_VersionList.clear();

	if (m_fpLoginServer != nullptr)
		fclose(m_fpLoginServer);

	if (m_fpUser != nullptr)
		fclose(m_fpUser);

	printf("Shutting down socket system...");

	for(int i = 0; i < 10 ; i++)
		m_socketMgr[i].Shutdown();

	printf(" done.\n");
	Sleep(1 * SECOND);
}