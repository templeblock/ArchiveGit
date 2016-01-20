//****************************************************************************
//
// File Name:   WinColorDialogInterfaceImp.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Implements the RColorDialogInterface.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//					  500 Redwood Blvd.
//					  Novato, CA 94948
//					  (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ColorDialogInterfaceImp.cpp              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "ColorDialogInterfaceImp.h"
#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "WinColorDlg.h"

//*****************************************************************************
//
// Function Name:  RColorDialogInterfaceImp
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RColorDialogInterfaceImp::RColorDialogInterfaceImp()
{
	RSolidColor crBlack( kBlack );

	m_crColor   = RColor( crBlack ); 
	m_nRefCount = 0L;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::AddRef( )
//
//  Description:		Increments reference count
//
//  Returns:			reference count
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG	RColorDialogInterfaceImp::AddRef( void )
{
	return ++m_nRefCount;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::Release( )
//
//  Description:		Decrements reference count, deleting the object
//                   when it reaches 0;
//
//  Returns:			reference count
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG	RColorDialogInterfaceImp::Release( void )
{
	if (--m_nRefCount == 0L)
		delete this;

	return m_nRefCount;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::QueryInterface( )
//
//  Description:		if the interface is supported, creates/assigns 
//                   the interface specified by the interface id into 
//                   ppvObject; 
//
//  Returns:			TRUE if successful; otherwise FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RColorDialogInterfaceImp::QueryInterface( YInterfaceId id, void ** ppvObject )
{
	*ppvObject = NULL;

	if (kUnknownInterfaceId == id)
		*ppvObject = (RIUnknown *) this;

	else if (kColorDialogInterfaceId == id)
		*ppvObject = (RColorDialogInterface *) this;

	else
		return FALSE;

	AddRef();

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::DoModal( )
//
//  Description:		Executes the dialog modally.
//
//  Returns:			An int value that specifies the value of the 
//                   nResult parameter that was passed to the 
//                   CDialog::EndDialog member function
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RColorDialogInterfaceImp::DoModal( CWnd* pParent, UINT uwFlags )
{
	BOOLEAN fShowTransparent = uwFlags & kShowTransparent;

	if (uwFlags & kAllowGradients)
	{
		RWinColorPaletteDlgEx dlg( pParent, fShowTransparent );
		dlg.SetColor( m_crColor );

		if (IDOK == dlg.DoModal())
		{
			m_crColor = dlg.SelectedColor() ;
			return IDOK;
		}
	}
	else
	{
		RWinColorPaletteDlg dlg( pParent, fShowTransparent );
		dlg.SetColor( m_crColor );
		int nResult = dlg.DoModal();

		if (IDOK == nResult)
		{
			m_crColor = dlg.SelectedColor() ;
			return IDOK;
		}
		else if (CONTROL_COLOR_MORE == nResult)
		{
			CColorDialog dlg( (YPlatformColor) m_crColor.GetSolidColor() );

			if (IDOK == dlg.DoModal())
			{
				m_crColor = RSolidColor( dlg.GetColor() );
				return IDOK;
			}
		}
	}

	return IDCANCEL;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::SelectedColor( )
//
//  Description:		Accessor to selected color.
//
//  Returns:			The selected color
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RColor& RColorDialogInterfaceImp::SelectedColor() const
{   
	return m_crColor;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::SelectedColor( )
//
//  Description:		Accessor to selected color with the specified 
//                   bounding rect.
//
//  Returns:			The selected color
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RColor& RColorDialogInterfaceImp::SelectedColor( const RIntRect& rcBounds )
{
	if (m_crColor.GetFillMethod() == RColor::kGradient)
		m_crColor.SetBoundingRect( rcBounds );

	return m_crColor;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::IsSelectedColorSolid( )
//
//  Description:		Determines if the currently selected color
//                   is solid.
//
//  Returns:			TRUE it the color is solid; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RColorDialogInterfaceImp::IsSelectedColorSolid() const
{
	return BOOLEAN( m_crColor.GetFillMethod() == RColor::kSolid );
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::IsSelectedColorSolid( )
//
//  Description:		Determines the solid color of the currently 
//                   selected color.  Note: this only works for
//                   solid and gradient colors.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RSolidColor RColorDialogInterfaceImp::SelectedSolidColor() const
{
	return m_crColor.GetSolidColor();
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::SetColor( )
//
//  Description:		Sets the current color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RColorDialogInterfaceImp::SetColor( const RColor& crColor )
{   
	m_crColor = crColor ;
}

// ****************************************************************************
//
//  Function Name:	RColorDialogInterfaceImp::CreateInterface( )
//
//  Description:		Instantiates an object of this type
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
BOOLEAN RColorDialogInterfaceImp::CreateInstance( YInterfaceId id, void** ppInterface )
{
	RColorDialogInterfaceImp* pObject = new RColorDialogInterfaceImp();

	if (pObject && !pObject->QueryInterface( id, ppInterface ))
	{
		delete pObject;
		return FALSE;
	}

	return TRUE;
}
