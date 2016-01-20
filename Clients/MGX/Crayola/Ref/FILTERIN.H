/*  $Header:   L:/dev/img/win32/lib/pvcs/filterin.h_v   1.0   11 Aug 1993 17:31:30   tedr  $     */
/*  $Log:   L:/dev/img/win32/lib/pvcs/filterin.h_v  $
 * 
 *    Rev 1.0   11 Aug 1993 17:31:30   tedr
 * Initial revision.
 * 
 *    Rev 1.0   13 Jul 1993 12:30:58   tedr
 * Initial revision.
 * 
 *    Rev 1.1   17 May 1993 15:58:28   michaelb
 * No change.
 * 
 *    Rev 1.0   09 May 1993 16:23:20   michaelb
 * Initial revision.
 * 
 *    Rev 1.7   09 Mar 1992 11:17:48   peterm
 * No change.
 * 
 *    Rev 1.6   17 Feb 1992 12:42:22   peterm
 * No change.
 * 
 *    Rev 1.5   11 Feb 1992 13:13:52   peterm
 * platform changed, new about interface.
 * 
 *    Rev 1.4   15 Jan 1992 08:08:10   peterm
 * 
 *    Rev 1.3   10 Jan 1992 10:03:58   giggey
 * No change.
 * 
 *    Rev 1.2   12 Dec 1991 12:37:36   peterm
 * ps126
 * 
 *    Rev 1.1   05 Dec 1991 16:42:22   peterm
 * shorten reserved data.
 * 
 *    Rev 1.0   25 Nov 1991 15:09:52   peterm
 * Initial revision.
*/
/*
	File: FilterInterface.h

	Copyright 1990-92 by Thomas Knoll and Adobe Systems Inc..

	This file describes version 4.0 of Photoshop's Filter module interface.
*/
#ifndef __FILTERINTERFACE_H
#define __FILTERINTERFACE_H


#include "Platform.h"
#include "AboutInt.h"

/* Operation selectors */

#define filterSelectorAbout 	 0
#define filterSelectorParameters 1
#define filterSelectorPrepare	 2
#define filterSelectorStart 	 3
#define filterSelectorContinue	 4
#define filterSelectorFinish	 5

/* Image modes */

#define filterModeGrayScale	   1
#define filterModeRGBColor 	   3
#define filterModeCMYKColor	   4
#define filterModeHSLColor	   5
#define filterModeHSBColor	   6
#define filterModeMultichannel 7

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */

#define filterBadParameters -30100	/* "a problem with the filter module interface" */
#define filterBadMode		-30101  /* "the filter module does not support <mode> images" */

typedef unsigned char FilterColor [4];

typedef struct FilterMonitor {

	PIFixed		gamma;				/* Gamma value */

	PIFixed		redX;				/* Red phosphor */
	PIFixed		redY;

	PIFixed		greenX;				/* Green phosphor */
	PIFixed		greenY;

	PIFixed		blueX;				/* Blue phosphor */
	PIFixed		blueY;

	PIFixed		whiteX;				/* White point */
	PIFixed		whiteY;

	PIFixed		ambient;			/* 0.0 = Low, 0.5 = Medium, 1.0 = High */

	} FilterMonitor;

typedef struct FilterRecord {

	long		serialNumber;		/* Host's serial number, to allow
									   copy protected plug-in modules. */

	PIProcPtr 	abortProc;			/* The plug-in module may call this no-argument
									   BOOLEAN function (using Pascal calling
									   conventions) several times a second during long
									   operations to allow the user to abort the operation.
									   If it returns TRUE, the operation should be aborted
									   (and a positive error code returned). */

	PIProcPtr 	progressProc;		/* The plug-in module may call this two-argument
									   procedure (using Pascal calling conventions)
									   periodically to update a progress indicator.
									   The first parameter (type LONGINT) is the number
									   of operations completed; the second (type LONGINT)
									   is the total number of operations. */

	PIHandle		parameters; 	/* A handle, initialized to NIL by Photoshop.
									   This should be used to hold the filter's
									   current parameters. */

	PIPoint		imageSize;			/* Size of image */
	short		planes; 			/* Samples per pixel */
	PIRect		filterRect; 		/* Rectangle to filter */

	PIRGBColor	background; 		/* Current background color */
	PIRGBColor	foreground; 		/* Current foreground color */

	long		maxSpace;			/* Maximum possible total of data and buffer space */

	long		bufferSpace;		/* If the plug-in filter needs to allocate
									   large internal buffers, the filterSelectorPrepare
									   routine should set this field to the number
									   of bytes the filterSelectorStart routine is
									   planning to allocate.  Relocatable blocks should
									   be used if possible. */

	PIRect		inRect; 			/* Requested input rectangle. Must be a subset of
									   the image's bounding rectangle. */
	short		inLoPlane;			/* First requested input plane */
	short		inHiPlane;			/* Last requested input plane */

	PIRect		outRect;			/* Requested output rectangle. Must be a subset of
									   filterRect. */
	short		outLoPlane; 		/* First requested output plane */
	short		outHiPlane; 		/* Last requested output plane */

	PIPtr 		inData; 			/* Pointer to input rectangle. If more than one
									   plane was requested, the data is interleaved. */
	long		inRowBytes; 		/* Offset between input rows */
	PIPtr 		outData;			/* Pointer to output rectangle. If more than one
									   plane was requested, the data is interleaved. */
	long		outRowBytes;		/* Offset between output rows */

	PIBoolean 	isFloating; 		/* Set to true if the selection is floating */
	PIBoolean 	haveMask;			/* Set to true if there is a selection mask */
	PIBoolean 	autoMask;			/* If there is a mask, and the selection is not
									   floating, the plug-in can change this field to
									   false to turn off auto-masking. */

	PIRect		maskRect;			/* Requested mask rectangle.  Must be a subset of
									   filterRect. Should only be used if haveMask is
									   true. */

	PIPtr 		maskData;			/* Pointer to (read only) mask data. */
	long		maskRowBytes;		/* Offset between mask rows */

	FilterColor	backColor;			/* Background color in native color space */
	FilterColor	foreColor;			/* Foreground color in native color space */

	PIOSType	hostSig;			/* Creator code for host application */
	PIProcPtr	hostProc;			/* Host specific callback procedure */

	short		imageMode;			/* Image mode */

	PIFixed		imageHRes;			/* Pixels per inch */
	PIFixed		imageVRes;			/* Pixels per inch */

	PIPoint		floatCoord;			/* Top left coordinate of selection */
	PIPoint 	wholeSize;			/* Size of image selection is floating over */

	FilterMonitor	monitor;		/* Information on current monitor */

	PlatformData	far *platform;  /* Platform specific data passed to filter. */

	char 		reserved [252];		/* Set to zero */

} FilterRecord, far *FilterRecordPtr;


  /* Dispatch routines based on FilterRecord */
void HandleDispatch (short selector, FilterRecordPtr stuff, long far *data, short far *result);
void Dispatch (short selector, FilterRecordPtr stuff, long far *data, short far *result);

#endif
