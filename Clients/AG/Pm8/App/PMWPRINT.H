/*
// $Workfile: PMWPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWPRINT.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 7     6/03/98 6:50p Rlovejoy
// Entry points for helpful hints.
// 
// 6     4/24/98 6:33p Fredf
// Fixed bug with simultaneous print previews. Moved preview refresh
// extent pointer into CPmwView... it was a global variable.
// 
// 5     4/20/98 9:01a Fredf
// Fixed bug where all projects were looking at "reversed" flag and
// printing out reversed!
// 
// 4     4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 3     3/06/98 3:59p Jayn
// Post card print code (double-sided).
// 
// 2     12/12/97 11:24a Jayn
// Fixed Class Wizard problem.
// 
//    Rev 1.0   14 Aug 1997 15:24:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:16   Fred
// Initial revision.
// 
//    Rev 1.10   08 Apr 1997 16:11:20   Jay
// Print Instructions control
// 
//    Rev 1.9   03 Apr 1997 14:31:34   Jay
// Resource cleanup
// 
//    Rev 1.8   01 Apr 1997 13:16:36   Jay
// Banner preview
// 
//    Rev 1.7   26 Mar 1997 14:52:04   Jay
// Interruptable preview
// 
//    Rev 1.6   21 Mar 1997 15:49:02   Jay
// More preview changes.
// 
//    Rev 1.5   04 Mar 1997 17:04:08   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.4   06 Aug 1996 14:37:30   Jay
//  
// 
//    Rev 1.3   06 Aug 1996 11:17:40   Jay
//  
// 
//    Rev 1.2   16 Jul 1996 08:50:56   Fred
// Hooked up address/sender book
// 
//    Rev 1.1   11 Apr 1996 12:25:50   FRED
// Copied from PMW2
// 
//    Rev 2.14   03 Apr 1996 09:06:42   FRED
// Fixes for DeskJet envelope printing
// 
//    Rev 2.13   09 Oct 1995 15:15:58   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.12   04 Sep 1995 14:56:46   JAY
// New collate handling.
// 
//    Rev 2.11   30 Aug 1995 11:39:42   FRED
// Test print
// 
//    Rev 2.10   25 Aug 1995 20:38:42   FRED
// First crack at print alignment dialog
*/

class CPmwPrint;
class CPmwPrinting;
class CPrintSetupDialog;

#ifndef __PMWPRINT_H__
#define __PMWPRINT_H__

class CFlatFileDatabaseCursor;
class CPmwDoc;
class CPmwView;

#include <afxpriv.h>				// for CPreviewView

#include "drawobj.h"					// for our arrow.

/*
// How much do we overlap tiles?
*/

#define	TILE_OVERLAP			250						/* 1/4" @ 1000 dpi */
#define	PAGE_TILE_OVERLAP		450						/* 1/4" @ 1800 dpi */

/*
// Cut marks are a single point thick.
*/

#define	CUT_MARK_THICK		(1800/72)

#define	HORZ_CUT_PATTERN		101
#define	VERT_CUT_PATTERN		102

/////////////////////////////////////////////////////////////////////////////
// TileInfo structure

/*
// Tile information.
*/

struct TileInfo
{
	BOOL		m_cut_sheet;
	PCOORD	m_tile_width;
 	PCOORD	m_tile_height;
	SHORT		m_x_tiles;
	SHORT		m_y_tiles;
	SHORT		m_x_tile_overlap;
	SHORT		m_y_tile_overlap;

	VOID		compute(CPmwDoc *pDoc, double scale);
};

/////////////////////////////////////////////////////////////////////////////
// CPmwPrint dialog

class CPmwPrint : public CPrintDialog
{
// Construction
public:
	CPmwPrint(CPmwView *pView, CPmwDoc* pDoc = NULL, UINT id = CPmwPrint::IDD);
	~CPmwPrint();

// Dialog Data
	//{{AFX_DATA(CPmwPrint)
	enum { IDD = IDD_PRINT };
	BOOL	m_smoothing;
	int		m_coloring_book;
	//}}AFX_DATA

	BOOL m_fDoubleSided;
	BOOL m_fReversed;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPmwPrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:

/*
// Current scale.
// This is changed by the CPrintSize dialog.
*/
	double	m_scale;

/*
// Print range.
// This is changed by the CPrintRange dialog.
*/

	LPBYTE	m_tiles;
	SHORT		m_tiles_chosen;

//	POINT		m_first_tile;
//	POINT		m_last_tile;

/*
// Document we are going to be printing.
*/

	CPmwDoc *m_pDoc;
	CPmwView *m_pView;			// Only used for fill-in fields call.
										// Is NULL for Picture Catalog print.

	virtual int DoModal();
// Implementation
protected:

	void update_scale();
	void set_all_tiles();
	void update_tiles();
	void update_paper_info();
	void update_name_list(void);
	void UpdateFastBitmaps(void);
	virtual void update_names_chosen(void);

	virtual void ChangedPrinter(void);

	// Generated message map functions
	//{{AFX_MSG(CPmwPrint)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickedPrintTiles();
	afx_msg void OnClickedPrintSize();
	afx_msg void OnClickedPrintSetup();
	afx_msg void OnClickedPrintAdvanced();
	afx_msg void OnDestroy();
	afx_msg void OnPrintNameList();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnFastBitmapPrinting();
	afx_msg void OnPrintDoubleSided();
	afx_msg void OnPrintInstructions();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	virtual CPrintDialog* AttachOnSetup();
	void UpdateInstructions(void);
};

/////////////////////////////////////////////////////////////////////////////
// CPmwPrinting dialog

class CPmwPrinting : public CDialog
{
// Construction
public:
	CPmwPrinting(WORD wSoundID = 0, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPmwPrinting)
	enum { IDD = IDD_PRINTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	WORD m_wSoundID;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPmwPrinting)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PrintContext structure
// (Perhaps this should be a class someday.)

struct PrintContext
{
	PrintContext();				// constructor

	void SetDefaults(void);

	CPrintInfo	*m_pInfo;		// print info
	BOOL			m_fStarted;		// Was the print job successfully started?
	BOOL			m_modified;		// Save for document Modified flag
	double		m_print_scale;	// Print scale (1 == 100%)
	CDC			m_dcPrint;		// DC for printing
	CPmwPrint	*m_pPD;			// Print dialog with all user choices
	TileInfo		m_tile_info;	// Calculated tile information
	PBOX			m_source;		// Bound of source we're currently printing
	BOOL			m_fBanding;		// Are we banding?
	POINT			m_scaling;		// Driver scaling factor
	CDialog		*m_dlgstatus;	// status dialog
	SHORT			m_copies;		// copies to print

	BOOL			m_name_shown;	// Is an update name shown in the dialog?
	HWND			m_hWndParent;
/*
// Some useful stuff for labels (at least).
*/
	CFlatFileDatabaseCursor*	m_pCursor;			// Cursor to get names from.

	int			m_nStartingLabel;

	int			m_nBoundName;		// What name is currently bound?
	int			m_nCurrentName;	// What name do we want to be bound?
	int			m_nCurrentCopy;	// What copy are we on?

	void ToStartOfBand(void);
	void SaveStartOfBand(void);

	BOOL			m_fDoubleSided;		// Are we doing double-sided printing?
	WORD			m_wDoubleSidedType;	// From the printer info.

	BOOL			m_fReversed;		// Are we doing a reversed print (i.e., T-Shirt)

protected:
	// Variables for holding the start-of-band variables.
	int			m_nBandLabel;
	int			m_nBandName;
	int			m_nBandCopy;
};

/////////////////////////////////////////////////////////////////////////////
// CPrintSetupDialog dialog

class CPrintSetupDialog : public CPrintDialog
{
// Construction
public:
	CPrintSetupDialog(CWnd* pParent = NULL);	// standard constructor
	CPrintSetupDialog(PRINTDLG& pdInit, CWnd* pParent, int nHelpId);

// Dialog Data
	//{{AFX_DATA(CPrintSetupDialog)
	enum { IDD = IDD_PRINT_SETUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrintSetupDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CKeyArrowObject

class CKeyArrowObject : public DrawingObject
{
	INHERIT(CKeyArrowObject, DrawingObject)

public:

	// Constructor.
	CKeyArrowObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);
	
	// Get the path for this object.
	virtual BOOL GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject = NULL);
};

/////////////////////////////////////////////////////////////////////////////
// CPmwPreviewView

class CPmwPreviewView : public CPreviewView
{
public:
	DECLARE_DYNCREATE(CPmwPreviewView)
	CPmwPreviewView();

	virtual void OnUpdate(CView *sender, LPARAM lHint, CObject *pHint);

	BOOL Refresh(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPmwPreviewView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	CRefreshArray m_RefreshArray;
};

extern BOOL printer_check_interrupt(VOIDPTR data);
extern VOID printer_update_name(VOIDPTR object, LPCSTR name, VOIDPTR data);
extern BOOL SupportsEscape(CDC *pDC, int nEscape);

extern RedisplayContext far printer_rc;

#endif					/* #ifndef __PMWPRINT_H__ */
