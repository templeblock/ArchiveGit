// prnstyle.h : header file
//
#include "propsht.h"
#include "id.h"
#include "ppmodal.h"
#include "dlgsmap.h"

/////////////////////////////////////////////////////////////////////////////
// CSeparatePage dialog

class CSeparatePage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CSeparatePage)

// Construction
public:
	CSeparatePage();
	~CSeparatePage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSeparatePage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();
	void InitBlackLuts();
	void SepChanged();

// Dialog Data
	//{{AFX_DATA(CSeparatePage)
	enum { IDD = IDD_PPSEP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

protected:
	UINT	m_idTimer;

};

/////////////////////////////////////////////////////////////////////////////
// CCMSPrintPage dialog

class CCMSPrintPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CCMSPrintPage)

// Construction
public:
	CCMSPrintPage();
	~CCMSPrintPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCMSPrintPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CCMSPrintPage)
	enum { IDD = IDD_CMSSEP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:

};

/////////////////////////////////////////////////////////////////////////////
// CHalftonePage dialog

class CHalftonePage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CHalftonePage)

// Construction
public:
	CHalftonePage();
	~CHalftonePage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHalftonePage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();
	void EnableHalftone();

// Dialog Data
	//{{AFX_DATA(CHalftonePage)
	enum { IDD = IDD_HALFTONE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:

};

/////////////////////////////////////////////////////////////////////////////
// CCalibratePage dialog

class CCalibratePage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CCalibratePage)

// Construction
public:
	CCalibratePage();
	~CCalibratePage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCalibratePage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();
	void InitCalLuts();
	void OnDotGain(ITEMID id, UINT codeNotify);
	void OnHilight(ITEMID id, UINT codeNotify);
	void OnShadow(ITEMID id, UINT codeNotify);
	void CalChanged();

// Dialog Data
	//{{AFX_DATA(CCalibratePage)
	enum { IDD = IDD_CALIBRATE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

protected:
	WORD	m_wChannel;
	UINT	m_idTimer;
	MAP		m_MasterMap;

};

/////////////////////////////////////////////////////////////////////////////
// CPrinterExtrasPage dialog

class CExtrasPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CExtrasPage)

// Construction
public:
	CExtrasPage();
	~CExtrasPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CExtrasPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CExtrasPage)
	enum { IDD = IDD_EXTRAS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:

};


/////////////////////////////////////////////////////////////////////////////
// CPrintStylePropSheet

class CPrintStylePropSheet : public CPPPropertySheet
{
	DECLARE_DYNCREATE(CPrintStylePropSheet)

// Construction
public:
	CPrintStylePropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	CPrintStylePropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	virtual ~CPrintStylePropSheet();

// Dialog Data
protected:
	CSeparatePage 		m_SeparatePage;
	CCMSPrintPage		m_CMSPrintPage;
	CHalftonePage 		m_HalftonePage;
	CCalibratePage 		m_CalibratePage;
	CExtrasPage 		m_ExtrasPage;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintStylePropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void HandleOK();
	virtual void HandleCancel();

protected:
	UINT GetSelectPage();
	void CommonConstruct();

protected:
	static int m_nLastPage;
};

///////////////////////////////////////////////////////////////////////////// 
class CBlackGenerateDlg : public CPPMapDlg
{
void OnOK();
void OnCancel();
BOOL OnInitDialog();
BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
enum {IDD = IDD_BLACKGENERATE};
CBlackGenerateDlg(CServerView* pView = NULL, CWnd* pParent = NULL);

DECLARE_DYNCREATE(CBlackGenerateDlg)
};

class CCalibratePrintEditDlg : public CPPMapDlg
{
	void OnCancel();
	BOOL OnInitDialog();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	enum {IDD = IDD_CALIBRATEPRINTEDIT};
	CCalibratePrintEditDlg(CServerView* pView = NULL, CWnd* pParent = NULL);

DECLARE_DYNCREATE(CCalibratePrintEditDlg)
};
