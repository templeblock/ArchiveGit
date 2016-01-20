/*
 * Name:
 *	SPSuites.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Defines SP API Suites. 
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */


#ifndef __SPSuites__
#define __SPSuites__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPAccess.h"
#include "SPPlugs.h"
#include "SPStrngs.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPSuitesSuite				"SP Suites Suite"
#define kSPSuitesSuiteVersion		2

#define kSPLatestInternalVersion	0

#define kSPRuntimeSuiteList			((SPSuiteListRef)NULL)


/*******************************************************************************
 **
 ** Types
 **
 **/

typedef struct SPSuite *SPSuiteRef;
typedef struct SPSuiteList *SPSuiteListRef;
typedef struct SPSuiteListIterator *SPSuiteListIteratorRef;


/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPSuitesSuite {

	SPAPI SPErr (*AllocateSuiteList)( SPStringPoolRef stringPool, SPPluginListRef plugins, SPSuiteListRef *suiteList );
	SPAPI SPErr (*FreeSuiteList)( SPSuiteListRef suiteList );

	SPAPI SPErr (*AddSuite)( SPSuiteListRef suiteList, SPPluginRef host, char *name, long apiVersion, long internalVersion, void *suiteProcs, SPSuiteRef *suite );
	SPAPI SPErr (*AcquireSuite)( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion, void **suiteProcs );
	SPAPI SPErr (*ReleaseSuite)( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion );

	SPAPI SPErr (*FindSuite)( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion, SPSuiteRef *suite );

	SPAPI SPErr (*NewSuiteListIterator)( SPSuiteListRef suiteList, SPSuiteListIteratorRef *iter );
	SPAPI SPErr (*NextSuite)( SPSuiteListIteratorRef iter, SPSuiteRef *suite );
	SPAPI SPErr (*DeleteSuiteListIterator)( SPSuiteListIteratorRef iter );

	SPAPI SPErr (*GetSuiteHost)( SPSuiteRef suite, SPPluginRef *plugin );
	SPAPI SPErr (*GetSuiteName)( SPSuiteRef suite, char **name );
	SPAPI SPErr (*GetSuiteAPIVersion)( SPSuiteRef suite, long *version );
	SPAPI SPErr (*GetSuiteInternalVersion)( SPSuiteRef suite, long *version );
	SPAPI SPErr (*GetSuiteProcs)( SPSuiteRef suite, void **suiteProcs );
	SPAPI SPErr (*GetSuiteAcquireCount)( SPSuiteRef suite, long *count );

} SPSuitesSuite;


SPAPI SPErr SPAllocateSuiteList( SPStringPoolRef stringPool, SPPluginListRef plugins, SPSuiteListRef *suiteList );
SPAPI SPErr SPFreeSuiteList( SPSuiteListRef suiteList );

SPAPI SPErr SPAddSuite( SPSuiteListRef suiteList, SPPluginRef host, char *name, long apiVersion, long internalVersion, void *suiteProcs, SPSuiteRef *suite );
SPAPI SPErr SPAcquireSuite( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion, void **suiteProcs );
SPAPI SPErr SPReleaseSuite( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion );

SPAPI SPErr SPFindSuite( SPSuiteListRef suiteList, char *name, long apiVersion, long internalVersion, SPSuiteRef *suite );

SPAPI SPErr SPNewSuiteListIterator( SPSuiteListRef suiteList, SPSuiteListIteratorRef *iter );
SPAPI SPErr SPNextSuite( SPSuiteListIteratorRef iter, SPSuiteRef *suite );
SPAPI SPErr SPDeleteSuiteListIterator( SPSuiteListIteratorRef iter );

SPAPI SPErr SPGetSuiteHost( SPSuiteRef suite, SPPluginRef *plugin );
SPAPI SPErr SPGetSuiteName( SPSuiteRef suite, char **name );
SPAPI SPErr SPGetSuiteAPIVersion( SPSuiteRef suite, long *version );
SPAPI SPErr SPGetSuiteInternalVersion( SPSuiteRef suite, long *version );
SPAPI SPErr SPGetSuiteProcs( SPSuiteRef suite, void **suiteProcs );
SPAPI SPErr SPGetSuiteAcquireCount( SPSuiteRef suite, long *count );


/*******************************************************************************
 **
 **	Errors
 **
 **/

#define kSPSuiteNotFoundError				'S!Fd'	
#define kSPSuiteAlreadyExistsError			'SExi'	
#define kSPSuiteAlreadyReleasedError		'SRel'	
#define kSPBadSuiteListIteratorError		'SLIt'	
#define kSPBadSuiteInternalVersionError		'SIVs'	


#ifdef __cplusplus
}
#endif

#endif
