//-------------------------------------------------------------------------------
//
//	File:
//		PIUtilities.r
//
//	Copyright 1993-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This rez file contains standard resources
//		to simplify the use of suites and also some
//		helpful common plug-in resources. 
//
//	Use:
//		PIUtilities is intended to group common resources
//		and functions into a simple library that provides
//		most of the basic plug-in functionality, including
//		error reporting and About boxes.
//
//	Version history:
//		Version 1.0.0	3/1/1993	Created for Photoshop 2.5
//			Written by Mark Hamburg
//
//		Version 1.0.1	4/8/1997	Updated for Photoshop 4.0.1
//			Comments fattened out.  Reorganized for clarity.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Defines -- Version information for SDK releases.
//-------------------------------------------------------------------------------

#ifndef VersionString
	#define VersionString 	"5.0"
#endif

#ifndef ReleaseString
	#define ReleaseString	"1 (4/1998)"
#endif

#ifndef CurrentYear
	#define CurrentYear		"1998"
#endif

//-------------------------------------------------------------------------------
//	Defines -- Generic resources, such as about boxes and resource IDs.
//	Redefine these if you need to.
//-------------------------------------------------------------------------------

#ifndef StringResource
	#define StringResource	'STR '
#endif

#ifndef ResourceID
	#define ResourceID		16000
#endif

#ifndef AboutID
	#define AboutID			ResourceID
#endif

#ifndef uiID
	#define uiID			ResourceID+1
#endif

#ifndef ADMAboutID
	#define ADMAboutID		uiID+1
#endif

#ifndef AlertID
	#define AlertID			16990
#endif

#ifndef kBadNumberID
	#define kBadNumberID	AlertID
#endif

#ifndef kBadDoubleID
	#define kBadDoubleID	kBadNumberID+1
#endif

#ifndef kNeedVers
	#define kNeedVers		kBadDoubleID+1
#endif

#ifndef kWrongHost
	#define kWrongHost		kNeedVers+1
#endif

#ifndef kSuiteMissing
	#define kSuiteMissing	kWrongHost+1
#endif

//-------------------------------------------------------------------------------
//	Alert strings.
//-------------------------------------------------------------------------------

resource StringResource (kBadNumberID, "Bad number", purgeable)
{
	"An integer is required between ^1 and ^2."
};

resource StringResource (kBadDoubleID, "Bad double", purgeable)
{
	"A decimal number is required between ^1 and ^2."
};

resource StringResource (kNeedVers, "Need vers", purgeable)
{
	"This plug-in requires Adobe Photoshop¨ ^1 or later functionality."
};

resource StringResource (kWrongHost, "Wrong host", purgeable)
{
	"This plug-in is incompatible with this version of the host program."
};

resource StringResource (kSuiteMissing, "Suite missing", purgeable)
{
	/* The host cannot complete this command because... */
	"the required suite \"^0\" is unavailable" /* period is added by host */
};

//-------------------------------------------------------------------------------
//
//	'FltD' resource.  Dynamic resource for Adobe Premiere.
//
//	Descriptor for allowing filters to animate over time. A
//	structure of this type can be added to a 'VFlt', an 'AFlt',
//	or a Photoshop filter to describe the data structure of its
//	data blob. Specify pdOpaque for any non-scalar data in the
//	record, or data that you don't want Premiere to interpolate
//	for you. Make the FltD describe all the bytes in the data
//	blob. Use ID 1000D
//
//-------------------------------------------------------------------------------

type 'FltD' {
	array {
		integer					// Specifies the type of the data
			pdOpaque = 0,			// Opaque - don't interpolate this
			pdChar = 1,				// Interpolate as signed byte
			pdShort = 2,			// Interpolate as signed short
			pdLong = 3,				// Interpolate as signed long
			pdUnsignedChar = 4,		// Interpolate as unsigned byte
			pdUnsignedShort = 5,	// Interpolate as unsigned short
			pdUnsignedLong = 6,		// Interpolate as unsigned long
			pdExtended = 7,			// Interpolate as an extended
			pdDouble = 8,			// Interpolate as a double
			pdFloat = 9;			// Interpolate as a float
		integer;				// Count of bytes to skip with pdOpaque, 0 otherwise.
	};
};

//-------------------------------------------------------------------------------

// end PIUtilities.r
