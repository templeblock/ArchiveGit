/*      gamutsurftherm.h
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/


#ifndef IN_GAMSURFTHERM
#define IN_GAMSURFTHERM

#include "mcostat.h"
#include "thermwin.h"
#include "gammut_comp.h"
#include "profiledoc.h"

class GamutSurfaceWin:public ThermWin {
private:
protected:
	
GammutComp	*gammutcomp;
RawData 	*pdata;
int 		delete_pdata;
public:

	GamutSurfaceWin(ProfileDoc *doc,RawData *pdata,int dr,MS_WindowTypes wint,int winn,unsigned char *patchFName);
	~GamutSurfaceWin(void);

};

#endif