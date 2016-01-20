// ****************************************************************************
//
//  File Name:			ThesaurusDialog.cpp
//
//  Project:			Renaissance Application
//
//  Author:				Mike Heydlauf
//
//  Description:		Definition of the RThesaurusDialog class
//
//  Portability:		Windows
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ThesaurusDialog.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ThesaurusDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "IntelliFinder.h"
#include "ResourceManager.h"

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::RThesaurusDialog
//
// Description:	ctor
//
// Returns:			nothing
//
// Exceptions:		void
//
//*****************************************************************************************************
RThesaurusDialog::RThesaurusDialog( RMBCString& rLookupWord, CWnd* pParent /*=NULL*/)	: 
	CDialog(RThesaurusDialog::IDD, pParent),
	m_rLookupWord( rLookupWord )
{

	//{{AFX_DATA_INIT(RThesaurusDialog)
	//}}AFX_DATA_INIT
}


//*****************************************************************************************************
// Function Name:	RThesaurusDialog::DoDataExchange
//
// Description:	Data exchange
//
// Returns:			void
//
// Exceptions:		none
//
//*****************************************************************************************************
void RThesaurusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RThesaurusDialog)
	CString cLookupWord( (LPCSZ)m_rLookupWord );
	DDX_Text( pDX, CONTROL_EDIT_THESAURUS_SELECTED_WORD, cLookupWord );
	m_rLookupWord = cLookupWord;

	DDX_Control( pDX, CONTROL_LISTBOX_THESAURUS_SYNONYMS, m_cSynonymListBox );
	DDX_Control( pDX, CONTROL_LISTBOX_THESAURUS_DEFINITIONS, m_cDefinitionListBox );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RThesaurusDialog, CDialog)
	//{{AFX_MSG_MAP(RThesaurusDialog)
	ON_BN_CLICKED(CONTROL_EDIT_THESAURUS_REPLACE, OnEditThesaurusReplace)
	ON_BN_CLICKED(CONTROL_EDIT_THESAURUS_DEFINE, OnEditThesaurusDefine)
	ON_LBN_DBLCLK(CONTROL_LISTBOX_THESAURUS_SYNONYMS, OnDblclkListboxSpellSynonyms)
	ON_LBN_SELCHANGE(CONTROL_LISTBOX_THESAURUS_SYNONYMS, OnSelchangeListboxSynonyms)
//	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnInitDialog
//
// Description:	Initialize dialog.
//
// Returns:			TRUE
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOL RThesaurusDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	try
	{
		LookupWord();
	}
	catch( ... )
	{
		RAlert rAlert;
		rAlert.AlertUser( STRING_ERROR_IF_ERRACCESSDB );//"IntelliFinder: Error accessing Thesaurus database." );
		EndDialog(IDCANCEL);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//*****************************************************************************************************
// Function Name:	RThesaurusDialog::LookupWord
//
// Description:	Lookup word in 'Selected Word' field.  If found, show definition and synonyms.
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::LookupWord()
{
	if( !m_rLookupWord.IsEmpty() && ::GetIntelliFinder().LookupWord( m_rLookupWord ) )
	{
		LoadSynonyms();
		LoadDefinitions();
		UpdateData( FALSE );
	}
	else
	{
		m_rLookupWord = ::GetResourceManager().GetResourceString( STRING_MISC_NOSUGGESTIONS );
		UpdateData( FALSE );
		m_cSynonymListBox.ResetContent();
		m_cDefinitionListBox.ResetContent();
		GetDlgItem( CONTROL_EDIT_THESAURUS_SELECTED_WORD )->SetFocus();
	}
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::LoadSynonyms
//
// Description:	Load the list of synonyms
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::LoadSynonyms()
{
	RMBCString rSynonym;
	m_cSynonymListBox.ResetContent();
	int nHorizScrollLen = 0;
	while( ::GetIntelliFinder().GetNextSynonym( rSynonym ) )
	{
		m_cSynonymListBox.AddString( (LPCSZ)rSynonym );
		MaintainHorizScrollLenForListBox( nHorizScrollLen, m_cSynonymListBox, rSynonym );
	}
	//
	// Setup horizontal scroll bar..
	m_cSynonymListBox.SetHorizontalExtent( nHorizScrollLen );
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::MaintainHorizScrollLenForListBox
//
// Description:	Maintains the longest string length (in pixels) of a CListBox entry so the horizontal 
//						scroll length can be properly maintained.
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::MaintainHorizScrollLenForListBox( int& nCurrMaxLen, CListBox& cLBox, const RMBCString& rNewString )
{
	int nNewSynLength = 0;
	CDC* pLBoxDC = cLBox.GetDC();
	nNewSynLength = pLBoxDC->GetTextExtent( (LPCSZ)rNewString, rNewString.GetStringLength() ).cx;
	cLBox.ReleaseDC(pLBoxDC);
	if( nNewSynLength > nCurrMaxLen ) nCurrMaxLen = nNewSynLength;
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::LoadDefinitions
//
// Description:	Load the list of Definitions
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::LoadDefinitions()
{
	RMBCString rDefinition;
	m_cDefinitionListBox.ResetContent();
	int nHorizScrollLen = 0;
	while( ::GetIntelliFinder().GetNextDefinition( rDefinition ) )
	{
		m_cDefinitionListBox.AddString( (LPCSZ)rDefinition );
		MaintainHorizScrollLenForListBox( nHorizScrollLen, m_cDefinitionListBox, rDefinition );
	}
	//
	// Setup horizontal scroll bar..
	m_cDefinitionListBox.SetHorizontalExtent( nHorizScrollLen );
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnOK
//
// Description:	User hit ok, save settings
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnOK()
{
	EndDialog( IDOK );
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnCancel
//
// Description:	User hit Cancel, end dialog with a 0 so caller knows not to continue processing
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnCancel()
{
	EndDialog( 0 );
}



//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnEditThesaurusReplace
//
// Description:	User clicked replace.  Select and dismiss dialog
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnEditThesaurusReplace() 
{
	UpdateData( TRUE );
	if( m_rLookupWord == ::GetResourceManager().GetResourceString( STRING_MISC_NOSUGGESTIONS ) )
	{
		EndDialog(0);
	}
	else
	{
		EndDialog(IDOK);
	}
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnEditThesaurusDefine
//
// Description:	User clicked define.  Look up word in Selected word field
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnEditThesaurusDefine() 
{
	UpdateData( TRUE );
	LookupWord();
}


//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnDblclkListboxSpellSynonyms
//
// Description:	User double clicked new synonym from list box. Select and dismiss dialog
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnDblclkListboxSpellSynonyms() 
{
	CString cSelSyn;
	m_cSynonymListBox.GetText( m_cSynonymListBox.GetCurSel( ), cSelSyn );
	m_rLookupWord = RMBCString( cSelSyn );
	EndDialog( IDOK );
}

//*****************************************************************************************************
// Function Name:	RThesaurusDialog::OnSelchangeListboxSynonyms
//
// Description:	User picked new synonym from list box, update 'Replace With' field
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::OnSelchangeListboxSynonyms() 
{
	CString cSelSyn;
	m_cSynonymListBox.GetText( m_cSynonymListBox.GetCurSel( ), cSelSyn );
	m_rLookupWord = RMBCString( cSelSyn );
	if( !m_rLookupWord.IsEmpty() && ::GetIntelliFinder().LookupWord( m_rLookupWord ) )
	{
		LoadDefinitions();
	}
	UpdateData( FALSE );
}
//*****************************************************************************************************
// Function Name:	RThesaurusDialog::FillData
//
// Description:	Put the selected synonym in the RMBCString argument
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RThesaurusDialog::FillData( RMBCString& rString )
{
	rString = m_rLookupWord;
}

