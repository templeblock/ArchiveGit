#if !defined(AFX_DSNCHECK_H__2043AD80_F17C_11D1_A49B_00A0C99E4C17__INCLUDED_)
#define AFX_DSNCHECK_H__2043AD80_F17C_11D1_A49B_00A0C99E4C17__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "htmlconv.h"    // for CWebPageStats
//#include "ownerlb.h"

class CListBoxThing
{
public:
	enum ListBoxThingType
	{
		Plain,
		Bold,
		Data
	};

	CListBoxThing(enum ListBoxThingType t, CHTMLCompliantInfo *pInfo = NULL)
	{
		mType = t;
		switch (t)
		{
			case Plain:					// Plain text, NULL data pointer
			ASSERT(pInfo == NULL);
			mpInfo = NULL;
			break;
			
			case Bold:					// Bold text, NULL data pointer
			ASSERT(pInfo == NULL);
			mpInfo = NULL;
			break;
			
			case Data:					// Special text, valid data pointer
			ASSERT(pInfo != NULL);
			mpInfo = pInfo;
			break;
			
			default:
			// Don't know what type this is!
			ASSERT(0);
			mType = Plain;
			mpInfo = NULL;
			break;
		}
	}	

	enum ListBoxThingType
	Type(void)
	{
		return mType;
	}

	CHTMLCompliantInfo*
	GetInfoPointer(void)
	{
		return mpInfo;
	}
protected:
	enum ListBoxThingType	mType;   
	CHTMLCompliantInfo		*mpInfo;
};

class CListDesignChecker : public CListBox //COwnerDrawListBox 
{
public:   
   
	CListDesignChecker(void)
	{
	}

	virtual
	~CListDesignChecker();
	
	int 
   AddString(LPCTSTR lpszItem, enum CListBoxThing::ListBoxThingType Type, CHTMLCompliantInfo *pInfo);

	int 
	AddString (LPCTSTR lpszItem)
	{
      return AddString (lpszItem, CListBoxThing::Plain, NULL);
	}

	int 
   AddString(LPCTSTR lpszItem, CHTMLCompliantInfo *pInfo)
	{
		ASSERT(pInfo != NULL);
		return AddString (lpszItem, CListBoxThing::Data, pInfo);
	}

	int 
   AddStringBold(LPCTSTR lpszItem)
	{
		return AddString (lpszItem, CListBoxThing::Bold, NULL);
	}

	CHTMLCompliantInfo*
	GetItemFor(int idx);

	int
	GetWidest(void)
	{
		int	ret = 0;
		CRect	Rect;

		for (int i = 0; i < GetCount(); i ++)
		{
			GetItemRect(i, &Rect);
			if (Rect.Width() > ret)
				ret = Rect.Width();
		}

		return ret;
	}
 
protected:   

	CArray<CListBoxThing*, CListBoxThing*>	mItems;

	virtual void 
   DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);

   virtual void 
   MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct);
};
// DsnCheck.h : header file
/////////////////////////////////////////////////////////////////////////////
// CDesignCheckerDlg dialog

class CPmwView;

class CDesignCheckerDlg : public CDialog
{
// Construction
public:
	
   CDesignCheckerDlg(LPCSTR ButtonOKText, CPmwView* pParent, BOOL ButtonOKEnable = TRUE);   // standard constructor

   ~CDesignCheckerDlg()
   {
      Empty();
   }

   void
   Empty(void);

   void
   AddPageStats(CWebPageStats &ps);

// Dialog Data
	//{{AFX_DATA(CDesignCheckerDlg)
	enum { IDD = IDD_WEBCHECK };
	CButton	m_ButtonOK;
	//}}AFX_DATA
	CListDesignChecker	m_List1;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignCheckerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignCheckerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrint();
	afx_msg void OnSelchangeList1();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CArray<CWebPageStats*, CWebPageStats*> PageStats;
   BOOL     m_fButtonOKEnable;
   CString  m_ButtonOKText;
   CPmwView *m_pView;

   void
   SetButtonOKText(LPCSTR Text, BOOL Enable = TRUE)
   {
      m_ButtonOKText = Text;
      m_fButtonOKEnable = Enable;
   }

	void
	SizeToWidestString(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSNCHECK_H__2043AD80_F17C_11D1_A49B_00A0C99E4C17__INCLUDED_)
