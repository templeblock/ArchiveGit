/*      gamutcomptherm.h
		
		thermometer window for loading a file and displaying the gamut
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/


#ifndef IN_GAMCOMPTHERM
#define IN_GAMCOMPTHERM

#include "mcostat.h"
#include "thermwin.h"
#include "gammut_comp.h"
#include "profiledoc.h"

struct ParamPass {
	GammutComp	*gammutcomp;
	ProfileDoc	*doc;
	double		gamutexpand;
	};

class GamutCompWin:public ThermWin {
private:
protected:
	
GammutComp	*gammutcomp;

public:

	GamutCompWin(ProfileDoc *doc,MS_WindowTypes wint,int winn);
	~GamutCompWin(void);

};

#endif