//****************************************************************************
//
// File Name:		HyperlinkDialog.h
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

#ifndef _HYPERLINKDIALOG_H_
#define _HYPERLINKDIALOG_H_

#include "FrameworkResourceIDs.h"
#include "URL.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

// ****************************************************************************
//
//  Class Name:		RHyperlinkDialog
//
//  Description:		Dialog which allows the user to edit the URL for a selected
//							object.
//
// ****************************************************************************
//
class FrameworkLinkage RHyperlinkDialog : public CDialog
{
public:
// Construction
	RHyperlinkDialog(const RURL &URL, CWnd* pParent = NULL);

	// Provide access to the resulting URL data.
	const RURL &GetURL() { return m_URL; }

public:
	// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RHyperlinkDialog)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Data members

	// Link radio button index.
	int	m_nLink;
	// The URL settings.
	RURL	m_URL;

	//{{AFX_DATA(RHyperlinkDialog)
	enum { IDD = IDD_ADDEDIT_HYPERLINK };
	CStatic	m_strStatic_Text;
	CEdit		m_ctlFileName;
	CEdit		m_ctlEditBox;
	CButton	m_ctlBrowse;
	//}}AFX_DATA

protected:
	// Operations

	// Generated message map functions
	//{{AFX_MSG(RHyperlinkDialog)
	afx_msg void OnHelp();
	afx_msg void OnBrowse();
	afx_msg void OnEmailLink();
	afx_msg void OnFileLink();
	afx_msg void OnWebLink();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRemoveLink();
	afx_msg void OnSoundfileLink();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	// String holding user's URL entry
	CString	m_strURL;

	// String holding user's mail entry
	CString	m_strMail;

	// Used to save the strings when going back & forth between Boxes
	bool	m_bInURLEditBox;
	bool	m_bInEmailEditBox;

	// Used to toggle between browsing for general files and browsing for sound files
	bool	m_bBrowseForSoundFiles;

	enum ELinkTypes
	{
		HLD_WEB_TYPE = 0,	// Web page radio position and type.
		HLD_EMAIL_TYPE,	// EMail radio position and type.
		HLD_FILE_TYPE,		// File link radio position and type.
		HLD_SOUND_TYPE		// Sound link radio position and type.
	};

	// Ensures that a URL string contains only legal characters.
	bool VerifyURL( const CString &strURL ) const;
	// Ensures that an e-mail address contains only legal characters.
	bool VerifyEMail( const CString &strEMail ) const;
	// Ensures that a file string contains only legal characters.
	bool VerifyFile( const CString &strFile ) const;
	// Checks to see that the file can be accessed on the user's system.
	bool VerifyFileExists(const CString &strFile) const;

	// Translates a filter resource string into a file dialog compatible version.
	char *CreateTranslatedFilter( const CString &strFilter ) const;
	// Releases the memory allocated by CreateTranslatedFilter.
	void FreeTranslatedFilter( char *pcArray ) const;
};

//********************************************************************************************************
//********************************************************************************************************
// Inlines
//********************************************************************************************************
//********************************************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _HYPERLINKDIALOG_H_