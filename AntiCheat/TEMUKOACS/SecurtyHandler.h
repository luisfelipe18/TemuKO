#pragma once
#include "stdafx.h"

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::UserCheckThreadProc()
*/
static void WINAPI wUserCheckThreadProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->UserCheckThreadProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::ProcessIntegrityLevelCheck()
*/
static void WINAPI wProcessIntegrityLevelCheck(LPVOID param)
{
	((TEMUKOACSEngine*)param)->ProcessIntegrityLevelCheck(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HookChecker()
*/
static void WINAPI wHookChecker(LPVOID param)
{
	((TEMUKOACSEngine*)param)->HookChecker(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::ClassScanProc()
*/
static void WINAPI wClassScanProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->ClassScanProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HeuristicClassCheckProc()
*/
static void WINAPI wHeuristicClassCheckProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->HeuristicClassCheckProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HeuristicWindowCheckProc()
*/
static void WINAPI wHeuristicWindowCheckProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->HeuristicWindowCheckProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::InjectScanProc()
*/
static void WINAPI wInjectScanProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->InjectScanProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::TileScanProc()
*/
static void WINAPI wTileScanProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->TileScanProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::QueryCounterCountProc()
*/
static void WINAPI wQueryCounterCountProc(LPVOID param)
{
	((TEMUKOACSEngine*)param)->QueryCounterCountProc(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::check_driver()
*/
static void WINAPI wcheck_driver(LPVOID param)
{
	((TEMUKOACSEngine*)param)->check_driver(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::child_check()
*/
static void WINAPI wchild_check(LPVOID param)
{
	((TEMUKOACSEngine*)param)->child_check(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::style_check()
*/
static void WINAPI wstyle_check(LPVOID param)
{
	((TEMUKOACSEngine*)param)->style_check(); // call the member function
}


/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::Terminate_Scan()
*/
static void WINAPI wTerminate_Scan(LPVOID param)
{
	((TEMUKOACSEngine*)param)->Terminate_Scan(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::Tbl_Scan()
*/
static void WINAPI wTbl_Scan(LPVOID param)
{
	((TEMUKOACSEngine*)param)->Tbl_Scan(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::Dump_Scan()
*/
static void WINAPI wDump_Scan(LPVOID param)
{
	((TEMUKOACSEngine*)param)->Dump_Scan(); // call the member function
}

/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::EnumWinHandle()
*/
BOOL CALLBACK wEnumWinHandle(HWND hWnd, LPARAM lparam)
{
	return ((TEMUKOACSEngine*)lparam)->EnumWinHandle(hWnd, lparam); // call the member function
}


/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HeuristicWindowCheck()
*/
BOOL CALLBACK wHeuristicWindowCheck(HWND hwnd, LPARAM lParam)
{
	return ((TEMUKOACSEngine*)lParam)->HeuristicWindowCheck(hwnd, lParam); // call the member function
}


/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HeuristicWindowCheck()
*/
BOOL CALLBACK wEnumChildProc(HWND hwnd, LPARAM lParam)
{
	return ((TEMUKOACSEngine*)lParam)->EnumChildProc(hwnd, lParam); // call the member function
}


/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::HeuristicWindowCheck()
*/
BOOL CALLBACK wHeuristicClassCheck(HWND hwnd, LPARAM lParam)
{
	return ((TEMUKOACSEngine*)lParam)->HeuristicClassCheck(hwnd, lParam); // call the member function
}


/**
* @brief	This is just a dummy wrapper over CHSUpdateVAGuardDLL::CheckRunTimeTBLs()
*/
static void WINAPI wCheckRunTimeTBLs(LPVOID param)
{
	((TEMUKOACSEngine*)param)->CheckRunTimeTBLs(); // call the member function
}