// Confidential
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	mcoicc.h
//
//	defines ICC color transforms
// 	makes use of Colorsync
//
//	Create Date:	9/14/96
//	By James Vogh
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_MCOICC
#define IN_MCOICC

#include "mcotypes.h"
#include "mcostat.h"
#include "mcolyout.h"

// The type of transform


//class McoXformIcc : public McoXformLuts {
class McoXformIcc {
private:
protected:
	

public:

	McoXformIcc(void) {}
	~McoXformIcc(void) {}
	

// eval using monaco's bitmap layout
virtual McoStatus eval(McoLayout *in, McoLayout *out) {return MCO_FAILURE;}

// give an array of colors, match the colors using the colorworld
virtual McoStatus evalColors(unsigned char *in, unsigned char *out,int32 num) {return MCO_FAILURE;}

};


#endif