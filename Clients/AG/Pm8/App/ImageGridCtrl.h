//****************************************************************************
//
// File Name:   ImageGridCtrl.h
//
// Project:     Renaissance
//
// Author:      Lance Wilson
//
// Description: Definition class for a multi-column listbox.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	              500 Redwood Blvd.
//               Novato, CA 94948
//	              (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImageGridCtrl.h                                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _IMAGEGRIDCTRL_H_
#define _IMAGEGRIDCTRL_H_

#include "WinGridCtrl.h"
#include "WindowsImageList.h"


class RImageGridCtrl : public RGridCtrlBase
{
public:
						RImageGridCtrl() ;
	virtual			~RImageGridCtrl() ;

public:

	void				SetImageList( RBitmapImageList* pImageList, int nImages = -1 ) ;

protected:

						//
						// Overrides
						//
	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) ;

private:

	RBitmapImageList*	m_pImageList ;
	RBitmapImageList	m_ImageList ;
	WORD					m_cxImageCols ;
} ;

/////////////////////////////////////////////////////////////////////////////

#endif // _IMAGEGRIDCTRL_H_
