// ****************************************************************************
//
//  File Name:			SpellCheckDialog.cpp
//
//  Project:			Renaissance Application
//
//  Author:				Mike Heydlauf
//
//  Description:		Definition of the RSpellCheckDialog class
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
//  $Logfile:: /PM8/Framework/Source/SpellCheckDialog.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "SpellCheckDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "SpellChecker.h"
#include "CorrectSpell.h"
#include "DialogUtilities.h"
#include "ResourceManager.h"


//
// REVIEW- MWH  -the following rect is a temporary solution only.  I can't yet dynamically calculate
//					  the rect for the context control accurately.
const CRect kContextRect( 265,20,475,100 );


#ifndef	_WINDOWS
	#error	This file can only be compilied on Windows
#endif

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::RSpellCheckDialog
//
// Description:	ctor
//
// Returns:			nothing
//
// Exceptions:		void
//
//*****************************************************************************************************
RSpellCheckDialog::RSpellCheckDialog( RSpellChecker* pSpellChecker, CWnd* pParent /*=NULL*/)	: 
	CDialog(RSpellCheckDialog::IDD, pParent),
	m_pSpellChecker( pSpellChecker )		
{

	//{{AFX_DATA_INIT(RSpellCheckDialog)
	m_rMisspelledWord = "";
	m_rContext			= "";
	m_rReplaceWith		= "";
	//}}AFX_DATA_INIT
}


//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::DoDataExchange
//
// Description:	Data exchange
//
// Returns:			void
//
// Exceptions:		none
//
//*****************************************************************************************************
void RSpellCheckDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RSpellCheckDialog)
	CString cMisspelledWord( (LPCSZ)m_rMisspelledWord );
	DDX_Text( pDX, CONTROL_EDIT_SPELL_NOT_FOUND, cMisspelledWord );
	m_rMisspelledWord = cMisspelledWord;

//	CString rContext( (LPCSZ)m_rContext );
//	DDX_Text( pDX, CONTROL_EDIT_SPELL_CONTEXT, rContext );
//	m_rContext = rContext;
	
//	DDX_Control( pDX, CONTROL_EDIT_SPELL_CONTEXT, m_cContextCtrl );

	CString rReplaceWith( (LPCSZ)m_rReplaceWith );
	DDX_Text( pDX, CONTROL_EDIT_SPELL_REPLACE_WITH, rReplaceWith );
	m_rReplaceWith = rReplaceWith;

	DDX_Control( pDX, CONTROL_LISTBOX_SPELL_ALTERNATIVES, m_cAltListBox );
	DDX_Control( pDX, CONTROL_EDIT_SPELL_REPLACE_WITH, m_cEditReplace );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RSpellCheckDialog, CDialog)
	//{{AFX_MSG_MAP(RSpellCheckDialog)
	ON_BN_CLICKED(CONTROL_BUTTON_SPELL_ADD_TO_DICTIONARY, OnButtonSpellAddToDictionary)
	ON_BN_CLICKED(CONTROL_EDIT_SPELL_IGNORE, OnEditSpellIgnore)
	ON_BN_CLICKED(CONTROL_EDIT_SPELL_REPLACE, OnEditSpellReplace)
	ON_LBN_DBLCLK(CONTROL_LISTBOX_SPELL_ALTERNATIVES, OnDblclkListboxSpellAlternatives)
	ON_LBN_SELCHANGE(CONTROL_LISTBOX_SPELL_ALTERNATIVES, OnSelchangeListboxSpellAlternatives)
//	ON_BN_CLICKED(ID_HELP, OnHelp)
	//ON_BN_CLICKED(CONTROL_EDIT_SPELL_IGNORE_ALL, OnEditSpellIgnoreAll)
	//ON_BN_CLICKED(CONTROL_EDIT_SPELL_REPLACE_ALL, OnEditSpellReplaceAll)
	//ON_BN_CLICKED(CONTROL_EDIT_SPELL_SUGGEST, OnEditSpellSuggest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnInitDialog
//
// Description:	Initialize dialog.
//
// Returns:			TRUE
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOL RSpellCheckDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	// I need to use a CRichEditCtrl for the context preview so I can hilite the misspelled word.
	// So what I'm doing is taking the CEdit control specified in the dialog's resource, getting 
	// its dimensions, deleting it, then dynamically replacing it with a CRichEditCtrl I just created...
	CEdit* pContextEditCtrl;
	CRect  cContextCtrlRect;
	//
	// Get handle to edit control that exists in the resource...
	pContextEditCtrl = (CEdit*)GetDlgItem( CONTROL_EDIT_SPELL_CONTEXT ) ;
	//
	// Get its dimensions...
	pContextEditCtrl->GetWindowRect( cContextCtrlRect );
	CRect cDlgRect;
	GetWindowRect( cDlgRect );
	cContextCtrlRect.left	-= cDlgRect.left;
	cContextCtrlRect.top		-= cDlgRect.top + ::GetSystemMetrics( SM_CYMENU );
	cContextCtrlRect.right	-= cDlgRect.left;
	cContextCtrlRect.bottom -= cDlgRect.top + ::GetSystemMetrics( SM_CYMENU );
	//
	// Destroy it...
	pContextEditCtrl->DestroyWindow();
	//
	// Create new CRichEditCtrl to replace it...
	m_cContextCtrl.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_NOHIDESEL | ES_READONLY,  
								  /*kContextRect*/cContextCtrlRect, this, CONTROL_EDIT_SPELL_CONTEXT );

	//m_cContextCtrl.HideSelection( FALSE, TRUE );

	m_rMisspelledWord = m_pSpellChecker->GetMisspelledWord();
	ShowMisspelledWord();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::ShowMisspelledWord
//
// Description:	Initialize all display fields with appropriate data for current misspelled word
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::ShowMisspelledWord()
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		LoadAlternatives();
		if ( m_cAltListBox.GetCount() < 1 )
		{
			m_rReplaceWith = ::GetResourceManager().GetResourceString( STRING_MISC_NOSUGGESTIONS );//"No Suggestions"
		}

		int nWordStartPos = 0;
		m_rContext = "";
		m_pSpellChecker->GetContext( m_rContext, nWordStartPos );
		FormatStringForEditCtrl( m_rContext, nWordStartPos );

		m_cContextCtrl.SetWindowText( (LPCSZ)m_rContext );
		UpdateData( FALSE );
		m_cContextCtrl.SetSel( nWordStartPos, (nWordStartPos + m_rMisspelledWord.GetStringLength()) );
		m_cEditReplace.SetFocus();
		m_cEditReplace.SetSel(0,-1);
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
}

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::LoadAlternatives
//
// Description:	Load the list of alternative spellings for the current word in the alternatives list box
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::LoadAlternatives()
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		m_cAltListBox.ResetContent();
		//
		// The first alternate is the suggested spelling..
		m_pSpellChecker->GetAlternateSpelling( m_rReplaceWith );
		//
		// Fill the list box with the other suggested alternates..
		RMBCString rCurrentAlternate;
		while( m_pSpellChecker->GetAlternateSpelling( rCurrentAlternate ) )
		{
			if ( !rCurrentAlternate.IsEmpty())
			{
				m_cAltListBox.AddString( (LPCSZ)rCurrentAlternate  );
			}
		}
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
}

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::ProcessNextMisspelledWord
//
// Description:	Get the next misspelled word (if there is one) and display it.  If there are no
//						more misspelled words, close the dialog.
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::ProcessNextMisspelledWord()
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		if( m_pSpellChecker->FindMisspelledWord() )
		{
			m_rMisspelledWord = m_pSpellChecker->GetMisspelledWord();
			ShowMisspelledWord();
		}
		else
		{
			//
			//No more misspelled words, close the dialog.
			CDialog::OnCancel();
		}
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
}
//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnOK
//
// Description:	User hit ok, save settings
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnOK()
{
	m_pSpellChecker->Done( );
	CDialog::OnOK();
}


//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnButtonSpellAddToDictionary
//
// Description:	Add misspelled word to personal dictionary
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnButtonSpellAddToDictionary() 
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		m_pSpellChecker->AddWordToPersonalDict( m_rMisspelledWord );
		ProcessNextMisspelledWord();
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
}

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnEditSpellIgnore
//
// Description:	User hit Ignore, go to next word.
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnEditSpellIgnore() 
{
	// TODO: Add your control notification handler code here
	ProcessNextMisspelledWord();
	
}

/*//For future reference, maybe.

void RSpellCheckDialog::OnEditSpellIgnoreAll() 
{
	// TODO: Add your control notification handler code here
	
}
void RSpellCheckDialog::OnEditSpellReplaceAll() 
{
	// TODO: Add your control notification handler code here
	
}
void RSpellCheckDialog::OnEditSpellSuggest() 
{
	// TODO: Add your control notification handler code here
	
}
*/

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnEditSpellReplace
//
// Description:	User clicked replace, replace with word in 'Replace with' edit ctrl
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnEditSpellReplace() 
{
	UpdateData( TRUE );
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		if ( m_cEditReplace.GetModify() )
		{
			//
			//User manually entered an alternative, see if it is spelled correctly. If not, warn them..
			//
			//REVIEW -MDH (MWH) -Mike, is it okay that the dialog refers to RCorrectSpell and not exclusively
			//							RSpellChecker.
			if ( ::GetCorrectSpell().CheckSpelling( m_rReplaceWith ) )
			{
				m_pSpellChecker->Replace( m_rReplaceWith );
				ProcessNextMisspelledWord();
			}
			else
			{
				//
				//User entered word was not found in dictionaries, warn them..
				RAlert rAlert;
				CString cUserMsg;
				cUserMsg.Format( STRING_MISC_SPELLREPLACEWORDNOTFOUND, (LPCSZ)m_rReplaceWith ); //"Warning: The word \"%s\" was not found in our dictionary. Continue?"
				if ( rAlert.QueryUser( (LPCSTR)cUserMsg ) == kAlertYes )
				{
					m_pSpellChecker->Replace( m_rReplaceWith );
					ProcessNextMisspelledWord();
				}
			}
		}
		else
		{
			if( m_rReplaceWith != ::GetResourceManager().GetResourceString( STRING_MISC_NOSUGGESTIONS ) )
			{
				m_pSpellChecker->Replace( m_rReplaceWith );
			}
			ProcessNextMisspelledWord();
		}
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
}



//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnDblclkListboxSpellAlternatives
//
// Description:	User double-clicked in list box, replace with word clicked on
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnDblclkListboxSpellAlternatives() 
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		UpdateData( TRUE );
		CString cReplaceWith;
		m_cAltListBox.GetText( m_cAltListBox.GetCurSel(), cReplaceWith );
		m_rReplaceWith = cReplaceWith;
		UpdateData( FALSE );
		OnEditSpellReplace();
		m_cEditReplace.SetModify( FALSE );
	}
	catch( ... )
	{	
		EndDialog(IDCANCEL);
		throw;
	}
	
}

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnSelchangeListboxSpellAlternatives
//
// Description:	User changed selection in the list box, update the 'Replace with' field
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellCheckDialog::OnSelchangeListboxSpellAlternatives() 
{
	//
	// Need to make sure if any exceptions are thrown the dialog cleans itself up..
	try
	{
		CString cReplaceWith;
		m_cAltListBox.GetText( m_cAltListBox.GetCurSel(), cReplaceWith );
		m_rReplaceWith = cReplaceWith;
		UpdateData( FALSE );
		m_cEditReplace.SetModify( FALSE );
	}
	catch( ... )
	{
		EndDialog(IDCANCEL);
		throw;
	}
	
}

//*****************************************************************************************************
// Function Name:	RSpellCheckDialog::OnHelp
//
// Description:	User hit Help, show help info
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
//void RSpellCheckDialog::OnHelp() 
//{
	// TODO: Add your control notification handler code here
	
//}

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*******************************                                 *************************************
//*******************************                                 *************************************
//*******************************          RContextEditCtrl       *************************************
//*******************************                                 *************************************
//*******************************                                 *************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

//*****************************************************************************************************
// Function Name:	RContextEditCtrl::RContextEditCtrl
//
// Description:	ctor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RContextEditCtrl::RContextEditCtrl()
{
	NULL;
}

//*****************************************************************************************************
// Function Name:	RContextEditCtrl::~RContextEditCtrl
//
// Description:	dtor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RContextEditCtrl::~RContextEditCtrl()
{
	NULL;
}


BEGIN_MESSAGE_MAP(RContextEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(RContextEditCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RContextEditCtrl message handlers


//*****************************************************************************************************
// Function Name:	RContextEditCtrl::OnEraseBkgnd
//
// Description:	Override to make dynamically created control's background same color as dialog.
//
// Returns:			result of base class OnEraseBkgnd
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOL RContextEditCtrl::OnEraseBkgnd(CDC* pDC) 
{
	LOGBRUSH logBrush;
	//
	// Sending WM_CTLCOLORDLG is the only way I can get the color I need for the control's background.  GetSysColor doesn't work here.
	CWnd *pParent = GetParent();
	CDC *pParentDC = GetParent()->GetDC();
	CBrush::FromHandle( (HBRUSH)::SendMessage( pParent->m_hWnd , WM_CTLCOLORDLG, (WPARAM)pParentDC->m_hDC, (LPARAM) pParent->m_hWnd ) )->GetLogBrush( &logBrush );
	pParent->ReleaseDC(pParentDC);
	if( logBrush.lbStyle == BS_SOLID )
	{
		SetBackgroundColor( FALSE, logBrush.lbColor );
	}
	else
	{
		//
		// Since the first param is TRUE, the second param is ignored in SetBackgroundColor...
		SetBackgroundColor( TRUE, NULL );
	}
	return CRichEditCtrl::OnEraseBkgnd(pDC);
}

//*****************************************************************************************************
// Function Name:	RContextEditCtrl::OnSetFocus
//
// Description:	Override to prevent control from getting focus (control is a display only)
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
void RContextEditCtrl::OnSetFocus(CWnd* /*pOldWnd*/) 
{
	//
	// Set focus to parent..
	GetParent()->SetFocus();
}


//*****************************************************************************************************
// Function Name:	RContextEditCtrl::OnLButtonDblClk
//
// Description:	Override to prevent hilited selection from changing
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
void RContextEditCtrl::OnLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/) 
{
	//CRichEditCtrl::OnLButtonDblClk(nFlags, point);
}

//*****************************************************************************************************
// Function Name:	RContextEditCtrl::OnLButtonDblClk
//
// Description:	Override to prevent cursor from changing
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOL RContextEditCtrl::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CRichEditCtrl::OnSetCursor(pWnd, nHitTest, message);
}

//*****************************************************************************************************
// Function Name:	RContextEditCtrl::OnLButtonDblClk
//
// Description:	Override to prevent hilited selection from changing
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
void RContextEditCtrl::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	//CRichEditCtrl::OnLButtonDown(nFlags, point);
}
