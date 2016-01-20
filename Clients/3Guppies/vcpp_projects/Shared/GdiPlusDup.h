// Create by Jim McCurdy
// Don't include this file directly; include Gdip.h
// This is a clone of <GdiPlus.h> except:
// o instead of including <GdiplusMem.h>, it includes its clone GdiplusMemDup.h
// o instead of including <GdiplusFlat.h>, it includes its clone GdiplusFlatDup.h

/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   Gdiplus.h
*
* Abstract:
*
*   GDI+ public header file
*
\**************************************************************************/

#ifndef _GDIPLUS_H
#define _GDIPLUS_H

struct IDirectDrawSurface7;

typedef signed   short   INT16;
typedef unsigned short  UINT16;

#include <pshpack8.h>   // set structure packing to 8

namespace Gdiplus
{
    namespace DllExports
    {
//j		#include "GdiplusMem.h"
		#include "GdiplusMemDup.h"
    };

//j	#include "GdiplusBase.h"
    #include "GdiplusBaseDup.h"

    #include "GdiplusEnums.h"
    #include "GdiplusTypes.h"
    #include "GdiplusInit.h"
    #include "GdiplusPixelFormats.h"
    #include "GdiplusColor.h"
    #include "GdiplusMetaHeader.h"
    #include "GdiplusImaging.h"
    #include "GdiplusColorMatrix.h"

    #include "GdiplusGpStubs.h"
    #include "GdiplusHeaders.h"

    namespace DllExports
    {
//j		#include "GdiplusFlat.h"
		#include "GdiplusFlatDup.h"
    };


    #include "GdiplusImageAttributes.h"
    #include "GdiplusMatrix.h"
    #include "GdiplusBrush.h"
    #include "GdiplusPen.h"
    #include "GdiplusStringFormat.h"
    #include "GdiplusPath.h"
    #include "GdiplusLineCaps.h"
    #include "GdiplusMetafile.h"
    #include "GdiplusGraphics.h"
    #include "GdiplusCachedBitmap.h"
    #include "GdiplusRegion.h"
    #include "GdiplusFontCollection.h"
    #include "GdiplusFontFamily.h"
    #include "GdiplusFont.h"
    #include "GdiplusBitmap.h"
    #include "GdiplusImageCodec.h"

}; // namespace Gdiplus

#include <poppack.h>    // pop structure packing back to previous state

#endif // !_GDIPLUS_HPP
