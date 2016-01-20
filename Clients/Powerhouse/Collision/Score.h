#ifndef _Score_h
#define _Score_h

#include <windows.h>
#include "phtypes.h"

const int MaxUserName = 80;

typedef struct {
	STRING szName;
	long   lScore;
} SCORE_REC;

class CScore
{
public:
	CScore()			{ m_lScore = 0L; }
	void Add(long lInc)	{ m_lScore += lInc; }
	long Get()			{ return m_lScore; }
	void Set(long lScore) {m_lScore = lScore;}
	//BOOL Register();
	BOOL Retrieve();
	BOOL Save();

private:
	long m_lScore;

	void WriteHighScore(char *szName, long lScore);

};

#endif