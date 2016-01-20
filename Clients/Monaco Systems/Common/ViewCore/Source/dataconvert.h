#ifndef DATA_CONVERT_H
#define DATA_CONVERT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcostat.h"
#include "mcotypes.h"
#include "colorconvert.h"
#include "define.h"


class DataConvert{

private:
protected:
	MonitorData*	_gTheMonitor;
	XYZColorType	tw, r, g, b, k, ak, aw, ar, ag, ab;
	RGBNumber 		rgb, whiteRatio, blackOffset;

public:
	DataConvert();
	DataConvert(MonitorData *gTheMonitor);
	~DataConvert();

	McoStatus reset(MonitorData *gTheMonitor);
	void calcProjection(XYZColorType W, XYZColorType R, XYZColorType G, 
								 XYZColorType B, RGBNumber *projection);
	McoStatus labtorgb(LabColorType *aLabColor, RGBColor *anRGBColor, long *outgamut, long num);
	McoStatus Yxytolab(YxyColorType *aYxyColor, LabColorType *mLabColor, long num);
};


#endif //DATA_CONVERT_H