/*
 * Name:
 *	SPPlugins.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Plug-ins Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPPlugins__
#define __SPPlugins__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPFiles.h"
#include "SPAdapts.h"
#include "SPProps.h"
#include "SPStrngs.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPPluginsSuite				"SP Plug-ins Suite"
#define kSPPluginsSuiteVersion		4

#define kSPRuntimePluginList		((SPPluginListRef)NULL)


/*******************************************************************************
 **
 ** Types
 **
 **/

typedef struct SPPlugin *SPPluginRef;
typedef struct SPPluginList *SPPluginListRef;
typedef struct SPPluginListIterator *SPPluginListIteratorRef;

/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPPluginsSuite {

	SPAPI SPErr (*AllocatePluginList)( SPStringPoolRef strings, SPPluginListRef *pluginList );
	SPAPI SPErr (*FreePluginList)( SPPluginListRef pluginList );

	SPAPI SPErr (*AddPlugin)( SPPluginListRef pluginList, SPPlatformFileSpecification *fileSpec, PIPropertyList *PiPL, char *adapterName, void *adapterInfo, SPPluginRef *plugin );

	SPAPI SPErr (*NewPluginListIterator)( SPPluginListRef pluginList, SPPluginListIteratorRef *iter );
	SPAPI SPErr (*NextPlugin)( SPPluginListIteratorRef iter, SPPluginRef *plugin );
	SPAPI SPErr (*DeletePluginListIterator)( SPPluginListIteratorRef iter );
	SPAPI SPErr (*GetPluginListNeededSuiteAvailable)( SPPluginListRef pluginList, SPBoolean *available );

	SPAPI SPErr (*GetPluginHostEntry)( SPPluginRef plugin, void **host );
	SPAPI SPErr (*GetPluginFileSpecification)( SPPluginRef plugin, SPPlatformFileSpecification *fileSpec );
	SPAPI SPErr (*GetPluginPropertyList)( SPPluginRef plugin, SPPropertyListRef *propertList );
	SPAPI SPErr (*GetPluginGlobals)( SPPluginRef plugin, void **globals );
	SPAPI SPErr (*SetPluginGlobals)( SPPluginRef plugin, void *globals );
	SPAPI SPErr (*GetPluginStarted)( SPPluginRef plugin, long *started );
	SPAPI SPErr (*SetPluginStarted)( SPPluginRef plugin, long started );
	SPAPI SPErr (*GetPluginSkipShutdown)( SPPluginRef plugin, long *skipShutdown );
	SPAPI SPErr (*SetPluginSkipShutdown)( SPPluginRef plugin, long skipShutdown );
	SPAPI SPErr (*GetPluginBroken)( SPPluginRef plugin, long *broken );
	SPAPI SPErr (*SetPluginBroken)( SPPluginRef plugin, long broken );
	SPAPI SPErr (*GetPluginAdapter)( SPPluginRef plugin, SPAdapterRef *adapter );
	SPAPI SPErr (*GetPluginAdapterInfo)( SPPluginRef plugin, void **adapterInfo );
	SPAPI SPErr (*SetPluginAdapterInfo)( SPPluginRef plugin, void *adapterInfo );

	SPAPI SPErr (*FindPluginProperty)( SPPluginRef plugin, PIType vendorID, PIType propertyKey, long propertyID, PIProperty **p );

	SPAPI SPErr (*GetPluginName)( SPPluginRef plugin, char **name );
	SPAPI SPErr (*SetPluginName)( SPPluginRef plugin, char *name );
	SPAPI SPErr (*GetNamedPlugin)( char *name, SPPluginRef *plugin);

	SPAPI SPErr (*SetPluginPropertyList)( SPPluginRef plugin, SPFileRef file );
} SPPluginsSuite;


SPAPI SPErr SPAllocatePluginList( SPStringPoolRef strings, SPPluginListRef *pluginList );
SPAPI SPErr SPFreePluginList( SPPluginListRef pluginList );
SPAPI SPErr SPGetPluginListNeededSuiteAvailable( SPPluginListRef pluginList, SPBoolean *available );

SPAPI SPErr SPAddPlugin( SPPluginListRef pluginList, SPPlatformFileSpecification *fileSpec, PIPropertyList *PiPL, char *adapterName, void *adapterInfo, SPPluginRef *plugin );

SPAPI SPErr SPNewPluginListIterator( SPPluginListRef pluginList, SPPluginListIteratorRef *iter );
SPAPI SPErr SPNextPlugin( SPPluginListIteratorRef iter, SPPluginRef *plugin );
SPAPI SPErr SPDeletePluginListIterator( SPPluginListIteratorRef iter );

SPAPI SPErr SPGetHostPluginEntry( SPPluginRef plugin, void **host );
SPAPI SPErr SPGetPluginFileSpecification( SPPluginRef plugin, SPPlatformFileSpecification *fileSpec );
SPAPI SPErr SPGetPluginPropertyList( SPPluginRef plugin, SPPropertyListRef *propertyList );
SPAPI SPErr SPGetPluginGlobals( SPPluginRef plugin, void **globals );
SPAPI SPErr SPSetPluginGlobals( SPPluginRef plugin, void *globals );
SPAPI SPErr SPGetPluginStarted( SPPluginRef plugin, long *started );
SPAPI SPErr SPSetPluginStarted( SPPluginRef plugin, long started );
SPAPI SPErr SPGetPluginSkipShutdown( SPPluginRef plugin, long *skipShutdown );
SPAPI SPErr SPSetPluginSkipShutdown( SPPluginRef plugin, long skipShutdown );
SPAPI SPErr SPGetPluginBroken( SPPluginRef plugin, long *broken );
SPAPI SPErr SPSetPluginBroken( SPPluginRef plugin, long broken );
SPAPI SPErr SPGetPluginAdapter( SPPluginRef plugin, SPAdapterRef *adapter );
SPAPI SPErr SPGetPluginAdapterInfo( SPPluginRef plugin, void **adapterInfo );
SPAPI SPErr SPSetPluginAdapterInfo( SPPluginRef plugin, void *adapterInfo );

SPAPI SPErr SPFindPluginProperty( SPPluginRef plugin, PIType vendorID, PIType propertyKey, long propertyID, PIProperty **p );

SPAPI SPErr SPGetPluginName( SPPluginRef plugin, char **name );
SPAPI SPErr SPSetPluginName( SPPluginRef plugin, char *name );
SPAPI SPErr SPGetNamedPlugin( char *name, SPPluginRef *plugin);

SPAPI SPErr SPSetPluginPropertyList( SPPluginRef plugin, SPFileRef file );

SPErr SPAddHostPlugin( SPPluginListRef pluginList, void *entry, void *access, char *adapterName, void *adapterInfo, SPPluginRef *plugin );
																  /* access is SPPlatformAccessRef */

/*******************************************************************************
 **
 ** Errors
 **
 **/

#define kSPUnknownAdapterError			'?Adp'
#define kSPBadPluginListIteratorError	'PiLI'
#define kSPBadPluginHost				'PiH0'
#define kSPCantAddHostPluginError		'AdHo'
#define kSPPluginNotFound				'P!Fd'


#ifdef __cplusplus
}
#endif

#endif
