#include "stdafx.h"
<<<<<<< HEAD
=======
#include "../shared/DateTime.h"
>>>>>>> koserver2

LSPacketHandler PacketHandlers[NUM_LS_OPCODES];
void InitPacketHandlers(void)
{
	memset(&PacketHandlers, 0, sizeof(LSPacketHandler) * NUM_LS_OPCODES);
	PacketHandlers[LS_VERSION_REQ]			= &LoginSession::HandleVersion;
	PacketHandlers[LS_DOWNLOADINFO_REQ]		= &LoginSession::HandlePatches;
	PacketHandlers[LS_LOGIN_REQ]			= &LoginSession::HandleLogin;
	PacketHandlers[LS_SERVERLIST]			= &LoginSession::HandleServerlist;
	PacketHandlers[LS_NEWS]					= &LoginSession::HandleNews;
<<<<<<< HEAD
#if __VERSION >= 1453
	PacketHandlers[LS_CRYPTION]				= &LoginSession::HandleSetEncryptionPublicKey;
	PacketHandlers[LS_UNKF7]				= &LoginSession::HandleUnkF7;
#endif
=======
	PacketHandlers[LS_CRYPTION]				= &LoginSession::HandleSetEncryptionPublicKey;
	PacketHandlers[LS_UNKF7]				= &LoginSession::HandleUnkF7;
>>>>>>> koserver2
}

LoginSession::LoginSession(uint16 socketID, SocketMgr *mgr) : KOSocket(socketID, mgr, -1, 2048, 64) {}

bool LoginSession::HandlePacket(Packet & pkt)
{
	uint8 opcode = pkt.GetOpcode();

	// Unknown packet
	if (opcode >= NUM_LS_OPCODES
		|| PacketHandlers[opcode] == nullptr)
		return false;

	(this->*PacketHandlers[opcode])(pkt);
	return true;
}

void LoginSession::HandleVersion(Packet & pkt)
{
	Packet result(pkt.GetOpcode());
	result << g_pMain->GetVersion();
	Send(&result);
}

void LoginSession::HandlePatches(Packet & pkt)
{
	Packet result(pkt.GetOpcode());
	std::set<std::string> downloadset;
	uint16 version;
	pkt >> version;

	foreach (itr, (*g_pMain->GetPatchList())) 
	{
		auto pInfo = itr->second;
		if (pInfo->sVersion > version)
<<<<<<< HEAD
			downloadset.insert(pInfo->strFileName);
=======
			downloadset.insert(pInfo->strFilename);
>>>>>>> koserver2
	}

	result << g_pMain->GetFTPUrl() << g_pMain->GetFTPPath();
	result << uint16(downloadset.size());

	foreach (itr, downloadset)
		result << (*itr);

	Send(&result);
}

void LoginSession::HandleLogin(Packet & pkt)
{
	enum LoginErrorCode
	{
		AUTH_SUCCESS	= 0x01,
		AUTH_NOT_FOUND	= 0x02,
		AUTH_INVALID	= 0x03,
		AUTH_BANNED		= 0x04,
		AUTH_IN_GAME	= 0x05,
		AUTH_ERROR		= 0x06,
<<<<<<< HEAD
=======
		AUTH_AGREEMENT	= 0xF,
>>>>>>> koserver2
		AUTH_FAILED		= 0xFF
	};

	Packet result(pkt.GetOpcode());
	uint16 resultCode = 0;
	string account, password;
<<<<<<< HEAD
=======
	DateTime time;
>>>>>>> koserver2

	pkt >> account >> password;
	if (account.size() == 0 || account.size() > MAX_ID_SIZE 
		|| password.size() == 0 || password.size() > MAX_PW_SIZE)
		resultCode = AUTH_NOT_FOUND; 
	else
		resultCode = g_pMain->m_DBProcess.AccountLogin(account, password);

	std::string sAuthMessage;

	switch (resultCode)
	{
	case AUTH_SUCCESS:
		sAuthMessage = "SUCCESS";
		break;
	case AUTH_NOT_FOUND:
		sAuthMessage = "NOT FOUND";
		break;
	case AUTH_INVALID:
		sAuthMessage = "INVALID";
		break;
	case AUTH_BANNED:
		sAuthMessage = "BANNED";
		break;
	case AUTH_IN_GAME:
		sAuthMessage = "IN GAME";
		break;
	case AUTH_ERROR:
		sAuthMessage = "ERROR";
		break;
<<<<<<< HEAD
=======
	case AUTH_AGREEMENT:
		sAuthMessage = "USER AGREEMENT";
		break;
>>>>>>> koserver2
	case AUTH_FAILED:
		sAuthMessage = "FAILED";
		break;
	default:
		sAuthMessage = string_format("UNKNOWN (%d)",resultCode);
		break;
	}

<<<<<<< HEAD
	printf(string_format("Login : %s / PW : %s / Authentication : %s\n",account.c_str(),password.c_str(),sAuthMessage.c_str()).c_str());
=======
	printf("[ LOGIN - %d:%d:%d ] ID=%s Authentication=%s\n", 
		time.GetHour(), time.GetMinute(), time.GetSecond(),
		account.c_str(), sAuthMessage.c_str());
>>>>>>> koserver2

	result << uint8(resultCode);
	if (resultCode == AUTH_SUCCESS)
	{
		result << g_pMain->m_DBProcess.AccountPremium(account);
		result << account;
	}
	else if (resultCode == AUTH_IN_GAME)
	{
<<<<<<< HEAD
		/*
		SessionMap & sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach (itr, sessMap)
		{

		}
		*/
	}
=======
	}
	else if (resultCode == AUTH_AGREEMENT)
	{
	}

	g_pMain->WriteUserLogFile(string_format("[ LOGIN - %d:%d:%d ] ID=%s Authentication=%s\n",time.GetHour(),time.GetMinute(),time.GetSecond(),account.c_str(),password.c_str(),sAuthMessage.c_str()));

>>>>>>> koserver2
	Send(&result);	
}

void LoginSession::HandleServerlist(Packet & pkt)
{
	Packet result(pkt.GetOpcode());

<<<<<<< HEAD
#if __VERSION >= 1500
	uint16 echo;
	pkt >> echo;
	result << echo;
#endif
=======
	uint16 echo;
	pkt >> echo;
	result << echo;
>>>>>>> koserver2

	g_pMain->GetServerList(result);
	Send(&result);
}

void LoginSession::HandleNews(Packet & pkt)
{
	Packet result(pkt.GetOpcode());
	News *pNews = g_pMain->GetNews();

	if (pNews->Size)
	{
		result << "Login Notice" << uint16(pNews->Size);
		result.append(pNews->Content, pNews->Size);
	}
	else // dummy news, will skip past it
	{
		result << "Login Notice" << "<empty>";
	}
	Send(&result);
}

<<<<<<< HEAD
#if __VERSION >= 1453
=======
>>>>>>> koserver2
void LoginSession::HandleSetEncryptionPublicKey(Packet & pkt)
{
	Packet result(pkt.GetOpcode());
	result << m_crypto.GenerateKey();
	Send(&result);
	EnableCrypto();
}

void LoginSession::HandleUnkF7(Packet & pkt)
{
	Packet result(pkt.GetOpcode());
	result << uint16(0);
	Send(&result);
}
<<<<<<< HEAD
#endif
=======
>>>>>>> koserver2
