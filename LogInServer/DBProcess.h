#pragma once

#include "../shared/database/OdbcConnection.h"

class CDBProcess  
{
public:
	bool Connect(std::string & szDSN, std::string & szUser, std::string & szPass);

	bool LoadVersionList();
	bool LoadUserCountList();

	uint16 AccountLogin(std::string & id, std::string & pwd);
	int16 AccountPremium(std::string & id);

private:
	OdbcConnection m_dbConnection;
};