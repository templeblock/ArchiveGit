#ifndef ICC_LUT8_H
#define ICC_LUT8_H

#include "icclut.h"

class Icclut8: public Icclut{
private:
protected:

public:
	Icclut8(){;}
	~Icclut8(){;}
	
	McoStatus combine(CMLut16Type *luttag, unsigned char *data);
};

#endif //ICC_LUT8_H