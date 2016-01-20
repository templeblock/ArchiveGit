#ifndef _CARSHOTS_H
#define _CARSHOTS_H

#include "Opponents.h"

// For maximum # of prefixes like H01, consider gates and # of looks
// 5 looks * 4 gates for hex plus 5 looks * 6 gates for union jacks
const int MaxNumPrefixs = 50;
const int MaxPrefixLen = 4;

// Length of the car gate portion of the shot name like - WBSCB2x
const int MaxCarGateLen = 7;

// End character for a car gate data block
const int cEndChar = 1;

class CCarShots
{
public:
	CCarShots();
	~CCarShots();

	int Load(char *pszFileName);
	char *GetOptimalShot(char *pszPrefix, OppCarShot *pOppCars, int nOppCarCnt); 
	void SetLastCarGate(char *pszLast) {m_pLastCarGate = pszLast;}

private:
	int  m_nNumPrefixs;	// Number of prefixs in the list
	char m_szPrefix[MaxNumPrefixs][MaxPrefixLen];	// prefix list
	//int  m_nLastFound[MaxNumPrefixs];	// last one found in a list
	char *m_pData[MaxNumPrefixs];	// data blocks for each prefix

	char *m_pLastCarGate;	// Last car gate shot used
};

#endif