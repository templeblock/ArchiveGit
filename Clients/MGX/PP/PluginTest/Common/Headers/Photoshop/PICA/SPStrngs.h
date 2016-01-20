/*
 * Name:
 *	SPTypes.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Strings Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPStrings__
#define __SPStrings__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPStringsSuite				"SP Strings Suite"
#define kSPStringsSuiteVersion		2

#define kSPRuntimeStringPool		((SPStringPoolRef)NULL)


/*******************************************************************************
 **
 ** Types
 **
 **/

/*  If you override the default string pool handler by defining host proc routines,
 *	how the string pool memory allocation and searching is done is up to you.  As an example,
 *	the structure below is similar to what is used by default string pool 
 *	routines. The pool is a sorted list of strings of number count, kept in memory referenced 
 *	by the heap field.
 *	
 *			typedef struct SPStringPool {
 *		
 *				SPPoolHeapRef heap;
 *				long count;
 *	
 *			} SPStringPool;
 */
 
typedef struct SPStringPool *SPStringPoolRef;


/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPStringsSuite {

	SPAPI SPErr (*AllocateStringPool)( SPStringPoolRef *stringPool );
	SPAPI SPErr (*FreeStringPool)( SPStringPoolRef stringPool );
	SPAPI SPErr (*MakeWString)( SPStringPoolRef stringPool, char *string, char **wString );

} SPStringsSuite;


SPAPI SPErr SPAllocateStringPool( SPStringPoolRef *stringPool );
SPAPI SPErr SPFreeStringPool( SPStringPoolRef stringPool );
SPAPI SPErr SPMakeWString( SPStringPoolRef stringPool, char *string, char **wString );


#ifdef __cplusplus
}
#endif

#endif
