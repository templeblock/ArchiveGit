#ifndef		_ADDPAGEWIZARD_H_
#define		_ADDPAGEWIZARD_H_

#pragma pack(push, BYTE_ALIGNMENT)

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif


enum	EAddPageType	{
			kAddBlankPage,
			kAddDupeDoc,
			kAddQSL
};

enum	EApplType {
			kPrintShopApplType,
			kPressWriterApplType,
			kWebSiteDesApplType
};

const int kMaxNumPages =	10;
const int kMinNumPages =	1;

//	Forward declarations
class AddPageWizard;

// ****************************************************************************
//
//  Class Name:	AddPgWizPage
//
//  Description:	Base Page of the Add Page Wizard
//
// ****************************************************************************
//
class AddPgWizPage : public CPropertyPage
{
	DECLARE_DYNCREATE(AddPgWizPage)

	public:
		AddPgWizPage( );
		AddPgWizPage( int idd );
		virtual ~AddPgWizPage( );
		int IDD;	//	The dialog's dialog id

	//	Operations
	public:
		virtual void SetParentSheet( AddPageWizard* pParent );
		virtual void OnCancel( );

	protected:
		AddPageWizard*		m_pParentSheet;				// Our parent sheet.
};


// ****************************************************************************
//
//  Class Name:	AddPageLayoutSel
//
//  Description: The layout selection page
//
// ****************************************************************************
//
class AddPageLayoutSel : public AddPgWizPage
{
	DECLARE_DYNCREATE(AddPageLayoutSel)

	// Construction
	public:
		AddPageLayoutSel( );
		virtual ~AddPageLayoutSel( );

	// Dialog Data
		//{{AFX_DATA(AddPageLayoutSel)
		//}}AFX_DATA

	//	Operations
	public:

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(AddPageLayoutSel)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(AddPageLayoutSel)
		virtual BOOL		OnInitDialog( );
		virtual BOOL		OnSetActive( );
		virtual LRESULT		OnWizardNext( );

		afx_msg	void		OnRadioBlankPage();
		afx_msg	void		OnRadioDupeDoc();
		afx_msg	void		OnRadioQSL();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )

	private:

		int		m_eAddPageType;	// index of the selected radio button
};


// ****************************************************************************
//
//  Class Name:	AddPageConfLayout
//
//  Description: Confirm page layout page
//
// ****************************************************************************
//
class AddPageConfLayout : public AddPgWizPage
{
	DECLARE_DYNCREATE(AddPageConfLayout)

	// Construction
	public:
		AddPageConfLayout( );
		virtual ~AddPageConfLayout( );

	// Dialog Data
		//{{AFX_DATA(AddPageConfLayout)
			CStatic			m_strNameStatic;
			CEdit			m_ctlPageName;

			CStatic			m_strNumPageStatic;
			CEdit			m_ctlNumPage_Edit;
			CSpinButtonCtrl m_ctlNumPage_Spin;
			int				m_iNumPage;

		//}}AFX_DATA

	//	Operations
	public:

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(AddPageConfLayout)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(AddPageConfLayout)
		virtual BOOL		OnInitDialog( );
		virtual BOOL		OnSetActive( );
		virtual LRESULT		OnWizardBack( );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )
};

// ****************************************************************************
//
//  Class Name:	AddPageDupeLayout
//
//  Description: Document page to duplicate 
//
// ****************************************************************************
//
class AddPageDupeLayout : public AddPgWizPage
{
	DECLARE_DYNCREATE(AddPageDupeLayout)

	// Construction
	public:
		AddPageDupeLayout( );
		virtual ~AddPageDupeLayout( );

	// Dialog Data
		//{{AFX_DATA(AddPageDupeLayout)
		CStatic			m_strPageNameStatic;
		CComboBox		m_ctlPageNameCombo;

		CStatic			m_strPageNumStatic;
		CEdit			m_ctlPageNum_Edit;
		CSpinButtonCtrl	m_ctlPageNum_Spin;
		int				m_iPageNum;

		CStatic			m_strNumOfPagesStatic;
		CEdit			m_ctlNumOfPages_Edit;
		CSpinButtonCtrl m_ctlNumOfPages_Spin;
		int				m_iNumOfPages;

		//}}AFX_DATA

	//	Operations
	public:

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(AddPageDupeLayout)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(AddPageDupeLayout)
		virtual BOOL		OnInitDialog( );
		virtual BOOL		OnSetActive( );
		virtual LRESULT		OnWizardBack( );
		virtual LRESULT		OnWizardNext( );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )
};

// ****************************************************************************
//
//  Class Name:	AddPageSelQSL
//
//  Description: Select the QSL for the page 
//
// ****************************************************************************
//
class AddPageSelQSL : public AddPgWizPage
{
	DECLARE_DYNCREATE(AddPageSelQSL)

	// Construction
	public:
		AddPageSelQSL( );
		virtual ~AddPageSelQSL( );

	// Dialog Data
		//{{AFX_DATA(AddPageSelQSL)
		//}}AFX_DATA

	//	Operations
	public:

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(AddPageSelQSL)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(AddPageSelQSL)
		virtual BOOL		OnInitDialog( );
		virtual BOOL		OnSetActive( );
		virtual LRESULT		OnWizardBack( );
		virtual LRESULT		OnWizardNext( );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )
};


// ****************************************************************************
//
//  Class Name:	AddPagePgName
//
//  Description: Set the page name
//
// ****************************************************************************
//
class AddPagePgName : public AddPgWizPage
{
	DECLARE_DYNCREATE(AddPagePgName)

	// Construction
	public:
		AddPagePgName( );
		virtual ~AddPagePgName( );

	// Dialog Data
		//{{AFX_DATA(AddPagePgName)
		CEdit			m_ctlPageName_Edit;
		//}}AFX_DATA

	//	Operations
	public:

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(AddPagePgName)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(AddPagePgName)
		virtual BOOL		OnInitDialog( );
		virtual BOOL		OnSetActive( );
		virtual LRESULT		OnWizardBack( );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )
};

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//
//  Class Name:	AddPageWizard
//
//  Description:	Page of the print calibration dialog
//
// ****************************************************************************
//
class FrameworkLinkage AddPageWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(AddPageWizard)

	// Construction
	public:
		//	REVIEW - make a resource string "Add Page".
		AddPageWizard(int applType, UINT nIDCaption = STRING_ADDPAGEWIZ, CWnd* pParentWnd = NULL
			, UINT iSelectPage = 0);
		virtual ~AddPageWizard( );

	// Attributes
	public:
		virtual void AddPage( AddPgWizPage* pPage );

	// Operations
	public:
		void UpdateWizBtns( BOOL bFirstPage, BOOL bDone );

		void	SetCurPageType(const int &theType){ m_curPageType = theType; }
		int		GetCurPageType() { return m_curPageType; }
		
		int		GetApplType() { return m_applType; }

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(AddPageWizard)
		//}}AFX_VIRTUAL

	// Implementation
	private:
		virtual void InitPages( );
		//virtual RIntRect GetUpdatedPortraitPrintableArea( );
		//virtual RIntRect GetUpdatedLandscapePrintableArea( );
	
	//	Data
	private:

		AddPageLayoutSel			m_PgLayoutSelPage;
		AddPageConfLayout			m_ConfPgLayoutPage;
		AddPageDupeLayout			m_SelPgLayoutPage;
		AddPageSelQSL				m_SelQSLPage;
		AddPagePgName				m_PgNamePage;

		int							m_applType;
		int							m_curPageType;


		// Generated message map functions
	protected:
		//{{AFX_MSG(AddPageWizard)
			// NOTE - the ClassWizard will add and remove member functions here.
		virtual BOOL OnInitDialog( );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP( )
};

#pragma pack(pop)

#endif	//	_ADDPAGEWIZARD_H_