//////////////////////////////////////////////////////////////////////////////
//  tabbedwindow.h															//
//	a window with tabs														//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_TABBEDWINDOW
#define IN_TABBEDWINDOW

#include "mcostat.h"
#include "basewin.h"
#include "windowtab.h"


class TabbedWindow:public baseWin {
private:
protected:
	WindowTab		*activeWindow;
	WindowTab		**windows;
	
	int				numTabs;
	int				activeTab;
	int				lastactiveTab;
	Rect			*tabRects;
	Rect			fullRect;
	
	int				*enabledTab;
	
public:
	TabbedWindow(void);
	~TabbedWindow(void);
	};
	
#endif