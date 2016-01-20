//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/OpenDlgs.cpp 1     3/03/99 6:08p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/OpenDlgs.cpp $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 8     1/25/99 3:44p Rgrenfell
// Added support for playing an animated GIF in the preview window.
// 
// 7     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 6     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     7/22/98 12:28p Rlovejoy
// Make composite path from graphic filename.
// 
// 4     6/24/98 2:37p Rlovejoy
// Increased max number of files which can be opened.
// 
// 3     6/05/98 5:40p Hforman
// center window
// 
// 2     4/16/98 7:11p Hforman
// finishing up basic functionality - categories, etc.
// 
// 1     4/16/98 10:16a Hforman
// 
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "pmw.h"
#include "utils.h"
#include "util.h"
#include "grafrec.h"
#include "OpenDlgs.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CNoUpdateButton, CButton)
	//{{AFX_MSG_MAP(CNoUpdateButton)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomOpenDlg dialog -- baseclass for Custom Open Dialogs

CCustomOpenDlg::CCustomOpenDlg(BOOL fProjects,	// opening projects or graphics?
										 LPSTR szFilename, LPSTR szDir,
										 LPCSTR filters, int IDTemplate,
										 FLAGS flags/*=NULL*/, CWnd* pParent/*=NULL*/)

 : CFileDialog(TRUE, NULL, szFilename, flags, filters, pParent)
{
	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE |
						 OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);

	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDTemplate);
	m_ofn.lpstrInitialDir = szDir;

	m_fProjects = fProjects;

	m_nTimer = 0;	// Initialize to no timer.
	m_bUpdating = FALSE;

	//{{AFX_DATA_INIT(COpenDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCustomOpenDlg::~CCustomOpenDlg()
{
}

CCustomOpenDlg::DestroyWindow()
{
	// Need to kill the timer before the window is destroyed to avoid assertions.
	if (m_nTimer != 0)
	{
		KillTimer( m_nTimer );
		m_nTimer = 0;
	}
	return CFileDialog::DestroyWindow();
}

void CCustomOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomOpenDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCustomOpenDlg, CFileDialog)
	//{{AFX_MSG_MAP(CCustomOpenDlg)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDialog message handlers

BOOL CCustomOpenDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCustomOpenDlg::OnInitDone()
{
	// Initialize the picture preview
	m_preview.Initialize(this, IDC_PICTURE_PREVIEW, IDC_PICTURE_DESCRIPTION);
	GetParent()->CenterWindow();
}

BOOL CCustomOpenDlg::OnFileNameOK(void)
{
	UpdateData(TRUE); // Otherwise, it's not called
	return FALSE;	// which means OK!
}

// Select notification.
void CCustomOpenDlg::OnFileNameChange()
{
	// Kill the timer if it's running.
	if (m_nTimer != 0)
	{
		// There was a loop count and the animation is over, so stop the timer.
		KillTimer( m_nTimer );
		m_nTimer = 0;
	}

	// Preview the file selected.
	// If multiple selections, preview the last file selected
	// (which is the first filename in the list)
	CString filename = GetPathName();
	if (m_ofn.Flags & OFN_ALLOWMULTISELECT)
	{
		// if multiple filenames are selected, each name is in
		// quotes, after the path name, e.g.:
		// C:\mydir\"firstfile.bmp" "secondfile.bmp"
		int pos;
		if ((pos = filename.Find("\" \"")) != -1)
		{
			// we've got multiple names -- grab the first
			filename = filename.Left(pos);
			// remove the quotes at beginning of filename
			if ((pos = filename.Find('"')) != -1)
				filename = filename.Left(pos) + filename.Mid(pos+1);
		}
	}

	if (!filename.IsEmpty() && Util::FileExists(filename) &&
		 m_strCurPreviewFile.Compare(filename) != 0)
	{
		if (m_fProjects)
			m_preview.NewDocPreview(filename);
		else
		{
			m_preview.NewGraphicPreview(filename);

			// Perform special handling if the preview is an animated GIF.
			if (m_preview.IsAnimatedGIF())
			{
				// Start a timer to allow for recurring redraws.
				m_nTimer = SetTimer(GIF_REDRAW_TIMER, 10, NULL);	// GIF animations have delays defined in 100ths of seconds.
			}
		}
		m_strCurPreviewFile = filename;
	}
}

void CCustomOpenDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_PICTURE_PREVIEW)
	{
		if (m_preview.IsAnimatedGIF())
		{
			// Update the animation.
			m_bUpdating = TRUE;
			if ( m_preview.Update( lpDrawItemStruct ) == IP_ANIMATION_COMPLETE )
			{
				// There was a loop count and the animation is over, so stop the timer.
				KillTimer( m_nTimer );
				m_nTimer = 0;
			}
			m_bUpdating = FALSE;
		}
		else
		{
			m_preview.DrawPreview(lpDrawItemStruct);
		}
	}
	else
		CFileDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


// Check to see if the preview needs to draw a new frame of the animated GIF.
void CCustomOpenDlg::OnTimer(UINT nIDEvent)
{
	if (!m_bUpdating && m_preview.NeedsUpdate())
	{
		RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImportDlg dialog -- Baseclass for Importing of Projects/Art

CImportDlg::CImportDlg(BOOL fProjects,	// opening projects or graphics?
							  LPSTR szFilename, LPSTR szDir,
							  LPCSTR filters, int IDTemplate, CWnd* pParent/*=NULL*/)
	: CCustomOpenDlg(fProjects, szFilename, szDir, filters, IDTemplate, OFN_ALLOWMULTISELECT, pParent)
{
	//{{AFX_DATA_INIT(CImportDlg)
	m_strCategory = _T("");
	//}}AFX_DATA_INIT

	// Set up max buffer
	m_szFileBuffer[0] = '\0';
	m_ofn.lpstrFile = m_szFileBuffer;
	m_ofn.nMaxFile = sizeof(m_szFileBuffer);
}

CImportDlg::~CImportDlg()
{
}

void CImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CCustomOpenDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportDlg)
	DDX_Control(pDX, IDC_NEW_CATEGORY, m_btnNewCategory);
	DDX_Control(pDX, IDC_CATEGORY_COMBO, m_CategoryCombo);
	DDX_CBString(pDX, IDC_CATEGORY_COMBO, m_strCategory);
	DDV_MaxChars(pDX, m_strCategory, 40);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportDlg, CCustomOpenDlg)
	//{{AFX_MSG_MAP(CImportDlg)
	ON_BN_CLICKED(IDC_NEW_CATEGORY, OnNewCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CImportDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	SetControlText(1, "Import");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImportDlg::InitCategories(CPMWCollection::CollectionTypes type)
{
	// init the category manager
	m_CategoryManager.Initialize(type);

	// fill the category combo with current categories
	CStringArray categories;
	m_CategoryManager.GetUserCategories(categories);
	for (int i = 0; i < categories.GetSize(); i++)
		m_CategoryCombo.AddString(categories[i]);
	m_CategoryCombo.SetCurSel(0);	
	UpdateData(TRUE);
}

void CImportDlg::OnNewCategory() 
{
	CString strCat;
	if (m_CategoryManager.GetNewCategory(strCat))
	{
		int index = m_CategoryCombo.AddString(strCat);
		if (index != CB_ERR)
		{
			m_CategoryCombo.SetCurSel(index);
			UpdateData(TRUE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImportPictureDlg dialog -- Import Pictures dialog

CImportPictureDlg::CImportPictureDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent/*=NULL*/)
	: CImportDlg(FALSE, szFilename, szDir, filters, IDD, pParent)
{
	m_ofn.lpstrTitle = "Import Pictures";

	//{{AFX_DATA_INIT(CImportPictureDlg)
	m_fCopyLocal = FALSE;
	m_strCategory = _T("");
	//}}AFX_DATA_INIT
}

CImportPictureDlg::~CImportPictureDlg()
{
}

void CImportPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CImportDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportPictureDlg)
	DDX_Control(pDX, IDC_COPY_LOCAL, m_btnCopyLocal);
	DDX_Check(pDX, IDC_COPY_LOCAL, m_fCopyLocal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportPictureDlg, CImportDlg)
	//{{AFX_MSG_MAP(CImportPictureDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CImportPictureDlg::OnInitDialog()
{
	CImportDlg::OnInitDialog();

	// init user categories
	InitCategories(CPMWCollection::typeArt);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// Select notification.
void CImportPictureDlg::OnFileNameChange()
{
	CImportDlg::OnFileNameChange();

	// set "Copy Local" button
	CString pathname;
	pathname = GetPathName();
	CString drive, dir, name, ext;
	Util::SplitPath(pathname, &drive, &dir, &name, &ext);
	drive += '\\';
	UINT type = ::GetDriveType(drive);
	m_fCopyLocal = (type != DRIVE_FIXED);
	m_btnCopyLocal.SetCheck(m_fCopyLocal);
}

/////////////////////////////////////////////////////////////////////////////
// CImportProjectDlg dialog -- Import Projects dialog

CImportProjectDlg::CImportProjectDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent/*=NULL*/)
	: CImportDlg(TRUE, szFilename, szDir, filters, IDD, pParent)
{
	m_ofn.lpstrTitle = "Import Projects";

	//{{AFX_DATA_INIT(CImportProjectDlg)
	//}}AFX_DATA_INIT
}

CImportProjectDlg::~CImportProjectDlg()
{
}

void CImportProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CImportDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportProjectDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportProjectDlg, CImportDlg)
	//{{AFX_MSG_MAP(CImportProjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CImportProjectDlg::OnInitDialog()
{
	CImportDlg::OnInitDialog();

	// init user categories
	InitCategories(CPMWCollection::typeProjects);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// COpenPictureDlg dialog -- Open Picture From Disk dialog

COpenPictureDlg::COpenPictureDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent/*=NULL*/)
	: CCustomOpenDlg(FALSE, szFilename, szDir, filters, IDD, NULL, pParent)
{
	m_ofn.lpstrTitle = "Open Picture From Disk";

	//{{AFX_DATA_INIT(COpenPictureDlg)
	m_fEmbedPicture = FALSE;
	//}}AFX_DATA_INIT
}

COpenPictureDlg::~COpenPictureDlg()
{
}

void COpenPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CCustomOpenDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenPictureDlg)
	DDX_Control(pDX, IDC_EMBED_PICTURE, m_btnEmbedPicture);
	DDX_Check(pDX, IDC_EMBED_PICTURE, m_fEmbedPicture);
	DDX_Control(pDX, IDC_PICTURE_PREVIEW, m_btnPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COpenPictureDlg, CCustomOpenDlg)
	//{{AFX_MSG_MAP(COpenPictureDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Select notification.
void COpenPictureDlg::OnFileNameChange()
{
	CCustomOpenDlg::OnFileNameChange();

	CString filename = GetPathName();
	CString csName = GetGlobalPathManager()->MakeCompositePath(filename);
	if (!filename.IsEmpty())
	{
		// set "Embed Picture" state
		BOOL checked = FALSE;
		BOOL fEnable = TRUE;

		switch (type_of_art_extension(filename))
		{
			case GRAPHIC_TYPE_PCD:
			case GRAPHIC_TYPE_PMO:
			case GRAPHIC_TYPE_DPP:
			{
				// embed is ALWAYS checked for these types
				fEnable = FALSE;
				checked = TRUE;
				break;
			}
			default:
			{
				checked = GetGlobalPathManager()->FileIsRemovable(csName);
				break;
			}
		}

		m_btnEmbedPicture.EnableWindow(TRUE); // make sure we can change check
		m_btnEmbedPicture.SetCheck(checked);
		m_btnEmbedPicture.EnableWindow(fEnable);

		m_fEmbedPicture = checked;
	}
}

/////////////////////////////////////////////////////////////////////////////
// COpenProjectDlg dialog -- Open Picture From Disk dialog

COpenProjectDlg::COpenProjectDlg(LPSTR szFilename, LPSTR szDir, LPCSTR filters, CWnd* pParent/*=NULL*/)
	: CCustomOpenDlg(TRUE, szFilename, szDir, filters, IDD, NULL, pParent)
{
	m_ofn.lpstrTitle = "Open Project From Disk";

	//{{AFX_DATA_INIT(COpenProjectDlg)
	//}}AFX_DATA_INIT
}

COpenProjectDlg::~COpenProjectDlg()
{
}

void COpenProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CCustomOpenDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenProjectDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COpenProjectDlg, CCustomOpenDlg)
	//{{AFX_MSG_MAP(COpenProjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

