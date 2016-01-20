/*
// $Workfile: SIMPLECB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIMPLECB.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:22   FRED
// Initial revision.
// 
//    Rev 1.1   13 May 1995 11:10:06   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.0   02 May 1995 15:41:20   FRED
// Initial revision.
*/

class CSimpleComboBoxItem;

#ifndef __SIMPLECB_H__
#define __SIMPLECB_H__

#include "ownercb.h"

/////////////////////////////////////////////////////////////////////////////
// CSimpleComboBoxItem object
//

class CSimpleComboBoxItem : public COwnerDrawComboBoxItem
{

public:

	CSimpleComboBoxItem();
	virtual ~CSimpleComboBoxItem();
	
public:

	virtual void DrawItem(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawComboBox* pComboBox, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawComboBox* pComboBox, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawComboBox* pComboBox, LPDELETEITEMSTRUCT pDeleteItemStruct);
	
protected:

	virtual void Paint(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void ComputeColors(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground);
	virtual void ComputeBounds(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crItem);
	virtual void PaintBackground(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground);
	virtual void PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintFocus(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void Preview(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsSelected(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsFocused(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct);
};

#endif
