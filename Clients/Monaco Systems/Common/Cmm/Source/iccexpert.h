#ifndef ICCEXPERT_H
#define ICCEXPERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "csprofile.h"

/*************************************************************
 * Define Usefull types for function prototypes
 *************************************************************/

typedef struct ICCinfo {
	unsigned long		profileClass;			// input, display, output, devicelink, abstract, or color conversion profile type
	unsigned long		dataColorSpace;			// color space of data 
	unsigned long		profileConnectionSpace;	// profile connection color space 
	unsigned long		devtocon;							// Boolean Flag	Does dev->PCS exist?
	unsigned long       dcInputFormat;      // Format of input channel values
	unsigned long       dcOutputFormat;     // Format of output channel values
	unsigned long 		dcinputChannels;		// Number of input channels 
	unsigned long		dcoutputChannels;		// Number of output channels 
	unsigned long		dcgridPoints;			// Number of clutTable grid points 
	unsigned long		dcbits;					// Number of bits per channel 
	unsigned long		dclutsize;				// lut size 
	unsigned long		contodev;            // Boolean Flag Does PCS->dev exist?
	unsigned long       cdInputFormat;      // Format of input channel values
	unsigned long       cdOutputFormat;     // Format of output channel values
	unsigned long 		cdinputChannels;		// Number of input channels 
	unsigned long		cdoutputChannels;		// Number of output channels 
	unsigned long		cdgridPoints;			// Number of clutTable grid points 
	unsigned long		cdbits;					// lut size 
	unsigned long		cdlutsize;				// lut size
	char				copystr[128];			// copy right string
}ICCinfo;

/*********************************************************************
 * Define Pixel format Macros
 *
 * Note: A pixel format is made up of a Photometric Interpretation or'd
 *       with the number of samples in the pixel
 *********************************************************************/
#define MONACO_FMT_MINISBLACK      0x0000
#define MONACO_FMT_MINISWHITE      0x0100
#define MONACO_FMT_XYZMODE         0x0200
#define MONACO_FMT_LABMODE         0x0400
#define MONACO_FMT_NUMSAMPLEONLY   0x0800

#define MONACO_FMT_TYPE_MASK       0xff00
#define MONACO_FMT_SAMPLE_MASK		0x00ff

// Define a special Pixel Format (using: CMS_FMT_MINISBLACK with no samples)
#define MONACO_FMT_UNKNOWN         0

//Note: Struct _COLOR_XFORM_INFO should contain header info
//      from the icc file being parsed

typedef struct {
	CM2Header	header;
	ICCinfo		info;
} COLOR_XFORM_INFO;

//Note: Xform Obj contains Info about and data to perform
//      transform
typedef struct {
  COLOR_XFORM_INFO Info;
  void *Interpolator;
  void *Data;
} COLOR_XFORM_OBJ;

typedef struct IccProfileSet {
	unsigned short 		keyIndex;		//Zero-based
	unsigned short 		count;			//Min 1
	long				intent;			//rendering intent
	CsFormat 			*profileSet[4];	//Variable. Ordered from Source -> Dest
}IccProfileSet;


class IccExpert{
private:
	long _grids;
	long _in_comp;	//input components such as 3 or 4 for(LAB and CMYK)
	long _out_comp;	//output components
	long _level;	//level of accuracy, not used now
	long _in_data;	//in data format (LAB, CMYK or ??)
	long _out_data;	//out data format
	McoHandle _tableH;	//conversion table, size 17 char

	long _check_connect_to_dev_tags(CsFormat *prof, CM2Header *header);
	long _check_connect_to_dev(CsFormat *prof, long pspace, CM2Header *header);
	long _check_dev_to_connect_tags(CsFormat *prof, CM2Header *header);
	long _check_dev_to_connect(CsFormat *prof, long pspace, CM2Header *header);
	McoStatus _checklink(IccProfileSet *profset);
	void _load_gamma(CMCurveType *ch, unsigned short *curv, double *gammat);
	void _load_rev_gamma(CMCurveType *ch, unsigned short *curv, double *gammat);

	McoStatus _load_lut_tag(CsFormat *prof, CM2Header *header, long tag, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_tag(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_LAB_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_RGB_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_CMYK_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_CMY_to_XYZ(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_LAB_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_RGB_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_CMYK_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_CMY_to_LAB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_link_tag(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_to_RGB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_to_CMYK(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_XYZ_to_CMY(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_LAB_to_RGB(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_LAB_to_CMYK(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_LAB_to_CMY(CsFormat *prof, CM2Header *header, long dev_to_con, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_dev_to_connect(CsFormat *prof, CM2Header *header, McoHandle* h, CMLut16Type* lut);
	McoStatus _load_connect_to_dev(CsFormat *prof, CM2Header *header, McoHandle* h, CMLut16Type* lut);
	McoStatus _buildlink(IccProfileSet *profset);
	McoStatus _size(McoHandle* tableH, CMLut16Type	*luttag, 
		long informat, long outformat);
	McoStatus _convert(McoHandle tableH, McoHandle srcH, McoHandle desH, 
		CMLut16Type *luttag, long informat, long outformat);

	void _convert_format(unsigned long devspace, unsigned long conspace, ICCinfo *info, long devtocon);

	//for test only
/*	
	McoStatus _testCMYK(unsigned char *in, long width, long length);
	McoStatus _testRGB(unsigned char *in, long width, long length);
	McoStatus _testLAB(unsigned char *in, long width, long length);
*/
public:
IccExpert(void);
~IccExpert(void);

//build expert based on input profile list
McoStatus buildexp(IccProfileSet *profset);

//evaluation on color data
McoStatus eval(unsigned char *in, unsigned char *out,int32 num);

//get table
void getTable(unsigned char *table);
void getInfo(ICCinfo *info);
};


#endif