//****************************************************************************
//
// File Name:  HeadlineSheet.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Defines CHeadlineSheet Class used to manage the Headline Property
//				Sheet.
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineSheet.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef	WIN
	#error	This file can only be compilied on Windows
#endif

#include "HeadlineIncludes.h"

ASSERTNAME

#include "EditHeadlineDlg.h"
#include "HeadlineDocument.h"
#include "HeadlineApplication.h"
#include "ComponentAttributes.h"
#include "DcDrawingSurface.h"
#include "EditHeadlineDlg.h"
#include "ComponentTypes.h"
#include "ComponentView.h"

#define kSheetInflateAmt	 55
#define kMaxNumChars        1000
#define kMaxNumLines        32
#define kXShrinkAmt			 8 

/////////////////////////////////////////////////////////////////////////////
// CHeadlineSheet

IMPLEMENT_DYNAMIC(CHeadlineSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CHeadlineSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CHeadlineSheet)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_EN_UPDATE( kTextCtrlId, OnTextEditCtrlUpdate )
END_MESSAGE_MAP()

//*****************************************************************************
//
// Function Name:  CHeadlineSheet::CHeadlineSheet
//
// Description:    Constructor which accepts default Headline text.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
CHeadlineSheet::CHeadlineSheet( CString* psText ) : m_pPreviewHeadline( NULL )
{
	m_sHeadlineText = *psText;
}

CHeadlineSheet::~CHeadlineSheet()
{
// Delete the preview headine
delete m_pPreviewHeadline;
}

void CHeadlineSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeTab)
	DDX_Text(pDX, kTextCtrlId, m_sHeadlineText);
	//}}AFX_DATA_MAP
}								

/////////////////////////////////////////////////////////////////////////////
// CHeadlineSheet message handlers

//*****************************************************************************
//
// Function Name: CHeadlineSheet::OnTextEditCtrlUpdate
//
// Description:  Function handler for the property sheet's text control's ON_EN_UPDATE
//		         Defined to prevent greater than kMaxNumLines from being entered.
//
// Returns:		 VOID
//
// Exceptions:	 None
//
//*****************************************************************************
afx_msg void CHeadlineSheet::OnTextEditCtrlUpdate()
{
	int	nLines	= m_ctrlText.GetLineCount();
	if( nLines > kMaxNumLines && m_ctrlText.CanUndo() )
	{
		::MessageBeep( MB_ICONASTERISK );
		m_ctrlText.Undo();
	}
	m_ctrlText.EmptyUndoBuffer();
	//
	//Need to enable the Apply button when text changes... using the Attrib tab is as good as any.
	m_pParentPropSheet->m_AttribPage.SetModified(TRUE);

	UpdateData( TRUE );
}

int CHeadlineSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//
	//Inflate Property sheet so there is room for controls above the 
	//property pages
	CRect sheetRect;
	GetWindowRect( &sheetRect );
	sheetRect.InflateRect( 0 , kSheetInflateAmt );
	MoveWindow( &sheetRect, FALSE );
	
	return 0;
}


BOOL CHeadlineSheet::OnInitDialog() 
{
	//
	//Move all child windows to bottom of inflated property sheet.
	MoveChildWindows( this, 0, (kSheetInflateAmt*2), FALSE );
	BOOL fFocusSet = CPropertySheet::OnInitDialog();
	//
	//Draw edit control at top of Propert Sheet
	CRect	TextCtrlRect;
	GetClientRect( &TextCtrlRect );
	GetClientRect( &m_cPrevRect );
	TextCtrlRect.bottom	= kSheetInflateAmt + kXShrinkAmt;
	TextCtrlRect.DeflateRect( kXShrinkAmt, (kSheetInflateAmt/6) );
	TextCtrlRect.top+= kXShrinkAmt;

	m_cPrevRect.top		= TextCtrlRect.bottom;
	m_cPrevRect.bottom	= TextCtrlRect.bottom + kSheetInflateAmt;
	m_cPrevRect.left		= TextCtrlRect.left;
	m_cPrevRect.right		= TextCtrlRect.right;

	m_ctrlText.Create( ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
					   ES_WANTRETURN | WS_CHILD | WS_VISIBLE | WS_BORDER , 
					   TextCtrlRect, this, kTextCtrlId );

	 
	m_ctrlText.LimitText( kMaxNumChars );

	int nRightVal = TextCtrlRect.right;
	int nBottomVal = TextCtrlRect.top - 1;
	CRect CaptionRect(0, 0, nRightVal, nBottomVal );
	m_ctrlTextCaption.Create(  kHeadlineCaption,
							   WS_CHILD | WS_VISIBLE , 
							   CaptionRect, this, kTextCaptionCtrlId );


	SetTitle( kHeadlineDlgTitle, PSH_PROPTITLE );

	UpdateData( FALSE );
	//
	//Give the edit control the default focus.
	m_ctrlText.SetFocus();

	// Create the preview headline
	m_pPreviewHeadline = new RHeadlineDocument( NULL, RComponentAttributes( ), kHeadlineComponent, FALSE );

	// Create and add a headline view
	RRealRect previewRect;
	GetPreviewRect( &previewRect );
	m_pPreviewHeadline->AddRView( m_pPreviewHeadline->CreateView( previewRect, NULL ) );

	// Update the preview headline
	m_pParentPropSheet->UpdateHeadlinePreview( );

	return fFocusSet;
}

////////////////////////////////////////////////////////////////////////////////////
//Function: SetPointerToOwner
//
//Description: Stores a pointer to the REditHeadlineDlg class that called it so I can
//					call methods in REditHeadlineDlg
//
//Paramaters: pParentPropSheet - pointer to REditHeadlineDlg that instantiated this 
//											instance.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CHeadlineSheet::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;
}

//*****************************************************************************
//
// Function Name:  CHeadlineSheet::FillData
//
// Description:    Loads a pointer to a CString with the current Headline text.
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void CHeadlineSheet::FillData( CString* pGetText )
{
	*pGetText = m_sHeadlineText;
}

//*****************************************************************************
//
// Function Name:  CHeadlineSheet::GetPreviewRect
//
// Description:    Loads the headline preview rect.
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void CHeadlineSheet::GetPreviewRect( RRealRect* rPrevRect )
{
	rPrevRect->m_Top		= m_cPrevRect.top	;
	rPrevRect->m_Bottom	= m_cPrevRect.bottom;
	rPrevRect->m_Left		= m_cPrevRect.left	;
	rPrevRect->m_Right	= m_cPrevRect.right	;
	::ScreenUnitsToLogicalUnits( *rPrevRect );
}

//*****************************************************************************
//
// Function Name:  CHeadlineSheet::OnPaint
//
// Description:    WM_PAINT handler
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
//
void CHeadlineSheet::OnPaint( )
	{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint( GetSafeHwnd( ), &ps );

	// Put it in a DcDrawingSurface
	RDcDrawingSurface drawingSurface;
	drawingSurface.Initialize( hdc );

	// Create a transform
	R2dTransform transform;

	// Scale to the device DPI
	RRealSize deviceDPI = drawingSurface.GetDPI( );
	transform.PreScale( (YFloatType)deviceDPI.m_dx / kSystemDPI, (YFloatType)deviceDPI.m_dy / kSystemDPI );

	// Get the preview rectangle
	RRealRect previewRect;
	GetPreviewRect( &previewRect );

	// Offset the transform by the top left corner of the preview rect
	transform.PreTranslate( previewRect.m_Left, previewRect.m_Top );

	// Render the preview headline
	m_pPreviewHeadline->Render( drawingSurface, transform, RRealRect( previewRect.WidthHeight( ) ), previewRect.WidthHeight( ) );

	drawingSurface.DetachDCs( );

	::EndPaint( GetSafeHwnd( ), &ps );
	}

//*****************************************************************************
//
// Function Name:  CHeadlineSheet::UpdatePreviewHeadline
//
// Description:    Updates the preview headline when the data changes
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
//
void CHeadlineSheet::UpdatePreviewHeadline( HeadlineDataStruct* pData )
	{
	m_pPreviewHeadline->SetHeadlineData( pData );
	InvalidateRect(m_cPrevRect);
	}
