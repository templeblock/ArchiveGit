// ****************************************************************************
//
//  File Name:			PropertyAttribImp.h
//
//  Project:			Renaissance Framework
//
//  Author:				Lance Wilson
//
//  Description:		Definition of the property interface classes
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
//  $Logfile:: /PM8/Framework/Include/PropertyAttribImp.h                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PROPERTYATTRIBIMP_H_
#define		_PROPERTYATTRIBIMP_H_

#include "PropertyInterface.h"

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RColorImp
//
//  Description:	Implementation of IColorInterface
//
// ****************************************************************************
//
class FrameworkLinkage RColorImp : public IColorInterface
{
// Constructor 
public:
											RColorImp( RColor& crColor, RComponentDocument* pDocument );

// Interfaces
public:

	// IFillColor
	virtual RColor						GetColor();
	virtual HRESULT					SetColor( const RColor& );

	virtual BOOLEAN					IsColorTypeSupported( RColor::EFillMethod );

private:

	RComponentDocument*				m_pDocument;
	RColor&								m_crColor;
};

// ****************************************************************************
//
//  Class Name:	RSolidColorImp
//
//  Description:	Implementation of IColorInterface
//
// ****************************************************************************
//
class FrameworkLinkage RSolidColorImp : public IColorInterface
{
// Constructor 
public:
											RSolidColorImp( RSolidColor& crColor, RComponentDocument* pDocument );

// Interfaces
public:

	// IFillColor
	virtual RColor						GetColor();
	virtual HRESULT					SetColor( const RColor& );

	virtual BOOLEAN					IsColorTypeSupported( RColor::EFillMethod );

private:

	RComponentDocument*				m_pDocument;
	RSolidColor&						m_crColor;
};

// ****************************************************************************
//
//  Class Name:	ROutlineAttribImp
//
//  Description:	Implementation of IOutlineAttrib
//
// ****************************************************************************
//
class FrameworkLinkage ROutlineAttribImp : public IOutlineAttrib
{
// Constructor 
public:
											ROutlineAttribImp( OUTLINE_FORMAT& ofs, RComponentDocument* pDocument );

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

	RComponentDocument*				m_pDocument;
	OUTLINE_FORMAT&					m_sOutlineFormat;
};

#endif // _PROPERTYATTRIBIMP_H_

