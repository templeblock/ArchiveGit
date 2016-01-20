//////////////////////////////////////////////////////////////////////////////
//  datatab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_SOURCETAB
#define IN_SOURCETAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"

class DataTab:public WindowTab {
private:
protected:
	RefConType	*refCon1;
	ProfileDoc	*doc;
	
public:
	DataTab(ProfileDoc *dc,WindowPtr d);	
	~DataTab(void);
	
	McoStatus CopyParameters(void);
	
	Boolean alwaysUpdate(void) { return TRUE;}
	McoStatus updateWindowData(int changed);
	
	McoStatus UpdateWindow(void);
	
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);


	};

#endif