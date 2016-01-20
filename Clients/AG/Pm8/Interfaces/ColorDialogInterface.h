// ****************************************************************************
//
//  File Name:			ColorDialogInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the RIColorDialog class
//
//  Portability:		Platform independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Interfaces/ColorDialogInterface.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COLORDIALOGINTERFACE_H_
#define		_COLORDIALOGINTERFACE_H_

#include "Interface.h"

const YInterfaceId	kColorDialogInterfaceId = 101;

const uWORD kShowTransparent = 0x0001 ;
const uWORD kShowGradients   = 0x0002 ;
const uWORD kAllowGradients  = 0x0004 ;
const uWORD kExcludeTextures = 0x0008 ;
const uWORD kColorDefault    = kShowTransparent | kAllowGradients ;

class RColorDialogInterface : public RIUnknown
{
public:
	
	virtual BOOLEAN				IsSelectedColorSolid() const = 0;
	virtual const RSolidColor	SelectedSolidColor() const = 0;
	virtual const RColor&		SelectedColor() const = 0;
	virtual const RColor&		SelectedColor( const RIntRect& rcBounds ) = 0;
	virtual void					SetColor( const RColor& color ) = 0;

	virtual int						DoModal( CWnd* pParent, UINT uwFlags ) = 0;
};

#endif // _COLORDIALOGINTERFACE_H_
