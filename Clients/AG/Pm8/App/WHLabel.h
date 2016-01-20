//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHLabel.h 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/WHLabel.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 12    9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 11    6/05/98 3:22p Psasse
// 
// 
// 10    6/04/98 9:31p Hforman
// remove m_pSheet since it's in baseclass
// 
// 9     6/04/98 1:11p Psasse
// Improved path management for Back button
// 
// 8     6/03/98 9:43p Hforman
// add CWHelpAveryPropertyPage to preview label types
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WHLABEL_H__
#define __WHLABEL_H__


#include "labels.h"

class CWView;
class CWHelpLabelView;
class CInsertFieldViewHelper;

// Label
class CLabelPageContainer : public CWHelpPageContainer
{
	enum
	{
		ID_WH_LABEL_TYPE = 1,
		ID_WH_LABEL_AVERYTYPE,
		ID_WH_LABEL_CATEGORY,
		ID_WH_LABEL_STYLE,
		ID_WH_LABEL_CUSTOMIZE,
		ID_WH_LABEL_LABELTEXT,
		ID_WH_LABEL_HERE_IT_IS
	};

// Construction
public:
	enum {
		SAME_TEXT,
		ADDRESS_BOOK,
		NO_TEXT
	};
	
	CLabelPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CLabelPageContainer();

	CString& GetLabelText (void)
	{  return m_strLabelText;}
	void SetLabelText (const CString& NewString)
	{  m_strLabelText = NewString;}

	int GetTextOption (void)
	{  return m_nTextOption;}
	void SetTextOption (const int NewInt)
	{  m_nTextOption = NewInt;}

	CWHelpWizPropertySheet* GetSheet (void)
	{  return m_pSheet;}


// Implementation
public:

private:
	int	 m_nTextOption;
	CString  m_strLabelText;
};

/////////////////////////////////////////////////////////////////////////////
// CWHCustomizeLabelPage dialog

class CWHCustomizeLabelPage : public CWHelpPropertyPage
{
// Construction
public:
	CWHCustomizeLabelPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWHCustomizeLabelPage)
	enum { IDD = IDD_WH_LABEL_TEXTOPTION };
	//}}AFX_DATA

public:

	CLabelPageContainer* GetContainer (void)
	{  return m_pContainer;}
	void SetContainer (CLabelPageContainer* NewContainer)
	{  m_pContainer = NewContainer;}


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHCustomizeLabelPage)
	virtual void OnTextOptionClick();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive(); 
	//}}AFX_VIRTUAL	


protected:

	CLabelPageContainer* m_pContainer;

	// Generated message map functions
	//{{AFX_MSG(CWHCustomizeLabelPage)
	virtual BOOL OnInitDialog();
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWHLabelTextLabelPage dialog

class CWHLabelTextLabelPage : public CWHelpPropertyPage
{
private:

// Construction
public:
	CWHLabelTextLabelPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

	CLabelPageContainer* GetContainer (void)
	{  return m_pContainer;}
	void SetContainer (CLabelPageContainer* NewContainer)
	{  m_pContainer = NewContainer;}

	// Dialog Data
	//{{AFX_DATA(CWHLabelTextLabelPage)
	enum { IDD = IDD_WH_LABEL_LABELTEXT };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHLabelTextLabelPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void OnKillFocusLabelTextEdit();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CLabelPageContainer* m_pContainer;

	// Generated message map functions
	//{{AFX_MSG(CWHLabelTextLabelPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CWHLabelView dialog

class CWHLabelView : public CWView
{
// Construction
public:
	CWHLabelView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWHLabelView)
	//}}AFX_DATA

public:
	void InsertReceiverBlock();
	void InsertBlock(LPCSTR* pStrings);

	CLabelPageContainer* GetContainer (void)
	{  return m_pContainer;}
	void SetContainer (CLabelPageContainer* NewContainer)
	{  m_pContainer = NewContainer;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHLabelView)
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	


protected:

	CLabelPageContainer* m_pContainer;

	// Generated message map functions
	//{{AFX_MSG(CWHLabelView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWHelpAveryPropertyPage dialog

class CWHelpAveryPropertyPage : public CWHelpCommonPropertyPage
{
// Construction
public:
	CWHelpAveryPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);
	~CWHelpAveryPropertyPage();

// Dialog Data
	//{{AFX_DATA(CWHelpAveryPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHelpAveryPropertyPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				SetLabelData(int nChoice);

	CLabelList		m_LabelList;

	// Generated message map functions
	//{{AFX_MSG(CWHelpAveryPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeChoiceList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // __WHLABEL_H__
