//////////////////////////////////////////////////////////////////////////////
//  blanktwtab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_BLANKTWTAB
#define IN_BLANKTWTAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"

class BlankTwTab:public WindowTab {
private:
protected:
	ProfileDoc	*doc;
	int			startGamut;
	int 		restartGamut;
	int			doneGamut;
	
	int			count;
	
	long _last_tick_time2;
	
public:
	BlankTwTab(ProfileDoc *dc,WindowPtr d);	
	~BlankTwTab(void);
	
	McoStatus CopyParameters(void);
	McoStatus UpdateWindow(void);
	McoStatus updateWindowData(int changed);
	Boolean TimerEventPresent(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
	int		  killTab(void);

	};

#endif