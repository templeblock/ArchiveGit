//-------------------------------------------------------------------------------
//
//	File:
//		PIProperties.h
//
//	Copyright 1994-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file contains the public definitions and structures
//		for the properties callback suite.
//
//	Use:
//		If you are working with any properties via the properties
//		or action get mechanisms, this documents what is available
//		and its basic format.
//
//	Version history:
//		Version 1.0.0	1/1/1994	MH		Created.
//			Created for Photoshop 3.0.
//
//		Version 1.5.0	1/1/1996	MP		Photoshop 4.0.
//			Added Photoshop 4.0 properties.
//
//		Version 2.0.0	4/1/1997	Ace		Photoshop 5.0.
//			Updated for new headers for Photoshop 5.0.
//
//-------------------------------------------------------------------------------

#ifndef __PIProperties_h__ // Already defined?
#define __PIProperties_h__

//-------------------------------------------------------------------------------

// Get the number of channels.  Returns the number of channels in the simple
// property field:   
#define propNumberOfChannels  'nuch'

// Returns a handle containing the characters for the name of the channel:
#define propChannelName		  'nmch'

// Returns the image mode using the plugInMode constants.  (Simple):
#define propImageMode		  'mode'

// Returns the number of channels including the work path. (Simple):
#define propNumberOfPaths	  'nupa'

// Returns the name of the indexed path (zero-based). (Complex):
#define propPathName          'nmpa'

// Returns the contents of the indexed path (zero-based). (Complex):
#define propPathContents      'path'

// The index of the work path.  -1 if no work path. (Simple):
#define propWorkPathIndex     'wkpa'

// The index of the clipping path.  -1 if none. (Simple):
#define propClippingPathIndex 'clpa'

// The index of the target path.  -1 if none. (Simple):
#define propTargetPathIndex   'tgpa'

// The file meta information is an IPTC-NAA record (Complex, Modifiable):
#define propCaption			  'capt'

// The big nudge distance. Horizontal and vertical components. These are
// represented as a 16.16 values. (Simple, Modifiable):
#define propBigNudgeH		  'bndH'
#define propBigNudgeV		  'bndV'

// The current interpolation method: 1 = point sample, 2 = bilinear, 3 = bicubic
// (Simple):
#define propInterpolationMethod 'intp'

// The current ruler units. (Simple):
#define propRulerUnits		    'rulr'

// Ruler units will be one from this enum:
typedef enum
	{
	kRulerPixels,
	kRulerInches,
	kRulerCm,
	kRulerPoints,
	kRulerPicas,
	kRulerPercent
	} RulerUnits;

// The current ruler origin.  Horizontal and vertical components.  These are
// represented as 16.16 values. (Simple, Modifiable):
#define propRulerOriginH		'rorH'
#define propRulerOriginV		'rorV'

// The current major grid rules, in inches, unless propRulerUnits is pixels, and
// then pixels.  Represented as 16.16 value. (Simple, Modifiable):
#define propGridMajor			'grmj'

// The current number of grid subdivisions per major rule (Simple, Modifiable):
#define propGridMinor			'grmn'

// The serial number string shown to the user. (Complex):
#define propSerialString		'sstr'

// The hardware gamma table. (PC Only) (Complex):
#define propHardwareGammaTable	'hgam'

// The interface Color scheme:
#define propInterfaceColor		'iclr'

// The watch suspension level. When non-zero, you can make callbacks to the host
// without fear that the watch will start spinning. It is reset to zero at the
// beginning of each call from the host to the plug-in. (Simple, modifiable):
#define propWatchSuspension		'wtch'

// Whether the current image is considered copywritten (Simple, Modifiable).
// These both access the same property. The first can only be used to
// set the flag.
#define propCopyright			'cpyr'
#define propCopyright2			'cpyR'

// Indicate whether a digital signature or watermark is present (Simple, Modifiable).
// The (c) copyright symbol will appear if this is set, OR if the user has checked
// the copyright property in the File Info dialog.  Do NOT turn the copyright
// flag off, ever.  Use propWatermark to indicate if you've found your digital
// signature or not:
#define propWatermark			'watr'

// The URL for the current image (Complex, Modifiable):
#define propURL					'URL '

// The title of the current document (Complex):
#define propTitle				'titl'

// The width of the current document in pixels (Simple):
#define propDocumentWidth		'docW'

// The height of the current document in pixels (Simple):
#define propDocumentHeight		'docH'

//-------------------------------------------------------------------------------

#endif // __PIProperties_h__
