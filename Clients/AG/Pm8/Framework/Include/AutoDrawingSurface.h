// ****************************************************************************
//
//  File Name:			AutoDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RAutoDrawingSurface class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/AutoDrawingSurface.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_AUTODRAWINGSURFACE_H_
#define		_AUTODRAWINGSURFACE_H_

#include "OffscreenDrawingSurface.h"
#include "ScratchBitmapImage.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RAutoDrawingSurface
//
//  Description:	This class will be able to create an use an offscreen drawing
//						surface or a given drawing surface if the offscreen cannot be
//						created.
//
// ****************************************************************************
//
class FrameworkLinkage RAutoDrawingSurface : public BaseDrawingSurface
	{
	// Construction, destruction & initialization
	public :
												RAutoDrawingSurface( );
		virtual								~RAutoDrawingSurface( );

	//	Operations
	public :
		BOOLEAN								Prepare( RDrawingSurface* pSurface, R2dTransform& transform, const RRealRect& bounds );
		void									Release( );
		void									Reset( );

	// Member data
	private :
		RIntRect								m_Bounds;							//	The output rect for drawing from the offscreen to the destination surface
		RIntSize								m_OffscreenSize;					//	Size the offscreen was created at
		RDrawingSurface*					m_pDestinationDrawingSurface;	//	Where to draw when we release the autodrawing surface
		ROffscreenDrawingSurface		m_OffscreenSurface;				//	Offscreen surface
		RScratchBitmapImage				m_rOffscreenBitmap;				//	Offscreen bitmap
		BOOLEAN								m_fInUse;							//	TRUE: this auto drawing surface is already in the process of drawing
		BOOLEAN								m_fSetup;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _AUTODRAWINGSURFACE_H_
