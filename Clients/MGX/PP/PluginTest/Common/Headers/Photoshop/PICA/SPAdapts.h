/*
 * Name:
 *	SPAdapters.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Adapters Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPAdapters__
#define __SPAdapters__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPCaches.h"
#include "SPMData.h"
#include "SPProps.h"

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPAdaptersSuite					"SP Adapters Suite"
#define kSPAdaptersSuiteVersion				3

#define kSPAdaptersCaller					"SP Adapters"
#define kSPAdaptersStartupSelector			"Start up"
#define kSPAdaptersShutdownSelector			"Shut down"
#define kSPAdaptersDisposeInfoSelector		"Dispose info"
#define kSPAdaptersFindPropertySelector		"Find property"
#define kSPAdaptersFlushSelector			"Flush"
#define kSPAdaptersAboutSelector			"About"

#define kSPAdaptersAcquireSuiteHostSelector		"Acquire Suite"
#define kSPAdaptersReleaseSuiteHostSelector		"Release Suite"

#define kSPRuntimeAdapterList				((SPAdapterListRef)NULL)


/*******************************************************************************
 **
 ** Types
 **
 **/

typedef struct SPAdapter *SPAdapterRef;
typedef struct SPAdapterList *SPAdapterListRef;
typedef struct SPAdapterListIterator *SPAdapterListIteratorRef;

typedef struct SPAdaptersMessage {

	SPMessageData d;
	
	SPAdapterRef adapter;	
	
	struct SPPlugin *targetPlugin;
	SPErr targetResult;

	/* for Find property selector */
	PIType vendorID;
	PIType propertyKey;
	long propertyID;
	void *property;		/* returned here */

	/* for Flush selector */
	SPFlushCachesProc flushProc;
	long flushed;		/* returned here */

	/* for Suites selectors */
	struct SPSuiteList *suiteList;	/* use these if you need name, apiVersion, internalVersion */
	struct SPSuite *suite;											
	struct SPPlugin *host;			/* plug-in hosting the suite, to be aquired/released by adapter */
	void *suiteProcs;				/* returned here if reallocated */
	long acquired;					/* returned here */

} SPAdaptersMessage;


/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPAdaptersSuite {

	SPAPI SPErr (*AllocateAdapterList)( struct SPStringPool *stringPool, SPAdapterListRef *adapterList );
	SPAPI SPErr (*FreeAdapterList)( SPAdapterListRef adapterList );

	SPAPI SPErr (*AddAdapter)( SPAdapterListRef adapterList, struct SPPlugin *host, char *name, long version, SPAdapterRef *adapter );
	SPAPI SPErr (*SPFindAdapter)( SPAdapterListRef adapterList, char *name, SPAdapterRef *adapter );

	SPAPI SPErr (*NewAdapterListIterator)( SPAdapterListRef adapterList, SPAdapterListIteratorRef *iter );
	SPAPI SPErr (*NextAdapter)( SPAdapterListIteratorRef iter, SPAdapterRef *adapter );
	SPAPI SPErr (*DeleteAdapterListIterator)( SPAdapterListIteratorRef iter );

	SPAPI SPErr (*GetAdapterHost)( SPAdapterRef adapter, struct SPPlugin **plugin );
	SPAPI SPErr (*GetAdapterName)( SPAdapterRef adapter, char **name );
	SPAPI SPErr (*GetAdapterVersion)( SPAdapterRef adapter, long *version );

} SPAdaptersSuite;


SPAPI SPErr SPAllocateAdapterList( struct SPStringPool *stringPool, SPAdapterListRef *adapterList );
SPAPI SPErr SPFreeAdapterList( SPAdapterListRef adapterList );

SPAPI SPErr SPAddAdapter( SPAdapterListRef adapterList, struct SPPlugin *host, char *name, long version, SPAdapterRef *adapter );
SPAPI SPErr SPFindAdapter( SPAdapterListRef adapterList, char *name, SPAdapterRef *adapter );

SPAPI SPErr SPNewAdapterListIterator( SPAdapterListRef adapterList, SPAdapterListIteratorRef *iter );
SPAPI SPErr SPNextAdapter( SPAdapterListIteratorRef iter, SPAdapterRef *adapter );
SPAPI SPErr SPDeleteAdapterListIterator( SPAdapterListIteratorRef iter );

SPAPI SPErr SPGetAdapterHost( SPAdapterRef adapter, struct SPPlugin **plugin );
SPAPI SPErr SPGetAdapterName( SPAdapterRef adapter, char **name );
SPAPI SPErr SPGetAdapterVersion( SPAdapterRef adapter, long *version );


/*******************************************************************************
 **
 ** Errors
 **
 **/

#define kSPAdapterAlreadyExistsError		'AdEx'
#define kSPBadAdapterListIteratorError		'BdAL'


#ifdef __cplusplus
}
#endif

#endif
