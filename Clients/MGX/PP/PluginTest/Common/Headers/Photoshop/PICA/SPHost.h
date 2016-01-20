/*
 * Name:
 *	SPHost.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	This is the file required for use of PICA.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPHost__
#define __SPHost__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPAccess.h"
#include "SPAdapts.h"
#include "SPBasic.h"
#include "SPBckDbg.h"
#include "SPBlocks.h"
#include "SPCaches.h"
#include "SPFiles.h"
#include "SPInterf.h"
#include "SPPlugs.h"
#include "SPProps.h"
#include "SPRuntme.h"
#include "SPStrngs.h"
#include "SPSuites.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 **	Constants
 **
 **/

/*
 *	The version number of the host interface. You can compare it to the
 *	version number that PICA was compiled with to determine that you are
 *	running the version you think you are. See SPVersion() below.
 *
 *	This number consists of a hi word which is the major version number reflecting
 *	changes to the SP inteface, and a low word which is the minor revision number,
 *	for instance indicating bug fixes.
 */

#define kSPVersion		0x00090004


/*
 *	Options available to the host at init time. They are:
 *
 *	kSPHostBlockDebugOption - block allocation debugging is enabled. See
 *		SPBckDbg.h for details.
 *
 *	Examples:
 *
 *		SPInit( hostProcs, pluginsFolder, kSPHostNoOptions, error );
 *		// No debugging.
 *
 *		SPInit( hostProcs, pluginsFolder, kSPHostBlockDebugOption, error );
 *		// Enable block debugging.
 */

#define kSPHostNoOptions			0
#define kSPHostBlockDebugOption		(1<<0L)


/*******************************************************************************
 **
 **	Functions
 **
 **/

/*
 *	SPInit() initializes PICA. It is the first call made. It
 *	initializes its suites and builds the list of application files.
 *
 *	HostProcs points to a filled-in structure of host callbacks, as described
 *	in SPRuntme.h. It may be NULL, in which case PICA uses its default
 *	versions for all of the host callbacks. PluginFolder is the file spec of the
 *	plug-ins folder. The contents of this folder are collected into the application
 *	files list (see SPFiles.h). Options may be any of the host options described
 *	above.
 */
SPErr SPInit( SPHostProcs *hostProcs, SPPlatformFileSpecification *pluginFolder, long options );

/*
 *	SPTerm() terminates PICA. Call this when your application quits.
 */
SPErr SPTerm( void );

/*
 *	SPStartupPlugins() scans the list of application files and builds the list
 *	of plug-ins. It then starts them up, calling each in turn with a start up
 *	message (see SPPlugs.h). Call this sometime after SPInit().
 */
SPErr SPStartupPlugins( void );

/*
 *	SPShutdownPlugins() shuts down the plug-ins. It calls each in turn with a
 *	shut down message (see SPPlugs.h). Call this when your application quits
 *	before calling SPTerm().
 */
SPErr SPShutdownPlugins( void );


/*
 *	SPSetPPCLoadHeap() sets the destination heap of plug-in accesses on PowerPPC
 *	Macintoshes.  Plug-ins can load into the system heap or the application heap.
 */
#ifdef MAC_ENV
typedef enum {
	kAppHeap = 0,
	kSysHeap
} SPTargetHeap;

SPErr SPSetPPCLoadHeap( SPTargetHeap target );
#endif


/*
 *	SPVersion() returns the version number of the PICA host interface (this
 *	file) for which PICA was built. You can compare this to the constant
 *	kSPVersion to make sure you're using the version you think you are.
 */
unsigned long SPVersion( void );


/*******************************************************************************
 **
 **	Errors
 **
 **/

/*
 *	Some non-descript problem encountered while starting up.
 */
#define kSPTroubleInitializingError	'TIni'
#define kHostCanceledStartupPluginsError 'H!St'

#ifdef __cplusplus
}
#endif

#endif
