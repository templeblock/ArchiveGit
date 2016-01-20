// ****************************************************************************
//
//  File Name:			PictureDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & R. Hood
//
//  Description:		Declaration of the RPictureDrawingSurface class
//
//  Portability:		Win32 or Mac dependent
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
//  $Logfile:: /PM8/Framework/Include/PictureDrawingSurface.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PICTUREDRAWINGSURFACE_H_
#define		_PICTUREDRAWINGSURFACE_H_

#ifdef _WINDOWS
#ifndef		_DCDRAWINGSURFACE_H_
#include		"DcDrawingSurface.h"
#endif

#else

#include		<PictUtils.h>
#ifndef		_GPDRAWINGSURFACE_H_
#include		"GpDrawingSurface.h"
#endif

#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RPictureDrawingSurface
//
//  Description:	 
//
// ****************************************************************************
//
#ifdef _WINDOWS
class FrameworkLinkage RPictureDrawingSurface : public RDcDrawingSurface
#else
class RPictureDrawingSurface : public RGpDrawingSurface
#endif
	{
	// Construction, destruction & Initialization
	public :
												RPictureDrawingSurface( );
		virtual								~RPictureDrawingSurface( );
#ifdef _WINDOWS
		BOOLEAN								Initialize( HDC hReferenceDC, const RIntRect& boundingRect );
#else
		BOOLEAN								Initialize( GrafPtr pPort, const RIntRect& boundingRect );
#endif

	// Operations
	public :
		void									CopyToClipboard( );
		void									Close( );

	// accessors
	public :
#ifdef _WINDOWS
			HENHMETAFILE					GetPicture( ) const;
#else
			PicHandle						GetPicture( ) const;
#endif

	// Member data
	private :
		RIntRect 							m_rcBounds;
#ifdef _WINDOWS
		HENHMETAFILE						m_hMetafile;
#else
		PicHandle							m_hPict;
		CGrafPtr								m_pGrafPort;
#endif

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::RPictureDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPictureDrawingSurface::RPictureDrawingSurface( ) :
#ifdef _WINDOWS
									RDcDrawingSurface( TRUE ),
	  								m_hMetafile( NULL )
#else
	  								m_hPict( NULL ),
	  								m_pGrafPort( NULL )
#endif
	{
	 NULL;
	}

// ****************************************************************************
//
// Function Name:		RPictureDrawingSurface::GetPicture( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
#ifdef _WINDOWS
inline HENHMETAFILE 	RPictureDrawingSurface::GetPicture( ) const
#else
inline PicHandle 		RPictureDrawingSurface::GetPicture( ) const
#endif
	{
#ifdef _WINDOWS
	return m_hMetafile;
#else
	return m_hPict;
#endif
	}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _PICTUREDRAWINGSURFACE_H_
