//for swapping the data on different platform
#ifndef SWAB_H
#define SWAB_H

#include <stdio.h>
#include "mcotypes.h"

class Swab{
	private:
	protected:
	
	Boolean _swab;
	
	public:
	Swab();
	~Swab();
	
	void 	setSwab(Boolean swab);
	Boolean getSwab(void);
	void	swabShort(u_int16 *s, u_int32 num);
	void	swabLong(u_int32 *l, u_int32 num);	
};

#endif	//Swab	