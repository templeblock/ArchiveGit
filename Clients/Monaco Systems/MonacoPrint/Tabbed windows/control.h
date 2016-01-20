////////////////////////////////////////////////////////////////////////////////
// control.h
//
// window controls such as tables and curves
//
// Jmaes Vogh
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_CONTROL
#define IN_CONTROL

#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"

class control:public baseWin {
private:
protected:


public:

control(WindowPtr wp) { dp = wp;}
~control(void) {dp = 0L;}

virtual McoStatus	DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed,int *didit) {return MCO_SUCCESS;}
virtual McoStatus 	CopyParameters(void) {return MCO_SUCCESS;}
virtual McoStatus 	UpdateControl(void) {return MCO_SUCCESS;}


};

#endif

