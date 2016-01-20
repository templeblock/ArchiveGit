/*
 * Name:
 *	SPBlockDebug.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Block Debug Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPBlockDebug__
#define __SPBlockDebug__


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

#define kSPBlockDebugSuite			"SP Block Debug Suite"
#define kSPBlockDebugSuiteVersion	2


/*******************************************************************************
 **
 **	Suite
 **
 **/

typedef struct SPBlockDebugSuite {

	SPAPI SPErr (*GetBlockDebugEnabled)( long *enabled );
	SPAPI SPErr (*SetBlockDebugEnabled)( long debug );
	SPAPI SPErr (*GetFirstBlock)( void **block );
	SPAPI SPErr (*GetNextBlock)( void *block, void **nextblock );
	SPAPI SPErr (*GetBlockDebug)( void *block, char **debug );

} SPBlockDebugSuite;


SPAPI SPErr SPGetBlockDebugEnabled( long *enabled );
SPAPI SPErr SPSetBlockDebugEnabled( long debug );
SPAPI SPErr SPGetFirstBlock( void **block );
SPAPI SPErr SPGetNextBlock( void *block, void **nextblock );
SPAPI SPErr SPGetBlockDebug( void *block, char **debug );


/*******************************************************************************
 **
 **	Errors
 **
 **/

#define kSPCantChangeBlockDebugNowError	'!Now'
#define kSPBlockDebugNotEnabledError	'!Nbl'


#ifdef __cplusplus
}
#endif

#endif
