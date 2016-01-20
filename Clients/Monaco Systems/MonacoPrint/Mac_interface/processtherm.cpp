/*      processtherm.cpp
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/

#include "processtherm.h"
#include "errordial.h"

pascal void *processThread(void *threadParameter );
pascal void *processThread(void *threadParameter )
{
ProfileDoc *doc;

doc = (ProfileDoc*)threadParameter;

MaxApplZone();

doc->locked = 1;
doc->processing = 1;
*doc->thermd->error =  doc->_handle_calibrate();
if (*doc->thermd->error == MCO_MEM_ALLOC_ERROR) 
	{
	McoErrorAlert(*doc->thermd->error);
	*doc->thermd->error = MCO_SUCCESS;
	}
doc->locked = 0;
doc->processing = 0;
doc->changedLinearTone = 0;
doc->changedProfile = 0;
*doc->thermd->finished = 1;
}

ProcessWin::ProcessWin(ProfileDoc *dc,MS_WindowTypes wint,int winn,unsigned char *patchFName):ThermWin(dc,Process_GammutSurface,1000,wint,winn)
{
	long free; 
	
	if (!doc) { finished = 1; error = MCO_OBJECT_NOT_INITIALIZED; return;}
	
	free = GetFreeMem();
	if (free <4900000) {finished = 1; error = MCO_MEM_ALLOC_ERROR; return;}

	((ProfileDoc*)doc)->thermd = thermd;

	((ProfileDoc*)doc)->thermd->SetUpTherm(Process_GammutSurface,10000);

// create the thread
	finished = 0;

#ifdef USE_THREADS

	if( NewThread( kCooperativeThread, processThread, (void *) doc, 0L, kCreateIfNeeded, 0L, &threadID) != noErr )
		{
		finished = 1;
		}

#else 
((ProfileDoc*)doc)->processing = 1;
((ProfileDoc*)doc)->_handle_calibrate();
finished = 1;
((ProfileDoc*)doc)->processing = 0;
#endif			

	if (patchFName != NULL) SetWTitle( dp, patchFName);

	return;
bail:	

	if (gdata) delete gdata;
}



ProcessWin::~ProcessWin(void)
{
}