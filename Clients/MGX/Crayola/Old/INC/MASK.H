//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _MASK_H_
#define _MASK_H_

#include "pixmap.h"

typedef struct _mask
	{
	PIXMAP	Pixmap;					// The mask bitmap data
	RECT	rMask;					// Mask rectangle
	LPTR	mbuffer;				// Mask buffer 
	BOOL	bInvert;				// Invert mask flag
	RECT	rUndoMask;				// The undo mask rectangle
	BOOL	bInvertState;			// Current state of invert
	} MASK, FAR* LPMASK;
typedef MASK ALPHA;
typedef LPMASK LPALPHA;

/*==========================================================*\
	Shape structure definition
\*==========================================================*/

typedef struct _shape
{
	int		iModes; // Add?, Subtract?, Reverse?, Circle?
	int     nPoints;
	LPPOINT lpPoints;
	RECT    rShape;
	TFORM   tform;
	struct  _shape far *lpNext;
	BOOL	bClosed;
} SHAPE, far *LPSHAPE;
typedef	LPSHAPE far *LPPSHAPE;

// Shape Mode Constants
#define SHAPE_REVERSE	1
#define SHAPE_ADD		2
#define SHAPE_SUBTRACT	3
#define SHAPE_DEFAULT SHAPE_ADD

// max output points for expanded bezier
#define MAX_BEZIER 25

// max input points for bezier (controll + end points)
#define MAX_BEZIER_IN 5
#define BEZIER_MARKER 32766
#define OPEN_BEZIER_MARKER 32765
#define PLACEHOLDER_MARKER 32767
#define	IS_BEZIER(lpPoint) \
	 (lpPoint->x == BEZIER_MARKER || lpPoint->x == OPEN_BEZIER_MARKER)
// # of last point in bezier structure
#define BEZIER_LAST(beginlist, bezier) \
	(bezier + beginlist[bezier].y)
// # of Tail of bezier
#define BEZIER_END(beginlist, bezier) \
	(bezier + beginlist[bezier].y + \
	((beginlist[bezier].x == OPEN_BEZIER_MARKER) ? 1:0))

// points in bezier structur itsself
#define BEZIER_POINTS(lpMarker) \
	lpMarker->y
// points in bezier (incluing end)
#define BEZIER_TOTAL_POINTS(lpMarker) \
	(lpMarker->y + ((lpMarker->x == OPEN_BEZIER_MARKER) ? 1:0))

#endif // _MASK_H_
