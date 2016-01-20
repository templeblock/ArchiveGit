//****************************************************************************
//
// File Name:		HyperlinkDialog.cpp
//
// Project:			Renaissance
//
// Author:			Richard Grenfell
//
// Description:	Definition of RHyperlinkDialog.
//
// Portability:	Windows Specific
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile:: $
//   $Author:: $
//     $Date:: $
// $Revision:: $
//
//****************************************************************************

#include	"FrameworkIncludes.h"

#include "HyperlinkDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RHyperlinkDialog::RHyperlinkDialog(const RURL& URL, CWnd* pParent /*=NULL*/)
	: CDialog(RHyperlinkDialog::IDD, pParent),
	m_URL(URL),
	m_bInEmailEditBox(FALSE),
	m_bInURLEditBox(FALSE),
	m_bBrowseForSoundFiles(FALSE)
{
	//{{AFX_DATA_INIT(RHyperlinkDialog)
	//}}AFX_DATA_INIT
}


void RHyperlinkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHyperlinkDialog)
	DDX_Control(pDX, IDC_AH_LINK_STATIC_TEXT, m_strStatic_Text);
	DDX_Control(pDX, IDC_AH_FILE_NAME, m_ctlFileName);
	DDX_Control(pDX, IDC_AH_URL, m_ctlEditBox);
	DDX_Control(pDX, IDC_AH_BROWSE, m_ctlBrowse);
	//}}AFX_DATA_MAP
	// Some reason ClassWizard won't do this
	DDX_Radio(pDX, IDC_AH_WEB_LINK, m_nLink);
}


BEGIN_MESSAGE_MAP(RHyperlinkDialog, CDialog)
	//{{AFX_MSG_MAP(RHyperlinkDialog)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_AH_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_AH_EMAIL_LINK, OnEmailLink)
	ON_BN_CLICKED(IDC_AH_FILE_LINK, OnFileLink)
	ON_BN_CLICKED(IDC_AH_WEB_LINK, OnWebLink)
	ON_BN_CLICKED(IDC_AH_REMOVE_LINK, OnRemoveLink)
	ON_BN_CLICKED(IDC_AH_SOUNDFILE_LINK, OnSoundfileLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void RHyperlinkDialog::OnHelp() 
{
    AfxGetApp()->WinHelp(0x00020000 + IDD_FOLDER_DIALOG);
}

//-----------------------------------------------------------------------------
//	When this button is selected, Bring up Open Dialog
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnBrowse() 
{
	CFileDialog	FileDlg(TRUE);
	char *pcArray = NULL;	// Must be external to block to avoid going out of scope.
									// The lpstrFilter pointer only keeps an address, not a copy.

	if (m_bBrowseForSoundFiles)
	{
		CString strSoundFilters( (LPCTSTR)STRING_SOUND_FILE_FILTERS );
		pcArray = CreateTranslatedFilter( strSoundFilters );
		FileDlg.m_ofn.lpstrFilter = pcArray;
	}

	if (FileDlg.DoModal() == IDOK)
	{
		m_ctlFileName.SetWindowText( (LPCTSTR) FileDlg.GetPathName());	
	}

	if (pcArray != NULL)
	{
		FreeTranslatedFilter( pcArray );
	}
}

//-----------------------------------------------------------------------------
//	When this button is selected, it brings up the edit box, browse button and
// updates the message text.
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnFileLink() 
{
	m_ctlEditBox.ShowWindow(FALSE);
	m_ctlBrowse.ShowWindow(TRUE);
	m_ctlFileName.ShowWindow(TRUE);
	CString strMsg;

	strMsg.LoadString( STRING_FILE_LINK );

	m_strStatic_Text.SetWindowText( (LPCTSTR) strMsg);
	m_ctlFileName.SetFocus();

	m_bBrowseForSoundFiles = FALSE;
}


//-----------------------------------------------------------------------------
//	When this button is selected, it brings up the edit box, browse button and
// updates the message text.
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnSoundfileLink() 
{
	OnFileLink();
	m_bBrowseForSoundFiles = TRUE;
}


//-----------------------------------------------------------------------------
//	When this button is selected, show edit box & change message
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnEmailLink() 
{
	m_ctlEditBox.ShowWindow(TRUE);
	m_ctlBrowse.ShowWindow(FALSE);
	m_ctlFileName.ShowWindow(FALSE);
	CString strMsg;

	if (m_bInURLEditBox == TRUE)
	{		
		m_ctlEditBox.GetWindowText(m_strURL);
		m_ctlEditBox.SetWindowText((LPCTSTR) m_strMail);
		m_bInURLEditBox = FALSE;
	}
	m_bInEmailEditBox = TRUE;
	
	strMsg.LoadString( STRING_EMAIL_LINK );

	m_strStatic_Text.SetWindowText( (LPCTSTR) strMsg);	
	m_ctlEditBox.SetFocus();
}

//-----------------------------------------------------------------------------
//	When this button is selected, show edit box & change message
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnWebLink() 
{
	m_ctlEditBox.ShowWindow(TRUE);
	m_ctlBrowse.ShowWindow(FALSE);
	m_ctlFileName.ShowWindow(FALSE);
	CString strMsg;

	if (m_bInEmailEditBox == TRUE)
	{		
		m_ctlEditBox.GetWindowText(m_strMail);
		m_ctlEditBox.SetWindowText((LPCTSTR)m_strURL );
		m_bInEmailEditBox = FALSE;
	}
	m_bInURLEditBox = TRUE;
		
	strMsg.LoadString( STRING_WEB_LINK );

	m_strStatic_Text.SetWindowText( (LPCTSTR) strMsg);	
	m_ctlEditBox.SetFocus();
}

//-----------------------------------------------------------------------------
//	Initialize data
//-----------------------------------------------------------------------------
BOOL RHyperlinkDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Based on the URL type load in the strings
	switch (m_URL.GetProtocol())
	{
		case PROTOCOL_HTTP:
		case PROTOCOL_FTP:
		case PROTOCOL_GOPHER:
			m_strURL = (CString)m_URL.GetAddress();
			m_ctlEditBox.SetWindowText( (LPCTSTR) m_strURL);
			m_nLink = HLD_WEB_TYPE;
			OnWebLink();
			break;

		case PROTOCOL_MAIL:
			m_strMail = (CString)m_URL.GetAddress();
			m_ctlEditBox.SetWindowText( (LPCTSTR) m_strMail);
			m_nLink = HLD_EMAIL_TYPE;
			OnEmailLink();
			break;

		case PROTOCOL_FILE:
			m_nLink = HLD_FILE_TYPE;
			m_ctlFileName.SetWindowText( (LPCTSTR) m_URL.GetAddress());
			OnFileLink();
			break;

		case PROTOCOL_SOUNDFILE:
			m_nLink = HLD_SOUND_TYPE;
			m_ctlFileName.SetWindowText( (LPCTSTR) m_URL.GetAddress());
			OnSoundfileLink();
			break;

		default:
			m_nLink = HLD_WEB_TYPE;
			OnWebLink();
	}
		   
	// send member variables to dialog
	UpdateData (FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//-----------------------------------------------------------------------------
// Create the appropriate URL
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnOK() 
{
	// Get member variables from dialog
	UpdateData (TRUE);
	switch(m_nLink)
	{
		case HLD_WEB_TYPE:
			m_ctlEditBox.GetWindowText(m_strURL);
			if (!VerifyURL(m_strURL))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILENAME_CHARS_INVALID_URL);
				return;
			}
			m_URL.SetAddress(m_strURL);
			m_URL.SetProtocol(PROTOCOL_HTTP);
			break;

		case HLD_EMAIL_TYPE:
			m_ctlEditBox.GetWindowText(m_strMail);
			if (!VerifyEMail(m_strMail))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILENAME_CHARS_INVALID_EMAIL);
				return;
			}
			if (m_strMail.Find("@") == -1)
			{
				RAlert alert;
				YAlertValues rResponse = alert.QueryUser(STRING_ERROR_EMAIL_ADDRESS_INVALID);
				if (rResponse == kAlertYes)
					return;
			}
			m_URL.SetAddress(m_strMail);
			m_URL.SetProtocol(PROTOCOL_MAIL);
			m_strMail = (CString)m_URL.GetURL();
			break;

		case HLD_FILE_TYPE:
		{
			CString strFileText;
			m_ctlFileName.GetWindowText(strFileText);
			if (!VerifyFile(strFileText))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILENAME_CHARS_INVALID);
				return;
			}
			if (!VerifyFileExists(strFileText))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILE_SPECIFIED_NOT_FOUND);
				return;
			}
			m_URL.SetAddress(strFileText);
			m_URL.SetProtocol(PROTOCOL_FILE);
		}
			break;
		case HLD_SOUND_TYPE:
		{
			CString strFileText;
			m_ctlFileName.GetWindowText(strFileText);
			if (!VerifyFile(strFileText))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILENAME_CHARS_INVALID);
				return;
			}
			if (!VerifyFileExists(strFileText))
			{
				RAlert alert;
				alert.AlertUser(STRING_ERROR_FILE_SPECIFIED_NOT_FOUND);
				return;
			}
			m_URL.SetAddress(strFileText);
			m_URL.SetProtocol(PROTOCOL_SOUNDFILE);
		}
			break;
	}
	CDialog::OnOK();
}


//-----------------------------------------------------------------------------
// Remove the current link
//-----------------------------------------------------------------------------
void RHyperlinkDialog::OnRemoveLink() 
{
	m_URL.SetAddress("");
	m_URL.SetProtocol(PROTOCOL_NONE);
	CDialog::OnOK();
}



// Ensures that a URL string contains only legal characters.
//
// Returns : TRUE for a valid string, FALSE if an invalid character is found.
bool RHyperlinkDialog::VerifyURL( const CString &strURL ) const
{
	// Assume the string is valid.
	bool bValid = TRUE;

	// Walk the string looking for invalid characters.
	int nStringLength = strURL.GetLength();
	for (int nIndex = 0; nIndex < nStringLength; nIndex++)
	{
		char cChar = strURL[nIndex];
		if (cChar == '*' || cChar == '"' || cChar == '<' || cChar == '>' || cChar == '|')
		{
			bValid = FALSE;
			break;	// We only need to find one invalid character.
		}
	}

	return bValid;
}


// Ensures that an e-mail address contains only legal characters.
//
// Returns : TRUE for a valid string, FALSE if an invalid character is found.
bool RHyperlinkDialog::VerifyEMail( const CString &strEMail ) const
{
	// Assume the string is valid.
	bool bValid = TRUE;

	// Walk the string looking for invalid characters.
	int nStringLength = strEMail.GetLength();
	for (int nIndex = 0; nIndex < nStringLength; nIndex++)
	{
		char cChar = strEMail[nIndex];
		if (cChar == '\\' || cChar == '/' || cChar == ':' ||
			 cChar == '*' || cChar == '?' || cChar == '"' ||
			 cChar == '<' || cChar == '>' || cChar == '|')
		{
			bValid = FALSE;
			break;	// We only need to find one invalid character.
		}
	}

	return bValid;
}


// Ensures that a file string contains only legal characters.
//
// Returns : TRUE for a valid string, FALSE if an invalid character is found.
bool RHyperlinkDialog::VerifyFile( const CString &strFile ) const
{
	// Assume the string is valid.
	bool bValid = TRUE;

	// Walk the string looking for invalid characters.
	int nStringLength = strFile.GetLength();
	for (int nIndex = 0; nIndex < nStringLength; nIndex++)
	{
		char cChar = strFile[nIndex];
		if (cChar == '*' || cChar == '?' || cChar == '"' ||
			 cChar == '<' || cChar == '>' || cChar == '|')
		{
			bValid = FALSE;
			break;	// We only need to find one invalid character.
		}
	}

	return bValid;
}


// Checks to see that the file can be accessed on the user's system.
//
// Returns : TRUE if the file can be found on the system.  FALSE if not present.
bool RHyperlinkDialog::VerifyFileExists(const CString &strFile) const
{
	CFileStatus tmpStatus;
	BOOL bFileFound = CFile::GetStatus( strFile, tmpStatus );

	return (bFileFound != FALSE);
}


// Translates the given string which contains a tokenized filter list into a
// version that the CFileDialog can use.  This means the replacement of the
// token seperators ('|') with Null characters ('\0') which the file dialog
// expects.
//
// Returns : an allocated character array that must be deleted by the caller.
//				 Correct deletion can be performed with the FreeTranslatedFilter
//				 Routine.
//
char *RHyperlinkDialog::CreateTranslatedFilter( const CString &strFilter ) const
{
	// Determine the length of the incomming string.
	int nSize = strFilter.GetLength();
	// Create an array to hold the 
	char *pcFilter = new char[nSize];
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		char cChar = strFilter[nIndex];
		if (cChar == '|')
		{
			pcFilter[nIndex] = '\0';
		}
		else
		{
			pcFilter[nIndex] = cChar;
		}
	}

	return pcFilter;
}


// Frees the memory allocated with the CreateTranslatedFilter routine.
//
void RHyperlinkDialog::FreeTranslatedFilter( char *pcArray ) const
{
	delete [] pcArray;
}
