// ****************************************************************************
//
//  File Name:			ScratchBitmapImage.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RScratchBitmapImage class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ScratchBitmapImage.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_SCRATCHBITMAPIMAGE_H_
#define	_SCRATCHBITMAPIMAGE_H_

#ifdef _WINDOWS

	#include "DeviceDepBitmapImage.h"
	#define ScratchImageBaseClass RDeviceDependentBitmapImage

#else

	#include "BitmapImage.h"
	#define ScratchImageBaseClass RBitmapImage

#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RScratchBitmapImage
//
//  Description:	Class used for scratch bitmaps like offscreens and caches.
//
// ****************************************************************************
//
class RScratchBitmapImage : public ScratchImageBaseClass
	{
	// Construction
	public :
						RScratchBitmapImage( BOOLEAN fStoreOnDisk = FALSE ) : ScratchImageBaseClass( fStoreOnDisk ) { };
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _SCRATCHBITMAPIMAGE_H_