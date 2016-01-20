////////////////////////////////////////////////////////////////////////////////
//	rawdata.h																  //
//																			  //
//	The code for dealing with the format of the raw data					  //
//	James Vogh																  //
//  Nov 18, 1996															  //
////////////////////////////////////////////////////////////////////////////////



#ifndef IN_RAWDATA
#define IN_RAWDATA


#include "mcostat.h"
#include "mcotypes.h"
#include "patchformat.h"

#include "mcoicc.h"
#include <stdio.h>


#define CHARTNAME_VER "MonacoChart 1.0"

// The commands

#define COMMAND_TAG "Tag:"
#define COMMAND_FORMAT "Format:"
#define COMMAND_DATA "Data:"

#define COMMENT	"*"

/// The tags

#define TAG_DATE "Date:"
#define TAG_CHARTCOLORS "ChartColorSpace:"
#define TAG_PRINTERNAME "Printer:"
#define TAG_PAPER "Paper:"
#define TAG_INK "Ink:"
#define TAG_ORDERING "Ordering:"

#define MaxTagTypes 256
// The colorspaces

#define COLORSPACE_CMYK "CMYK"
#define COLORSPACE_RGB "RGB"

// The ordering formats

#define ORDERING_MCYK "MCYK"
#define ORDERING_RGB "RGB"

// The formats

#define FORM_BLOCKTYPE "BlockType:"

typedef enum {
	No_Patches = 0,
	CMYKPatches,
	RGBPatches} PatchType;
	
typedef enum {
	Order_MCYK = 0,
	Order_RGB
	} OrderingType;

typedef enum {
	Tag_Date = 0,
	Tag_ColorSpace,
	Tag_PrinterName,
	Tag_Paper,
	Tag_Ink,
	Tag_Ordering,
	Tag_Unknown } TagTypes;

struct Tag {
	TagTypes	type;
	char tagData[255];
	};


// format of the data

struct  Format{
	// Total number of components
	int32		total;
	// Num of components
	int32		numc;
	// Number of the the slowest changing cmp
	int32		numslow;
	// Values of the slowest changing
	double		*slowValues;
	// For each value of slow, the dimensions of the hypercube
	int32		*cubedim;
	// values for each of the components in the hypercube
	double 		*cmpValues;
	// the number of values used for linearization
	int32		numLinear;
	// the values for linearization data
	double		*linearValues;
	};

// defines the layout of the patches

struct Patches{
	// 1 = strips, 0 = rectangle
	int		strips;
	// number of strips/sheets
	int		sheets;
	// number or rows
	int		rows;
	// number of columns
	int   	columns;
	// starting position
	//    0			1
	//
	//	  2			3
	int		starting;
	// direction  0 = horz, 1 = vert
	int 	direction;
	//width in mm
	double	width;
	//height in mm
	double height;
	};
	

class RawData {
private:
protected:
public:
	// the type of patch data
	PatchType	type;
	// the number of tags
	int32		numTags;
	// the tag data 
	Tag			tags[MaxTagTypes];
	// the format of the data
	Format		format;
	// the format of patches
	Patches		patches;
	// the ordering of the data
	OrderingType	ordering;
	// the actual data
	McoHandle	dataH;
	// the linearization data
	McoHandle	linearDataH;
	// A text descriptiion
	char		desc[100];
	
	unsigned long date;	// the date the data was measured in window's format (seconds since jan 1 1970)
	
	RawData(void);
	~RawData(void);
	
	// set up from an external source (like the XRITE)
	McoStatus SetUp(int32 ty,int32 numc,int32 slow,int32 linear,int32 *dims);
	
	// get the value of the component for slow and num
	int32 getcmpValues(int32 slow,int32 num);
	
	// get the index in data for slow value of k
	int32 getDataIndex(int32 k);
	
	// get the lab of the paper
	McoStatus getPaperLab(double *lab);
	
	// a multidimensional make refrence
	// for CMYK data generates a table of CMYK that matches the 
	// CMYK values used to create the raw data
	McoStatus makeRef(double *ref);

	// Smooth the lab space of the patches, smoothing only the L channel
	McoStatus smooth_patch(void);
	
	// perform a combination of median filtering and smoothing
	McoStatus median_patch(void);
	
	// Do four dimensional interpolation
	McoStatus doFourDInterp(double *cmyktable,int32 nums, double *vals,int useLuts);

	// Do four dimensional interpolation
	// with a different level of K values
	McoStatus doFourDInterp(double *cmyktable,int32 nums, int32 numsK, double *vals, double *valsK);

	// do threeD interpolation
	McoStatus doThreeDInterp(double *cmyktable,int32 nums, double *vals);
	
	// copy the contents of 
	McoStatus copyRawData(RawData *tocopy);
	
	// copy just the patch information
	McoStatus copyPatchData(RawData *tocopy);

	// copy from the PatchFormat structure
	McoStatus copyOutOfPatchData(PatchFormat *tocopy);
	
	// copy into the PatchFormat structure
	McoStatus copyIntoPatchData(PatchFormat *tocopy);
	
	// see if the formats are the same
	int	sameFormat(RawData *data);
	
	// save the data to a text file
	McoStatus saveData(FILE *ofsb);
	
	// load the data from a file
	McoStatus loadData(FILE *fs);
	
	McoStatus loadStruct(void *data);
	
	// set the description
	McoStatus SetDesc(char *d);
	
	// get the description
	McoStatus GetDesc(char *d);
	
	McoStatus readICCFile(McoXformIcc *link);

	};
	
	
#endif
	
	
	
	
	
	