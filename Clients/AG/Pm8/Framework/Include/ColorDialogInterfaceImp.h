// ****************************************************************************
//
//  File Name:	  ColorDialogInterfaceImp.h
//
//  Project:	  Renaissance Application Framework
//
//  Author:		  Lance Wilson
//
//  Description: Declaration of the RColorDialogInterfaceImp class
//
//  Portability: Platform independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ColorDialogInterfaceImp.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COLORDIALOGINTERFACEIMP_H_
#define		_COLORDIALOGINTERFACEIMP_H_

#include "ColorDialogInterface.h"

class FrameworkLinkage RColorDialogInterfaceImp : public RColorDialogInterface
{
public:

	// Construction
										RColorDialogInterfaceImp();
	virtual							~RColorDialogInterfaceImp() {}
	
	// RIUnknown Implementation
	virtual uLONG					AddRef( void ); 
	virtual uLONG					Release( void ); 
	virtual BOOLEAN				QueryInterface( YInterfaceId, void ** ppvObject );

	// RIColorDialog Implementation
	virtual BOOLEAN				IsSelectedColorSolid() const;
	virtual const RSolidColor	SelectedSolidColor() const;
	virtual const RColor&		SelectedColor() const;
	virtual const RColor&		SelectedColor( const RIntRect& rcBounds );
	virtual void					SetColor( const RColor& color );

	virtual int						DoModal( CWnd* pParent, UINT uwFlags );

	static BOOLEAN					CreateInstance( YInterfaceId, void ** );

protected:

	RColor			m_crColor;

private:

	uLONG				m_nRefCount;
};

#endif //_COLORDIALOGINTERFACEIMP_H_
