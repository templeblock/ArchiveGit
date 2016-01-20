/*
 * Name:
 *	SPTypes.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	These are the basic declarations used by PICA.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1997	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPTypes__
#define __SPTypes__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPConfig.h"


/*
 *	You can replace SPTypes.h with your own. Define OTHER_SP_TYPES_H on the
 *	command line or in SPConfig.h to be the name of the replacement file.
 *
 *	Example:
 *
 *	#define OTHER_SP_TYPES_H "MySPTypes.h"
 *	#include "SPBasic.h"  // for example
 *
 *	Your replacement must define TRUE, FALSE, SPErr, etc.
 *
 */

#ifdef OTHER_SP_TYPES_H
#include OTHER_SP_TYPES_H
#else


/*******************************************************************************
 **
 **	Constants
 **
 **/

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL

#ifdef MAC_ENV
#if !defined(__cplusplus) && (defined(__SC__) || defined(THINK_C))
#define NULL	((void *) 0)
#else
#define NULL	0
#endif
#endif

#ifdef WIN_ENV
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif


/*
 *	SPAPI is placed in front of procedure declarations in the API. On the Mac
 *	it is 'pascal', which forces consistent calling conventions across different
 *	compilers. On Windows it's nothing.
 *
 *	Example:
 *
 *	SPAPI void *SPAllocateBlock( long size, char *debug, SPErr *error );
 *
 */

#ifdef MAC_ENV
#define SPAPI	pascal
#endif

#ifdef WIN_ENV
#define SPAPI
#endif


#ifdef MAC_ENV

/* SPBoolean is the same a Macintosh Boolean. */
typedef unsigned char SPBoolean; 

#endif


#ifdef WIN_ENV

/* SPBoolean is the same a Windows BOOL. */
typedef int SPBoolean;

#endif


/*******************************************************************************
 **
 **	Error Handling
 **
 **/

/*
 *	Error codes are C strings, with the exception of the code for
 *	no error, which is NULL. The error can first be compared with kSPNoError to
 *	test if the function succeeded. If it is not NULL then the error can be
 *	string-compared with predefined error strings.
 *
 *	Example:
 *
 *	SPErr error = kSPNoError;
 *
 *	block = SPAllocateBlock( size, debug, &error );
 *	if ( error != kSPNoError ) {
 *		if ( strcmp( error, kSPOutOfMemoryError ) == 0 )
 *			FailOutOfMemory();
 *		...
 *	}
 */

typedef long SPErr;

/*
 *	kSPNoError and kSPUnimplementedError are universal. Other error codes should
 *	be defined in the appropriate header files.
 */

#define kSPNoError				0x00000000L
#define kSPUnimplementedError	'!IMP'	


#endif /* OTHER_SP_TYPES_H */

#endif
