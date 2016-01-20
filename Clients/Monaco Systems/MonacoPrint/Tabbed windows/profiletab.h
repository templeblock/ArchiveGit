//////////////////////////////////////////////////////////////////////////////
//  profiletab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_PROFILETAB
#define IN_PROFILETAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"

class ProfileTab:public WindowTab {
private:
protected:
	RefConType	*refCon1;
	RefConType	*refCon2;
	RefConType	*refCon3;
	RefConType	*refCon4;
	ProfileDoc	*doc;
	
	int			tabwinnum;
	
	double		params[5];
	int			lmode;
	
public:
	ProfileTab(ProfileDoc *dc,WindowPtr d,int twn);	
	~ProfileTab(void);
	
	McoStatus CopyParameters(void);
	McoStatus SetParameters(int *ch);
	McoStatus updateWindowData(int changed);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
	McoStatus SetUpDial(void);

	};

#endif