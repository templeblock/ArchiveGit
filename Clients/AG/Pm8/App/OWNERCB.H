/*
// $Workfile: OWNERCB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/OWNERCB.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:29:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:16   FRED
// Initial revision.
// 
//    Rev 1.0   02 May 1995 15:41:18   FRED
// Initial revision.
*/

class COwnerDrawComboBoxItem;
class COwnerDrawComboBox;

#ifndef __OWNERCB_H__
#define __OWNERCB_H__

// ownercb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawComboBoxItem object
//
// One of these objects is attached to each item in the owner draw combo box.

class COwnerDrawComboBoxItem
{

public:

	COwnerDrawComboBoxItem();
	virtual ~COwnerDrawComboBoxItem();
	
public:

	virtual void DrawItem(COwnerDrawComboBox* pCombo, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawComboBox* pCombo, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawComboBox* pCombo, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawComboBox* pCombo, LPDELETEITEMSTRUCT pDeleteItemStruct);
};


/////////////////////////////////////////////////////////////////////////////
// COwnerDrawComboBox window

class COwnerDrawComboBox : public CComboBox
{
// Construction
public:
	COwnerDrawComboBox();

// Attributes
public:

// Operations
public:
	void SizeItems(void);
	BOOL FontMetrics(TEXTMETRIC& Metrics);

// Implementation
public:
	virtual ~COwnerDrawComboBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(COwnerDrawComboBox)
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
