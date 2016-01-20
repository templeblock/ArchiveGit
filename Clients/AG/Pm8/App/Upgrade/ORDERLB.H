/*
// $Workfile: ORDERLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/ORDERLB.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:50   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:58   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:06   JAY
// Initial revision.
// 
//    Rev 1.7   24 Feb 1995 14:34:58   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.6   20 Jan 1995 15:26:06   FRED
// New "free item marking" code.
// 
// 
//    Rev 1.5   19 Jan 1995 14:20:32   JAY
// New free pictures!
// 
//    Rev 1.4   10 Jan 1995 14:24:44   JAY
// Order state reflected back into order dialog.
// 
//    Rev 1.3   15 Dec 1994 17:29:46   FRED
//  
// 
//    Rev 1.2   15 Dec 1994 17:12:44   FRED
//  
// 
//    Rev 1.1   13 Dec 1994 18:29:16   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:03:04   FRED
// Initial revision.
*/

class COrderListBoxItem;
class COrderListBox;

#ifndef __ORDERLB_H__
#define __ORDERLB_H__

class CProductItems;

// orderlb.h : header file
//

#include "checklb.h"
#include "ownerlb.h"
#include "prod.h"

/////////////////////////////////////////////////////////////////////////////
// COrderListBoxItem object
//

class COrderListBoxItem : public CProductCheckListBoxItem
{

public:

	COrderListBoxItem(CProductItem* pItem, int nIndent = 0);
	virtual ~COrderListBoxItem();
	
protected:

	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};

/////////////////////////////////////////////////////////////////////////////
// COrderListBox object
//

class COrderListBox : public CProductCheckListBox
{
// Construction
public:
	COrderListBox();

// Attributes
public:

// Operations
public:
	void AddProducts(CProductItems* pItems);
	void AddItems(CProductItems* pItems, int nIndent);

	void UpdateMarkedItems(void);

	virtual void AdjustFree(void);
	virtual BOOL Select(int nIndex, BOOL fMark, BOOL fDragging);

// Implementation
public:
	virtual ~COrderListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(COrderListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
