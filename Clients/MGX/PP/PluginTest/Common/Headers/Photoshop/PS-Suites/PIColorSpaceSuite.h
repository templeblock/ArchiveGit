/*
 * Name:
 *	PIColorSpaceSuite.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Photoshop ColorSpace Suite for Plug-ins.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1998	JF	First Version.
 *		Created by Josh Freeman.
 */

#ifndef __PIColorSpaceSuite__
#define __PIColorSpaceSuite__

#include "PIGeneral.h"
#include "ASZStringSuite.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define kPSColorSpaceSuite 			"Photoshop ColorSpace Suite for Plug-ins"

//-------------------------------------------------------------------------
//	ColorSpace suite typedefs 
//-------------------------------------------------------------------------

// These are defined in PIGeneral.h
/*
	Constants for colorSpace fields
	
#define plugIncolorServicesRGBSpace		0
#define plugIncolorServicesHSBSpace		1
#define plugIncolorServicesCMYKSpace	2
#define plugIncolorServicesLabSpace		3
#define plugIncolorServicesGraySpace	4
#define plugIncolorServicesHSLSpace		5
#define plugIncolorServicesXYZSpace		6
*/

typedef struct Color_T *ColorID;

// formatted as 0RGB, CMYK, 0LAB, 0XYZ, 0HSB, 0HSL, 000Gray
typedef unsigned char 	Color8[4];
typedef unsigned short 	Color16[4];


typedef struct
{
	unsigned short x;
	unsigned short y;
	unsigned short z;
} CS_XYZColor;

typedef SPAPI SPErr (*ColorSpace_Make) (ColorID *id);

typedef SPAPI SPErr (*ColorSpace_Delete) (ColorID *id);

typedef SPAPI SPErr (*ColorSpace_StuffComponents) (ColorID id, short colorSpace, 
					unsigned char component0, unsigned char component1, 
					unsigned char component2, unsigned char component3);

typedef SPAPI SPErr (*ColorSpace_ExtractComponents) (ColorID id, short colorSpace,
					unsigned char *component0, unsigned char *component1, 
					unsigned char *component2, unsigned char *component3, Boolean *gamutFlag);
					
typedef SPAPI SPErr (*ColorSpace_StuffXYZ) (ColorID id, CS_XYZColor xyz);

typedef SPAPI SPErr (*ColorSpace_ExtractXYZ) (ColorID id, CS_XYZColor *xyz);

typedef SPAPI SPErr (*ColorSpace_GetNativeSpace) (ColorID id, short *colorSpace);

typedef SPAPI SPErr (*ColorSpace_Convert8) (short inputCSpace, short outputCSpace,
											Color8 *colorArray, short count);

typedef SPAPI SPErr (*ColorSpace_Convert16) (short inputCSpace, short outputCSpace,
											Color16 *colorArray, short count);

typedef SPAPI SPErr (*ColorSpace_IsBookColor) (ColorID id, Boolean *isBookColor);
typedef SPAPI SPErr (*ColorSpace_ExtractColorName) (ColorID id, ASZString *colorName);

typedef SPAPI SPErr (*ColorSpace_PickColor) (ColorID *id, ASZString promptString);


// these use the Photoshop internal 16 bit range of [0..32768]
// data can be single or multiple channels, as long as the count includes all of it
typedef SPAPI SPErr	(*ColorSpace_Convert8to16)( unsigned char *input_data,
												unsigned short *output_data,
												short count );

typedef SPAPI SPErr	(*ColorSpace_Convert16to8)( unsigned short *input_data,
												unsigned char *output_data,
												short count );


//-------------------------------------------------------------------------
//	ColorSpace suite version 1
//-------------------------------------------------------------------------

#define kPSColorSpaceSuiteVersion1		1

typedef struct 
{
	ColorSpace_Make					Make;
	ColorSpace_Delete				Delete;
	ColorSpace_StuffComponents		StuffComponents;
	ColorSpace_ExtractComponents	ExtractComponents;
	ColorSpace_StuffXYZ				StuffXYZ;
	ColorSpace_ExtractXYZ			ExtractXYZ;
	ColorSpace_Convert8 			Convert8;
	ColorSpace_Convert16			Convert16;
	ColorSpace_GetNativeSpace		GetNativeSpace;
	ColorSpace_IsBookColor			IsBookColor;
	ColorSpace_ExtractColorName		ExtractColorName;
	ColorSpace_PickColor			PickColor;
	ColorSpace_Convert8to16			Convert8to16;
	ColorSpace_Convert16to8			Convert16to8;
	
} PSColorSpaceSuite1;

/******************************************************************************/

#ifdef __cplusplus
}
#endif


#endif	// PIColorSpaceSuite
