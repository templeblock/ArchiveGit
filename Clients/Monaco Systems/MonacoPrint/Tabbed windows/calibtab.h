//////////////////////////////////////////////////////////////////////////////
//  calibtab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_CALIBTAB
#define IN_CALIBTAB

#include "mcostat.h"
#include "profiledoc.h"
#include "windowtab.h"
#include "control.h"
#include "measure.h"

class CalibTab:public WindowTab {
private:
protected:
	RefConType	*refCon1;
	ProfileDoc	*doc;
	
	control 	*controls[2];
	int			numControls;
	
	int 		numhand;
	
	RawData		*patchD;
	measureDevice *measure;
	int32		_last_tick_time2;
	
	int		last_changed;
	
public:
	CalibTab(ProfileDoc *dc,WindowPtr d);	
	~CalibTab(void);
	
	McoStatus setUpNewGrid(int newhand);
	McoStatus setUpMeasure(void);
	McoStatus CopyPatchIntoDoc(void);
	
	void CopyIntoControls(int cntl_num);
	void CopyOutofControls(int cntl_num);
	
	McoStatus CopyParameters(void);
	// check to see if the current X's in doc match the current X's in the grid
	// if all X's in the doc also exist in the grid then the grid does not need to be resized
	int doesXMatch(void);
	McoStatus updateWindowData(int changed);
	McoStatus UpdateWindow(void);
	Boolean isMyObject(Point where,short *item);
	// handle keyboard events
	Boolean KeyFilter(EventRecord *theEvent,short *itemHit );
	Boolean TimerEventPresent(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);


	};

#endif