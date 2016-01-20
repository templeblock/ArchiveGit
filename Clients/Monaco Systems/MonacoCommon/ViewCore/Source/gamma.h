#ifndef GAMMA_H
#define GAMMA_H

#include "mcostat.h"
#include "mcotypes.h"
#include "colorconvert.h"
#include "lookuptable.h"

#include "define.h"

class Gamma{
private:
protected:
	LookUpTable *_redTable;
	LookUpTable *_greenTable;
	LookUpTable *_blueTable;
	short		_calGammaLut[256*3];

	McoStatus _Init(void);

public:
	Gamma(void);
	~Gamma(void);

	void calcProjection(XYZColorType W, XYZColorType R, XYZColorType G, XYZColorType B, RGBNumber *projection);
	McoStatus calcGamma(MonitorData theMonitor, RGBNumber *gamma);
//	McoStatus initGamma(GammaTable *theGammaTable);	
	McoStatus ComputeGamma(MonitorData *theMonitor);
	BOOL	ComputeTable(MonitorData *theMonitor, double min, double max, LookUpTable *lut, short* table);
	short*	GetGammaLut(int* lutSize);
};

#endif //end of gamma