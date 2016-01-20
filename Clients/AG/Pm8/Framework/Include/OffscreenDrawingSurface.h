// ****************************************************************************
//
//  File Name:			OffscreenDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the ROffscreenDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Include/OffscreenDrawingSurface.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _OFFSCREENDRAWINGSURFACE_H_
#define _OFFSCREENDRAWINGSURFACE_H_

#ifdef _WINDOWS
	#include "DcDrawingSurface.h"
	#define BaseDrawingSurface RDcDrawingSurface
#endif // _WINDOWS
#ifdef MAC
	#include "GpDrawingSurface.h"	
	#define BaseDrawingSurface RGpDrawingSurface
#endif // MAC

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RImage;
class RBitmapImage;
class RVectorImage;
class RDeviceDependentBitmapImage;

class FrameworkLinkage ROffscreenDrawingSurface : public BaseDrawingSurface
{
	// Construction, destruction & initialization
	public:	
												ROffscreenDrawingSurface( BOOLEAN fIsPrinting = FALSE, BOOLEAN fClip = TRUE, RDrawingSurface* pAttributeDrawingSurface = NULL );
		virtual 								~ROffscreenDrawingSurface();
	
	public:	
	// Operations
		void									SetImage( RImage* pImage );
		RImage*								ReleaseImage();

	// operations
	public :
#ifdef MAC
		virtual void						SetClipRect( const RIntRect& clipRect );
#endif

#ifdef _WINDOWS
		virtual BOOLEAN					IsVisible( const RIntPoint& pt );
		virtual BOOLEAN					IsVisible( const RIntRect& rc );
#endif

	// Accessors
	public :
		virtual RIntSize					GetDPI( ) const;
		virtual uLONG						GetBitDepth() const;
		virtual long						GetSurface( ) const;
		virtual RIntRect					GetClipRect( ) const;
		RBitmapImage*						GetBitmapImage( ) const;

	// Implementation
	private:
		void									SetBitmapImage( RImage* pBitmapImage );
		void									SetVectorImage( RVectorImage* pVectorImage );

		void									ThrowExceptionOnError();

	// Attributes & Member Data
	private:
		RImage*								m_pImage;
		BOOLEAN								m_fClip;
#ifdef _WINDOWS
		HDC									m_hOffscreenDC;	
		HBITMAP								m_hOldBitmap;
#endif // _WINDOWS
#ifdef MAC
		RBitmapImage*						m_pBitmapImage;
		RVectorImage*						m_pVectorImage;
		GrafPtr								m_pSavedGrafPort;
		RIntRect								m_ClipRect;
#endif // MAC
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _OFFSCREENDRAWINGSURFACE_H_

