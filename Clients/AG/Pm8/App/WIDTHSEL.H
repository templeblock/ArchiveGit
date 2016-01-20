/*
// $Workfile: WIDTHSEL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WIDTHSEL.H $
// 
// 1     3/03/99 6:14p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:08   FRED
// Initial revision.
// 
//    Rev 1.2   28 Aug 1995 15:26:04   FRED
// None/NoNone cases for line width selection.
// 
//    Rev 1.1   19 Aug 1995 17:49:20   FRED
// Maximum line sizes
// 
//    Rev 1.0   03 May 1995 10:42:46   FRED
// Initial revision.
*/ 

class CWidthSelectComboBox;
class CWidthSelectComboBoxItem;

#ifndef _WIDTHSEL_H_
#define _WIDTHSEL_H_

#include "simplecb.h"

/////////////////////////////////////////////////////////////////////////////
// CWidthElement

class CWidthElement
{
public:
	CWidthElement(LPCSTR pszName, int nWidth);
	~CWidthElement(void);
public:
	void Name(LPCSTR pszName)
		{ m_csName = pszName; }
	const CString& Name(void)
		{ return m_csName; }
	void Width(int nWidth)
		{ m_nWidth = nWidth; }
	int Width(void)
		{ return m_nWidth; }
protected:
	CString m_csName;
	int m_nWidth;
};

/////////////////////////////////////////////////////////////////////////////
// CWidthSelectComboBoxItem

class CWidthSelectComboBoxItem : public CSimpleComboBoxItem
{
public:
	CWidthSelectComboBoxItem(CWidthElement* pWidth);
	~CWidthSelectComboBoxItem(void);
public:
	virtual void PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	void Width(CWidthElement* pWidth)
		{ m_pWidth = pWidth; }
	CWidthElement* Width(void)
		{ return m_pWidth; }
protected:
	CWidthElement* m_pWidth;
};

/////////////////////////////////////////////////////////////////////////////
// CWidthSelectComboBox window

class CWidthSelectComboBox : public COwnerDrawComboBox
{
	INHERIT(CWidthSelectComboBox, COwnerDrawComboBox)
	
public:
	enum
	{
		DEFAULT_MAX_WIDTH = 36
	};
	
// Construction
public:
	CWidthSelectComboBox();

// Implementation
public:
	virtual ~CWidthSelectComboBox();
	
public:
	void Widths(LPCSTR pszNoneLabel = NULL);
	void Width(int nCurrentWidth);
	int Width(void);
	void WidthName(CString& csName, int nWidth, BOOL fCustom = FALSE);
	BOOL ChooseCustomWidth(int nMaximumWidth = DEFAULT_MAX_WIDTH);

protected:
	// Generated message map functions
	//{{AFX_MSG(CWidthSelectComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
	
public:
	const CString& NoneLabel(void)
		{ return m_csNoneLabel; }
	
protected:
	CString m_csNoneLabel;
	CWidthElement* m_pCustomWidth;
	int m_nCustomWidthIndex;
	BOOL m_fAllowNone;
};

/////////////////////////////////////////////////////////////////////////////

#endif
