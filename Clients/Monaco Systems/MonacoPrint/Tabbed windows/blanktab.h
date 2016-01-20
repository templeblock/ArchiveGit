//////////////////////////////////////////////////////////////////////////////
//  blanktab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_BLANKTAB
#define IN_BLANKTAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"

class BlankTab:public WindowTab {
private:
protected:
	ProfileDoc	*doc;

	
public:
	BlankTab(ProfileDoc *dc,WindowPtr d);	
	~BlankTab(void);
	
	McoStatus CopyParameters(void);
	McoStatus UpdateWindow(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);

	};

#endif