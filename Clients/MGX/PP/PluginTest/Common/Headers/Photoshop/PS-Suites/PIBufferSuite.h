/*
 * Name:
 *	PIBufferSuite.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Photoshop Buffer Suite for Plug-ins
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1998	JF	First Version.
 *		Created by Josh Freeman.
 */

#ifndef __PIBufferSuite__
#define __PIBufferSuite__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define kPSBufferSuite 			"Photoshop Buffer Suite for Plug-ins"

//-------------------------------------------------------------------------
//	Buffer suite typedefs 
//-------------------------------------------------------------------------

typedef SPAPI Ptr (*BufferNewProc) (size_t *pRequestedSize,	/* IN/OUT: Actual size returned here. */
							  size_t minimumSize);	
							  				
    /* Attempts to allocate the number of bytes specified by the
    /  variable pointed to by the requested size parameter and if
    /  this many bytes cannot be allocated, then the largest possible
    /  number (greater than minimumSize) will be allocated and the
    /  number of bytes actually allocated will be returned in the
    /  requestedSize variable.  NULL may be passed as the pointer-
    /  to-requestedSize parameter, in which case the specified minimum
    /  number of bytes will be allocated.  If this minimum number
    /  of bytes cannot be allocated, then the function will fail
    /  and return NULL. */


typedef SPAPI void (*BufferDisposeProc)	(Ptr *ppBuffer);	
										  
    /* Disposes of the buffer and sets the variable that refers to it
    /  to NULL. Does nothing if the buffer pointer is already NULL. */


typedef SPAPI size_t (*BufferGetSizeProc) (Ptr pBuffer);

    /* Returns the size of the buffer OR zero if the buffer pointer
    /  is invalid. */
    

typedef SPAPI size_t (*BufferGetSpaceProc) (void);
	/* Returns the amount of remaining space available - may not be contiguous */

//-------------------------------------------------------------------------
//	Buffer suite version 1 
//-------------------------------------------------------------------------

#define kPSBufferSuiteVersion1		1

typedef struct 
{
    BufferNewProc		New;
    BufferDisposeProc	Dispose;
    BufferGetSizeProc	GetSize;
    BufferGetSpaceProc	GetSpace;

} PSBufferSuite1;


/******************************************************************************/

#ifdef __cplusplus
}
#endif


#endif	// PIBufferSuite
