#pragma once

#include "mcotypes.h"
#include "mcostat.h"
#include "rawdata.h"
#include "Gammut_comp.h"
#include "thermobject.h"

class CGamutData
{
public:
	GammutComp* gammutcomp;
	RawData* pdata;
	BOOL delete_pdata;
	McoHandle gdata_gH;
	McoHandle gdata_g1H;
	McoHandle gdata_g2H;

public:
	CGamutData();
	~CGamutData();
	McoStatus Init(RawData* pd, ThermObject *therm);
};
