/*
	File: PIFilter.h

	Copyright 1990-91 by Thomas Knoll.	Copyright 1992 by Adobe Systems, Inc.
	All rights reserved.

	This file describes version 4.0 of Photoshop's Filter module interface.
*/

#ifndef __PIFilter__
#define __PIFilter__

#include "PIGeneral.h"

/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif


/* Operation selectors */

#define filterSelectorAbout 	 0
#define filterSelectorParameters 1
#define filterSelectorPrepare	 2
#define filterSelectorStart 	 3
#define filterSelectorContinue	 4
#define filterSelectorFinish	 5

/* Various bits of filter information. */

typedef struct FilterInfo
	{

	PlugInInfo	plugInInfo; 			/* The information common to all plug-ins.			*/
	Boolean		needDstCopy;			/* Copy to destination before filtering?  Defaults TRUE */

	}
FilterInfo;

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */

#define filterBadParameters -30100	/* "a problem with the filter module interface" */
#define filterBadMode		-30101	/* "the filter module does not support <mode> images" */

typedef unsigned char FilterColor [4];

typedef struct FilterRecord 
	{

	long		serialNumber;			/* Host's serial number, to allow
										   copy protected plug-in modules. */

	TestAbortProc	abortProc;			/* The plug-in module may call this no-argument
										   BOOLEAN function (using Pascal calling
										   conventions) several times a second during long
										   operations to allow the user to abort the operation.
										   If it returns TRUE, the operation should be aborted
										   (and a positive error code returned). */

	ProgressProc	progressProc;		/* The plug-in module may call this two-argument
										   procedure (using Pascal calling conventions)
										   periodically to update a progress indicator.
										   The first parameter (type LONGINT) is the number
										   of operations completed; the second (type LONGINT)
										   is the total number of operations. */

	Handle		parameters; 			/* A handle, initialized to NIL by Photoshop.
										   This should be used to hold the filter's
										   current parameters. */

	Point		imageSize;				/* Size of image */
	short		planes; 				/* Samples per pixel */
	Rect		filterRect; 			/* Rectangle to filter */

	RGBColor	background; 			/* Current background color */
	RGBColor	foreground; 			/* Current foreground color */

	long		maxSpace;				/* Maximum possible total of data and buffer space */

	long		bufferSpace;			/* If the plug-in filter needs to allocate
										   large internal buffers, the filterSelectorPrepare
										   routine should set this field to the number
										   of bytes the filterSelectorStart routine is
										   planning to allocate.  Relocatable blocks should
										   be used if possible. */

	Rect				inRect;			/* Requested input rectangle. Must be a subset of
										   the image's bounding rectangle. */
	short				inLoPlane;		/* First requested input plane */
	short				inHiPlane;		/* Last requested input plane */

	Rect				outRect;		/* Requested output rectangle. Must be a subset of
										   filterRect. */
	short				outLoPlane; 	/* First requested output plane */
	short				outHiPlane; 	/* Last requested output plane */

	void *				inData; 		/* Pointer to input rectangle. If more than one
										   plane was requested, the data is interleaved. */
	long				inRowBytes; 	/* Offset between input rows */
	void *				outData;		/* Pointer to output rectangle. If more than one
										   plane was requested, the data is interleaved. */
	long				outRowBytes;	/* Offset between output rows */

	Boolean 			isFloating; 	/* Set to true if the selection is floating */
	Boolean 			haveMask;		/* Set to true if there is a selection mask */
	Boolean 			autoMask;		/* If there is a mask, and the selection is not
										   floating, the plug-in can change this field to
										   false to turn off auto-masking. */

	Rect				maskRect;		/* Requested mask rectangle.  Must be a subset of
										   filterRect. Should only be used if haveMask is
										   true. */

	void *				maskData;		/* Pointer to (read only) mask data. */
	long				maskRowBytes;	/* Offset between mask rows */

	FilterColor 		backColor;		/* Background color in native color space */
	FilterColor 		foreColor;		/* Foreground color in native color space */

	OSType				hostSig;		/* Creator code for host application */
	HostProc			hostProc;		/* Host specific callback procedure */

	short				imageMode;		/* Image mode */

	Fixed				imageHRes;		/* Pixels per inch */
	Fixed				imageVRes;		/* Pixels per inch */

	Point				floatCoord; 	/* Top left coordinate of selection */
	Point				wholeSize;		/* Size of image selection is floating over */

	PlugInMonitor		monitor;		/* Information on current monitor */

	void *				platformData;	/* Platform specific information. */

	BufferProcs 		*bufferProcs;	/* The host buffer procedures. */
	
	ResourceProcs 		*resourceProcs;	/* The host plug-in resource procedures. */
	
	ProcessEventProc	processEvent;	/* Pass event to the application. */
	
	DisplayPixelsProc	displayPixels;	/* Display dithered pixels. */

	HandleProcs			*handleProcs;	/* Platform independent handle manipulation. */

	char				reserved [232]; /* Set to zero */

	}
FilterRecord, *FilterRecordPtr;

/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

#endif
