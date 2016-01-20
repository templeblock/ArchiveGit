//****************************************************************************
//
// File Name:  ImageGridCtrl.cpp
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Implementation for a multi-column listbox.
//
// Portability: Windows Specific
//
// Developed by:   Broderbund Software
//	                500 Redwood Blvd.
//                 Novato, CA 94948
//	                (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImageGridCtrl.cpp                                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "ImageGridCtrl.h"
#include "DcDrawingSurface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RImageGridCtrl::RImageGridCtrl() : RGridCtrlBase( kPushLike | k3dStyle | kIntegral ),
	m_pImageList( NULL ) 
{
}

RImageGridCtrl::~RImageGridCtrl()
{
}

void RImageGridCtrl::SetImageList( RBitmapImageList* pImageList, int nImages ) 
{
	if (nImages < 0)
	{
		nImages = pImageList->GetImageCount() ;
	}

	m_pImageList = pImageList ;

	InitStorage( nImages ) ;
	SetTopIndex( 0 ) ;
	Invalidate() ;
}

void RImageGridCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	RDcDrawingSurface ds ;
	ds.Initialize( lpDrawItemStruct->hDC ) ;

	RBitmapImageList* pImageList = m_pImageList ;
	RIntSize size = pImageList->GetImageSize() ;
	RIntRect rcDestRect( 0, 0, size.m_dx, size.m_dy ) ;

	CRect rect( lpDrawItemStruct->rcItem ) ;
	CPoint ptCenter = rect.CenterPoint() ;

	RIntSize szOffset( 
		ptCenter.x - rcDestRect.Width() / 2,
		ptCenter.y - rcDestRect.Height() / 2 ) ;

	rcDestRect.Offset( szOffset ) ;

	pImageList->Render( ds, lpDrawItemStruct->itemID, rcDestRect,
		RBitmapImageList::kTransparent ) ;

	ds.Draw3dRect( rcDestRect,
		RSolidColor( GetSysColor( COLOR_BTNSHADOW ) ), 
		RSolidColor( GetSysColor( COLOR_BTNHILIGHT ) ) ) ;

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		CRect rect = rcDestRect ;
		rect.DeflateRect( 2, 2, 1, 1 ) ;
//		rect.OffsetRect( 1, 1 ) ;

		DrawFocusRect( (HDC) ds.GetSurface(), rect ) ;
	}

	ds.DetachDCs( );

}

void RImageGridCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	RIntSize size = m_ImageList.GetImageSize() ;

	lpMeasureItemStruct->itemWidth  = size.m_dx ;
	lpMeasureItemStruct->itemHeight = size.m_dy ;
}
