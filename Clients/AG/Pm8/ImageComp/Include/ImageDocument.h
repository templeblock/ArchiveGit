// ****************************************************************************
//
//  File Name:			ImageDocument.h
//
//  Project:			Bitmap Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RImageDocument class
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
//  $Logfile:: /PM8/ImageComp/Include/ImageDocument.h                        $
//   $Author:: Rgrenfel                                                       $
//     $Date:: 3/08/99 1:11p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#ifndef	_IMAGEDOCUMENT_H_
#define	_IMAGEDOCUMENT_H_

#ifndef	_COMPONENTDOCUMENT_H_
#include	"ComponentDocument.h"
#endif

#ifndef	_BITMAPIMAGE_H_
#include	"BitmapImage.h"
#endif

#ifndef	_VECTORIMAGE_H_
#include	"VectorImage.h"
#endif

#ifndef	_MEMORYMAPPEDBUFFER_H_
#include	"MemoryMappedBuffer.h"
#endif

#ifndef	_IMAGELIBRARY_H_
#include	"ImageLibrary.h"
#endif

#ifndef	_IMAGEINTERFACE_H_
#include	"ImageInterface.h"
#endif

#ifndef	_EDGEOUTLINESETTINGS_H_
#include	"EdgeOutlineSettings.h"
#endif

class RClippingPath;

// ****************************************************************************
//
//  Class Name:	RImageDocument
//
//  Description:	The Bitmap document.
//
// ****************************************************************************
class RImageDocument : public RComponentDocument
{
friend class RImageInterfaceImp;
friend class REditImageInterfaceImp;
friend class RImagePropertiesImp;

// Construction & Destruction
public:	
												RImageDocument(RApplication* pApp, const RComponentAttributes& rComponentAttributes, const YComponentType& componentType, BOOLEAN fLoading);
	virtual									~RImageDocument();
	
// RComponentDocument overrides
public:
	virtual void							Render(RDrawingSurface& rDS, const R2dTransform& rTransform, const RIntRect& rRender, const RRealSize& rSize) const;	
	void										Render(RDrawingSurface& rDS, const R2dTransform& rTransform, const RIntRect& rRender, const RRealSize& rSize);
	virtual void							RenderShadow(RDrawingSurface& rDS, const R2dTransform& rTransform, const RIntRect& rRender, const RRealSize& rSize, const RSolidColor& shadowColor) const;
	void										RenderBitmap(RBitmapImage& rBitmap, RDrawingSurface& rDS, const R2dTransform& rTransform, const RRealSize& rSize);
	void										RenderVector(RVectorImage& rVector, RDrawingSurface& rDS, const R2dTransform& rTransform, const RRealSize& rSize);
	virtual RComponentView*				CreateView(const YComponentBoundingRect& boundingRect, RView* pParentView);
	virtual void							Read(RChunkStorage& rStorage, EReadReason eReason);
	virtual void							Write(RChunkStorage& rStorage, EWriteReason reason) const;
	virtual void							ResetToDefaultSize( RDocument* pParentDocument );
	virtual BOOLEAN						GetDefaultSize( const RRealSize &sizePanel, RRealSize *psizeDefault );
	virtual void							Paste(YDataFormat yDataFormat, const RDataTransferSource& rDataTransferSource);
	virtual void							InvalidateAllViews( BOOLEAN fErase = TRUE );
	virtual void							ReleaseResources( );

// RImageInterface accessors
public :
	virtual RImage*						GetImage();
	virtual void							SetImage(const RImage& rImage, BOOLEAN fResize = TRUE, BOOLEAN fRetainPathname = FALSE);
	virtual void							SetImage(IBufferInterface* pImageData, BOOLEAN fResize = TRUE, BOOLEAN fLinkData = FALSE);

	virtual BOOLEAN						Export(const RMBCString& rPath, EImageFormat eFormat);	
	
	virtual BOOLEAN						Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath = NULL, BOOLEAN fResizeToDefaultSize = TRUE);
	virtual BOOLEAN						Load(const RMBCString& rPath, YException& hException);	
	virtual BOOLEAN						LoadForPreview(const RMBCString& rPath, const RIntSize& rMaxSize, YException& hException);
	virtual BOOLEAN						IsPreview() const;
	virtual const RMBCString&			GetPath() const;

	virtual BOOLEAN						QueryEditImage();
	virtual BOOLEAN						MakeImageEditable();

	void 										QueryRawImageSize( RIntSize *pSize );
	
// REditImageInterface accessors
public :
	virtual void							EnableEditResolution(const RIntSize& rMaxEditSize);
	virtual void							DisableEditResolution();

	virtual const RImageEffects&		GetImageEffects() ;
	virtual void							SetImageEffects(const RImageEffects& rEffects);
	virtual void							ResetImageEffects();
	virtual void							ApplyImageEffects( RClippingPath* pClipPath = NULL );


// RImageTypeInterface accessors
public :
	virtual BOOLEAN						QueryImageIsAnAnimatedGIF();
	virtual CPGIFAnimator *				CreateGIFAnimator();
	virtual BOOLEAN						QueryIsWebImageFormat();
	virtual EImageFormat					QueryImageFormat();

// Implementation
private:	
	void										ResetProxy();
	void										ResizeView(RRealSize rNewViewSize, BOOLEAN fFitInsidePanel);
	BOOLEAN									IsCompressed( );
	void										CompressInternalData( );
	void										WritePlttData( const RMBCString& rOutputFileName );	

	void										SetCropRectFromPath( RPath* path, BOOLEAN fUpdatePath );

// Data members
protected:
	RTempFileBuffer						m_rInternalDataBuffer;
	BOOLEAN									m_fLinkedData;

private:	
	// The base image and effects
	RImage*									m_pImage;
	RImageEffects							m_rEffects;	
	
	// Bitmaps used in assisting rendering
	RBitmapImage							m_rProxyBitmap;	
	RBitmapImage							m_rMaskBitmap;	
	RBitmapImage							m_rPlaceholderBitmap;

	// The image library
	RImageLibrary							m_rLibrary;

	// The previous transform passed to Render()
	RRealSize								m_rLastSize;
	R2dTransform							m_rLastTransform;
	YTint										m_nLastTint;

	// A flag indicating the print quality to use
	BOOLEAN									m_fHiResPhotos;

	// A flag indicating that the image in this component was loaded by
	// calling LoadForPreview()
	BOOLEAN									m_fPreview;
	
	// Data members used in editing images
	BOOLEAN									m_fEditResolution;
	RPageFileBuffer						m_rEditDataBuffer;

	// The blend color (currently only solid colors are allowed)
	// At some time, blending with another texture would be a
	// cool effect.
	RColor									m_rTintColor;

	// The image path, if loaded from a file
	RMBCString								m_rPath;

	//	Remember whether we have been unloaded.
	BOOLEAN									m_fResourcesReleased;

	// The previous edgeOutline (recreate the proxy if changed)
	REdgeOutlineSettings					m_nLastEdgeOutline;

// Debugging support
#ifdef TPSDEBUG
public:
	virtual void Validate() const;
#endif TPSDEBUG
};


// ****************************************************************************
//
//  Class Name:	RPSDPhotoDocument
//
//  Description:	The PSD Photo Bitmap document.  This is here to allow the
//						subclass of various functions to be specific to the PSD Photo,
//						but not to the generic Image
//
// ****************************************************************************
class RPSDPhotoDocument : public RImageDocument
{
public:
												// Construction & Destruction
												RPSDPhotoDocument(RApplication* pApp, const RComponentAttributes& rComponentAttributes, const YComponentType& componentType, BOOLEAN fLoading);

// RImageInterface accessors
public :
	virtual BOOLEAN						Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath = NULL, BOOLEAN fResizeToDefaultSize = TRUE);

// RImageDocument overrides
public:
	virtual void							Read(RChunkStorage& rStorage, EReadReason eReason);
	virtual void							Write(RChunkStorage& rStorage, EWriteReason reason) const;

private:
	// The partial path, if saved as part of a QSL
	RMBCString								m_rPartialPath;

};

#endif	// _IMAGEDOCUMENT_H_
