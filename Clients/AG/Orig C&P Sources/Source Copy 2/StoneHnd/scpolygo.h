/****************************************************************************

	File:		SCPOLYGO.H


	$Header: /Projects/Toolbox/ct/SCPOLYGO.H 2	   5/30/97 8:45a Wmanis $

	Contains:	Interface for polygon code.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	

****************************************************************************/

#ifndef _H_SCPOLYGO
#define _H_SCPOLYGO

#include "sctypes.h"

void			POLYDuplicate( scVertHandle*, ushort&, scVertHandle, ushort );
ushort			POLYCountVerts( const scVertex* );
long			POLYExternalSize( scVertHandle, long );
void			POLYtoFile( APPCtxPtr, IOFuncPtr, scVertHandle, ushort );
scVertHandle	POLYfromFile( APPCtxPtr, IOFuncPtr, ushort );
MicroPoint		POLYMaxDepth( scVertHandle );


#endif /*_POLYGONH_*/


