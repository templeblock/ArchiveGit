/*
// $Workfile: WARPSEL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPSEL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:58   FRED
// Initial revision.
// 
//    Rev 1.0   13 May 1995 11:57:34   FRED
// Initial revision.
*/ 

class CWarpSelectListBox;
class CWarpSelectListBoxItem;

#ifndef _WARPSEL_H_
#define _WARPSEL_H_

#include "simplelb.h"
#include "warpfile.h"
#include "warpfld.h"

/////////////////////////////////////////////////////////////////////////////
// CWarpSelectListBoxItem

class CWarpSelectListBoxItem : public CSimpleListBoxItem
{
	INHERIT(CWarpSelectListBoxItem, CSimpleListBoxItem)
	
public:
	CWarpSelectListBoxItem(CWarpField* pWarpField);
public:
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
protected:
	virtual void PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	CWarpField* WarpField(void)
		{ return m_pWarpField; }
	void WarpField(CWarpField* pWarpField)
		{ m_pWarpField = pWarpField; }
protected:
	CWarpField* m_pWarpField;
};

/////////////////////////////////////////////////////////////////////////////
// CWarpSelectListBox window

class CWarpSelectListBox : public COwnerDrawListBox
{
	INHERIT(CWarpSelectListBox, COwnerDrawListBox)
	
// Construction
public:
	CWarpSelectListBox();

// Implementation
public:
	virtual ~CWarpSelectListBox();
	
public:
	void WarpFields(CWarpFile* pWarpFile, LPCSTR pszNoneLabel = NULL);
	void WarpField(CWarpField* pCurrentWarpField);
	CWarpField* WarpField(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CWarpSelectListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
	
public:
	const CString& NoneLabel(void)
		{ return m_csNoneLabel; }
	
protected:
	CString m_csNoneLabel;
};

/////////////////////////////////////////////////////////////////////////////

#endif
