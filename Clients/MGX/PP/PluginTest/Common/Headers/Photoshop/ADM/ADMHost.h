/*
 * Name:
 *	ADMHost.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Standard Enviornment Configuration
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.2.0 3/9/1996	MF	Second version.
 *		Created by Matt Foster.
 */

#ifndef __ADMHost__
#define __ADMHost__

#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifndef __SPMessageData__
#include "SPMData.h"
#endif

#ifndef __SPAccess__
#include "SPAccess.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


/*
 * Constants
 */

#define kADMPluginName			"ADM Plugin"

#define kADMHostSuite			"ADM Host Suite"
#define kADMHostSuiteVersion1	1
#define kADMHostSuiteVersion2	2

#define kADMHostSuiteVersion	kADMHostSuiteVersion2	// The current version

#define kADMHostCaller 			"ADM Host"
#define kADMHostStartSelector 	"ADM Started"
#define kADMHostEndSelector 	"ADM Ending"
//#ifdef MODELESS_DIAOG_HACK
#define kADMHostBeginSet		"ADM Host Begin Set"
#define kADMHostBeginReset		"ADM Host Begin Reset"
#define kADMHostForceBeginSet	"ADM Host Force Begin Set"
#define kADMHostForceBeginReset	"ADM Host Force Begin Reset"

#ifndef kMaxHostNameLen
#define kMaxHostNameLen 256
#endif

/*	GetADMVersion returns the version number ADM is compiled with.  This number consists
	of a hi word which is the major version number, and a low word which is the minor
	revision number. 
	
	The macro ADM_VERSION can be used to make sure you are running the version
	you think you are.
*/
#define ADM_VERSION 0x00020019


/*
 * Types
 */
typedef struct ADMHostInterfaceMessage {

	SPMessageData d;

} ADMHostInterfaceMessage;


typedef void ASAPI (*ADMAGMCallibrationProc)(_t_AGMPortPtr port);
typedef void ASAPI (*ModifyPaletteLayoutBoundsProc)(ASRect *dim);


// This is a reference to the AppContext. It is never dereferenced.
typedef struct _t_ADMHostAppContextOpaque *ADMHostAppContextRef;

typedef ASErr ASAPI (*BeginHostAppContextProc) ( ADMPluginRef plugin, ADMHostAppContextRef *appContext );
typedef ASErr ASAPI (*EndHostppContextProc) ( ADMHostAppContextRef appContext );

typedef struct ADMHostAppContextProcs {
	BeginHostAppContextProc beginAppContext;
	EndHostppContextProc	endAppContext;
} 
ADMHostAppContextProcs;


// This is a reference to the ResContext. It is never dereferenced.
typedef struct _t_ADMResContextOpaque *ADMResContextRef;

// This is basically the same as the SPAccess suite call GetAccessInfo().  Use it on Windows
// to supply an instance handle and on Mac if you have highly managed resource chain.
typedef ASErr ASAPI (*GetResourceAccessInfoProc)( ADMPluginRef plugin, SPPlatformAccessInfo *info);

// These can be used to order the MAcresource chain (e.g. bring a plug-in to the top) and restore
// a previous order if necessary.  Restore doesn't necessarily have to do anything.  The resContext
// you return to the set function will be returned by restore if you need it.
typedef ASErr ASAPI (*SetResourceContextProc)( ADMPluginRef plugin, ADMResContextRef *resContext );
typedef ASErr ASAPI (*RestoreResourceContextProc)( ADMResContextRef resContext );

typedef struct ADMResourceAccessProcs {
	// Supply functions getResAccessInfo() *OR* set/resetResContext() 
	GetResourceAccessInfoProc 	getResAccessInfo;
		
	SetResourceContextProc 		setResContext;		
	RestoreResourceContextProc	resetResContext;
} 
ADMResourceAccessProcs;


// Overridable defaults for text items
typedef struct ADMDefaultTextInfo {
	/* Text */
	int maxTextLen;
	ADMJustify justify;

	/* Numerics */
	ADMUnits units;
	int numberOfDecimalPlaces;
	ASBoolean allowUnits;
	ASBoolean allowMath;
} ADMDefaultTextInfo;



/*
 * Suite
 */

typedef struct ADMHostSuite1
{
	void ASAPI (*SetAGMCallibrationProc)(ADMAGMCallibrationProc callibrationProc);
	ADMAGMCallibrationProc ASAPI (*GetAGMCallibrationProc)();
	ASBoolean ASAPI (*PointerInUse)();
	void ASAPI (*SetModifyPaletteLayoutBoundsProc)(ModifyPaletteLayoutBoundsProc modifyBoundsProc);
	ModifyPaletteLayoutBoundsProc ASAPI (*GetModifyPaletteLayoutBoundsProc)();
}
ADMHostSuite1;

typedef struct ADMHostSuite2
{
	void ASAPI (*SetAGMCallibrationProc)(ADMAGMCallibrationProc callibrationProc);
	ADMAGMCallibrationProc ASAPI (*GetAGMCallibrationProc)();
	ASBoolean ASAPI (*PointerInUse)();
	void ASAPI (*SetModifyPaletteLayoutBoundsProc)(ModifyPaletteLayoutBoundsProc modifyBoundsProc);
	ModifyPaletteLayoutBoundsProc ASAPI (*GetModifyPaletteLayoutBoundsProc)();

	void ASAPI (*SetADMHostAppContextProcs)(ADMHostAppContextProcs *appContextProcs);
	void ASAPI (*GetADMHostAppContextProcs)(ADMHostAppContextProcs *appContextProcs);
	
	void ASAPI (*SetADMHostAppName)(char *hostName);
	void ASAPI (*GetADMHostAppName)(char *hostName, short length);

	void ASAPI (*SetADMResourceAccessProcs)(ADMResourceAccessProcs *resProcs);
	void ASAPI (*GetADMResourceAccessProcs)(ADMResourceAccessProcs *resProcs);

	ASUInt32 ASAPI (*GetADMVersion)( void );
	
	void ASAPI (*SetADMDefaultTextInfo)(ADMDefaultTextInfo *defaultTextInfo);
	void ASAPI (*GetADMDefaultTextInfo)(ADMDefaultTextInfo *defaultTextInfo);
}
ADMHostSuite2;	


typedef ADMHostSuite2 ADMHostSuite;	// The current version: kADMHostSuiteVersion2





#ifdef MAC_ENV

#define kADMMacHostSuite		"ADM Mac Host Suite"
#define kADMMacHostSuiteVersion1	1
#define kADMMacHostSuiteVersion2	2

#define kADMMacHostSuiteVersion		kADMMacHostSuiteVersion2	// The current version


typedef ASErr ASAPI (*ADMModalEventProc)(struct EventRecord *event);
typedef ASErr ASAPI (*ADMWindowActivateProc)(struct EventRecord *event);

/*
 * Suite
 */

typedef struct ADMMacHostSuite1 
{
	ASBoolean ASAPI (*HandleADMEvent)(struct EventRecord *event);
	
	void ASAPI (*SetModalEventProc)(ADMModalEventProc modalEventProc);
	ADMModalEventProc ASAPI (*GetModalEventProc)();
	
	void ASAPI (*ActivateWindows)(ASBoolean activate);
}
ADMMacHostSuite1;

typedef struct ADMMacHostSuite2
{
	ASBoolean ASAPI (*HandleADMEvent)(struct EventRecord *event);
	
	void ASAPI (*SetModalEventProc)(ADMModalEventProc modalEventProc);
	ADMModalEventProc ASAPI (*GetModalEventProc)();
	
	void ASAPI (*ActivateWindows)(ASBoolean activate);
	
	void ASAPI (*SetWindowActivateProc)(ADMWindowActivateProc windowActivateProc);
	ADMWindowActivateProc ASAPI (*GetWindowActivateProc)();
}
ADMMacHostSuite2;	

typedef ADMMacHostSuite2 ADMMacHostSuite;	// The current version: kADMMacHostSuiteVersion2

#endif


#ifdef WIN_ENV

#define kADMWinHostSuite			"ADM Win Host Suite"
#define kADMWinHostSuiteVersion1	1
#define kADMWinHostSuiteVersion2	2

#define kADMWinHostSuiteVersion		kADMWinHostSuiteVersion2	// The current version

typedef ASErr ASAPI (*ADMModalMessageProc)(unsigned int message, unsigned int wParam, long lParam); 	// SPAM : added by jojanen

/*
 * Suite
 */

typedef struct ADMWinHostSuite1 
{
	ASBoolean ASAPI (*HandleADMMessage)(ASWindowRef frameWnd, unsigned int message, unsigned int wParam, long lParam);
	ASWindowRef ASAPI (*GetCurrentOwnerWindow)();
	ASBoolean ASAPI (*IsADMMessage)(struct tagMSG *message);

} 
ADMWinHostSuite1;

typedef struct ADMWinHostSuite2
{
	ASBoolean ASAPI (*HandleADMMessage)(ASWindowRef frameWnd, unsigned int message, unsigned int wParam, long lParam);
	ASWindowRef ASAPI (*GetCurrentOwnerWindow)();
	ASBoolean ASAPI (*IsADMMessage)(struct tagMSG *message);

	ASWindowRef ASAPI (*GetPlatformAppWindow)();
	void ASAPI (*SetPlatformAppWindow) (ASWindowRef appWindow);

	ADMModalMessageProc ASAPI (*GetModalMessageProc)();							// SPAM : added by jojanen
	void ASAPI (*SetModalMessageProc)(ADMModalMessageProc modalMessageProc);	// SPAM : added by jojanen
} 
ADMWinHostSuite2;	

typedef ADMWinHostSuite2 ADMWinHostSuite;	// The current version: kADMWinHostSuiteVersion2



#endif

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif
