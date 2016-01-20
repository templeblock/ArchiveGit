// Confidential
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	mcoiccmac.h
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

#ifndef IN_MCOICCMAC
#define IN_MCOICCMAC

#include "mcoicc.h"
#include "mcotypes.h"
#include "mcostat.h"
//#include "mcoxluts.h"
#include "mcolyout.h"
#include "mcocolsyn.h"
//#include "mcoalljob.h"

// The type of transform

// initial internal format is RGB24

// This struct is the header of the file encoding of a McoXformLuts
typedef struct {
	int32 selColMagic;	// A magic number for verification
	int32 selColVersion;	// File version
} McoICCFileHeader;



#define MCO_ICC_MAGIC (0x53AD)
#define MCO_ICC_VERSION_1 (0)


//class McoXformIcc : public McoXformLuts {
class McoXformIccMac : public McoXformIcc{
private:
protected:
	
	int16		NullXForm;
	int16		validRef;
	CMWorldRef	cw;
	
	int16		in_dataspace;
	int16		out_dataspace;

public:

	McoXformIccMac(void);
	~McoXformIccMac(void);
	
// build a transform given a list of profiles
McoStatus buildCW(McoColorSync **colsynList,int32 numProf);

// eval using monaco's bitmap layout
McoStatus eval(McoLayout *in, McoLayout *out);

// apply the transform to the pixmap
// the pixmap must be locked
McoStatus evalPixMap(PixMap *pixmap);

// give an array of colors, match the colors using the colorworld
McoStatus evalColors(unsigned char *in, unsigned char *out,int32 num);

};


#endif