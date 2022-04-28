#pragma once

#include "../shared/database/OdbcConnection.h"

class CDBProcess  
{
public:
	bool Connect(std::string & szDSN, std::string & szUser, std::string & szPass);

	bool LoadVersionList();
	bool LoadServerList();
	bool LoadUserCountList();

	uint16 AccountLogin(std::string & strAccountID, std::string & strPasswd, std::string & OTP_Key);
	uint16 AccountLoginOTP(std::string & strAccountID, std::string & strPasswd);
	int16 AccountPremium(std::string & strAccountID);

private:
	OdbcConnection m_dbConnection;
};