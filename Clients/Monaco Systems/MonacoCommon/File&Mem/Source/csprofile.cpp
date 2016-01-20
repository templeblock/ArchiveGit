#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csprofile.h"

#if defined(_WIN32)
#include <time.h>

#else

#endif

CsFormat::CsFormat(void)
{
	long i;
	
	_tagcount = 0;
	
	_desclen = 0;
	for(i = 0; i < 256; i++)
		_desc[i] = 0x00;
		
	_cprtlen = 0;
	for(i = 0; i < 256; i++)
		_cprt[i] = 0x00;
		
#if defined(_WIN32)
	_swab.setSwab(TRUE);
#else

#endif
	
}

CsFormat::~CsFormat(void)
{
;
}

void CsFormat::_setDefaultTags(void)
{
	long i;
	
	_red.typeDescriptor = cmSigXYZType;
	_red.reserved = 0x00000000;
	_red.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	_red.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	_red.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;
	
	_green.typeDescriptor = cmSigXYZType;
	_green.reserved = 0x00000000;
	_green.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	_green.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	_green.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;

	_blue.typeDescriptor = cmSigXYZType;
	_blue.reserved = 0x00000000;
	_blue.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	_blue.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	_blue.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;

	_white.typeDescriptor = cmSigXYZType;
	_white.reserved = 0x00000000;
	_white.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	_white.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	_white.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;

	_black.typeDescriptor = cmSigXYZType;
	_black.reserved = 0x00000000;
	_black.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	_black.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	_black.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;
	
	_rcurveh.typeDescriptor = cmSigCurveType;
	_rcurveh.reserved = 0x00000000;
	_rcurveh.countValue = 0;

	_gcurveh.typeDescriptor = cmSigCurveType;
	_gcurveh.reserved = 0x00000000;
	_gcurveh.countValue = 0;

	_bcurveh.typeDescriptor = cmSigCurveType;
	_bcurveh.reserved = 0x00000000;
	_bcurveh.countValue = 0;
	
	for(i = 0; i < 256; i++)
		_rcurve[i] = (unsigned short)i*256;
		
	for(i = 0; i < 256; i++)
		_gcurve[i] = (unsigned short)i*256;

	for(i = 0; i < 256; i++)
		_bcurve[i] = (unsigned short)i*256;		
}

void	CsFormat::_swabTagRecord(CMTagRecord *record, u_int32 num)
{
	u_int32	i;

	for(i = 0; i < num; i++)
		_swab.swabLong((u_int32*)&record[i], 3);
}

void	CsFormat::_swabLut8Type(CMLut8Type *luttype)
{
	_swab.swabLong((u_int32*)luttype, 2);
	_swab.swabLong((u_int32*)luttype->matrix, 9);
}

void	CsFormat::_swabLut16Type(CMLut16Type *luttype)
{
	_swab.swabLong((u_int32*)luttype, 2);
	_swab.swabLong((u_int32*)luttype->matrix, 9);
	_swab.swabShort((u_int16*)&luttype->inputTableEntries, 2);
}

void	CsFormat::_swabTagType(u_int32* tagtype)
{
	_swab.swabLong((u_int32*)tagtype, 2);
}

void	CsFormat::_swabCurveType(u_int32* curvetype)
{
	u_int32	num;

	_swab.swabLong((u_int32*)curvetype, 3);
	num = curvetype[2];
	_swab.swabShort((u_int16*)(&curvetype[3]), num);
}

void	CsFormat::_swabXYZType(u_int32* xyztype)
{
	_swab.swabLong((u_int32*)xyztype, 5);
}

void	CsFormat::_swabProfileSequenceDescTagType(u_int32* data)
{
	_swab.swabLong((u_int32*)data, 3);
}

//this is kind of default version of setHeader
//profiletype is what kind of profile
McoStatus	CsFormat::setHeader(long profileclass, long luttype)
{
#if defined(_WIN32)
	struct dtrp{
		short year;
		short month;
		short day;
		short hour;
		short minute;
		short second;
	}dtrp;

	struct tm *local;
    time_t long_time;

    time( &long_time );                /* Get time as long integer. */
    local = localtime( &long_time ); /* Convert to local time. */

	dtrp.second= (short)local->tm_sec;     /* seconds after the minute - [0,59] */
	dtrp.minute= (short)local->tm_min;     /* minutes after the hour - [0,59] */
	dtrp.day= (short)local->tm_mday;    /* day of the month - [1,31] */
	dtrp.year= (short)local->tm_year+1900;    /* years since 1900 */
	dtrp.month= (short)local->tm_mon+1;    /* days since January 1 - [0,365] */
	dtrp.hour= (short)local->tm_hour;        
    
#else
	u_int32 secs;
	DateTimeRec	dtrp;
	GetDateTime(&secs);
	SecondsToDate(secs,&dtrp);
    
#endif

	_tagcount = 0;
	_profileclass = profileclass;
	_luttype = luttype;

	_header.creator = 0x00000000;
	for(int i = 0; i < 44; i++)
		_header.reserved[i] = 0x00;

	_header.CMMType = MONACO_CMM;
	_header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	_header.CS2profileSignature = cmMagicNumber;	//?
	_header.platform = MCOP_DEFAULT_PLATFORM;
	_header.flags = MCOP_DEFAULT_FLAGS;
	_header.deviceManufacturer = MCOP_DEFAULT_MANUFACTURER;
	_header.deviceModel = MCOP_DEFAULT_MODEL;
	_header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	_header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	_header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	_header.white.X = MCOP_DEFAULT_WHITE_X;
	_header.white.Y = MCOP_DEFAULT_WHITE_Y;
	_header.white.Z = MCOP_DEFAULT_WHITE_Z;

	switch(_profileclass){
		case(cmInputClass):
		_header.profileClass = cmInputClass;
		_header.dataColorSpace = cmRGBData;
		_header.profileConnectionSpace = MCOP_DEFAULT_CON_SPACE;
		memcpy((char*)&(_header.dateTime), (char*)&dtrp, 12);
		break;
		
		case(cmDisplayClass):
		_header.profileClass = cmDisplayClass;
		_header.dataColorSpace = cmRGBData;
		_header.profileConnectionSpace = cmXYZData;
		memcpy((char*)&(_header.dateTime), (char*)&dtrp, 12);
		break;
		
		case(cmOutputClass):
		_header.profileClass = cmOutputClass;
		_header.dataColorSpace = cmCMYKData;
		_header.profileConnectionSpace = MCOP_DEFAULT_CON_SPACE;
		memcpy((char*)&(_header.dateTime), (char*)&dtrp, 12);
		break;

		case(cmLinkClass):
		_header.profileClass = cmLinkClass;
		_header.dataColorSpace = cmCMYKData;
		_header.profileConnectionSpace = MCOP_DEFAULT_CON_SPACE;
		memcpy((char*)&(_header.dateTime), (char*)&dtrp, 12);
		break;
		
		default:
		return MCO_FAILURE;
	}

/*
//set copy right
	char	*cprt_des = MCOP_CPRT; //"Monaco Systems Inc.";
	_setCprttypedata(cprt_des, strlen(cprt_des));
*/
	
	return MCO_SUCCESS;		
}

//user has more control of header infomation
McoStatus	CsFormat::setHeader(CM2Header* header, long luttype)
{
#if defined(_WIN32)
	struct dtrp{
		short year;
		short month;
		short day;
		short hour;
		short minute;
		short second;
	}dtrp;

	struct tm *local;
    time_t long_time;

    time( &long_time );                /* Get time as long integer. */
    local = localtime( &long_time ); /* Convert to local time. */

	dtrp.second= (short)local->tm_sec;     /* seconds after the minute - [0,59] */
	dtrp.minute= (short)local->tm_min;     /* minutes after the hour - [0,59] */
	dtrp.day= (short)local->tm_mday;    /* day of the month - [1,31] */
	dtrp.year= (short)local->tm_year+1900;    /* years since 1900 */
	dtrp.month= (short)local->tm_mon+1;    /* days since January 1 - [0,365] */
	dtrp.hour= (short)local->tm_hour;        

#else    
	u_int32 secs;
	DateTimeRec	dtrp;
	GetDateTime(&secs);
	SecondsToDate(secs,&dtrp);
	
#endif

	_tagcount = 0;
	_luttype = luttype;
	memcpy((char*)&_header, (char*)header, sizeof(CM2Header));
	memcpy((char*)&(_header.dateTime), (char*)&dtrp, 12);

/*
//set copy right
	char	*cprt_des = MCOP_CPRT; //"Monaco Systems Inc.";
	_setCprttypedata(cprt_des, strlen(cprt_des));
*/
	return MCO_SUCCESS;
}

long 	CsFormat::_next_offset(long offset, long add)
{
	return (offset +  (((add + 3) >> 2) << 2));
}	

McoStatus	CsFormat::setLuttypedata(long rendertype, Ctype *ctype, McoHandle dataH)
{
	switch(rendertype){
		case(1):	//cmAToB0Tag
		memcpy((char*)&_A2B0, (char*)ctype, sizeof(Ctype));
		_A2B0_H = dataH;
		_tags[_tagcount].tag = cmAToB0Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;
		
		case(2):	//cmBToA0Tag
		memcpy((char*)&_B2A0, (char*)ctype, sizeof(Ctype));
		_B2A0_H = dataH;
		_tags[_tagcount].tag = cmBToA0Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;

		case(3):	//cmGamutTag
		memcpy((char*)&_out_of_gamut, (char*)ctype, sizeof(Ctype));
		_out_of_gamut_H = dataH;
		_tags[_tagcount].tag = cmGamutTag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;

		case(4):	//cmAToB1Tag
		memcpy((char*)&_A2B1, (char*)ctype, sizeof(Ctype));
		_A2B1_H = dataH;
		_tags[_tagcount].tag = cmAToB1Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;
		
		case(5):	//cmBToA1Tag
		memcpy((char*)&_B2A1, (char*)ctype, sizeof(Ctype));
		_B2A1_H = dataH;
		_tags[_tagcount].tag = cmBToA1Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;

		case(6):	//cmAToB2Tag
		memcpy((char*)&_A2B2, (char*)ctype, sizeof(Ctype));
		_A2B2_H = dataH;
		_tags[_tagcount].tag = cmAToB2Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;
		
		case(7):	//cmBToA2Tag
		memcpy((char*)&_B2A2, (char*)ctype, sizeof(Ctype));
		_B2A2_H = dataH;
		_tags[_tagcount].tag = cmBToA2Tag;
		_tags[_tagcount].elementSize = _calLutTypesize(ctype);
		_tagcount++;
		break;
	
		default:
			return MCO_FAILURE;
	}

	return MCO_SUCCESS;
}

//note! namelen including the null character
McoStatus	CsFormat::setCprttypedata(char* name, long namelen)
{
	long taghead = cmSigTextType;
	long 	zero = 0;

	memcpy(_cprt, (char *)&taghead, 4);
	memcpy(&_cprt[4], (char *)&zero, 4);
	memcpy(&_cprt[8], (char *)name, namelen+1);
	
	_cprtlen = 8 + namelen + 1;
	
	_tags[_tagcount].tag = cmCopyrightTag;
	_tags[_tagcount].elementSize = _cprtlen;
	_tagcount++;
		
	return MCO_SUCCESS;
}

//type defines red, green or blue curve type
McoStatus	CsFormat::setCurvetypedata(long type, CMCurveType *ch, unsigned short *data)
{
	long entrysize;
	
	entrysize = ch->countValue;
	switch(type){
		case(cmGrayTRCTag):
		memcpy((char*)&_rcurveh, (char*)ch, sizeof(CMCurveType));
		memcpy((char*)_rcurve, (char*)data, entrysize*sizeof(unsigned short));
		_tags[_tagcount].tag = cmGrayTRCTag;
		_tags[_tagcount].elementSize = _calCurveTypesize(ch);
		_tagcount++;
		break;
	
		case(cmRedTRCTag):
		memcpy((char*)&_rcurveh, (char*)ch, sizeof(CMCurveType));
		memcpy((char*)_rcurve, (char*)data, entrysize*sizeof(unsigned short));
		_tags[_tagcount].tag = cmRedTRCTag;
		_tags[_tagcount].elementSize = _calCurveTypesize(ch);
		_tagcount++;
		break;
		
		case(cmGreenTRCTag):
		memcpy((char*)&_gcurveh, (char*)ch, sizeof(CMCurveType));
		memcpy((char*)_gcurve, (char*)data, entrysize*sizeof(unsigned short));
		_tags[_tagcount].tag = cmGreenTRCTag;
		_tags[_tagcount].elementSize = _calCurveTypesize(ch);
		_tagcount++;
		break;

		case(cmBlueTRCTag):
		memcpy((char*)&_bcurveh, (char*)ch, sizeof(CMCurveType));
		memcpy((char*)_bcurve, (char*)data, entrysize*sizeof(unsigned short));
		_tags[_tagcount].tag = cmBlueTRCTag;
		_tags[_tagcount].elementSize = _calCurveTypesize(ch);
		_tagcount++;
		break;
		
		default:
		return MCO_FAILURE;
	}					


	return MCO_SUCCESS;
}

//note! namelen including the null character
McoStatus	CsFormat::setDesctypedata(char* name, long namelen)
{
	long taghead = cmSigProfileDescriptionType;
	unsigned char nameclen;
	long	swapnamelen;

	nameclen = (unsigned char)namelen;
	swapnamelen = namelen;
	if(_swab.getSwab())	_swab.swabLong((u_int32*)&swapnamelen, 1);

	memcpy(_desc, (char *)&taghead, 4);
	memcpy(&_desc[8], (char *)&swapnamelen, 4);
	memcpy(&_desc[12], name, namelen-1);
	memcpy(&_desc[12+namelen+4], (char *)&swapnamelen, 4); 
	memcpy(&_desc[12+namelen+8], name, namelen-1);

	memcpy(&_desc[12+2*namelen+10], (char *)&nameclen, 1);
	memcpy(&_desc[12+2*nameclen+11], name, namelen-1);
	
	_desclen = 12 + namelen + 8 + namelen + 3 + namelen;

	_tags[_tagcount].tag = cmProfileDescriptionTag;
	_tags[_tagcount].elementSize = _desclen;	
	_tagcount++;
	
	return MCO_SUCCESS;
}

void	CsFormat::setPseqtypedata(char* name, long namelen)
{	
	memcpy(_pseq, name, namelen);
	_tags[_tagcount].tag = cmProfileSequenceDescTag;
	_tags[_tagcount].elementSize = namelen;	
	_tagcount++;	
}

McoStatus	CsFormat::setPseqtypedata(long times)
{
	long 	i = 0;
	long 	zero = 0;
	char	des[256];
	long 	taghead = 'pseq';
	long	fake[6] ={
		0x20202020,
		0x20202020,
		0x00000000,	
		0x00000000,
		0x20202020,
		0x3F003F00
	};

	memcpy(des, (char*)&taghead, 4);
	if(_swab.getSwab())	_swab.swabLong((u_int32*)des, 1);	
	memcpy(&des[4], (char*)&zero, 4);
	memcpy(&des[8], (char*)&times, 4);
	if(_swab.getSwab())	_swab.swabLong((u_int32*)&des[8], 1);	
	if(_swab.getSwab())	_swab.swabLong((u_int32*)fake, 6);	
	for(i = 0; i < times; i++)
		memcpy(&des[12+24*i], (char*)fake, 24);

	setPseqtypedata(des, 12 + 24*i);
	
	return MCO_SUCCESS;
}

//type defines red, green, blue, black or white point type
McoStatus	CsFormat::setXYZtypedata(long type, CMXYZType	*xyz)
{
	
	switch(type){
		case(cmRedColorantTag):
		memcpy((char*)&_red, (char*)xyz, sizeof(CMXYZType));
		_tags[_tagcount].tag = cmRedColorantTag;
		_tags[_tagcount].elementSize = sizeof(CMXYZType);	
		_tagcount++;
		break;
		
		case(cmGreenColorantTag):
		memcpy((char*)&_green, (char*)xyz, sizeof(CMXYZType));
		_tags[_tagcount].tag = cmGreenColorantTag;
		_tags[_tagcount].elementSize = sizeof(CMXYZType);	
		_tagcount++;
		break;

		case(cmBlueColorantTag):
		memcpy((char*)&_blue, (char*)xyz, sizeof(CMXYZType));
		_tags[_tagcount].tag = cmBlueColorantTag;
		_tags[_tagcount].elementSize = sizeof(CMXYZType);	
		_tagcount++;
		break;

		case(cmMediaWhitePointTag):
		memcpy((char*)&_white, (char*)xyz, sizeof(CMXYZType));
		_tags[_tagcount].tag = cmMediaWhitePointTag;
		_tags[_tagcount].elementSize = sizeof(CMXYZType);	
		_tagcount++;
		break;

		case(cmMediaBlackPointTag):
		memcpy((char*)&_black, (char*)xyz, sizeof(CMXYZType));
		_tags[_tagcount].tag = cmMediaBlackPointTag;
		_tags[_tagcount].elementSize = sizeof(CMXYZType);	
		_tagcount++;
		break;

		default:
		return MCO_FAILURE;
	}					

	return MCO_SUCCESS;
}

//added for saving the raw data file
McoStatus	CsFormat::sets15Fixed16ArrayTypedata(long tag, long size, Fixed *data)
{
	switch(tag){
		case(MC0P_C_LINEAR_TAG):
		_clineardata = data;
		break;

		case(MC0P_M_LINEAR_TAG):
		_mlineardata = data;
		break;
		
		case(MC0P_Y_LINEAR_TAG):
		_ylineardata = data;
		break;
 
 		case(MC0P_K_LINEAR_TAG):
		_klineardata = data;
		break;		

		case(MC0P_C_TONE_TAG):
		_ctonedata = data;
		break;

		case(MC0P_M_TONE_TAG):
		_mtonedata = data;
		break;
		
		case(MC0P_Y_TONE_TAG):
		_ytonedata = data;
		break;
 
 		case(MC0P_K_TONE_TAG):
		_ktonedata = data;
		break;		
	
		default:
		return MCO_FAILURE;
	}
			
	_tags[_tagcount].tag = tag;
	_tags[_tagcount].elementSize = 8 + size*4;	
	_tagcount++;
	
	return MCO_SUCCESS;
}

McoStatus	CsFormat::setDataTypedata(long tag, long size, char *data)
{
//	switch(tag){
//		case(MC0P_RAW_DATA_TAG):
//		_settingtag.typeDescriptor = cmSigDataType;
//		_settingtag.reserved = 0x00000000;
//		_settingtag.dataFlag = 0x00000001;
//		_settingdata = data;
//		break;
//	
//		default:
//		break;
//	}
			
	_tags[_tagcount].tag = tag;
	_tags[_tagcount].elementSize = 12 + size;	
	_tagcount++;
	
	return MCO_SUCCESS;
}

McoStatus	CsFormat::setSettingTypedata(MNSettingType	*setting)
{

	_settingdata = (long*)setting;
	_tags[_tagcount].tag = MC0P_SETTING_TAG;
	_tags[_tagcount].elementSize = 8 + sizeof(MNSettingType);	
	_tagcount++;
	
	return MCO_SUCCESS;
}


McoStatus	CsFormat::setPatchTypedata(long type, MNPatchType *patch)
{

	switch(type){
		case(MCOP_SRC_PATCH_TAG):
		_srcpatch = patch;
		_tags[_tagcount].tag = MCOP_SRC_PATCH_TAG;
		break;
			
		case(MCOP_DEST_PATCH_TAG):
		_destpatch = patch;
		_tags[_tagcount].tag = MCOP_DEST_PATCH_TAG;
		break;
		
		default:
		break;
	}
		
	_tags[_tagcount].elementSize = 8 + _calPatchTypesize(patch);	
	_tagcount++;
	
	return MCO_SUCCESS;
}

McoStatus CsFormat::setTweaktypedata(long numTweaks, MNTweakType *tweaktype)
{
	_numtweak = numTweaks;
	_tweak = tweaktype;

	_tags[_tagcount].tag = MC0P_TWEAK_TAG;
	
	_tags[_tagcount].elementSize = 8 + _numtweak * sizeof(MNTweakType);	
	_tagcount++;
	
	return MCO_SUCCESS;
}

long	CsFormat::_calPatchTypesize(MNPatchType *patch)
{
	long totalsize;
	long numcalpatch, numlinearpatch, totalstep, step, linearsize;
	
	linearsize = patch->numLinear;
	numlinearpatch = linearsize*patch->numComp*3;
	
	totalstep = 0;
	unsigned long i;
	for(i = 0; i < patch->numCube; i++)
		totalstep += patch->steps[i];
		
	numcalpatch = 0;
	for(i =0; i < patch->numCube; i++){
		step = patch->steps[i];
		numcalpatch += step*step*step;
	}	
	numcalpatch *= 3;
	
	totalsize = 4 + patch->numCube + patch->numCube + totalstep + patch->numLinear + 
			numcalpatch + numlinearpatch;
	
	totalsize *= 4;
	totalsize += 100;
			
	return (totalsize);		
}

//calculate the curve data size
long	CsFormat::_calCurveTypesize(CMCurveType *curve)
{
	return (12 + curve->countValue*sizeof(unsigned short));
}

//calculate the lut data size
long	CsFormat::_calLutTypesize(Ctype* AB)
{
	long i;
	long lutsize;
	long datalen = 0;
	
	if( _luttype == cmSigLut8Type){
		datalen = 48 + AB->input_channels*256 + \
						AB->output_channels*256;
		lutsize = 1;				
		for(i = 0; i < AB->input_channels; i++)
			lutsize *= AB->grid_points;			
		lutsize *= 	AB->output_channels;
		datalen += lutsize;
	}
	else{ //_luttype == cmSigLut16Type
		datalen = 52 + AB->input_channels*AB->input_entries*2 + \
						AB->output_channels*AB->output_entries*2;
		lutsize = 1;				
		for(i = 0; i < AB->input_channels; i++)
			lutsize *= AB->grid_points;			
		lutsize *= 	AB->output_channels*2;
		datalen += lutsize;
	}	
	return datalen;
}


McoStatus	CsFormat::_writeCurvetype(CMTagRecord *tag, CMCurveType* curve, char* value)
{
	McoStatus status;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	int count = curve->countValue;
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)curve, 3);
	
	status = relWrite(12, (char *)curve);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
	
	if(count > 0){	 
		if(_swab.getSwab())
			_swab.swabShort((unsigned short*)value, count);

		status = relWrite(count*2, value);
		if(status != MCO_SUCCESS) {
			 return MCO_FILE_WRITE_ERROR; }
	}		 

	return MCO_SUCCESS;
}

McoStatus	CsFormat::_writeSettingtype(CMTagRecord *tag)
{
	McoStatus status;
	long taghead = MNSETTINGTYPE;	
	long zero = 0x00000000;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)&taghead, 1);
	
	status = relWrite(4, (char *)&taghead);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	status = relWrite(4, (char *)&zero);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
		 
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)( (char*)_settingdata + 200 ), (sizeof(MNSettingType) - 200)/4);
	
	status = relWrite(sizeof(MNSettingType), (char*)_settingdata);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	return MCO_SUCCESS;
}

McoStatus	CsFormat::_writeTweakTypedata(CMTagRecord *tag)
{
	McoStatus status;
	long taghead = MNTWEAKTYPE;	
	long zero = 0x00000000;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)&taghead, 1);
	
	status = relWrite(4, (char *)&taghead);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	status = relWrite(4, (char *)&zero);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
		 
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)_tweak, (tag->elementSize - 8)/4);
	
	status = relWrite(tag->elementSize - 8, (char*)_tweak);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	return MCO_SUCCESS;
}

McoStatus	CsFormat::_writePatchtype(CMTagRecord *tag, MNPatchType *patch)
{
	McoStatus status;
	long taghead = MNPATCHTYPE;	
	long zero = 0x00000000;
	long numcalpatch, numlinearpatch, totalstep, step, linearsize;
	long numCube,numComp;
	
	numComp = patch->numComp;
	numCube = patch->numCube;
	linearsize = patch->numLinear;
	numlinearpatch = 3*linearsize*patch->numComp;
	
	totalstep = 0;
	unsigned long i;
	for(i = 0; i < patch->numCube; i++)
		totalstep += patch->steps[i];
		
	numcalpatch = 0;
	for(i =0; i < patch->numCube; i++){
		step = patch->steps[i];
		numcalpatch += step*step*step;
	}	
	numcalpatch *= 3;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)&taghead, 1);
	
	status = relWrite(4, (char *)&taghead);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	status = relWrite(4, (char *)&zero);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	status = relWrite(100, (char *)patch->name);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
		 
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)&(patch->patchType), 4);
	status = relWrite(4*sizeof(long), (char *)&(patch->patchType));
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong(patch->steps, numCube);
	status = relWrite(numCube*sizeof(long), (char *)patch->steps);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)patch->blacks, numCube);
	status = relWrite(numCube*sizeof(long), (char *)patch->blacks);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)patch->cmy, totalstep);
	status = relWrite(totalstep*sizeof(long), (char *)patch->cmy);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(linearsize){
		if(_swab.getSwab())
			_swab.swabLong((unsigned long*)patch->linear, linearsize);
		status = relWrite(linearsize*sizeof(long), (char *)patch->linear);
		if(status != MCO_SUCCESS) {
			 return MCO_FILE_WRITE_ERROR; }
	}
	
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)patch->data, numcalpatch);
	status = relWrite(numcalpatch*sizeof(long), (char *)patch->data);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
	
	if(linearsize){
		if(_swab.getSwab())
			_swab.swabLong((unsigned long*)patch->ldata, numlinearpatch);
		status = relWrite(numlinearpatch*sizeof(long), (char *)patch->ldata);
		if(status != MCO_SUCCESS) {
			 return MCO_FILE_WRITE_ERROR; }
	}
		
	return MCO_SUCCESS;		
}

//rendertype is for what kind of rendering
//luttype is for what kind of lut, cmSigLut8Type or cmSigLut16Type
//direc is which direction, from dev_to_con, or con_to_dev
//1 is from dev_to_con, 0 is from con_to_dev to dev_to_con
//updated for handling the prelog and postlog
McoStatus	CsFormat::_writeLuttype(CMTagRecord *tag, long luttype, long rendertype, short cont)
{
	McoStatus status;
	long one = 0x00010000;
	char tag_cont[256];
	unsigned short tag_cont16[256];
	char *data;
	Ctype* tagspec;
	long lut_size;
	long	i;
	long taghead;	
	
	switch(rendertype){
		case(1):	//cmAToB0Tag
		if(cont)
			data = (char*)McoLockHandle(_A2B0_H);
		tagspec = &_A2B0;
		break;
		
		case(2):	//cmBToA0Tag
		if(cont)
			data = (char*)McoLockHandle(_B2A0_H);
		tagspec = &_B2A0;
		break;

		case(3):	//cmGamutTag
		if(cont)
			data = (char*)McoLockHandle(_out_of_gamut_H);
		tagspec = &_out_of_gamut;
		break;

		case(4):	//cmAToB1Tag
		if(cont)
			data = (char*)McoLockHandle(_A2B1_H);
		tagspec = &_A2B1;
		break;
		
		case(5):	//cmBToA1Tag
		if(cont)
			data = (char*)McoLockHandle(_B2A1_H);
		tagspec = &_B2A1;
		break;

		case(6):	//cmAToB2Tag
		if(cont)
			data = (char*)McoLockHandle(_A2B2_H);
		tagspec = &_A2B2;
		break;
		
		case(7):	//cmBToA2Tag
		if(cont)
			data = (char*)McoLockHandle(_B2A2_H);
		tagspec = &_B2A2;
		break;
	
		default:
			return MCO_FAILURE;
	}

	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;
		
	for( i = 0; i < 256; i++)
		tag_cont[i] = 0;
	
	if(luttype	== 	cmSigLut8Type)
		taghead = cmSigLut8Type;
	else
		taghead = cmSigLut16Type;
	memcpy(tag_cont, (char *)&taghead, 4);

	tag_cont[8] = tagspec->input_channels;
	tag_cont[9] = tagspec->output_channels;
	tag_cont[10] = tagspec->grid_points;

	if(tagspec->identity_matrix){
		memcpy(&tag_cont[12], (char *)&one, 4);
		memcpy(&tag_cont[28], (char *)&one, 4);
		memcpy(&tag_cont[44], (char *)&one, 4);
	}
	else
		memcpy(&tag_cont[12], (char *)&(tagspec->e00), 36);
	
	if(taghead == cmSigLut8Type){
		if(_swab.getSwab())
			_swabLut8Type((CMLut8Type*)tag_cont);
		status = relWrite(48, tag_cont);
		if(status != MCO_SUCCESS) {
			 return MCO_FILE_WRITE_ERROR; }
	}
	else{	//taghead == cmsigLut16Type
		memcpy(&tag_cont[48], (char*)&(tagspec->input_entries), 2);
		memcpy(&tag_cont[50], (char*)&(tagspec->output_entries), 2);

		if(_swab.getSwab())
			_swabLut16Type((CMLut16Type*)tag_cont);

		status = relWrite(52, tag_cont);
		if(status != MCO_SUCCESS) {
			 return MCO_FILE_WRITE_ERROR; }
	}


//modified on 11/3
	if(luttype	== 	cmSigLut8Type){
		//for( i = 0; i < 256; i++)
		//	tag_cont[i] = i;
		for( i = 0; i < tagspec->input_channels; i++){
			status = relWrite(256, tagspec->input_tables[i]);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
		
		lut_size = 1;
		for( i = 0; i < tagspec->input_channels; i++)
			lut_size *= tagspec->grid_points;
		lut_size *= tagspec->output_channels;	
			
		if(cont){
			status = relWrite(lut_size, data);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
		else{
			status = getFilePosition(&(tagspec->offset));
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
			status = setFilePosition(SEEK_SET, tagspec->offset + lut_size);
			if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;
		}
			
		for( i = 0; i < tagspec->output_channels; i++){
			status = relWrite(256, tagspec->output_tables[i]);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
	}
	else{
/*
		for( i = 0; i < 256; i++)
			tag_cont16[i] = (i + (i << 8));
*/			
		for( i = 0; i < tagspec->input_channels; i++){
			status = relWrite(tagspec->input_entries*2, tagspec->input_tables[i]);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
	
		lut_size = 1;
		for( i = 0; i < tagspec->input_channels; i++)
			lut_size *= tagspec->grid_points;
		lut_size *= tagspec->output_channels*2;	
		
		if(cont){
			if(_swab.getSwab() && (taghead == cmSigLut16Type))
				_swab.swabShort((u_int16*)data, lut_size/2);

			status = relWrite(lut_size, data);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
		else{
			status = getFilePosition(&(tagspec->offset));
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
			status = setFilePosition(SEEK_SET, tagspec->offset + lut_size);
			if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;
		}
		for( i = 0; i < tagspec->output_channels; i++){
			status = relWrite(tagspec->output_entries*2, tagspec->output_tables[i]);
			if(status != MCO_SUCCESS) {
				 return MCO_FILE_WRITE_ERROR; }
		}
	}		

	return MCO_SUCCESS;
}

//simplest tag writing
McoStatus	CsFormat::_writeTagdata(CMTagRecord *tag, char *data)
{
	McoStatus status;

	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	status = relWrite(tag->elementSize, data);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	return MCO_SUCCESS;
}

//write the tag
McoStatus	CsFormat::writeTag(short	cont)
{
	McoStatus	status;
	long	offset;
	long i;

	offset = 128 + 4 + _tagcount*sizeof(CMTagRecord);

//set offset for the tags	
	for(i = 0; i < _tagcount; i++){
		_tags[i].elementOffset = offset;
		offset = _next_offset(offset, _tags[i].elementSize);
	}

//write header stuff
	_header.size = _tags[_tagcount-1].elementOffset + _tags[_tagcount-1].elementSize;

	status = setFilePosition(SEEK_SET, 0);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	if(_swab.getSwab()){
		_swab.swabLong((u_int32*)&_header, 6);
		_swab.swabShort((u_int16*)&(_header.dateTime), 6);
		_swab.swabLong((u_int32*)&(_header.CS2profileSignature), 11);
	}

	status = relWrite(sizeof(_header), (char*)&_header);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&_tagcount, 1);

	status = relWrite(4, (char*)&_tagcount);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&_tagcount, 1);

	if(_swab.getSwab())
		_swabTagRecord(_tags, _tagcount);

	status = relWrite(sizeof(CMTagRecord)*_tagcount, (char*)_tags);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
	_swabTagRecord(_tags, _tagcount);

//write tags
	for(i = 0; i < _tagcount; i++){
	
		switch(_tags[i].tag){
			case(cmAToB0Tag):
			status = _writeLuttype(&_tags[i], _luttype, 1, cont);
			break;	
			
			case(cmAToB1Tag):
			status = _writeLuttype(&_tags[i], _luttype, 4, cont);
			break;	
			
			case(cmAToB2Tag):
			status = _writeLuttype(&_tags[i], _luttype, 6, cont);
			break;	
			
			case(cmBlueColorantTag):
			if(_swab.getSwab())
				_swabXYZType((u_int32*)&_blue);
			status = _writeTagdata(&_tags[i], (char*)&_blue);
			break;
			
			case(cmBlueTRCTag):
			status = _writeCurvetype(&_tags[i], &_bcurveh, (char*)_bcurve);
			break;
			
			case(cmBToA0Tag):
			status = _writeLuttype(&_tags[i], _luttype, 2, cont);
			break;	
							
			case(cmBToA1Tag):
			status = _writeLuttype(&_tags[i], _luttype, 5, cont);
			break;	
			
			case(cmBToA2Tag):
			status = _writeLuttype(&_tags[i], _luttype, 7, cont);
			break;	
			
			//not supported					
			case(cmCalibrationDateTimeTag):	
			case(cmCharTargetTag):	
			break;
						
			case(cmCopyrightTag):
			if(_swab.getSwab())
				_swab.swabLong((u_int32*)&_cprt, 2);
			status = _writeTagdata(&_tags[i], (char*)&_cprt);
			break;
			
			//not supported		
			case(cmDeviceMfgDescTag):		
			case(cmDeviceModelDescTag):	
			break;
				
			case(cmGamutTag):
			status = _writeLuttype(&_tags[i], _luttype, 3, cont);
			break;	
								
			case(cmGrayTRCTag):
			status = _writeCurvetype(&_tags[i], &_rcurveh, (char*)_rcurve);
			break;
							
			case(cmGreenColorantTag):
			if(_swab.getSwab())
				_swabXYZType((u_int32*)&_green);
			status = _writeTagdata(&_tags[i], (char*)&_green);
			break;
						
			case(cmGreenTRCTag):
			status = _writeCurvetype(&_tags[i], &_gcurveh, (char*)_gcurve);
			break;
			
			//not supported				
			case(cmLuminanceTag):				
			case(cmMeasurementTag):
			break;
			
			case(cmMediaBlackPointTag):
			if(_swab.getSwab())
				_swabXYZType((u_int32*)&_black);
			status = _writeTagdata(&_tags[i], (char*)&_black);
			break;
			
			case(cmMediaWhitePointTag):
			if(_swab.getSwab())
				_swabXYZType((u_int32*)&_white);
			status = _writeTagdata(&_tags[i], (char*)&_white);
			break;
			
			//not supported
			case(cmNamedColorTag):
			case(cmPreview0Tag):
			case(cmPreview1Tag):
			case(cmPreview2Tag):
			break;
			
			case(cmProfileDescriptionTag):
			if(_swab.getSwab())
				_swab.swabLong((u_int32*)&_desc, 2);
			status = _writeTagdata(&_tags[i], (char*)&_desc);
			break;
			
			case(cmProfileSequenceDescTag):
			status = _writeTagdata(&_tags[i], (char*)&_pseq);
			break;
			
			//not supported 
			case(cmPS2CRD0Tag):
			case(cmPS2CRD1Tag):
			case(cmPS2CRD2Tag):
			case(cmPS2CRD3Tag):
			case(cmPS2CSATag):
			case(cmPS2RenderingIntentTag):
			break;
			
			case(cmRedColorantTag):
			if(_swab.getSwab())
				_swabXYZType((u_int32*)&_red);
			status = _writeTagdata(&_tags[i], (char*)&_red);
			break;
			
			case(cmRedTRCTag):
			status = _writeCurvetype(&_tags[i], &_rcurveh, (char*)_rcurve);
			break;
			
			//not supported
			case(cmScreeningDescTag):
			case(cmScreeningTag):
			case(cmTechnologyTag):
			case(cmUcrBgTag):
			case(cmViewingConditionsDescTag):
			case(cmViewingConditionsTag):
			break;
			
			case(MC0P_SETTING_TAG):
			status = _writeSettingtype(&_tags[i]);
			break;
			
			case(MCOP_SRC_PATCH_TAG):
			status = _writePatchtype(&_tags[i], _srcpatch);
			break;
			
			case(MCOP_DEST_PATCH_TAG):
			status = _writePatchtype(&_tags[i], _destpatch);
			break;

			case(MC0P_C_LINEAR_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _clineardata);
			break;
			
			case(MC0P_M_LINEAR_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _mlineardata);
			break;

			case(MC0P_Y_LINEAR_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _ylineardata);
			break;

			case(MC0P_K_LINEAR_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _klineardata);
			break;

			case(MC0P_C_TONE_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _ctonedata);
			break;
			
			case(MC0P_M_TONE_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _mtonedata);
			break;

			case(MC0P_Y_TONE_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _ytonedata);
			break;

			case(MC0P_K_TONE_TAG):
			status = _writes15Fixed16ArrayTypedata(&_tags[i], _ktonedata);
			break;

			case(MC0P_TWEAK_TAG):
			status = _writeTweakTypedata(&_tags[i]);
			break;
									
			default:
			break;
		}	

		if(status != MCO_SUCCESS)	return status;
	}
	
	return MCO_SUCCESS;
}

McoStatus CsFormat::_writes15Fixed16ArrayTypedata(CMTagRecord *tag, Fixed *value)
{
	McoStatus status;
	long taghead = cmSigS15Fixed16Type;	
	long zero = 0x00000000;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;

	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)&taghead, 1);

	status = relWrite(4, (char *)&taghead);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	status = relWrite(4, (char *)&zero);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
	
	if(_swab.getSwab())
		_swab.swabLong((unsigned long*)value, tag->elementSize/4 - 2);
	
	status = relWrite(tag->elementSize - 8, (char *)value);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	return MCO_SUCCESS;
}

McoStatus	CsFormat::_writeDatatype(CMTagRecord *tag, CMDataType* taghead, char* value)
{
	McoStatus status;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_WRITE_ERROR;
	
	status = relWrite(12, (char *)taghead);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }
	
	status = relWrite(tag->elementSize - 12, (char *)value);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	return MCO_SUCCESS;
}

//for step by step writing
//use this as your own risk
long	CsFormat::getA2B0position(void)
{
	return _A2B0.offset;	 
}

//for step by step writing
//use this as your own risk
long	CsFormat::getA2B1position(void)
{
	return _A2B1.offset;	 
}

//for step by step writing
//use this as your own risk
long	CsFormat::getA2B2position(void)
{
	return _A2B2.offset;	 
}

McoStatus	CsFormat::_extractHeader(void)
{
	McoStatus status;
	
	status = setFilePosition(SEEK_SET, 0);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	status = relRead(sizeof(_header), (char*)&_header);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_READ_ERROR; }

	if(_swab.getSwab()){
		_swab.swabLong((u_int32*)&_header, 6);
		_swab.swabShort((u_int16*)&(_header.dateTime), 6);
		//Peter, swab xyz also
		_swab.swabLong((u_int32*)&(_header.CS2profileSignature), 11);
	}

	if(	!(_header.profileVersion & cmCS2ProfileVersion) )	return MCO_FILE_READ_ERROR;

	_profileclass = _header.profileClass;	

	status = relRead(4, (char*)&_tagcount);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_WRITE_ERROR; }

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&_tagcount, 1);

	status = relRead(sizeof(CMTagRecord)*_tagcount, (char*)_tags);

	if(_swab.getSwab())
		_swabTagRecord(_tags, _tagcount);
	
	return status;
}

Boolean	CsFormat::isVersion2(void)
{
	McoStatus status;
	CM2Header	header;
	
	status = setFilePosition(SEEK_SET, 0);
	if(status != MCO_SUCCESS) return FALSE;

	status = relRead(sizeof(header), (char*)&header);
	if(status != MCO_SUCCESS) {
		 return FALSE; }

	if(	(header.profileVersion & cmCS2ProfileVersion) )	return TRUE;
	return FALSE;
}

McoStatus	CsFormat::getHeader(CM2Header* header, long *tagcount)
{
	McoStatus status;
	
	status = _extractHeader();
	if(status != MCO_SUCCESS)
		return status;
		
	*tagcount = _tagcount;
	memcpy((char*)header, (char*)&_header, sizeof(CM2Header));	 

	return MCO_SUCCESS;		 
}
	
//get a tag info from index	
//index is 1 based
McoStatus	CsFormat::getIndexTaginfo(long index, CMTagRecord* tags)
{
	memcpy((char*)tags, (char*)&_tags[index - 1], sizeof(CMTagRecord));

	return MCO_SUCCESS; 
}	

//get all the tag info once and for all	
McoStatus	CsFormat::getAllTaginfo(CMTagRecord* tags)
{
	memcpy((char*)tags, (char*)_tags, _tagcount*sizeof(CMTagRecord));

	return MCO_SUCCESS; 
}	

//return address of the tag record
CMTagRecord* CsFormat::_getTagrecord(long tag)
{
	long i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	

	if(i >= _tagcount) return (CMTagRecord*)NULL;
	
	return &_tags[i];
}

//get the curve data
McoStatus	CsFormat::getcurveTagdata(long tag, char *data)
{
	McoStatus status;
	CMCurveType curve;

	status = getCurveTypeinfo(tag, &curve);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	//changed by Peter
	//status = getPartialTagdata(tag, 12, (long)curve.countValue, data);
	status = getPartialTagdata(tag, 12, (long)curve.countValue*2, data);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	if(!_swab.getSwab())	return MCO_SUCCESS;

	_swab.swabShort((u_int16*)data, curve.countValue);

	return MCO_SUCCESS;
}

McoStatus	CsFormat::gets15Fixed16ArrayTagdata(long tag, char *data)
{
	McoStatus status;
	long 	tagsize;
	
	tagsize = getTagsize(tag);
	if( (status = getPartialTagdata(tag, 8, tagsize-8, (char*)data)) != MCO_SUCCESS)
		return status;
	
	if(!_swab.getSwab())	return MCO_SUCCESS;
	
	_swab.swabLong((u_int32*)data, (tagsize-8)/4);
	
	return MCO_SUCCESS;
}


//get the curve data
McoStatus	CsFormat::getMonacoSettingTagdata(char *data)
{
	McoStatus status;
	CMCurveType curve;
	long 	tagsize;

	tagsize = getTagsize(MC0P_SETTING_TAG);

	status = getPartialTagdata(MC0P_SETTING_TAG, 8, tagsize-8, data);
	if(status != MCO_SUCCESS)	return status;

	if(!_swab.getSwab())	return MCO_SUCCESS;

	_swab.swabLong((u_int32*)( data + 200 ), (sizeof(MNSettingType) - 200)/4);

	return MCO_SUCCESS;
}

McoStatus	CsFormat::getTweakTagdata(long tag, MNTweakType *tweaktype)
{
	McoStatus status;
	long 	tagsize, i;
	long	totalcmy = 0, calpatch = 0;
	
	tagsize = getTagsize(tag);
	if( (status = getPartialTagdata(tag, 8, tagsize-8, (char*)tweaktype)) != MCO_SUCCESS)
		return status;

	if(!_swab.getSwab())	return MCO_SUCCESS;
	_swab.swabLong((u_int32*)tweaktype, (tagsize-8)/4);

	return MCO_SUCCESS;
}

//load Monaco Patch data
//pointer in MNPatchType should be inited to 0 before calling this function
//caller should release the memory by calling delete
McoStatus	CsFormat::getMonacoPatchTagdata(long tag, MNPatchType *patchtype)
{
	McoStatus status;
	long 	tagsize;
	long	size;
	long	totalcmy = 0, calpatch = 0;
	
	size = 8;
	tagsize = getTagsize(tag);
	if( (status = getPartialTagdata(tag, size, 100, (char*)patchtype)) != MCO_SUCCESS)
		goto bail;
	
	size += 100;
	if( (status = getPartialTagdata(tag, size, 16, (char*)&(patchtype->patchType))) != MCO_SUCCESS)
		goto bail;
		
	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&(patchtype->patchType), 4);
		
	status = MCO_MEM_ALLOC_ERROR;
	patchtype->steps = new unsigned long[patchtype->numCube];
	if(!patchtype->steps)	goto bail;
	
	patchtype->blacks = new unsigned long[patchtype->numCube];
	if(!patchtype->blacks)	goto bail;
	
	size += 16;
	if( (status = getPartialTagdata(tag, size, 4*patchtype->numCube, (char*)patchtype->steps)) != MCO_SUCCESS)
		goto bail;

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)patchtype->steps, patchtype->numCube);

	size += 4*patchtype->numCube;
	if( (status = getPartialTagdata(tag, size, 4*patchtype->numCube, (char*)patchtype->blacks)) != MCO_SUCCESS)
		goto bail;

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)patchtype->blacks, patchtype->numCube);
	
	unsigned long i;
	for(i = 0; i < patchtype->numCube; i++)
		totalcmy += patchtype->steps[i];

	status = MCO_MEM_ALLOC_ERROR;
	patchtype->cmy = new unsigned long[totalcmy];
	if(!patchtype->cmy)	goto bail;

	size += 4*patchtype->numCube;
	if( (status = getPartialTagdata(tag, size, 4*totalcmy, (char*)patchtype->cmy)) != MCO_SUCCESS)
		goto bail;

	if(_swab.getSwab())
		_swab.swabLong((u_int32*)patchtype->cmy, totalcmy);
	
	if(patchtype->numLinear){
		status = MCO_MEM_ALLOC_ERROR;
		patchtype->linear = new Fixed[patchtype->numLinear];
		if(!patchtype->linear)	goto bail;
		patchtype->ldata = new Fixed[patchtype->numLinear*3*patchtype->numComp];
		if(!patchtype->ldata)	goto bail;
	}
	
	size += 4*totalcmy;
	if(patchtype->numLinear){
		if( (status = getPartialTagdata(tag, size, 4*patchtype->numLinear, (char*)patchtype->linear)) != MCO_SUCCESS)
			goto bail;

		if(_swab.getSwab())
			_swab.swabLong((u_int32*)patchtype->linear, patchtype->numLinear);
	}
	
	for(i = 0; i < patchtype->numCube; i++)
		calpatch += patchtype->steps[i]*patchtype->steps[i]*patchtype->steps[i];

	status = MCO_MEM_ALLOC_ERROR;
	patchtype->data = new Fixed[calpatch*3];
	if(!patchtype->data)	goto bail;
	
	size += 4*patchtype->numLinear;
	if( (status = getPartialTagdata(tag, size, 4*3*calpatch, (char*)patchtype->data)) != MCO_SUCCESS)
		goto bail;
	if(_swab.getSwab())
		_swab.swabLong((u_int32*)patchtype->data, 3*calpatch);
	
	size += 4*3*calpatch;
	if(patchtype->numLinear){
		if( (status = getPartialTagdata(tag, size, 4*3*patchtype->numLinear*patchtype->numComp, (char*)patchtype->ldata)) != MCO_SUCCESS)
			goto bail;

		if(_swab.getSwab())
			_swab.swabLong((u_int32*)patchtype->ldata, 3*patchtype->numLinear*patchtype->numComp);
	}
		
	return MCO_SUCCESS;		

bail:
	return status;
}

//get the tag data from CMTagRecord
McoStatus	CsFormat::getTagdata(CMTagRecord *tag, unsigned char *data)
{
	McoStatus status;
	long	i;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	status = relRead(tag->elementSize, (char*)data);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_READ_ERROR; }

	if(!_swab.getSwab())	return MCO_SUCCESS;

		switch(tag->tag){
			case(cmAToB0Tag):
			case(cmAToB1Tag):
			case(cmAToB2Tag):
			case(cmBToA0Tag):
			case(cmBToA1Tag):
			case(cmBToA2Tag):
			case(cmGamutTag):
			_swabTagType((u_int32*)data);
			//not been supported, use other functions instead
			break;	

			case(cmGreenColorantTag):
			case(cmBlueColorantTag):
			case(cmRedColorantTag):
			case(cmMediaBlackPointTag):
			case(cmMediaWhitePointTag):
			_swabXYZType((u_int32*)data);
			break;

			case(cmGrayTRCTag):
			case(cmRedTRCTag):
			case(cmGreenTRCTag):
			case(cmBlueTRCTag):
			_swabCurveType((u_int32*)data);
			break;

			
			//not supported					
			case(cmCalibrationDateTimeTag):	
			case(cmCharTargetTag):	
			_swabTagType((u_int32*)data);
			break;
						
			case(cmCopyrightTag):	
			_swabTagType((u_int32*)data);
			break;
			
			//not supported		
			case(cmDeviceMfgDescTag):		
			case(cmDeviceModelDescTag):	
			_swabTagType((u_int32*)data);
			break;
				
			//not supported				
			case(cmLuminanceTag):				
			case(cmMeasurementTag):
			_swabTagType((u_int32*)data);
			break;
			
			//not supported
			case(cmNamedColorTag):
			case(cmPreview0Tag):
			case(cmPreview1Tag):
			case(cmPreview2Tag):
			_swabTagType((u_int32*)data);
			break;
			
			case(cmProfileDescriptionTag):
			_swabTagType((u_int32*)data);
			break;
			
			case(cmProfileSequenceDescTag):
			_swabProfileSequenceDescTagType((u_int32*)data);
			break;
			
			//not supported 
			case(cmPS2CRD0Tag):
			case(cmPS2CRD1Tag):
			case(cmPS2CRD2Tag):
			case(cmPS2CRD3Tag):
			case(cmPS2CSATag):
			case(cmPS2RenderingIntentTag):
			_swabTagType((u_int32*)data);
			break;
						
			//not supported
			case(cmScreeningDescTag):
			case(cmScreeningTag):
			case(cmTechnologyTag):
			case(cmUcrBgTag):
			case(cmViewingConditionsDescTag):
			case(cmViewingConditionsTag):
			_swabTagType((u_int32*)data);
			break;
			
			default:
			//know nothing about the tag
			//so no tag swab
			break;
		}

	return MCO_SUCCESS; 	
}	

//get the tag data from tag
McoStatus	CsFormat::getTagdata(long tag, unsigned char *data)
{
	CMTagRecord*	tagrec;
	
	tagrec = _getTagrecord(tag);
	if(tagrec == 0)	return MCO_FAILURE;
	
	return getTagdata(tagrec, data);
}	

//get tag data starting from the offset of the specified tag
McoStatus	CsFormat::getPartialTagdata(long tag, long offset, long datasize, char *data)
{
	CMTagRecord*	tagrec;
	
	tagrec = _getTagrecord(tag);
	if(tagrec == 0)	return MCO_FAILURE;

	return getPartialTagdata(tagrec, offset, datasize, data);
}

//get tag data starting from the offset of the specified tag
McoStatus	CsFormat::getPartialTagdata(CMTagRecord *tag, long offset, long datasize, char *data)
{
	McoStatus status;
	
	status = setFilePosition(SEEK_SET, tag->elementOffset+offset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	
	status = relRead(datasize, data);
	if(status != MCO_SUCCESS) return status;
	
	return MCO_SUCCESS;
}

//get the tag info
//only for A2B0, A2B1, A2B2, B2A0, B2A1, B2A2
McoStatus	CsFormat::getLutTagdatainfo(long tag, CMLut16Type *tag16info)
{
	McoStatus status;
	long		luttype;
	long 		i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	
		
	status = setFilePosition(SEEK_SET, _tags[i].elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	status = relRead(4, (char*)&luttype);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&luttype, 1);

	status = setFilePosition(SEEK_SET, _tags[i].elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	status = relRead(sizeof(CMLut16Type), (char*)tag16info);
		if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	if( luttype == cmSigLut8Type){
		tag16info->inputTableEntries = 256;
		tag16info->outputTableEntries = 256;
	}	
	
	if(!_swab.getSwab())	return MCO_SUCCESS;

	if( luttype == cmSigLut16Type)
		_swabLut16Type(tag16info);
	else if( luttype == cmSigLut8Type)
		_swabLut8Type((CMLut8Type*)tag16info);
	else
		return MCO_FAILURE; 

	return MCO_SUCCESS;
}

//get the tag info
//only for curvetype tags
McoStatus	CsFormat::getCurveTypeinfo(long tag, CMCurveType *curve)
{
	McoStatus status;
	long		luttype;
	long 		i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	
		
	status = setFilePosition(SEEK_SET, _tags[i].elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	status = relRead(sizeof(CMCurveType), (char*)curve);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	if(_swab.getSwab())
		_swab.swabLong((u_int32*)curve, 3);

	return MCO_SUCCESS;
}

//get raw data from the tag, not including the tag info
//only for A2B0, A2B1, A2B2, B2A0, B2A1, B2A2
McoStatus	CsFormat::getLutTagdata(long tag, unsigned char *data)
{
	McoStatus status;
	CMLut16Type tag16info;
	CMLut8Type	tag8info;
	long		luttype;
	long 		dataoffset, datasize;
	long 		i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	

	if( (tag!=cmAToB0Tag) && (tag!=cmAToB1Tag) && (tag!=cmAToB2Tag) &&
		(tag!=cmBToA0Tag) && (tag!=cmBToA1Tag) && (tag!=cmBToA2Tag) )
		return MCO_FAILURE;
		
	status = setFilePosition(SEEK_SET, _tags[i].elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	status = relRead(4, (char*)&luttype);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	if(_swab.getSwab())
		_swab.swabLong((u_int32*)&luttype, 1);
	
	status = setFilePosition(SEEK_SET, _tags[i].elementOffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

	dataoffset = _tags[i].elementOffset;
	
	if( luttype == cmSigLut16Type){
		status = relRead(sizeof(CMLut16Type), (char*)&tag16info);
		if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

		if(_swab.getSwab())
			_swabLut16Type(&tag16info);  
			
		dataoffset += 52;
		//changed by Peter
		//dataoffset += 52 + tag16info.inputChannels*tag16info.inputTableEntries*2;
		datasize = 1; 
		for(i = 0; i < tag16info.inputChannels; i++)
			datasize *= tag16info.gridPoints;			
		datasize *= tag16info.outputChannels*2;
		
		datasize += tag16info.inputChannels*tag16info.inputTableEntries*2;
		datasize += tag16info.outputChannels*tag16info.outputTableEntries*2;
	}
	else if( luttype == cmSigLut8Type){	
		status = relRead(sizeof(CMLut8Type), (char*)&tag8info);
		if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;

		if(_swab.getSwab())
			_swabLut8Type(&tag8info);

		//changed by Peter
		dataoffset += 48;
		//dataoffset += 48 + tag8info.inputChannels*256;
		datasize = 1; 
		for(i = 0; i < tag8info.inputChannels; i++)
			datasize *= tag8info.gridPoints;			
		datasize *= tag8info.outputChannels;

		datasize += tag8info.inputChannels*256;
		datasize += tag8info.outputChannels*256;
	}
	else
		return MCO_FAILURE; 
	
	status = setFilePosition(SEEK_SET, dataoffset);
	if(status != MCO_SUCCESS) return MCO_FILE_READ_ERROR;
	status = relRead(datasize, (char*)data);
	if(status != MCO_SUCCESS) {
		 return MCO_FILE_READ_ERROR; }

	if(_swab.getSwab() && (luttype == cmSigLut16Type))
	_swab.swabShort((u_int16*)data, datasize/2);

	return MCO_SUCCESS; 
}	

//if exist return the tag index which is 1 based
//otherwise return 0
long	CsFormat::checkTagExist(long tag)
{
	long i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	

	if(i >= _tagcount) return 0;

	return (i+1);
}

long	CsFormat::getTagsize(long tag)
{
	long i;
	
	//check to see which tag
	for( i = 0; i < _tagcount; i++){
		if(_tags[i].tag == (unsigned long)tag)
			break;
	}	

	if(i >= _tagcount) return 0;

	return _tags[i].elementSize;
}

long	CsFormat::validProfile(void)
{
	McoStatus status;
	long exist = 0;
	
	status = _extractHeader();
	if(status != MCO_SUCCESS)
		return exist;
		
	if(_header.profileVersion < cmCS2ProfileVersion)
		return exist;
		
	switch(_header.profileClass){
		case(cmInputClass):
		switch(_header.dataColorSpace){
			case(cmGrayData):
			exist = _validinputGrayProfile();
			break;
			
			case(cmRGBData):
			exist = _validinputRGBProfile();
			break;
			
			case(cmCMYKData):
			exist = _validinputCMYKProfile();
			break;
		}	
		break;
		
		case(cmDisplayClass):
		switch(_header.dataColorSpace){
			case(cmGrayData):
			exist = _validdisplayGrayProfile();
			break;
			
			case(cmRGBData):
			exist = _validdisplayRGBProfile();
			break;
		}	
		break;
		
		case(cmOutputClass):
		switch(_header.dataColorSpace){
			case(cmGrayData):
			exist = _validoutputGrayProfile();
			break;
			
			case(cmRGBData):
			exist = _validoutputRGBProfile();
			break;
			
			case(cmCMYKData):
			exist = _validoutputCMYKProfile();
			break;
		}	
		break;

		case(cmLinkClass):
		exist = _validlinkProfile();
		break;

		case(cmColorSpaceClass):
		exist = _validspaceProfile();
		break;

		case(cmAbstractClass):
		exist = _validabstractProfile();
		break;
		
		default:
		exist = 0;
	}

	return exist;
}	

//not been tested yet
long	CsFormat::_validinputGrayProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;	
	if(checkTagExist(cmGrayTRCTag))
		i++;	
	if(checkTagExist(cmMediaWhitePointTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 4)	return 1;
	
	return 0;	
}		

//not been tested yet
long	CsFormat::_validinputRGBProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;	
	if(checkTagExist(cmRedColorantTag))
		i++;		
	if(checkTagExist(cmGreenColorantTag))
		i++;		
	if(checkTagExist(cmBlueColorantTag))
		i++;
	if(checkTagExist(cmRedTRCTag))
		i++;
	if(checkTagExist(cmGreenTRCTag))
		i++;
	if(checkTagExist(cmBlueTRCTag))
		i++;
	if(checkTagExist(cmMediaWhitePointTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;
		
	if(i == 9)	return 1;
	
	return 0;	
}	

//not been tested yet
long	CsFormat::_validinputCMYKProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;
	if(checkTagExist(cmAToB0Tag))
		i++;
	if(checkTagExist(cmMediaWhitePointTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 4)	return 1;
	
	return 0;	
}		

//not been tested yet
long	CsFormat::_validdisplayGrayProfile(void)
{
	return _validinputGrayProfile();
}	

//not been tested yet
long	CsFormat::_validdisplayRGBProfile(void)
{
	return _validinputRGBProfile();
}	

//not been tested yet
long	CsFormat::_validoutputGrayProfile(void)
{
	return _validinputGrayProfile();
}	

//not been tested yet
long	CsFormat::_validoutputRGBProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;
	if(checkTagExist(cmAToB0Tag))
		i++;
	if(checkTagExist(cmBToA0Tag))
		i++;
	if(checkTagExist(cmGamutTag))
		i++;
	if(checkTagExist(cmAToB1Tag))
		i++;
	if(checkTagExist(cmBToA1Tag))
		i++;
	if(checkTagExist(cmAToB2Tag))
		i++;
	if(checkTagExist(cmBToA2Tag))
		i++;
	if(checkTagExist(cmMediaWhitePointTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 10)	return 1;
	
	return 0;	
}	

long	CsFormat::_validoutputCMYKProfile(void)
{
	return _validoutputRGBProfile();
}	

long	CsFormat::_validlinkProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;
	if(checkTagExist(cmAToB0Tag))
		i++;
	if(checkTagExist(cmProfileSequenceDescTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 4)	return 1;
	
	return 0;	
}		

long	CsFormat::_validspaceProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;
	if(checkTagExist(cmAToB0Tag))
		i++;
	if(checkTagExist(cmBToA0Tag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 4)	return 1;
	
	return 0;	
}		

long	CsFormat::_validabstractProfile(void)
{
	long i = 0;
	
	if(checkTagExist(cmProfileDescriptionTag))
		i++;
	if(checkTagExist(cmAToB0Tag))
		i++;
	if(checkTagExist(cmMediaWhitePointTag))
		i++;
	if(checkTagExist(cmCopyrightTag))
		i++;

	if(i == 4)	return 1;
	
	return 0;	
}		
