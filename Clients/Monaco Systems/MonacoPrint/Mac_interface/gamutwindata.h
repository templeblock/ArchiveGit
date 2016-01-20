/*  gamutwindata.h

	store information needed for gamut window
	
	James Vogh
	
*/

#ifndef IN_GAMUTWINDATA
#define IN_GAMUTWINDATA

#include "mcomem.h"

class GammutWinData {
private:
protected:
public:
	McoHandle gH;
	McoHandle g1H;
	McoHandle g2H;
	int		  mine;
	Str255 fn;
	
	GammutWinData(void);
	~GammutWinData(void);	
	
};	

#endif