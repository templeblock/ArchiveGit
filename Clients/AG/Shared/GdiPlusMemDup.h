// Create by Jim McCurdy
// Don't include this file directly; include Gdip.h
// This is a clone of <GdiPlusFlat.h> except:
// o functions are converted to typedefs
// o a big declaration list was added

/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusMem.h
*
* Abstract:
*
*   GDI+ Private Memory Management APIs
*
\**************************************************************************/

#ifndef _GDIPLUSMEM_H
#define _GDIPLUSMEM_H

#ifdef __cplusplus
extern "C" {
#endif

#define WINGDIPAPI __stdcall

//----------------------------------------------------------------------------
// Memory Allocation APIs
//----------------------------------------------------------------------------

typedef void* (WINGDIPAPI *PFNGdipAlloc)(size_t size);

typedef void (WINGDIPAPI *PFNGdipFree)(void* ptr);

#ifdef GDIP_NO_EXTERN
	#define Declare(name) PFN##name name
#else
	#define Declare(name) extern PFN##name name
#endif GDIP_NO_EXTERN 

// Declaration List
Declare(GdipAlloc);
Declare(GdipFree);

#ifdef __cplusplus
}
#endif

#endif // !_GDIPLUSMEM_H
