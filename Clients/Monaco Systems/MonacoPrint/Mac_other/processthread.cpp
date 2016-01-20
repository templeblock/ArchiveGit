/********************************************************************************/
/*  Start processing of the table by using threds								*/
/*  processthread.cpp															*/
/* James Vogh																	*/
/*  July 22 1997																*/
/********************************************************************************/



pascal void *processStart(void *dc)
{
	ProfileDoc	*doc;
	ThreadID	threadid;
	OSErr		er;
	
	doc = (ProfileDoc*)dc;
	
	doc->thermd->setMax(10000);
	doc->_handle_calibrate()();
}


McoStatus startProcessing(ProfileDoc *doc)
{

if( NewThread( kCooperativeThread, processStart, (void *) doc, 0L, kCreateIfNeeded, 0L, &threadid) != noErr )
	return MCO_FAILURE;

return MCO_SUCCESS;
}

pascal void *gammutSurfaceStart(void *dc)
{
	ProfileDoc	*doc;
	ThreadID	threadid;
	OSErr		er;
	
	doc = (ProfileDoc*)dc;
	
	doc->thermd->setMax(1500);
	doc->_handle_calibrate()();
}


McoStatus startgammutSurface(ProfileDoc *doc)
{

if( NewThread( kCooperativeThread, processStart, (void *) doc, 0L, kCreateIfNeeded, 0L, &threadid) != noErr )
	return MCO_FAILURE;

return MCO_SUCCESS;
}

pascal void *gammutCompressStart(void *gc)
{
	GammutComp	*gammutcomp;
	ThreadID	threadid;
	OSErr		er;
	
	doc = (ProfileDoc*)gc;
	
	gc->thermd->setMax(1500);
	doc->_handle_calibrate()();
}


McoStatus startgammutCompress(ProfileDoc *doc)
{

if( NewThread( kCooperativeThread, processStart, (void *) doc, 0L, kCreateIfNeeded, 0L, &threadid) != noErr )
	return MCO_FAILURE;

return MCO_SUCCESS;
}