// ****************************************************************************
//
//  File Name:			PathPropertyImp.h
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Definition of the path property interface classes
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
//  $Logfile:: /PM8/PathComp/Include/PathPropertyImp.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PATHPROPERTYIMP_H_
#define		_PATHPROPERTYIMP_H_

#include "PropertyInterface.h"

class RPathDocument;
class RPathView;

// ****************************************************************************
//
//  Class Name:	RPathObjectPropertiesImp
//
//  Description:	Definition of the RPathObjectPropertiesImp class.
//
// ****************************************************************************
//
class RPathObjectPropertiesImp : public IObjectProperties
{
//	Construction & Destruction
public :
									RPathObjectPropertiesImp( const RComponentView* pView );
	static RInterface*		CreateInterface( const RComponentView* pView );

public:

	virtual BOOLEAN			FindProperty( YInterfaceId iid, RInterface** ppInterface );
//	virtual void				EnumProperties( const RColor& );

private:

	RPathDocument*				m_pPathDocument;
};

// ****************************************************************************
//
//  Class Name:	RPathOutlineAttribImp
//
//  Description:	Implementation of IOutlineAttrib
//
// ****************************************************************************
//
class RPathColorImp : public IColorInterface
{
// Constructor 
public:
											RPathColorImp( RColor& crColor, RPathDocument* pPathDoc  );

// Interfaces
public:

	// IFillColor
	virtual RColor						GetColor();
	virtual HRESULT					SetColor( const RColor& );

	virtual BOOLEAN					IsColorTypeSupported( RColor::EFillMethod );

private:

	RPathDocument*						m_pPathDocument;
	RColor&								m_crColor;
};

// ****************************************************************************
//
//  Class Name:	RPathOutlineAttribImp
//
//  Description:	Implementation of IOutlineAttrib
//
// ****************************************************************************
//
class RPathOutlineAttribImp : public IOutlineAttrib
{
// Constructor 
public:
											RPathOutlineAttribImp( OUTLINE_FORMAT& ofs, RPathDocument* pPathDoc );

// Interfaces
public:

	// IOutlineColor
	virtual RColor						GetColor();
	virtual HRESULT					SetColor( const RColor& );

	virtual BOOLEAN					IsColorTypeSupported( RColor::EFillMethod );

	// IOutlineAttribInterface
	//
	virtual uWORD						GetPenStyle();
	virtual void						SetPenStyle( uWORD );

	virtual uWORD						GetLineWeight();
	virtual void						SetLineWeight( uWORD );

	virtual OUTLINE_FORMAT			GetOutlineFormat();
	virtual void						SetOutlineFormat( const OUTLINE_FORMAT& );

private:

	RPathDocument*						m_pPathDocument;
	OUTLINE_FORMAT&					m_sOutlineFormat;
};

#endif // _PATHPROPERTYIMP_H_

