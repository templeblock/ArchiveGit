/*      gamutsurf.cpp
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/

#include "gamutsurf.h"


pascal void *gammutSurfaceThread(void *threadParameter )
{
GammutComp *gammutcomp;

gammutcomp = (GammutComp*)threadParameter;

MaxApplZone();

gammutcomp->get_gamut_surface(void);
}

GamutSurfaceWin::GamutSurfaceWin(ProfileDoc *doc,RawData *pdata,MS_WindowTypes wint,int winn,unsigned char *patchFName):ThermWin(Process_GammutSurface,1000,wint,winnum)
{

	gdata = new GammutWinData;	// will delete this in winevent
	if (gdata == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}
	
	gdata->g1H = McoNewHandle(sizeof(double)*4*256*360);
	if (gdata->g1H == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}
	gdata->g2H = McoNewHandle(sizeof(double)*4*256*360);
	if (gdata->g2H == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}

	doc->thermd = &thermd;

	doc->thermd->SetUpTherm(Process_GammutSurface,1000);
	gammutcomp = new GammutComp(pdata,0L,0L,0L,gdata->g1H,gdata->g2H,0L,0L,0L,
								0,100,0,0,doc->_maxgrayL,doc->_mingrayL,100,0,0,0L,doc->thermd);


	if (gammutcomp == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}

// create the thread
	finished = 0;

	if( NewThread( kCooperativeThread, gammutSurfaceThread, (void *) gammutcomp, 0L, kCreateIfNeeded, 0L, &threadID) != noErr )
		{
		finished = 1;
		}
	else goto bail;	
		
	if (pdata->type != RGBPatches) gdata->gH = gdata->g1H;
	else gdata->gH = gdata->g2H;
	
	memcpy(gdata.fn, patchFName, *(patchFName)+1);

	if (patchFName != NULL) SetWTitle( dp, patchFName);

	return;
bail:	

	if (gdata) delete gdata;
}



GamutSurfaceWin1::~GamutSurfaceWin1(void)
{
	delete gammutcomp;
}