#include "stdafx.h"
#include "JvCryption.h"
#include "version.h"

// Cryption
#if __VERSION >= 1700
#define g_private_key 0x1207500120128966
#elif __VERSION >= 1298 && __VERSION < 1453
#define g_private_key 0x1234567890123456
#else
#define g_private_key 0x1257091582190465
#endif

void CJvCryption::Init() { m_tkey = m_public_key ^ g_private_key; }

uint64 CJvCryption::GenerateKey()
{
#ifdef USE_CRYPTION
	// because of their sucky encryption method, 0 means it effectively won't be encrypted. 
	// We don't want that happening...
	do
	{
		m_public_key = RandUInt64();
	} while (!m_public_key); 
#endif
	return m_public_key;
}

void CJvCryption::JvEncryptionFast(int len, uint8 *datain, uint8 *dataout)
{
	uint8 *pkey, lkey, rsk;
	int rkey = 2157;

	pkey = (uint8 *)&m_tkey;
	lkey = (len * 157) & 0xff;

	for (int i = 0; i < len; i++)
	{
		rsk = (rkey >> 8) & 0xff;
		dataout[i] = ((datain[i] ^ rsk) ^ pkey[(i % 8)]) ^ lkey;
		rkey *= 2171;
	}
}

int CJvCryption::JvDecryptionWithCRC32(int len, uint8 *datain, uint8 *dataout)
{
	int result;
	JvDecryptionFast(len, datain, dataout);

	if (crc32(dataout, len - 4, -1) == *(uint32 *)(len - 4 + dataout))
		result = len - 4;
	else
		result = -1;

	return result;
}
