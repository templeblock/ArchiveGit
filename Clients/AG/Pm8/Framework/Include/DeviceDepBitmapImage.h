// ****************************************************************************
//
//  File Name:			DeviceDepBimtapImage.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RDeviceDependentBitmapImage class
//
//  Portability:		Windows specific
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
//  $Logfile:: /PM8/Framework/Include/DeviceDepBitmapImage.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _DEVICEDEPBIMTAPIMAGE_H_
#define _DEVICEDEPBIMTAPIMAGE_H_

#include "BitmapImage.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RDeviceDependentBitmapImage
//
//  Description:	Class to encapsulate device dependent bitmaps
//
// ****************************************************************************
//
class FrameworkLinkage RDeviceDependentBitmapImage : public RBitmapImageBase
	{
	// Construction & Destruction
	public :
												RDeviceDependentBitmapImage( BOOLEAN fStoreOnDisk = FALSE );
												~RDeviceDependentBitmapImage( );

	// Operations
	public:
		virtual void						Initialize( YImageHandle yHandle );
		void									Initialize( uLONG uWidthInPixels, uLONG HeightInPixels, uLONG uBitDepth );
		virtual void						Initialize( RDrawingSurface& rDS, uLONG uWidthInPixels, uLONG uHeightInPixels );	
		
		virtual void						Render( RDrawingSurface& drawingSurface, const RIntRect& rDestRect );
		virtual void						Render( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );
		virtual void						RenderWithMask( RDrawingSurface& drawingSurface, RBitmapImageBase& mask, const RIntRect& destRect );
		virtual void						RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rSrcRect, const RIntRect& rDestRect );	

		virtual void						Read( RArchive& rArchive );
		virtual void						Write( RArchive& rArchive ) const;	
		virtual void						Copy( RDataTransferTarget& rDataTarget, YDataFormat yDataFormat ) const;	
		virtual void						Paste( const RDataTransferSource& rDataSource, YDataFormat yDataFormat );	
		virtual YImageHandle				GetSystemHandle( ) const;
		virtual RRealSize					GetSizeInLogicalUnits( ) const;
		
	// Attributes
	public :
		virtual uLONG						GetWidthInPixels( ) const;
		virtual uLONG						GetHeightInPixels( ) const;

	// Implementation
	private :
		void									Delete( );

	// Members
	private:
		HBITMAP								m_hBitmap;
		RIntSize								m_Size;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _DEVICEDEPBIMTAPIMAGE_H_
