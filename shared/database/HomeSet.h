#pragma once

class CHomeSet : public OdbcRecordset
{
public:
	CHomeSet(OdbcConnection * dbConnection, HomeArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("HOME"); }
	virtual tstring GetColumns() { return _T("Nation, ElmoZoneX, ElmoZoneZ, ElmoZoneLX, ElmoZoneLZ, KarusZoneX, KarusZoneZ, KarusZoneLX, KarusZoneLZ, FreeZoneX, FreeZoneZ, FreeZoneLX, FreeZoneLZ, BattleZoneX, BattleZoneZ, BattleZoneLX, BattleZoneLZ, BattleZone2X, BattleZone2Z, BattleZone2LX, BattleZone2LZ, BattleZone3X, BattleZone3Z, BattleZone3LX, BattleZone3LZ, BattleZone4X, BattleZone4Z, BattleZone4LX, BattleZone4LZ, BattleZone5X, BattleZone5Z, BattleZone5LX, BattleZone5LZ, BattleZone6X, BattleZone6Z, BattleZone6LX, BattleZone6LZ"); }

	virtual bool Fetch()
	{
		_HOME_INFO *pData = new _HOME_INFO;

		_dbCommand->FetchByte(1, pData->bNation);
		_dbCommand->FetchUInt32(2, pData->ElmoZoneX);
		_dbCommand->FetchUInt32(3, pData->ElmoZoneZ);
		_dbCommand->FetchByte(4, pData->ElmoZoneLX);
		_dbCommand->FetchByte(5, pData->ElmoZoneLZ);
		_dbCommand->FetchUInt32(6, pData->KarusZoneX);
		_dbCommand->FetchUInt32(7, pData->KarusZoneZ);
		_dbCommand->FetchByte(8, pData->KarusZoneLX);
		_dbCommand->FetchByte(9, pData->KarusZoneLZ);
		_dbCommand->FetchUInt32(10, pData->FreeZoneX);
		_dbCommand->FetchUInt32(11, pData->FreeZoneZ);
		_dbCommand->FetchByte(12, pData->FreeZoneLX);
		_dbCommand->FetchByte(13, pData->FreeZoneLZ);
		_dbCommand->FetchUInt32(14, pData->BattleZoneX);
		_dbCommand->FetchUInt32(15, pData->BattleZoneZ);
		_dbCommand->FetchByte(16, pData->BattleZoneLX);
		_dbCommand->FetchByte(17, pData->BattleZoneLZ);
		_dbCommand->FetchUInt32(18, pData->BattleZone2X);
		_dbCommand->FetchUInt32(19, pData->BattleZone2Z);
		_dbCommand->FetchByte(20, pData->BattleZone2LX);
		_dbCommand->FetchByte(21, pData->BattleZone2LZ);
		_dbCommand->FetchUInt32(22, pData->BattleZone3X);
		_dbCommand->FetchUInt32(23, pData->BattleZone3Z);
		_dbCommand->FetchByte(24, pData->BattleZone3LX);
		_dbCommand->FetchByte(25, pData->BattleZone3LZ);
		_dbCommand->FetchUInt32(26, pData->BattleZone4X);
		_dbCommand->FetchUInt32(27, pData->BattleZone4Z);
		_dbCommand->FetchByte(28, pData->BattleZone4LX);
		_dbCommand->FetchByte(29, pData->BattleZone4LZ);
		_dbCommand->FetchUInt32(30, pData->BattleZone5X);
		_dbCommand->FetchUInt32(31, pData->BattleZone5Z);
		_dbCommand->FetchByte(32, pData->BattleZone5LX);
		_dbCommand->FetchByte(33, pData->BattleZone5LZ);
		_dbCommand->FetchUInt32(34, pData->BattleZone6X);
		_dbCommand->FetchUInt32(35, pData->BattleZone6Z);
		_dbCommand->FetchByte(36, pData->BattleZone6LX);
		_dbCommand->FetchByte(37, pData->BattleZone6LZ);

		if (!m_pMap->PutData(pData->bNation, pData))
			delete pData;

		return true;
	}

	HomeArray *m_pMap;
};