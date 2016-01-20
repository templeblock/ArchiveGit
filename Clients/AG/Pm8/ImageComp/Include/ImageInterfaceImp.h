// ****************************************************************************
//
//  File Name:			ImageInterface.h
//
//  Project:			Renaissance Image Component
//
//  Author:				G. Brown, M. Houle
//
//  Description:		Declaration of the RImageInterfaceImp class
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
//  $Logfile:: /PM8/ImageComp/Include/ImageInterfaceImp.h                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_IMAGEINTERFACEIMP_H_
#define	_IMAGEINTERFACEIMP_H_

#ifndef	_IMAGEINTERFACE_H_
#include	"ImageInterface.h"
#endif

#ifndef	_PROPERTYINTERFACE_H_
#include "PropertyInterface.h"
#endif

#ifndef	_PROPERTYATTRIBIMP_H_
#include "PropertyAttribImp.h"
#endif

class RImageDocument;
class RImageView;
class RImage;
class RClippingPath;

// ****************************************************************************
//
//  Class Name:	RImageInterfaceImp
//
//  Description:	Interface implementation
//
// ****************************************************************************
//
class RImageInterfaceImp : public RImageInterface
{
//	Construction & Destruction
public :
												RImageInterfaceImp(const RImageView* pView);
	static RInterface*					CreateInterface(const RComponentView* pView);

//	Operations
public :
	virtual RImage*						CopySourceImage();
	virtual RImage*						GetImage();
	virtual void							SetImage(const RImage& rImage, BOOLEAN fResize = TRUE, BOOLEAN fRetainPathname = FALSE);
	virtual void							SetImage(IBufferInterface* pImageData, BOOLEAN fResize = TRUE, BOOLEAN fLinkData = FALSE);
	
	virtual BOOLEAN						Export( const RMBCString& rPath, EImageFormat eFormat ) ;
	virtual BOOLEAN						Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath = NULL);
	virtual BOOLEAN						Load(const RMBCString& rPath, YException& hException);	
	virtual BOOLEAN						LoadForPreview(const RMBCString& rPath, const RIntSize& rMaxSize, YException& hException);
	virtual BOOLEAN						IsPreview() const;
	virtual const RMBCString&			GetPath() const;

	virtual BOOLEAN						QueryEditImage();
	virtual BOOLEAN						MakeImageEditable();

	virtual void							QueryRawImageSize( RIntSize *pSize );

//	Private Members
private :
	RImageDocument*						m_pImageDocument;
} ;

// ****************************************************************************
//
//  Class Name:	REditImageInterfaceImp
//
//  Description:	Interface implementation
//
// ****************************************************************************
//
class REditImageInterfaceImp : public REditImageInterface
{
//	Construction & Destruction
public :
												REditImageInterfaceImp(const RImageView* pView);
	static RInterface*					CreateInterface(const RComponentView* pView);

//	Operations
public :
	virtual void							EnableEditResolution(const RIntSize& rMaxEditSize);
	virtual void							DisableEditResolution();

	virtual const RImageEffects&		GetImageEffects() ;
	virtual void							SetImageEffects(const RImageEffects& rEffects);
	virtual void							ResetImageEffects();
	virtual void							ApplyImageEffects();

	virtual void							SetClippingPath( const RClippingPath* path );
	virtual BOOLEAN						GetClippingPath( RClippingPath& rPath);

//	Private Members
private :
	RImageDocument*						m_pImageDocument;
};

// ****************************************************************************
//
//  Class Name:	RImageTypeInterfaceImp
//
//  Description:	Interface Implementation
//
// ****************************************************************************
//
class RImageTypeInterfaceImp : public RImageTypeInterface
{
//	Construction & Destruction
public :
												RImageTypeInterfaceImp(const RImageView* pView);
	static RInterface*					CreateInterface(const RComponentView* pView);

//	Operations
public :
	virtual BOOLEAN						QueryImageIsAnAnimatedGIF();
	virtual CPGIFAnimator *				CreateGIFAnimator();
	virtual BOOLEAN						QueryIsWebImageFormat();
	virtual EImageFormat					QueryImageFormat();

//	Private Members
private :
	RImageDocument*						m_pImageDocument;
};

// ****************************************************************************
//
//  Class Name:	RImagePropertiesImp
//
//  Description:	Definition of the RImagePropertiesImp class.
//
// ****************************************************************************
//
class RImagePropertiesImp : public IObjectProperties
{
//	Construction & Destruction
public :
									RImagePropertiesImp( const RComponentView* pView );
	static RInterface*		CreateInterface( const RComponentView* pView );

public:

	virtual BOOLEAN			FindProperty( YInterfaceId iid, RInterface** ppInterface );
//	virtual void				EnumProperties(  );

private:

	RImageDocument*			m_pDocument;
};

// ****************************************************************************
//
//  Class Name:	RImageFillColorImp
//
//  Description:	Definition of the RImageFillColorImp class.
//
// ****************************************************************************
//
class RImageFillColorImp : public RColorImp
{
// Constructor 
public:
											RImageFillColorImp( RColor& crColor, RComponentDocument* pDocument );
	virtual BOOLEAN					IsColorTypeSupported( RColor::EFillMethod );

};

#endif	//	_IMAGEINTERFACEIMP_H_
