#include "stdafx.h"

CN3ShapeMgr::CN3ShapeMgr()
{
	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;
	m_pvCollisions = nullptr;
	memset(m_pCells, 0, sizeof(m_pCells));
}

CN3ShapeMgr::~CN3ShapeMgr()
{
	if (m_pvCollisions != nullptr)
	{
		delete [] m_pvCollisions; 
		m_pvCollisions = nullptr;
	}

	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
		}
	}
}

void CN3ShapeMgr::Release()
{
	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;

	if (m_pvCollisions != nullptr)
	{
		delete [] m_pvCollisions; 
		m_pvCollisions = nullptr;
	}

	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
			m_pCells[x][z] = nullptr;
		}
	}
	memset(m_pCells, 0, sizeof(MAX_CELL_MAIN));
}

bool CN3ShapeMgr::LoadCollisionData(FILE *fp)
{
	if (fread(&m_fMapWidth, 4, 1, fp) != 1
		|| fread(&m_fMapLength, 4, 1, fp) != 1)
	{
		ASSERT(0);
		return false;
	}

	Create(m_fMapWidth, m_fMapLength);

	if (fread(&m_nCollisionFaceCount, 4, 1, fp) != 1)
	{
		ASSERT(0);
		return false;
	}

	if (m_pvCollisions != nullptr)
	{
		delete [] m_pvCollisions; 
		m_pvCollisions = nullptr;
	}

	if (m_nCollisionFaceCount > 0)
	{
		m_pvCollisions = new __Vector3[m_nCollisionFaceCount * 3];
		if (fread(m_pvCollisions, sizeof(__Vector3) * m_nCollisionFaceCount * 3, 1, fp) != 1)
		{
			ASSERT(0);
			return false;
		}
	}

	// Cell data
	int z = 0;
	for(float fZ = 0.0f; fZ < m_fMapLength; fZ += CELL_MAIN_SIZE, z++)
	{
		int x = 0;
		for(float fX = 0.0f; fX < m_fMapWidth;  fX += CELL_MAIN_SIZE, x++)
		{
			if (m_pCells[x][z] != nullptr)
			{
				delete m_pCells[x][z]; 
				m_pCells[x][z] = nullptr;
			}

			uint32 bExist;
			if (fread(&bExist, 4, 1, fp) != 1)
			{
				ASSERT(0);
				return false;
			}

			if (!bExist) 
				continue;

			m_pCells[x][z] = new __CellMain;
			m_pCells[x][z]->Load(fp);
		}
	}

	return true;
}

bool CN3ShapeMgr::Create(float fMapWidth, float fMapLength) // ���� �ʺ�� ���̸� ���� ������ �ִ´�..
{
	if(	fMapWidth <= 0.0f || fMapWidth > MAX_CELL_MAIN * CELL_MAIN_SIZE ||
		fMapLength <= 0.0f || fMapLength > MAX_CELL_MAIN * CELL_MAIN_SIZE )
	{
		return false;
	}

	m_fMapWidth = fMapWidth;
	m_fMapLength = fMapLength;

	return true;
}

bool CN3ShapeMgr::CheckCollision(	const __Vector3& vPos,		// �浹 ��ġ
								 const __Vector3& vDir,		// ���� ����
								 float fSpeedPerSec,			// �ʴ� �����̴� �ӵ�
								 __Vector3* pvCol,			// �浹 ����
								 __Vector3* pvNormal,		// �浹�Ѹ��� ��������
								 __Vector3* pVec)			// �浹�� �� �� ������ __Vector3[3]
{
	if(fSpeedPerSec <= 0) return false; // �����̴� �ӵ��� ���ų� �ݴ�� �����̸� �Ѿ��..
	static __CellSub* ppCells[128];
	__Vector3 vPosNext = vPos + (vDir* fSpeedPerSec); // ���� ��ġ
	int nSubCellCount = this->SubCellPathThru(vPos, vPosNext, ppCells); // ����ϴ� ���꼿�� �����´�..
	if(nSubCellCount <= 0 || nSubCellCount > 128) return false; // ���� ����.

	__Vector3 vColTmp(0,0,0);
	int nIndex0, nIndex1, nIndex2;
	static float fT, fU, fV, fDistTmp, fDistClosest;
	fDistClosest = FLT_MAX;

	for ( int i = 0; i < nSubCellCount; i++ )
	{
		if ( ppCells[i]->nCCPolyCount <= 0 ) continue;

		for ( int j = 0; j < ppCells[i]->nCCPolyCount; j++ )
		{
			nIndex0 = ppCells[i]->pdwCCVertIndices[j*3];
			nIndex1 = ppCells[i]->pdwCCVertIndices[j*3+1];
			nIndex2 = ppCells[i]->pdwCCVertIndices[j*3+2];

			if(false == ::_IntersectTriangle(vPos, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp)) continue;
			if(false == ::_IntersectTriangle(vPosNext, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2]))
			{
				fDistTmp = (vPos - vColTmp).Magnitude(); // �Ÿ��� �纸��..
				if(fDistTmp < fDistClosest) 
				{
					fDistClosest = fDistTmp;
					// �浹�̴�..
					if(pvCol) *pvCol = vColTmp;
					if(pvNormal)
					{
						(*pvNormal).Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], 
							m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
						(*pvNormal).Normalize();
					}
					if ( pVec )
					{
						pVec[0] = m_pvCollisions[nIndex0];
						pVec[1] = m_pvCollisions[nIndex1];
						pVec[2] = m_pvCollisions[nIndex2];
					}
				}
			}
		}
	}

	if(fDistClosest != FLT_MAX)
		return true;

	return false;
}

int CN3ShapeMgr::SubCellPathThru(const __Vector3& vFrom, const __Vector3& vAt, __CellSub** ppSubCells) // ���� ���̿� ��ģ �������� �����ش�..
{
	if(nullptr == ppSubCells) return 0;

	// ������ ���ϰ�..
	int xx1 = 0, xx2 = 0, zz1 = 0, zz2 = 0;

	if(vFrom.x < vAt.x) { xx1 = (int)(vFrom.x / CELL_SUB_SIZE); xx2 = (int)(vAt.x / CELL_SUB_SIZE); }
	else { xx1 = (int)(vAt.x / CELL_SUB_SIZE); 	xx2 = (int)(vFrom.x / CELL_SUB_SIZE); }

	if(vFrom.z < vAt.z)	{ zz1 = (int)(vFrom.z / CELL_SUB_SIZE); zz2 = (int)(vAt.z / CELL_SUB_SIZE); }
	else { zz1 = (int)(vAt.z / CELL_SUB_SIZE); zz2 = (int)(vFrom.z / CELL_SUB_SIZE); }

	bool bPathThru;
	float fZMin, fZMax, fXMin, fXMax;
	int nSubCellCount = 0;
	for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..
	{
		fZMin = (float)(z * CELL_SUB_SIZE);
		fZMax = (float)((z+1) * CELL_SUB_SIZE);
		for(int x = xx1; x <= xx2; x++)
		{
			fXMin = (float)(x * CELL_SUB_SIZE);
			fXMax = (float)((x+1) * CELL_SUB_SIZE);

			// Cohen thuderland algorythm
			uint32 dwOC0 = 0, dwOC1 = 0; // OutCode 0, 1
			if(vFrom.z > fZMax) dwOC0 |= 0xf000;
			if(vFrom.z < fZMin) dwOC0 |= 0x0f00;
			if(vFrom.x > fXMax) dwOC0 |= 0x00f0;
			if(vFrom.x < fXMin) dwOC0 |= 0x000f;
			if(vAt.z > fZMax) dwOC1 |= 0xf000;
			if(vAt.z < fZMin) dwOC1 |= 0x0f00;
			if(vAt.x > fXMax) dwOC1 |= 0x00f0;
			if(vAt.x < fXMin) dwOC1 |= 0x000f;

			bPathThru = false;
			if(dwOC0 & dwOC1) bPathThru = false; // �� ������ ���� ���� �ܺο� �ִ�.
			else if(dwOC0 == 0 && dwOC1 == 0) bPathThru = true;// ������ �簢�� ���ο� ����
			else if((dwOC0 == 0 && dwOC1 != 0) || (dwOC0 != 0 && dwOC1 == 0)) bPathThru = true;// ���� ������ ���� ���ο� ������ �ܺο� ����.
			else if((dwOC0 & dwOC1) == 0) // �� �L�� ��� �� �ܺο� ������ �Ǵ��� �ٽ� �ؾ� �Ѵ�.
			{
				float fXCross = vFrom.x + (fZMax - vFrom.z) * (vAt.x - vFrom.x) / (vAt.z - vFrom.z); // ���� ������ �������� ����ϰ�..
				if(fXCross < fXMin) bPathThru = false; // ������ �ܰ��� �ִ�.
				else bPathThru = true; // ��ó�ִ�.
			}

			if(false == bPathThru) continue;

			// �浹 ������ ��� �Ѵ�..
			int nX = x / CELL_MAIN_DEVIDE;
			int nZ = z / CELL_MAIN_DEVIDE;
			if(nX < 0 || nX >= MAX_CELL_MAIN || nZ < 0 && nZ >= MAX_CELL_MAIN) continue; // ���μ��ٱ��� ���� ��������.
			if(nullptr == m_pCells[nX][nZ]) continue; // ���μ��� ���̸� ��������..

			int nXSub = x%CELL_MAIN_DEVIDE;
			int nZSub = z%CELL_MAIN_DEVIDE;

			ppSubCells[nSubCellCount] = &(m_pCells[nX][nZ]->SubCells[nXSub][nZSub]);
			nSubCellCount++;
		} // end of for(int x = xx1; x <= xx2; x++)
	} // end of for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..

	return nSubCellCount; // ��ģ �� ������ �����ֱ�..
}

float CN3ShapeMgr::GetHeightNearstPos(const __Vector3 &vPos, __Vector3* pvNormal) // ���� ����� ���̰��� �����ش�. ������ -FLT_MAX �� �����ش�.
{
	__CellSub* pCell = this->SubCell(vPos.x, vPos.z); // ���꼿�� �����´�..
	if(nullptr == pCell || pCell->nCCPolyCount <= 0) return -FLT_MAX; // ���� ����.

	__Vector3 vPosV = vPos; vPosV.y = 5000.0f; // ����⿡ ��ġ�� �ϰ�..
	__Vector3 vDir(0,-1, 0); // ���� ���� ����
	__Vector3 vColTmp(0,0,0); // ���������� ���� ����� �浹 ��ġ..

	int nIndex0, nIndex1, nIndex2;
	float fT, fU, fV;
	float fNearst = FLT_MAX, fMinTmp = 0, fHeight = -FLT_MAX;		// �ϴ� �ּҰ��� ū������ ���..

	for ( int i = 0; i < pCell->nCCPolyCount; i++ )
	{
		nIndex0 = pCell->pdwCCVertIndices[i*3];
		nIndex1 = pCell->pdwCCVertIndices[i*3+1];
		nIndex2 = pCell->pdwCCVertIndices[i*3+2];

		// �浹�� ���� ������..
		if(true == ::_IntersectTriangle(vPosV, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp))
		{
			fMinTmp = (vColTmp - vPos).Magnitude();
			if(fMinTmp < fNearst) // ���� ����� �浹 ��ġ�� ã�� ���� �ڵ�..
			{
				fNearst = fMinTmp;
				fHeight = vColTmp.y; // ���̰�.

				if(pvNormal)
				{
					pvNormal->Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
					pvNormal->Normalize();
				}
			}
		}
	}

	return fHeight;
}

float CN3ShapeMgr::GetHeight(float fX, float fZ, __Vector3* pvNormal) // ���� ���� ���� �����ش�.. ������ -FLT_MAX���� �����ش�.
{
	__CellSub* pCell = this->SubCell(fX, fZ); // ���꼿�� �����´�..
	if(nullptr == pCell || pCell->nCCPolyCount <= 0) return -FLT_MAX; // ���� ����.

	__Vector3 vPosV(fX, 5000.0f, fZ); // ����⿡ ��ġ�� �ϰ�..
	__Vector3 vDir(0,-1, 0); // ���� ���� ����
	__Vector3 vColTmp(0,0,0); // ���������� ���� ����� �浹 ��ġ..

	int nIndex0, nIndex1, nIndex2;
	float fT, fU, fV;
	float fMaxTmp = -FLT_MAX;;

	for ( int i = 0; i < pCell->nCCPolyCount; i++ )
	{
		nIndex0 = pCell->pdwCCVertIndices[i*3];
		nIndex1 = pCell->pdwCCVertIndices[i*3+1];
		nIndex2 = pCell->pdwCCVertIndices[i*3+2];

		// �浹�� ���� ������..
		if(true == ::_IntersectTriangle(vPosV, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp))
		{
			if(vColTmp.y > fMaxTmp)
			{
				fMaxTmp = vColTmp.y;
				if(pvNormal)
				{
					pvNormal->Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
					pvNormal->Normalize();
				}
			}
		}
	}

	return fMaxTmp;
}

void CN3ShapeMgr::SubCell(const __Vector3& vPos, __CellSub** ppSubCell)			// �ش� ��ġ�� �� �����͸� �����ش�.
{
	int x = (int)(vPos.x / CELL_MAIN_SIZE);
	int z = (int)(vPos.z / CELL_MAIN_SIZE);

	// _ASSERT(x >= 0 && x < MAX_CELL_MAIN && z >= 0 && z < MAX_CELL_MAIN);

	int xx = (((int)vPos.x)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;			// 2, 3, 4
	int zz = (((int)vPos.z)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;			// 1, 0, 5
	// 8, 7, 6	
	for ( int i = 0; i < 9; i++ )
	{
		switch( i )
		{
		case 0:
			if ( m_pCells[x][z] != nullptr )
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz]);
			else
				ppSubCell[i] = nullptr;
			break;

		case 1:
			if ( (x == 0) && (xx == 0) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != 0) && (xx == 0) )
			{
				if ( m_pCells[x-1][z] != nullptr )
					ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz]);
				else
					ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz]);
			else
				ppSubCell[i] = nullptr;
			break;

		case 2:
			if ( (x == 0) && (xx == 0) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z == (CELL_MAIN_SIZE-1)) && ( zz == (CELL_MAIN_DEVIDE-1) ) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != 0) && (xx == 0) )											// x ����, z ����.
			{
				if ( (z != (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1) ) )
					if ( m_pCells[x-1][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x-1][z+1]->SubCells[CELL_MAIN_DEVIDE-1][0]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x-1][z] != nullptr )
						ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz+1]);
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1) ) )		// x ����, z ����.
			{
				if ( (x != 0) && (xx == 0) )
					if ( m_pCells[x-1][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x-1][z+1]->SubCells[CELL_MAIN_DEVIDE-1][0]);	
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x][z+1]->SubCells[xx-1][0]);	
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz+1]);						
			else
				ppSubCell[i] = nullptr;					
			break;

		case 3:
			if ( (z == (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )			// z ����.
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )
			{
				if ( m_pCells[x-1][z] != nullptr )
					ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[xx][0]);
				else
					ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz+1]);
			else
				ppSubCell[i] = nullptr;					
			break;

		case 4:
			if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )			// x ����, z ����.
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z == (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1)) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
			{
				if ( (z != (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1)) )
					if ( m_pCells[x+1][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x+1][z+1]->SubCells[0][0]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x+1][z] != nullptr )
						ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz+1]);
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )
			{
				if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
					if ( m_pCells[x+1][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x+1][z+1]->SubCells[0][0]);	
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x][z+1] != nullptr )
						ppSubCell[i] = &(m_pCells[x][z+1]->SubCells[xx+1][0]);	
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )								
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz+1]);						
			else
				ppSubCell[i] = nullptr;					
			break;

		case 5:																		// x ����.
			if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
			{
				if ( m_pCells[x+1][z] != nullptr )
					ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz]);
				else
					ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )								
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz]);
			else
				ppSubCell[i] = nullptr;					
			break;

		case 6:																		// x ����. z ����.		
			if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z == 0) && (zz == 0) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )			
			{
				if ( (z != 0) && (zz == 0) )
					if ( m_pCells[x+1][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x+1][z-1]->SubCells[0][CELL_MAIN_DEVIDE-1]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x+1][z] != nullptr )								
						ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz-1]);
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != 0) && (zz == 0) )
			{
				if ( (x != (CELL_MAIN_SIZE-1)) && (xx == (CELL_MAIN_DEVIDE-1) ) )
					if ( m_pCells[x+1][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x+1][z-1]->SubCells[0][CELL_MAIN_DEVIDE-1]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx+1][CELL_MAIN_DEVIDE-1]);	
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )								
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz-1]);						
			else
				ppSubCell[i] = nullptr;					
			break;

		case 7:																		// z ����.
			if ( (z == 0) && (zz == 0) )	
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != 0) && (zz == 0) )
			{
				if ( m_pCells[x][z-1] != nullptr )								
					ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx][CELL_MAIN_DEVIDE-1]);
				else
					ppSubCell[i] = nullptr;					
				break;
			}

			if ( m_pCells[x][z] != nullptr )								
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz-1]);
			else
				ppSubCell[i] = nullptr;					
			break;

		case 8:																		// x ����, z ����.
			if ( (x == 0) && (xx == 0) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (z == 0) && (zz == 0) )
			{
				ppSubCell[i] = nullptr;
				break;
			}

			if ( (x != 0) && (xx == 0) )
			{
				if ( (z != 0) && (zz == 0) )
					if ( m_pCells[x-1][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x-1][z-1]->SubCells[CELL_MAIN_DEVIDE-1][CELL_MAIN_DEVIDE-1]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x-1][z] != nullptr )								
						ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz-1]);
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( (z != 0) && (zz == 0) )
			{
				if ( (x != 0) && (xx == 0) )
					if ( m_pCells[x-1][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x-1][z-1]->SubCells[CELL_MAIN_DEVIDE-1][CELL_MAIN_DEVIDE-1]);
					else
						ppSubCell[i] = nullptr;
				else
					if ( m_pCells[x][z-1] != nullptr )								
						ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx-1][CELL_MAIN_DEVIDE-1]);	
					else
						ppSubCell[i] = nullptr;
				break;
			}

			if ( m_pCells[x][z] != nullptr )								
				ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz-1]);						
			else
				ppSubCell[i] = nullptr;
			break;
		}	// switch
	}	// for 
}

