#ifndef _AMBIENTSND_H
#define _AMBIENTSND_H

#include "DirSnd.h"

enum AmbSndType
{
	TauntSnd,
	PraiseSnd,
	DeathSnd,
	Energy1Snd,
	Energy2Snd
};

const int MaxTaunts = 6;
const int MaxPraises = 5;
const int MaxDeaths = 1;

class CAmbientSnd
{
public:
	CAmbientSnd();
	~CAmbientSnd();

	void Init(void);
	void Free(void);

	BOOL Play(AmbSndType SndType);

private:
	CDirSnd *pTaunts[MaxTaunts];

	CDirSnd *pPraises[MaxPraises];

	CDirSnd *pDeaths[MaxDeaths];

	CDirSnd *pEnergy1;
	CDirSnd *pEnergy2;
};

#endif
