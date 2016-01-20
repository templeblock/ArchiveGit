/*
 * Name:
 *	SPBasic.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Basic Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPBasic__
#define __SPBasic__


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
 **	Constants
 **
 **/

#define kSPBasicSuite				"SP Basic Suite"
#define kSPBasicSuiteVersion		4


/*******************************************************************************
 **
 **	Suite
 **
 **/

typedef struct SPBasicSuite {

	SPAPI SPErr (*AcquireSuite)( char *name, long version, void **suite );
	SPAPI SPErr (*ReleaseSuite)( char *name, long version );
	SPAPI SPBoolean (*IsEqual)( char *token1, char *token2 );
	SPAPI SPErr (*AllocateBlock)( long size, void **block );
	SPAPI SPErr (*FreeBlock)( void *block );
	SPAPI SPErr (*ReallocateBlock)( void *block, long newSize, void **newblock );
	SPAPI SPErr (*Undefined)( void );

} SPBasicSuite;


SPAPI SPErr SPBasicAcquireSuite( char *name, long version, void **suite );
SPAPI SPErr SPBasicReleaseSuite( char *name, long version );
SPAPI SPBoolean SPBasicIsEqual( char *token1, char *token2 );
SPAPI SPErr SPBasicAllocateBlock( long size, void **block );
SPAPI SPErr SPBasicFreeBlock( void *block );
SPAPI SPErr SPBasicReallocateBlock( void *block, long newSize, void **newblock );
SPAPI SPErr SPBasicUndefined( void );


/*******************************************************************************
 **
 **	Errors
 **
 **/

/* Suite errors are in SPSuites.h */
#define kSPBadParameterError		'Parm'	


#ifdef __cplusplus
}
#endif

#endif
