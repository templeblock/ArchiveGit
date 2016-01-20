/*
// $Workfile: ENVDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVDOC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:42   Fred
// Initial revision.
// 
//    Rev 1.1   03 Apr 1997 14:31:32   Jay
// Resource cleanup
// 
//    Rev 1.0   14 Mar 1996 13:27:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:02   FRED
// Initial revision.
// 
//    Rev 1.5   06 Oct 1995 11:59:42   JAY
// New print code, etc.
// 
//    Rev 1.4   30 Aug 1995 14:13:38   JAY
// Added PrintPaperInfo() method.
// 
//    Rev 1.3   09 Aug 1995 13:11:22   JAY
// Misc changes.
// 
//    Rev 1.2   28 Jul 1995 12:31:12   JAY
// Printer info. Envelope printing.
// 
//    Rev 1.1   26 Jul 1995 07:41:22   JAY
// Envelope support
// 
//    Rev 1.0   20 Jul 1995 12:52:22   JAY
// Initial revision.
*/

#ifndef __ENVDOC_H__
#define __ENVDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeDoc document

class CEnvelopeDoc : public CPmwDoc
{
	DECLARE_SERIAL(CEnvelopeDoc)
protected:
	CEnvelopeDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

	virtual int DefaultTextSize(int nDefault);

	int EnvelopeOrientation(CPrinterInfo* pPrinterInfo);

	virtual ORIENTATION GetPrintOrientation(void);

// Implementation
protected:
	virtual ~CEnvelopeDoc();

/*
// Resize the document to fit on the paper.
*/

	virtual VOID size_to_paper(VOID);

/*
// Return the paper info to use for tile calculations.
*/

	virtual CPaperInfo* PrintPaperInfo(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnvelopeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
