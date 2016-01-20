/*
 * Name:
 *	PIChannelPortsSuite.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Photoshop ChannelPorts Suite for Plug-ins
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1998	JF	First Version.
 *		Created by Josh Freeman.
 */

#ifndef __PIChannelPortsSuite__
#define __PIChannelPortsSuite__

#include "PIGeneral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define kPSChannelPortsSuite 			"Photoshop ChannelPorts Suite for Plug-ins"

/******************************************************************************/

#define kPSChannelPortsSuiteVersion2			2	// we're using version 2

/******************************************************************************/

typedef struct PSChannelPortsSuite1
	{
	// Determine how many levels we have. Zero if the port is invalid.
	SPAPI SPErr (*CountLevels)
		(
		PIChannelPort port, 
		int32 *count
		);

	// Get the depth at a given level. Zero if the port or level is
	// invalid.
	SPAPI SPErr (*GetDepth)
		(
		PIChannelPort port, 
		int32 level, 
		int32 *depth
		);

	// Get the bounds for the pixel data. Return an empty rectangle if
	// the parameters are invalid.
	SPAPI SPErr (*GetDataBounds) 
		(
		PIChannelPort port,
		int32 level,
		VRect *bounds
		);

	// Get the bounds to which we can write at a given level.
	SPAPI SPErr (*GetWriteLimit) 
		(
		PIChannelPort port,
		int32 level,
		VRect *writeBounds
		);

	// Get the tiling information at a given level.
	SPAPI SPErr (*GetTilingGrid) 
		(
		PIChannelPort port,
		int32 level,
		VPoint *tileOrigin,
		VPoint *tileSize
		);

	// Find the rectangle used as the downsample source for a particular
	// level in the pyramid. level is set to -1 if no support rectangle
	// exists.
	SPAPI SPErr (*GetSupportRect) 
		(
		PIChannelPort port,
		int32 level, const 
		VRect *bounds,
		int32 *supportLevel, 
		VRect *supportBounds
		);

	// Get the dependent rectangle at a particular level.
	SPAPI SPErr (*GetDependentRect) 
		(
		PIChannelPort port,
		int32 sourceLevel, 
		const VRect *sourceBounds,
		int32 dependentLevel, 
		VRect *dependentBounds
		);

	// Can we read from this port?
	SPAPI SPErr (*CanRead) 
		(
		PIChannelPort port, 
		Boolean *canRead
		);

	// Can we write to this port?
	SPAPI SPErr (*CanWrite) 
		(
		PIChannelPort port, 
		Boolean *canWrite
		);

	// Read pixels from a given level of the port. If the result is
	// noErr, then bounds will reflect the pixels actually read. If
	// it reflects an error, then the value of bounds is undefined.
	SPAPI SPErr (*ReadPixelsFromLevel) 
		(
		PIChannelPort port,
		int32 level, 
		VRect *bounds,
		const PixelMemoryDesc *destination
		);

	// Write to a level in the pyramid.
	SPAPI SPErr (*WritePixelsToBaseLevel) 
		(
		PIChannelPort port,
		VRect *bounds,
		const PixelMemoryDesc *source
		);

	// Read scaled data from the pyramid. Adjusts readRect to reflect the area
	// actually read.
	SPAPI SPErr (*ReadScaledPixels) 
		(
		PIChannelPort port,
		VRect *readRect,
		const PSScaling *scaling,
		const PixelMemoryDesc *destination
		);

	// If we just want to find out what level would be used for a given scaling,
	// we can use the following routine. sourceLevel and sourceRect describe
	// where in the pyramid we will be reading from. sourceScalingBounds is the
	// bounds used for scaling from this level to the final result.
	SPAPI SPErr (*FindSourceForScaledRead) 
		(
		PIChannelPort port,
		const VRect *readRect,
		const PSScaling *scaling,
		int32 dstDepth,
		int32 *sourceLevel,
		VRect *sourceRect,
		VRect *sourceScalingBounds
		);

	// Create a pixel array and the port to go with it.
	SPAPI SPErr (*New) 
		(
		PIChannelPort *port,
		const VRect *rect,
		int32 depth,
		Boolean globalScope
		);

	// Dispose of a port and set the port to NULL.
	SPAPI SPErr(* Dispose)
		(
		PIChannelPort *port
		);
	
	} PSChannelPortsSuite1;


#ifdef __cplusplus
}
#endif


#endif	// PIChannelPortsSuite
