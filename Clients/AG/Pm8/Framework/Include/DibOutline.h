// ****************************************************************************
//
//  File Name:			DibOutline.h
//
//  Project:			TPS Libraries
//
//  Author:				Greg Brown, Bruce Rosenblum
//
//  Description:		Advanced DIB manipulation functions
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/DibOutline.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#ifndef _DIBOUTLINE_H_
#define _DIBOUTLINE_H_

#include	"PolyPolygon.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

enum ORIENTATION
{
	TOP,
	LEFT,
	BOTTOM,
	RIGHT
};

//	structure that contains portable information about a DIB for
//	cross-platform DIB manipulation routines
typedef	struct tagDIBINFO
{
	uBYTE*	pBits;
	sLONG		Width;
	sLONG		Height;
	uLONG		TransparentColor; // A 32-bit valuein 0BGR format (COLORREF)
	uLONG		BytesPerPixel;
	uLONG		BytesPerRow;
} DIBINFO ;
typedef	DIBINFO FAR* LPDIBINFO;	//	dib, lpdib

/***  prototypes  ***/
#ifdef __cplusplus
extern "C" {
#endif

FrameworkLinkage BOOLEAN OutlineDibShapes (const DIBINFO& Dib, RPolyPolygon& rPolyPoly);

BOOLEAN FindFirstEdge (RIntPoint& StartPoint,
						  const DIBINFO& Dib,
						  const ORIENTATION FromWhere,
						  const sLONG StartAt);

#ifdef __cplusplus
}
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _DIBOUTLINE_H_
