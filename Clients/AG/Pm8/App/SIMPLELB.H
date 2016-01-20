/*
// $Workfile: SIMPLELB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIMPLELB.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:22   FRED
// Initial revision.
// 
//    Rev 1.0   13 May 1995 11:57:38   FRED
// Initial revision.
*/

class CSimpleListBoxItem;

#ifndef __SIMPLELB_H__
#define __SIMPLELB_H__

#include "ownerlb.h"

/////////////////////////////////////////////////////////////////////////////
// CSimpleListBoxItem object
//

class CSimpleListBoxItem : public COwnerDrawListBoxItem
{

public:

	CSimpleListBoxItem();
	virtual ~CSimpleListBoxItem();
	
public:

	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);
	
protected:

	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void ComputeColors(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground);
	virtual void ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crItem);
	virtual void PaintBackground(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground);
	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsSelected(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};

#endif
