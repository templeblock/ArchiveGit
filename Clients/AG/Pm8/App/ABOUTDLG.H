/*
// $Header: /PM8/App/ABOUTDLG.H 1     3/03/99 6:01p Gbeddow $
//
// About dialog defs.
//
// $Log: /PM8/App/ABOUTDLG.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 6     7/01/98 5:28p Mwilson
// changed icon to bitmap
// 
// 5     6/16/98 8:27p Psasse
// resizing startup dialog (in progress)
// 
// 4     5/26/98 1:53p Dennis
// Program icon now originates from App specific resource file (if any)
// 
// 3     4/20/98 2:18p Fredf
// Migrated 4.0 About Box and Startup dialog changes.
// 
//    Rev 1.6   02 Feb 1998 15:15:34   Jay
// New copyrights
// 
//    Rev 1.5   30 Jul 1997 12:43:38   johno
// Added RunAboutAddOn
// 
// 
//    Rev 1.4   01 Jul 1997 17:18:30   johno
// Added CBadPrinterDialog
// 
//    Rev 1.3   01 Jul 1997 10:10:34   johno
// Added exit dialog
// 
//    Rev 1.2   06 Jun 1997 16:47:08   Jay
// Startup dialog.
// 
//    Rev 1.1   03 Apr 1997 14:31:30   Jay
// Resource cleanup
// 
//    Rev 1.0   14 Mar 1996 13:28:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:14   FRED
// Initial revision.
// 
//    Rev 2.2   17 Nov 1995 13:14:32   JAY
// Now gets license/registration info from register DLL.
// 
//    Rev 2.1   08 Feb 1995 13:35:38   JAY
// Reverted. New series.
// 
//    Rev 1.3   30 Nov 1994 17:01:42   JAY
//  
// 
//    Rev 1.2   16 Sep 1994 14:12:36   JAY
// New reduced dialog for Micro Center version.
// 
//    Rev 1.1   31 May 1994 12:44:12   JAY
// format_string() now takes an LPCSTR (not LPSTR).
// 
//    Rev 1.0   06 May 1994 17:12:20   JAY
// Initial revision.
*/

#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

#include "pmwdlg.h"
#include "bmStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CPmwDialog
{
public:
   CAboutDlg();

// Dialog Data
   //{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
//	CStatic	m_iconAbout;
	CBmpStatic m_cImage;
	//}}AFX_DATA

	static UINT GetIDD(void);

// Implementation
protected:
   void format_string(UINT id, LPCSTR str);

   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   //{{AFX_MSG(CAboutDlg)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CStartupDialog dialog

class CStartupDialog : public CDialog
{
// Construction
public:
   CStartupDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CStartupDialog();              // standard destructor

// Dialog Data
   //{{AFX_DATA(CStartupDialog)
	enum { IDD = IDD_STARTUP };
	CBmpStatic m_cImage;
	//}}AFX_DATA

	static UINT GetIDD(void);
	virtual void ResizeDlg();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CStartupDialog)
   public:
   virtual BOOL DestroyWindow();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CStartupDialog)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CExitDialog dialog

class CExitDialog : public CDialog
{
// Construction
public:
   CExitDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CExitDialog();              // standard destructor

// Dialog Data
   //{{AFX_DATA(CExitDialog)
	enum { IDD = IDD_CONFIRM_EXIT };
	CBmpStatic m_cImage;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CExitDialog)
   public:
   virtual BOOL DestroyWindow();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CExitDialog)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBadPrinterDialog dialog

class CBadPrinterDialog : public CDialog
{
// Construction
public:
   CBadPrinterDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CBadPrinterDialog();              // standard destructor

// Dialog Data
   //{{AFX_DATA(CBadPrinterDialog)
	enum { IDD = IDD_BAD_PRINTER };
	CBmpStatic m_cImage;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CBadPrinterDialog)
   public:
   virtual BOOL DestroyWindow();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CBadPrinterDialog)
   virtual BOOL OnInitDialog();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

extern void RunAboutAddOn (void);

#endif               // #ifndef __ABOUTDLG_H__
