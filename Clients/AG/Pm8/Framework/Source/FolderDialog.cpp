//****************************************************************************
//
// File Name:		FolderDialog.cpp
//
// Project:			Renaissance framework
//
// Author:			Richard E. Grenfell
//
// Description:	Defines class RFolderDialog.  This dialog can be used for
//						directory selection.
//
// Portability:	Windows Specific
//
// Developed by:	Broderbund Software Inc.
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile: /PM8/Framework/Source/FolderDialog.cpp $
//   $Author: Gbeddow $
//     $Date: 3/03/99 6:17p $
// $Revision: 1 $
//
//****************************************************************************


#include	"FrameworkIncludes.h"

#include "FrameworkResourceIDs.h"
#include "FolderDialog.h"
#include "RFileFinder.h"
#include <dlgs.h> // for (MULTI)FILEOPENORD

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RFolderDialog dialog
static char BASED_CODE szFilter[] = "Folders (.*)|.*||";

//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::RFolderDialog()
//
// Description:	Constructor.
//
// Returns:			Nothing
//-----------------------------------------------------------------------------
RFolderDialog::RFolderDialog(CString& path, int nCurHTMLExt, CWnd* pParent /*=NULL*/)
	: CFileDialog(TRUE, NULL, NULL, 0, szFilter, pParent),
	m_bOK(FALSE)
{

	//{{AFX_DATA_INIT(RFolderDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nHTML = nCurHTMLExt;

	m_ofn.Flags          = m_ofn.Flags | OFN_ENABLETEMPLATE | OFN_HIDEREADONLY | OFN_NOCHANGEDIR
                                       | OFN_EXPLORER;
	m_ofn.hInstance      = AfxGetResourceHandle();
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FOLDER_DIALOG);

	// Determine initial directory, if MRU exists, use first file in MRU list
	// for directory, else use PSWeb default directory.

	if (path.IsEmpty())	
	{
		CString strPath;
		// specify default directory 
		RFileFinder ff;
		strPath = ff.AppDirectory();

		// Set the initial directory
		m_ofn.lpstrInitialDir = strPath ; 
	}
	else
	{
		m_ofn.lpstrInitialDir = path ; 	
	}

	m_nPageListIndex = 0;
}


//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::DoDataExchange()
//
// Description:	Initialize Dialog data.
//
// Returns:			Nothing
//-----------------------------------------------------------------------------
void RFolderDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RFolderDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Radio(pDX, IDC_FD_HTML, m_nHTML);
	DDX_Control(pDX, IDC_FD_PAGE_SIZE, m_ctlPageSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RFolderDialog, CFileDialog)
	//{{AFX_MSG_MAP(RFolderDialog)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_FD_FOLDER, OnFolder)
	ON_BN_CLICKED(IDC_FD_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RFolderDialog message handlers

//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::OnHelp()
//
// Description:	Performs context sensitive help.
//
// Returns:			Nothing
//-----------------------------------------------------------------------------
void RFolderDialog::OnHelp()
{
    AfxGetApp()->WinHelp(0x00020000 + IDD_FOLDER_DIALOG);
}

//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::OnInitDialog()
//
// Description:	Initialize Dialog
//
// Returns:			Always TRUE.
//-----------------------------------------------------------------------------
BOOL RFolderDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	
	// Hide Controls we don't want
	HideControl(1136);
	HideControl(1040);
	HideControl(1152);
	HideControl(1089);
	HideControl(1090);
	HideControl(1);		// Open
	HideControl(2);		// Cancel
	
	CString strDir;
	strDir.LoadString(STRING_SELECT_DIRECTORY);
	
	GetParent()->SetWindowText( (LPCTSTR)strDir );

	m_ctlPageSize.AddString( "100%" );
	m_ctlPageSize.AddString( "90%" );
	m_ctlPageSize.AddString( "80%" );
	m_ctlPageSize.AddString( "70%" );
	m_ctlPageSize.AddString( "60%" );
	m_ctlPageSize.AddString( "50%" );

	m_ctlPageSize.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::OnFolder()
//
// Description:	User pressed Select folder button - Close dialog
//
// Returns:			Nothing.
//-----------------------------------------------------------------------------
void RFolderDialog::OnFolder()
{
	// With some creative use of the Spy++ utility, you will
	// find that the listview is not actually ID = lst1 as
	// documented in some references, but is actually a child
	// of dlg item ID = lst2
	
	// WARNING! Although this is a non-intrusive customization,
	// it does rely on unpublished (but easily obtainable)
	// information. The Windows common file dialog box implementation
	// may be subject to change in future versions of the
	// operating systems, and may even be modified by updates of
	// future Microsoft applications. This code could break in such
	// a case. It is intended to be a demonstration of one way of
	// extending the standard functionality of the common dialog boxes.
	
	UpdateData( TRUE );	// Get the current data for dialog controls.

	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return;
	
	CString strDirectory = GetFolderPath();

	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
	if (wndLst1 == NULL)
		return;

	UINT nSelected = wndLst1->GetSelectedCount();
	// Nothing Selected. Use current directory
	if (!nSelected)
	{
		m_bOK = TRUE;
		m_PathString = strDirectory;
		GetParent()->SendMessage(WM_COMMAND, IDCANCEL);

		// Fetch the current selection in the page size control.
		m_nPageListIndex = m_ctlPageSize.GetCurSel();

		return;
	}

							// /\ 4.2: undocumented, but non-implementation
	CString strItemText;
	// Get the first Item selected
	int nItem = wndLst1->GetNextItem(-1,LVNI_SELECTED); 
	strItemText = wndLst1->GetItemText(nItem,0);
	// Check for My Computer directory	
	if (strDirectory.IsEmpty())
	{
		int Drive = strItemText.Find(':');
		if (Drive != -1)
		{
			strItemText = strItemText.Mid(Drive-1, 2);
			strItemText += _T("\\");
		}
	}
	else if (strDirectory.Right(1) == _T("\\"))
		strItemText = strDirectory + strItemText;
	else
		strItemText = strDirectory + _T("\\") + strItemText;

	m_bOK = TRUE;
	m_PathString = strItemText;
	GetParent()->SendMessage(WM_COMMAND, IDCANCEL);
}

//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::OnCancel()
//
// Description:	User pressed Cancel button - Send message to parent
//
// Returns:			Nothing.
//-----------------------------------------------------------------------------
void RFolderDialog::OnCancel()
{
	GetParent()->SendMessage(WM_COMMAND, IDCANCEL);
}


/*	@mfunc <c CBFolderDlg> method that provides the user selected page size as
			a percentage value.

	@rdesc	A floating point value containing the percentage page size selected.
*/
//-----------------------------------------------------------------------------
// Function Name:	RFolderDialog::GetPageSizePercentage()
//
// Description:	provides the user selected page size as a percentage value.
//
// Returns:			A floating point value containing the percentage page size
//						selected.
//-----------------------------------------------------------------------------
YFloatType RFolderDialog::GetPageSizePercentage()
{
	float fPercentMultiplier = (float)m_nPageListIndex;
	return 1.0 - (0.1 * fPercentMultiplier);
}
