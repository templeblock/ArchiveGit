/*
// $Workfile: DocDump.h $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DocDump.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 6     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 5     6/05/98 2:31p Fredf
// Allows graphics to be optionally embedded.
// 
// 4     5/19/98 11:34a Dennis
// Moved some methods from CDocDumpDialog into new class "CDocDump" 
// 
// 3     1/13/98 11:16a Jayn
// Compact, freeing old frames and paragraphs, etc.
// 
// 2     12/23/97 10:27a Jayn
// 
// 1     12/23/97 9:58a Jayn
// Document dump and utility classes
*/

#ifndef __DOCDUMP_H__
#define __DOCDUMP_H__

#include "pmwdlg.h"

class CPmwDoc;

class CDocDump
   {
   public:
   // Construction
   	CDocDump(CPmwDoc* pDoc, IPathManager* pPathManager);

   // Operations
   	void BatchFix(LPCSTR szInputFileName, BOOL bRunSilent=FALSE, BOOL bFreeEmbedded=TRUE, BOOL bEmbed=FALSE);
	   void DoFreeEmbedded(BOOL fBatch = FALSE, CStdioFile* pLogFile = NULL);
   	void DoCompact();
   	void DoFreeOldTextBoxes();

   protected:
   // Helpers

   protected:
   // Data members
   	CPmwDoc *      m_pDoc;
		IPathManager*	m_pPathManager;
   };

/////////////////////////////////////////////////////////////////////////////
// CDocDumpDialog dialog

class CDocDumpDialog : public CPmwDialog
{
// Construction
public:
	CDocDumpDialog(CPmwDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDocDumpDialog)
	enum { IDD = IDD_DUMP_DOCUMENT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocDumpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocDumpDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFreeEmbedded();
	afx_msg void OnCompact();
	afx_msg void OnBatchFix();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Helpers
	void UpdateRecordList(void);
	void UpdateStorageList(void);

protected:
// Data Members
	CPmwDoc *   m_pDoc;
   CDocDump    m_docDump;
};

#endif
