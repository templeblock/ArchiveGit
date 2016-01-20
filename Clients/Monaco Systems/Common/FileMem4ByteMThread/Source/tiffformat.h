/************************************************************************************
*
*  	header file for reading photoshop2.5 format.
*	monaco system.
*
**************************************************************************************/

#ifndef _TIFF_FORMAT_H_
#define _TIFF_FORMAT_H_

#include "mcotypes.h"
#include "mcostat.h"
#include "confileformat.h"

#define	TIFF_VERSION		42
#define	TIFF_BIGENDIAN		0x4D4D
#define	TIFF_LITTLEENDIAN	0x4949
#define TIFF_INIT_OFFSET	-1

#define TIFF_SWAB_ON		0
#define TIFF_SWAB_OFF		1

#define TIFF_STORE_UNKNOWN	0
#define TIFF_STORE_LINE		1
#define TIFF_STORE_TILE		2

//defaults value for the tags which have defaults value 

#define NEWSUBFILETYPE_DEFAULT		0x00000000
#define	BITSPERSAMPLE_DEFAULT		1
#define	COMPRESSION_DEFAULT			1 //NO COMPRESSION
#define PHOTOMETRIC_DEFAULT			2 //RGB	
#define	FILLORDER_DEFAULT			1
#define	ORIENTATION_DEFAULT			1
#define	PLANARCONFIGRATION_DEFAULT	1
#define	RESOLUTIONUNIT_DEFAULT		2 //inch
#define	ROWSPERSTRIP_DEFAULT		0xffffffff
#define	SAMPLESPERPIXEL_DEFAULT		8 //GRAY and RGB
#define	PREDICTOR_DEFAULT			1
#define DOTRANGE0_DEFAULT			0x00 //value FF is 0% for photoshop2.5
#define DOTRANGE1_DEFAULT			0xFF //value 0 is 1%  for photoshop2.5


//following are the current support tags

#define TIFFTAG_NEWSUBFILETYPE		254
#define	TIFFTAG_IMAGEWIDTH			256	
#define	TIFFTAG_IMAGELENGTH			257	
#define	TIFFTAG_BITSPERSAMPLE		258		/* bits per channel (sample) */
#define	TIFFTAG_COMPRESSION			259		
#define	    COMPRESSION_NONE		1		/* dump mode */
#define	    COMPRESSION_CCITTRLE	2		/* CCITT modified Huffman RLE */
#define	    COMPRESSION_LZW			5		/* Lempel-Ziv  & Welch */
#define	    COMPRESSION_PACKBITS	32773	/* Macintosh RLE */
#define	TIFFTAG_PHOTOMETRIC			262		/* photometric interpretation */
#define	    PHOTOMETRIC_MINISWHITE	0		/* min value is white */
#define	    PHOTOMETRIC_MINISBLACK	1		/* min value is black */
#define	    PHOTOMETRIC_RGB			2		/* RGB color model */
#define		PHOTOMETRIC_CMYK		5		/* CMYK color model */
#define		PHOTOMETRIC_LAB			8		/* LAB color model */
#define	TIFFTAG_FILLORDER			266		/* data order within a byte */
#define	    FILLORDER_MSB2LSB		1		/* most significant -> least */
#define	    FILLORDER_LSB2MSB		2		/* least significant -> most */
#define	TIFFTAG_STRIPOFFSETS		273		/* offsets to data strips */
#define	TIFFTAG_ORIENTATION			274		/* +image orientation */
#define	    ORIENTATION_TOPLEFT		1		/* row 0 top, col 0 lhs */
#define	    ORIENTATION_TOPRIGHT	2		/* row 0 top, col 0 rhs */
#define	    ORIENTATION_BOTRIGHT	3		/* row 0 bottom, col 0 rhs */
#define	    ORIENTATION_BOTLEFT		4		/* row 0 bottom, col 0 lhs */
#define	    ORIENTATION_LEFTTOP		5		/* row 0 lhs, col 0 top */
#define	    ORIENTATION_RIGHTTOP	6		/* row 0 rhs, col 0 top */
#define	    ORIENTATION_RIGHTBOT	7		/* row 0 rhs, col 0 bottom */
#define	    ORIENTATION_LEFTBOT		8		/* row 0 lhs, col 0 bottom */
#define	TIFFTAG_SAMPLESPERPIXEL		277		/* samples per pixel */
#define	TIFFTAG_ROWSPERSTRIP		278		/* rows per strip of data */
#define	TIFFTAG_STRIPBYTECOUNTS		279		/* bytes counts for strips */
#define TIFFTAG_XRESOLUTION			282
#define TIFFTAG_YRESOLUTION 		283 
#define	TIFFTAG_PLANARCONFIG		284		/* storage organization */
#define	    PLANARCONFIG_CONTIG		1		/* single image plane */
#define	    PLANARCONFIG_SEPARATE	2		/* separate planes of data */
#define	TIFFTAG_RESOLUTIONUNIT		296		/* units of resolutions */
#define	    RESUNIT_NONE			1		/* no meaningful units */
#define	    RESUNIT_INCH			2		/* english */
#define	    RESUNIT_CENTIMETER		3		/* metric */
#define TIFFTAG_PREDICTOR			317
#define TIFFTAG_TILEWIDTH			322
#define TIFFTAG_TILELENGTH			323
#define TIFFTAG_TILEOFFSETS			324
#define TIFFTAG_TILEBYTECOUNTS		325
#define TIFFTAG_DOTRANGE			336

//added by peter for new vesion of plug and batch
#define TIFFTAG_MONACO				351
//end of addition

//added by peter for photoshop file caption
#define TIFFTAG_PHOTO1				34377	//photoshop caption
#define TIFFTAG_PHOTO2				33723	//photoshop caption
//end of addition

typedef	struct _TIFFHeader {
	u_int16 	magic;	/* magic number (defines byte order) */
	u_int16 	version;	/* TIFF version number */
	u_int32  	diroff;	/* byte offset to first directory */
} TIFFHEADER, *TIFFHEADERPTR;

typedef	struct _TIFFDirEntry {
	u_int16 	tag;		/* see below */
	u_int16 	type;		/* data type; see below */
	u_int32  	count;		/* number of items; length in spec */
	u_int32  	offset;	/* byte offset to field data */
} TIFFDIRENTRY, *TIFFDIRENTRYPTR;

typedef	struct _TIFFDirectory{
	u_int32		newsubfiletype;	/* has default */
	u_int32		imagewidth;		/* must present */
	u_int32		imagelength;	/* must present */
	u_int16		bitspersample;	/* has default */
	u_int16		compression;		/* has  default */
	u_int16		photometric;		/* has default */
	u_int16		fillorder;		/* has default */
	u_int32		*stripoffset;
	u_int16		orientation;		/* has default */
	u_int16		samplesperpixel;	/* has default  */
	u_int32		rowsperstrip;	/* must present */
	u_int32		*stripbytecount;
	float		xresolution;
	float		yresolution;
	u_int32		xresolutionnumerator;
	u_int32		yresolutionnumerator;
	u_int32		xresolutiondenominator;
	u_int32		yresolutiondenominator;
	u_int16		planarconfig;	/* has default */
	u_int16		resolutionunit;	/* has default */
	u_int16		predictor;		/* has default */
	
	u_int32		tilewidth;
	u_int32		tilelength;
	u_int32		*tileoffset;
	u_int32		*tilebytecount;
		
	u_int16		inkset;			/* has default = 1 (CMYK) */
	unsigned char	dotrange0;		/* has default */
	unsigned char	dotrange1;		/* has default */

	u_int32		stripsperimage;
	u_int32		tilesperimage;	/* defaults 0 */
	u_int32		nstrips;		/* size of offset & bytecount arrays */

	//added by peter for new plug and filter
	char		*monacofield;
	//end

	//added by peter for photoshop file caption
	char		*caption1;
	char		*caption2;
	//end of modification
	
} TIFFDIRECTORY, *TIFFDIRECTORYPTR;

typedef	enum {
	TIFF_NOTYPE	= 0,	/* placeholder */
	TIFF_BYTE	= 1,	/* 8-bit unsigned integer */
	TIFF_ASCII	= 2,	/* 8-bit bytes w/ last byte null */
	TIFF_SHORT	= 3,	/* 16-bit unsigned integer */
	TIFF_LONG	= 4,	/* 32-bit unsigned integer */
	TIFF_RATIONAL	= 5,	/* 64-bit unsigned fraction */
	TIFF_SBYTE	= 6,	/* !8-bit signed integer */
	TIFF_UNDEFINED	= 7,	/* !8-bit untyped data */
	TIFF_SSHORT	= 8,	/* !16-bit signed integer */
	TIFF_SLONG	= 9,	/* !32-bit signed integer */
	TIFF_SRATIONAL	= 10,	/* !64-bit signed fraction */
	TIFF_FLOAT	= 11,	/* !32-bit IEEE floating point */
	TIFF_DOUBLE	= 12	/* !64-bit IEEE floating point */
} TIFFDataType;

typedef	struct {
	u_int16			tag;		/* field's tag */
	TIFFDataType 	type1;	/* type of associated data */
	TIFFDataType	type2;  /* handle the different type */
	u_int32			count;	
	
#ifndef PLUGIN
	char *			name;	
#else
	char			name[20];
#endif

} TagInfo;


class TiffFormat:public ConFileFormat {
	private:
	
	
	protected:
	u_int32		compressedimagesize;
	TIFFDIRECTORY	_tiffptr;
	McoHandle	compressionHandle;
	u_int16 	fileMode; //this should be in base class FileFormat
	int16		swab;
	int16		byteorder;
	u_int16		numofsupportedtags;
	u_int32		diroffset;
	u_int32		processedimagesize;
	u_int32		currentstrip;
	u_int32		readingstrip;
	u_int32		_readtilenums;

	u_int32		_currenttile;
	//added by Peter for tile images
	int16		_storeformat;
	//end
	
	//added by peter for new plug and filter
	u_int32		monacosize;
	//end

	//added by peter for photoshop caption
	u_int32		captionsize1;
	//end

	//added by peter for photoshop caption
	u_int32		captionsize2;
	//end

	McoStatus 	continueReaduncompressed(int16 *);
	McoStatus 	continueWriteuncompressed(int16 *);
	McoStatus 	continueReaduncomptile(int16 *);
	McoStatus 	continueWriteuncomptile(int16 *);

	void		setDefaulttag(void);	//Unlocks tiffHandle (did not unlock before James)
	void 		setSwab(int16);
	void		setByteorder(int16);
	void 		setNumofsupportedtags(u_int16);
	void		setDiroffset(u_int32);
	void		setProcessedimagesize(u_int32);
	void		setReadingstrip(u_int32);
	void    	setCurrentstrip(u_int32);
	void		_setCurrenttile(u_int32);
	u_int32		_getCurrenttile(void);
	void 		_setReadtilenums(u_int32);
	u_int32		_getReadtilenums(void);
	McoStatus	setStoreformat(int16);
	int16		getStoreformat(void);
	
	int16		getSwab(void);	
	int16		getByteorder(void);	
	McoStatus	getHeader(TIFFHEADERPTR);
	u_int16		getNumofsupportedtags(void);
	u_int32		getDiroffset(void);
	u_int32		getProcessedimagesize(void);
	u_int32		getReadingstrip();
	u_int32    	getCurrentstrip();
		
	void		swabShort(u_int16 *);
	void		swabLong(u_int32 *);
	void 		swabLongleftjusted(u_int32 * l);
	void		swabDir(TIFFDIRENTRYPTR);
	
	McoStatus 	checkTag(TIFFDIRENTRYPTR);
	McoStatus	adjustLow(TIFFDIRENTRYPTR);
	McoStatus	adjustHigh(TIFFDIRENTRYPTR);
	McoStatus	checkTiff(TIFFDIRECTORYPTR);
	McoStatus	checkGrayscale(TIFFDIRECTORYPTR);
	McoStatus	checkRGB(TIFFDIRECTORYPTR);
	McoStatus	checkCMYK(TIFFDIRECTORYPTR);
	McoStatus	checkLAB(TIFFDIRECTORYPTR);
	McoStatus	fetchBitspersample(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR); //Unlocks tiffHandle
	McoStatus	fetchStripoffset(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	fetchStripbytecounts(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	fetchXresolution(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	fetchYresolution(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	fetchTileoffset(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	fetchTilebytecounts(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);

	//added by peter for new plug and filter
	McoStatus	fetchMonaco(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	//end

	//added by Peter for photoshop caption
	McoStatus 	fetchCaption1(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	//end
	
	//added by Peter for photoshop caption
	McoStatus	fetchCaption2(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	//end

	McoStatus 	fetchImage(TIFFDIRECTORYPTR); //Unlocks image_dataHandle
	
	McoStatus 	packDirnewsubfiletype(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirimagewidth(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirimagelength(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirbitspersample(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);	
	McoStatus 	packDircompression(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirphotometric(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirfillorder(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirstripoffset(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirorientation(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirsamplesperpixel(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirrowsperstrip(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirstripbytecount(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus   packDirxresolution(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus   packDiryresolution(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirplanarconfig(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirresolutionunit(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirpredictor(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirtilewidth(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirtilelength(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirtilebytecount(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	packDirtileoffset(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus 	packDirdotrange(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);

	McoStatus   packImage(TIFFDIRECTORYPTR);

	//added by peter for new plug and filter
	McoStatus 	packDirmonaco(TIFFDIRECTORYPTR,TIFFDIRENTRYPTR);
	//end

	//add for photoshop caption1
	McoStatus	packDircaption1(TIFFDIRECTORYPTR,TIFFDIRENTRYPTR);
	
	//add for photoshop caption1
	McoStatus	packDircaption2(TIFFDIRECTORYPTR,TIFFDIRENTRYPTR);
		
/*	
	McoStatus 	calImagesize(u_int32*);
	void 		setCompressedimagesize(u_int32);
	McoStatus 	uncompressimage(char *);
	McoStatus 	compressimage(char *, u_int32);
*/	
	
	McoStatus	setStripsperimage(u_int32);
	McoStatus	setPredictor(u_int16);
	McoStatus 	setStripbytecount(void);
	McoStatus	setStripoffset(TIFFDIRECTORYPTR, TIFFDIRENTRYPTR);
	McoStatus	setTilesperimage(void);

	McoStatus 	getStripsperimage(u_int32 *);
	McoStatus	getPredictor(u_int16 *);
	McoStatus	getTilesperimage(u_int32 *);
	//u_int32	*	getStripbytecount(void);
	//u_int32	*	getStripoffset(void);

	McoStatus   setResolutionunit(u_int16);
		//1 is numerator, 2 is denominator
	McoStatus	setXresolution(u_int32, u_int32);
		//1 is numerator, 2 is denominator
	McoStatus	setYresolution(u_int32, u_int32);
	
	public:
	TiffFormat(void);	//Unlocks tiffHandle
	~TiffFormat(void);	//Unlocks tiffHandle
	
	McoStatus 	getSamplesperpixel(u_int16 *);
	McoStatus 	getWidth(u_int32 *);			//u_int16 or u_int32
	McoStatus 	getLength(u_int32 *);			//u_int16 or u_int32
	McoStatus 	getBitspersample(u_int16 *);
	McoStatus 	getPhotometric(u_int16 *);
	McoStatus 	getCompression(u_int16 *);
	McoStatus	getFillorder(u_int16 *);
	McoStatus	getOrientation(u_int16 *);
	McoStatus	getRowsperstrip(u_int32 *); 	//u_int16 or u_int32
	McoStatus	getPlanarconfig(u_int16 *);
	McoStatus   getResolutionunit(u_int16 *);
	McoStatus	getXresolution(u_int32 *, u_int32 *); //1 is numerator, 2 is denominator
	McoStatus	getYresolution(u_int32 *, u_int32 *); //1 is numerator, 2 is denominator
	McoStatus	getTilewidth(u_int32 *);
	McoStatus	getTilelength(u_int32 *);

	//added by peter for new plug and filter
	McoStatus	getMonaco(char *); //Unlocks tiffHandle
	McoStatus	getMonacosize(long *size);
	//end
	//added by James 
	McoStatus 	getMonacoJob(char *jobP,int t);
	//end

/*
	//add for photoshop caption1
	McoStatus	setCaption1(char *caption, int32 size);
*/
	
	//add for photoshop caption2
	McoStatus	getCaption2(char *, u_int32 *);
	
	McoStatus 	getImage(char *);	//Unlocks image_dataHandle
	McoStatus 	getImagehandle(McoHandle *);
	McoStatus 	getImagesize(u_int32 *);
/*	
	McoStatus	getCompressedimagesize(u_int32 *);
*/
		
	McoStatus 	setSamplesperpixel(u_int16);
	McoStatus 	setWidth(u_int32);
	McoStatus 	setLength(u_int32);
	McoStatus 	setBitspersample(u_int16);
	McoStatus 	setPhotometric(u_int16);
	McoStatus 	setCompression(u_int16);
	McoStatus	setFillorder(u_int16);
	McoStatus	setOrientation(u_int16);
	McoStatus	setRowsperstrip(u_int32);
	McoStatus	setPlanarconfig(u_int16);
	McoStatus 	setResolution(u_int16, u_int32, u_int32, u_int32, u_int32);	
	McoStatus 	setResolutionfromphoto(u_int16, u_int32, u_int32, u_int32, u_int32);	
 	McoStatus 	setImage(char *, u_int32);
	McoStatus	setTilewidth(u_int32);
	McoStatus	setTilelength(u_int32);
 	McoStatus	setImagesize(u_int32);			//Unlocks image_dataHandle

	//added by peter for new plug and filter
	McoStatus	setMonaco(char *, int32 size);
	//end

	//add for photoshop caption1
	McoStatus	setCaption1(char *caption, int32 size);
	
	//add for photoshop caption2
	McoStatus	setCaption2(char *caption, int32 size);
	
	//add by peter for sequential read and write
	McoStatus	prepareRead(void);
	McoStatus	startRead(McoHandle, int32);
	McoStatus	continueRead(int16 *);
		
	McoStatus	prepareWrite(void);
	McoStatus	startWrite(McoHandle, int32);
	McoStatus	continueWrite(int16 *);
};	
		
#endif //_TIFF_FORMAT_NEW_H_