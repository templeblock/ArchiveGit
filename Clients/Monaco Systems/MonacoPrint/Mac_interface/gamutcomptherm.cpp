/*      gamutcomptherm.cpp
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/

#include "gamutcomptherm.h"


pascal void *gammutCompThread(void *threadParameter )
{
ParamPass *parampass;
McoStatus status = MCO_SUCCESS;

parampass = (ParamPass*)threadParameter;

MaxApplZone();

parampass->doc->locked = 1;
if ((!parampass->doc->didGammutSurface) || (!parampass->doc->didGammutCompression))
	{
	parampass->doc->_initicclab();
	parampass->doc->_apply_color_balance();
	status = parampass->gammutcomp->apply_gamut_comp2(parampass->gamutexpand);
	}
if (!status)
	{
	parampass->doc->didGammutSurface = 1;
	parampass->doc->didGammutCompression = 1;
	} 
*parampass->gammutcomp->thermd->finished = 1;
parampass->doc->locked = 0;
delete parampass;
}

GamutCompWin::GamutCompWin(ProfileDoc *dc,MS_WindowTypes wint,int winn):ThermWin(dc,Process_GammutSurface,1000,wint,winn)
{
double l_linear;
ParamPass	*parampass = 0L;
ProfileDoc 	*pdoc = (ProfileDoc*)doc;

	gammutcomp = 0L;

	pdoc->thermd = thermd;

	pdoc->thermd->SetUpTherm(Process_GammutSurface,2000);
	
	pdoc->_SaturationAmount = pdoc->_condata.saturation;
	pdoc->_PaperWhite = pdoc->_condata.col_balance;
	pdoc->_col_bal_cutoff = pdoc->_condata.col_bal_cut;

	// do gammut compression starting her

	
	l_linear = (double)pdoc->_condata.L_linear/100.0;
	
	gammutcomp = new GammutComp(pdoc->patchD,pdoc->_labtableH,pdoc->_labtableCopyH,pdoc->_out_of_gamutH,
		pdoc->_gammutSurfaceH,pdoc->_gammutSurfaceBlackH,NULL,NULL,NULL,
		pdoc->_SaturationAmount,pdoc->_PaperWhite,pdoc->_col_bal_cutoff,l_linear,
		pdoc->_maxgrayL,pdoc->_mingrayL,100,0,0,0L,pdoc->thermd);

	if (gammutcomp == 0L) {error = MCO_MEM_ALLOC_ERROR; goto bail;}

// create the thread
	finished = 0;

	parampass = new ParamPass;
	
	parampass->gammutcomp = gammutcomp;
	parampass->gamutexpand = (1-pdoc->_SaturationAmount/100)+1.2*(pdoc->_SaturationAmount/100);
	parampass->doc = pdoc;
	
	if( NewThread( kCooperativeThread, gammutCompThread, (void *) parampass, 0L, kCreateIfNeeded, 0L, &threadID) != noErr )
		{
		finished = 1;
		}


	return;

bail:
	if (parampass) delete parampass;
	return;
	
}



GamutCompWin::~GamutCompWin(void)
{
	// must kill before deleting any of the objects
	KillThread();
	if (gammutcomp) delete gammutcomp;
}