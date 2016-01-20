#ifndef __DBLSIDE_H_
#define __DBLSIDE_H_

#include "propsht.h"
#include "resource.h"

class CSelectFrame : public CWindowImpl<CSelectFrame>
{
public:

BEGIN_MSG_MAP (CSelectFrame)
	MESSAGE_HANDLER (WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER (WM_NCHITTEST, OnNCHitTest)
END_MSG_MAP ()

public:
	LRESULT OnLButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & bHandled)
	{
		::SendMessage (GetParent (), WM_COMMAND,
		  MAKEWPARAM (GetDlgCtrlID (), BN_CLICKED), (LPARAM) m_hWnd);
		bHandled = TRUE;
		return (TRUE);
	}

	LRESULT OnNCHitTest (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & bHandled)
	{
		bHandled = TRUE;
		return (HTCLIENT);
	}
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideIntro
class CDblSideIntro : 
	public CPropertyPageImpl<CDblSideIntro>
{
public:
	enum { IDD = IDD_DBLSIDEINTRO };

	BOOL OnSetActive ();
	BOOL OnWizardNext ();

public:
	char	*m_pszDriver;
	char	*m_pszDevice;
	char	*m_pszOutput;
	DEVMODE	*m_pDevMode;
};


/////////////////////////////////////////////////////////////////////////////
// CDblSideStep1
class CDblSideStep1 : 
	public CPropertyPageImpl<CDblSideStep1>
{
public:
	enum { IDD = IDD_DBLSIDESTEP1 };
	
	BOOL OnSetActive ();
	BOOL OnWizardNext ();

public:
	char	*m_pszDriver;
	char	*m_pszDevice;
	char	*m_pszOutput;
	DEVMODE	*m_pDevMode;
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideStep2
class CDblSideStep2 : 
	public CPropertyPageImpl<CDblSideStep2>
{
public:
	enum { IDD = IDD_DBLSIDESTEP2 };

	CDblSideStep2 ()
		{ m_nSelected = -1; }

	BEGIN_MSG_MAP (CDblSideStep2)
		COMMAND_HANDLER (IDC_FRAME1, BN_CLICKED, OnFrame)
		COMMAND_HANDLER (IDC_FRAME2, BN_CLICKED, OnFrame)
		COMMAND_HANDLER (IDC_FRAME3, BN_CLICKED, OnFrame)
		COMMAND_HANDLER (IDC_FRAME4, BN_CLICKED, OnFrame)
		CHAIN_MSG_MAP(CPropertyPageImpl<CDblSideStep2>)
	END_MSG_MAP()

	int GetSelected ()
		{ return (m_nSelected); }
	LRESULT OnFrame (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	BOOL OnKillActive ();
	BOOL OnSetActive ();
	BOOL OnWizardNext ();

protected:
	int				m_nSelected;
	CSelectFrame	m_Frame1;
	CSelectFrame	m_Frame2;
	CSelectFrame	m_Frame3;
	CSelectFrame	m_Frame4;
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideEnd
class CDblSideEnd : 
	public CPropertyPageImpl<CDblSideEnd>
{
public:
	enum { IDD = IDD_DBLSIDEEND };

	CDblSideEnd ()
		{ m_bFinished = false; }

	bool IsFinished ()
		{ return (m_bFinished); }
	BOOL OnSetActive ();
	BOOL OnWizardFinish ();

protected:
	bool m_bFinished;
};

#endif //__DBLSIDE_H_
