#include "stdafx.h"

void CUser::VersionCheck(Packet & pkt)
{
	Packet result(WIZ_VERSION_CHECK);
	/*
	string strAccountID;
	int16 unk = pkt.read<int16>(); // -1
	pkt >> strAccountID;
	if (strAccountID.empty() || strAccountID.size() > MAX_ID_SIZE)
	return;
	*/

	result	<< uint8(0) << uint16(__VERSION) << m_crypto.GenerateKey()
		<< uint8(0); // 0 = success, 1 = prem error
	Send(&result);

	// Enable encryption
	EnableCrypto();
}

void CUser::LoginProcess(Packet & pkt)
{
	// Enforce only one login request per session
	// It's common for players to spam this at the server list when a response isn't received immediately.
	if (!m_strAccountID.empty())
		return;

	Packet result(WIZ_LOGIN);
	std::string strAccountID, strPasswd;
	pkt >> strAccountID >> strPasswd;
	if (strAccountID.empty() || strAccountID.size() > MAX_ID_SIZE
		|| strPasswd.empty() || strPasswd.size() > MAX_PW_SIZE)
		goto fail_return;

	CUser * pUser = g_pMain->GetUserPtr(strAccountID, TYPE_ACCOUNT);

	if (pUser && (pUser->GetSocketID() != GetSocketID()))
	{
		pUser->Disconnect();
		goto fail_return;
	}

	result << strPasswd;
	m_strAccountID = strAccountID;
	g_pMain->AddDatabaseRequest(result, this);
	return;

fail_return:
	result << uint8(-1);
	Send(&result);
}