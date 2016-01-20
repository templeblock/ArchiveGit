// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "framelib.h"

//************************************************************************
LPFRAME frame_set( LPFRAME lpNewFrame )
//************************************************************************
{
LPFRAME lpOldFrame;

lpOldFrame = FrameGetCurrent();

if ( lpNewFrame )
	FrameSetCurrent( lpNewFrame );
return( lpOldFrame );
}

