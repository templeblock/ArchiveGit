#ifndef	CMM_CONVERT_H
#define CMM_CONVERT_H

#include "mcostat.h"
#include "csprofile.h"
#include "newinterpolate.h"
#include "iccexpert.h"

class CCmmConvert
{
private:
protected:

COLOR_XFORM_OBJ	_obj;	
McoStatus _LinkGetXform( IccProfileSet *set);

public:
CCmmConvert();
~CCmmConvert();

McoStatus LinkGetXform( char* srcname, char* destname, long renderintent);
void ApplyXform( char* insrc, char* indes, int BitsPerSample, long numpixels);

};

#endif





