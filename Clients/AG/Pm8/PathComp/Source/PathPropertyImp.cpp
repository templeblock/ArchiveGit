// ****************************************************************************
//
//  File Name:			PathPropertyImp.cpp
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
//  $Logfile:: /PM8/PathComp/Source/PathPropertyImp.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "PathIncludes.h"
ASSERTNAME

#include "PathPropertyImp.h"
#include "PathDocument.h"
#include "PathView.h"


//////////////////////////////////////////////////////////////////////////////////////
// RObjectPropertiesImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RPathObjectPropertiesImp::RPathObjectPropertiesImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathObjectPropertiesImp::RPathObjectPropertiesImp( const RComponentView* pView )
	: IObjectProperties( pView )
	, m_pPathDocument( (RPathDocument *) pView->GetComponentDocument() )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RPathObjectPropertiesImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RPathView, pView );
	return new RPathObjectPropertiesImp( static_cast<const RPathView*>( pView ) );
}

BOOLEAN RPathObjectPropertiesImp::FindProperty( YInterfaceId iid, RInterface** ppInterface )
{
	switch (iid)
	{
	case kOutlineAttribInterfaceId:
	case kOutlineColorAttribInterfaceId:
		*ppInterface = new RPathOutlineAttribImp( m_pPathDocument->m_sOutlineFormat, m_pPathDocument );
		break;

	case kFillColorAttribInterfaceId:
		*ppInterface = new RPathColorImp( m_pPathDocument->m_rFillColor, m_pPathDocument );
		break;

	default:
		*ppInterface = NULL;
	}

	return (*ppInterface != NULL);
}


//////////////////////////////////////////////////////////////////////////////////////
// RPathColorImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RPathColorImp::RPathColorImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathColorImp::RPathColorImp( RColor& crColor, RPathDocument* pDocument )	
	: IColorInterface( NULL )
	, m_pPathDocument( pDocument )
	, m_crColor( crColor )
{
}

RColor RPathColorImp::GetColor()
{
	return m_crColor;
}

HRESULT RPathColorImp::SetColor( const RColor& crColor )
{
	m_crColor = crColor;
	m_pPathDocument->InvalidateAllViews();

	return S_OK;
}

BOOLEAN RPathColorImp::IsColorTypeSupported( RColor::EFillMethod /*eMethod*/ )
{
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////
// RPathOutlineAttribImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RPathOutlineAttribImp::RPathOutlineAttribImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathOutlineAttribImp::RPathOutlineAttribImp( OUTLINE_FORMAT& ofs, RPathDocument* pPathDoc )
	: IOutlineAttrib( NULL )
	, m_pPathDocument( pPathDoc )
	, m_sOutlineFormat( ofs )
{
}


RColor RPathOutlineAttribImp::GetColor()
{
	return RColor( m_sOutlineFormat.m_yPenColor );
}

HRESULT RPathOutlineAttribImp::SetColor( const RColor& crColor )
{
	if (RColor::kSolid != crColor.GetFillMethod())
		return E_INVALIDARG;

	m_sOutlineFormat.m_yPenColor = crColor.GetSolidColor();
	m_pPathDocument->InvalidateAllViews();

	return S_OK;
}

BOOLEAN RPathOutlineAttribImp::IsColorTypeSupported( RColor::EFillMethod eMethod )
{
	if (RColor::kSolid == eMethod)
		return TRUE;

	return FALSE;
}

uWORD RPathOutlineAttribImp::GetPenStyle()
{
	return m_sOutlineFormat.m_uwPenStyle;
}

void RPathOutlineAttribImp::SetPenStyle( uWORD yPenStyle )
{
	m_sOutlineFormat.m_uwPenStyle = yPenStyle;
	m_pPathDocument->InvalidateAllViews();
}

uWORD RPathOutlineAttribImp::GetLineWeight()
{
	return m_sOutlineFormat.m_uwLineWeight;
}

void RPathOutlineAttribImp::SetLineWeight( uWORD uwLineWeight )
{
	m_sOutlineFormat.m_uwLineWeight = uwLineWeight;
	m_pPathDocument->InvalidateAllViews();
}

OUTLINE_FORMAT	RPathOutlineAttribImp::GetOutlineFormat()
{
	return m_sOutlineFormat;
}

void RPathOutlineAttribImp::SetOutlineFormat( const OUTLINE_FORMAT& sOutlineFormat )
{
	m_sOutlineFormat = sOutlineFormat;
	m_pPathDocument->InvalidateAllViews();
}


