#include "stdafx.h"

bool CDBProcess::Connect(string & szDSN, string & szUser, string & szPass)
{
	if (!m_dbConnection.Connect(szDSN, szUser, szPass))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	return true;
}

bool CDBProcess::LoadVersionList()
{
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("SELECT sVersion, sHistoryVersion, strFileName FROM VERSION")))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	if (dbCommand->hasData())
	{
		g_pMain->m_sLastVersion = 0;
		do
		{
			_VERSION_INFO *pVersion = new _VERSION_INFO;

			dbCommand->FetchUInt16(1, pVersion->sVersion);
			dbCommand->FetchUInt16(2, pVersion->sHistoryVersion);
			dbCommand->FetchString(3, pVersion->strFileName);

			g_pMain->m_VersionList.insert(make_pair(pVersion->strFileName, pVersion));

			if (g_pMain->m_sLastVersion < pVersion->sVersion)
				g_pMain->m_sLastVersion = pVersion->sVersion;

		} while (dbCommand->MoveNext());
	}

	return true;
}

bool CDBProcess::LoadUserCountList()
{
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(_T("SELECT serverid, zone1_count, zone2_count, zone3_count FROM CONCURRENT")))
	{
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return false;
	}

	if (dbCommand->hasData())
	{
		do
		{
			uint16 zone_1 = 0, zone_2 = 0, zone_3 = 0; uint8 serverID;
			dbCommand->FetchByte(1, serverID);
			dbCommand->FetchUInt16(2, zone_1);
			dbCommand->FetchUInt16(3, zone_2);
			dbCommand->FetchUInt16(4, zone_3);

			if ((uint8)(serverID - 1) < g_pMain->m_ServerList.size())
				g_pMain->m_ServerList[serverID - 1]->sUserCount = zone_1 + zone_2 + zone_3;
		} while (dbCommand->MoveNext());
	}

	return true;
}

uint16 CDBProcess::AccountLogin(string & id, string & pwd)
{
	uint16 result = 2; // account not found
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return 6;

	dbCommand->AddParameter(SQL_PARAM_INPUT, id.c_str(), id.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pwd.c_str(), pwd.length());
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);

	if (!dbCommand->Execute(_T("{CALL MAIN_LOGIN(?, ?, ?)}"))){
		g_pMain->ReportSQLError(m_dbConnection.GetError());
		return 0x06;// DB³ö´íÊ±Ä¬ÈÏ·¶Î§auth_error
	}

	return result;
}

int16 CDBProcess::AccountPremium(string & id)
{
	int16 result = -1;
	unique_ptr<OdbcCommand> dbCommand(m_dbConnection.CreateCommand());
	if (dbCommand.get() == nullptr)
		return -1;
	
	dbCommand->AddParameter(SQL_PARAM_INPUT, id.c_str(), id.length());
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);

	if (!dbCommand->Execute(_T("{CALL ACCOUNT_PREMIUM(?, ?)}")))
		g_pMain->ReportSQLError(m_dbConnection.GetError());

	return result;
}