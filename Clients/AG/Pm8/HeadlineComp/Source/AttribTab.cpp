//****************************************************************************
//
// File Name:		AttribTab.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Manages CAttribTab class which is used for the Attrib 
//						Property Page
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/AttribTab.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME


#ifndef	WIN
	#error	This file must be compilied only on Windows
#endif	//	WIN

#include "EditHeadlineDlg.h"

#define kEnableUnderline FALSE

/////////////////////////////////////////////////////////////////////////////
// CAttribTab property page

IMPLEMENT_DYNCREATE(CAttribTab, CPropertyPage)

CAttribTab::CAttribTab() : CPropertyPage(CAttribTab::IDD)
{
	//{{AFX_DATA_INIT(CAttribTab)
	m_uwScale1 = 1;
	m_uwScale2 = 1;
	m_uwScale3 = 1;
	//}}AFX_DATA_INIT
	m_fBoldChecked		 = TRUE;
	m_fItalicChecked	 = TRUE;
	m_fUnderlineChecked  = TRUE;
	m_fKerningChecked    = TRUE;
	m_fLeadingChecked	 = TRUE;
	m_flCompDegree		 = 100.0F;
	m_eSelJust			 = kLeftJust;
	m_eSelComp			 = kRegular;

}
CAttribTab::CAttribTab(AttribDataStruct* pAttribData ) : 
	CPropertyPage(CAttribTab::IDD),
	m_pCurrentRadioButton( NULL ),
	m_btnJustLeft( &m_pCurrentRadioButton ),
	m_btnJustCenter( &m_pCurrentRadioButton ),
	m_btnJustRight( &m_pCurrentRadioButton ),
	m_btnJustFull( &m_pCurrentRadioButton )
{
	SetData( pAttribData );
	m_flCompDegree = 100.0f;
}

void CAttribTab::SetData( AttribDataStruct* pAttribData )
{
	m_uwScale1				= static_cast<sBYTE>(pAttribData->uwScale1);
	m_uwScale2				= static_cast<sBYTE>(pAttribData->uwScale2);
	m_uwScale3				= static_cast<sBYTE>(pAttribData->uwScale3);
	m_fBoldChecked			= pAttribData->fBoldChecked;
	m_fItalicChecked		= pAttribData->fItalicChecked;
	m_fUnderlineChecked	= pAttribData->fUnderlineChecked;
	m_fKerningChecked		= pAttribData->fKerningChecked;
	m_fLeadingChecked		= pAttribData->fLeadingChecked;
//	m_flCompDegree			= ::Round( (pAttribData->flCompDegree * 100) );
	m_eSelJust				= pAttribData->eSelJust;
	m_eSelComp				= pAttribData->eSelComp;
	m_sFontName				= pAttribData->sFontName.operator CString( );
	m_eSelEscape			= pAttribData->eEscapement;

}

CAttribTab::~CAttribTab()
{
}

void CAttribTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttribTab)
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_UNDERLINE, m_btnStyleUnderline);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_ITALIC, m_btnStyleItalic);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_BOLD, m_btnStyleBold);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_RIGHTJUST, m_btnJustRight);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_LEFTJUST, m_btnJustLeft);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_FULLJUST, m_btnJustFull);
	DDX_Control(pDX, CONTROL_BUTTON_ATTRIBUTES_TAB_CENTERJUST, m_btnJustCenter);
	DDX_Control(pDX, CONTROL_COMBO_ATTRIBUTES_TAB_FONT, m_ctrlFont);
	DDX_Control(pDX, CONTROL_COMBO_ATTRIBUTES_TAB_ESCAPEMENT, m_ctrlEscapement);
	DDX_Text(pDX, CONTROL_EDIT_ATTRIBUTES_TAB_SCALE1, m_uwScale1);
	DDV_MinMaxByte(pDX, m_uwScale1, 1, 255);
	DDX_Text(pDX, CONTROL_EDIT_ATTRIBUTES_TAB_SCALE2, m_uwScale2);
	DDV_MinMaxByte(pDX, m_uwScale2, 1, 255);
	DDX_Text(pDX, CONTROL_EDIT_ATTRIBUTES_TAB_SCALE3, m_uwScale3);
	DDV_MinMaxByte(pDX, m_uwScale3, 1, 255);
	DDX_CBString( pDX, CONTROL_COMBO_ATTRIBUTES_TAB_FONT, m_sFontName );
	DDX_Check(pDX, CONTROL_CHECKBOX_ATTRIBUTES_TAB_KERNING, m_fKerningChecked);
	DDX_Check(pDX, CONTROL_CHECKBOX_ATTRIBUTES_TAB_LEADING, m_fLeadingChecked);
	//}}AFX_DATA_MAP

	int	nConvertComp = int(m_eSelComp);
	DDX_Radio( pDX, CONTROL_RADIO_ATTRIBUTES_TAB_REGULAR, nConvertComp );
	m_eSelComp = ETextCompensation(nConvertComp);
	
	//
	//Get Escapement enum index
	int	nConvertEscape = int(m_eSelEscape);
	DDX_CBIndex( pDX, CONTROL_COMBO_ATTRIBUTES_TAB_ESCAPEMENT, nConvertEscape );
	// Temporary fix because enumeration is all outta whack with the combobox
	if( nConvertEscape == 0 )
	{
		m_eSelEscape = kLeft2RightTop2Bottom;
	}
	else
	{
		m_eSelEscape = kTop2BottomLeft2Right;
	}

}


BEGIN_MESSAGE_MAP(CAttribTab, CPropertyPage)
	//{{AFX_MSG_MAP(CAttribTab)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_EDIT_DEGREE, OnBtnEditDegree)
	ON_BN_CLICKED(CONTROL_CHECKBOX_ATTRIBUTES_TAB_KERNING, OnChkKerning)
	ON_BN_CLICKED(CONTROL_CHECKBOX_ATTRIBUTES_TAB_LEADING, OnChkLeading)
	ON_BN_CLICKED(CONTROL_RADIO_ATTRIBUTES_TAB_LOOSE, OnRdLoose)
	ON_BN_CLICKED(CONTROL_RADIO_ATTRIBUTES_TAB_REGULAR, OnRdRegular)
	ON_BN_CLICKED(CONTROL_RADIO_ATTRIBUTES_TAB_TIGHT, OnRdTight)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_CENTERJUST, OnBtnJustCenter)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_FULLJUST, OnBtnJustFull)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_LEFTJUST, OnBtnJustLeft)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_RIGHTJUST, OnBtnJustRight)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_BOLD, OnBtnStyleBold)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_ITALIC, OnBtnStyleItalic)
	ON_BN_CLICKED(CONTROL_BUTTON_ATTRIBUTES_TAB_UNDERLINE, OnBtnStyleUnderline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAttribTab message handlers


BOOL CAttribTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	BeginWaitCursor();
	
	//
	//Get device for font list...
	CDC* pDC = GetDC();
	//
	//Define CArray of CStrings to hold font names...
	StringArray	arFonts; 
	//
	//Load arFonts with device's font names..
	GetFontNames( pDC, &arFonts);
	SortFontNames( &arFonts );
	//
	//Must release DC after call to GetDC()..
	ReleaseDC( pDC ); 
	//
	//Initialize Font ComboBox
	int InitStorage( int nItems, UINT nBytes );
	int nNumFonts = arFonts.GetSize();
	m_ctrlFont.InitStorage( nNumFonts, kFontNameLen );
	int nFontSel = kDefaultFontSel;
	for( int nIdx = 0; nIdx < nNumFonts; nIdx++ )
	{
		m_ctrlFont.InsertString( nIdx, (LPCTSTR)arFonts[nIdx] );
		if ( m_sFontName == arFonts[nIdx] )
		{
			nFontSel = nIdx;
		}

	}
	//
	//Set Combo Box Defaults
	m_ctrlFont.SetCurSel( nFontSel );
	m_ctrlEscapement.SetCurSel( m_eSelEscape );

	LoadAttribButtonBmps();
	UpdateData( FALSE );

	InitializePushlikeButtons();

#if !kEnableUnderline
	m_btnStyleUnderline.ShowWindow( SW_HIDE );
#endif

	EndWaitCursor();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************
// Function Name: AttribTab::InitializePushlikeButtons
//
// Description:   Depress the appropriate simulated "push-like" buttons.
//
// Returns:		  Nothing
//
// Exceptions:    None
//
//*****************************************************************************
void CAttribTab::InitializePushlikeButtons()
{
	switch( m_eSelJust )
	{
	case kLeftJust:
		m_btnJustLeft.SimulateRadioClick();
		break;

	case kCenterJust:
		m_btnJustCenter.SimulateRadioClick();
		break;

	case kRightJust:
		m_btnJustRight.SimulateRadioClick();
		break;

	case kFullJust:
		m_btnJustFull.SimulateRadioClick();
		break;

	default:
		ASSERT( FALSE );
		break;
	}

	//
	//Set Style buttons
	if ( m_fBoldChecked )
	{
		m_btnStyleBold.SimulateCheck();
	}
	if ( m_fItalicChecked )
	{
		m_btnStyleItalic.SimulateCheck();
	}
#if kEnableUnderline
	if ( m_fUnderlineChecked )
	{
		m_btnStyleUnderline.SimulateCheck();
	}
#endif

}

////////////////////////////////////////////////////////////////////////////////////
//Function: LoadAttribButtonBmps
//
//Description: Loads the Style and Justification Button bitmaps
//
//Paramaters:  VOID
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CAttribTab::LoadAttribButtonBmps()
{
	m_btnStyleBold.LoadBitmaps( kBoldBtnBmpU, kBoldBtnBmpD ); 
	m_btnStyleItalic.LoadBitmaps( kItalicBtnBmpU, kItalicBtnBmpD );
	m_btnStyleUnderline.LoadBitmaps( kUnderlineBtnBmpU, kUnderlineBtnBmpD );
	m_btnJustLeft.LoadBitmaps( kLeftJustBtnBmpU, kLeftJustBtnBmpD );
	m_btnJustCenter.LoadBitmaps( kCenterJustBtnBmpU, kCenterJustBtnBmpD );
	m_btnJustRight.LoadBitmaps( kRightJustBtnBmpU, kRightJustBtnBmpD );
	m_btnJustFull.LoadBitmaps( kFullJustBtnBmpU, kFullJustBtnBmpD );


#if 0
/***************************************************************************
	int nButtonBmpIdx = 0;
	m_arrayButtonBmps.SetSize( kMaxNumAttribBmpBtns );

 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kBoldBtnBmp ) != 0)
	{
		m_btnStyleBold.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kItalicBtnBmp ) != 0)
	{
		m_btnStyleItalic.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kUnderlineBtnBmp ) != 0)
	{
		m_btnStyleUnderline.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}

 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kLeftJustBtnBmp ) != 0)
	{
		m_btnJustLeft.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kCenterJustBtnBmp ) != 0)
	{
		m_btnJustCenter.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kRightJustBtnBmp ) != 0)
	{
		m_btnJustRight.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
 	if ( m_arrayButtonBmps[nButtonBmpIdx].LoadBitmap( kFullJustBtnBmp ) != 0)
	{
		m_btnJustFull.SetBitmap((HBITMAP)m_arrayButtonBmps[nButtonBmpIdx++]);
	}
****************************************************************************/
#endif
}

void CAttribTab::OnBtnEditDegree() 
{
	//
	//Draw Edit Degree dialog...
	dlgEditComp.m_flCompDegree = m_flCompDegree;
	dlgEditComp.DoModal();
	//
	//If the user edited the Compensation Degree, enable the Apply button
	if ( m_flCompDegree != dlgEditComp.m_flCompDegree )
	{
		m_flCompDegree = dlgEditComp.m_flCompDegree;
		SetModified( TRUE );
	}
}

void CAttribTab::OnChkKerning() 
{
	m_fKerningChecked = !m_fKerningChecked;
	SetModified( TRUE );
	
}

void CAttribTab::OnChkLeading() 
{
	m_fLeadingChecked = !m_fLeadingChecked;
	SetModified( TRUE );
	
}

void CAttribTab::OnRdLoose() 
{
	if ( m_eSelComp != kLoose )
	{
		m_eSelComp = kLoose;
		SetModified( TRUE );
	}
	
}

void CAttribTab::OnRdRegular() 
{
	if ( m_eSelComp != kRegular )
	{
		m_eSelComp = kRegular;
		SetModified( TRUE );
	
	}
}

void CAttribTab::OnRdTight() 
{
	if ( m_eSelComp != kTight )
	{
		m_eSelComp = kTight;
		SetModified( TRUE );
	}
	
}

void CAttribTab::OnBtnJustCenter() 
{
	if ( m_eSelJust != kCenterJust )
	{
		m_eSelJust = kCenterJust;
		SetModified( TRUE );
	}
	m_btnJustCenter.SimulateRadioClick();

}

void CAttribTab::OnBtnJustFull() 
{
	if ( m_eSelJust != kFullJust )
	{
		m_eSelJust = kFullJust;
		SetModified( TRUE );
	}
	m_btnJustFull.SimulateRadioClick();

	
}

void CAttribTab::OnBtnJustLeft() 
{
	if ( m_eSelJust != kLeftJust )
	{
		m_eSelJust = kLeftJust;
		SetModified( TRUE );
	}
	m_btnJustLeft.SimulateRadioClick();
	
}

void CAttribTab::OnBtnJustRight() 
{
	if ( m_eSelJust != kRightJust )
	{
		m_eSelJust = kRightJust;
		SetModified( TRUE );
	}
	m_btnJustRight.SimulateRadioClick();
	
}

void CAttribTab::OnBtnStyleBold() 
{
	m_fBoldChecked = !m_fBoldChecked;
	m_btnStyleBold.SimulateCheck();
	SetModified( TRUE );
}

void CAttribTab::OnBtnStyleItalic() 
{
	m_fItalicChecked = !m_fItalicChecked;
	m_btnStyleItalic.SimulateCheck();
	SetModified( TRUE );
}

void CAttribTab::OnBtnStyleUnderline() 
{
	m_fUnderlineChecked = !m_fUnderlineChecked;
	m_btnStyleUnderline.SimulateCheck();
	SetModified( TRUE );
}

void CAttribTab::ApplyFollowPathModifications( BOOLEAN /*fApplying*/, int /*m_nSelShapeId*/ )
{
	/*if ( fApplying )
	{
		if ( m_eSelJust == kFull )
		{
			OnBtnJustCenter();
		}
		m_btnJustFull.ShowWindow( SW_HIDE );
		if ( m_nSelShapeId == kWarpPath1 || m_nSelShapeId == kWarpPath2)
		{
			m_uwScale1 = 2;
			m_uwScale2 = 1;
			m_uwScale3 = 1;
		}
		else
		{
			ASSERT( m_nSelShapeId == kWarpPath3 || m_nSelShapeId == kWarpPath4 );
			m_uwScale1 = 4;
			m_uwScale2 = 1;
			m_uwScale3 = 1;
		}

	}
	else
	{
		m_btnJustFull.ShowWindow( SW_SHOW );
		m_uwScale1 = 1;
		m_uwScale2 = 1;
		m_uwScale3 = 1;

	}
	*/

}

void CAttribTab::FillData( AttribDataStruct* pAttribData)
{
	pAttribData->sFontName				= RMBCString(m_sFontName);
	pAttribData->eEscapement			= m_eSelEscape;
	pAttribData->fBoldChecked			= static_cast<BOOLEAN>( m_fBoldChecked );
	pAttribData->fItalicChecked		= static_cast<BOOLEAN>( m_fItalicChecked );
	pAttribData->fUnderlineChecked	= static_cast<BOOLEAN>( m_fUnderlineChecked );
	pAttribData->eSelJust				= m_eSelJust;
	pAttribData->eSelComp				= m_eSelComp;
	pAttribData->flCompDegree			= m_flCompDegree / 100.0f ;
	pAttribData->fKerningChecked		= static_cast<BOOLEAN>( m_fKerningChecked );
	pAttribData->fLeadingChecked		= static_cast<BOOLEAN>( m_fLeadingChecked );
	pAttribData->uwScale1				= m_uwScale1;
	pAttribData->uwScale2				= m_uwScale2;
	pAttribData->uwScale3				= m_uwScale3;

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
void CAttribTab::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;

}

BOOL CAttribTab::OnApply()
{

	m_pParentPropSheet->UpdateHeadlinePreview();
	return CPropertyPage::OnApply();
}

BOOL CAttribTab::OnKillActive() 
{
	UpdateData( TRUE );
	m_pParentPropSheet->UpdateTabData( REditHeadlineDlg::kAttribTab );
	return CPropertyPage::OnKillActive();
}
