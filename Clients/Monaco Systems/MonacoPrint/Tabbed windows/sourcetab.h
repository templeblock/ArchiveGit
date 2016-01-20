//////////////////////////////////////////////////////////////////////////////
//  sourcetab.h															//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_SOURCETAB
#define IN_SOURCETAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"

class SourceTab:public WindowTab {
private:
protected:
	RefConType	*refCon1;
	ProfileDoc	*doc;
	
public:
	SourceTab(ProfileDoc *dc,WindowPtr d);	
	~SourceTab(void);
	
	McoStatus CopyParameters(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);


	};

#endif