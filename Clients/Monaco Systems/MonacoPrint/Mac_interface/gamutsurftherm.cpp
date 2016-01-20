/*      gamutsurftherm.cpp
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/

#include "gamutsurftherm.h"

struct ParamPass {
	GammutComp	*gammutcomp;
	ProfileDoc	*doc;
	McoHandle	g1H,g2H;
	double		gamutexpand;
	int			notdocdata;
	};


pascal void *gammutSurfaceThread(void *threadParameter )
{
ParamPass *parampass;
McoStatus	status = MCO_SUCCESS;
double		*temp1,*temp2;
long		i;

parampass = (ParamPass*)threadParameter;


MaxApplZone();
if (parampass->doc) 
	{
	if ((!parampass->doc->didGammutSurface) || (parampass->notdocdata))
		status = parampass->gammutcomp->get_gamut_surface();
	}
else status = parampass->gammutcomp->get_gamut_surface();
if (!status)
	{
	if ((parampass->doc) && (!parampass->notdocdata))
		{
		parampass->doc->didGammutSurface = 1;

		temp1 = (double*)McoLockHandle(parampass->g1H);
		temp2 = (double*)McoLockHandle(parampass->doc->_gammutSurfaceH);
		for (i=0; i<4*256*360; i++) temp2[i] = temp1[i];
		McoUnlockHandle(parampass->g1H);
		McoUnlockHandle(parampass->doc->_gammutSurfaceH);
		temp1 = (double*)McoLockHandle(parampass->g2H);
		temp2 = (double*)McoLockHandle(parampass->doc->_gammutSurfaceBlackH);
		for (i=0; i<4*256*360; i++) temp2[i] = temp1[i];
		McoUnlockHandle(parampass->g2H);
		McoUnlockHandle(parampass->doc->_gammutSurfaceBlackH);
		}
	}
*parampass->gammutcomp->thermd->finished = 1;
delete parampass;
}

GamutSurfaceWin::GamutSurfaceWin(ProfileDoc *dc,RawData *pd,int dpd,MS_WindowTypes wint,int winn,unsigned char *patchFName):ThermWin(dc,Process_GammutSurface,1000,wint,winn)
{
	gammutcomp = 0L;
	pdata = pd;
	delete_pdata = dpd;
	ParamPass	*parampass = 0L;
	double		maxgrayL,mingrayL;
	double		*temp1,*temp2;
	long		i;
	int			notdocdata = 0;

	gdata = new GammutWinData;	// will delete this in winevent
	if (gdata == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}
	
	gdata->g1H = McoNewHandle(sizeof(double)*4*256*360);
	if (gdata->g1H == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}
	gdata->g2H = McoNewHandle(sizeof(double)*4*256*360);
	if (gdata->g2H == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}

	if (dc)
		{
		maxgrayL = dc->_maxgrayL;
		mingrayL = dc->_mingrayL;
		if ((dc->didGammutSurface)  && (pd == dc->patchD)) // just copy
			{
			temp1 = (double*)McoLockHandle(gdata->g1H);
			temp2 = (double*)McoLockHandle(dc->_gammutSurfaceH);
			for (i=0; i<4*256*360; i++) temp1[i] = temp2[i];
			McoUnlockHandle(gdata->g1H);
			McoUnlockHandle(dc->_gammutSurfaceH);
			temp1 = (double*)McoLockHandle(gdata->g2H);
			temp2 = (double*)McoLockHandle(dc->_gammutSurfaceBlackH);
			for (i=0; i<4*256*360; i++) temp1[i] = temp2[i];
			McoUnlockHandle(gdata->g2H);
			McoUnlockHandle(dc->_gammutSurfaceBlackH);
			}
		else if (pd != dc->patchD) notdocdata = 1;
		}
	else
		{
		maxgrayL = 100;
		mingrayL = 0;
		}
		

#ifdef USE_THREADS

	thermd->SetUpTherm(Process_GammutSurface,1000);
	gammutcomp = new GammutComp(pdata,0L,0L,0L,gdata->g1H,gdata->g2H,0L,0L,0L,
								0,100,0,0,maxgrayL,
								mingrayL,100,0,0,0L,thermd);


	if (gammutcomp == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}
	
	parampass = new ParamPass;
	
	parampass->gammutcomp = gammutcomp;
	parampass->gamutexpand = 0;
	parampass->doc = (ProfileDoc*)dc;
	parampass->g1H = gdata->g1H;
	parampass->g2H = gdata->g2H;
	parampass->notdocdata = notdocdata;

// create the thread
	finished = 0;
	if( NewThread( kCooperativeThread, gammutSurfaceThread, (void *) parampass, 0L, kCreateIfNeeded, 0L, &threadID) != noErr )
		{
		finished = 1;
		}

#else	

	gammutcomp = new GammutComp(pdata,0L,0L,0L,gdata->g1H,gdata->g2H,0L,0L,0L,
								0,100,0,0,maxgrayL,
								mingrayL,100,0,0,0L,thermd);


	if (gammutcomp == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}

	gammutcomp->get_gamut_surface();
	finished = 1;
#endif
		
	if (pdata->type != RGBPatches) gdata->gH = gdata->g2H;
	else gdata->gH = gdata->g1H;
	
	memcpy(gdata->fn, patchFName, *(patchFName)+1);

	if (patchFName != NULL) SetWTitle( dp, patchFName);

	return;
bail:	

	if (gdata) delete gdata;
}



GamutSurfaceWin::~GamutSurfaceWin(void)
{
	// must kill before deleting any of the objects
	KillThread();
	if (gammutcomp) delete gammutcomp;
	if ((delete_pdata) && (pdata)) delete pdata;
	
}