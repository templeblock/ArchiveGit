// prefs.h : header file
//
#include "propsht.h"
#include "id.h"

/////////////////////////////////////////////////////////////////////////////
// CMemoryPrefPage dialog

class CMemoryPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CMemoryPrefPage)

// Construction
public:
	CMemoryPrefPage();
	~CMemoryPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMemoryPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CMemoryPrefPage)
	enum { IDD = IDD_PREFMEMORY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	FNAME	m_szRamDisk;
	int 	m_MainMemMin;
	int 	m_MainMemFactor;
	BOOL 	m_LineArtAsGray;
};

/////////////////////////////////////////////////////////////////////////////
// CUndoPrefPage dialog

class CUndoPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CUndoPrefPage)

// Construction
public:
	CUndoPrefPage();
	~CUndoPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUndoPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();
	void WackCmdList();

// Dialog Data
	//{{AFX_DATA(CUndoPrefPage)
	enum { IDD = IDD_PREFUNDO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL	m_UseApply;
	BOOL	m_NoUndo;
	BOOL	m_UndoMasks;
	BOOL	m_UndoObjects;
	BOOL	m_DoCommandList;
};

/////////////////////////////////////////////////////////////////////////////
// CObjectsPrefPage dialog

class CObjectsPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CObjectsPrefPage)

// Construction
public:
	CObjectsPrefPage();
	~CObjectsPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CObjectsPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CObjectsPrefPage)
	enum { IDD = IDD_PREFOBJECTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL 	m_bObjects;
	BOOL 	m_bObjectMasks;
};

/////////////////////////////////////////////////////////////////////////////
// CScratchpadPrefPage dialog

class CScratchpadPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CScratchpadPrefPage)

// Construction
public:
	CScratchpadPrefPage();
	~CScratchpadPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScratchpadPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CScratchpadPrefPage)
	enum { IDD = IDD_PREFSCRATCHPAD };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	int m_ScratchpadWidth;
	int m_ScratchpadHeight;
	BOOL m_IsScratchpadColor;
};

/////////////////////////////////////////////////////////////////////////////
// CPluginPrefPage dialog

class CPluginPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CPluginPrefPage)

// Construction
public:
	CPluginPrefPage();
	~CPluginPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPluginPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CPluginPrefPage)
	enum { IDD = IDD_PREFPLUGIN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL 	m_UsePlugins;
	FNAME 	m_szPlugin1;
	FNAME	m_szPlugin2;
};

/////////////////////////////////////////////////////////////////////////////
// CUnitsPrefPage dialog

class CUnitsPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CUnitsPrefPage)
 
// Construction
public:
	CUnitsPrefPage();
	~CUnitsPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUnitsPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CUnitsPrefPage)
	enum { IDD = IDD_PREFUNITS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	WORD 	m_Units;
	LFIXED 	m_ScreenWidth;
	BOOL 	m_UseRulers;
	BOOL 	m_DisplayPixelValue;
};

/////////////////////////////////////////////////////////////////////////////
// CMiscPrefPage dialog

class CMiscPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CMiscPrefPage)

// Construction
public:
	CMiscPrefPage();
	~CMiscPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMiscPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CMiscPrefPage)
	enum { IDD = IDD_PREFMISC };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	FNAME	m_szPouchPath;
	FNAME	m_szStartMacro;
	int 	m_UseTablet;
	int 	m_MaskTint;
	BOOL 	m_IndependentSettings;
	BOOL 	m_PasteAtRealSize;
	BOOL 	m_UseGrayScaleIcons;
	BOOL 	m_DisableAutoScroll;
	BOOL 	m_DisableDither;
	ITEMID 	m_DefaultFileType;
};

/////////////////////////////////////////////////////////////////////////////
// CLocationsPrefPage dialog

class CLocationsPrefPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CLocationsPrefPage)

// Construction
public:
	CLocationsPrefPage();
	~CLocationsPrefPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLocationsPrefPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CLocationsPrefPage)
	enum { IDD = IDD_LOCATIONS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
protected:
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()

protected:
	FNAME m_Clipbits; 
	FNAME m_Printstyles;
	FNAME m_Brushes; 
	FNAME m_Palettes;
	FNAME m_Textures; 
	FNAME m_Devices;
	FNAME m_Effects; 
	FNAME m_Macros;
	FNAME m_Importfilters;
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPropSheet

class CPreferencesPropSheet : public CPPPropertySheet
{
	DECLARE_DYNCREATE(CPreferencesPropSheet)

// Construction
public:
	CPreferencesPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	CPreferencesPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	virtual ~CPreferencesPropSheet();

// Dialog Data
protected:
	CMemoryPrefPage		m_MemoryPage;
	CUndoPrefPage		m_UndoPage;
	CObjectsPrefPage  	m_ObjectsPage;
	CScratchpadPrefPage	m_ScratchpadPage;
	CPluginPrefPage		m_PluginPage;
	CUnitsPrefPage		m_UnitsPage;
	CMiscPrefPage		m_MiscPage;
	CLocationsPrefPage 	m_LocationsPage;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void HandleOK();
	virtual void HandleCancel();

protected:
	UINT GetSelectPage();
	void CommonConstruct();

protected:
	afx_msg void OnSave();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

protected:
	static int m_nLastPage;
};

///////////////////////////////////////////////////////////////////////////// 




















	   
