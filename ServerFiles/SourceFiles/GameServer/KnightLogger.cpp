#include "stdafx.h"
#include "KnightLogger.h"
#include "../shared/Condition.h"


using std::string;

static std::queue<Packet *> _queue;
static bool _running = true;
static std::recursive_mutex _lock;

static Condition s_hEvent;
static Thread * s_thread;

void KnightLogger::Startup()
{
	s_thread = new Thread(ThreadProc, (void *)1);
}

void KnightLogger::AddRequest(Packet * pkt)
{
	_lock.lock();
	_queue.push(pkt);
	_lock.unlock();
	s_hEvent.Signal();
}

uint32 THREADCALL KnightLogger::ThreadProc(void * lpParam)
{
	while (true)
	{
		Packet *p = nullptr;

		// Pull the next packet from the shared queue
		_lock.lock();
		if (_queue.size())
		{
			p = _queue.front();
			_queue.pop();
		}
		_lock.unlock();

		// If there's no more packets to handle, wait until there are.
		if (p == nullptr)
		{
			// If we're shutting down, don't bother waiting for more (there are no more).
			if (!_running)
				break;

			s_hEvent.Wait();
			continue;
		}

		// References are fun =p
		Packet & pkt = *p;
		DateTime time;
		// First byte is always going to be the log type
		// or -1 for no user.
		uint8 subOpCode = pkt.read<uint8>();
		pkt.DByte();
		std::string logmsg;
		pkt >> logmsg;
		switch (subOpCode)
		{
		case DEADLOGUSER:
			g_pMain->m_fpDeathUser = fopen(string_format("./Logs/DeathUserLogs/DeathUser_%d_%d_%d_%d.log", time.GetHour(), time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpDeathUser == nullptr)
			{
				printf("ERROR: Unable to open death user log file.::DEADLOGUSER \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpDeathUser);
			fflush(g_pMain->m_fpDeathUser);
			fclose(g_pMain->m_fpDeathUser);
			break;
		case DEADLOGNPC:
			g_pMain->m_fpDeathNpc = fopen(string_format("./Logs/DeathNpcLogs/DeathNpc_%d_%d_%d_%d.log", time.GetHour(), time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpDeathNpc == nullptr)
			{
				printf("ERROR: Unable to open death npc log file.::DEADLOGNPC \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpDeathNpc);
			fflush(g_pMain->m_fpDeathNpc);
			fclose(g_pMain->m_fpDeathNpc);
			break;
		case MERTCHANTLOG:
			g_pMain->m_fpMerchant = fopen(string_format("./Logs/MerchantLogs/Merchant_%d_%d_%d_%d.log", time.GetHour(), time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpMerchant == nullptr)
			{
				printf("ERROR: Unable to open Merchant log file.::MERTCHANTLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpMerchant);
			fflush(g_pMain->m_fpMerchant);
			fclose(g_pMain->m_fpMerchant);
			break;
		case TRADELOG:
			g_pMain->m_fpTrade = fopen(string_format("./Logs/TradeLogs/Trades_%d_%d_%d_%d.log", time.GetHour(), time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpTrade == nullptr)
			{
				printf("ERROR: Unable to open trade log file.::TRADELOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpTrade);
			fflush(g_pMain->m_fpTrade);
			fclose(g_pMain->m_fpTrade);
			break;
		case LETTERLOG:
			g_pMain->m_fpLetter = fopen(string_format("./Logs/LetterLogs/LetterLogs_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpLetter == nullptr)
			{
				printf("ERROR: Unable to open letter log file.::LETTERLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpLetter);
			fflush(g_pMain->m_fpLetter);
			fclose(g_pMain->m_fpLetter);
			break;
		case CHATLOG:
			g_pMain->m_fpChat = fopen(string_format("./Logs/ChatLogs/Chat_%d_%d_%d_%d.log", time.GetHour(), time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpChat == nullptr)
			{
				printf("ERROR: Unable to open chat log file.::CHATLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpChat);
			fflush(g_pMain->m_fpChat);
			fclose(g_pMain->m_fpChat);
			break;
		case CHEATLOG:
			g_pMain->m_fpCheat = fopen(string_format("./Logs/CheatLogs/Cheat_%d_%d.log", time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpCheat == nullptr)
			{
				printf("ERROR: Unable to open cheat log file.::CHEATLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpCheat);
			fflush(g_pMain->m_fpCheat);
			fclose(g_pMain->m_fpCheat);
			break;
		case BANLOG:
			g_pMain->m_fpBanList = fopen(string_format("./Logs/BanLogs/BanList_%d_%d.log", time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpBanList == nullptr)
			{
				printf("ERROR: Unable to open ban log file.::BANLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpBanList);
			fflush(g_pMain->m_fpBanList);
			fclose(g_pMain->m_fpBanList);
			break;
		case ITEMLOG:
			g_pMain->m_fpItemTransaction = fopen(string_format("./Logs/ItemLogs/ItemLog_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpItemTransaction == nullptr)
			{
				printf("ERROR: Unable to open ban log file.::ITEMLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpItemTransaction);
			fflush(g_pMain->m_fpItemTransaction);
			fclose(g_pMain->m_fpItemTransaction);
			break;
		case ITEMUPGRADELOG:
			g_pMain->m_fpItemUpgradeTransaction = fopen(string_format("./Logs/ItemUpgradeLogs/ItemUpgradeLog_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpItemUpgradeTransaction == nullptr)
			{
				printf("ERROR: Unable to open ban log file.::ITEMUPGRADELOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpItemUpgradeTransaction);
			fflush(g_pMain->m_fpItemUpgradeTransaction);
			fclose(g_pMain->m_fpItemUpgradeTransaction);
			break;
		case CHARACTERIPLOG:
			g_pMain->m_fpCharacterIpTransaction = fopen(string_format("./Logs/CharacterIpLogs/CharacterIpLog_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpCharacterIpTransaction == nullptr)
			{
				printf("ERROR: Unable to open ban log file.::CHARACTERIPLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpCharacterIpTransaction);
			fflush(g_pMain->m_fpCharacterIpTransaction);
			fclose(g_pMain->m_fpCharacterIpTransaction);
			break;
		case TOURNAMENTLOG:
			g_pMain->m_fpTournament = fopen(string_format("./Logs/TournamentLogs/TournamentLogs_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpTournament == nullptr)
			{
				printf("ERROR: Unable to open ban log file.::TOURNAMENTLOG \n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpTournament);
			fflush(g_pMain->m_fpTournament);
			fclose(g_pMain->m_fpTournament);
			break;
		case OFFLINE_MERCHANT:
			g_pMain->m_fpOfflineSystemLog = fopen(string_format("./Logs/OfflineSystemLogs/OfflineSystem_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpOfflineSystemLog == nullptr)
			{
				printf("ERROR: Unable to open OfflineSystem file.\n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpOfflineSystemLog);
			fflush(g_pMain->m_fpOfflineSystemLog);
			fclose(g_pMain->m_fpOfflineSystemLog);
			break;
		case HOOK_PUS:
			g_pMain->m_fpHookPusSystemLog = fopen(string_format("./Logs/HookPusSystemLogs/PusSystem_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
			if (g_pMain->m_fpHookPusSystemLog == nullptr)
			{
				printf("ERROR: Unable to open Hook Pus Log file.\n");
				break;
			}
			fwrite(logmsg.c_str(), logmsg.length(), 1, g_pMain->m_fpHookPusSystemLog);
			fflush(g_pMain->m_fpHookPusSystemLog);
			fclose(g_pMain->m_fpHookPusSystemLog);
			break;

		default:
			break;
		}

		// Free the packet.
		delete p;
	}

	TRACE("[KOLogger System][Thread %d] Exiting...\n", lpParam);
	return 0;
}


void KnightLogger::Shutdown()
{
	_running = false;

	// Wake them up in case they're sleeping.
	s_hEvent.Broadcast();

	if(s_thread)
	{
		s_thread->waitForExit();
		delete s_thread;
	}

	_lock.lock();
	while (_queue.size())
	{
		auto *p = &_queue.front();
		_queue.pop();
		delete p;
	}
	_lock.unlock();
}
