//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHNwsltr.h 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHNwsltr.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 1     9/22/98 3:07p Hforman
// Changed name from Newlett.h
//

class CWView;
class CWHelpNewsLetterView;

// NewsLetter
class CNewsLetterPageContainer : public CWHelpPageContainer
{
	enum
	{
		ID_NEWSLETTER_CATEGORY = 1,
		ID_NEWSLETTER_STYLE,
		ID_NEWSLETTER_CUSTOMIZE,
		ID_NEWSLETTER_HERE_IT_IS
	};

// Construction
public:
	CNewsLetterPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CNewsLetterPageContainer();

	CString& GetNumPages (void)
	{  return m_strNumPages;}
	void SetNumPages (const CString& NewString)
	{  m_strNumPages = NewString;}

	BOOL GetDoubleSided (void)
	{  return m_fDoubleSided;}
	void SetDoubleSided (const BOOL NewFlag)
	{  m_fDoubleSided = NewFlag;}


// Implementation
public:

private:
	BOOL	 m_fDoubleSided;
	CString  m_strNumPages;

};

/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterView dialog

class CWHNewsletterView : public CWView
{
// Construction
public:
	CWHNewsletterView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWHNewsletterView)
	//}}AFX_DATA

public:
	void AddPages(CPmwDoc* pDocument, int indexTo, int indexFrom);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHNewsletterView)
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	


protected:

	CNewsLetterPageContainer* m_pContainer;

	// Generated message map functions
	//{{AFX_MSG(CWHNewsletterView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterPage dialog

class CWHNewsletterPage : public CWHelpPropertyPage
{
private:

// Construction
public:
	CWHNewsletterPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

	CNewsLetterPageContainer* GetContainer (void)
	{  return m_pContainer;}
	void SetContainer (CNewsLetterPageContainer* NewContainer)
	{  m_pContainer = NewContainer;}

	// Dialog Data
	//{{AFX_DATA(CWHNewsletterPage)
	enum { IDD = IDD_WH_NEWSLETTER_PAGES };
	BOOL 	m_fDoubleSided;
	CString	m_strNumPages;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHNewsletterPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void OnDoubleSidedClick();
	virtual void OnNumPagesChange();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CNewsLetterPageContainer* m_pContainer;

	// Generated message map functions
	//{{AFX_MSG(CWHNewsletterPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};






