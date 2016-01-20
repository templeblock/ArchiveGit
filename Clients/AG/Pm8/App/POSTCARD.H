#ifndef __POSTCARD_H__
#define __POSTCARD_H__

#include "labels.h"
#include "pdlgclr.h"

class CEnvelopePreview;

/////////////////////////////////////////////////////////////////////////////
// CPostCardTypeDialog dialog

class CPostCardTypeDialog : public CPmwDialogColor
{
// Construction
public:
	CPostCardTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent = NULL, DWORD WizFlags = WIZBUT_CANCEL | WIZBUT_OK, COLORREF cr = INVALID_DIALOG_COLOR);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPostCardTypeDialog)
	enum { IDD = IDD_POSTCARD_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CLabelData* GetChosenPostCard(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void BuildList(void);
	void UpdatePreview(void);
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Generated message map functions
	//{{AFX_MSG(CPostCardTypeDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePostCardList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDblclkPostCardList();
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
