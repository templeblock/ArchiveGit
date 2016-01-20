// monitor.h : header file
//
#include "propsht.h"
#include "id.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorGammaPage dialog

class CMonitorGammaPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CMonitorGammaPage)

// Construction
public:
	CMonitorGammaPage();
	~CMonitorGammaPage();

	BOOL HandleQueryNewPalette();
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMonitorGammaPage)
	protected:
	//}}AFX_VIRTUAL

protected:
	void SetupGammaImage();
	void ModifyGamma();
	void SetupNew(int idCtl, LFIXED fGamma );

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CMonitorGammaPage)
	enum { IDD = IDD_GAMMA };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	LFIXED		m_RGamma;
	LFIXED		m_GGamma;
	LFIXED		m_BGamma;
	LFIXED		m_RGammaCurr;
	LFIXED		m_GGammaCurr;
	LFIXED		m_BGammaCurr;
	BOOL		m_fGammaDisable;
	BOOL		m_fGammaLock;
	LPIMAGE 	m_lpGammaImage;
	MAP			m_RedMap;
	MAP			m_BlueMap;
	MAP			m_GreenMap;

};
/////////////////////////////////////////////////////////////////////////////
// CMonitorCMSPage dialog

class CMonitorCMSPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CMonitorCMSPage)

// Construction
public:
	CMonitorCMSPage();
	~CMonitorCMSPage();

// Dialog Data
	//{{AFX_DATA(CMonitorCMSPage)
	enum { IDD = IDD_MONITORCMS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMonitorCMSPage)
	//}}AFX_VIRTUAL

// Implementation 
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

};

/////////////////////////////////////////////////////////////////////////////
// CMonitorPropSheet

class CMonitorPropSheet : public CPPPropertySheet
{
	DECLARE_DYNCREATE(CMonitorPropSheet)

// Construction
public:
	CMonitorPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	CMonitorPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMonitorPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMonitorPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	UINT GetSelectPage();
	void CommonConstruct();

// Dialog Data
protected:
	CMonitorGammaPage 	m_GammaPage;
	CMonitorCMSPage		m_CMSPage;
	static int			m_nLastPage;
};

// helper functions
void GetGammaSettings(LPBLTSETTINGS lpSettings);
BOOL SetDisplayGamma(LPBLT lpBlt);

/////////////////////////////////////////////////////////////////////////////
