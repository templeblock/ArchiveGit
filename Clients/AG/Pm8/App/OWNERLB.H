/*
// $Workfile: OWNERLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/OWNERLB.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:56   Fred
// Initial revision.
// 
//    Rev 1.1   08 Aug 1996 11:11:40   Fred
// SuperWndProc
// 
//    Rev 1.0   14 Mar 1996 13:29:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:16   FRED
// Initial revision.
// 
//    Rev 1.1   13 May 1995 11:10:12   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.0   19 Apr 1995 12:37:30   JAY
// Initial revision.
// 
//    Rev 1.1   13 Dec 1994 18:29:26   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:02:16   FRED
// Initial revision.
*/

class COwnerDrawListBoxItem;
class COwnerDrawListBox;

#ifndef __OWNERLB_H__
#define __OWNERLB_H__

// ownerlb.h : header file
//

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
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct);
};

/////////////////////////////////////////////////////////////////////////////

#endif			// #ifndef __OWNERLB_H__
