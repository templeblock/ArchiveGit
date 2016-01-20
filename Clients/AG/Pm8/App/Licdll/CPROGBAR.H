/*
// $Workfile: CPROGBAR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CPROGBAR.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:16   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:30   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CPROGBAR.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:34   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:51:58   FRED
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 13:36:42   JAY
// Initial revision.
// 
//    Rev 2.2   13 Oct 1995 14:20:02   FRED
// 32 bit changes
// 
//    Rev 2.1   08 Feb 1995 13:39:16   JAY
// Reverted. New series.
// 
//    Rev 1.1   16 Jun 1994 18:15:16   FRED
// Some improvements including Hide/Show
// methods that work.
// 
// 
//    Rev 1.0   16 Jun 1994 09:03:22   FRED
// Initial revision.
*/ 

#ifndef _CPROGBAR_H_
#define _CPROGBAR_H_

/////////////////////////////////////////////////////////////////////////////
// CProgressBar window

class CProgressBar : public CWnd
{
// flags and constants
public:
	// Default range and position constants
	enum
	{
		DEFAULT_MIN			=	0,
		DEFAULT_MAX			=	100,
		DEFAULT_POS			=	0
	};
	
	// Control specific styles
	enum
	{
		PBS_TEXTHASRANGE	=	0x0001,
	};
	
// Construction
public:
	CProgressBar();

// Attributes
public:

// Operations
public:
	DWORD GetMin(void)
		{ return m_dwMin; }
	DWORD GetMax(void)
		{ return m_dwMax; }
	DWORD GetPos(void)
		{ return m_dwPos; }
	UINT GetPosPercentage(void)
		{ return (UINT)GetPercentage(); }
	void SetMin(DWORD dwMin)
		{ m_dwMin = dwMin; Redraw(); }
	void SetMax(DWORD dwMax)
		{ m_dwMax = dwMax; Redraw(); }
	void SetPos(DWORD dwPos)
		{ m_dwPos = dwPos; Redraw(); }
	void SetPosPercentage(UINT uPercentage)
		{ SetPercentage(uPercentage); Redraw(); }
	void SetMinMaxPos(DWORD dwMin, DWORD dwMax, DWORD dwPos)
		{ m_dwMin = dwMin; m_dwMax = dwMax; m_dwPos = dwPos; Redraw(); }
	void SetMinMaxPosPercentage(DWORD dwMin, DWORD dwMax, UINT uPercentage)
		{ m_dwMin = dwMin; m_dwMax = dwMax; SetPercentage(uPercentage); Redraw(); }
	void Hide(void)
		{ if (!m_fIsHidden) { ShowWindow(SW_HIDE); m_fIsHidden = TRUE; } }
	void Show(void)
		{ if (m_fIsHidden) { ShowWindow(SW_SHOW); m_fIsHidden = FALSE; } }
	BOOL IsVisible(void)
		{ return !m_fIsHidden; }
	void Redraw(void)
		{ InvalidateRect(NULL, FALSE); UpdateWindow(); }

// Implementation
public:
	virtual ~CProgressBar();

protected:
	// Generated message map functions
	//{{AFX_MSG(CProgressBar)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL ParseText(LPSTR lpszText, LPSTR lpszFormat, int nFormatSize, LPDWORD lpdwMin, LPDWORD lpdwMax, LPDWORD lpdwPos);
	DWORD NumberUpToChar(LPSTR FAR *lplpszText, char cDelimeter);
	BOOL StringUpToChar(LPSTR FAR *lplpszText, LPSTR lpszBuffer, int nMaximum, char cDelimeter);
	DWORD GetPosition(DWORD dwNumber);
	UINT GetPercentage(void)
		{ return (UINT)GetPosition(100L); }
	void SetPercentage(UINT uPercentage);
	
protected:
	DWORD m_dwMin;						// minimum
	DWORD m_dwMax;						// maximum
	DWORD m_dwPos;						// current position
	char m_szFormat[128];			// format string
	COLORREF m_crColor;				// color
	DWORD m_dwStyle;					// control style bits
	BOOL m_fIsHidden;					// window is hidden
	
// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc ;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
private:
#ifdef WIN32
	static LONG CALLBACK FirstMsgOnlyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LONG lParam);
#else		
	static LONG CALLBACK __export FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
#endif

	BOOL m_fCreatedByClassName ;
	virtual void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////

#endif
