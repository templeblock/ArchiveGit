//////////////////////////////////////////////////////////////////////////////
//  sourcetab.cpp															//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#define ST_DITL_ID	6003
#define ST_DITL_NUM	14

SourceTab::SourceTab(ProfileDoc *dc,WindowPtr d):WindowTab(dp,ST_DITL_ID,ST_DITL_NUM)
{

	doc = dc;

// set up the slider


// set up the parameters

	CopyParameters();
	
}


SourceTab::~SourceTab(void)
{
if (refCon1) delete refCon1;
}

McoStatus SourceTab::CopyParameters(void)
{
}


// The event handler for the main menu
McoStatus	SourceTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{

	// no codes passed back
	*numwc = 0;
	
}