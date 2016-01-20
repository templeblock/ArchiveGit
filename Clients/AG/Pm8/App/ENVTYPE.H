/*
// $Workfile: ENVTYPE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVTYPE.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:44   Fred
// Initial revision.
// 
//    Rev 1.3   15 Apr 1997 17:34:28   johno
// 
//    Rev 1.2   15 Apr 1997 16:55:30   johno
// Changes for "New Project" path
// 
// 
//    Rev 1.1   10 Apr 1997 15:55:30   johno
// Now a CPmwDialogColor
// 
// 
//    Rev 1.0   14 Mar 1996 13:37:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:08   FRED
// Initial revision.
// 
//    Rev 1.1   17 Aug 1995 07:49:18   JAY
// Added double-click handler for the list.
// 
//    Rev 1.0   16 Aug 1995 18:03:12   JAY
// Envelope type.
*/

#ifndef __ENVTYPE_H__
#define __ENVTYPE_H__

#include "labels.h"
#include "pdlgclr.h"

class CEnvelopePreview;

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeTypeDialog dialog

class CEnvelopeTypeDialog : public CPmwDialogColor
{
// Construction
public:
	CEnvelopeTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent = NULL, DWORD WizFlags = WIZBUT_CANCEL | WIZBUT_OK, COLORREF cr = INVALID_DIALOG_COLOR);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEnvelopeTypeDialog)
	enum { IDD = IDD_ENVELOPE_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CPaperInfo* GetChosenEnvelope(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void BuildList(void);
	void UpdatePreview(void);
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Generated message map functions
	//{{AFX_MSG(CEnvelopeTypeDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeEnvelopeList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDblclkEnvelopeList();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	DWORD       m_WizFlags;
   CLabelList  m_List;
	CPaperInfo* m_pOldInfo;

	CLabelData* m_pChosenInfo;
};

#endif
