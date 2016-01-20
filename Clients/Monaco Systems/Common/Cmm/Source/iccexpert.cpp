#include <math.h>
#include "iccexpert.h"
#include "iccindex.h"
#include "NewInterpolate.h"
#include "colortran.h"
#include "matrix.h"
//#include "fortestonly.h"
#include "icclut8.h"
#include "icclut16.h"


static const ONE_D_ENTRIES = 17;
static const THREE_D_ENTRIES = ONE_D_ENTRIES*ONE_D_ENTRIES*ONE_D_ENTRIES;
static const FOUR_D_ENTRIES = THREE_D_ENTRIES*ONE_D_ENTRIES;

//the conversion space is LAB

IccExpert::IccExpert(void)
{
	_grids = 0;
	_in_comp = 0;
	_out_comp = 0;
	_level = 0;
	_in_data = 0;
	_out_data = 0;
	_tableH = 0;
}

IccExpert::~IccExpert(void)
{
	if(_tableH){
		McoDeleteHandle(_tableH);
		_tableH = 0;
	}
}		

//check connection to device tags
long IccExpert::_check_connect_to_dev_tags(CsFormat *prof, CM2Header *header)
{
	long exist = 0, ok = 0;
	
	switch(header->profileConnectionSpace)
	{
		case(cmXYZData):
		switch(header->profileClass)
		{
			case(cmInputClass):
			case(cmDisplayClass):
			case(cmOutputClass):
			exist = prof->checkTagExist(cmGrayTRCTag);
			exist += prof->checkTagExist(cmRedColorantTag);
			exist += prof->checkTagExist(cmGreenColorantTag);
			exist += prof->checkTagExist(cmBlueColorantTag);
			if(exist)	ok = 1;
			break;
			
			case(cmLinkClass):
			case(cmAbstractClass):
			case(cmColorSpaceClass):
			exist = prof->checkTagExist(cmBToA0Tag);
			if(exist)	ok = 1;
			break;
			
			default:
			break;
		}
		break;	
			
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYKData):
		case(cmCMYData):
		exist = prof->checkTagExist(cmBToA0Tag);
		if(exist) 	ok = 1;
		break;
		
		default:
		break;
	}

	return ok;
}	

//finisihed
//space is prvious profile's connection space
long IccExpert::_check_connect_to_dev(CsFormat *prof, long pspace, CM2Header *header)
{
	long	exist = 0;
	long	ok = 0;
	
	switch(pspace)
	{
		case(cmXYZData):
		switch(header->profileConnectionSpace)
		{
			//need some refinement
			case(cmXYZData):
			case(cmLabData):
			ok = _check_connect_to_dev_tags(prof, header);
			break;
			
			//not compatible
			case(cmRGBData):
			case(cmCMYKData):
			case(cmCMYData):
			default:
			break;
		}
		break;
		
		case(cmLabData):
		switch(header->profileConnectionSpace)
		{
			//need some refinement
			case(cmXYZData):
			case(cmLabData):
			ok = _check_connect_to_dev_tags(prof, header);
			break;
			
			//not compatible
			case(cmRGBData):
			case(cmCMYKData):
			case(cmCMYData):
			default:
			break;
		}
		break;
		
		case(cmRGBData):
		//note: for link profile, this could be different
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmCMYKData):
			case(cmCMYData):
			break;
			
			//may need refinement 
			case(cmRGBData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;			
			
			default:
			break;
		}
		break;
		
/*			
		switch(header->profileConnectionSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmCMYKData):
			case(cmCMYData):
			break;
			
			//may need refinement 
			case(cmRGBData):
			ok = _check_connect_to_dev_tags(prof, &header);
			break;			
			
			default:
			break;
		}
		break;
*/		
		
		case(cmCMYKData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmRGBData):
			case(cmCMYData):
			break;
			
			//may need refinement 
			case(cmCMYKData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;			
			
			default:
			break;
		}
		break;
	
		case(cmCMYData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmRGBData):
			case(cmCMYKData):
			break;
			
			//may need refinement 
			case(cmCMYData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;			
			
			default:
			break;
		}
		break;
		
		//others are not supported yet
		default:
		break;
	}
	
	return ok;
}		

//check device to connect tags
long IccExpert::_check_dev_to_connect_tags(CsFormat *prof, CM2Header *header)
{
	long exist = 0, ok = 0;
	
	switch(header->profileConnectionSpace)
	{
		case(cmXYZData):
		switch(header->profileClass)
		{
			case(cmInputClass):
			case(cmDisplayClass):
			exist = prof->checkTagExist(cmGrayTRCTag);
			exist += prof->checkTagExist(cmRedColorantTag);
			exist += prof->checkTagExist(cmGreenColorantTag);
			exist += prof->checkTagExist(cmBlueColorantTag);
			if(exist)	ok = 1;
			break;
			
			case(cmOutputClass):
			case(cmLinkClass):
			case(cmAbstractClass):
			case(cmColorSpaceClass):
			exist = prof->checkTagExist(cmAToB0Tag);
			if(exist)	ok = 1;
			break;
			
			default:
			break;
		}
		break;	
			
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYKData):
		case(cmCMYData):
		exist = prof->checkTagExist(cmAToB0Tag);
		if(exist) 	ok = 1;
		break;
		
		default:
		break;
	}

	return ok;
}	
	
//finisihed
//space is the previous profile's device space
long IccExpert::_check_dev_to_connect(CsFormat *prof, long pspace, CM2Header *header)
{
	long	exist = 0;
	long	ok = 0;
	
	switch(pspace)
	{
		case(cmXYZData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):			
			case(cmLabData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;
			
			//not compatible
			case(cmRGBData):
			case(cmCMYKData):
			case(cmCMYData):
			default:
			break;
		}
		break;
		
		case(cmLabData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):
			case(cmLabData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;
			
			//not compatible
			case(cmRGBData):
			case(cmCMYKData):
			case(cmCMYData):
			default:
			break;
		}
		break;
		
		case(cmRGBData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmCMYKData):
			case(cmCMYData):
			break;
			
			//may need refinement 
			case(cmRGBData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;			
			
			default:
			break;
		}
		break;
		
		case(cmCMYKData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmRGBData):
			case(cmCMYData):
			break;
			
			//may need refinement 
			case(cmCMYKData):
			ok = _check_dev_to_connect_tags(prof, header);
			break;			
			
			default:
			break;
		}
		break;
	
		case(cmCMYData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			case(cmRGBData):
			case(cmCMYKData):
			break;
			
			//may need refinement 
			case(cmCMYData):
			ok = _check_dev_to_connect_tags(prof, header);
			if(exist) 	ok = 1;
			break;			
			
			default:
			break;
		}
		break;
		
		//others are not supported yet
		default:
		break;
	}
	
	return ok;
}		

//not support link and abstract profile yet
McoStatus IccExpert::_checklink(IccProfileSet *profset)
{
	McoStatus 	status;
	long i;
	CsFormat	*prof;
	CM2Header	header;
	long 		tagcount;
	long		prev, curr;	//previous and current output format
	long		check_dev_to_con;	//what kind of tag we should check
	long		valid;
	
	//if there is only one Icc file return error
	if(profset->count <= 1)
		return MCO_ICC_ERROR;
		
	//check if these profiles can be linked together
	for(i = 0; i < profset->count; i++)
	{
		prof = profset->profileSet[i];
		
		//check to make sure the profile is valid
		//status = prof->openFile();
		valid = prof->validProfile();			
		if(!valid)	return 	MCO_NOT_VALID_ICC;
		prof->getHeader(&header, &tagcount);
		
		//need a tag for dev -> connect
		if(i == 0){
			prev = header.dataColorSpace;
			valid = _check_dev_to_connect(prof, prev, &header);			
			if(!valid)	return 	MCO_ICC_MISS_TAG;
			
			//what next we should check
			prev = header.profileConnectionSpace;
		}
		//need a tag for connect -> dev
		else if( i == profset->count-1){
			valid = _check_connect_to_dev(prof, prev, &header);			
			if(!valid)	return 	MCO_ICC_MISS_TAG;
		}
		//need tags for connect -> dev and dev -> connect
		else{
			//check connect -> dev space
			valid = _check_connect_to_dev(prof, prev, &header);			
			if(!valid)	return 	MCO_ICC_MISS_TAG;
			prev = header.dataColorSpace;
				
			valid = _check_dev_to_connect(prof, prev, &header);			
			if(!valid)	return 	MCO_ICC_MISS_TAG;
			prev = header.profileConnectionSpace;
		}			
	}

	return MCO_SUCCESS;
}


//build expert based on input profile list
McoStatus IccExpert::buildexp(IccProfileSet *profset)
{
	McoStatus status;

// 	keyIndex is ignored
//	unsigned short 		keyIndex;		//Zero-based

	status = _checklink(profset);
	if(status != MCO_SUCCESS)	return status;
	
	
	//change data to a lut, so it is easier to do interpolation
	status = _buildlink(profset);
	
	return status;
}

/*
//load the lut tag
//disable for testing 
McoStatus IccExpert::_load_lut_tag(CsFormat *prof, CM2Header *header, long tag, McoHandle* h)
{
	McoStatus status;
	CMLut16Type	luttag;
	
	//get lut info
	status = prof->getLutTagdatainfo(tag, &luttag);
	if(status != MCO_SUCCESS)	return status;
	
	//compute the lut size	
	//change has been made here to get not only the
	//lut but also the input and output table
	//the sequence of the tables is not changed
	//it should be input, lut , output
	
	long datasize, i;
	long lutsize, lutentry, insize, outsize;
	
	if( luttag.typeDescriptor == cmSigLut16Type){
		lutentry = 1; 
		
		//lut
		for(i = 0; i < luttag.inputChannels; i++)
			lutentry *= luttag.gridPoints;
		lutentry *= luttag.outputChannels;
		lutsize = lutentry*2;			
		datasize = lutsize;
		
		//input
		insize = luttag.inputTableEntries*2;
		datasize += luttag.inputChannels*insize;
		
		//output
		outsize = luttag.outputTableEntries*2;
		datasize += luttag.outputChannels*outsize;
	}
	else if( luttag.typeDescriptor == cmSigLut8Type){	
		lutentry = 1; 
		
		//lut
		for(i = 0; i < luttag.inputChannels; i++)
			lutentry *= luttag.gridPoints;
		lutentry *= luttag.outputChannels;	
		lutsize = lutentry;			
		datasize = lutsize;
		
		//input
		insize = 256;
		datasize += luttag.inputChannels*insize;
		
		//output
		outsize = 256;
		datasize += luttag.outputChannels*outsize;		
	}
	else
		return MCO_FAILURE; 

	McoHandle	srctH;
	unsigned char* 	srct;
	
	//allocate the memory for lut	
	srctH = McoNewHandle(datasize);
	if(!srctH)	return MCO_MEM_ALLOC_ERROR;
	srct = (unsigned char*)McoLockHandle(srctH);

	status = prof->getLutTagdata(tag, srct);
	if(status != MCO_SUCCESS)	return status;
	
	//check whether the matrix is identity matrix
	long iden = 0, equal = 0;
	long *mat, *imat;

	//mat = &luttag.matrix;
	for(i = 0; i < 9; i++)
		if( *mat++ == *imat++)
			equal += 1;	
	
	if(equal == 9)	iden = 1;	

	//now only support identity matrix
	McoHandle tH;
	
	tH = McoNewHandle(lutsize);
	unsigned char *tt, *ttemp;
	
	tt = (unsigned char*)McoLockHandle(tH);
	
	//create a new table
	if(luttag.typeDescriptor == cmSigLut8Type)	
		Iccindex index(tt, luttag.gridPoints, luttag.inputChannels, 
			luttag.outputChannels);
	else
		Iccindex index((unsigned short*)tt, luttag.gridPoints, luttag.inputChannels,
			luttag.outputChannels);

	//not support uniden matrix
	if(!iden)	return MCO_FAILURE;

	//input interpolation
	ttemp = tt;
	for(i = 0; i < luttag.inputChannels; i++){
		Linterp1 inter1(srct+insize*i, 1, luttag.gridPoints, ttemp++, 
			luttag.inputChannels-1, lutentry, tt, luttag.inputChannels-1);
	}
	
	//lut interpolation
	srct += luttag.inputChannels*insize;
	switch(luttag.inputChannels){
		case(1):
		Linterp1 inter1(srct, luttag.outputChannels, luttag.gridPoints, tt,
			(luttag.outputChannels - luttag.inputChannels), lutentry, tt);
			
		case(3):
		Linterp3 inter3(srct, luttag.outputChannels, luttag.gridPoints, tt,
			(luttag.outputChannels - luttag.inputChannels), lutentry, tt);

		case(4):
		Linterp4 inter4(srct, luttag.outputChannels, luttag.gridPoints, tt,
			(luttag.outputChannels - luttag.inputChannels), lutentry, tt);
			
		default:
		return MCO_FAILURE;
	}
	
	//input interpolation
	srct += lutsize;
	ttemp = tt;
	for(i = 0; i < luttag.outputChannels; i++){
		Linterp1 inter1(srct+outsize*i, 1, luttag.gridPoints, ttemp++, 
			luttag.outputChannels-1, lutentry, tt, luttag.outputChannels-1);
	}
			
	return MCO_SUCCESS;
}
*/

/*
//load device -> LAB or LAB -> device table
McoStatus IccExpert::_load_lut_tag(CsFormat *prof, CM2Header *header, long tag, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status;
	long 		tagcount;
	long		valid;
	CMLut16Type	luttag;
	long		i;

	//status = prof->getPartialTagdata(tag, 0, sizeof(CMLut16Type), (char*)&luttag);
	status = prof->getLutTagdatainfo(tag, &luttag);
	if(status != MCO_SUCCESS) return status;

	//compute the lut size	
	long datasize;
	if( luttag.typeDescriptor == cmSigLut16Type){
		datasize = 1; 
		for(i = 0; i < luttag.inputChannels; i++)
			datasize *= luttag.gridPoints;			
		datasize *= luttag.outputChannels*2;
	}
	else if( luttag.typeDescriptor == cmSigLut8Type){	
		datasize = 1; 
		for(i = 0; i < luttag.inputChannels; i++)
			datasize *= luttag.gridPoints;			
		datasize *= luttag.outputChannels;
	}
	else
		return MCO_FAILURE; 

	McoHandle srcH;
	unsigned char *src;
	srcH = McoNewHandle(datasize);
	if(!srcH)	return MCO_MEM_ALLOC_ERROR;
	src = (unsigned char*)McoLockHandle(srcH);
	
	//get source data
	status = prof->getLutTagdata(tag, src);	
	McoUnlockHandle(srcH);
	*h = srcH;

		
	lut->typeDescriptor = luttag.typeDescriptor;
	lut->inputChannels = luttag.inputChannels;	
	lut->outputChannels = luttag.outputChannels;
	lut->gridPoints = luttag.gridPoints;
	
	return status;
}
*/

//load device -> LAB or LAB -> device table
McoStatus IccExpert::_load_lut_tag(CsFormat *prof, CM2Header *header, long tag, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status;
	long 		tagcount;
	long		valid;
	CMLut16Type	luttag;
	long		i, j;
	long 		datasize = 1;

	//status = prof->getPartialTagdata(tag, 0, sizeof(CMLut16Type), (char*)&luttag);
	status = prof->getLutTagdatainfo(tag, &luttag);
	if(status != MCO_SUCCESS) return status;

	//compute the lut size	
	if( luttag.typeDescriptor == cmSigLut16Type){
		for(i = 0; i < luttag.inputChannels; i++)
			datasize *= luttag.gridPoints;			
		datasize *= luttag.outputChannels*2;
		datasize += luttag.inputChannels*luttag.inputTableEntries*2;
		datasize += luttag.outputChannels*luttag.outputTableEntries*2;
	}
	else if( luttag.typeDescriptor == cmSigLut8Type){	
		for(i = 0; i < luttag.inputChannels; i++)
			datasize *= luttag.gridPoints;			
		datasize *= luttag.outputChannels;
		datasize += luttag.inputChannels*256;
		datasize += luttag.outputChannels*256;
	}
	else
		return MCO_FAILURE; 


	McoHandle srcH;
	unsigned char *src;
	srcH = McoNewHandle(datasize);
	if(!srcH)	return MCO_MEM_ALLOC_ERROR;
	src = (unsigned char*)McoLockHandle(srcH);
	
	//get source data
	status = prof->getLutTagdata(tag, src);	
	
	//combine the input, lut & output table together
	if(status != MCO_SUCCESS){
		McoUnlockHandle(srcH);
		McoDeleteHandle(srcH);
		return MCO_FAILURE;
	}

	lut->typeDescriptor = luttag.typeDescriptor;
	lut->inputChannels = luttag.inputChannels;	
	lut->outputChannels = luttag.outputChannels;
	lut->gridPoints = luttag.gridPoints;
		
	if( luttag.typeDescriptor == cmSigLut16Type){
		Icclut16 lut16;
		long pixelsize;
		unsigned short *temp;
		status = lut16.combine(&luttag, src);	
		//convert to 8 bits
		datasize -= luttag.inputChannels*luttag.inputTableEntries*2;
		datasize -= luttag.outputChannels*luttag.outputTableEntries*2;
		datasize /= 2;
		
		temp = (unsigned short *)src;
		for(i = 0; i < datasize; i++)
			src[i] = (unsigned char)(temp[i] >> 8);
			
		datasize /= luttag.outputChannels;
	}
	else if( luttag.typeDescriptor == cmSigLut8Type){
		Icclut8 lut8;
		datasize -= luttag.inputChannels*256;
		datasize -= luttag.outputChannels*256;
		datasize /= luttag.outputChannels;
		status = lut8.combine(&luttag, src);	
	}
	else
		return MCO_FAILURE; 
		
	McoUnlockHandle(srcH);
	*h = srcH;

//begin of a test
	//saveLABTiff(srcH, 64, 8);
	//saveLABTiff(srcH, 33*33, 33);
//end of a test

	return status;
}

//create a gamma table
void IccExpert::_load_gamma(CMCurveType *ch, unsigned short *curv, double *gammat)
{
	long i;
	double gamma;
	unsigned char gindex[256];

	if(ch->countValue == 0){
		for(i = 0; i < 256; i++)
			gammat[i] = (double)i;
	}
	else if(ch->countValue == 1){
		gamma = (double)curv[0]/256.0;
		for( i = 0; i < 256; i++)
			gammat[i] = pow((double)i/255.0, gamma);
	}
	else{	
		for(i = 0; i < 256; i++)
			gindex[i] = i;
		Linterp1 inter1(curv, 1, ch->countValue, gindex,
			1, 256, gammat, 1);
	}		

}

void IccExpert::_load_rev_gamma(CMCurveType *ch, unsigned short *curv, double *gammat)
{
	long i;
	double gamma;
	unsigned char gindex[256];

	if(ch->countValue == 0){
		for(i = 0; i < 256; i++)
			gammat[i] = (double)i;
	}
	else if(ch->countValue == 1){
		gamma = 256.0/(double)curv[0];
		for( i = 0; i < 256; i++)
			gammat[i] = pow((double)i/255.0, gamma);
	}
	else{	
		for(i = 0; i < 256; i++)
			gindex[i] = i;
		Linterp1r inter1r(curv, 1, ch->countValue, gindex,
			1, 256, gammat, 1);
	}		

}

//load the XYZ tag
//has not been tested on grayscale tags
McoStatus IccExpert::_load_XYZ_tag(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status;
	double monitor[12];
	double xyztorgb[9];
	double rgbtoxyz[9];
	
	//now, read in the xyz data
	CMXYZType	rxyz, gxyz, bxyz, white;
	double		whitex, whitey, whitez, gamma;
	
	status = prof->getTagdata(cmRedColorantTag, (unsigned char*)&rxyz);	
	if(status != MCO_SUCCESS) return status;
	status = prof->getTagdata(cmGreenColorantTag, (unsigned char*)&gxyz);	
	if(status != MCO_SUCCESS) return status;
	status = prof->getTagdata(cmBlueColorantTag, (unsigned char*)&bxyz);	
	if(status != MCO_SUCCESS) return status;
	status = prof->getTagdata(cmMediaWhitePointTag, (unsigned char*)&white);	
	if(status != MCO_SUCCESS) return status;
	
	//read in the input curve
	CMCurveType rch, gch, bch;
	long	rtagsize, gtagsize, btagsize;
	unsigned short rcurv[256], gcurv[256], bcurv[256];
	double		rf[256], gf[256], bf[256];
	double 		x[256], step;	
	long		i;
	
	//check tag size, and load tag data
	rtagsize = prof->getTagsize(cmRedTRCTag);
	status = prof->getCurveTypeinfo(cmRedTRCTag, &rch);
	if(status != MCO_SUCCESS) return status;
	status = prof->getcurveTagdata(cmRedTRCTag, (char*)&rcurv);
	if(status != MCO_SUCCESS) return status;

	gtagsize = prof->getTagsize(cmGreenTRCTag);
	status = prof->getCurveTypeinfo(cmGreenTRCTag, &gch);
	if(status != MCO_SUCCESS) return status;
	status = prof->getcurveTagdata(cmGreenTRCTag, (char*)&gcurv);
	if(status != MCO_SUCCESS) return status;
	
	btagsize = prof->getTagsize(cmBlueTRCTag);
	status = prof->getCurveTypeinfo(cmBlueTRCTag, &bch);
	if(status != MCO_SUCCESS) return status;
	status = prof->getcurveTagdata(cmBlueTRCTag, (char*)&bcurv);
	if(status != MCO_SUCCESS) return status;

//following needs to be modified
	//fitting to a gamma curve
/*	
	if(rch.countValue == 0)
		gamma = 1.0;
	if(rch.countValue == 1)
		gamma = (double)rcurv[0]/256.0;
	else{	
	//following may need to change	
		gamma = 0.0;
		step = 1.0/(double)(rch.countValue - 1);
		for(i = 1; i < rch.countValue - 1; i++){
			rf[i] = log( (double)(rcurv[i])/65536.0 );
			x[i] = log(i*step);
		}	
		
		long halfcount;
		halfcount = rch.countValue/2 - 1;
		for(i = 1; i < halfcount - 1; i++)
			gamma += (rf[i+halfcount] - rf[i])/(x[i+halfcount] - x[i]);		
		gamma /= (double)halfcount;	 
	}	
*/	
//above needs to be modified

//get white point and phosphor values		
	whitex = (double)(white.XYZ[0].X)/65536.0; 
	whitey = (double)(white.XYZ[0].Y)/65536.0;
	whitez = (double)(white.XYZ[0].Z)/65536.0;

	rgbtoxyz[0] = (double)(rxyz.XYZ[0].X)/65536.0;
	rgbtoxyz[1] = (double)(gxyz.XYZ[0].X)/65536.0;
	rgbtoxyz[2] = (double)(bxyz.XYZ[0].X)/65536.0;
	rgbtoxyz[3] = (double)(rxyz.XYZ[0].Y)/65536.0;
	rgbtoxyz[4] = (double)(gxyz.XYZ[0].Y)/65536.0;
	rgbtoxyz[5] = (double)(bxyz.XYZ[0].Y)/65536.0;
	rgbtoxyz[6] = (double)(rxyz.XYZ[0].Z)/65536.0;
	rgbtoxyz[7] = (double)(gxyz.XYZ[0].Z)/65536.0;
	rgbtoxyz[8] = (double)(bxyz.XYZ[0].Z)/65536.0;

	//convert xyz to nxyz
	for(i = 0; i < 3; i++){
		rgbtoxyz[i] /= 0.9642;
		rgbtoxyz[i+6] /= 0.8249;
	}
			
//init rgb table
	McoHandle	srcH, idxtableH;
	double 		*idxtable;
	unsigned	char	*src;
	long j;

	//memory for real table
	srcH = McoNewHandle(THREE_D_ENTRIES*sizeof(unsigned char)*3);
	if(!srcH)	return MCO_MEM_ALLOC_ERROR;
	src = (unsigned char*)McoLockHandle(srcH);
	
	//memory for process table
	idxtableH = McoNewHandle(THREE_D_ENTRIES*sizeof(double)*3);
	if(!idxtableH)	return MCO_MEM_ALLOC_ERROR;
	idxtable = (double*)McoLockHandle(idxtableH);

	//create gamma curve table
	double rgammat[256], ggammat[256], bgammat[256];

	//device to connection space
	if(dev_to_con){
		
		_load_gamma(&rch, rcurv, rgammat);
		_load_gamma(&gch, gcurv, ggammat);
		_load_gamma(&bch, bcurv, bgammat);
		
		Iccindex index(RGB_INDEX, idxtable, ONE_D_ENTRIES, 3);

		//convert rgb to LAB 	
		//rgbtonxyzover(rgbtoxyz, gamma, idxtable, THREE_D_ENTRIES);	
		rgbtonxyzover2(rgbtoxyz, rgammat, ggammat, bgammat, idxtable, THREE_D_ENTRIES);
		nxyztoLabinplace(idxtable, THREE_D_ENTRIES);
		
	//pack to icc data format
		for(i = 0; i < THREE_D_ENTRIES; i++){
			j = i*3;
			src[j] = (unsigned char)(idxtable[j]*2.55);
			src[j+1] = (unsigned char)(idxtable[j+1]+128.0+0.5);
			src[j+2] = (unsigned char)(idxtable[j+2]+128.0+0.5);
		}
		
		McoUnlockHandle(srcH);
		*h = srcH;
		
		McoUnlockHandle(idxtableH);
		McoDeleteHandle(idxtableH);
	}
	//connection to device space
	else{
		
		_load_rev_gamma(&rch, rcurv, rgammat);
		_load_rev_gamma(&gch, gcurv, ggammat);
		_load_rev_gamma(&bch, bcurv, bgammat);

		//compute the reverse matrix
		Matrix	xyztorgbmat(3);				
		xyztorgbmat.loadstruct(rgbtoxyz);
		xyztorgbmat.inv();
		status = xyztorgbmat.get_status();
		if(status != MCO_SUCCESS){
			McoUnlockHandle(idxtableH);
			McoDeleteHandle(idxtableH);
			McoUnlockHandle(srcH);
			McoDeleteHandle(srcH);
			return status;
		}	
		xyztorgbmat.savestruct(rgbtoxyz);
		
		//change to non nxyz
		//nxyztoxyz(rgb, THREE_D_ENTRIES);


		Iccindex index(LAB_INDEX, idxtable, ONE_D_ENTRIES, 3);

//begin of a test
/*
		McoHandle testH;
		char*		test;
		
		testH = McoNewHandle(THREE_D_ENTRIES*3);
		test = (char*)McoLockHandle(testH);
		for(i = 0; i < THREE_D_ENTRIES; i++){
			j = i*3;
			test[j] = (char)(idxtable[j]*2.55 + 0.5);
			test[j+1] = (char)(idxtable[j+1]);
			test[j+2] = (char)(idxtable[j+2]);
		}
		
		McoUnlockHandle(testH);
		
		saveLABTiff(testH, 289, 17);
		McoDeleteHandle(testH);
*/
//end of a test

		//convert LAB to XYZ
		labtonxyzinplace(idxtable, THREE_D_ENTRIES);

		//change to rgb space
		//nxyztorgb(rgbtoxyz, gamma, idxtable, src, THREE_D_ENTRIES);
		nxyztorgb2(rgbtoxyz, rgammat, ggammat, bgammat, idxtable, src, THREE_D_ENTRIES);
		
		McoUnlockHandle(srcH);
		*h = srcH;

//begin of a test
/*
		unsigned char *test2;
		
		test2 = (unsigned char*)McoLockHandle(srcH);
		status = _testRGB(test2, 289, 17);
		if(status != MCO_SUCCESS)	return status;
		
		McoUnlockHandle(srcH);
*/		
//end of the test	

		McoUnlockHandle(idxtableH);
		McoDeleteHandle(idxtableH);
			
	}
	
	lut->typeDescriptor = cmSigLut8Type;
	lut->inputChannels = 3;	
	lut->outputChannels = 3;
	lut->gridPoints = 17;
		
	return MCO_SUCCESS;
}


//set dev_to_con if the device to connection tag is needed
McoStatus IccExpert::_load_XYZ_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_LAB_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_RGB_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	long	exist;
	
	if(dev_to_con){
		exist = prof->checkTagExist(cmAToB0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}		
	else{
		exist = prof->checkTagExist(cmBToA0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}
	
	return status;
}

McoStatus IccExpert::_load_CMYK_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	long	exist;
	
	if(dev_to_con){
		exist = prof->checkTagExist(cmAToB0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}		
	else{
		exist = prof->checkTagExist(cmBToA0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}
	
	return status;
}

McoStatus IccExpert::_load_CMY_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	long	exist;
	
	if(dev_to_con){
		exist = prof->checkTagExist(cmAToB0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}		
	else{
		exist = prof->checkTagExist(cmBToA0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}
	
	return status;
}

McoStatus IccExpert::_load_XYZ_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
	
	//change the XYZ to LAB
	
	
	return status;
}

McoStatus IccExpert::_load_LAB_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		
	return status;
}

McoStatus IccExpert::_load_RGB_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		
	return status;
}

McoStatus IccExpert::_load_CMYK_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	return _load_RGB_to_LAB(prof, header, dev_to_con, h,  lut);
}
	
McoStatus IccExpert::_load_CMY_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	return _load_RGB_to_LAB(prof, header, dev_to_con, h,  lut);
}

McoStatus IccExpert::_load_link_tag(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	return _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);;
}


McoStatus IccExpert::_load_XYZ_to_RGB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	long exist;
	
	if(dev_to_con){
		exist = prof->checkTagExist(cmAToB0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}		
	else{
		exist = prof->checkTagExist(cmBToA0Tag);
		if(exist)
			status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);
		else	
			status = _load_XYZ_tag(prof, header, dev_to_con, h,  lut);
	}
	
	return status;
}

McoStatus IccExpert::_load_XYZ_to_CMYK(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_XYZ_to_CMY(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_LAB_to_RGB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_LAB_to_CMYK(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

McoStatus IccExpert::_load_LAB_to_CMY(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut)
{
	McoStatus status = MCO_SUCCESS;
	
	if(dev_to_con)
		status = _load_lut_tag(prof, header, cmAToB0Tag, h,  lut);
	else
		status = _load_lut_tag(prof, header, cmBToA0Tag, h,  lut);

	//create lab table here
	
	return status;
}

//load device to connect tags
McoStatus IccExpert::_load_dev_to_connect(CsFormat *prof, CM2Header *header, McoHandle* h, CMLut16Type* lut)
{
	McoStatus	status = MCO_FAILURE;
	
	switch(header->profileConnectionSpace)
	{
		case(cmXYZData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):	
			status = _load_XYZ_to_XYZ(prof, header, 1, h,  lut);
			break;
					
			case(cmLabData):
			status = _load_LAB_to_XYZ(prof, header, 1, h,  lut);
			break;
			
			//not compatible
			case(cmRGBData):
			status = _load_RGB_to_XYZ(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYKData):
			status = _load_CMYK_to_XYZ(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_CMY_to_XYZ(prof, header, 1, h,  lut);
			break;
			
			default:
			break;
		}
		break;
		
		case(cmLabData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):
			status = _load_XYZ_to_LAB(prof, header, 1, h,  lut);
			break;
			
			case(cmLabData):
			status = _load_LAB_to_LAB(prof, header, 1, h,  lut);
			break;
			
			case(cmRGBData):
			status = _load_RGB_to_LAB(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYKData):
			status = _load_CMYK_to_LAB(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_CMY_to_LAB(prof, header, 1, h,  lut);
			break;
			
			default:
			break;
		}
		break;
		
		case(cmRGBData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;
			
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;
			
			//may need refinement 
			case(cmRGBData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
		
		case(cmCMYKData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;			
			
			case(cmRGBData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;
			
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;			
						
			case(cmCMYData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
	
		case(cmCMYData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;			
			
			case(cmRGBData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;
						
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;			
			
			//may need refinement 
			case(cmCMYData):
			status = _load_link_tag(prof, header, 1, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
		
		//others are not supported yet
		default:
		break;
	}
	
	return status;
}		

//load connect to device tags
McoStatus IccExpert::_load_connect_to_dev(CsFormat *prof, CM2Header *header, McoHandle* h, CMLut16Type* lut)
{
	McoStatus	status = MCO_FAILURE;
	
	switch(header->profileConnectionSpace)
	{
		case(cmXYZData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):	
			status = _load_XYZ_to_XYZ(prof, header, 0, h,  lut);
			break;
					
			case(cmLabData):
			status = _load_XYZ_to_LAB(prof, header, 0, h,  lut);
			break;
			
			//not compatible
			case(cmRGBData):
			status = _load_XYZ_to_RGB(prof, header, 0, h,  lut);
			break;
			
			case(cmCMYKData):
			status = _load_XYZ_to_CMYK(prof, header, 0, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_XYZ_to_CMY(prof, header, 0, h,  lut);
			break;
			
			default:
			break;
		}
		break;
		
		case(cmLabData):
		switch(header->dataColorSpace)
		{
			//need some refinement
			case(cmXYZData):
			status = _load_LAB_to_XYZ(prof, header, 0, h,  lut);
			break;
			
			case(cmLabData):
			status = _load_LAB_to_LAB(prof, header, 0, h,  lut);
			break;
			
			case(cmRGBData):
			status = _load_LAB_to_RGB(prof, header, 0, h,  lut);
			break;
			
			case(cmCMYKData):
			status = _load_LAB_to_CMYK(prof, header, 0, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_LAB_to_CMY(prof, header, 0, h,  lut);
			break;
			
			default:
			break;
		}
		break;
		
		case(cmRGBData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;
			
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;
			
			case(cmCMYData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;
			
			//may need refinement 
			case(cmRGBData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
		
		case(cmCMYKData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;			
			
			case(cmRGBData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;
						
			case(cmCMYData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;			
			
			//may need refinement 
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
	
		case(cmCMYData):
		switch(header->dataColorSpace)
		{
			//not compatible
			case(cmXYZData):
			case(cmLabData):
			status = MCO_NOT_VALID_ICC;
			break;			
			
			case(cmRGBData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;
						
			case(cmCMYKData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;			
			
			//may need refinement 
			case(cmCMYData):
			status = _load_link_tag(prof, header, 0, h,  lut);
			break;			
			
			default:
			break;
		}
		break;
		
		//others are not supported yet
		default:
		break;
	}
	
	return status;
}		


McoStatus IccExpert::_buildlink(IccProfileSet *profset)
{
	McoStatus	status;
	McoHandle 	imageH;
	McoHandle 	curH;
	CM2Header	header;
	long 		tagcount;
	CMLut16Type	prevluttag, curluttag;
	CsFormat  	*prof;
	long	i;
	
	prof = profset->profileSet[0];
	prof->getHeader(&header, &tagcount);
	//set in data
	_in_data = header.dataColorSpace;
	prof = profset->profileSet[profset->count - 1];
	prof->getHeader(&header, &tagcount);
	//set in data
	_out_data = header.dataColorSpace;
	switch(_in_data){
		case(cmGrayData):
			_in_comp = 1;
			break;
			
		case(cmXYZData):
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			_in_comp = 3;
			break;
			
		case(cmCMYKData):
			_in_comp = 4;
			break;
	}

	switch(_out_data){
		case(cmGrayData):
			_out_comp = 1;
			break;
			
		case(cmXYZData):
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			_out_comp = 3;
			break;
			
		case(cmCMYKData):
			_out_comp = 4;
			break;
	}

//find table entry size and table byte size			
	long tableentry, tablesize;
	tableentry = 1;
	for(i = 0; i < _in_comp; i++)	
		tableentry *= ONE_D_ENTRIES;
	tablesize = tableentry*_out_comp;
	
	_tableH = McoNewHandle(tablesize*sizeof(unsigned char));
	if(!_tableH)	return MCO_MEM_ALLOC_ERROR;	

//build table	
	for(i = 0; i < profset->count; i++)
	{
		prof = profset->profileSet[i];
		prof->getHeader(&header, &tagcount);
		
		//need a tag for dev -> connect
		if(i == 0){
			status = _load_dev_to_connect(prof, &header, &imageH, &prevluttag);
			if(status != MCO_SUCCESS)	return status;

			status = _size(&imageH, &prevluttag, 
				header.dataColorSpace, cmLabData);
			if(status != MCO_SUCCESS)	return status;

/*
	//begin of a test	
	
	McoHandle testH;
	long width, length;
	unsigned char	*src, *image;
	
	openRGBTiff(&testH, &width, &length);

	src = (unsigned char*)McoLockHandle(testH);
	image = (unsigned char*)McoLockHandle(imageH);
	
	Linterp3 inter3(image, 3, 33, src,3, width*length, src);	
	
	long j;

	for(i = 0; i < width*length; i++){
		j = i*3;
		src[j+1] += 128;
		src[j+2] += 128;
	}	
	
	saveLABTiff(testH, width, length);
		
			//saveLABTiff(imageH, 289, 17);
	//end of a test
*/

		}
		//need a tag for connect -> dev
		else if( i == profset->count-1){
			status = _load_connect_to_dev(prof, &header, &curH, &prevluttag);
			if(status != MCO_SUCCESS)	return status;
			
			//interpolation
			status = _convert(_tableH, imageH, curH, &prevluttag, 
				_in_data, _out_data);
			if(status != MCO_SUCCESS)	return status;
			
	//begin of a test		
			//saveCMYKTiff(_tableH, 289, 17);
	//end of a test
		}	
		//need tags for connect -> dev and dev -> connect
		else
		{
			McoHandle tempH;
			long tsize, outcomp;
			unsigned char *src, *image, *temp;
						
			status = _load_connect_to_dev(prof, &header, &curH, &prevluttag);
			if(status != MCO_SUCCESS)	return status;

			//interpolation
			//upto now, only support Lab
			
			//need some temp memory to hold the temp table
			tsize = tableentry*prevluttag.outputChannels;
			tempH = McoNewHandle(tsize);
			temp = (unsigned char*)McoLockHandle(tempH);
			
			src = (unsigned char*)McoLockHandle(curH);			
			image = (unsigned char*)McoLockHandle(imageH);
			
			switch(prevluttag.inputChannels){
				case(1):
				{
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp1 inter1(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, prevluttag.outputChannels);
				else		
					Linterp1 inter1((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, prevluttag.outputChannels);		
				}
				break;
				
				case(3):
				{
				long nouse;
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp3 inter3(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);
				else		
					Linterp3 inter3((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, nouse);		
				}
				break;
				
				case(4):
				{
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp4 inter4(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);
				else		
					Linterp4 inter4((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);		
				}
				break;
				
				default:
				break;
			}
			
			McoUnlockHandle(curH);
			McoDeleteHandle(curH);
			McoUnlockHandle(imageH);
			McoDeleteHandle(imageH);
			McoUnlockHandle(tempH);
			imageH = tempH;


	//begin of a test		
			//saveCMYKTiff(imageH, 289, 17);
	//end of a test
						
			status = _load_dev_to_connect(prof, &header, &curH, &prevluttag);
			if(status != MCO_SUCCESS)	return status;
						
			//interpolation
			tsize = tableentry*prevluttag.outputChannels;
			tempH = McoNewHandle(tsize);
			temp = (unsigned char*)McoLockHandle(tempH);
			
			src = (unsigned char*)McoLockHandle(curH);			
			image = (unsigned char*)McoLockHandle(imageH);
			
			switch(prevluttag.inputChannels){
				case(1):
				{
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp1 inter1(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, prevluttag.outputChannels);
				else		
					Linterp1 inter1((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, prevluttag.outputChannels);		
				}
				break;
				
				case(3):
				{
				long nouse;
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp3 inter3(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);
				else		
					Linterp3 inter3((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp, nouse);		
				}
				break;
				
				case(4):
				{
				if(prevluttag.typeDescriptor == cmSigLut8Type)
					Linterp4 inter4(src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);
				else		
					Linterp4 inter4((unsigned short*)src, prevluttag.outputChannels, prevluttag.gridPoints, image,
						prevluttag.inputChannels, tableentry, temp);		
				}
				break;
				
				default:
				break;
			}
				
			McoUnlockHandle(curH);
			McoDeleteHandle(curH);
			McoUnlockHandle(imageH);
			McoDeleteHandle(imageH);
			
			McoUnlockHandle(tempH);
			imageH = tempH;

	//begin of a test		
			//saveLABTiff(imageH, 289, 17);
	//end of a test
				
		}			
	
	}
	
	return status;
}


McoStatus IccExpert::_convert(McoHandle tableH, McoHandle srcH, McoHandle desH, 
	CMLut16Type *luttag, long informat, long outformat)
{
	McoStatus status = MCO_FAILURE;
	unsigned char *src;
	unsigned char* table;
	unsigned char * des;
	long	incomp, inentry;
	
	src = (unsigned char*)McoLockHandle(srcH);
	table	= (unsigned char*)McoLockHandle(tableH);
	des = (unsigned char*)McoLockHandle(desH);

	switch(informat){
		case(cmGrayData):
			incomp = 1;
			inentry = ONE_D_ENTRIES;
			break;
		
		case(cmXYZData):
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			incomp = 3;
			inentry = THREE_D_ENTRIES;
			break;
		
		case(cmCMYKData):
			incomp = 4;
			inentry = FOUR_D_ENTRIES;
			break;
		
	}		
	
	switch(informat){
		case(cmGrayData):
			//not supported yet
			break;
			
		case(cmXYZData):
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
		case(cmCMYKData):
			switch(outformat){
				case(cmGrayData):
				{
				Linterp3 interp3(des, 1, luttag->gridPoints, src, 3, inentry, table);
				}
				status = MCO_SUCCESS;
				break;
				
				case(cmXYZData):
				case(cmLabData):
				case(cmRGBData):
				case(cmCMYData):
				{
				Linterp3 interp3(des, 3, luttag->gridPoints, src, 3, inentry, table);
				}
				status = MCO_SUCCESS;
				break;
				
				case(cmCMYKData):
				{
				Linterp3 interp3(des, 4, luttag->gridPoints, src, 3, inentry, table);
				}
				status = MCO_SUCCESS;
				break;
			}	
			break;
	}			

//begin of a test
/*
	unsigned char *test1;
	
	test1 = src;
	status = _testLAB(test1, 289, 17);
	if(status != MCO_SUCCESS)	return status;
*/			
//end of the test		

	McoUnlockHandle(srcH);
	McoDeleteHandle(srcH);
	srcH = 0;
			
//begin of a test
/*
	unsigned char *test2;
	
	test2 = des;
	status = _testRGB(test2, 289, 17);
	if(status != MCO_SUCCESS)	return status;
*/			
//end of the test		
	
	McoUnlockHandle(desH);
	McoDeleteHandle(desH);
	desH = 0;
	McoUnlockHandle(tableH);

//begin of a test
/*
	unsigned char *test3;
	
	test3 = (unsigned char*)McoLockHandle(tableH);
	status = _testRGB(test3, 289, 17);
	if(status != MCO_SUCCESS)	return status;
	
	McoUnlockHandle(tableH);
*/			
//end of the test		
	
	return status;
}

//convert to a fixed size
McoStatus IccExpert::_size(McoHandle* tableH, CMLut16Type	*luttag, 
	long informat, long outformat)
{
	McoStatus status;
	McoHandle srcH;
	long incomp, outcomp;
	unsigned char *src;

	//check whether sizing is needed
	if(luttag->gridPoints == ONE_D_ENTRIES &&
		luttag->typeDescriptor == cmSigLut8Type )
		return MCO_SUCCESS;
	
	//set original table
	srcH = *tableH;	

	switch(informat){
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			incomp = 3;
			break;
		
		case(cmCMYKData):
			incomp = 4;
			break;
	}
	
	switch(outformat){
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			outcomp = 3;
			break;
		
		case(cmCMYKData):
			outcomp = 4;
			break;
	}

//input image table for interpolation			
	long tablesize = 1;
	long	i;
	
	for(i = 0; i < incomp; i++)
		tablesize *= ONE_D_ENTRIES;
	tablesize *= incomp;
		
	McoHandle tempH;
	unsigned char* temp;

	tempH = McoNewHandle(tablesize);
	if(!tempH)	return MCO_MEM_ALLOC_ERROR;

	temp = (unsigned char*)McoLockHandle(tempH);
	

	switch(informat){
		case(cmLabData):
			{
			Iccindex init(LAB_INDEX, temp, ONE_D_ENTRIES, incomp); 
			}
			break;
			
		case(cmRGBData):
			{
			Iccindex init(RGB_INDEX, temp, ONE_D_ENTRIES, incomp); 
			}
			break;
		
		case(cmCMYData):
			{
			Iccindex init(CMY_INDEX, temp, ONE_D_ENTRIES, incomp); 
			}
			break;
		
		case(cmCMYKData):
			{
			Iccindex init(CMYK_INDEX, temp, ONE_D_ENTRIES, incomp); 
			}
			break;
	}	

//new table
	tablesize = 1;
	for(i = 0; i < incomp; i++)
		tablesize *= ONE_D_ENTRIES;
	tablesize *= outcomp;
		
	McoHandle temp2H;
	unsigned char* temp2;
		
	temp2H = McoNewHandle(tablesize);
	if(!temp2H)	return MCO_MEM_ALLOC_ERROR;
	
	temp2 = (unsigned char*)McoLockHandle(temp2H);
	
	src = (unsigned char*)McoLockHandle(srcH);
	
	switch(informat){
		case(cmLabData):
		case(cmRGBData):
		case(cmCMYData):
			long nouse;
//08-23-97
/*			
			if(luttag->typeDescriptor != cmSigLut8Type)
				Linterp3 interp3((unsigned short*)src, 3, luttag->gridPoints, temp, 
					3, THREE_D_ENTRIES, temp2, nouse);				
			else
*/			
			{	Linterp3 interp3(src, 3, luttag->gridPoints, temp, 
				3, THREE_D_ENTRIES, temp2);	}			
			break;		
		
		case(cmCMYKData):
//08-23-97
/*			
			if(luttag->typeDescriptor != cmSigLut8Type)
				Linterp4 interp4((unsigned short*)src, 3, luttag->gridPoints, temp, 
					4, FOUR_D_ENTRIES, temp2);
			else
*/			
				Linterp4 interp4(src, 3, luttag->gridPoints, temp, 
					4, FOUR_D_ENTRIES, temp2);					
			break;
	}	
		
	McoUnlockHandle(tempH);
	McoDeleteHandle(tempH);
	McoUnlockHandle(srcH);
	McoDeleteHandle(srcH);
	McoUnlockHandle(temp2H);
	*tableH = temp2H;

//begin of a test
/*
	unsigned char *test;
	
	test = (unsigned char*)McoLockHandle(*tableH);
	status = _testLAB(test, 289, 17);
	if(status != MCO_SUCCESS)	return status;
	
	McoUnlockHandle(*tableH);
*/	
//end of the test		
		
	return MCO_SUCCESS;
}

/*
	//format the table to 8 bits and 17by17by17
	if(luttag->gridPoints != ONE_D_ENTRIES ||
		luttag->typeDescriptor != cmSigLut8Type ){
		
		McoHandle tempH;
		unsigned char* temp;
		tempH = McoNewHandle(FOUR_D_ENTRIES*4);
		if(!tempH)	return MCO_MEM_ALLOC_ERROR;
		
		temp = (unsigned char*)McoLockHandle(tempH);
		if(comp == 4)
			Iccindex init(temp, ONE_D_ENTRIES, 4);
		else
			Iccindex init(temp, ONE_D_ENTRIES, 3);

		//begin of a test
		//_testCMYK(temp, 289, 289);
		//end of a test

		McoHandle temp2H;
		unsigned char* temp2;
		temp2H = McoNewHandle(TOTAL_CMYK_LAB_DATA);
		if(!temp2H)	return MCO_MEM_ALLOC_ERROR;
		
		temp2 = (unsigned char*)McoLockHandle(temp2H);
		src = (unsigned char*)McoLockHandle(srcH);
		if(comp == 4)
			Linterp4 interp4(src, 3, luttag.gridPoints, temp, 
				4, FOUR_D_ENTRIES, temp2);
		else
			Linterp3 interp3(src, 3, luttag.gridPoints, temp, 
				3, THREE_D_ENTRIES, temp2);				

		McoUnlockHandle(tempH);
		McoDeleteHandle(tempH);
		McoUnlockHandle(srcH);
		McoDeleteHandle(srcH);
		McoUnlockHandle(temp2H);
		srcH = temp2H;
	}
*/

void IccExpert::_convert_format(unsigned long devspace, unsigned long conspace, ICCinfo *info, long devtocon)
{
	if(devtocon){
		switch(devspace){
		case(cmRGBData):
			info->dcInputFormat = MONACO_FMT_MINISBLACK;
			break;

		case(cmCMYKData):
		case(cmCMYData):
			info->dcInputFormat = MONACO_FMT_MINISWHITE;
			break;

		case(cmLabData):
			info->dcInputFormat = MONACO_FMT_LABMODE;
			break;

		default:
			info->dcInputFormat = MONACO_FMT_NUMSAMPLEONLY;
			break;
		}

		switch(conspace){
		case(cmRGBData):
			info->dcOutputFormat = MONACO_FMT_MINISBLACK;
			break;

		case(cmCMYKData):
		case(cmCMYData):
			info->dcOutputFormat = MONACO_FMT_MINISWHITE;
			break;

		case(cmLabData):
			info->dcOutputFormat = MONACO_FMT_LABMODE;
			break;

		default:
			info->dcOutputFormat = MONACO_FMT_NUMSAMPLEONLY;
			break;
		}

	}
	else{
		switch(devspace){
		case(cmRGBData):
			info->cdInputFormat = MONACO_FMT_MINISBLACK;
			break;

		case(cmCMYKData):
		case(cmCMYData):
			info->cdInputFormat = MONACO_FMT_MINISWHITE;
			break;

		case(cmLabData):
			info->cdInputFormat = MONACO_FMT_LABMODE;
			break;

		default:
			info->cdInputFormat = MONACO_FMT_NUMSAMPLEONLY;
			break;
		}

		switch(conspace){
		case(cmRGBData):
			info->cdOutputFormat = MONACO_FMT_MINISBLACK;
			break;

		case(cmCMYKData):
		case(cmCMYData):
			info->cdOutputFormat = MONACO_FMT_MINISWHITE;
			break;

		case(cmLabData):
			info->cdOutputFormat = MONACO_FMT_LABMODE;
			break;

		default:
			info->cdOutputFormat = MONACO_FMT_NUMSAMPLEONLY;
			break;
		}

	}
}

void IccExpert::getInfo(ICCinfo *info)
{
	long tablesize = 1;
	long	i;

	info->profileClass = cmLinkClass;			// input, display, output, devicelink, abstract, or color conversion profile type
	info->dataColorSpace = _in_data;			// color space of data 
	info->profileConnectionSpace = _out_data;	// profile connection color space 
	info->devtocon = 1;
	info->dcinputChannels = _in_comp;		// Number of input channels 
	info->dcoutputChannels = _out_comp;		// Number of output channels 
	info->dcgridPoints = ONE_D_ENTRIES;			// Number of clutTable grid points 
	info->dcbits = 8;					// Number of bits per channel 

	_convert_format(_in_data, _out_data, info, 1);

	for(i = 0; i < _in_comp; i++)
		tablesize *= ONE_D_ENTRIES;
	tablesize *= _out_comp;

	info->dclutsize = tablesize;				// lut size 
	info->contodev = 0;
	info->cdinputChannels = 0;		
	info->cdoutputChannels = 0;		 
	info->cdgridPoints = 0;			
	info->cdbits = 0;					
	info->cdlutsize = 0;	

	//_convert_format(_in_data, _out_data, info, 0);
}

void IccExpert::getTable(unsigned char *table)
{
	long tablesize = 1;
	long	i;
	
	for(i = 0; i < _in_comp; i++)
		tablesize *= ONE_D_ENTRIES;
	tablesize *= _out_comp;

	unsigned char* data;
	
	data = (unsigned char*)McoLockHandle(_tableH);
	memcpy((char*)table, (char*)data, tablesize);
	McoUnlockHandle(_tableH);
}

//evaluation on color data
McoStatus IccExpert::eval(unsigned char *in, unsigned char *out, int32 num)
{
	McoStatus status = MCO_FAILURE;
	unsigned char *table;
	
	table = (unsigned char*)McoLockHandle(_tableH);
	
	switch(_in_comp){
		case(1):
			//not supported yet
			break;
			
		case(3):
			switch(_out_comp){
				case(1):
				{
				Linterp3 interp3(table, 1, ONE_D_ENTRIES, in, 3, num, out);
				}
				status = MCO_SUCCESS;
				break;
				
				case(3):
				{
				Linterp3 interp3(table, 3, ONE_D_ENTRIES, in, 3, num, out);
				}
				status = MCO_SUCCESS;
				break;
				
				case(4):
				{
				Linterp3 interp3(table, 4, ONE_D_ENTRIES, in, 3, num, out);
				}
				status = MCO_SUCCESS;
				break;
			}	
			break;

		case(4):
			switch(_out_comp){
				case(1):
				{
				Linterp4 interp4(table, 1, ONE_D_ENTRIES, in, 4, num, out);
				}
				status = MCO_SUCCESS;
				break;
				
				case(3):
				{
				Linterp4 interp4(table, 3, ONE_D_ENTRIES, in, 4, num, out);
				}
				status = MCO_SUCCESS;
				break;
				
				case(4):
				{
				Linterp4 interp4(table, 4, ONE_D_ENTRIES, in, 4, num, out);
				}
				status = MCO_SUCCESS;
				break;
			}	
			break;
	}			
				
	return status;
}

/*
//test table
McoStatus IccExpert::_testCMYK(unsigned char *in, long width, long length)
{
	McoHandle testH;
	char*		test;
	long		i, j;
	long		size;
	
	size = width*length;
	testH = McoNewHandle(size*4);
	if(!testH)	return MCO_MEM_ALLOC_ERROR;
	test = (char*)McoLockHandle(testH);
	for(i = 0; i < size; i++){
		j = i*4;
		test[j] = in[j];
		test[j+1] = in[j+1];
		test[j+2] = in[j+2];
		test[j+3] = in[j+3];
	}
	
	McoUnlockHandle(testH);
	
	saveCMYKTiff(testH, width, length);
	McoDeleteHandle(testH);
	
	return MCO_SUCCESS;
}	

//test table
McoStatus IccExpert::_testRGB(unsigned char *in, long width, long length)
{
	McoHandle testH;
	char*		test;
	long		i, j;
	long		size;
	
	size = width*length;
	testH = McoNewHandle(size*3);
	if(!testH)	return MCO_MEM_ALLOC_ERROR;
	test = (char*)McoLockHandle(testH);
	for(i = 0; i < size; i++){
		j = i*3;
		test[j] = in[j];
		test[j+1] = in[j+1];
		test[j+2] = in[j+2];
	}
	
	McoUnlockHandle(testH);
	
	saveRGBTiff(testH, width, length);
	McoDeleteHandle(testH);
	
	return MCO_SUCCESS;
}

//test table
McoStatus IccExpert::_testLAB(unsigned char *in, long width, long length)
{
	McoHandle testH;
	char*		test;
	long		i, j;
	long		size;
	
	size = width*length;
	testH = McoNewHandle(size*3);
	if(!testH)	return MCO_MEM_ALLOC_ERROR;
	test = (char*)McoLockHandle(testH);
	for(i = 0; i < size; i++){
		j = i*3;
		test[j] = in[j];
		test[j+1] = in[j+1];
		test[j+2] = in[j+2];
	}
	
	McoUnlockHandle(testH);
	
	saveLABTiff(testH, width, length);
	McoDeleteHandle(testH);
	
	return MCO_SUCCESS;
}	
*/