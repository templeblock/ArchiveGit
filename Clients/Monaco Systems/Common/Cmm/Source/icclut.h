//the class to handle icc lut tag

#ifndef ICC_LUT_H
#define ICC_LUT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CMICCProfile.h"
#include "mcostat.h"

class Icclut{
private:
protected:


public:
	
	virtual McoStatus	combine(CMLut16Type *luttag, unsigned char *data){ return MCO_FAILURE;}
};

#endif //ICC_LUT_H	