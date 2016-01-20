// ****************************************************************************
//
//  File Name:			BitmapImage.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RBitmapImage class
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
//  $Logfile:: /PM8/Framework/Include/BitmapImage.h                           $
//   $Author:: Lwilson                                                        $
//     $Date:: 3/08/99 7:16p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#ifndef _BITMAPIMAGE_H_
#define _BITMAPIMAGE_H_

#include "Image.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef _WINDOWS
	#define BaseDrawingSurface RDcDrawingSurface
#endif // _WINDOWS
#ifdef MAC
	#define BaseDrawingSurface RGpDrawingSurface
#endif // MAC

#ifdef _WINDOWS
	const kDefaultXDpi = 0;
	const kDefaultYDpi = 0;
#endif // _WINDOWS
#ifdef MAC
	const kDefaultXDpi = 72;
	const kDefaultYDpi = 72;
#endif // MAC
const int kMaxPaths = 128;

class RMemoryMappedBuffer;
class RClippingPath;

const YChunkTag kDibBufferChunkID = 0x04160001;

class FrameworkLinkage RBitmapImageBase : public RImage
	{
	// Operations
	public :
		virtual void						Initialize( YImageHandle yHandle ) = 0;
		virtual void						Initialize( RDrawingSurface& rDS, uLONG uWidthInPixels, uLONG uHeightInPixels ) = 0;
		virtual void						Render( RDrawingSurface& rDS, const RIntRect& rDestRect ) = 0;
		virtual void						Render( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect ) = 0;
		virtual void						RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect ) = 0;
		virtual void						RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rSrcRect, const RIntRect& rDestRect ) = 0;	
		virtual void						Read( RArchive& rArchive ) = 0;
		virtual void						Write( RArchive& rArchive ) const = 0;
		virtual void						Copy( RDataTransferTarget& rDataTarget, YDataFormat yDataFormat ) const = 0;
		virtual void						Paste( const RDataTransferSource& rDataSource, YDataFormat yDataFormat ) = 0;
		virtual YImageHandle				GetSystemHandle() const = 0;
		virtual RRealSize					GetSizeInLogicalUnits() const = 0;
		virtual uLONG						GetWidthInPixels( ) const = 0;
		virtual uLONG						GetHeightInPixels( ) const = 0;
		virtual YFloatType				GetAspectRatio() const {return (YFloatType)GetWidthInPixels() / (YFloatType)GetHeightInPixels();}


	};

enum PaletteOption
{
	kUseApplicationPalette = 0,
	kUseGrayScalePalette
};

class FrameworkLinkage RBitmapImage : public RBitmapImageBase
{
public:
	// Construction/destruction
											RBitmapImage(BOOLEAN fStoreOnDisk = FALSE);
											RBitmapImage(const RBitmapImage& rhs);
	virtual 								~RBitmapImage( );

	// Initialization
	virtual void						Initialize( YImageHandle yHandle );
	virtual void						Initialize( YResourceId nId );
	void									Initialize( uLONG uWidthInPixels, uLONG HeightInPixels, uLONG uBitDepth, uLONG sXDpi = kDefaultXDpi, uLONG sYDpi = kDefaultYDpi, PaletteOption paletteOption = kUseApplicationPalette );
	void									Initialize( RDrawingSurface& rDS, uLONG uWidthInPixels, uLONG uHeightInPixels );	
	void									Initialize( void* pRawData );
	void									Initialize( void* pRawData, void* pMemBuffer, int nBufferSize);

	void									Uninitialize();

	// Operations
	virtual void						Render( RDrawingSurface& rDS, const RIntRect& rDestRect );
	void									Render( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );
	void									RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect );
	void									RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rSrcRect, const RIntRect& rDestRect );	
	void									RenderAsObject( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect,const RRealSize& rSize, const R2dTransform& rTransform,  RPath*	path, bool bUseHiResBitmap = FALSE );
	void									RenderTransparent( RDrawingSurface &rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect );
	void									RenderToScreenOrPrinter( RDrawingSurface& rSrcDS, RDrawingSurface& rDstDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );
	virtual void						Read( RArchive& rArchive );
	virtual void						Write( RArchive& rArchive ) const;
	virtual void						Copy( RDataTransferTarget& rDataTarget, YDataFormat yDataFormat ) const;
	virtual void						Paste( const RDataTransferSource& rDataSource, YDataFormat yDataFormat );

	virtual YImageHandle				GetSystemHandle() const;
	virtual RRealSize					GetSizeInLogicalUnits() const;

	// Operator overrides
	virtual void						operator=(const RImage& rhs);
	void									operator=(const RBitmapImage& rhs);

	// Attributes
	uLONG									GetWidthInPixels( ) const;
	uLONG									GetHeightInPixels( ) const;
	uLONG									GetBitDepth( ) const;	
	uLONG									GetXDpi( ) const;
	uLONG									GetYDpi( ) const;
	uLONG									GetIndexOfColor( const RSolidColor& transparentColor ) const;
	void									SetTransparentColor(RSolidColor& rcTransparent){m_bTransparent = TRUE;	m_rcTransparentColor = rcTransparent;return;}
	RSolidColor							GetTransparentColor(){return m_rcTransparentColor;}
	BOOL									IsTransparent(void){return m_bTransparent;}
	RSolidColor							MakeTransparentColorUnique( int nTransparentIndex, int nUsedPaletteSize = 256 );
	void									DisableTransparency() {m_bTransparent = FALSE;}
	void									EnableTransparency() {m_bTransparent = TRUE;}
	void									SetAlphaChannel( RBitmapImage* pAlphaChannel );
	RBitmapImage*						GetAlphaChannel() { return m_pAlphaChannel; }
	RClippingPath*						GetClippingRPath(){return m_pClipPath;}
	RPath*								GetFrameRPath(){return m_pFramePath;}
	BOOL									SetRPath(RClippingPath* pPath);
	BOOL									SetFrameRPath(RPath* path);
	BOOL									PathFromImport();
	BOOL									DeleteRPath();
	
	// Low-level data access and manipulation
	void*									GetRawData() const;
	uLONG									GetRawDataSize() const;	
	void									UpdatePalette();

#ifdef MAC
	LPVOID								Lock( );
	void									Unlock( );
#endif

	virtual void						LockImage( );
	virtual void						UnlockImage( );

	// Static members used for interpreting raw data
	static uLONG						GetDibSize( void* pRawData );
	static void*						GetImageData( void* pRawData );
	static uLONG						GetImageDataSize( void* pRawData );
	static uLONG						GetImageDataSize( uLONG uWidth, uLONG uHeight, uLONG uBitDepth );	
	static void*						GetColorData( void* pRawData );
	static uLONG						GetColorDataSize( void* pRawData );
	static uLONG						GetColorDataSize( uLONG sBitDepth );
	static uLONG						GetBytesPerRow( uLONG uWidth, uLONG uBitDepth );	
	static uBYTE**						Get2dImageData( void* pRawData);

#ifdef _WINDOWS		
	static RGBQUAD						GetRgbQuad(BYTE sRed, BYTE sGreen, BYTE sBlue);	
#endif // _WINDOWS

protected:
	void									RenderTransparent( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );
	void									RenderSection( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );

private:
	// Internal data manipulation functions	
	YIntCoordinate						SkipOneBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const;
	YIntCoordinate						SkipZeroBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const;

	YIntCoordinate						SkipTransparentPixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight, WORD nTransIndex, uLONG nBitDepth ) const;
	YIntCoordinate						SkipTransparentPixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight ) const;
	YIntCoordinate						SkipOpaquePixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight, WORD nTransIndex, uLONG nBitDepth ) const;
	YIntCoordinate						SkipOpaquePixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight ) const;

	BOOL  								FindPathInData(void* pMemBuffer, int nBufferSize, const int nType);
	RPath*								CreateRPath();
	RPath*								RPathFromPoints();

	void									RenderToPrinterSubdivide( RDrawingSurface& rSrcDS, RDrawingSurface& rDstDS, const RIntRect& rSrcRect, const RIntRect& rDestRect );

#ifdef _WINDOWS
	void									InitializeDibSection();

#endif // _WINDOWS
	
private:
	// member data 	
#ifdef _WINDOWS		
	CArray<CPoint,CPoint>				m_ptArray[kMaxPaths];
	int									m_nSubPaths;
	RMemoryMappedBuffer*				m_pDibBuffer;
	HBITMAP								m_hBitmap;
	RSolidColor							m_rcTransparentColor;
	BOOL									m_bTransparent;
	char*									m_pClipBuffer;
	int									m_nClipBufferLen;

	RBitmapImage*						m_pAlphaChannel;
	RClippingPath*						m_pClipPath;
	RPath*								m_pFramePath;
	
	int									m_nLockCount;
#endif // _WINDOWS
#ifdef MAC
	HTBIT									m_hBitmap;
#endif // MAC
};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _BITMAPIMAGE_H_
