// ****************************************************************************
//
//  File Name:			ImageInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RImageInterface class
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
//  $Logfile:: /PM8/Interfaces/ImageInterface.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_IMAGEINTERFACE_H_
#define	_IMAGEINTERFACE_H_

#ifndef	_INTERFACE_H_
#include	"Interface.h"
#endif

#ifndef	_IMAGELIBRARY_H_
#include	"ImageLibrary.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStorage;
class RClippingPath;
class CPGIFAnimator;
class IBufferInterface;

// ****************************************************************************
//
//  Class Name:	RImageInterface
//
//  Description:	A pure virtual class interfacing with an Image component
//
// ****************************************************************************
//
const	YInterfaceId kImageInterfaceId = 80;
class RImageInterface : public RInterface
{
//	Construction & Destruction
public :
												RImageInterface(const RComponentView* pView) : RInterface(pView) {}

//	Operations
public :
	virtual RImage*						CopySourceImage() = 0;
	virtual RImage*						GetImage() = 0 ;
	virtual void							SetImage(const RImage& rImage, BOOLEAN fResize = TRUE, BOOLEAN fRetainPathname = FALSE) = 0;
	virtual void							SetImage(IBufferInterface* pImageData, BOOLEAN fResize = TRUE, BOOLEAN fLinkData = FALSE) = 0;

	virtual BOOLEAN						Export( const RMBCString& rPath, EImageFormat eFormat ) = 0;
	virtual BOOLEAN						Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath = NULL) = 0;
	virtual BOOLEAN						Load(const RMBCString& rPath, YException& hException) = 0;	
	virtual BOOLEAN						LoadForPreview(const RMBCString& rPath, const RIntSize& rMaxSize, YException& hException) = 0;
	virtual BOOLEAN						IsPreview() const = 0;
	virtual const RMBCString&			GetPath() const = 0;

	virtual BOOLEAN						QueryEditImage() = 0;
	virtual BOOLEAN						MakeImageEditable() = 0;

	virtual void							QueryRawImageSize( RIntSize *pSize ) = 0;
};

// ****************************************************************************
//
//  Class Name:	struct RImageEffects
//
//  Description:	A class to encapsulate image effects
//
// ****************************************************************************
//
struct RImageEffects
{	
	YBrightness		m_nBrightness;
	YContrast		m_nContrast;
	YSharpness		m_nSharpness;	
	RRealRect		m_rCropArea;
};

// ****************************************************************************
//
//  Class Name:	REditImageInterface
//
//  Description:	A pure virtual class interfacing with an Image component
//
// ****************************************************************************
//
const YInterfaceId kEditImageInterfaceId = 81;
class REditImageInterface : public RInterface
{
//	Construction & Destruction
public :
												REditImageInterface(const RComponentView* pView) : RInterface(pView) {}

//	Operations
public :
	virtual void							EnableEditResolution(const RIntSize& rMaxEditSize) = 0;
	virtual void							DisableEditResolution() = 0;

	virtual const RImageEffects&		GetImageEffects() = 0 ;
	virtual void							SetImageEffects(const RImageEffects& rEffects) = 0;
	virtual void							ResetImageEffects() = 0;
	virtual void							ApplyImageEffects() = 0;

	virtual void							SetClippingPath( const RClippingPath* path ) = 0;
	virtual BOOLEAN						GetClippingPath( RClippingPath& rPath) = 0;
};


// ****************************************************************************
//
//  Class Name:	RImageTypeInterface
//
//  Description:	A pure virtual class interfacing with an Image component
//
// ****************************************************************************
//
const YInterfaceId kImageTypeInterfaceId = 82;
class RImageTypeInterface : public RInterface
{
//	Construction & Destruction
public :
												RImageTypeInterface(const RComponentView* pView) : RInterface(pView) {}

//	Operations
public :
	virtual BOOLEAN						QueryImageIsAnAnimatedGIF() = 0;
	virtual CPGIFAnimator *				CreateGIFAnimator() = 0;
	virtual BOOLEAN						QueryIsWebImageFormat() = 0;
	virtual EImageFormat					QueryImageFormat() = 0;
};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_IMAGEINTERFACE_H_
