/*
	File: PIExport.h

	Copyright 1990-91 by Thomas Knoll.	Copyright 1992 by Adobe Systems, Inc..
	All rights reserved.

	This file describes version 4.0 of Photoshop's Export module interface.
*/

#ifndef __PIExport__
#define __PIExport__

#include "PIGeneral.h"

/* Operation selectors */

#define exportSelectorAbout    0
#define exportSelectorStart    1
#define exportSelectorContinue 2
#define exportSelectorFinish   3
#define exportSelectorPrepare  4

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */

#define exportBadParameters -30200	/* "a problem with the export module interface" */
#define exportBadMode		-30201	/* "the export module does not support <mode> images" */

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/

typedef struct ExportRecord
	{

	long				serialNumber;	/* Host's serial number, to allow
										   copy protected plug-in modules. */

	TestAbortProc		abortProc;		/* The plug-in module may call this no-argument
										   BOOLEAN function (using Pascal calling
										   conventions) several times a second during long
										   operations to allow the user to abort the operation.
										   If it returns TRUE, the operation should be aborted
										   (and a positive error code returned). */

	ProgressProc		progressProc;	/* The plug-in module may call this two-argument
										   procedure (using Pascal calling conventions)
										   periodically to update a progress indicator.
										   The first parameter (type LONGINT) is the number
										   of operations completed; the second (type LONGINT)
										   is the total number of operations. */

	long				maxData;		/* Maximum number of bytes that should be
										   requested at once (the plug-in should reduce
										   its requests by the size any large buffers
										   it allocates). The plug-in may reduce this
										   value in the exportSelectorPrepare routine. */

	short				imageMode;		/* Image mode */
	Point				imageSize;		/* Size of image */
	short				depth;			/* Bits per sample, currently will be 1 or 8 */
	short				planes; 		/* Samples per pixel */

	Fixed				imageHRes;		/* Pixels per inch */
	Fixed				imageVRes;		/* Pixels per inch */

	LookUpTable			redLUT; 		/* Red LUT, only used for Indexed Color images */
	LookUpTable			greenLUT;		/* Green LUT, only used for Indexed Color images */
	LookUpTable			blueLUT;		/* Blue LUT, only used for Indexed Color images */

	Rect				theRect;		/* Rectangle requested, set to empty rect when done */
	short				loPlane;		/* First plane requested */
	short				hiPlane;		/* Last plane requested */

	void *				data;			/* A pointer to the requested image data */
	long				rowBytes;		/* Spacing between rows */

	Str255 				filename;		/* Document file name */
	short				vRefNum;		/* Volume reference number, or zero if none */
	Boolean 			dirty;			/* Changes since last saved flag. The plug-in may clear
										   this field to prevent prompting the user when
										   closing the document. */

	Rect				selectBBox; 	/* Bounding box of current selection, or an empty
										   rect if there is no current selection. */

	OSType				hostSig;		/* Creator code for host application */
	HostProc			hostProc;		/* Host specific callback procedure */

	Handle				duotoneInfo;	/* Handle to duotone information. */

	short				thePlane;		/* Currently selected channel,
										   or -1 if a composite color channel,
										   or -2 if all channels. */

	PlugInMonitor		monitor;		/* Information on current monitor */

	void *				platformData;	/* Platform specific hook. */

	BufferProcs 		*bufferProcs;	/* Host buffer maintenance procedures. */
	
	ResourceProcs 		*resourceProcs;	/* Host plug-in resource procedures. */
	
	ProcessEventProc	processEvent;	/* Pass events to the application. */
	
	DisplayPixelsProc	displayPixels;	/* Display dithered pixels. */

	HandleProcs			*handleProcs;	/* Platform independent handle manipulation. */

	char				reserved [232]; /* Set to zero */

	}
ExportRecord, *ExportRecordPtr;

/******************************************************************************/

/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

/******************************************************************************/

#endif
