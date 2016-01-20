/*
// $Workfile: OWNERLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/OWNERLB.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:50   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:58   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:00   JAY
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

// Attributes
public:

// Operations
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

#endif
