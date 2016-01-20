#pragma once

#include "PropertyPage.h"
#include "Print.h"
#include "Resource.h"

class CSelectFrame : public CWindowImpl<CSelectFrame>
{
public:
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_hWnd);
		return TRUE;
	}

	LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return HTCLIENT;
	}

public:
	BEGIN_MSG_MAP(CSelectFrame)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
	END_MSG_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideIntro
class CDblSideIntro : public CPropertyPageImpl<CDblSideIntro>
{
public:
	enum { IDD = IDD_DBLSIDEINTRO };

	CDblSideIntro(CPrint* pPrint);
	~CDblSideIntro();
	BOOL OnSetActive();
	BOOL OnWizardNext();

protected:
	CPrint* m_pPrint;
	HFONT m_hStpFont;
	HFONT m_hHdrFont;
	HFONT m_hDlgFont;
};


/////////////////////////////////////////////////////////////////////////////
// CDblSideStep1
class CDblSideStep1 : public CPropertyPageImpl<CDblSideStep1>
{
public:
	enum { IDD = IDD_DBLSIDESTEP1 };
	
	CDblSideStep1(CPrint* pPrint);
	~CDblSideStep1();
	BOOL OnSetActive();
	BOOL OnWizardNext();

protected:
	CPrint* m_pPrint;
	HFONT m_hStpFont;
	HFONT m_hHdrFont;
	HFONT m_hDlgFont;
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideStep2
class CDblSideStep2 : public CPropertyPageImpl<CDblSideStep2>
{
public:
	enum { IDD = IDD_DBLSIDESTEP2 };

	CDblSideStep2(CPrint* pPrint);
	~CDblSideStep2();
	int GetDuplexCode()
		{ return (m_iDuplex); }
	LRESULT OnFrame(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	BOOL OnKillActive();
	BOOL OnSetActive();
	BOOL OnWizardNext();

public:
	BEGIN_MSG_MAP(CDblSideStep2)
		COMMAND_HANDLER(IDC_FRAME1, BN_CLICKED, OnFrame)
		COMMAND_HANDLER(IDC_FRAME2, BN_CLICKED, OnFrame)
		COMMAND_HANDLER(IDC_FRAME3, BN_CLICKED, OnFrame)
		COMMAND_HANDLER(IDC_FRAME4, BN_CLICKED, OnFrame)
		CHAIN_MSG_MAP(CPropertyPageImpl<CDblSideStep2>)
	END_MSG_MAP()

protected:
	CPrint* m_pPrint;
	int m_iDuplex;
	CSelectFrame m_Frame1;
	CSelectFrame m_Frame2;
	CSelectFrame m_Frame3;
	CSelectFrame m_Frame4;
	HFONT m_hStpFont;
	HFONT m_hOpt1Font;
	HFONT m_hDlgFont;
	HFONT m_hDirFont;
};

/////////////////////////////////////////////////////////////////////////////
// CDblSideEnd
class CDblSideEnd : public CPropertyPageImpl<CDblSideEnd>
{
public:
	enum { IDD = IDD_DBLSIDEEND };

	CDblSideEnd(CPrint* pPrint);
	~CDblSideEnd();
	bool IsFinished()
		{ return (m_bFinished); }
	BOOL OnSetActive();
	BOOL OnWizardFinish();

protected:
	CPrint* m_pPrint;
	bool m_bFinished;
	HFONT m_hStpFont;
	HFONT m_hDlgFont;
};
