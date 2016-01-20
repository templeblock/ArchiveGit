//implementation of tiffformat class

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Memory.h>
#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "tiffformat.h"
//Added by James and Peter
//end


#define  CHANGABLE_COUNT	0 //is the count changable

#ifndef PLUGIN

TagInfo tagInfo[] = {
	{ 0, 	(TIFFDataType)0, 	(TIFFDataType)0,  0, "dummyTag" },//0
	{ TIFFTAG_NEWSUBFILETYPE, TIFF_LONG, TIFF_LONG, 1, "NewSubFiletype"}, //1
    { TIFFTAG_IMAGEWIDTH, TIFF_LONG, TIFF_SHORT, 1, "ImageWidth" }, //2
    { TIFFTAG_IMAGELENGTH, TIFF_LONG, TIFF_SHORT, 1, "ImageLength" },//3
    { TIFFTAG_BITSPERSAMPLE, TIFF_SHORT, TIFF_SHORT, 0, "BitsPerSample" },//4 
    { TIFFTAG_COMPRESSION, TIFF_SHORT, TIFF_SHORT, 1, "Compression" },//5
    { TIFFTAG_PHOTOMETRIC, TIFF_SHORT, TIFF_SHORT, 1, "Photometric" },//6
    { TIFFTAG_FILLORDER, TIFF_SHORT, TIFF_SHORT, 1, "FillOrder" },//7
    { TIFFTAG_STRIPOFFSETS, TIFF_LONG, TIFF_SHORT, 0, "StripOffsets" },//8
    { TIFFTAG_ORIENTATION, TIFF_SHORT, TIFF_SHORT, 1, "Orientation" },//9
    { TIFFTAG_SAMPLESPERPIXEL, TIFF_SHORT, TIFF_SHORT, 1, "SamplesPerPixel" },//10
    { TIFFTAG_ROWSPERSTRIP, TIFF_LONG, TIFF_SHORT, 1, "RowsPerStrip" },//11
    { TIFFTAG_STRIPBYTECOUNTS, TIFF_LONG, TIFF_SHORT, 0, "StripByteCounts" },//12
    { TIFFTAG_XRESOLUTION, TIFF_RATIONAL, TIFF_RATIONAL, 1, "XResolution" },//13
    { TIFFTAG_YRESOLUTION, TIFF_RATIONAL, TIFF_RATIONAL, 1, "YResolution" },//14
    { TIFFTAG_PLANARCONFIG, TIFF_SHORT,	TIFF_SHORT, 1, "PlanarConfiguration" },//15
    { TIFFTAG_RESOLUTIONUNIT, TIFF_SHORT, TIFF_SHORT, 1, "ResolutionUnit" },//16
    { TIFFTAG_PREDICTOR, TIFF_SHORT, TIFF_SHORT, 1, "Predictor" },//17

    { TIFFTAG_TILEWIDTH, TIFF_LONG, TIFF_SHORT, 1, "TileWidth" },//18
    { TIFFTAG_TILELENGTH, TIFF_LONG, TIFF_SHORT, 1, "TileLength" },//19
    { TIFFTAG_TILEOFFSETS, TIFF_LONG, TIFF_LONG, 0, "TileOffsets" },//20
    { TIFFTAG_TILEBYTECOUNTS, TIFF_LONG, TIFF_SHORT, 0, "TileByteCounts" },//21
    
    { TIFFTAG_DOTRANGE, TIFF_SHORT, TIFF_BYTE, 0, "DotRange" },//22
    { TIFFTAG_MONACO, TIFF_BYTE, TIFF_BYTE, 0, "Monaco" },//23  
    { TIFFTAG_PHOTO1, TIFF_BYTE, TIFF_BYTE, 0, "Caption1" },//24  
    { TIFFTAG_PHOTO2, TIFF_LONG, TIFF_BYTE, 0, "Caption2" },//25  
};

// As a code resource initilazation to strings and malloc and free are not available

#else 

TagInfo tagInfo[25];

void place_taginfo_data(int i, u_int16	tag, TIFFDataType 	type1,TIFFDataType	type2, u_int32	count, char *name)
{
tagInfo[i].tag = tag;
tagInfo[i].type1 = type1;
tagInfo[i].type2 = type2;
tagInfo[i].count = count;
strcpy(tagInfo[i].name,name);
}

void initialize_taginfo(void)
{
place_taginfo_data(0, 0, 	0, 	0,  0, "dummyTag" );
place_taginfo_data(1, TIFFTAG_NEWSUBFILETYPE, TIFF_LONG, TIFF_LONG, 1, "NewSubFiletype");
place_taginfo_data(2, TIFFTAG_IMAGEWIDTH, TIFF_LONG, TIFF_SHORT, 1, "ImageWidth");
place_taginfo_data(3, TIFFTAG_IMAGELENGTH, TIFF_LONG, TIFF_SHORT, 1, "ImageLength");
place_taginfo_data(4, TIFFTAG_BITSPERSAMPLE, TIFF_SHORT, TIFF_SHORT, 0, "BitsPerSample");
place_taginfo_data(5, TIFFTAG_COMPRESSION, TIFF_SHORT, TIFF_SHORT, 1, "Compression");
place_taginfo_data(6, TIFFTAG_PHOTOMETRIC, TIFF_SHORT, TIFF_SHORT, 1, "Photometric");
place_taginfo_data(7, TIFFTAG_FILLORDER, TIFF_SHORT, TIFF_SHORT, 1, "FillOrder");
place_taginfo_data(8, TIFFTAG_STRIPOFFSETS, TIFF_LONG, TIFF_SHORT, 0, "StripOffsets");
place_taginfo_data(9, TIFFTAG_ORIENTATION, TIFF_SHORT, TIFF_SHORT, 1, "Orientation");
place_taginfo_data(10, TIFFTAG_SAMPLESPERPIXEL, TIFF_SHORT, TIFF_SHORT, 1, "SamplesPerPixel");
place_taginfo_data(11, TIFFTAG_ROWSPERSTRIP, TIFF_LONG, TIFF_SHORT, 1, "RowsPerStrip");
place_taginfo_data(12, TIFFTAG_STRIPBYTECOUNTS, TIFF_LONG, TIFF_SHORT, 0, "StripByteCounts");
place_taginfo_data(13, TIFFTAG_XRESOLUTION, TIFF_RATIONAL, TIFF_RATIONAL, 1, "XResolution");
place_taginfo_data(14, TIFFTAG_YRESOLUTION, TIFF_RATIONAL, TIFF_RATIONAL, 1, "YResolution");
place_taginfo_data(15, TIFFTAG_PLANARCONFIG, TIFF_SHORT,	TIFF_SHORT, 1, "PlanarConfiguration");
place_taginfo_data(16, TIFFTAG_RESOLUTIONUNIT, TIFF_SHORT, TIFF_SHORT, 1, "ResolutionUnit");
place_taginfo_data(17, TIFFTAG_PREDICTOR, TIFF_SHORT, TIFF_SHORT, 1, "Predictor");

place_taginfo_data(18, TIFFTAG_TILEWIDTH, TIFF_LONG, TIFF_SHORT, 1, "TileWidth");
place_taginfo_data(19, TIFFTAG_TILELENGTH, TIFF_LONG, TIFF_SHORT, 1, "TileLength");
place_taginfo_data(20, TIFFTAG_TILEOFFSETS, TIFF_LONG, TIFF_LONG, 0, "TileOffsets");
place_taginfo_data(21, TIFFTAG_TILEBYTECOUNTS, TIFF_LONG, TIFF_SHORT, 0, "TileByteCounts");

place_taginfo_data(22, TIFFTAG_DOTRANGE, TIFF_SHORT, TIFF_BYTE, 0, "DotRange");
place_taginfo_data(23, TIFFTAG_MONACO, TIFF_BYTE, TIFF_BYTE, 0, "Monaco");
place_taginfo_data(24, TIFFTAG_PHOTO1, TIFF_BYTE, TIFF_BYTE, 0, "Caption1");  
place_taginfo_data(25, TIFFTAG_PHOTO2, TIFF_LONG, TIFF_LONG, 0, "Caption2"); 
}

// The following is not included in the A4 libraries and must be included here

void *malloc(size_t the_size)
{
return NewPtr(the_size);
}

void free(void *thePtr)
{
DisposPtr(thePtr);
}

#endif


// note Monaco is temporaryly assigned to tag # 351
u_int16 tagIndex[] = {
	1,0,2,3,  4,5,0,0,  6,0,0,0,  7,0,0,0,  0,0,0,8,  9,0,0,10, 11,12,0,0,  13,14,15, 0,
	0,0,0,0,  0,0,0,0,  0,0,16,0, 0,0,0,0,  0,0,0,0,  0,0,0,0,   0, 0,0,0,   0, 0, 0,17,
	0,0,0,0,18,19,20,21, 0,0,0,0, 0,0,0,0,  0,0,22,0, 0,0,0,0,	 0, 0,0,0,   0, 0, 0, 0, 
	0,23	
	};

TiffFormat::TiffFormat(void){

	#ifdef PLUGIN
	initialize_taginfo();
	#endif

	_error = MCO_SUCCESS;
	setByteorder(TIFF_BIGENDIAN);
	setSwab(TIFF_SWAB_OFF);
	setDefaulttag();	
	//setNumofsupportedtags(16); 	//right now only support 16 tags
	setDiroffset(0);			//current end of diroffset
		
	//setStoreformat(TIFF_STORE_UNKNOWN);
	_storeformat = TIFF_STORE_UNKNOWN;
	//added for new version of plug and batch
	monacosize = 0;
	//end of modification

	captionsize1 = 0;
	captionsize2 = 0;
		
	setReuse(FALSE);		
}

TiffFormat::~TiffFormat(void)
{
	
	if(_tiffptr.stripoffset != NULL){
		free(_tiffptr.stripoffset);
	}
	if(_tiffptr.stripbytecount != NULL){
		free(_tiffptr.stripbytecount);
	}
	//added by peter for new plug and filter
	if(_tiffptr.monacofield != NULL){
		free(_tiffptr.monacofield);
	}
	//end
	
	if(_tiffptr.tileoffset != NULL){
		free(_tiffptr.tileoffset);
	}
	if(_tiffptr.tilebytecount != NULL){
		free(_tiffptr.tilebytecount);
	}
	
}

void TiffFormat::setDefaulttag(void)
{
	
	_tiffptr.newsubfiletype = NEWSUBFILETYPE_DEFAULT;
	_tiffptr.imagewidth = 0;
	_tiffptr.imagelength = 0;
	_tiffptr.bitspersample = BITSPERSAMPLE_DEFAULT;
	_tiffptr.compression = COMPRESSION_DEFAULT;
	_tiffptr.photometric = PHOTOMETRIC_DEFAULT;
	_tiffptr.fillorder = FILLORDER_DEFAULT;
	_tiffptr.stripoffset = NULL;
	_tiffptr.orientation = ORIENTATION_DEFAULT;
	_tiffptr.samplesperpixel = SAMPLESPERPIXEL_DEFAULT;
	_tiffptr.rowsperstrip = ROWSPERSTRIP_DEFAULT;
	_tiffptr.stripbytecount = NULL;
	_tiffptr.xresolution = 0.0;
	_tiffptr.yresolution = 0.0;
	_tiffptr.xresolutionnumerator = 0;
	_tiffptr.yresolutionnumerator = 0;
	_tiffptr.xresolutiondenominator = 0;
	_tiffptr.yresolutiondenominator = 0;
	_tiffptr.planarconfig = PLANARCONFIGRATION_DEFAULT;
	_tiffptr.resolutionunit = RESOLUTIONUNIT_DEFAULT;
	_tiffptr.predictor = PREDICTOR_DEFAULT;
	_tiffptr.inkset = 1;
	_tiffptr.dotrange0 = DOTRANGE0_DEFAULT;
	_tiffptr.dotrange1 = DOTRANGE1_DEFAULT;
	_tiffptr.tilewidth = 0;
	_tiffptr.tilelength = 0;
	_tiffptr.tileoffset = NULL;
	_tiffptr.tilebytecount = NULL;
	_tiffptr.stripsperimage = 0;
	_tiffptr.tilesperimage = 0;
	_tiffptr.nstrips = 0;		/* size of offset & bytecount arrays */
	
	//added by peter for new plug and filter
	_tiffptr.monacofield = NULL;
	//end

	//added by peter for photoshop caption
	_tiffptr.caption1 = NULL;
	//end

	//added by peter for photoshop caption
	_tiffptr.caption2 = NULL;
	//end
}		

void TiffFormat::setSwab(int16 s)
{
	swab = s;
}

void TiffFormat::setNumofsupportedtags(u_int16 num_of_tags)
{
	numofsupportedtags = num_of_tags;
}

void TiffFormat::setDiroffset(u_int32 offset)
{
	diroffset = offset;
}

McoStatus TiffFormat::setStoreformat(int16 storeformat)
{
	switch(_storeformat){
		case(TIFF_STORE_UNKNOWN):
		_storeformat = storeformat;
		break;
			
		case(TIFF_STORE_LINE):
		if(_storeformat != storeformat)
			return MCO_PARSE_TIFF_ERROR;
		break;
		
		case(TIFF_STORE_TILE):
		if(_storeformat != storeformat)
			return MCO_PARSE_TIFF_ERROR;
		break;
	}
				
	switch(_storeformat){
		case(TIFF_STORE_UNKNOWN):
		break;
		
		case(TIFF_STORE_LINE):
		setNumofsupportedtags(16);
		break;
		
		case(TIFF_STORE_TILE):
		setNumofsupportedtags(17);
		break;
	}	
	
	return MCO_SUCCESS;
}	

int16 TiffFormat::getSwab(void)
{
	return swab;
}

void TiffFormat::setByteorder(int16 b)
{
	byteorder = b;
}

int16 TiffFormat::getByteorder(void)
{
	return byteorder;
}	

u_int16 TiffFormat::getNumofsupportedtags(void)
{
	return numofsupportedtags;
}

u_int32 TiffFormat::getDiroffset(void)
{
	return diroffset;
}

McoStatus TiffFormat::getHeader(TIFFHEADERPTR tiffheader)
{	
	tiffheader->magic = getByteorder();
	tiffheader->version	= 42;
	tiffheader->diroff = 8;
	return MCO_SUCCESS;
} 
	
int16 TiffFormat::getStoreformat(void)
{
	return _storeformat;
}	

void TiffFormat::swabShort(u_int16 * s)
{
	char temp;
	char *head = (char*)s;
	
	temp = head[0];
	head[0] = head[1];
	head[1] = temp;
}

//normally swabbing
void TiffFormat::swabLong(u_int32 * l)
{
	char temp;
	char *head = (char*)l;

	temp = head[0];
	head[0] = head[3];
	head[3] = temp;
	temp = head[1];
	head[1] = head[2];
	head[2] = temp;
}

//swabbing for dir offset(See Tiff spec for detail)
//before swabbing		after swabbing
//-------------			-------------
//|0 |1 |2 |3 |		-->	|1 |0 |3 |2 |
//-------------			-------------
void TiffFormat::swabLongleftjusted(u_int32 * l)
{
	char temp;
	char *head = (char*)l;

	temp = head[0];
	head[0] = head[1];
	head[1] = temp;
	temp = head[2];
	head[2] = head[3];
	head[3] = temp;
}

//modified by Peter to support the Intel format
void TiffFormat::swabDir(TIFFDIRENTRYPTR tiffdir)
{
	swabShort(&(tiffdir->tag));
	swabShort(&(tiffdir->type));
	swabLong(&(tiffdir->count));
	
	switch(tiffdir->type){
		case(TIFF_BYTE):
		case(TIFF_ASCII):
		case(TIFF_SBYTE):
		if(tiffdir->count > 4)
			swabLong(&(tiffdir->offset));
		break;
				
		case(TIFF_SHORT):
		case(TIFF_SSHORT):
		if(tiffdir->count <= 2)
			swabLongleftjusted(&(tiffdir->offset));
		else
			swabLong(&(tiffdir->offset));
		break;

		case(TIFF_LONG):
		case(TIFF_SLONG):
		swabLong(&(tiffdir->offset));
		break;
		
		default:
		swabLong(&(tiffdir->offset));
		break;
	}	
}


McoStatus	TiffFormat::checkTag(TIFFDIRENTRYPTR tiffdir)
{
	u_int16 index;
	
	if( tiffdir->tag < 254 )
		return MCO_PARSE_TIFF_ERROR;

//added for photoshop caption
	if(	tiffdir->tag == 34377)
		index = 24;
	else if( tiffdir->tag == 33723)
		index = 25;
	else{
		if( tiffdir->tag - 254 > sizeof(tagIndex)/sizeof(u_int16) ) //unknown tag,ignore
			return 	MCO_SUCCESS;
		index = tagIndex[tiffdir->tag - 254 ];
	}

	if(index == 0)
		return MCO_SUCCESS;	
	if(tiffdir->type != tagInfo[index].type1 && tiffdir->type != tagInfo[index].type2)
		return MCO_PARSE_TIFF_ERROR;
	if(tagInfo[index].count == CHANGABLE_COUNT )
		return MCO_SUCCESS;
	if(tiffdir->count != tagInfo[index].count)
		return MCO_PARSE_TIFF_ERROR;	
		
	return MCO_SUCCESS;
}

McoStatus TiffFormat::adjustLow(TIFFDIRENTRYPTR tiffdir)
{	
	if( tiffdir->type == TIFF_LONG )
		return MCO_SUCCESS;
	if( tiffdir->type == TIFF_SHORT )
		tiffdir->offset >>= 16;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::adjustHigh(TIFFDIRENTRYPTR tiffdir)
{
	if( tiffdir->type == TIFF_LONG )
		return MCO_SUCCESS;
	if( tiffdir->type == TIFF_SHORT )
		tiffdir->offset <<= 16;
	return 	MCO_SUCCESS;
}	

McoStatus TiffFormat::checkTiff(TIFFDIRECTORYPTR tiffptr)
{
	if( tiffptr->photometric == PHOTOMETRIC_MINISWHITE || 
		tiffptr->photometric == PHOTOMETRIC_MINISBLACK )
		return checkGrayscale(tiffptr);
		
	if( tiffptr->photometric == PHOTOMETRIC_RGB )
		return checkRGB(tiffptr);

	if( tiffptr->photometric == PHOTOMETRIC_CMYK )
		return checkCMYK(tiffptr);
	
	if( tiffptr->photometric == PHOTOMETRIC_LAB )
		return checkLAB(tiffptr);
		
	//other photometric are not supported by this version
	return MCO_PARSE_TIFF_ERROR;	
}
			
McoStatus TiffFormat::checkGrayscale(TIFFDIRECTORYPTR tiffptr)
{
	if( tiffptr->bitspersample != 4 && tiffptr->bitspersample != 8 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagewidth == 0 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagelength == 0 )
		return MCO_PARSE_TIFF_ERROR;
		
	//LZW not implemented	
	if( tiffptr->compression != 1 )
		return MCO_COMPRESSION_TIFF_NOTSUPPORTED;
	if( tiffptr->resolutionunit != 1 &&
		tiffptr->resolutionunit != 2 &&
		tiffptr->resolutionunit != 3 )
		return MCO_PARSE_TIFF_ERROR;
	
	return MCO_SUCCESS;
}


McoStatus TiffFormat::checkRGB(TIFFDIRECTORYPTR tiffptr)
{
	//only implement 888 
	if( tiffptr->bitspersample != 8 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagewidth == 0 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagelength == 0 )
		return MCO_PARSE_TIFF_ERROR;
		
	//LZW not implemented 	
	if( tiffptr->compression != 1 )
		return MCO_COMPRESSION_TIFF_NOTSUPPORTED;	
	if( tiffptr->samplesperpixel < 3 )
		return MCO_PARSE_TIFF_ERROR;	
	if( tiffptr->resolutionunit != 1 &&
		tiffptr->resolutionunit != 2 &&
		tiffptr->resolutionunit != 3 )
		return MCO_PARSE_TIFF_ERROR;
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::checkCMYK(TIFFDIRECTORYPTR tiffptr)
{
	//only implement 888 
	if( tiffptr->bitspersample != 8 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagewidth == 0 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagelength == 0 )
		return MCO_PARSE_TIFF_ERROR;
		
	//LZW not implemented 	
	if( tiffptr->compression != 1 && tiffptr->compression != 32773 )
		return MCO_COMPRESSION_TIFF_NOTSUPPORTED;	
	if( tiffptr->samplesperpixel < 4 )
		return MCO_PARSE_TIFF_ERROR;	
	if( tiffptr->resolutionunit != 1 &&
		tiffptr->resolutionunit != 2 &&
		tiffptr->resolutionunit != 3 )
		return MCO_PARSE_TIFF_ERROR;
	
	return MCO_SUCCESS;
}


McoStatus TiffFormat::checkLAB(TIFFDIRECTORYPTR tiffptr)
{
	//only implement 888 
	if( tiffptr->bitspersample != 8 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagewidth == 0 )
		return MCO_PARSE_TIFF_ERROR;
	if( tiffptr->imagelength == 0 )
		return MCO_PARSE_TIFF_ERROR;
		
	//LZW not implemented 	
	if( tiffptr->compression != 1 && tiffptr->compression != 32773 )
		return MCO_COMPRESSION_TIFF_NOTSUPPORTED;
		
	//1 implies L for monochrome data, 3 implies Lab		
	if( tiffptr->samplesperpixel != 1 && tiffptr->samplesperpixel != 3)
		return MCO_PARSE_TIFF_ERROR;	
	if( tiffptr->resolutionunit != 1 &&
		tiffptr->resolutionunit != 2 &&
		tiffptr->resolutionunit != 3 )
		return MCO_PARSE_TIFF_ERROR;
	
	return MCO_SUCCESS;
}


//modified by Peter to support stripsperimage > 1
McoStatus TiffFormat::setStripsperimage(u_int32 stripsperimage)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	if( stripsperimage < 1)
		return MCO_PARSE_TIFF_ERROR;

	_tiffptr.stripsperimage = stripsperimage;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setTilesperimage(void)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if(_tiffptr.tilewidth == 0 || _tiffptr.tilelength == 0)
		return MCO_PARSE_TIFF_ERROR;
		
		
	_tiffptr.tilesperimage =((_tiffptr.imagewidth + _tiffptr.tilewidth -1)/_tiffptr.tilewidth)*
							((_tiffptr.imagelength + _tiffptr.tilelength -1)/_tiffptr.tilelength);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setPredictor(u_int16 predictor)
{
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.predictor = predictor;
	
	return MCO_SUCCESS;
}

//add for new version of plug and batch
McoStatus TiffFormat::setMonaco(char *monaco, int32 size)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		 
	if(_tiffptr.monacofield != NULL )
		free(_tiffptr.monacofield);

	monacosize = size;
	
	_tiffptr.monacofield = (char *)malloc(monacosize*sizeof(u_int32));
	if(_tiffptr.monacofield == NULL ){
		monacosize = 0; return MCO_MEM_ALLOC_ERROR; }

	memcpy((char*)_tiffptr.monacofield, monaco, monacosize);
	
	return MCO_SUCCESS;
}
	
//add for photoshop caption1
McoStatus TiffFormat::setCaption1(char *caption, int32 size)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		 
	if(_tiffptr.caption1 != NULL )
		free(_tiffptr.caption1);

	captionsize1 = size;
	
	_tiffptr.caption1 = (char *)malloc(captionsize1*sizeof(u_int32));
	if(_tiffptr.caption1 == NULL ){
		captionsize1 = 0; return MCO_MEM_ALLOC_ERROR; }

	memcpy((char*)_tiffptr.caption1, caption, captionsize1);
	
	return MCO_SUCCESS;
}

//add for photoshop caption2
//fix on 8/4, captionsize2 is byte size, not long size 
McoStatus TiffFormat::setCaption2(char *caption, int32 size)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		 
	if(_tiffptr.caption2 != NULL )
		free(_tiffptr.caption2);

	captionsize2 = size;
	
	//_tiffptr.caption2 = (char *)malloc(captionsize2*sizeof(long));
	_tiffptr.caption2 = (char *)malloc(captionsize2);
	if(_tiffptr.caption2 == NULL ){
		captionsize2 = 0; return MCO_MEM_ALLOC_ERROR; }

	//memcpy((char*)_tiffptr.caption2, caption, captionsize2*sizeof(long));
	memcpy((char*)_tiffptr.caption2, caption, captionsize2);

	return MCO_SUCCESS;
}

	
//modified by Peter to support stripsperimage > 1	
/*
McoStatus TiffFormat::setStripbytecount(void)
{
	TIFFDIRECTORYPTR tiffptr;
	McoStatus	status;
	int32	imagesize;
	u_int32 i;
	u_int32 stripsperimage, rowsperstrip, width;
	char *	imageptr;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if( (imagesize = getDatasize()) == 0 ) //0 size image cannot write out
		return MCO_WRITE_TIFF_ERROR;
	
	getStripsperimage(&stripsperimage);
	
	if(_tiffptr.stripbytecount != NULL )
		free(_tiffptr.stripbytecount);
	
	//just implement the simplest case	
	_tiffptr.stripbytecount = (u_int32 *)malloc(stripsperimage*sizeof(u_int32));
	if(_tiffptr.stripbytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }

	//modified by Peter to support stripsperimage > 1
	status = getRowsperstrip(&rowsperstrip);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;

	status = getWidth(&width);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;
		 
	for( i = 0; i < stripsperimage; i++ )
		*(_tiffptr.stripbytecount + i) = rowsperstrip*width;
		
	//the last one may not be correct, compensate
	*(_tiffptr.stripbytecount + stripsperimage - 1) = 
		imagesize - rowsperstrip*width*(stripsperimage - 1);	
	//end of modification 		
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setStripoffset(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	u_int32 stripsperimage;
	int32 i;
	int32 offset;

	getStripsperimage(&stripsperimage);

	if(tiffptr->stripoffset != NULL )
		free(tiffptr->stripoffset);
	
	//just implement the simplest case	
	tiffptr->stripoffset = (u_int32 *)malloc(stripsperimage*sizeof(u_int32));
	if(tiffptr->stripbytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }
	for( i = 0; i < stripsperimage; i++ ) {
		if( tiffdir->type == TIFF_SHORT ) 
			*(tiffptr->stripoffset + i) = (tiffdir->offset >> 16 );
		else
			*(tiffptr->stripoffset + i) = tiffdir->offset;
	}			
	return MCO_SUCCESS;
}
*/

/*
//modified by Peter to support stripsperimage > 1	
McoStatus TiffFormat::setTilebytecount(void)
{
	McoStatus	status;
	int32	imagesize;
	u_int32 i;
	u_int32 tilesperimage, tilewidth, tilelength;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if( (imagesize = getDatasize()) == 0 ) //0 size image cannot write out
		return MCO_WRITE_TIFF_ERROR;
		
	getTilesperimage(&tilesperimage);
	
	if(_tiffptr.tilebytecount != NULL )
		free(_tiffptr.tilebytecount);
	
	//just implement the simplest case	
	_tiffptr.tilebytecount = (u_int32 *)malloc(tilesperimage*sizeof(u_int32));
	if(_tiffptr.tilebytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }

	status = getTilewidth(&tilewidth);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;

	status = getTilelength(&tilelength);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;
		 
	for( i = 0; i < tilesperimage; i++ )
		*(_tiffptr.tilebytecount + i) = tilewidth*tilelength;
	
	return MCO_SUCCESS;
}
	
McoStatus TiffFormat::setTileoffset(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	u_int32 tilesperimage;
	int32 i;
	int32 offset;

	getTilesperimage(&tilesperimage);

	if(tiffptr->tileoffset != NULL )
		free(tiffptr->tileoffset);
	
	//just implement the simplest case	
	tiffptr->tileoffset = (u_int32 *)malloc(tilesperimage*sizeof(u_int32));
	if(tiffptr->tilebytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }
	for( i = 0; i < tilesperimage; i++ ) {
		if( tiffdir->type == TIFF_SHORT ) 
			*(tiffptr->tileoffset + i) = (tiffdir->offset >> 16 );
		else
			*(tiffptr->tileoffset + i) = tiffdir->offset;
	}			
	return MCO_SUCCESS;
}
*/

McoStatus TiffFormat::getStripsperimage(u_int32 *stripsperimage)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	*stripsperimage = _tiffptr.stripsperimage;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getTilesperimage(u_int32 *tilesperimage)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	*tilesperimage = _tiffptr.tilesperimage;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getPredictor(u_int16 * predictor)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	*predictor = (unsigned short)_tiffptr.stripsperimage;
	return MCO_SUCCESS;
}
	
McoStatus TiffFormat::fetchBitspersample(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int16 value_array[16]; //assume the value of samplesperpixel is < 16
	u_int16 num_of_value;
	u_int16	value;
	int16	i;
	
	if( tiffdir->count != tiffptr->samplesperpixel)
		return MCO_PARSE_TIFF_ERROR;
	if( tiffdir->count == 1 ) {
		adjustLow(tiffdir);
		return setBitspersample(tiffdir->offset); }  //Unlocks tiffHandle
	
	//bitspersample = 2 is not implemented in this version
		
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) return MCO_PARSE_TIFF_ERROR;

	status = relRead(tiffdir->count * sizeof(u_int16), (char *)value_array);
	if(status != MCO_SUCCESS) return MCO_PARSE_TIFF_ERROR;

	//modified by Peter to support Intel format
	if(getSwab() == TIFF_SWAB_ON)
		for( i = 0; i < (int16)tiffdir->count; i++ )
			swabShort(&value_array[i]);
	//end of modification		

	for( i = 0, value = value_array[0]; i < (int16)tiffdir->count; i++ )
		if( value != value_array[i] )
			return MCO_PARSE_TIFF_ERROR;
	
	return setBitspersample(value); 
}

//stop here for a joggling
McoStatus TiffFormat::fetchStripoffset(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int32	stripsperimage;
	u_int16 *short_value_array;
	u_int32	temp, i;

	getStripsperimage(&stripsperimage);	//Should be locked

	//fix on 8/4
	//planar = 2 means RRRRR....GGGGG....BBBBB
	if( stripsperimage != tiffdir->count ){
		if(tiffptr->planarconfig == 2){
			if(tiffptr->samplesperpixel*stripsperimage != tiffdir->count)
				return MCO_PARSE_TIFF_ERROR;
		}
		else
			return MCO_PARSE_TIFF_ERROR;
	}


	if( tiffptr->stripoffset != NULL )
		free(tiffptr->stripoffset);
		
	tiffptr->stripoffset = (u_int32 *)malloc(sizeof(u_int32)* tiffdir->count);
	if( tiffptr->stripoffset == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	if( tiffdir->count == 1 ) {
		adjustLow(tiffdir);
		*(tiffptr->stripoffset) = tiffdir->offset;
		return MCO_SUCCESS;
	}
	
	if( tiffdir->count == 2 && tiffdir->type == TIFF_SHORT ) {
		*(tiffptr->stripoffset) = tiffdir->offset >> 16;
		temp = tiffdir->offset << 16;
		*(tiffptr->stripoffset + 1) = temp >> 16;
		return MCO_SUCCESS;
	}

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->stripoffset); tiffptr->stripoffset = NULL; return MCO_PARSE_TIFF_ERROR; }
	
	if( tiffdir->type == TIFF_SHORT ){
		short_value_array = (u_int16 *)malloc(sizeof(u_int16)* tiffdir->count);
		if( short_value_array == NULL) {
			free(tiffptr->stripoffset); tiffptr->stripoffset = NULL;
			return MCO_MEM_ALLOC_ERROR; }
		
		status = relRead(tiffdir->count * sizeof(u_int16), (char *)short_value_array);
		if(status != MCO_SUCCESS) {
			free(short_value_array); free(tiffptr->stripoffset); 
			tiffptr->stripoffset = NULL; return MCO_PARSE_TIFF_ERROR; }
		
		for( i = 0; i < tiffdir->count; i++ )
			*tiffptr->stripoffset++ = (u_int32)*short_value_array++;
		free(short_value_array);
		return MCO_SUCCESS;
	}
	
	status = relRead(tiffdir->count * sizeof(u_int32), (char *)tiffptr->stripoffset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->stripoffset);tiffptr->stripoffset = NULL; return MCO_PARSE_TIFF_ERROR; }

	if(getSwab() == TIFF_SWAB_ON){ 
		for( i = 0; i < tiffdir->count; i++ )
			swabLong(tiffptr->stripoffset+i);
	}		
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::fetchStripbytecounts(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int32	stripsperimage;
	u_int16 *short_value_array;
	u_int32	temp, i;
	
	getStripsperimage(&stripsperimage);

	//fix on 8/4
	//planar = 2 means RRRRR....GGGGG....BBBBB
	if( stripsperimage != tiffdir->count ){
		if(tiffptr->planarconfig == 2){
			if(tiffptr->samplesperpixel*stripsperimage != tiffdir->count)
				return MCO_PARSE_TIFF_ERROR;
		}
		else
			return MCO_PARSE_TIFF_ERROR;
	}

	if( tiffptr->stripbytecount != NULL )
		free(tiffptr->stripbytecount);
		
	tiffptr->stripbytecount = (u_int32 *)malloc(sizeof(u_int32)* tiffdir->count);
	if( tiffptr->stripbytecount == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	if( tiffdir->count == 1 ) {
		adjustLow(tiffdir);
		*(tiffptr->stripbytecount) = tiffdir->offset;
		return MCO_SUCCESS;
	}
	
	if( tiffdir->count == 2 && tiffdir->type == TIFF_SHORT ) {
		*(tiffptr->stripbytecount) = tiffdir->offset >> 16;
		temp = tiffdir->offset << 16;
		*(tiffptr->stripbytecount + 1) = temp >> 16;
		return MCO_SUCCESS;
	}

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->stripbytecount); tiffptr->stripbytecount = NULL; return MCO_PARSE_TIFF_ERROR; }
	
	if( tiffdir->type == TIFF_SHORT ){
		short_value_array = (u_int16 *)malloc(sizeof(u_int16)* tiffdir->count);
		if( short_value_array == NULL) {
			free(tiffptr->stripbytecount); tiffptr->stripbytecount = NULL; 
			return MCO_MEM_ALLOC_ERROR; }
		
		status = relRead(tiffdir->count * sizeof(u_int16), (char *)short_value_array);
		if(status != MCO_SUCCESS) {
			free(short_value_array); free(tiffptr->stripbytecount); 
			tiffptr->stripbytecount = NULL; return MCO_PARSE_TIFF_ERROR; }
		
		for( i = 0; i < tiffdir->count; i++ )
			*tiffptr->stripbytecount++ = (u_int32)*short_value_array++;
		free(short_value_array);
		return MCO_SUCCESS;
	}
	
	status = relRead(tiffdir->count * sizeof(u_int32), (char *)tiffptr->stripbytecount);
	if(status != MCO_SUCCESS) {
		free(tiffptr->stripbytecount);tiffptr->stripbytecount = NULL; 
		return MCO_PARSE_TIFF_ERROR; }

	if(getSwab() == TIFF_SWAB_ON){ 
		for( i = 0; i < tiffdir->count; i++ )
			swabLong(tiffptr->stripbytecount+i);
	}		
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::fetchXresolution(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;

	if( tiffdir->count != 1 )
		return MCO_PARSE_TIFF_ERROR;	

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) 
		return MCO_PARSE_TIFF_ERROR;

	status = relRead( sizeof(u_int32), (char *)&(tiffptr->xresolutionnumerator) );
	if(status != MCO_SUCCESS)
		return MCO_PARSE_TIFF_ERROR;

	status = relRead( sizeof(u_int32), (char *)&(tiffptr->xresolutiondenominator) );
	if(status != MCO_SUCCESS)
		return MCO_PARSE_TIFF_ERROR;

	//modified by Peter to support Intel format
	if(getSwab() == TIFF_SWAB_ON){
		swabLong(&(tiffptr->xresolutionnumerator));
		swabLong(&(tiffptr->xresolutiondenominator));
	}
	//end of modification
		
	return MCO_SUCCESS;
}	

McoStatus TiffFormat::fetchYresolution(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;

	if( tiffdir->count != 1 )
		return MCO_PARSE_TIFF_ERROR;	

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) 
		return MCO_PARSE_TIFF_ERROR;

	status = relRead( sizeof(u_int32), (char *)&(tiffptr->yresolutionnumerator) );
	if(status != MCO_SUCCESS)
		return MCO_PARSE_TIFF_ERROR;

	status = relRead( sizeof(u_int32), (char *)&(tiffptr->yresolutiondenominator) );
	if(status != MCO_SUCCESS)
		return MCO_PARSE_TIFF_ERROR;

	//modified by Peter to support Intel format
	if(getSwab() == TIFF_SWAB_ON){
		swabLong(&(tiffptr->yresolutionnumerator));
		swabLong(&(tiffptr->yresolutiondenominator));
	}
	//end of modification
		
	return MCO_SUCCESS;
}	


McoStatus TiffFormat::fetchTileoffset(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int32	tilesperimage;
	u_int16 *short_value_array;
	u_int32	temp, i;

	getTilesperimage(&tilesperimage);	//Should be locked
	if( tilesperimage != tiffdir->count )
		return MCO_PARSE_TIFF_ERROR;

	if( tiffptr->tileoffset != NULL )
		free(tiffptr->tileoffset);
		
	tiffptr->tileoffset = (u_int32 *)malloc(sizeof(u_int32)* tiffdir->count);
	if( tiffptr->tileoffset == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	if( tiffdir->count == 1 ) {
		adjustLow(tiffdir);
		*(tiffptr->tileoffset) = tiffdir->offset;
		return MCO_SUCCESS;
	}
	
	if( tiffdir->count == 2 && tiffdir->type == TIFF_SHORT ) {
		*(tiffptr->tileoffset) = tiffdir->offset >> 16;
		temp = tiffdir->offset << 16;
		*(tiffptr->tileoffset + 1) = temp >> 16;
		return MCO_SUCCESS;
	}

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->tileoffset); tiffptr->tileoffset = NULL; return MCO_PARSE_TIFF_ERROR; }
	
	if( tiffdir->type == TIFF_SHORT ){
		short_value_array = (u_int16 *)malloc(sizeof(u_int16)* tiffdir->count);
		if( short_value_array == NULL) {
			free(tiffptr->tileoffset); tiffptr->tileoffset = NULL;
			return MCO_MEM_ALLOC_ERROR; }
		
		status = relRead(tiffdir->count * sizeof(u_int16), (char *)short_value_array);
		if(status != MCO_SUCCESS) {
			free(short_value_array); free(tiffptr->tileoffset); 
			tiffptr->tileoffset = NULL; return MCO_PARSE_TIFF_ERROR; }
		
		for( i = 0; i < tiffdir->count; i++ )
			*tiffptr->tileoffset++ = (u_int32)*short_value_array++;
		free(short_value_array);
		return MCO_SUCCESS;
	}
	
	status = relRead(tiffdir->count * sizeof(u_int32), (char *)tiffptr->tileoffset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->tileoffset);tiffptr->tileoffset = NULL; return MCO_PARSE_TIFF_ERROR; }
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::fetchTilebytecounts(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int32	tilesperimage;
	u_int16 *short_value_array;
	u_int32	temp, i;
	
	getTilesperimage(&tilesperimage);
	if( tilesperimage != tiffdir->count )
		return MCO_PARSE_TIFF_ERROR;

	if( tiffptr->tilebytecount != NULL )
		free(tiffptr->tilebytecount);
		
	tiffptr->tilebytecount = (u_int32 *)malloc(sizeof(u_int32)* tiffdir->count);
	if( tiffptr->tilebytecount == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	if( tiffdir->count == 1 ) {
		adjustLow(tiffdir);
		*(tiffptr->tilebytecount) = tiffdir->offset;
		return MCO_SUCCESS;
	}
	
	if( tiffdir->count == 2 && tiffdir->type == TIFF_SHORT ) {
		*(tiffptr->tilebytecount) = tiffdir->offset >> 16;
		temp = tiffdir->offset << 16;
		*(tiffptr->tilebytecount + 1) = temp >> 16;
		return MCO_SUCCESS;
	}

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->tilebytecount); tiffptr->tilebytecount = NULL; return MCO_PARSE_TIFF_ERROR; }
	
	if( tiffdir->type == TIFF_SHORT ){
		short_value_array = (u_int16 *)malloc(sizeof(u_int16)* tiffdir->count);
		if( short_value_array == NULL) {
			free(tiffptr->tilebytecount); tiffptr->tilebytecount = NULL; 
			return MCO_MEM_ALLOC_ERROR; }
		
		status = relRead(tiffdir->count * sizeof(u_int16), (char *)short_value_array);
		if(status != MCO_SUCCESS) {
			free(short_value_array); free(tiffptr->tilebytecount); 
			tiffptr->tilebytecount = NULL; return MCO_PARSE_TIFF_ERROR; }
		
		for( i = 0; i < tiffdir->count; i++ )
			*tiffptr->tilebytecount++ = (u_int32)*short_value_array++;
		free(short_value_array);
		return MCO_SUCCESS;
	}
	
	status = relRead(tiffdir->count * sizeof(u_int32), (char *)tiffptr->tilebytecount);
	if(status != MCO_SUCCESS) {
		free(tiffptr->tilebytecount);tiffptr->tilebytecount = NULL; 
		return MCO_PARSE_TIFF_ERROR; }
	
	return MCO_SUCCESS;
}


McoStatus TiffFormat::fetchMonaco(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	
	if(tiffdir->count <= 0)
		return MCO_SUCCESS;
		
	tiffptr->monacofield = (char *)malloc(sizeof(char)* tiffdir->count);
	if( tiffptr->monacofield == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->monacofield); tiffptr->monacofield = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	status = relRead( tiffdir->count, (char *)(tiffptr->monacofield) );
	if(status != MCO_SUCCESS){
		free(tiffptr->monacofield); tiffptr->monacofield = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	monacosize = tiffdir->count;

	return MCO_SUCCESS;
}	

//added for photoshop caption1
McoStatus TiffFormat::fetchCaption1(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	
	if(tiffdir->count <= 0)
		return MCO_SUCCESS;
		
	tiffptr->caption1 = (char *)malloc(sizeof(char)* tiffdir->count);
	if( tiffptr->caption1 == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->caption1); tiffptr->caption1 = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	status = relRead( tiffdir->count, (char *)(tiffptr->caption1) );
	if(status != MCO_SUCCESS){
		free(tiffptr->caption1); tiffptr->caption1 = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	captionsize1 = tiffdir->count;

	return MCO_SUCCESS;
}	

//added for photoshop caption2
//new fix by Peter on 8/4, captionsize2 is byte size, not long size
McoStatus TiffFormat::fetchCaption2(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	long	count;
	
	if(tiffdir->count <= 0)
		return MCO_SUCCESS;
	
	if(tiffdir->type == TIFF_BYTE){
		count = tiffdir->count;
		tiffptr->caption2 = (char *)malloc(tiffdir->count);
		if( tiffptr->caption2 == NULL)
			return MCO_MEM_ALLOC_ERROR;
	}
	else if(tiffdir->type == TIFF_LONG){	
		count = tiffdir->count*sizeof(long);
		tiffptr->caption2 = (char *)malloc(tiffdir->count*sizeof(long));
		if( tiffptr->caption2 == NULL)
			return MCO_MEM_ALLOC_ERROR;
	}
	else
		return MCO_PARSE_TIFF_ERROR;

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) {
		free(tiffptr->caption2); tiffptr->caption2 = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	status = relRead(count, (char *)(tiffptr->caption2) );
	if(status != MCO_SUCCESS){
		free(tiffptr->caption2); tiffptr->caption2 = NULL;
		return MCO_PARSE_TIFF_ERROR;
	}
	
	//captionsize2 = tiffdir->count;
	captionsize2 = count;

	return MCO_SUCCESS;
}	

// not been used
McoStatus 	TiffFormat::fetchImage(TIFFDIRECTORYPTR tiffptr)
{
	u_int32 i;
	u_int32 total_size = 0;
	char * 	imageptr;
	McoStatus status;
	
	//the last strip may not has the full strip size
	for(i = 0 ; i < tiffptr->stripsperimage; i++ )
		total_size += *(tiffptr->stripbytecount + i);
	
	setDatasize(total_size);
	
	if( image_dataHandle != NULL )
		McoDeleteHandle(image_dataHandle);
	image_dataHandle = McoNewHandle(getDatasize());
	if( image_dataHandle == NULL ) {
		setDatasize(0); return MCO_MEM_ALLOC_ERROR; }
	
	imageptr = (char *)McoLockHandle(image_dataHandle);
	
	for( i = 0; i < tiffptr->stripsperimage; i++ ) {
		status = setFilePosition(SEEK_SET, *(tiffptr->stripoffset + i));
		if(status != MCO_SUCCESS) {
			setDatasize(0); McoDeleteHandle(image_dataHandle); return status; }
		status = relRead(sizeof(char)*(*(tiffptr->stripbytecount +i)), (char *)imageptr);
		if(status != MCO_SUCCESS) {
			setDatasize(0); McoDeleteHandle(image_dataHandle); return status; }
	}
	
	McoUnlockHandle(image_dataHandle);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::prepareRead(void)
{
	TIFFHEADER 			tiffheader;
	TIFFDIRENTRYPTR		dircurrent, dirlast, dirarray;
	TIFFDIRENTRYPTR		dirbitspersample, dirstripoffset, dirstripbytecounts;
	TIFFDIRENTRYPTR		dirxresolution, diryresolution;
	TIFFDIRENTRYPTR		dirtileoffset, dirtilebytecounts;

	//added for new version of plug and batch	
	TIFFDIRENTRYPTR		dirmonaco = NULL;
	//end of add

	//added for photoshop caption1
	TIFFDIRENTRYPTR		dircaption1 = NULL;
	//end

	//added for photoshop caption1
	TIFFDIRENTRYPTR		dircaption2 = NULL;
	//end
	
	McoHandle			tiffdirHandle;
	u_int16				num_of_dir;
	int32				next_diroff;
	u_int32   			imagesize;
	McoStatus 			status;
	unsigned char 		pascal_length;
	unsigned int  		resource_name_len;
	u_int32 			resource_loop;
	u_int16				*compressionptr;
	int32				diroff;
	char 				*imageptr;
	u_int32 			i;
	u_int32 			total_size = 0;
	TIFFDIRENTRY		fakestripbytedir;
	u_int32				rpstrip;	//fix bug for older tiff
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	status = setFilePosition(SEEK_SET, 0);
	if(status != MCO_SUCCESS) return MCO_NOT_TIFF_FILE;
	
	status = relRead(2, (char *)&(tiffheader.magic));
	if(status != MCO_SUCCESS)
		return MCO_NOT_TIFF_FILE;
			
	if( tiffheader.magic != TIFF_BIGENDIAN && tiffheader.magic != TIFF_LITTLEENDIAN )
		return MCO_NOT_TIFF_FILE;

	if( tiffheader.magic != getByteorder())
		setSwab(TIFF_SWAB_ON);
		
	status = relRead(2, (char *)&(tiffheader.version));
	if(status != MCO_SUCCESS)
		return MCO_NOT_TIFF_FILE;
	
	if(getSwab() == TIFF_SWAB_ON) 
		swabShort(&(tiffheader.version));
			
	if( tiffheader.version != TIFF_VERSION ) 
		return MCO_NOT_TIFF_FILE;

	status = relRead(4, (char *)&(tiffheader.diroff));
	if(status != MCO_SUCCESS) 
		return MCO_PARSE_TIFF_ERROR;
	
	if(getSwab() == TIFF_SWAB_ON) 
		swabLong(&(tiffheader.diroff));
	
	diroff = tiffheader.diroff;
				
	status = setFilePosition(SEEK_SET, diroff);
	if(status != MCO_SUCCESS) 
		return MCO_PARSE_TIFF_ERROR;	

	status = relRead(2, (char *)&(num_of_dir));
	if(status != MCO_SUCCESS)
		return MCO_PARSE_TIFF_ERROR;	
	
	if(getSwab() == TIFF_SWAB_ON) 
		swabShort(&(num_of_dir));

	tiffdirHandle = McoNewHandle(num_of_dir * sizeof(TIFFDIRENTRY));
	if( tiffdirHandle == NULL)
		return MCO_MEM_ALLOC_ERROR;
	
	dirarray = (TIFFDIRENTRYPTR)McoLockHandle(tiffdirHandle);

	status = relRead(num_of_dir * sizeof(TIFFDIRENTRY), (char *)dirarray);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle); return MCO_PARSE_TIFF_ERROR; }

	status = relRead(4, (char *)&(next_diroff));
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle); return MCO_PARSE_TIFF_ERROR; }

	dircurrent = dirlast = dirarray;
	//start of the loop
	for( i = 0 ; i < num_of_dir; i++, dircurrent++) {

	if(getSwab() == TIFF_SWAB_ON) 
		swabDir(dircurrent);		
	
//modified on 1/26 because of problems of tag ordering of old tiff format
/*
	if(dircurrent->tag <= 32768 && dircurrent->tag < dirlast->tag) {
		McoDeleteHandle(tiffdirHandle); return MCO_PARSE_TIFF_ERROR; }
*/
//end of modification

	status = checkTag(dircurrent);
	if(status != MCO_SUCCESS ){
	McoDeleteHandle(tiffdirHandle); return status; }
		
		
	switch(dircurrent->tag) {
		case(TIFFTAG_IMAGEWIDTH):
			adjustLow(dircurrent);
			setWidth(dircurrent->offset);  
			break;
			
		case(TIFFTAG_IMAGELENGTH):
			adjustLow(dircurrent);
			setLength(dircurrent->offset); 
			break;
			
		case(TIFFTAG_BITSPERSAMPLE):
			dirbitspersample = dircurrent;
			break;
		
		case(TIFFTAG_COMPRESSION):
			adjustLow(dircurrent);				
			setCompression((u_int16)dircurrent->offset); 
			break;
		
		case(TIFFTAG_PHOTOMETRIC):
			adjustLow(dircurrent);		
			setPhotometric((u_int16)dircurrent->offset);
			//set default for samplesperpixel
			switch(dircurrent->offset){
				case(PHOTOMETRIC_MINISWHITE):
				case(PHOTOMETRIC_MINISBLACK):
				_tiffptr.samplesperpixel = 1;
				break;
				
				default:
				break;
			}	
			break;
		
		case(TIFFTAG_FILLORDER):
			adjustLow(dircurrent);		
			setFillorder((u_int16)dircurrent->offset); 
			break;
		
		case(TIFFTAG_STRIPOFFSETS):
			dirstripoffset = dircurrent;
			break;
		
		case(TIFFTAG_ORIENTATION):
			adjustLow(dircurrent);		
			setOrientation((u_int16)dircurrent->offset); 
			break;
		
		case(TIFFTAG_SAMPLESPERPIXEL):
			adjustLow(dircurrent);
			setSamplesperpixel((unsigned short)dircurrent->offset);  
			break;
		
		case(TIFFTAG_ROWSPERSTRIP):
			adjustLow(dircurrent);		
			setRowsperstrip(dircurrent->offset); 
			break;
		
		case(TIFFTAG_STRIPBYTECOUNTS):
			dirstripbytecounts = dircurrent;
			break;
		
		case(TIFFTAG_XRESOLUTION):
			dirxresolution = dircurrent;
			break;
		
		case(TIFFTAG_YRESOLUTION):
			diryresolution = dircurrent;
			break;
		
		case(TIFFTAG_PLANARCONFIG):
			adjustLow(dircurrent);		
			setPlanarconfig((unsigned short)dircurrent->offset);
			break;
		
		case(TIFFTAG_RESOLUTIONUNIT):
			adjustLow(dircurrent);		
			setResolutionunit((unsigned short)dircurrent->offset);
			break;

		case(TIFFTAG_PREDICTOR):
			adjustLow(dircurrent);	
			setPredictor((unsigned short)dircurrent->offset);
			break;

		case(TIFFTAG_TILEWIDTH):
			adjustLow(dircurrent);	
			setTilewidth(dircurrent->offset);
			break;

		case(TIFFTAG_TILELENGTH):
			adjustLow(dircurrent);	
			setTilelength(dircurrent->offset);
			break;

		case(TIFFTAG_TILEOFFSETS):
			adjustLow(dircurrent);	
			dirtileoffset = dircurrent;
			break;

		case(TIFFTAG_TILEBYTECOUNTS):
			adjustLow(dircurrent);	
			dirtilebytecounts = dircurrent;
			break;
	
		case(TIFFTAG_MONACO):
			dirmonaco = dircurrent;
			break;

		case(TIFFTAG_PHOTO1):
			dircaption1 = dircurrent;
			break;

		case(TIFFTAG_PHOTO2):
			dircaption2 = dircurrent;
			break;
				
		default:
			break;
	}	
	dirlast = dircurrent;
	
	} //end of loop	
	
	
	//find the Bitspersample field
	status = fetchBitspersample(&_tiffptr, dirbitspersample);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle); return status; }
	
	//start to check the neccessary condition
	status = checkTiff(&_tiffptr);
	if( status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle); return status; }

	//some tiff writer ingores the tag rowsperstrip & stripbytecounts
	//so set storeformat to TIFF_STORE_LINE in this case
	if(getStoreformat() == TIFF_STORE_UNKNOWN){
	
		//check how many strips using tag Stripoffset
		dircurrent = dirlast = dirarray;
		for( i = 0 ; i < num_of_dir; i++,dircurrent++) {
			if(dircurrent->tag == TIFFTAG_STRIPOFFSETS)
				break;
		}
		
		//if no stripoffset tag, reasonable guess is 
		//there is only one strip for whole image
		if(dircurrent->tag != TIFFTAG_STRIPOFFSETS)
			setRowsperstrip(_tiffptr.imagelength);	
		else{
			rpstrip = _tiffptr.imagelength/dircurrent->count;
			if(_tiffptr.imagelength%dircurrent->count != 0)
				rpstrip++;
			setRowsperstrip(rpstrip);	
			//setRowsperstrip(_tiffptr.imagelength);	
		}	
				
		//check tag stripbytecounts is present
		dircurrent = dirlast = dirarray;
		for( i = 0 ; i < num_of_dir; i++,dircurrent++) {
			if(dircurrent->tag == TIFFTAG_STRIPBYTECOUNTS)
				break;
		}
				
		if(i >= num_of_dir){
			//if there is more than one strip for image
			//there is no way to guess, something wrong!!
			if(_tiffptr.rowsperstrip != _tiffptr.imagelength){
			McoDeleteHandle(tiffdirHandle); return MCO_PARSE_TIFF_ERROR;}
			
			fakestripbytedir.tag = TIFFTAG_STRIPBYTECOUNTS;
			fakestripbytedir.type = TIFF_LONG;
			fakestripbytedir.count = 0x0001;
			fakestripbytedir.offset = _tiffptr.imagewidth;
			dirstripbytecounts = &fakestripbytedir;
		}	
		
	}
		
	switch(getStoreformat()){
		case (TIFF_STORE_LINE):
		status = setStripsperimage((_tiffptr.imagelength + _tiffptr.rowsperstrip - 1) /
				_tiffptr.rowsperstrip );
		if(status != MCO_SUCCESS){
			McoDeleteHandle(tiffdirHandle); return status; }
			
		status = fetchStripoffset(&_tiffptr, dirstripoffset);
		if(status != MCO_SUCCESS) {
			McoDeleteHandle(tiffdirHandle); return status; }
	
		status = fetchStripbytecounts(&_tiffptr, dirstripbytecounts);
		if(status != MCO_SUCCESS) {
			McoDeleteHandle(tiffdirHandle);	return status; }
			
		break;

		case (TIFF_STORE_TILE):
		status = setTilesperimage();
		if(status != MCO_SUCCESS) {
			McoDeleteHandle(tiffdirHandle); return status; }
		
		status = fetchTileoffset(&_tiffptr, dirtileoffset);
		if(status != MCO_SUCCESS) {
			McoDeleteHandle(tiffdirHandle); return status; }
		
		status = fetchTilebytecounts(&_tiffptr, dirtilebytecounts);
		if(status != MCO_SUCCESS) {
			McoDeleteHandle(tiffdirHandle);	return status; }

		break;
		
		default:
			return MCO_PARSE_TIFF_ERROR;
	}

	setDatasize(_tiffptr.imagewidth*_tiffptr.imagelength*_tiffptr.samplesperpixel);
			
	status = fetchXresolution(&_tiffptr, dirxresolution);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle);	return status; }
		
	status = fetchYresolution(&_tiffptr, diryresolution);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle);	return status; }

	if( dirmonaco != NULL){
	status = fetchMonaco(&_tiffptr, dirmonaco);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle);	return status; }
	}	

	if( dircaption1 != NULL){
	status = fetchCaption1(&_tiffptr, dircaption1);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle);	return status; }
	}	

	if( dircaption2 != NULL){
	status = fetchCaption2(&_tiffptr, dircaption2);
	if(status != MCO_SUCCESS) {
		McoDeleteHandle(tiffdirHandle);	return status; }
	}	
		
	McoDeleteHandle(tiffdirHandle);
	return MCO_SUCCESS;		
}

//modified to support stripsperimage > 1
//currently, only uncompressed format are supported
McoStatus TiffFormat::startRead(McoHandle imagehandle, int32 readsize)
{
	int32				i = 1;
	int32				datasize;
	u_int32				tilesize, tilesperimage;
	char* 				imageptr;
	McoStatus 			status;

	if( readsize <= 0)
		return 	MCO_READ_TIFF_ERROR;

	datasize = (u_int32)getDatasize();
		
	setDatahandle(imagehandle);	
		
	switch(getStoreformat()){
		case (TIFF_STORE_LINE):	
		if( _tiffptr.planarconfig == 1 ){
			setCurrentposition(*(_tiffptr.stripoffset));
			setCurrentstrip(0);
			if( readsize > datasize)
				readsize = datasize;
				
			setReadingstrip(readsize);
			setProcessedimagesize(0);
		}
		else if( _tiffptr.planarconfig == 2 ){
			setCurrentposition(*(_tiffptr.stripoffset));
			setCurrentstrip(0);
			if( readsize > datasize)
				readsize = datasize;
				
			readsize = (readsize/_tiffptr.samplesperpixel)*_tiffptr.samplesperpixel;
			setReadingstrip(readsize);
			setProcessedimagesize(0);
		}
		else
			return 	MCO_READ_TIFF_ERROR;
		break;
		
		case (TIFF_STORE_TILE):
		setCurrentposition(*(_tiffptr.tileoffset));
		_setCurrenttile(0);
		tilesize = _tiffptr.tilewidth*_tiffptr.tilelength*_tiffptr.samplesperpixel;
		status = getTilesperimage(&tilesperimage);
		
		if( readsize > datasize)
			_setReadtilenums(tilesperimage);
		else 
			_setReadtilenums((readsize + tilesize -1)/tilesize);
		setProcessedimagesize(0);		
		break;
		
		case (TIFF_STORE_UNKNOWN):
		return MCO_PARSE_TIFF_ERROR;
	}
		
	return MCO_SUCCESS;
}


McoStatus TiffFormat::continueRead(int16 *end)
{
	u_int16 		compression;
	McoStatus 	status;
	
	if(_error == MCO_MEM_ALLOC_ERROR)
		return MCO_SYSTEM_ERROR;
	
	status = getCompression(&compression);
	if( status != MCO_SUCCESS)
		return status;

	switch (compression){
		case COMPRESSION_NONE:	
		switch (getStoreformat()){
			case TIFF_STORE_LINE:
				return continueReaduncompressed(end);	
				
			case TIFF_STORE_TILE:
				return continueReaduncomptile(end);
		}
					
		default:		
			return MCO_READ_TIFF_ERROR;
	}
}

void TiffFormat::setReadingstrip(u_int32 size)
{
	readingstrip = size;
}

void TiffFormat::_setReadtilenums(u_int32 size)
{
	_readtilenums = size;
}

void TiffFormat::setCurrentstrip(u_int32 currentsize)
{
	currentstrip = currentsize;
}

void TiffFormat::_setCurrenttile(u_int32 currenttile)
{
	_currenttile = currenttile;
}

u_int32 TiffFormat::getReadingstrip(void)
{
	return readingstrip;
}

u_int32 TiffFormat::_getReadtilenums(void)
{
	return _readtilenums;
}

u_int32 TiffFormat::getCurrentstrip(void)
{
	return currentstrip;
}

u_int32 TiffFormat::_getCurrenttile(void)
{
	return _currenttile;
}

McoStatus TiffFormat::continueReaduncompressed(int16 *end)
{
	McoStatus 			status;
	McoHandle			imageHandle;
	char*				image;
	u_int32 			curline;
	u_int32 			copysize = 0;
	u_int16*			copypos;
	u_int32 			imagesize;
	u_int32 			readlinesize;
	u_int32				copylinesize;
	u_int32				curpos;
	u_int32				i, j;
	u_int32				processedimagesize;
	u_int32				totalstrips;
	char 				*buffer, *temp, *tempimage;
	u_int16				samplesperpixel;


	imageHandle = getDatahandle();
	if( imageHandle == NULL )
		return MCO_MEM_ALLOC_ERROR;
	
	image = (char *)McoLockHandle(imageHandle);

	getImagesize(&imagesize);

	if( (curline = getCurrentstrip()) == imagesize){
		*end = 1;
		return MCO_SUCCESS;
	}	

	readlinesize = getReadingstrip();
	
	if( (curline + readlinesize) > imagesize)
		copylinesize = imagesize - curline;
	else
		copylinesize = readlinesize;

	processedimagesize = getProcessedimagesize();

//the following part is for planarconfiguration == 1
	if (getReuse() == FALSE) image += processedimagesize;

	if( _tiffptr.planarconfig == 1 ){
		curpos = (u_int32)getCurrentposition();
		//The following was modified by James
		//End of modification
		status = setFilePosition(SEEK_SET, curpos);
		if(status != MCO_SUCCESS) {
			return MCO_READ_TIFF_ERROR; }
		status = relRead(copylinesize, (char *)image);
		if(status != MCO_SUCCESS) {
			return MCO_READ_TIFF_ERROR; }
	}

//the following part is for planarconfiguration == 2
//so far it only support format for only one strip per image for every channel
	else if( _tiffptr.planarconfig == 2 ){
		samplesperpixel = _tiffptr.samplesperpixel;
		copylinesize /= samplesperpixel;
		processedimagesize /= samplesperpixel;

		buffer = (char*)malloc(copylinesize);
		if(!buffer)	return MCO_MEM_ALLOC_ERROR;

		for( i = 0; i < samplesperpixel; i++ ) {
			status = setFilePosition(SEEK_SET, *(_tiffptr.stripoffset + i) + processedimagesize );
			if(status != MCO_SUCCESS) {
				free(buffer);
				return MCO_READ_TIFF_ERROR; }
			status = relRead(copylinesize, (char *)buffer);
			if(status != MCO_SUCCESS) {
				free(buffer);
				return MCO_READ_TIFF_ERROR; }

			tempimage = image + i;
			temp = buffer;
			for( j = 0; j < copylinesize; j++){
				*tempimage = *temp++;
				tempimage += samplesperpixel;
			}
		}
		
		free(buffer);		

		copylinesize *= samplesperpixel;
		processedimagesize *= samplesperpixel;
	}


	if(_tiffptr.photometric	== 0)
		for(i=0; i < copylinesize; i++,image++)
			*image = ~(*image);

	setProcessedimagesize(processedimagesize + copylinesize);	

	setCurrentstrip(curline + copylinesize);
	setCurrentposition(curpos + copylinesize);
	McoUnlockHandle(imageHandle);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::continueReaduncomptile(int16 *end)
{
	McoStatus 			status;
	McoHandle			imageHandle;
	char*				image;
	char*				imagebuf;
	u_int32 			curtile, curtileacross, curtiledown;
	u_int32				tileacross, tiledown;
	u_int16*			copypos;
	u_int32 			tilesperimage;
	u_int32 			readtilenums, copytilenums;
	u_int32				curpos;
	u_int32				tilesize;
	u_int32				acrosssize, downsize;
	u_int32				copysize, linesize;
	u_int32				imagelinesize;
	int32				i, j, top, bottom, left, right;
	
	imageHandle = getDatahandle();
	if( imageHandle == NULL )
		return MCO_MEM_ALLOC_ERROR;
	
	image = (char *)McoLockHandle(imageHandle);

	getTilesperimage(&tilesperimage);

	if( (curtile = _getCurrenttile()) == tilesperimage){
		*end = 1;
		return MCO_SUCCESS;
	}	

		
	tileacross = (_tiffptr.imagewidth + _tiffptr.tilewidth -1)/_tiffptr.tilewidth;
	tiledown = (_tiffptr.imagelength + _tiffptr.tilelength -1)/_tiffptr.tilelength;
	
	curtileacross = curtile%tileacross;
	curtiledown = curtile/tileacross;

	acrosssize = _tiffptr.tilewidth*_tiffptr.samplesperpixel;
	downsize = _tiffptr.tilelength;
	tilesize = acrosssize * downsize;
		
	curpos = (u_int32)getCurrentposition();
	readtilenums = _getReadtilenums();

	if( (curtile + readtilenums) > tilesperimage)
		copytilenums = tilesperimage - curtile;
	else
		copytilenums = readtilenums;
	
	imagelinesize = _tiffptr.imagewidth*_tiffptr.samplesperpixel;
	
	//should always use getReuse = TRUE, no matter wether you set or not
	status = setFilePosition(SEEK_SET, curpos);
	if(status != MCO_SUCCESS) {
		return MCO_READ_TIFF_ERROR; }

	left = curtileacross*acrosssize;
	right = left + acrosssize;
	top = curtiledown*downsize;
	bottom = top + downsize;	

	imagebuf = (char*)malloc(tilesize);
	if(imagebuf == NULL){
		McoUnlockHandle(imageHandle); return MCO_MEM_ALLOC_ERROR; }
	
	for( i = 0; i < (int32)copytilenums; i++){
		status = relRead(tilesize, (char *)imagebuf);
		if(status != MCO_SUCCESS) {
			free(imagebuf); McoUnlockHandle(imageHandle);
			return MCO_READ_TIFF_ERROR;
		}
		
		if(right > (int32)imagelinesize)
			right = imagelinesize;
		if(bottom > (int32)_tiffptr.imagelength)
			bottom = _tiffptr.imagelength;
		image = (char*)McoLockHandle(imageHandle);
		image += left + top*imagelinesize;
		copysize = right - left;
		
		for(j = 0; j < bottom - top; j++)
			memcpy(image+imagelinesize*j, imagebuf+acrosssize*j, copysize);
		
		if(right == (int32)imagelinesize){
			top = bottom;
			bottom += downsize;
			left = 0;
			right = acrosssize;
		}
		else{		
			left = right;
			right += acrosssize;	
		}	
	}

	free(imagebuf); 
	_setCurrenttile(curtile + copytilenums);
	setCurrentposition(curpos + copytilenums*tilesize);
	McoUnlockHandle(imageHandle);
	return MCO_SUCCESS;
}


void TiffFormat::setProcessedimagesize(u_int32 size)
{
	processedimagesize = size;
}

u_int32 TiffFormat::getProcessedimagesize(void)
{
	return processedimagesize;
}

McoStatus TiffFormat::packDirnewsubfiletype(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_NEWSUBFILETYPE;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->newsubfiletype;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirimagewidth(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	if(tiffptr->imagewidth < 65536 )
		tiffdir->type = TIFF_SHORT;
	else
		tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_IMAGEWIDTH;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->imagewidth;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirimagelength(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	if(tiffptr->imagelength < 65536 )
		tiffdir->type = TIFF_SHORT;
	else
		tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_IMAGELENGTH;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->imagelength;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirbitspersample(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	u_int16	num_of_tags;
	u_int32 offset;
	
	tiffdir->tag = TIFFTAG_BITSPERSAMPLE;
	tiffdir->type = TIFF_SHORT;
	tiffdir->count = tiffptr->samplesperpixel;
	if(tiffdir->count == 1){
		tiffdir->offset = tiffptr->bitspersample;
		adjustHigh(tiffdir);
		return MCO_SUCCESS;
	}
	if(tiffdir->count == 2 ){
		tiffdir->offset = tiffptr->bitspersample;
		tiffdir->offset <<= 16;
		tiffdir->offset += tiffptr->bitspersample;
		return MCO_SUCCESS;
	}
	
	offset = getDiroffset();
	setDiroffset( (offset + ((tiffdir->count*2 + 2)/4)*4) );
	tiffdir->offset = offset;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDircompression(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_COMPRESSION;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->compression;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirphotometric(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_PHOTOMETRIC;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->photometric;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirfillorder(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_FILLORDER;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->fillorder;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirstripoffset(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int16	num_of_tags;
	u_int32 offset;
	u_int32 image_length = 0;
	u_int32 strip_length;
	u_int32 i;

	
	for( i = 0; i < tiffptr->stripsperimage; i++ ) 
		image_length += *(tiffptr->stripbytecount + i);
		
	tiffdir->tag = TIFFTAG_STRIPOFFSETS;
	tiffdir->type = TIFF_LONG;
	
	tiffdir->count = tiffptr->stripsperimage;
	
	if(tiffptr->stripoffset != NULL )
		free(tiffptr->stripoffset);
	
	tiffptr->stripoffset = (u_int32 *)malloc(tiffptr->stripsperimage*sizeof(u_int32));
	if(tiffptr->stripbytecount == NULL ) return MCO_MEM_ALLOC_ERROR;

	if(tiffdir->count == 1){
		tiffdir->offset = getDiroffset();
		*tiffptr->stripoffset = tiffdir->offset;
		setDiroffset( ((tiffdir->offset + (*(tiffptr->stripbytecount) + 3 )/4) *4) );
		adjustHigh(tiffdir);
		return MCO_SUCCESS;
	}
	
	offset = getDiroffset();
	tiffdir->offset = offset;
	offset += sizeof(u_int32)*tiffptr->stripsperimage + image_length;	
	while( offset % 4)
		offset++;
	setDiroffset( offset );	

	for( i = 0; i < tiffptr->stripsperimage; i++ )
		*(tiffptr->stripoffset + i) = tiffdir->offset;

	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	status = relWrite(sizeof(u_int32)*tiffptr->stripsperimage, (char*)tiffptr->stripoffset); 
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;	

	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirorientation(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_ORIENTATION;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->orientation;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirsamplesperpixel(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_SAMPLESPERPIXEL;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->samplesperpixel;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirrowsperstrip(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	if(tiffptr->imagelength < 65536 )
		tiffdir->type = TIFF_SHORT;
	else
		tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_ROWSPERSTRIP;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->rowsperstrip;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirstripbytecount(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	//right now, only support stripsperimage = 1
	McoStatus	status;
	u_int32 offset, imagesize;
	u_int32 image_length = 0;
	u_int32 strip_length = 65532; //largest strip_length so far
	u_int32 i;
	u_int32 stripsperimage, rowsperstrip, width;

	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if( (imagesize = getDatasize()) == 0 ) //0 size image cannot write out
		return MCO_WRITE_TIFF_ERROR;
	
	getStripsperimage(&stripsperimage);
	
	if(_tiffptr.stripbytecount != NULL )
		free(_tiffptr.stripbytecount);
	
	//just implement the simplest case	
	_tiffptr.stripbytecount = (u_int32 *)malloc(stripsperimage*sizeof(u_int32));
	if(_tiffptr.stripbytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }

	//modified by Peter to support stripsperimage > 1
	status = getRowsperstrip(&rowsperstrip);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;

	status = getWidth(&width);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;
		 
	for( i = 0; i < stripsperimage; i++ )
		*(_tiffptr.stripbytecount + i) = rowsperstrip*_tiffptr.imagewidth;
		
	//the last one may not be correct, compensate
	*(_tiffptr.stripbytecount + stripsperimage - 1) = 
		imagesize - rowsperstrip*width*(stripsperimage - 1);	
	//end of modification 		
	
	for( i = 0; i < tiffptr->stripsperimage; i++ ) 
		image_length += *(tiffptr->stripbytecount + i);
		
	tiffdir->tag = TIFFTAG_STRIPBYTECOUNTS;
	tiffdir->type = TIFF_LONG;
	
	tiffdir->count = tiffptr->stripsperimage;
	if(tiffdir->count == 1){
		tiffdir->offset = image_length;
		adjustHigh(tiffdir);
		return MCO_SUCCESS;
	}
	
	offset = getDiroffset();
	tiffdir->offset = offset;
	offset += sizeof(u_int32)*tiffptr->stripsperimage;	
	while( offset % 4)
		offset++;
	setDiroffset( offset );
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	status = relWrite(sizeof(u_int32)*tiffptr->stripsperimage, (char*)tiffptr->tilebytecount); 
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;	
			
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirxresolution(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_RATIONAL;
	tiffdir->tag = TIFFTAG_XRESOLUTION;
	tiffdir->count = 1;
	tiffdir->offset = getDiroffset();
	setDiroffset(tiffdir->offset + 8 );
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDiryresolution(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_RATIONAL;
	tiffdir->tag = TIFFTAG_YRESOLUTION;
	tiffdir->count = 1;
	tiffdir->offset = getDiroffset();
	setDiroffset(tiffdir->offset + 8 );
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirplanarconfig(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_PLANARCONFIG;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->planarconfig;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirresolutionunit(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_RESOLUTIONUNIT;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->resolutionunit;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirpredictor(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	tiffdir->type = TIFF_SHORT;
	tiffdir->tag = TIFFTAG_PREDICTOR;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->predictor;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirtilewidth(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	if(tiffptr->imagelength < 65536 )
		tiffdir->type = TIFF_SHORT;
	else
		tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_TILEWIDTH;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->tilewidth;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirtilelength(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	if(tiffptr->imagelength < 65536 )
		tiffdir->type = TIFF_SHORT;
	else
		tiffdir->type = TIFF_LONG;
	tiffdir->tag = TIFFTAG_TILELENGTH;
	tiffdir->count = 1;
	tiffdir->offset = tiffptr->tilelength;
	adjustHigh(tiffdir);
	return MCO_SUCCESS;
}


McoStatus TiffFormat::packDirtileoffset(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int16	num_of_tags;
	u_int32 offset;
	u_int32 image_length = 0;
	u_int32 strip_length;
	u_int32 i;
	u_int32 tilesperimage;

	status = getTilesperimage(&tilesperimage);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	
	for( i = 0; i < tilesperimage; i++ ) 
		image_length += *(tiffptr->tilebytecount + i);
		
	tiffdir->tag = TIFFTAG_TILEOFFSETS;
	tiffdir->type = TIFF_LONG;
	
	tiffdir->count = tilesperimage;
	
	if(tiffptr->tileoffset != NULL ) free(tiffptr->tileoffset);
	tiffptr->tileoffset = (u_int32 *)malloc(tilesperimage*sizeof(u_int32));
	if(tiffptr->tilebytecount == NULL ) return MCO_MEM_ALLOC_ERROR;

	if(tiffdir->count == 1){
		tiffdir->offset = getDiroffset();
		*tiffptr->tileoffset = tiffdir->offset;
		setDiroffset( ((tiffdir->offset + (*(tiffptr->tilebytecount) + 3 )/4) *4) );
		adjustHigh(tiffdir);
		return MCO_SUCCESS;
	}
	
	//need to be save into the file
	offset = getDiroffset();
	tiffdir->offset = offset;
	offset += sizeof(u_int32)*tilesperimage + image_length;	
	while( offset % 4)
		offset++;
		
	setDiroffset( offset );		
	for( i = 0; i < tilesperimage; i++ )
		*(tiffptr->tileoffset + i) = tiffdir->offset + 4*tilesperimage +
		tiffptr->tilewidth*tiffptr->tilelength*tiffptr->samplesperpixel*i;
	
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	status = relWrite(sizeof(u_int32)*tilesperimage, (char*)tiffptr->tileoffset); 
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;		
	
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirtilebytecount(TIFFDIRECTORYPTR tiffptr, 
	TIFFDIRENTRYPTR tiffdir)
{
	McoStatus status;
	u_int32 offset;
	u_int32 image_length = 0, imagesize;
	u_int32 i;
	u_int32 tilesperimage, tilewidth, tilelength;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if( (imagesize = getDatasize()) == 0 ) //0 size image cannot write out
		return MCO_WRITE_TIFF_ERROR;
		
	status = getTilesperimage(&tilesperimage);
	if(status != MCO_SUCCESS)
		return MCO_WRITE_TIFF_ERROR;
	
	if(_tiffptr.tilebytecount != NULL )
		free(_tiffptr.tilebytecount);
	
	//just implement the simplest case	
	_tiffptr.tilebytecount = (u_int32 *)malloc(tilesperimage*sizeof(u_int32));
	if(_tiffptr.tilebytecount == NULL ){
		return MCO_MEM_ALLOC_ERROR; }

	for( i = 0; i < tilesperimage; i++ )
		*(_tiffptr.tilebytecount + i) = 
		_tiffptr.tilewidth*_tiffptr.tilelength*_tiffptr.samplesperpixel;
			
	for( i = 0; i < tilesperimage; i++ ) 
		image_length += *(tiffptr->tilebytecount + i);
		
	tiffdir->tag = TIFFTAG_TILEBYTECOUNTS;
	tiffdir->type = TIFF_LONG;
	
	tiffdir->count = tilesperimage;
	if(tiffdir->count == 1){
		tiffdir->offset = image_length;
		adjustHigh(tiffdir);
		return MCO_SUCCESS;
	}
		
	offset = getDiroffset();
	tiffdir->offset = offset;
	offset += sizeof(u_int32)*tilesperimage;	
	while( offset % 4)
		offset++;
		
	setDiroffset( offset );		
	status = setFilePosition(SEEK_SET, tiffdir->offset);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	status = relWrite(sizeof(u_int32)*tilesperimage, (char*)tiffptr->tilebytecount); 
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::packDirdotrange(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	u_int32	temp;
	
	tiffdir->type = TIFF_BYTE;
	tiffdir->tag = TIFFTAG_DOTRANGE;
	tiffdir->count = 2;
	tiffdir->offset = tiffptr->dotrange0;
	tiffdir->offset <<= 24;
	temp = tiffptr->dotrange1;
	temp <<= 16;
	tiffdir->offset += temp;
	return MCO_SUCCESS;
}



//add for new plug and filter
McoStatus TiffFormat::packDirmonaco(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
	u_int16	num_of_tags;
	u_int32 offset;
	u_int32 image_length = 0;
	u_int32 strip_length;
	u_int32 i;
			
	tiffdir->tag = TIFFTAG_MONACO;

	tiffdir->type = TIFF_BYTE;
	
	tiffdir->count = monacosize;
	tiffdir->offset = getDiroffset();
	setDiroffset( ((tiffdir->offset + (monacosize + 3 )/4) *4) );
	adjustHigh(tiffdir);
	return MCO_SUCCESS;

}

//add for photoshop caption1
McoStatus TiffFormat::packDircaption1(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
			
	tiffdir->tag = TIFFTAG_PHOTO1;

	tiffdir->type = TIFF_BYTE;
	
	tiffdir->count = captionsize1;
	tiffdir->offset = getDiroffset();
	setDiroffset( ((tiffdir->offset + (captionsize1 + 3 )/4) *4) );
	adjustHigh(tiffdir);
	return MCO_SUCCESS;

}

//add for photoshop caption2
//fix on 8/4, tiffdir->type is Byte instead of LONG
McoStatus TiffFormat::packDircaption2(TIFFDIRECTORYPTR tiffptr,
	TIFFDIRENTRYPTR tiffdir)
{
			
	tiffdir->tag = TIFFTAG_PHOTO2;

	//tiffdir->type = TIFF_LONG;	
	tiffdir->type = TIFF_BYTE;

	tiffdir->count = captionsize2;
	tiffdir->offset = getDiroffset();
	setDiroffset( ((tiffdir->offset + (captionsize2 + 3 )/4) *4) );
	adjustHigh(tiffdir);
	return MCO_SUCCESS;

}

McoStatus TiffFormat::packImage(TIFFDIRECTORYPTR tiffptr)
{
	McoStatus status;
	u_int32 imagesize;
	u_int32 stripsperimage;
	u_int32 i;
	char *imageptr, *currentimageptr;
	
	if( (imagesize = getDatasize()) == 0 ) //can't be 0 image
		return MCO_FAILURE;
	
	 getStripsperimage(&stripsperimage); //Should be locked
	
	if(image_dataHandle == NULL )
		return MCO_FAILURE;
	imageptr = (char *)McoLockHandle(image_dataHandle);
	currentimageptr = imageptr;
	for( i = 0; i < stripsperimage; i++ ) {
		status = setFilePosition(SEEK_SET, *(tiffptr->stripoffset+i));
		if(status != MCO_SUCCESS) { 
			McoUnlockHandle(image_dataHandle); return status; }
		status = relWrite(*(tiffptr->stripbytecount+i), (char *)currentimageptr);
		currentimageptr += *(tiffptr->stripbytecount+i);
		if(status != MCO_SUCCESS) {
			McoUnlockHandle(image_dataHandle); return status; }
	}
	
	McoUnlockHandle(image_dataHandle);
	return MCO_SUCCESS;
}		

McoStatus	TiffFormat::setImagesize(u_int32 size)
{
	setDatasize(size);
	return MCO_SUCCESS;
}
	
McoStatus TiffFormat::prepareWrite(void)
{
	TIFFHEADER 			tiffheader;
	TIFFDIRENTRY		tiffdir[32]; //currently,up to 32 entries supported,increase if neccessary.
	TIFFDIRENTRYPTR		dircurrent, dirlast, dirarray;
	TIFFDIRENTRYPTR		dirbitspersample, dirstripoffset, dirstripbytecounts;
	TIFFDIRENTRYPTR		dirphotometric, dirtileoffset, dirtilebytecounts;
	
	//added by peter for new plug and filter
	TIFFDIRENTRYPTR		dirmonaco;
	//end

	//added by peter for new plug and filter
	TIFFDIRENTRYPTR		dircaption2;	
	//end 

	TIFFDIRENTRYPTR		dirxresolution, diryresolution;
	u_int16				bitspersample, short_stripbytecount, short_stripoffset;
	McoHandle			tiffdirHandle;
	McoStatus 			status;	
	u_int16				num_of_dir;
	int32				next_diroff = 0;
	u_int32   			imagesize;
	u_int16				*compressionptr;
	int32				diroff;
	char 				*imageptr;
	int16				i;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	setDatasize(_tiffptr.imagewidth*_tiffptr.imagelength*_tiffptr.samplesperpixel);
	switch(getStoreformat()){
		case TIFF_STORE_LINE:	
		setStripsperimage(1);
		break;
		
		case TIFF_STORE_TILE:
		status = setTilesperimage();
		if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
		break;
		
		default:
			return MCO_WRITE_TIFF_ERROR; 
	}
			
	status = setFilePosition(SEEK_SET, 0);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	
	getHeader(&tiffheader);
	status = relWrite(sizeof(tiffheader), (char *)&tiffheader);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;

	status = setFilePosition(SEEK_SET, tiffheader.diroff);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
		
	num_of_dir = getNumofsupportedtags();
	if( _tiffptr.photometric == PHOTOMETRIC_CMYK)
		dirphotometric = &tiffdir[num_of_dir++];

	//add for new plug and batch
	if( monacosize != 0)
		dirmonaco = &tiffdir[num_of_dir++];
	//end of mod

	//add for new plug and batch
	if( captionsize2 != 0)
		dircaption2 = &tiffdir[num_of_dir++];
	//end of mod
			
	status = relWrite(2, (char *)&num_of_dir);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;

	setDiroffset( sizeof(TIFFHEADER) + 4 + num_of_dir * sizeof(TIFFDIRENTRY) + 4);
	
	switch(getStoreformat()){
		case(TIFF_STORE_LINE):
		packDirnewsubfiletype(&_tiffptr, &tiffdir[0]);	
		packDirimagewidth(&_tiffptr, &tiffdir[1]);
		packDirimagelength(&_tiffptr, &tiffdir[2]);
		packDirbitspersample(&_tiffptr, &tiffdir[3]);
		dirbitspersample = &tiffdir[3];
		packDircompression(&_tiffptr, &tiffdir[4]);
		packDirphotometric(&_tiffptr, &tiffdir[5]);
		packDirfillorder(&_tiffptr, &tiffdir[6]);
		
		packDirorientation(&_tiffptr, &tiffdir[8]);
		packDirsamplesperpixel(&_tiffptr, &tiffdir[9]);
		packDirrowsperstrip(&_tiffptr, &tiffdir[10]);
		packDirstripbytecount(&_tiffptr, &tiffdir[11]);
		packDirxresolution(&_tiffptr, &tiffdir[12]);
		dirxresolution = &tiffdir[12];
		packDiryresolution(&_tiffptr, &tiffdir[13]);
		diryresolution = &tiffdir[13];
		packDirplanarconfig(&_tiffptr, &tiffdir[14]);
		packDirresolutionunit(&_tiffptr, &tiffdir[15]);
		packDirstripoffset(&_tiffptr, &tiffdir[7]);
		break;
		
		case(TIFF_STORE_TILE):
		packDirnewsubfiletype(&_tiffptr, &tiffdir[0]);	
		packDirimagewidth(&_tiffptr, &tiffdir[1]);
		packDirimagelength(&_tiffptr, &tiffdir[2]);
		packDirbitspersample(&_tiffptr, &tiffdir[3]);
		dirbitspersample = &tiffdir[3];
		packDircompression(&_tiffptr, &tiffdir[4]);
		packDirphotometric(&_tiffptr, &tiffdir[5]);
		packDirfillorder(&_tiffptr, &tiffdir[6]);
		
		packDirorientation(&_tiffptr, &tiffdir[7]);
		packDirsamplesperpixel(&_tiffptr, &tiffdir[8]);
		packDirxresolution(&_tiffptr, &tiffdir[9]);
		dirxresolution = &tiffdir[9];
		packDiryresolution(&_tiffptr, &tiffdir[10]);
		diryresolution = &tiffdir[10];
		packDirplanarconfig(&_tiffptr, &tiffdir[11]);
		packDirresolutionunit(&_tiffptr, &tiffdir[12]);
		packDirtilewidth(&_tiffptr, &tiffdir[13]);
		packDirtilelength(&_tiffptr, &tiffdir[14]);
		packDirtilebytecount(&_tiffptr, &tiffdir[16]);				
		packDirtileoffset(&_tiffptr, &tiffdir[15]);
		break;
	}
		

	//if support LZW, add the following line
	//packDirpredictor(&_tiffptr, &tiffdir[15]);

	//support CMYK writing
	if( _tiffptr.photometric == PHOTOMETRIC_CMYK)
		packDirdotrange(&_tiffptr, dirphotometric);
	
	//if there is a monaco tag
	if( monacosize != 0)
		packDirmonaco(&_tiffptr, dirmonaco);
	//end of mod

	//added for photoshop caption2
	if( captionsize2 != 0)
		packDircaption2(&_tiffptr, dircaption2);
	//end of mod

	status = setFilePosition(SEEK_SET, tiffheader.diroff+2);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	status = relWrite(sizeof(TIFFDIRENTRY)*num_of_dir, (char *)&tiffdir);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
	
	status = relWrite(4, (char *)&next_diroff);
	if(status != MCO_SUCCESS) return MCO_WRITE_TIFF_ERROR;
				
	if(dirbitspersample->count > 2 ){
		status = setFilePosition(SEEK_SET, dirbitspersample->offset);
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
		for( i = 0; i < (int16)dirbitspersample->count; i++ ) 
			status = relWrite(2, (char *)&(_tiffptr.bitspersample));
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
	}

	status = setFilePosition(SEEK_SET, dirxresolution->offset);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	
	status = relWrite(4, (char *)&(_tiffptr.xresolutionnumerator));
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	
	status = relWrite(4, (char *)&(_tiffptr.xresolutiondenominator));
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	
	
	status = setFilePosition(SEEK_SET, diryresolution->offset);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	
	status = relWrite(4, (char *)&(_tiffptr.yresolutionnumerator));
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	
	status = relWrite(4, (char *)&(_tiffptr.yresolutiondenominator));
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }	


	//add for new plug and batch
	//if there is a monaco tag
	if( monacosize != 0){
		status = setFilePosition(SEEK_SET, dirmonaco->offset);
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
		status = relWrite(dirmonaco->count, (char *)_tiffptr.monacofield);
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
	}		
	//end of mod

	//added for photoshop caption
	if( captionsize2 != 0){
		status = setFilePosition(SEEK_SET, dircaption2->offset);
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
		status = relWrite(dircaption2->count*sizeof(long), (char *)_tiffptr.caption2);
		if(status != MCO_SUCCESS) {
			return MCO_WRITE_TIFF_ERROR; }	
	}		
	//end
		
	return MCO_SUCCESS;		
}

//currently, only uncompressed format are supported
McoStatus TiffFormat::startWrite(McoHandle imagehandle, int32 readsize)
{
	int32				i = 1;
	int32				datasize;
	u_int32				tilesize, tilesperimage;
	char* 				imageptr;
	McoStatus 			status;

	if( readsize <= 0)
		return MCO_WRITE_TIFF_ERROR;
		
	datasize = (u_int32)getDatasize();
		
	setDatahandle(imagehandle);	

	switch(getStoreformat()){
		case(TIFF_STORE_LINE):
		//currently only supported stripsperimage = 1
		if( _tiffptr.stripsperimage != 1){
			return MCO_WRITE_TIFF_ERROR;
		}
		
		setCurrentposition(*(_tiffptr.stripoffset));
		setCurrentstrip(0);
		if( readsize > datasize)
			readsize = datasize;
		
		setReadingstrip(readsize);
		setProcessedimagesize(0);
		break;
		
		case(TIFF_STORE_TILE):
		setCurrentposition(*(_tiffptr.tileoffset));
		_setCurrenttile(0);
		tilesize = _tiffptr.tilewidth*_tiffptr.tilelength*_tiffptr.samplesperpixel;
		status = getTilesperimage(&tilesperimage);
		
		if( readsize > datasize)
			_setReadtilenums(tilesperimage);
		else 
			_setReadtilenums((readsize + tilesize -1)/tilesize);
		setProcessedimagesize(0);
		break;
	}			

	return MCO_SUCCESS;
}

//stop here third time
McoStatus TiffFormat::continueWrite(int16 *end)
{
	u_int16 	compression;
	u_int16		storeformat;
	McoStatus 	status;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	status = getCompression(&compression);
	if( status != MCO_SUCCESS)
		return status;
	
	switch (compression){
		case COMPRESSION_NONE:	
		switch (getStoreformat()){
			case TIFF_STORE_LINE: 
				return continueWriteuncompressed(end);					
			case TIFF_STORE_TILE:
				return continueWriteuncomptile(end);
		}
					
		default:		
			return MCO_WRITE_TIFF_ERROR;
	} 
}

McoStatus TiffFormat::continueWriteuncompressed(int16 *end)
{
	McoStatus 			status;
	McoHandle			imageHandle;
	char*				image;
	u_int32 			curline;
	u_int32 			copysize = 0;
	u_int16*			copypos;
	u_int32 			imagesize;
	u_int32 			readlinesize;
	u_int32				copylinesize;
	u_int32				curpos;
	u_int32				i, j;
	u_int32				processedimagesize;
	u_int32				totalstrips;

	imageHandle = getDatahandle();
	if( imageHandle == NULL )
		return MCO_MEM_ALLOC_ERROR;
	
	image = (char *)McoLockHandle(imageHandle);

	getImagesize(&imagesize);


	if( (curline = getCurrentstrip()) == imagesize){
		*end = 1;
		return MCO_SUCCESS;
	}	

	readlinesize = getReadingstrip();
	
	if( (curline + readlinesize) > imagesize)
		copylinesize = imagesize - curline;
	else
		copylinesize = readlinesize;
		
//	McoMessAlert(MCO_NO_ERROR,"\pwrite 4");

	processedimagesize = getProcessedimagesize();
	curpos = (u_int32)getCurrentposition();
	//The following was modified by James
	if (getReuse() == FALSE) image += processedimagesize;
	//End of modification
	status = setFilePosition(SEEK_SET, curpos);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }
	status = relWrite(copylinesize, (char *)image);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }
	setProcessedimagesize(processedimagesize + copylinesize);	
	
//	McoMessAlert(MCO_NO_ERROR,"\pwrite 5");

	setCurrentstrip(curline + copylinesize);
	setCurrentposition(curpos + copylinesize);
	McoUnlockHandle(imageHandle);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::continueWriteuncomptile(int16 *end)
{
	McoStatus 			status;
	McoHandle			imageHandle;
	char*				image;
	u_int32 			curtile;
	u_int16*			copypos;
	u_int32 			tilesperimage;
	u_int32 			readtilenums, copytilenums;
	u_int32				curpos;

	imageHandle = getDatahandle();
	if( imageHandle == NULL )
		return MCO_MEM_ALLOC_ERROR;
	
	image = (char *)McoLockHandle(imageHandle);

	getTilesperimage(&tilesperimage);

	if( (curtile = _getCurrenttile()) == tilesperimage){
		*end = 1;
		return MCO_SUCCESS;
	}	

	curpos = (u_int32)getCurrentposition();
	readtilenums = _getReadtilenums();

	if( (curtile + readtilenums) > tilesperimage)
		copytilenums = tilesperimage - curtile;
	else
		copytilenums = readtilenums;
	
	//should always use getReuse = TRUE, no matter wether you set or not
	status = setFilePosition(SEEK_SET, curpos);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }
	status = relWrite(copytilenums*_tiffptr.tilewidth*_tiffptr.tilelength*_tiffptr.samplesperpixel, (char *)image);
	if(status != MCO_SUCCESS) {
		return MCO_WRITE_TIFF_ERROR; }

	_setCurrenttile(curtile + copytilenums);
	setCurrentposition(curpos + copytilenums*_tiffptr.tilewidth*_tiffptr.tilelength*_tiffptr.samplesperpixel);
	McoUnlockHandle(imageHandle);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setSamplesperpixel(u_int16 samplesperpixel)
{
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.samplesperpixel = samplesperpixel;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setWidth(u_int32 imagewidth)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		
	_tiffptr.imagewidth = imagewidth;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setLength(u_int32 imagelength)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.imagelength = imagelength;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setBitspersample(u_int16 bitspersample)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.bitspersample = bitspersample;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setPhotometric(u_int16 photometric)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.photometric = photometric;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setCompression(u_int16 compression)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.compression = compression;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setFillorder(u_int16 fillorder)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.fillorder = fillorder;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setOrientation(u_int16 orientation)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	_tiffptr.orientation = orientation;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setRowsperstrip(u_int32 rowsperstrip)
{
	McoStatus status;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		
	status = setStoreformat(TIFF_STORE_LINE);
	if(status != MCO_SUCCESS)
		return status;

	_tiffptr.rowsperstrip = rowsperstrip;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setPlanarconfig(u_int16 planarconfig)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.planarconfig = planarconfig;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setResolution(u_int16 unit, u_int32 xn, u_int32 xd, u_int32 yn, u_int32 yd)
{
	McoStatus status;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	if( (status = setResolutionunit(unit)) != MCO_SUCCESS)
		return status;
	if( (status = setXresolution(xn, xd)) != MCO_SUCCESS)
		return status;
	if( (status = setYresolution(yn, yd)) != MCO_SUCCESS)
		return status;
		
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setResolutionfromphoto(u_int16 unit, u_int32 hres, u_int32 hihres, u_int32 vres, u_int32 hivres)
{
	McoStatus status;
	u_int32 xn, xd, yn, yd;
	float total;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
	
	if( unit != 1 ) //always check it is inch
		return MCO_WRITE_TIFF_ERROR;

	total = (float)((float)hres + (float)hihres/65536.0);
	xn = (u_int32)(total*10000.0);
	xd = 10000;
	
	total = (float)((float)vres + (float)hivres/65536.0);
	yn = (u_int32)(total*10000.0);
	yd = 10000;
	
	if( (status = setResolutionunit(2)) != MCO_SUCCESS)
		return status;
	if( (status = setXresolution(xn, xd)) != MCO_SUCCESS)
		return status;
	if( (status = setYresolution(yn, yd)) != MCO_SUCCESS)
		return status;
		
	return MCO_SUCCESS;
}


McoStatus TiffFormat::setResolutionunit(u_int16 resolutionunit)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.resolutionunit = resolutionunit;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setXresolution(u_int32 numerator, u_int32 denominator)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.xresolutionnumerator = numerator;
	_tiffptr.xresolutiondenominator = denominator;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setYresolution(u_int32 numerator, u_int32 denominator)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	_tiffptr.yresolutionnumerator = numerator;
	_tiffptr.yresolutiondenominator = denominator;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setTilewidth(u_int32 tilewidth)
{
	McoStatus status;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		
	status = setStoreformat(TIFF_STORE_TILE);
	if(status != MCO_SUCCESS)
		return status;
	_tiffptr.tilewidth = tilewidth;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::setTilelength(u_int32 tilelength)
{
	McoStatus status;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;
		
	status = setStoreformat(TIFF_STORE_TILE);
	if(status != MCO_SUCCESS)
		return status;
	_tiffptr.tilelength = tilelength;
	return MCO_SUCCESS;
}
	
McoStatus TiffFormat::setImage(char *image, u_int32 imagesize)
{
	char *imageptr;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	setDatasize(imagesize);
	if(image_dataHandle == NULL)
		McoDeleteHandle(image_dataHandle);
	image_dataHandle = McoNewHandle(imagesize);
	if( image_dataHandle == NULL ){
		setDatasize(0); return MCO_MEM_ALLOC_ERROR; }	
	imageptr = (char *)McoLockHandle(image_dataHandle);
	memcpy(imageptr, image, imagesize);
	McoUnlockHandle(image_dataHandle);
	return MCO_SUCCESS;
}

	

McoStatus TiffFormat::getSamplesperpixel(u_int16 *samplesperpixel)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*samplesperpixel = _tiffptr.samplesperpixel;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getWidth(u_int32 *imagewidth)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*imagewidth = _tiffptr.imagewidth;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getLength(u_int32 *imagelength)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*imagelength = _tiffptr.imagelength;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getBitspersample(u_int16 *bitspersample)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*bitspersample = _tiffptr.bitspersample;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getPhotometric(u_int16 *photometric)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*photometric = _tiffptr.photometric;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getCompression(u_int16 *compression)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*compression = _tiffptr.compression;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getFillorder(u_int16 *fillorder)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*fillorder = _tiffptr.fillorder;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getOrientation(u_int16 *orientation)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*orientation = _tiffptr.orientation;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getRowsperstrip(u_int32 *rowsperstrip)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*rowsperstrip = _tiffptr.rowsperstrip;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getPlanarconfig(u_int16 *planarconfig)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*planarconfig = _tiffptr.planarconfig;
	return MCO_SUCCESS;
}
	
McoStatus TiffFormat::getResolutionunit(u_int16 *resolutionunit)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*resolutionunit = _tiffptr.resolutionunit;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getXresolution(u_int32 * numerator, u_int32 * denominator)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*numerator = _tiffptr.xresolutionnumerator;
	*denominator = _tiffptr.xresolutiondenominator;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getYresolution(u_int32 * numerator, u_int32 * denominator)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*numerator = _tiffptr.yresolutionnumerator;
	*denominator = _tiffptr.yresolutiondenominator;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getTilewidth(u_int32 *tilewidth)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*tilewidth = _tiffptr.tilewidth;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getTilelength(u_int32 *tilelength)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*tilelength = _tiffptr.tilelength;
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getMonaco(char * monaco)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if(monacosize == 0)	 
		return MCO_FAILURE;
	
	memcpy(monaco, (char*)(_tiffptr.monacofield), monacosize);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getMonacosize(long *size)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*size = monacosize;	 
	return MCO_SUCCESS;
}


//add for photoshop caption2
McoStatus TiffFormat::getCaption2(char * caption, u_int32 *size)
{
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	*size = captionsize2;
	if(captionsize2 > 0)
		memcpy(caption, _tiffptr.caption2, captionsize2);
	//*caption = _tiffptr.caption2;
	return MCO_SUCCESS;
}



// This function was added by James
// It loads the data directly into the profile structure and makes it
// unnecessary to allocate more temporary memory

McoStatus TiffFormat::getMonacoJob(char *jobP,int t)
{
	McoStatus	state;

/*
#ifndef NO_JOB
	MS_color_correct *job;
	
	job = (MS_color_correct*)jobP;
	
	if(_error != MCO_SUCCESS)
		return MCO_SYSTEM_ERROR;

	if(monacosize == 0)	 
		return MCO_NOT_PRESENT;
	
	state = job->do_load_job_mem((MS_type)t,(char*)(_tiffptr.monacofield));
	return state;
#else
	return MCO_FAILURE;
#endif	
*/	
	return MCO_FAILURE;
}


//This seems to be not good, since it produce second buffer	
McoStatus TiffFormat::getImage(char *image)
{
	char * imageptr;
	
	if(_error == MCO_MEM_ALLOC_ERROR)
		return MCO_SYSTEM_ERROR;
	if(image_dataHandle == NULL )
		return MCO_FAILURE;
	
	imageptr = (char *)McoLockHandle(image_dataHandle);	
	memcpy((void *)image, (void *)imageptr, getDatasize());	 
	McoUnlockHandle(image_dataHandle);
	return MCO_SUCCESS;
}

McoStatus TiffFormat::getImagehandle(McoHandle *imagehandle)
{
	if(_error == MCO_MEM_ALLOC_ERROR)
		return MCO_SYSTEM_ERROR;
	if(image_dataHandle == NULL )
		return MCO_FAILURE;

	*imagehandle = image_dataHandle;
	return MCO_SUCCESS;
}	
	
McoStatus TiffFormat::getImagesize(u_int32 *imagesize)
{	
	if(_error == MCO_MEM_ALLOC_ERROR)
		return MCO_SYSTEM_ERROR;
	
	*imagesize = (u_int32)getDatasize();
	return MCO_SUCCESS;
}

