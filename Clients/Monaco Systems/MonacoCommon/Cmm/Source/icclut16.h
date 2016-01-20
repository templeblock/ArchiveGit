#ifndef ICC_LUT16_H
#define ICC_LUT16_H

#include "icclut.h"

class Icclut16: public Icclut{
private:
protected:

public:
	Icclut16(){;}
	~Icclut16(){;}
	
	McoStatus combine(CMLut16Type *luttag, unsigned char *data);
};

#endif //ICC_LUT16_H