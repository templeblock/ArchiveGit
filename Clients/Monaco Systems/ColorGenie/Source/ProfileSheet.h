#pragma once
// ProfileSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProfileSheet

#define MAX_TABS 5
#define MAX_TAB_IMAGE_BITMAPS (MAX_TABS*3) // 3 bitmaps per tab; 0=unselected, 1=selected, 2=disabled

/////////////////////////////////////////////////////////////////////////////
class CProfileSheet : public CPropertySheet
{
public:
	DECLARE_DYNCREATE(CProfileSheet)
	CProfileSheet();
	~CProfileSheet();
	int TabIsEnabled(int iTab);
	void EnableTab(int iTab, BOOL bEnable);
	void UpdateTab(int iTab);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

public:
	//{{AFX_DATA(CProfileSheet)
	//}}AFX_DATA

private:
	CImageList* m_pTabImageList;
	int m_iTabSelected;
	BOOL m_bTabEnabled[MAX_TABS];
	static CBitmap* m_pTabBitmap[MAX_TAB_IMAGE_BITMAPS];
	static int m_pSheetCount;

// Generated message map functions
protected:
	//{{AFX_MSG(CProfileSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
