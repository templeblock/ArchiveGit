//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/svpicdlg.h 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/svpicdlg.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 14    4/13/98 6:54p Hforman
// more category stuff
// 
// 13    4/13/98 2:33p Hforman
// add Category, remove Friendly Name and Description
// 
// 12    1/29/98 8:13p Hforman
// add support for "pmo" file type; other cleanup
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SVPICDLG_H__
#define __SVPICDLG_H__

#include "pagedefs.h"	// for PBOX
#include "UsrCtgry.h"

#define  MAX_WIDTH   0x7fff
#define  MIN_WIDTH   2        // Some graphics programs don't like < 2
#define  MAX_HEIGHT  0x7fff
#define  MIN_HEIGHT  2

/////////////////////////////////////////////////////////////////////////////
// CFileSaveAsGraphicDlg dialog

class CFileSaveAsGraphicDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CFileSaveAsGraphicDlg)
 
public:
	CFileSaveAsGraphicDlg(BOOL Selection, LPCSTR Filter, LPCSTR defExt, LPCSTR initialDir, CWnd* pParentWnd = NULL);

   void SetDimensions (PBOX *pbAll, PBOX *pbSel, int LogPixX, int LogPixY);

   BOOL IsSelectedAvailable()		{ return (m_SaveAll > 0); }
   BOOL IsSaveSelected()			{ return (m_SaveAll == 1); }
   int GetGraphicTypeIndex()		{ return m_nGraphicIndex; }
   BOOL AddToGallery()				{ return m_fAddToGallery; }

   // IDD is appended to the common file dialog, makes CW happy here
   //{{AFX_DATA(CFileSaveAsGraphicDlg)
	enum { IDD = IDD_SAVE_2_PICT_OPTS };
	CButton	m_btnNewCategory;
	CComboBox	m_CategoryCombo;
	CButton	m_btnAspect;
	CEdit	m_editWidth;
	CEdit	m_editHeight;
   UINT  m_Width;
   UINT  m_Height;
   int   m_SaveAll;
	BOOL	m_fAddToGallery;
	CString	m_strCategory;
	//}}AFX_DATA

protected:
   static BOOL  m_Aspect;

	CUserCategoryManager		m_CategoryManager;
	double		m_dWidth, m_dHeight;
	CPoint		DimensionsAll, DimensionsSel;
	int			m_nGraphicIndex;   

	CPoint		PboxToScreen(PBOX *pb, int LogPixX, int LogPixY);
	void			SetDimensions (CPoint *pAll, CPoint *pSell);
	CPoint		GetSelectedDimensionSet();
	void			SelectDimensionSet();
	void			CalcWidth();
	void			CalcHeight();
	void			LimitDimensions();

	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
	//{{AFX_MSG(CFileSaveAsGraphicDlg)
	virtual BOOL OnInitDialog ();
	virtual BOOL OnFileNameOK ();
	afx_msg void OnKillFocusHeight();
	afx_msg void OnKillFocusWidth();
	afx_msg void OnSave2All();
	afx_msg void OnSave2Sel();
	afx_msg void OnAddToGallery();
	afx_msg void OnNewCategory();
	//}}AFX_MSG
	virtual void OnTypeChange();
	DECLARE_MESSAGE_MAP()
};

#endif // __SVPICDLG_H__
