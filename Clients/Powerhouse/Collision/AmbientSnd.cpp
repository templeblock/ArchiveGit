#include <stdlib.h>
#include "cllsnid.h"
#include "AmbientSnd.h"
#include "proto.h"

//***********************************************************************
CAmbientSnd::CAmbientSnd()
//***********************************************************************
{
	Init();
}

//***********************************************************************
CAmbientSnd::~CAmbientSnd()
//***********************************************************************
{
	Free();
}

//***********************************************************************
void CAmbientSnd::Init(void)
//***********************************************************************
{
	int i;
	ITEMID Id;

	Id = IDC_TAUNT01;

	for (i=0; i<MaxTaunts; i++)
	{
		pTaunts[i] = new CDirSnd();
		pTaunts[i]->Open(Id, GetApp()->GetInstance());
		++Id;
	}

	Id = IDC_PRAISE01;

	for (i=0; i<MaxPraises; i++)
	{
		pPraises[i] = new CDirSnd();
		pPraises[i]->Open(Id, GetApp()->GetInstance());
		++Id;
	}

	Id = IDC_DEATH01;

	for (i=0; i<MaxDeaths; i++)
	{
		pDeaths[i] = new CDirSnd();
		pDeaths[i]->Open(Id, GetApp()->GetInstance());
		++Id;
	}

	pEnergy1 = new CDirSnd();
	pEnergy1->Open(IDC_ENERGY1SND, GetApp()->GetInstance());
	pEnergy2 = new CDirSnd();
	pEnergy2->Open(IDC_ENERGY2SND, GetApp()->GetInstance());
}

//***********************************************************************
void CAmbientSnd::Free(void)
//***********************************************************************
{
	int i;

	for (i=0; i<MaxTaunts; i++)
	{
		delete pTaunts[i];
	}
	for (i=0; i<MaxPraises; i++)
	{
		delete pPraises[i];
	}
	for (i=0; i<MaxDeaths; i++)
	{
		delete pDeaths[i];
	}
}

//***********************************************************************
BOOL CAmbientSnd::Play(AmbSndType SndType)
//***********************************************************************
{
	srand( (unsigned)timeGetTime());
	int iRand = rand();

	int iIndex;
	CDirSnd *pSnd = NULL;

	switch(SndType)
	{
		case TauntSnd:
			iIndex = iRand * (MaxTaunts-1) / RAND_MAX;
			pSnd = pTaunts[iIndex];
			break;
		case PraiseSnd:
			iIndex = iRand * (MaxPraises-1) / RAND_MAX;
			pSnd = pPraises[iIndex];
			break;
		case DeathSnd:
			//iRand = rand();
			//iIndex = iRand * MaxDeaths / RAND_MAX;
			//pSnd = pDeaths[0];
			return FALSE;
			break;
		case Energy1Snd:
			pSnd = pEnergy1;
			break;
		case Energy2Snd:
			pSnd = pEnergy2;
			break;
		default:
			return FALSE;
	}

	if (pSnd)
		pSnd->Play();

	return TRUE;
}
