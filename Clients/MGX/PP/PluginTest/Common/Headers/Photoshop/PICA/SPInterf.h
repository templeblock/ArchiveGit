/*
 * Name:
 *	SPInterface.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Interface Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPInterface__
#define __SPInterface__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPBasic.h"
#include "SPFiles.h"
#include "SPMData.h"
#include "SPPlugs.h"
#include "SPProps.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPInterfaceSuite				"SP Interface Suite"
#define kSPInterfaceSuiteVersion		2

#define kSPInterfaceCaller				"SP Interface"
#define kSPInterfaceStartupSelector		"Startup"
#define kSPInterfaceShutdownSelector	"Shutdown"
#define kSPInterfaceAboutSelector		"About"

#define kSPSweetPea2Adapter				"Sweet Pea 2 Adapter"
#define kSPSweetPea2AdapterVersion		1

/*******************************************************************************
 **
 ** Types
 **
 **/

typedef struct SPInterfaceMessage {

	SPMessageData d;

} SPInterfaceMessage;


/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPInterfaceSuite {

	SPAPI SPErr (*SendMessage)( SPPluginRef plugin, char *caller, char *selector, void *message, SPErr *result );
	SPAPI SPErr (*SetupMessageData)( SPPluginRef plugin, SPMessageData *data );
	SPAPI SPErr (*EmptyMessageData)( SPPluginRef plugin, SPMessageData *data );
	SPAPI SPErr (*StartupExport)( SPPluginListRef pluginList, char *name, long version, long *started );

} SPInterfaceSuite;


SPAPI SPErr SPSendMessage( SPPluginRef plugin, char *caller, char *selector, void *message, SPErr *result );
SPAPI SPErr SPSetupMessageData( SPPluginRef plugin, SPMessageData *data );
SPAPI SPErr SPEmptyMessageData( SPPluginRef plugin, SPMessageData *data );
SPAPI SPErr SPStartupExport( SPPluginListRef pluginList, char *name, long version,long *started );


/*******************************************************************************
 **
 ** Errors
 **
 **/

#define kSPNotASweetPeaPluginError	'NSPP'
#define kSPAlreadyInSPCallerError	'AISC'

#ifdef __cplusplus
}
#endif

#endif
