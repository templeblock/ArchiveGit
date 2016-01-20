#include <windowsx.h>
#include <fstream.h>
#include "CarShots.h"

//
// NOTES:
// 1) Change memory allocation to use GameGuts
//

//***********************************************************************
CCarShots::CCarShots()
//***********************************************************************
{
	// Init
	m_nNumPrefixs = 0;
	m_pLastCarGate = NULL;
}

//***********************************************************************
CCarShots::~CCarShots()
//***********************************************************************
{
	// Delete the data blocks for all of the prefixs
	for(int i=0; i<m_nNumPrefixs; i++)
	{
		if (m_pData[i])
			GlobalFreePtr((HGLOBAL)m_pData[i]);
	}
}

//***********************************************************************
int CCarShots::Load(char *pszFileName)
//***********************************************************************
{	//
	// Load the car shot data which is a list of shot names.
	// Example list:
	//		H01WASC
	//		H01WAS2
	//		H01S8
	//		H01SA
	//		H01SB
	//		H02SB
	//		H03SAW2
	//		H03SCW2
	//		H03S2
	//		H03S4
	//		H04WB
	//		H04W1
	//		U02WCS2
	//		U02W2
	//		U03WBS1
	// The shot data must be grouped by cell type, look, and gate. Then
	// the most interesting or important shots should be first since
	// the list will be searched in sequential order.
	//

	char szLine[50];		// Line read in
	char *pszPrefix;		// Pointer to a prefix
	char *pData = NULL;		// Pointer to a prefix data block (all the shots
							// with the same prefix.)

	// init
	pszPrefix = m_szPrefix[0];
	pszPrefix[0] = NULL;
	m_nNumPrefixs = -1;		// Number of different prefixs
	int nByteCount = 0;		// Data block byte count
	int nBlockSize = 0;		// Size of the data block

	// open file
	ifstream in;
	in.open(pszFileName);	
	if (!in.bad())
	{
		// while there are entries
		while(!in.eof())
		{
			// Read in a line
			in >> szLine;

			// if prefix is different
			if (strncmp(szLine, pszPrefix, 3) != 0)
			{
				// Resize previous datablock
				if (pData)
				{
					// Add byte count for end block character
					++nByteCount;

					// Resize data block to exact count
					 pData = (char *)GlobalReAllocPtr(pData, nByteCount, 0);

					// Add end block character
					*(pData + nByteCount - 1) = cEndChar;

					// Assign data block to data block array
					m_pData[m_nNumPrefixs] = pData;
				}

				// start new prefix
				++m_nNumPrefixs;
				if (m_nNumPrefixs >= MaxNumPrefixs)
				{
					MessageBox(NULL, "Car Shot prefix overrun.", "Error", MB_ICONERROR);
					break;
				}
				
				// Copy prefix (first three chars)
				strncpy(m_szPrefix[m_nNumPrefixs], szLine, 3);
				m_szPrefix[m_nNumPrefixs][3] = NULL;
				pszPrefix = m_szPrefix[m_nNumPrefixs];

				// Allocate new data blcck
				nBlockSize = 2048;
				nByteCount = 0;
				pData = (char *)GlobalAllocPtr(GHND, nBlockSize);
			}

			// Make sure there is room in the block, max # of car gate chars plus null
			if ((nByteCount+MaxCarGateLen+1) >= nBlockSize)
			{
				nBlockSize += 2048;
				pData = (char *)GlobalReAllocPtr(pData, nBlockSize, 0);
			}

			// add item to data block, the item starts with the 4th char
			// and has a max length of MaxCarGateLen
			for(int i=3; i<(3+MaxCarGateLen); i++)
			{
				// If already at the end then break
				if (szLine[i] == NULL)
					break;

				*(pData + nByteCount) = szLine[i];
				++nByteCount;
			}

			// Make sure there is a null char so that car gate groups are seperated in 
			// the block
			*(pData + nByteCount) = NULL;
			++nByteCount;
		}

		// Clean up last data block one
		if (pData)
		{
			// Add byte count for end block character
			++nByteCount;

			// Resize data block to exact count
			 pData = (char *)GlobalReAllocPtr(pData, nByteCount, 0);

			// Add end block character
			*(pData + nByteCount - 1) = cEndChar;

			// Assign data block to data block array
			m_pData[m_nNumPrefixs] = pData;
		}

		// Inc num of prefixs to reflect actual number
		++m_nNumPrefixs;

	}
	in.close();

	return 0;
}

//***********************************************************************
char *CCarShots::GetOptimalShot(char *pszPrefix, OppCarShot *pOppCars, int nOppCarCnt)
//***********************************************************************
{	//
	// Gets the best shot name given a prefix and a list of cars (pOppCars)
	//
	int i;
	int nIndex = -1;
	char *pData = NULL;

	if (nOppCarCnt < 1)
		return 0;

	// Find the prefix
	for(i=0; i<m_nNumPrefixs; i++)
	{
		if (strcmp(pszPrefix, m_szPrefix[i]) == 0)
		{
			pData = m_pData[i];
			break;
		}
	}

	// Not found
	if (!pData)
		return NULL;

	char *pszCarGate = pData;
	int iLen;
	char cType;
	char cGate;
	BOOL bFound = FALSE;
	int j;
	int iCarCnt = 0;
	const int MaxOpps = 3;
	OppCarShot *pOppShots[MaxOpps];

	// Search the data block
	while ( *pszCarGate != cEndChar )
	{
		iLen = lstrlen(pszCarGate);

		iCarCnt = 0;

		// For each car type, gate combination (two chars)
		// in the shot name
		for(i=0; i<iLen; i+=2)
		{
			cType = *(pszCarGate + i);
			cGate = *(pszCarGate + i + 1);

			bFound = FALSE;

			// Loop through the car shot list, if cType and cGate
			// are found then set bFound to true
			for(j=0; j<nOppCarCnt; j++)
			{
				if (cType == pOppCars[j].cType &&
					cGate == pOppCars[j].cGate)
				{
					bFound = TRUE;
					if (iCarCnt < MaxOpps)
					{
						pOppShots[iCarCnt++] = &pOppCars[j];
					}
					break;
				}

			} // end for j

			// The car and gate were not found so stop searching
			// for this particular shot name and move on to the next
			// in the data block
			if (bFound == FALSE)
				break;
		} // end for i

		if (m_pLastCarGate == pszCarGate)
			i = 0;


		// If a shot name was found
		if (bFound && (m_pLastCarGate != pszCarGate))
		{
			// For all the cars in the shot name
			for(i=0; i<iCarCnt; i++)
			{
				// Flag the car as in the shot and set the drive mode
				// to indicate that the opponent is in position
				pOppShots[i]->nState = CARSHOT_STATE_INSHOT;
				pOppShots[i]->pOpp->SetDriveMode(DRIVEMODE_POSITION);
			}
			return pszCarGate;
		}

		// Else, point to the next shot name in the data block
		else
			pszCarGate = pszCarGate + iLen + 1;

	} // end while

	return NULL;
}

