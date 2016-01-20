#ifndef _ShotTbl_h
#define _ShotTbl_h

#include "vtable.h"

const int MaxHeadDesc = 40;
const int MaxVer = 20;
const int MaxLastMod = 20;

typedef struct tagShotHead
{
	char szDesc[MaxHeadDesc];
	char szVer[MaxVer];
	char szLastMod[MaxLastMod];
	DWORD dwNumRecs;
	DWORD dwReserved[10];
} SHOT_HEAD;

const int MaxComment = 80;

typdef struct tagShotRec
{
	SHOT Shot;
	char szComment[MaxComment];
} SHOT_REC;

#endif