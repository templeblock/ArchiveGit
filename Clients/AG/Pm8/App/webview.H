/*
// $Workfile: webview.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/webview.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 34    2/05/99 5:31p Johno
// Disabled master page stuff for web projects
// 
// 33    2/03/99 5:13p Johno
// Fixed web page print preview crash
// 
// 32    1/25/99 4:36p Johno
// Moved OnPageProperties() from here to PmwView
// 
// 31    1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 30    12/22/98 10:30a Johno
// Moved most HTML conversion stuff to pmwview.*
// 
// 29    7/17/98 5:53p Johno
// Added WebPublishWarnGet(), QueryPublish() uses new CWebPublishQuery
// dialog
// 
// 28    7/14/98 11:11a Johno
// OnDraw over ride for print preview, for the case of a web project
// larger than the printer's paper.
// 
// 27    7/12/98 5:34p Jayn
// Fix for red flashing border.
// 
// 26    7/11/98 3:09p Johno
// Added QueryPublish()
// 
// 25    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 24    6/04/98 2:19p Johno
// Changes for doing HTML format changes on grouped objects
// 
// 23    6/03/98 2:47p Johno
// Changes for "run design checker" menu option
// 
// 22    6/03/98 1:45p Johno
// Added DesignCheckState methods
// 
// 21    5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 20    5/30/98 6:27p Johno
// Changes for HTML warning rect
// 
// 19    5/29/98 5:57p Johno
// HTML Format menu now enabled for multiple text object selection
// 
// 18    5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 17    5/28/98 10:20a Johno
// Compile update
// 
// 16    5/26/98 6:56p Johno
// 
// 15    5/20/98 12:14p Johno
// Changes for design checker
// 
// 14    5/18/98 4:08p Johno
// Changes for layer menu enable and hotspots
// 
// 13    5/11/98 12:07p Johno
// Undo for web text publishing changes
// 
// 12    5/07/98 4:53p Johno
// Added interface for control of conversion of text objects to HTML
// 
// 11    4/20/98 4:40p Johno
// Added scaling web pages to paper size for printing (OnPreparePrinting)
// 
// 10    4/06/98 5:18p Johno
// SelectFullHyperlink now works - it changes text selection bounds to
// contain one, and only one, complete hyperlink
// 
// 9     3/25/98 6:38p Johno
// Changes for text links
// 
// 8     3/24/98 5:25p Johno
// Split OnWebHyperlink into  DoHyperlinkText and  DoHyperlinkObject.
// Added  SelectFullHyperlink (incomplete);
// 
// 7     3/10/98 5:24p Jayn
// Master page changes
// 
// 6     3/10/98 4:19p Johno
// 
// 5     3/06/98 5:50p Johno
// Added CWebView::SetObjectHyperLink
// 
// 4     3/05/98 6:31p Johno
// Various changes for HyperLink
// 
// 3     3/02/98 5:04p Johno
// Compile update
// 
// 2     2/24/98 9:24a Dennis
// Added WebEditDesktop to take control of background drawing.
// 
// 1     12/11/97 1:43p Dennis
// 
*/

#ifndef __WEBVIEW_H__
#define __WEBVIEW_H__

#include "stdafx.h"
#include "newsview.h"
#include "urlproc.h"    // For processing URL's when previewing web pages
#include "filemgr.h"    // For CTempFileManager
#include "carray.h"		// For CVarArray

//#define  MODIFY_LAYER_MENU

#define OVERLAPPING_EXTENTS_TIMER	628			// A nice random number.

/////////////////////////////////////////////////////////////////////////////
// CWebView view

class CWebDoc;

class CWebView : public CNewsletterView
{
	INHERIT(CWebView, CNewsletterView)

   DECLARE_DYNCREATE(CWebView)
protected:
	CWebView();			// protected constructor used by dynamic creation
// Attributes
public:
	CWebDoc* GetWebDocument() const
   {
      return (CWebDoc*)INHERITED::GetDocument();
   }
// Operations
public:

	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

// Implementation
protected:
	virtual ~CWebView();

	virtual CPmwPrint* CreatePrintDialog(void);
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);
	virtual void OnDraw(CDC* pDC);
	// A hook function to handle things after an object has changed position.
	virtual void AfterPositionChange(void);
	// Generated message map functions
protected:
	//{{AFX_MSG(CWebView)
	afx_msg void OnWebSiteProperties();
	afx_msg void OnWebPmsite();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateGotoMasterPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIgnoreMasterItems(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	// Show the overlapping extents in the above array.
	void ShowOverlappingExtents(void);
	// Hide the overlapping extents in the above array,
	// and remove them from the array.
	void ResetOverlappingExtents(void);
	// Toggle all the overlapping extents.
	void ToggleOverlappingExtents(CDC* pInDC = NULL, RedisplayContext* rc = NULL);
	// The toggle function used when overlapping extents are on-screen.
	static BOOL OverlappingExtentsToggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data);

	double
	GetPaperToProjectScale(void);
};

#endif
