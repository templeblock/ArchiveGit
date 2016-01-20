/*
// $Workfile: SHARENOT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHARENOT.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     5/26/98 2:01p Dennis
// Added call to GetConfiguration()->ReplaceDialogText() so app. name can
// be substituted in dialog strings.
// 
//    Rev 1.0   14 Aug 1997 15:25:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.1   07 Nov 1996 13:44:30   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:34:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:54   FRED
// Initial revision.
// 
//    Rev 1.1   20 Dec 1995 09:56:44   JAY
// 32-bit fixes
// 
//    Rev 1.0   15 Nov 1995 14:41:42   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "autoexec.h"
#include "sharenot.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShareNotLoadedDialog dialog

CShareNotLoadedDialog::CShareNotLoadedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShareNotLoadedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShareNotLoadedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_fAutoexecBatModified = FALSE;
	m_csBackupName.Empty();
}

void CShareNotLoadedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShareNotLoadedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShareNotLoadedDialog, CDialog)
	//{{AFX_MSG_MAP(CShareNotLoadedDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShareNotLoadedDialog message handlers

void CShareNotLoadedDialog::OnOK()
{
	// The user wants PrintMaster to modify AUTOEXEC.BAT (ugh!)
	
	// Find AUTOEXEC.BAT and modify it.
	CAutoexecBatFile AutoexecBat;
	CString csLine;
	
	TRY
	{
		if (AutoexecBat.Locate())
		{
			// We'll be adding the SHARE line to the start of AUTOEXEC.BAT.
			// Use the Windows OpenFile() function to search for SHARE.EXE.
			OFSTRUCT of;
#ifdef WIN32
			if (OpenFile("SHARE.EXE", &of, OF_EXIST|OF_READ|OF_SHARE_DENY_NONE) != HFILE_ERROR)
#else
			if (OpenFile("SHARE.EXE", &of, OF_EXIST|OF_SEARCH|OF_READ|OF_SHARE_DENY_NONE) != HFILE_ERROR)
#endif
			{
				TRACE("Found SHARE.EXE at '%s'\n", of.szPathName);
				
				// Create a new AUTOEXEC.BAT
				TRY
				{
					// JRO csLine = "REM *** The following SHARE line was added by PrintMaster Gold ***\n";
               csLine = GET_PMWAPP()->GetResourceStringPointer (IDS_REM_AUTOEXEC);
					AutoexecBat.WriteLine(csLine);
					csLine = of.szPathName;
					csLine += '\n';
					AutoexecBat.WriteLine(csLine);
					csLine = "\n";
					AutoexecBat.WriteLine(csLine);
					AutoexecBat.CopyRestOfLines();
					AutoexecBat.Done(TRUE, &m_csBackupName);
					m_fAutoexecBatModified = TRUE;
				}
				CATCH_ALL(e)
				{
					// JRO m_csReason = "Your hard disk or root directory may be full or the AUTOEXEC.BAT file may be write protected.";
               m_csReason = GET_PMWAPP()->GetResourceStringPointer (IDS_ROOT_FULL);
					THROW_LAST();
				}
				END_CATCH_ALL
			}
			else
			{
				// JRO m_csReason = "The SHARE.EXE program could not be found.";
            m_csReason = GET_PMWAPP()->GetResourceStringPointer (IDS_NOT_FOUND_SHARE);
			}
		}
		else
		{
			// JRO m_csReason = "The AUTOEXEC.BAT file could not be found.";
         m_csReason = GET_PMWAPP()->GetResourceStringPointer (IDS_NOT_FOUND_AUTOEXEC);
		}
	}
	CATCH_ALL(e)
	{
		m_fAutoexecBatModified = FALSE;
	}
	END_CATCH_ALL
	
	CDialog::OnOK();
}

BOOL CShareNotLoadedDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CenterWindow(GetDesktopWindow());
	
   GetConfiguration()->ReplaceDialogText(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CAutoexecBatModified dialog

CAutoexecBatModifiedDialog::CAutoexecBatModifiedDialog(CString& csBackupName, CWnd* pParent /*=NULL*/)
	: CDialog(CAutoexecBatModifiedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoexecBatModifiedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	TRY
	{
		m_csBackupName = csBackupName;
	}
	END_TRY
}

void CAutoexecBatModifiedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoexecBatModifiedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoexecBatModifiedDialog, CDialog)
	//{{AFX_MSG_MAP(CAutoexecBatModifiedDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoexecBatModified message handlers

BOOL CAutoexecBatModifiedDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CString csFormat;
	CString csNewText;

	TRY
	{		
		GetDlgItem(IDC_BACKUP_NAME)->GetWindowText(csFormat);
		wsprintf(csNewText.GetBuffer(512), csFormat, (LPCSTR)m_csBackupName);
		csNewText.ReleaseBuffer();
		SetDlgItemText(IDC_BACKUP_NAME, csNewText);
	}
	END_TRY
	
	CenterWindow(GetDesktopWindow());
	
   GetConfiguration()->ReplaceDialogText(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CCannotModifyAutoexecBat dialog

CCannotModifyAutoexecBatDialog::CCannotModifyAutoexecBatDialog(CString& csReason, CWnd* pParent /*=NULL*/)
	: CDialog(CCannotModifyAutoexecBatDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCannotModifyAutoexecBatDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	TRY
	{
		m_csReason = csReason;
	}
	END_TRY
}

void CCannotModifyAutoexecBatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCannotModifyAutoexecBatDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCannotModifyAutoexecBatDialog, CDialog)
	//{{AFX_MSG_MAP(CCannotModifyAutoexecBatDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoexecBatModified message handlers

BOOL CCannotModifyAutoexecBatDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	TRY
	{
		if (!m_csReason.IsEmpty())
		{
			SetDlgItemText(IDC_REASON, m_csReason);
		}
	}
	END_TRY
	
	CenterWindow(GetDesktopWindow());
	
   GetConfiguration()->ReplaceDialogText(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
