#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CmmConvert.h"


CCmmConvert::CCmmConvert()
{
	memset((char*)&_obj, 0, sizeof(COLOR_XFORM_OBJ));
}

CCmmConvert::~CCmmConvert()
{
	if(_obj.Interpolator)
	{
		delete _obj.Interpolator;
		_obj.Interpolator = 0;
	}
	if(_obj.Data)
	{
		delete _obj.Data;
		_obj.Data = 0;
	}
}

//src is the source ICC profile
//des is the destination ICC profile
//structure COLOR_XFORM_OBJ contains the linked profile data

//if return NULL, something wrong happened.
//source ICC profile can be monitor, scanner, printer profile
//destination ICC profile can be monitor, scanner, printer profile
McoStatus CCmmConvert::LinkGetXform( char* srcname, char* destname, long renderintent)
{
	McoStatus 	status;
	CsFormat	src, dest;
	COLOR_XFORM_OBJ		*iccobj;

	if( (status = src.createFile(srcname, _O_BINARY | _O_RDONLY)) != MCO_SUCCESS)
		return status;

	if( (status = dest.createFile(destname, _O_BINARY | _O_RDONLY)) != MCO_SUCCESS)
	{
		src.closeFile();
		return status;
	}

//build expert
	IccProfileSet	set;

	set.keyIndex = 0;
	set.count = 2;
	set.intent = renderintent;
	set.profileSet[0] = &src;
	set.profileSet[1] = &dest;

	status = _LinkGetXform( &set);

	src.closeFile();
	dest.closeFile();
	return status;
}

McoStatus CCmmConvert::_LinkGetXform( IccProfileSet *set)
{
	McoStatus status;
	IccExpert expert;

	status = expert.buildexp(set);
	if(status != MCO_SUCCESS)	return status;	

	expert.getInfo(&(_obj.Info.info));
	
	unsigned char *data;
	data = new unsigned char[_obj.Info.info.dclutsize];
	
	if(!data)	return MCO_MEM_ALLOC_ERROR;
	
	expert.getTable(data);
	
	_obj.Data = data;
	_obj.Interpolator = 0;
	return MCO_SUCCESS;
}

void CCmmConvert::ApplyXform( char* insrc, char* indes, int BitsPerSample, long numpixels)
{		
	unsigned char *src, *des;
	unsigned char *data;
	long incomp, outcomp, grids, bits;
	short i;
	NLinterp	*interp;
	
	src = (unsigned char *)insrc;
	des = (unsigned char *)indes;

	if(!_obj.Interpolator){
		data = (unsigned char *)_obj.Data;
		if(_obj.Info.info.devtocon){
			incomp = _obj.Info.info.dcinputChannels;
			outcomp = _obj.Info.info.dcoutputChannels;
			grids = _obj.Info.info.dcgridPoints;
			bits = _obj.Info.info.dcbits;
		}	
		else{
			incomp = _obj.Info.info.cdinputChannels;
			outcomp = _obj.Info.info.cdoutputChannels;
			grids = _obj.Info.info.cdgridPoints;
			bits = _obj.Info.info.cdbits;
		}	
	
		switch(incomp){
			case(3):
			if(bits == 8)
				interp = new NLinterp3(data, outcomp, grids, incomp);
			else // 16 bits
				interp = new NLinterp3s((unsigned short*)data, outcomp, grids, incomp);
			_obj.Interpolator = interp;
			break;
			
			case(4):
			if(bits == 8)
				interp = new NLinterp4(data, outcomp, grids, incomp);
			else // 16 bits
				interp = new NLinterp4s((unsigned short*)data, outcomp, grids, incomp);
			_obj.Interpolator = interp;
			break;	
		}
	
	}	
	
	interp = (NLinterp*)_obj.Interpolator;
	interp->interp(src, numpixels, des);	
}
