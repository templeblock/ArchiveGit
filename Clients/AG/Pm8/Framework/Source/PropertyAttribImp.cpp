// ****************************************************************************
//
//  File Name:			PropertyAttribImp.cpp
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Implementation of the path property interface classes
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/PropertyAttribImp.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "PropertyAttribImp.h"
#include "ComponentDocument.h"

//////////////////////////////////////////////////////////////////////////////////////
// RColorImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RColorImp::RColorImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RColorImp::RColorImp( RColor& crColor, RComponentDocument* pDocument )	
	: IColorInterface( NULL )
	, m_pDocument( pDocument )
	, m_crColor( crColor )
{
}

RColor RColorImp::GetColor()
{
	return m_crColor;
}

HRESULT RColorImp::SetColor( const RColor& crColor )
{
	m_crColor = crColor;
	m_pDocument->InvalidateAllViews();

	return S_OK;
}

BOOLEAN RColorImp::IsColorTypeSupported( RColor::EFillMethod /*eMethod*/ )
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// RSolidColorImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RSolidColorImp::RSolidColorImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSolidColorImp::RSolidColorImp( RSolidColor& crColor, RComponentDocument* pDocument )	
	: IColorInterface( NULL )
	, m_pDocument( pDocument )
	, m_crColor( crColor )
{
}

RColor RSolidColorImp::GetColor()
{
	return m_crColor;
}

HRESULT RSolidColorImp::SetColor( const RColor& crColor )
{
	if (RColor::kSolid != crColor.GetFillMethod())
		return E_INVALIDARG;

	m_crColor = crColor.GetSolidColor();
	m_pDocument->InvalidateAllViews();

	return S_OK;
}

BOOLEAN RSolidColorImp::IsColorTypeSupported( RColor::EFillMethod eMethod )
{
	if (RColor::kSolid == eMethod)
		return TRUE;

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////
// ROutlineAttribImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	ROutlineAttribImp::ROutlineAttribImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
ROutlineAttribImp::ROutlineAttribImp( OUTLINE_FORMAT& ofs, RComponentDocument* pDocument )
	: IOutlineAttrib( NULL )
	, m_pDocument( pDocument )
	, m_sOutlineFormat( ofs )
{
}

RColor ROutlineAttribImp::GetColor()
{
	return RColor( m_sOutlineFormat.m_yPenColor );
}

HRESULT ROutlineAttribImp::SetColor( const RColor& crColor )
{
	if (RColor::kSolid != crColor.GetFillMethod())
		return E_INVALIDARG;

	m_sOutlineFormat.m_yPenColor = crColor.GetSolidColor();
	m_pDocument->InvalidateAllViews();

	return S_OK;
}

BOOLEAN ROutlineAttribImp::IsColorTypeSupported( RColor::EFillMethod eMethod )
{
	if (RColor::kSolid == eMethod)
		return TRUE;

	return FALSE;
}

uWORD ROutlineAttribImp::GetPenStyle()
{
	return m_sOutlineFormat.m_uwPenStyle;
}

void ROutlineAttribImp::SetPenStyle( uWORD yPenStyle )
{
	m_sOutlineFormat.m_uwPenStyle = yPenStyle;
	m_pDocument->InvalidateAllViews();
}

uWORD ROutlineAttribImp::GetLineWeight()
{
	return m_sOutlineFormat.m_uwLineWeight;
}

void ROutlineAttribImp::SetLineWeight( uWORD uwLineWeight )
{
	m_sOutlineFormat.m_uwLineWeight = uwLineWeight;
	m_pDocument->InvalidateAllViews();
}

OUTLINE_FORMAT	ROutlineAttribImp::GetOutlineFormat()
{
	return m_sOutlineFormat;
}

void ROutlineAttribImp::SetOutlineFormat( const OUTLINE_FORMAT& sOutlineFormat )
{
	m_sOutlineFormat = sOutlineFormat;
	m_pDocument->InvalidateAllViews();
}
