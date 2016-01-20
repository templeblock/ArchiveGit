/*
 * Name:
 *	SPBlocks.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Blocks Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPBlocks__
#define __SPBlocks__


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

#define kSPBlocksSuite				"SP Blocks Suite"
#define kSPBlocksSuiteVersion		2


/*******************************************************************************
 **
 **	Suite
 **
 **/

typedef struct SPBlocksSuite {

	SPAPI SPErr (*AllocateBlock)( long size, char *debug, void **block );
	SPAPI SPErr (*FreeBlock)( void *block );
	SPAPI SPErr (*ReallocateBlock)( void *block, long newSize, char *debug, void **rblock );

} SPBlocksSuite;


SPAPI SPErr SPAllocateBlock( long size, char *debug, void **block );
SPAPI SPErr SPFreeBlock( void *block );
SPAPI SPErr SPReallocateBlock( void *block, long newSize, char *debug, void **rblock );


/*******************************************************************************
 **
 **	Errors
 **
 **/

#define kSPOutOfMemoryError				0xFFFFFF6cL  /* -108, same as Mac memFullErr */
#define kSPBlockSizeOutOfRangeError		'BkRg'


#ifdef __cplusplus
}
#endif

#endif
