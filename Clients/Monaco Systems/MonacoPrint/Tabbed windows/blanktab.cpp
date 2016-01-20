//////////////////////////////////////////////////////////////////////////////
// blanktab.cpp															//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "blanktab.h"

#define ST_DITL_ID	6006
#define WAIT_ID		1
#define WAIT_PICT	150

BlankTab::BlankTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{

	doc = dc;
	
	WinType = BlankWinTab;

	
}


BlankTab::~BlankTab(void)
{
dp = 0L;
}

McoStatus BlankTab::CopyParameters(void)
{
return MCO_SUCCESS;
}

McoStatus BlankTab::UpdateWindow(void)	
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



// The event handler for the main menu
McoStatus	BlankTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{

	// no codes passed back
	*numwc = 0;
	

	return MCO_SUCCESS;
}