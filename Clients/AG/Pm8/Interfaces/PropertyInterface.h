// ****************************************************************************
//
//  File Name:			PropertyInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the property interfaces
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1999 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Interfaces/PropertyInterface.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PROPERTYINTERFACE_H_
#define		_PROPERTYINTERFACE_H_

#include "Interface.h"

const YInterfaceId	kObjectPropertiesInterfaceId        = 200;

const YInterfaceId	kFillColorAttribInterfaceId         = 210;
const YInterfaceId	kBackgroundColorAttribInterfaceId   = 211;
const YInterfaceId	kShadowColorAttribInterfaceId       = 212;
const YInterfaceId	kOutlineColorAttribInterfaceId      = 213;

const	YInterfaceId	kOutlineAttribInterfaceId           = 220;
const	YInterfaceId	kShadowAttribInterfaceId            = 221;

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const uWORD kFixedWeight = 0x8000;

typedef struct tagOUTLINE_FORMAT
{
	uWORD				m_uwPenStyle;
	uWORD				m_uwLineWeight;
	YPlatformColor m_yPenColor;

} OUTLINE_FORMAT, *LPOUTLINE_FORMAT;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

// ****************************************************************************
//
//  Class Name:	IObjectProperties
//
//  Description:	Definition of the IObjectProperties interface.
//
// ****************************************************************************
//
class IObjectProperties : public RInterface
{
//	Construction & Destruction
public :
									IObjectProperties( const RComponentView* pView ) : RInterface( pView ) { ; }

public:

	virtual BOOLEAN			FindProperty( YInterfaceId iid, RInterface** ppInterface ) = 0;
//	virtual void				EnumProperties( const RColor& ) = 0;
};

// ****************************************************************************
//
//  Class Name:	IColorInterface
//
//  Description:	A pure virtual class interfacing with a color component 
//
// ****************************************************************************
//
class IColorInterface : public RInterface
{
//	Construction & Destruction
public :
									IColorInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

public:

	virtual RColor				GetColor() = 0;
	virtual HRESULT			SetColor( const RColor& ) = 0;

	virtual BOOLEAN			IsColorTypeSupported( RColor::EFillMethod ) = 0;
};

typedef IColorInterface	IFillColor;
typedef IColorInterface	IBackgroundColor;
typedef IColorInterface	IOutlineColor;

// ****************************************************************************
//
//  Class Name:	ROutlineInterface
//
//  Description:	A pure virtual class interfacing with a component 
//                that supports outlines
//
// ****************************************************************************
//
class IOutlineAttrib : public IOutlineColor
{
public:

	enum ELineStyle			{ 	kNone, kHairLine, kThin, kMedium, kThick, kExtraThick, kFixed, kDashed };

//	Construction & Destruction
public :
									IOutlineAttrib( const RComponentView* pView ) : IColorInterface( pView ) { ; }

public :

	virtual uWORD				GetPenStyle() = 0;
	virtual void				SetPenStyle( uWORD ) = 0;

	virtual uWORD				GetLineWeight() = 0;
	virtual void				SetLineWeight( uWORD ) = 0;

	virtual OUTLINE_FORMAT	GetOutlineFormat() = 0;
	virtual void				SetOutlineFormat( const OUTLINE_FORMAT& ) = 0;
};

#endif	//	_PROPERTYINTERFACE_H_
