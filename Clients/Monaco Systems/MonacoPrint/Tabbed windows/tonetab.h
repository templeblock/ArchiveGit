//////////////////////////////////////////////////////////////////////////////
//  tonetab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_TONETAB
#define IN_TONETAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"
#include "control.h"

class ToneTab:public WindowTab {
private:
protected:
	RefConType	*refCon1;
	ProfileDoc	*doc;
	
	control 	*controls[2];
	int			numControls;
	
	int		last_changed;
	
public:
	ToneTab(ProfileDoc *dc,WindowPtr d);	
	~ToneTab(void);
	
	void CopyIntoControl(int cnt_num);
	
	void CopyOutofControl(int cnt_num);
	
	McoStatus CopyParameters(void);
	McoStatus updateWindowData(int changed);
	McoStatus UpdateWindow(void);
	Boolean isMyObject(Point where,short *item);
	Boolean   KeyFilter(EventRecord *theEvent,short *itemHit );
	McoStatus   updateDataForControl(int num);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);


	};

#endif