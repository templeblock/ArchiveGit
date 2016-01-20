#if !defined(AFX_STATICSPINCTRL_H__D6A4DB69_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_STATICSPINCTRL_H__D6A4DB69_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StaticSpinCtrl.h : header file
//
#include "timestmp.h"

#define UM_SPIN_CHANGE  WM_USER + 500

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinText window

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinButton window

class CStaticSpinButton : public CBitmapButton
{
// Construction
public:
	CStaticSpinButton(UINT nBmpID);

// Attributes
public:

// Operations
public:


protected:	

	enum
	{
		MinimumMargin	=	5,
		Separator		=	4
	};
	UINT m_nTimer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSpinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticSpinButton();


	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticSpinButton)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetState(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	void CreateBkgndBmp(CDC* pDC, CBitmap& bmBitmap, BOOL bSelected);
	COLORREF m_clFace;
	COLORREF m_clText;
	COLORREF m_clHilight;
	COLORREF m_clShadow;
	UINT m_nBmpID;
};


class CStaticSpinText : public CStatic
{
// Construction
public:
	CStaticSpinText();

// Attributes
public:

// Operations
public:
	void SetText(CString strText);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSpinText)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticSpinText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticSpinText)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CString m_strWindowText;
};



/////////////////////////////////////////////////////////////////////////////
// CStaticSpinCtrl window

class CStaticSpinCtrl : public CStatic
{
// Construction
public:
	CStaticSpinCtrl();

// Attributes
public:

// Operations
public:

	void	Init();
	virtual void	SetRange(int nMin, int nMax);
	virtual void	SetPos(int nPos);
	int		GetPos(){return m_nPos;}
	virtual int GetValue(){return m_nMin + m_nPos;}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSpinCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticSpinCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticSpinCtrl)
	afx_msg void OnIncrease();
	afx_msg void OnDecrease();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void Increase();
	virtual void Decrease();

	CStaticSpinButton m_cButtonDecrease;
	CStaticSpinButton m_cButtonIncrease;
	CBitmap m_bmpInc;
	CStaticSpinText m_cStaticText;
	int m_nMin;
	int m_nMax;
	int m_nPos;
	CString m_strStaticText;
	CBitmap m_Bmp;

};


class CMonthSpinCtrl : public CStaticSpinCtrl
{
public:
	CMonthSpinCtrl();

	virtual void SetPos(int nPos);

};



/////////////////////////////////////////////////////////////////////////////
// CSpinDate window

class CSpinDate : public CStatic
{
// Construction
public:
	CSpinDate();

// Attributes
public:

// Operations
public:
	CTimeStamp GetSetTime();
	CTimeStamp* GetInitialTimeStamp(){return &m_Time;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpinDate)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpinDate();
	void Init(BOOL bIncludeDays);

	// Generated message map functions
protected:

	void OnChangeMonth( int nMonth );
	void OnChangeDay( int nDay );
	void OnChangeYear(int nYear );
	int DaysInMonth(int nMonth);
	BOOL IsBeforeSysTime(int nDay, int nMonth, int nYear );
	//{{AFX_MSG(CSpinDate)
	afx_msg LRESULT OnSpinChange(WORD wParam, LONG lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CTimeStamp m_Time;
	SYSTEMTIME m_sysTime;

	CStaticSpinCtrl m_cDay;
	CMonthSpinCtrl m_cMonth;
	CStaticSpinCtrl m_cYear;
	BOOL m_bIncludeDays;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICSPINCTRL_H__D6A4DB69_E4EF_11D1_B06E_00A024EA69C0__INCLUDED_)
