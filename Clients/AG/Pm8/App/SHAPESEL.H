/*
// $Workfile: SHAPESEL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPESEL.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:14   FRED
// Initial revision.
// 
//    Rev 1.0   23 May 1995 09:17:48   FRED
// Initial revision.
*/ 

class CShapeSelectListBox;
class CShapeSelectListBoxItem;

#ifndef _SHAPESEL_H_
#define _SHAPESEL_H_

#include "simplelb.h"
#include "shapfile.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeSelectListBoxItem

class CShapeSelectListBoxItem : public CSimpleListBoxItem
{
	INHERIT(CShapeSelectListBoxItem, CSimpleListBoxItem)
	
public:
	CShapeSelectListBoxItem(CShapeEntry* pShapeEntry);
public:
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
protected:
	virtual void PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	CShapeEntry* Shape(void)
		{ return m_pShapeEntry; }
	void Shape(CShapeEntry* pShapeEntry)
		{ m_pShapeEntry = pShapeEntry; }
protected:
	CShapeEntry* m_pShapeEntry;
};

/////////////////////////////////////////////////////////////////////////////
// CShapeSelectListBox window

class CShapeSelectListBox : public COwnerDrawListBox
{
	INHERIT(CShapeSelectListBox, COwnerDrawListBox)
	
// Construction
public:
	CShapeSelectListBox();

// Implementation
public:
	virtual ~CShapeSelectListBox();
	
public:
	void Shapes(CShapeFile* pShapeFile);
	void Shape(CShapeEntry* pCurrentShapeEntry);
	CShapeEntry* Shape(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CShapeSelectListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
};

/////////////////////////////////////////////////////////////////////////////

#endif
