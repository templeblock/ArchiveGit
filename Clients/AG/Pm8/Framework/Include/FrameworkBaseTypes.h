// ****************************************************************************
//
//  File Name:			FrameworkBaseTypes.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of basic framework types
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/FrameworkBaseTypes.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//										  
// ****************************************************************************

#ifndef		_FRAMEWORKTYPESBASE_H_
#define		_FRAMEWORKTYPESBASE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// Mouse messages
enum EMouseMessages
{
	kLeftButtonDown = 0, kLeftButtonDoubleClick,		kLeftButtonUp,
	kMiddleButtonDown,	kMiddleButtonDoubleClick,	kMiddleButtonUp,
	kRightButtonDown,		kRightButtonDoubleClick,	kRightButtonUp,
	kMouseMove,				kContextMenu,					kSetCursor
};

// Modifier key, such as shift, control, command. Used to modify mouse clicks
// for example.
#ifdef	_WINDOWS
	typedef UINT YModifierKey;
	const YModifierKey kModifierShift			= MK_SHIFT;
	const YModifierKey kModifierControl			= MK_CONTROL;
	const YModifierKey kModifierLeftButton		= MK_LBUTTON;
	const YModifierKey kModifierMiddleButton	= MK_MBUTTON;
	const YModifierKey kModifierRightButton	= MK_RBUTTON;
#else		// MAC
	typedef uWORD YModifierKey;
	const YModifierKey kModifierShift		= shiftKey;
	const YModifierKey kModifierControl		= cmdKey;
#endif	// _WINDOWS

typedef int			YCounter;
typedef int			YComponentIndex;

// Measurement types
typedef		sLONG			YIntDegrees;
typedef		sLONG			YIntDimension;
typedef		sLONG			YIntCoordinate;

typedef		YFloatType	YRealDegrees;
typedef		YFloatType	YRealDimension;
typedef		YFloatType	YRealCoordinate;
typedef		YFloatType	YPercentage;			//	valid range is 0.0 -> 1.0 inclusive
typedef		YFloatType	YScaleFactor;

// Angles
typedef		YFloatType	YAngle;
const			YFloatType	kPI	= asin( 1.0 ) * 2.0;
const			YFloatType	k2PI	= 2.0 * kPI;

// Resource types. These must be a value between 1 and 32767...
typedef	uWORD		YResourceId;		//	use yrid
typedef	uWORD		YResourceIdx;		//	use yridx - for indexed resources

//	System Specific structures to a common Renaissance Typedef
typedef	HNATIVE		YResource;		//	use yres - for unknown resource types

#ifdef	_WINDOWS
typedef	uLONG			YResourceType;
typedef	HINSTANCE	YResourceFileReference;
typedef	HCURSOR		YResourceCursor;
typedef	HBITMAP		YResourceBitmap;
typedef	HGLOBAL		YResourceDialogTemplate;
typedef	HMENU			YResourceMenu;
#else
typedef	ResType		YResourceType;
typedef	uWORD			YResourceFileReference;
typedef	CursHandle	YResourceCursor;
typedef	PicHandle	YResourceBitmap;
typedef	DialogTHndl	YResourceDialogTemplate;
typedef	MenuHandle	YResourceMenu;
#endif

// Component attributes
typedef uLONG YUniqueId;
typedef sLONG YZLayer;

// Timing types
typedef uLONG YTickCount;

// Data transfer types
typedef uLONG YDataLength;

typedef	uLONG		YAlertFlags;

// File I/O types
typedef uLONG			YChunkTag;
typedef uWORD			YHeaderSize;
typedef uLONG			YFlags;
typedef uLONG			YChunkId;
typedef uBYTE			YVersion;
typedef uLONG			YAuxSize;
//	In a chunk header this is the default data version used for all currently known chunk contents.
const YVersion kDefaultDataVersion =	1;
//	In a chunk search this version indicates we are not searching on the version.
const YVersion kDontCareDataVersion =	0;


// Menu types
typedef int	YMenuIndex;

#ifdef _WINDOWS
	typedef UINT YDataFormat;
#else
	typedef OSType YDataFormat;
#endif

typedef uLONG	YInterfaceId;

// Frame types
enum EFrameType { kNoFrame = 0, 
						kThinLineFrame, kMediumLineFrame, kThickLineFrame, 
						kDoubleLineFrame, kMediumDoubleLineFrame, kThickDoubleLineFrame, 
						kRoundCornerFrame, 
						kDropShadowFrame, kPictureFrame, 
						kNumFrameTypes };

// Reason for saving
enum EWriteReason { kSaving, kCopying, kPrintingWrite, kPrintShopQslPhotoSaveByRef };

// Reason for loading
enum EReadReason	{ kLoading, kPasting, kPrintingRead };

// Reason for updating scroll bars
enum EScrollUpdateReason { kResize, kZoom, kChangeObjectPosition, kScroll };

// Component activate enum
enum EActivationMethod { kSingleClick, kDoubleClick, kEnterKey, kMenu };

//	Alert return values
enum YAlertValues { kAlertOk = 1, kAlertCancel, kAlertYes, kAlertNo };

//	Graphic Browser CD swap alert return values
enum YGBCDSwapAlertValues { kGBAlertOk = 1, kGBAlertCancel, kGBOtherGraphics };

// Document type id
typedef uLONG YDocumentType;

// Drag&Drop types
#ifdef _WINDOWS
	typedef DROPEFFECT YDropEffect;

	const YDropEffect kDropEffectMove =		DROPEFFECT_MOVE;
	const YDropEffect kDropEffectCopy =		DROPEFFECT_COPY;
	const YDropEffect kDropEffectNone =		DROPEFFECT_NONE;
	const YDropEffect kDropEffectScroll =	DROPEFFECT_SCROLL;
#endif
					  
// Drawing surface types
typedef int					YPointCount;
typedef int					YPolygonCount;
typedef int					YPolylineCount;
typedef uWORD				YPenStyle;
typedef YIntDimension	YPenWidth;

typedef sWORD				YTint;
const YTint kMaxTint = 100;
const YTint kMinTint = 0;

typedef		uWORD		YDrawMode;

#ifdef	_WINDOWS
	const YDrawMode		kNormal		= R2_COPYPEN;
	const YDrawMode		kXOR			= R2_XORPEN;
	const YDrawMode		kXNOR			= R2_NOTXORPEN;
#else
	const YDrawMode		kNormal		= 0x0000;
	const YDrawMode		kXOR			= 0x0001;
	const YDrawMode		kXNOR			= 0x0002;
#endif

// Commands send from menus and other controls
#ifdef	_WINDOWS
	typedef		UINT		YCommandID;
#else		// MAC
	typedef		uWORD		YCommandID;
#endif	// _WINDOWS

// Storage types and stream
enum EAccessMode { kRead, kWrite, kReadWrite, kWriteReplaceExisting, kReadWriteReplaceExisting };
typedef uLONG YStreamPosition;
typedef sLONG YStreamOffset;
typedef uLONG YStreamLength;

//	View update notification types
enum EUpdateTypes
{
	kUpdateEverything = 0,
	kLayoutChanged,
	kViewZoomChanged
};

//	Language versions
enum ELanguageVersions		{ kDefaultLanguage=0, kEnglishLanguage=1, kEuropeanLanguage=2 };

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _FRAMEWORKTYPES_H_
