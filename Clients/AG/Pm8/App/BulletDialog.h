#if !defined(AFX_BULLETDIALOG_H__515C9381_9403_11D2_8D3A_00A0C9A342CC__INCLUDED_)
#define AFX_BULLETDIALOG_H__515C9381_9403_11D2_8D3A_00A0C9A342CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BulletDialog.h : header file
//
#include "text.h"
#include "pmwdoc.h"
#include "pmgfont.h"

/////////////////////////////////////////////////////////////////////////////
// CPBulletNumberedPage dialog

class CPBulletNumberedPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPBulletNumberedPage)

// Construction
public:
	CPBulletNumberedPage();
	~CPBulletNumberedPage();

	void SetStyle( CTextStyle *pStyle )
	{	m_pStyle = pStyle;	}

// Dialog Data
	//{{AFX_DATA(CPBulletNumberedPage)
	enum { IDD = IDD_BULLET_NUMBERED_PAGE };
	CBitmapButton	m_NumberedOutlineButton2;
	CBitmapButton	m_NumberedOutlineButton1;
	CBitmapButton	m_NumberedButton4;
	CBitmapButton	m_NumberedButton3;
	CBitmapButton	m_NumberedButton2;
	CBitmapButton	m_NumberedButton1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPBulletNumberedPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// style with bullet information
	CTextStyle* m_pStyle;

protected:
	// Generated message map functions
	//{{AFX_MSG(CPBulletNumberedPage)
	afx_msg void OnBulletNone();
	afx_msg void OnDoubleclickedBulletNone();
	afx_msg void OnBulletNumbered1();
	afx_msg void OnDoubleclickedBulletNumbered1();
	afx_msg void OnBulletNumbered2();
	afx_msg void OnDoubleclickedBulletNumbered2();
	afx_msg void OnBulletNumbered3();
	afx_msg void OnDoubleclickedBulletNumbered3();
	afx_msg void OnBulletNumbered4();
	afx_msg void OnDoubleclickedBulletNumbered4();
	afx_msg void OnBulletOutline1();
	afx_msg void OnDoubleclickedBulletOutline1();
	afx_msg void OnBulletOutline2();
	afx_msg void OnDoubleclickedBulletOutline2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CPBulletSymbolPage dialog

class CPBulletSymbolPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPBulletSymbolPage)

// Construction
public:
	CPBulletSymbolPage();
	~CPBulletSymbolPage();

	void SetStyle( CTextStyle *pStyle )
	{	m_pStyle = pStyle;	}

	void SetFontServer( PMGFontServer *pServer )
	{	m_pFontServer = pServer;	}

// Dialog Data
	//{{AFX_DATA(CPBulletSymbolPage)
	enum { IDD = IDD_BULLET_SYMBOL_PAGE };
	CBitmapButton	m_SymbolOutlineButton1;
	CBitmapButton	m_SymbolButton3;
	CBitmapButton	m_SymbolButton2;
	CBitmapButton	m_SymbolButton1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPBulletSymbolPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	
	// style with bullet information;
	CTextStyle*		m_pStyle;

	// for setting the bullet font
	PMGFontServer*	m_pFontServer;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPBulletSymbolPage)
	afx_msg void OnBulletCustom();
	afx_msg void OnDoubleclickedBulletCustom();
	afx_msg void OnBulletNone();
	afx_msg void OnDoubleclickedBulletNone();
	afx_msg void OnBulletSymbol1();
	afx_msg void OnDoubleclickedBulletSymbol1();
	afx_msg void OnBulletSymbol2();
	afx_msg void OnDoubleclickedBulletSymbol2();
	afx_msg void OnBulletSymbol3();
	afx_msg void OnDoubleclickedBulletSymbol3();
	afx_msg void OnBulletSymbolOutline1();
	afx_msg void OnDoubleclickedBulletSymbolOutline1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPBulletPropertySheet

class CPBulletPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPBulletPropertySheet)

// Construction
public:
	CPBulletPropertySheet(CTextStyle* pStyle, PMGFontServer* pFontServer, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPBulletPropertySheet(CTextStyle* pStyle, PMGFontServer* pFontServer, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPBulletPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPBulletPropertySheet();

	CTextStyle & GetStyle()
	{	return m_Style;	}


protected:

	CPBulletNumberedPage	m_NumberedPage;
	CPBulletSymbolPage		m_SymbolPage;
	CTextStyle				m_Style;
	PMGFontServer*			m_FontServer; 

	// Generated message map functions
protected:
	//{{AFX_MSG(CPBulletPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BULLETDIALOG_H__515C9381_9403_11D2_8D3A_00A0C9A342CC__INCLUDED_)
