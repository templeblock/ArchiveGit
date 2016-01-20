#include "stdafx.h"
#include "GamutData.h"

/////////////////////////////////////////////////////////////////////////////
CGamutData::CGamutData()
{
	gdata_gH = NULL;
	gdata_g1H = NULL;
	gdata_g2H = NULL;
	gammutcomp = NULL;
	pdata = NULL;
	delete_pdata = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//P
McoStatus CGamutData::Init(RawData* pd, ThermObject *therm)
{
	McoStatus status;

	if (gdata_g1H)
		McoDeleteHandle(gdata_g1H);
	if (gdata_g2H)
		McoDeleteHandle(gdata_g2H);
	if (gammutcomp)
		delete gammutcomp;

	gdata_gH = NULL;
	gdata_g1H = NULL;
	gdata_g2H = NULL;
	gammutcomp = NULL;
	pdata = NULL;
	delete_pdata = FALSE;

	gdata_g1H = McoNewHandle(sizeof(double)*4*256*360);
	if (!gdata_g1H)
		return MCO_MEM_ALLOC_ERROR;

	gdata_g2H = McoNewHandle(sizeof(double)*4*256*360);
	if (!gdata_g2H)
		return MCO_MEM_ALLOC_ERROR;

	double maxgrayL = 100;
	double mingrayL = 0;

	gammutcomp = new GammutComp(pd, 0L, 0L, 0L, gdata_g1H, gdata_g2H, 0L, 0L, 0L,
		0, 100, 0, 0, maxgrayL, mingrayL, 100, 0, 0, 0L, therm/*thermd*/);
	if (!gammutcomp)
		return MCO_MEM_ALLOC_ERROR;

	status = gammutcomp->get_gamut_surface();
	if(status != MCO_SUCCESS){
		McoDeleteHandle(gdata_g1H);
		gdata_g1H = 0;
		McoDeleteHandle(gdata_g2H);
		gdata_g2H = 0;
		return status;
	}

	if (pd->type != RGBPatches)
	{
		gdata_gH = gdata_g2H;
		McoDeleteHandle(gdata_g1H);
		gdata_g1H = 0;
	}
	else
	{
		gdata_gH = gdata_g1H;
		McoDeleteHandle(gdata_g2H);
		gdata_g2H = 0;
	}

	pdata = pd;
	delete_pdata = TRUE;

	return MCO_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
CGamutData::~CGamutData(void)
{
	if (gammutcomp)
		delete gammutcomp;
//	if ((delete_pdata) && (pdata))
//		delete pdata;
}
