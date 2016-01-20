/*
// $Workfile: PMAODLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1996 by Mindscape.
// All Rights Reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMAODLG.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     2/11/98 11:15a Dennis
// Removed GetRegistryString() and now use GetRegistryString() located in
// util.*
// 
//    Rev 1.0   14 Aug 1997 15:23:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:08   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 14:33:52   johno
// 
//    Rev 1.0   11 Jun 1997 16:35:18   johno
// Initial revision.
// 
//    Rev 1.0   16 Dec 1996 16:01:20   Fred
// Initial revision.
*/ 

#ifndef _PMTWDLG_H_
#define _PMTWDLG_H_

#include "resource.h"

// Hook function for extra validation by client.
typedef BOOL (*ChooseDirectoryValidationHook)(DWORD dwData, LPCSTR pszDirectory, HWND hParent);

/////////////////////////////////////////////////////////////////////////////
// CAddonInstallDlg dialog

class CAddonInstallDlg : public CDialog
{
// Construction
public:
   CAddonInstallDlg(int nFindResult, const CString& csPmDirectory, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAddonInstallDlg)
	enum { IDD = IDD_ADDONINST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddonInstallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAddonInstallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSetfocusDirectory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString& GetPmDirectory(void)
		{ return m_csPmDirectory; }
	static CString s_csPmDirectory;

protected:
	int            m_nFindResult;
	CString        m_csPmDirectory;

	static BOOL ValidateChooseDirectory(DWORD dwData, LPCSTR pszDirectory, HWND hParent);
	void UpdateDirectoryString(void);
	BOOL UpdateInterface(void);
	void EnableControl(UINT nId, BOOL fEnable);
/////////////////////////////////////////////////////////////////////////////
public:
	enum
	{
		// The order of these is significant. They are listed
		// in ascending order of how well they match a proper
		// installation of PrintMaster Gold 3.0 which can be
		// upgraded with TWAIN support. This allows the most
		// relevant error message to be shown.
		//
		// Note: These values are used to index an array of
		// dialog template id's in PMTWDLG.CPP.

		FIND_NotFound = 0,
		FIND_Before400,
		FIND_WrongBitness16,
		FIND_WrongBitness32,
		FIND_After400,
		FIND_Found,
	};

   static int 
   FindPrintMasterDirectory(void);
   
   static int 
   CheckPrintMasterDirectory(LPCSTR pszPath);
   
   static BOOL 
   GetMsregUsrIniFileName(CString& csFileName);
#ifdef WIN32
   static CString 
   GetAppPathKey(void);
#endif
#endif
};

#ifndef _CHDIRDLG_H_
#define _CHDIRDLG_H_

/*
// ID's for important controls. These ID's must be used in the template.
*/

#define	IDC_CHOOSE_DIRECTORY_NAME		(1200)
#define	IDC_CHOOSE_DIRECTORY_BUFFER	(1088)		// same as in FileOpen common dialog

// Hook function for extra validation by client.
typedef BOOL (*ChooseDirectoryValidationHook)(DWORD dwData, LPCSTR pszDirectory, HWND hParent);

/////////////////////////////////////////////////////////////////////////////
// CAddOnChooseDirectoryDialog dialog

class CAddOnChooseDirectoryDialog : public CFileDialog
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
	CAddOnChooseDirectoryDialog(int IDD, CWnd* pParent = NULL);
	
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
	//{{AFX_DATA(CAddOnChooseDirectoryDialog)
	//}}AFX_DATA

public:	
	CString	m_csDirectory;
	ChooseDirectoryValidationHook m_pClientValidation;
	DWORD m_dwClientValidation;
	
protected:
	int m_nIDD;
	WORD m_wFlags;
	BOOL m_fSuccess;
	BOOL m_fFirstChangeNotification;
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAddOnChooseDirectoryDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
