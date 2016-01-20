/*      processtherm.h
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/


#ifndef IN_PROCESSTHERM
#define IN_PROCESSTHERM

#include "mcostat.h"
#include "thermwin.h"
#include "profiledoc.h"

class ProcessWin:public ThermWin {
private:
protected:
public:

	ProcessWin(ProfileDoc *doc,MS_WindowTypes wint,int winn,unsigned char *patchFName);
	~ProcessWin(void);

};

#endif