// sysinfo.h : header file
//

#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCpuInfo

extern "C" class CCpuInfo
{
public:
	enum
	{
		CPU_Unknown			=	  -1,
		CPU_8086				=		0,
		CPU_80186			=		1,
		CPU_80286			=		2,
		CPU_80386			=		3,
		CPU_80486			=		4,
		CPU_Pentium			=		5,
		CPU_PentiumPro		=		6
	};

protected:
	struct FREQUENCY_INFO
	{
		unsigned long in_cycles;   // Internal clock cycles during test
		unsigned long ex_ticks;    // Microseconds elapsed during test
		unsigned long raw_freq;    // Raw frequency of CPU in MHz
		unsigned long norm_freq;   // Normalized frequency of CPU in MHz.
	};

#ifdef WIN32
	typedef WORD (FAR _cdecl * PFN_WINCPUID)();
	typedef FREQUENCY_INFO (FAR _cdecl * PFN_CPUSPEED)(int nClocks);
#else
	typedef WORD (FAR PASCAL * PFN_WINCPUID)();
	typedef FREQUENCY_INFO (FAR PASCAL * PFN_CPUSPEED)(int nClocks);
#endif

public:
	CCpuInfo(LPCSTR pszPath = NULL);
	virtual ~CCpuInfo();

public:
	BOOL IsValid(void);
	LPCSTR GetDllName(void)
		{ return m_csDllName; }

protected:
	BOOL LoadDll(void);
	void UnloadDll(void);

public:
	int GetCpuId(void);
	DWORD GetCpuSpeed(void);

protected:
	HINSTANCE m_hDll;
	CString m_csDllName;
};

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialogReadOnlyControl

class CSystemInformationDialogReadOnlyControl : public CWnd
{
// Construction
public:
	CSystemInformationDialogReadOnlyControl();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CSystemInformationDialogReadOnlyControl();

protected:
	// Generated message map functions
	//{{AFX_MSG(CSystemInformationDialogReadOnlyControl)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT uHitTest, UINT uMessage);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
};

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialog dialog

class CSystemInformationDialog : public CBaseDialog
{
public:
	enum
	{
		PLATFORM_Unknown,
		PLATFORM_Windows,
		PLATFORM_Windows_NT
	};

// Construction
public:
	CSystemInformationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemInformationDialog)
	enum { IDD = IDD_SYSTEM_INFORMATION };
	BOOL	m_fIncludeInformation;
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
public:
	CString m_csSystemInformation;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemInformationDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL GetOperatingSystem(int& nPlatform, WORD& wVersion);
	void GetProcessor(int& nProcessor, DWORD& dwSpeed);
	int GetRamSize(void);
	void ScanDrives(int& nDiskSpace, BOOL& fCdrom);
	CSize GetVideoResolution(void);
	int GetVideoDepth(void);
	BOOL CheckForSoundCard(void);

protected:
	CSystemInformationDialogReadOnlyControl m_OperatingSystem;
	CSystemInformationDialogReadOnlyControl m_Processor;
	CSystemInformationDialogReadOnlyControl m_RamSize;
	CSystemInformationDialogReadOnlyControl m_HardDiskSize;
	CSystemInformationDialogReadOnlyControl m_VideoResolution;
	CSystemInformationDialogReadOnlyControl m_VideoColors;
	CSystemInformationDialogReadOnlyControl m_Cdrom;
	CSystemInformationDialogReadOnlyControl m_SoundCard;

protected:
	CCpuInfo m_CpuInfo;
};

#endif
