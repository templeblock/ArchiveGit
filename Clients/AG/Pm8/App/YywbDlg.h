#if !defined(AFX_YYWBDLG_H__27A43EA1_B50E_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_YYWBDLG_H__27A43EA1_B50E_11D1_8680_0060089672BE__INCLUDED_

#include "yeardll.h"
#include "pictprvw.h"
#include "frameobj.h"
#include "grafobj.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// YywbDlg.h : header file
//

#define ID_YYWB_ERROR -1

#define DEFAULT_N_CATEGORIES 32

#define LOW_YEAR 1890
#define HIGH_YEAR 1998

// Standard page size (inches)
#define STANDARD_W			(8)
#define STANDARD_H			(10.5)
#define STANDARD_PT_SIZE	(10)

#define INCHFRAC_TO_DPI(wh, n, d) \
	(int)(PAGE_RESOLUTION * (wh + (double)n / d))

struct InchFracBoundT
{
	// Normalized spacing units (all values are between 0 and 1)
	double left;
	double top;
	double width;
	double height;
};

/////////////////////////////////////////////////////////////////////////////
// CYywbCategory and derived classed

class CYywbCategory
{
// Data
protected:
	CString m_csName;
	BOOL m_bSaveObjects;
	CFrameObject* m_pFrameObject;
	GraphicObject* m_pGraphicObject;
	BOOL m_bChange;		// TRUE if objects need to be reinitialized

	// Placement
	CYywbCategory* m_pNeighborAbove;
	InchFracBoundT m_InchBound;
	static CPoint m_ptPush;
	static CSize m_WorldSize;

	// Category codes
	DWORD m_dwTitle, m_dwArticle, m_dwGraphic;

	// Graphic placement
	enum GraphicLinesE {
		NO_G_LINE, G_LINE_X, G_LINE_Y
	};

// Construction
public:
	CYywbCategory(CYywbCategory* pNeighborAbove);

// Attributes
public:
	CString& GetCategoryName()
	{
		return m_csName;
	}
	double GetBottom()
	{
		return (m_InchBound.top + m_InchBound.height);
	}
	void Change(BOOL bChange)
	{
		m_bChange = TRUE;
	}

// Operations
public:
	void GetTitleData(CYearDllInterface* pYearDllInterface, CString& csData);
	void GetArticleData(CYearDllInterface* pYearDllInterface, CString& csData);
	void UpdateDoc(CPmwDoc* pDoc);
	void Attach(CPmwDoc* pDoc);
	void Detach(CPmwDoc* pDoc);
	void ComputeLocalBound(CPmwDoc* pDoc, InchFracBoundT& InchBound, PBOX& Bound, int& nFontSize, BOOL bKeepAspect = FALSE, enum GraphicLinesE GLine = NO_G_LINE);
	BOOL CreateFrame(CPmwDoc* pDoc, enum ALIGN_TYPE align, CTxp& Txp, int& nFontSize);
	BOOL CreateGraphic(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface, InchFracBoundT& InchBound, enum GraphicLinesE GLine);
	BOOL CreateBulletFrame(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	BOOL CreateParagraphFrame(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface, UINT nID, BOOL bArticle = TRUE);
	void FreeData();
	BOOL IsValid(CYearDllInterface* pYearDllInterface);
	static void ComputeWorldBound(CPmwDoc* pDoc);

// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void ReCreate(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();

// Implementation
public:
	virtual ~CYywbCategory();
};

class CYywbTime : public CYywbCategory
{
// Construction
public:
	CYywbTime(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbNews : public CYywbCategory
{
// Construction
public:
	CYywbNews(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbEntertainment : public CYywbCategory
{
// Construction
public:
	CYywbEntertainment(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbSports : public CYywbCategory
{
// Construction
public:
	CYywbSports(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbFamous : public CYywbCategory
{
// Construction
public:
	CYywbFamous(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbZodiac : public CYywbCategory
{
// Construction
public:
	CYywbZodiac(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbChinese : public CYywbCategory
{
// Construction
public:
	CYywbChinese(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbStoned : public CYywbCategory
{
// Construction
public:
	CYywbStoned(CYywbCategory* pNeighborAbove);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

class CYywbExternal : public CYywbCategory
{
// Construction
public:
	CYywbExternal(CYywbCategory* pNeighborAbove, CString& csName, DWORD dwCode);

// Operations
public:
// Overrides
	virtual void Create(CPmwDoc* pDoc, CYearDllInterface* pYearDllInterface);
	virtual void Display();
};

/////////////////////////////////////////////////////////////////////////////
// CCategoryListBox window

class CCategoryListBox : public CCheckListBox
{
// Construction
public:
	CCategoryListBox();

// Attributes
public:
	void AddCategory(CYywbCategory* pCategory);

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCategoryListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCategoryListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCategoryListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CYywbDlg dialog

class CPmwView;
class CYywbDlg : public CDialog
{
protected:
	CYearDll m_YearDll;
	CYearDllInterface* m_pYearDllInterface;
	CPmwView* m_pView;
	int m_nCategory;		// current selected category

	// Preview
	CPmwDoc* m_pScrapDoc;
	CPicturePreview m_ppCategoryPict;

	// Birthdate
	SYSTEMTIME m_SystemTime;
	BOOL m_bValidDate;

	// Category data
	CCategoryListBox m_lbCategory;
	CPtrArray m_aCategories;

// Construction
public:
	CYywbDlg(CPmwView* pView, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CYywbDlg)
	enum { IDD = IDD_YYWB };
	static CString	m_csName;
	static CString	m_csBirthdate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYywbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableCategories(BOOL bEnabled);
	void ParseBirthdate();
	int GetMonth(const CString& csMonth) const;
	void RemoveCharacters(CString& csString, const CString& csCharacters) const;
	BOOL GetNextElement(CString& csElement, CString& csString, const CString& csSeparators) const;
	void PreviewSelection(int nOldCategory, int nNewCategory);
	void CheckForEnabledState();

public:
	~CYywbDlg();

	// Generated message map functions
	//{{AFX_MSG(CYywbDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdBirthdate();
	afx_msg void OnChangeEdName();
	afx_msg void OnSelchangeCategoryList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelectAll();
	afx_msg void OnSelectNone();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YYWBDLG_H__27A43EA1_B50E_11D1_8680_0060089672BE__INCLUDED_)
