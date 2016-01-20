//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/OpenDlgs.h 1     3/03/99 6:08p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/OpenDlgs.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 6     1/25/99 3:44p Rgrenfell
// Added support for playing an animated GIF in the preview window.
// 
// 5     6/24/98 3:59p Rlovejoy
// Made file name buffer 32K.
// 
// 4     6/24/98 2:36p Rlovejoy
// Added a large buffer to hold filenames.
// 
// 3     4/17/98 6:30p Hforman
// changed an ID name
// 
// 2     4/16/98 7:11p Hforman
// finishing up basic functionality - categories, etc.
// 
// 1     4/16/98 10:12a Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#include "pictprvw.h"
#include "UsrCtgry.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomOpenDlg dialog

#define GIF_REDRAW_TIMER 371	// A pseudo-random number - I picked one.

class CCustomOpenDlg : public CFileDialog
{
public:
	CCustomOpenDlg(BOOL fPreviewDocs,	// showing Docs (projects) or graphics?
						LPSTR szFilename,		// initial filename (if any)
						LPSTR szDir,			// initial directory
						LPCSTR filters,		// file type filters
						int IDTemplate,		// controls to be added to dialog
						FLAGS flags = NULL,	// additional flags of Open dialog
						CWnd* pParent = NULL);

	~CCustomOpenDlg();

// Dialog Data
	//{{AFX_DATA(CCustomOpenDlg)
	//}}AFX_DATA

// Implementation
protected:
	BOOL				m_fProjects;	// showing projects or graphics?
	CPicturePreview m_preview;
	CString			m_strCurPreviewFile;	// file currently being previewed

	virtual void	OnInitDone();
	virtual void	OnFileNameChange();
	virtual BOOL	OnFileNameOK();
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL	DestroyWindow();

	// Generated message map functions
	//{{AFX_MSG(CCustomOpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_nTimer;
	bool m_bUpdating;
};

/////////////////////////////////////////////////////////////////////////////
// CImportDlg dialog -- baseclass for Importing of Projects/Art

class CImportDlg : public CCustomOpenDlg
{
public:
	CImportDlg(BOOL fProjects, LPSTR szFilename, LPSTR szDir, LPCSTR filters, int IDTemplate, CWnd* pParent = NULL);
	~CImportDlg();

// Dialog Data
	//{{AFX_DATA(CImportDlg)
	CButton	m_btnNewCategory;
	CComboBox	m_CategoryCombo;
	CString	m_strCategory;
	//}}AFX_DATA

protected:
	CUserCategoryManager		m_CategoryManager;
	void				InitCategories(CPMWCollection::CollectionTypes type);
	char				m_szFileBuffer[32768];

	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewCategory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CImportPictureDlg dialog

class CImportPictureDlg : public CImportDlg
{
public:
	CImportPictureDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent = NULL);
	~CImportPictureDlg();

// Dialog Data
	//{{AFX_DATA(CImportPictureDlg)
	enum { IDD = IDD_IMPORT_PICT_OPTS };
	CButton	m_btnCopyLocal;
	BOOL	m_fCopyLocal;
	//}}AFX_DATA

protected:
	virtual void	OnFileNameChange();
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CImportPictureDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CImportProjectDlg dialog

class CImportProjectDlg : public CImportDlg
{
public:
	CImportProjectDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent = NULL);
	~CImportProjectDlg();

// Dialog Data
	//{{AFX_DATA(CImportProjectDlg)
	enum { IDD = IDD_IMPORT_PROJ_OPTS };
	//}}AFX_DATA

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CImportProjectDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COpenPictureDlg dialog

class CNoUpdateButton : public CButton
{
public:
	CNoUpdateButton() : CButton()
		{ m_bNoUpdate = TRUE; }

	afx_msg BOOL OnEraseBkgnd( CDC *pDC )
		{
			if (m_bNoUpdate)
			{
				RedrawWindow(NULL, NULL, RDW_NOERASE);
				return TRUE;
			}
			else
				return CButton::OnEraseBkgnd(pDC);
		}

	void SetNoUpdate( bool bNoUpdate )
		{ m_bNoUpdate = bNoUpdate; }

private:
	bool m_bNoUpdate;

	DECLARE_MESSAGE_MAP()
};

class COpenPictureDlg : public CCustomOpenDlg
{
public:
	COpenPictureDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent = NULL);
	~COpenPictureDlg();

// Dialog Data
	//{{AFX_DATA(COpenPictureDlg)
	enum { IDD = IDD_OPEN_PICT_OPTS };
	CNoUpdateButton	m_btnEmbedPicture;
	CNoUpdateButton	m_btnPreview;
	BOOL	m_fEmbedPicture;
	//}}AFX_DATA

protected:
	virtual void	OnFileNameChange();
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(COpenPictureDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COpenProjectDlg dialog

class COpenProjectDlg : public CCustomOpenDlg
{
public:
	COpenProjectDlg(LPSTR szFilename, LPSTR szDir,
						 LPCSTR filters, CWnd* pParent = NULL);
	~COpenProjectDlg();

// Dialog Data
	//{{AFX_DATA(COpenProjectDlg)
	enum { IDD = IDD_OPEN_PREVIEW_OPTS };
	//}}AFX_DATA

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(COpenProjectDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

