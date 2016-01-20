// ****************************************************************************
//
//  File Name:			RenderCache.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RRenderCache class
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
//  $Logfile:: /PM8/Framework/Include/RenderCache.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_RENDERCACHE_H_
#define		_RENDERCACHE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;
class RDeviceDependentBitmapImage;
class RScratchBitmapImage;
class RBitmapImageBase;


//#define	USE_SCRATCHBITMAPIMAGE	1

// ****************************************************************************
//
//  Class Name:	RRenderCache
//
//  Description:	Base class for render caches. Use a class derived from this
//						class when you wish to cache rendered objects in an offscreen
//
// ****************************************************************************
//
class FrameworkLinkage RRenderCache : public RObject
	{
	// Construction & destruction
	public :
												RRenderCache( );
		virtual								~RRenderCache( );

	// Operations
	public :
		void									Render( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rcRender );

		void									Enable( BOOLEAN fEnable );
		void									SetRenderThreshold( YTickCount renderThreshold );
		void									Invalidate( );

	// Implementation
	protected :
		virtual void						RenderData( RDrawingSurface& drawingSurface,
																const RRealSize& size,
																const R2dTransform& transform,
																const RIntRect& rcRender ) const = 0;

		virtual RBitmapImageBase *		AllocateCacheBitmap( RDrawingSurface& referenceSurface, uLONG uWidthInPixels, uLONG uHeightInPixels );
		virtual RBitmapImageBase *		AllocateCacheMask( uLONG uWidthInPixels, uLONG uHeightInPixels );

		void									SetCacheBitmap( RBitmapImageBase *pBitmap );
		void									SetCacheMask( RBitmapImageBase *pMask );
				
	// Implementation
	private :
		void									DeleteOffscreens( );
		void									CreateOffscreens( RDrawingSurface& referenceSurface, const RRealSize& size, const R2dTransform& transform );
		BOOLEAN								IsCacheValid( RDrawingSurface& drawingSurface, const RRealSize& size, const R2dTransform& transform ) const;
		void									RenderCache( RDrawingSurface& drawingSurface, const RRealSize& size, const R2dTransform& transform ) const;

	// Member data
	private :
		R2dTransform						m_LastTransform;
		YTint									m_LastTint;
		YTickCount							m_RenderThreshold;
		YTickCount							m_LastRenderTime;
#if 0
#ifdef USE_SCRATCHBITMAPIMAGE // REVIEW GCB 3/6/98 - this may need to change to an RBitmapImage
		RScratchBitmapImage*				m_pBitmap;
		RScratchBitmapImage*				m_pMaskBitmap;	//	MDH - 3/9/98 Should be same format as pBitmap for speed
#else
		RBitmapImage*						m_pBitmap;
		RBitmapImage*						m_pMaskBitmap;	//	MDH - 3/9/98 Should be same format as pBitmap for speed
#endif
#else
		RBitmapImageBase *				m_pBitmap;
		RBitmapImageBase *				m_pMaskBitmap;	// Rich - 4/15/98 Use allocation member now so that specific cache derived classes can specify image type.
#endif
		BOOLEAN								m_fEnabled;
		BOOLEAN								m_fValid;
		RRealSize							m_LastSize;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _RENDERCACHE_H_
