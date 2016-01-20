#pragma once

#include "AGSym.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDoc.h"
#include "AGMatrix.h"
#include "AGDC.h"
#include "AGDib.h"
#include "Font.h"

class CXMLDlg : public CDialog
{
// Construction
public:
	CXMLDlg(CWnd* pParent = NULL);	// standard constructor

private:
	void CreateNewProject(HTREEITEM pItem);
	int CreatePages(HTREEITEM pItem);
	void HandlePageItems(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset);
	void CreateImageItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset);
	bool HandleAddImage(CAGPage* pPage, RECT* pDestRect, LPCTSTR strFileName, double fAngle);
	bool AddImage(HGLOBAL hMemory, bool bIsDIB, CAGPage* pPage, RECT* pDestRect);
	void CreateTextItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset);
	CAGSpec* HandleAddText(HTREEITEM pChildItem, LOGFONT& Font, COLORREF& Color, eTSJustTypes& Alignment, BYTE* pTextBuffer);
	void CreateGraphicItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset, CString& szItemType);
	COLORREF ParseColor(CString& strColor);
	void GetCurrentObjectLocation(HTREEITEM pItem, POINT& Location);
	void GetCurrentObjectSize(HTREEITEM pItem, POINT& Size);
	void GetCurrentObjectRect(HTREEITEM pItem, RECT& Rect);
	CString GetCurrentObjectFile(HTREEITEM pItem);
	bool GetCurrentPortrait(HTREEITEM pItem);
	AGDOCTYPE GetCurrentDocType(HTREEITEM pItem);
	AGDOCTYPE ConvertType(const CString& szType);
	bool SaveDoc(LPCTSTR strFileName);
	CString GetPageName(int nPage);
	void Separate(const CString& szItemType, CString& szTitle, CString& szValue);
	bool Intersect(const RECT& ItemRect, const RECT& PageRect);
	void GetPageRect(CAGPage* pPage, RECT* pRect);

protected:
	static void _cdecl MyXMLCallback(int iLevel, LPCSTR pNameString, LPCSTR pValueString, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CXMLDlg)
	enum { IDD = IDD_XML_DIALOG };
	CButton	m_btnAsync;
	CTreeCtrl m_treeOutput;
	CString m_strFileName;
	CString m_strFileNameOut;
	BOOL m_bAsync;
	BOOL m_bStream;
	CString	m_strSearchPattern;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXMLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnStreamClicked();
	afx_msg void OnBrowseClicked();
	afx_msg void OnCreateOutput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CAGDoc* m_pAGDoc;
	CString m_szProjectPath;
	CFontList m_FontList;
};
