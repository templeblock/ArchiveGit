/*
// $Workfile: COLORSEL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLORSEL.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:26   FRED
// Initial revision.
// 
//    Rev 1.2   03 May 1995 10:41:48   FRED
//  colorsel.cpp
// 
//    Rev 1.1   02 May 1995 15:37:32   FRED
// 
//    Rev 1.0   01 May 1995 13:39:38   FRED
// Initial revision.
*/ 

class CColorSelectComboBox;
class CColorSelectComboBoxItem;

#ifndef _COLORSEL_H_
#define _COLORSEL_H_

#include "simplecb.h"
#include "color.h"
#include "dibbrush.h"

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBoxItem

class CColorSelectComboBoxItem : public CSimpleComboBoxItem
{
public:
	CColorSelectComboBoxItem(ColorElement* pColor);
public:
	virtual void PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	void Color(ColorElement* pColor)
		{ m_pColor = pColor; }
	ColorElement* Color(void)
		{ return m_pColor; }
protected:
	ColorElement* m_pColor;
};

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBox window

class CColorSelectComboBox : public COwnerDrawComboBox
{
	INHERIT(CColorSelectComboBox, COwnerDrawComboBox)
	
// Construction
public:
	CColorSelectComboBox();

// Implementation
public:
	virtual ~CColorSelectComboBox();
	
public:
	void Colors(ColorList* pList = NULL, LPCSTR pszNoneLabel = NULL);
	void Color(COLOR CurrentColor, BOOL fColorExists = TRUE);
	BOOL Color(COLOR* pCurrentColor);
	BOOL ChooseCustomColor(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CColorSelectComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
	
public:
	const CString& NoneLabel(void)
		{ return m_csNoneLabel; }
	CDibPatternBrush* Brush(void)
		{ return &m_DibBrush; }
	
protected:
	CString m_csNoneLabel;
	CDibPatternBrush m_DibBrush;
	ColorElement* m_pCustomColor;
	int m_nCustomColorIndex;
};

/////////////////////////////////////////////////////////////////////////////

#endif
