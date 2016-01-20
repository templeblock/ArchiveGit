/****************************************************************************
Contains:	Interface for polygon code.
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

#endif _H_SCPOLYGO
