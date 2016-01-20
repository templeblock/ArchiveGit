/*  gamutwindata.cpp

	store information needed for gamut window
	
	James Vogh
	
*/

#include "gamutwindata.h"

GammutWinData::GammutWinData(void)
{
gH = 0L;
g1H = 0L;
g2H = 0L;
}

GammutWinData::~GammutWinData(void)
{

if (g1H) McoDeleteHandle(g1H);
if (g2H) McoDeleteHandle(g2H);

}
