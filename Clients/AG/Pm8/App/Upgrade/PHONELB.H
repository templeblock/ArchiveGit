/*
// $Workfile: PHONELB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONELB.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:52   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:58   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:50   JAY
// Initial revision.
// 
//    Rev 1.7   24 Feb 1995 14:34:50   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.6   21 Jan 1995 10:54:52   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.5   19 Jan 1995 14:20:56   JAY
// New free pictures!
// 
//    Rev 1.4   17 Jan 1995 16:10:38   FRED
// Better fix for flag changing bug. The problem was
// the signature for the virtual function Select()
// was wrong.
// 
// 
//    Rev 1.3   17 Jan 1995 15:43:50   FRED
// Fixed bug where item flags could be changed
// by mouse clicks.
// 
// 
//    Rev 1.2   15 Dec 1994 17:29:32   FRED
//  
// 
//    Rev 1.1   14 Dec 1994 17:20:24   FRED
//  
// 
//    Rev 1.0   13 Dec 1994 18:30:34   FRED
// Initial revision.
*/

class CPhoneListBoxItem;
class CPhoneListBox;

#ifndef __PHONELB_H__
#define __PHONELB_H__

class CProductItems;

// Phonelb.h : header file
//

#include "checklb.h"
#include "ownerlb.h"
#include "prod.h"

/////////////////////////////////////////////////////////////////////////////
// CPhoneListBoxItem object
//

class CPhoneListBoxItem : public CProductCheckListBoxItem
{

public:

	CPhoneListBoxItem(CProductItem* pItem, int nIndent = 0);
	virtual ~CPhoneListBoxItem();
	
protected:

	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem);
	virtual void Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};

/////////////////////////////////////////////////////////////////////////////
// CPhoneListBox object
//

class CPhoneListBox : public CProductCheckListBox
{
// Construction
public:
	CPhoneListBox();

// Attributes
public:

// Operations
public:
	void AddProducts(CProductItems* pItems);
	void AddItems(CProductItems* pItems, int nIndent);
	void AddFreeItems(CProductItems* pItems, BOOL fFirst);

	virtual BOOL Select(int nIndex, BOOL fMark, BOOL fDragging);
	
// Implementation
public:
	virtual ~CPhoneListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CPhoneListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
