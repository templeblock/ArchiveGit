/*
 * Name:
 *	SPCaches.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Caches Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPCaches__
#define __SPCaches__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPMData.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 **	Constants
 **
 **/

#define kSPCachesSuite				"SP Caches Suite"
#define kSPCachesSuiteVersion		2

/* To receive these, an PIConditionalMessages property must be present 
 * with the PIAcceptsPurgeCachesMessage flag set. */
#define kSPCacheCaller					"SP Cache"
#define kSPPluginPurgeCachesSelector	"Plug-in Purge"

/* The plug-in should return one of these two responses when it receives the kSPPluginPurgeCachesSelector */
#define kSPPluginCachesFlushResponse	'pFls'
#define kSPPluginCouldntFlushResponse	kSPNoError;

/*******************************************************************************
 **
 **	Types
 **
 **/

typedef SPErr (*SPFlushCachesProc)( char *type, void *data, long *flushed );

typedef struct SPPurgeCachesMessage {

	SPMessageData d;

} SPPurgeCachesMessage;


/*******************************************************************************
 **
 **	Suite
 **
 **/

typedef struct SPCachesSuite {

	SPAPI SPErr (*SPFlushCaches)( SPFlushCachesProc flushProc, long *flushed );

} SPCachesSuite;


SPAPI SPErr SPFlushCaches( SPFlushCachesProc flushProc, long *flushed );


#ifdef __cplusplus
}
#endif

#endif
