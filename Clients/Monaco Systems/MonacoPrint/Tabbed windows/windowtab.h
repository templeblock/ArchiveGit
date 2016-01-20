//////////////////////////////////////////////////////////////////////////////
//  windowtab.h																//
//	a tab within a window													//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_WINDOWTAB
#define IN_WINDOWTAB

#include "basewin.h"

#define TAB_TOP	50
#define TAB_BOTTOM 379
#define TAB_LEFT 14
#define TAB_RIGHT 590

class WindowTab:public virtual baseWin {
private:
protected:
	int			numDITL;	// the number of items for this tab
	short		idDITL;		// the id of the DITL
	short		startNum;	// the starting id 
public:
	WindowTab(DialogPtr dialp,int	Ditl_Id);
	~WindowTab(void);
	McoStatus hideWindow(void);
	virtual McoStatus CopyParameters(void) {return MCO_SUCCESS;}
	virtual McoStatus SetUpDial(void) {return MCO_SUCCESS;}
	virtual McoStatus showWindow(void);
	virtual int		  killTab(void)  {return 0;}
	};
	
#endif