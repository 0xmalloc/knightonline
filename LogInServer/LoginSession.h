#pragma once

class LoginSession : public KOSocket
{
public:
	LoginSession(uint16 socketID, SocketMgr *mgr); 

	virtual bool HandlePacket(Packet & pkt);
	void HandleVersion(Packet & pkt);
	void HandlePatches(Packet & pkt);
	void HandleLogin(Packet & pkt);
	void HandleServerlist(Packet & pkt);
	void HandleNews(Packet & pkt);
#if __VERSION >= 1453
	void HandleSetEncryptionPublicKey(Packet & pkt);
	void HandleUnkF7(Packet & pkt);
#endif
};

enum LogonOpcodes
{
	LS_VERSION_REQ				= 0x01,
	LS_DOWNLOADINFO_REQ			= 0x02,
	LS_CRYPTION					= 0xF2,
	LS_LOGIN_REQ				= 0xF3,
	LS_MGAME_LOGIN				= 0xF4,
	LS_SERVERLIST				= 0xF5,
	LS_NEWS						= 0xF6,
	LS_UNKF7					= 0xF7,

	NUM_LS_OPCODES
};

void InitPacketHandlers(void);
typedef void (LoginSession::*LSPacketHandler)(Packet &);
