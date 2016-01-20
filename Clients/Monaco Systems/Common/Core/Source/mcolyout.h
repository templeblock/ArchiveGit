
#ifndef MCO_LAYOUT
#define MCO_LAYOUT
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//	mcolyout.h
//
//	Defines McoLayout structure
//
//	Create Date:	8/8/93
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"

typedef struct {
	int32	numPixels;		// Number of pixels in line
	int32	numLines;		// Number of image lines
	int16	numComp;		// Number of components. Maximum is 4.
	int16	compStride;		// Stride in chars between same component along line
	int32	lineStride;		// Stride in chars between same component to next line
	int16	compBits;		// Bits per component
	int16	colorspace;		// The colorspace, uses colorsync definitions
	void	*compPtrs[4];	// Component pointers
} McoLayout;

#endif