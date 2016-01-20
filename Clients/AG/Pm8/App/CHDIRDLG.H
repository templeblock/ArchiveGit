/*
// $Workfile: CHDIRDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CHDIRDLG.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:02   Fred
// Initial revision.
// 
//    Rev 1.2   30 Aug 1996 19:46:42   Fred
// Some directory validation done only for WIN16
// 
//    Rev 1.1   30 Aug 1996 13:18:10   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:27:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:08   FRED
// Initial revision.
// 
//    Rev 2.4   23 Jan 1996 12:32:12   JAY
//  
// 
//    Rev 2.3   07 Aug 1995 17:12:38   FRED
// Publishing Suite Install
// 
//    Rev 2.2   09 Jun 1995 07:43:32   FRED
// Sound Manager changes
// 
//    Rev 2.1   08 Feb 1995 13:38:58   JAY
// Reverted. New series.
// 
//    Rev 1.1   17 Jun 1994 16:59:22   FRED
// Adds sound buttons.
// 
// 
//    Rev 1.0   15 Jun 1994 19:00:12   FRED
// Initial revision.
*/ 

#ifndef _CHDIRDLG_H_
#define _CHDIRDLG_H_

/*
// ID's for important controls. These ID's must be used in the template.
*/

#define	IDC_CHOOSE_DIRECTORY_NAME		(1200)
#define	IDC_CHOOSE_DIRECTORY_BUFFER	(1088)		// same as in FileOpen common dialog

/////////////////////////////////////////////////////////////////////////////
// CChooseDirectoryDialog dialog

class CChooseDirectoryDialog : public CFileDialog
{
public:
	/*
	// Input flags for member functions.
	*/
	
	enum
	{
		OPTION_no_validate				=	0x0001,		// do not validate file name in any way
		OPTION_no_exists_check			=	0x0002,		// do not check for existance
		OPTION_no_create					=	0x0004,		// do not attempt to create
		OPTION_no_ask_to_create			=	0x0008,		// do not ask user before attempting create
		OPTION_no_writable_check		=	0x0010,		// do not check if writable
		OPTION_quiet						=	0x0020,		// do not ask any questions or report any errors
		OPTION_expand						=	0x0040		// return result in expanded form
	};
	
	/*
	// Return flags for CheckDirectory().
	*/
	
	enum
	{
		DIRECTORY_exists					=	0x0001,		// the directory exists
		DIRECTORY_writable				=	0x0002,		// the directory is writable
		DIRECTORY_is_file					=	0x0004		// the directory is actually a file
	};
	
// Construction
public:
	CChooseDirectoryDialog(int IDD, CWnd* pParent = NULL);
	
public:
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);
	virtual BOOL OnFileNameOK();
	virtual BOOL GetDirectory(WORD wFlags = 0);
	virtual BOOL ValidateDirectory(const CString &csDirectory, WORD wFlags);
	virtual WORD CheckDirectory(const CString &csDirectory, WORD wFlags);
	static int DirectoryMessageBox(CWnd *pcwParent, const CString &csDirectory, int nFormatID, int nTitleID, UINT nType, WORD wFlags = 0, int nDefaultResult = IDOK);
	virtual void FreeAllocatedData(void);
	virtual BOOL DirectoryNameIsValid(const CString &csDirectory);
	
// Dialog Data
	//{{AFX_DATA(CChooseDirectoryDialog)
	//}}AFX_DATA

public:	
	CString	m_csDirectory;
	
protected:
	int m_nIDD;
	WORD m_wFlags;
	BOOL m_fSuccess;
	BOOL m_fFirstChangeNotification;
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CChooseDirectoryDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
};

#endif
                 