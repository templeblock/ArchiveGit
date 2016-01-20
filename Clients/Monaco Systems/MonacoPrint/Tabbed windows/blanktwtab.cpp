//////////////////////////////////////////////////////////////////////////////
// blanktwtab.cpp															//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "blanktwtab.h"

#define ST_DITL_ID	6006
#define WAIT_ID		1
#define WAIT_PICT	150

BlankTwTab::BlankTwTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{

	doc = dc;

// set up the slider


// set up the parameters

	startGamut = 0;
	doneGamut = 0;
	restartGamut = 0;
	count = 0;
	_last_tick_time2 = 0;
	
}


BlankTwTab::~BlankTwTab(void)
{
dp = 0L;
}

McoStatus BlankTwTab::CopyParameters(void)
{
return MCO_SUCCESS;
}

McoStatus BlankTwTab::UpdateWindow(void)	
{			
PicHandle	pict;
Rect r1;
short	iType;
Handle	iHandle;

pict = GetPicture(WAIT_PICT);
GetDItem (dp, WAIT_ID+startNum, &iType, (Handle*)&iHandle, &r1);
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}
return MCO_SUCCESS;
}

Boolean BlankTwTab::TimerEventPresent(void)
{
if ((!doc->didGammutCompression) && (!startGamut))
	{
	return TRUE;
	}
return FALSE;
}

McoStatus BlankTwTab::updateWindowData(int changed)
{

if ((doc->didGammutCompression) && (!doneGamut))
	{
	doneGamut = 1;
	}
	
restartGamut = 0;	
if ((!doc->locked) && (!doneGamut) && (startGamut)) 
	{
	if (count > 4)
		{
		startGamut = 0;
		restartGamut = 1;
		count = 0;
		}
	count++;
	}
	
return MCO_SUCCESS;
}

int	BlankTwTab::killTab(void)
{
if ( (doc->didGammutCompression) && (doneGamut)) return 1;
if (restartGamut) return 2;
return 0;
}

// The event handler for the main menu
McoStatus	BlankTwTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{

	// no codes passed back
	*numwc = 0;
	
	if ((!doc->didGammutCompression) && (!startGamut))
		{
		wc[0].code = WE_Open_Window;
		wc[0].wintype = GammutCompTherm;
		wc[0].winnum = 0;
		wc[0].doc = doc;
		*numwc = 1;
		startGamut = 1; 
		} 

	
	return MCO_SUCCESS;
}