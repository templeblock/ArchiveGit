/*
// $Workfile: OWNERLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/OWNERLB.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:26   Fred
// Initial revision.
*/

class COwnerDrawListBoxItem;
class COwnerDrawListBox;

#ifndef __OWNERLB_H__
#define __OWNERLB_H__

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBoxItem object
//
// One of these objects is attached to each item in the owner draw list box.

class COwnerDrawListBoxItem
{

public:

	COwnerDrawListBoxItem();
	virtual ~COwnerDrawListBoxItem();
	
public:

	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);
};


/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBox window

class COwnerDrawListBox : public CListBox
{
// Construction
public:
	COwnerDrawListBox();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
// Attributes
public:

// Operations
public:
	void SizeItems(void);
	BOOL FontMetrics(TEXTMETRIC& Metrics);
	
public:
	int Index(const CPoint& cpPoint);

// Implementation
public:
	virtual ~COwnerDrawListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(COwnerDrawListBox)
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct);
	
public:
	int DoCharToItem(UINT nChar, UINT nIndex)
		{ return OnCharToItem(nChar, this, nIndex); }
};

/////////////////////////////////////////////////////////////////////////////

#endif			// #ifndef __OWNERLB_H__
