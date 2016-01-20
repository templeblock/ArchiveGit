#if !defined(FontPrvw_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
#define FontPrvw_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FontPrvw.h : header file
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CFontPreviewWnd


class CFontPreviewWnd : public CWnd
{
	int m_nFontID;

// Construction
public:
	CFontPreviewWnd(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CFontPreviewWnd)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontPreviewWnd)
	//}}AFX_VIRTUAL

	void SetFontID(int nFont)
	{
		m_nFontID = nFont;
	}
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFontPreviewWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(FontPrvw_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
