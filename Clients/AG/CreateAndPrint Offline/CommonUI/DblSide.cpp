#include "stdafx.h"
#include "DblSide.h"
#include "Print.h"
#include "AGDC.h"
#include "AGSym.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CDblSideIntro::CDblSideIntro(CPrint* pPrint)
{
	m_pPrint = pPrint;
}

//////////////////////////////////////////////////////////////////////
CDblSideIntro::~CDblSideIntro()
{
	DeleteObject(m_hStpFont);
	DeleteObject(m_hHdrFont);
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideIntro::OnSetActive()
{
	CWindow Parent(GetParent());
	Parent.CenterWindow();
	{
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		::GetObject(GetFont(), sizeof(lf), &lf);

		strcpy(lf.lfFaceName, "Arial"); //"Microsoft Sans Serif");
		lf.lfHeight = -14;
		m_hDlgFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_1).SetFont(m_hDlgFont);
		GetDlgItem(IDC_STATIC_2).SetFont(m_hDlgFont);

		lf.lfWeight = FW_SEMIBOLD;
		m_hHdrFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_WIZSTEP1_HDR).SetFont(m_hHdrFont);

		lf.lfHeight = -16;
		lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		m_hStpFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STEP1).SetFont(m_hStpFont);
	}
	SetWizardButtons(PSWIZB_NEXT);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideIntro::OnWizardNext()
{
	if (m_pPrint)
		m_pPrint->DoubleSidedTest(true);
	return true;
}

//////////////////////////////////////////////////////////////////////
CDblSideStep1::CDblSideStep1(CPrint* pPrint)
{
	m_pPrint = pPrint;
}

//////////////////////////////////////////////////////////////////////
CDblSideStep1::~CDblSideStep1()
{
	DeleteObject(m_hStpFont);
	DeleteObject(m_hHdrFont);
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideStep1::OnSetActive()
{
	{
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		::GetObject(GetFont(), sizeof(lf), &lf);

		strcpy(lf.lfFaceName, "Arial"); //"Microsoft Sans Serif");
		lf.lfHeight = -14;
		m_hDlgFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_3).SetFont(m_hDlgFont);
		GetDlgItem(IDC_STATIC_4).SetFont(m_hDlgFont);

		lf.lfWeight = FW_SEMIBOLD;
		m_hHdrFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_WIZSTEP2_HDR).SetFont(m_hHdrFont);

		lf.lfHeight = -16;
		lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		m_hStpFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STEP2).SetFont(m_hStpFont);
	}
	SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideStep1::OnWizardNext()
{
	if (m_pPrint)
		m_pPrint->DoubleSidedTest(false);
	return true;
}

//////////////////////////////////////////////////////////////////////
CDblSideStep2::CDblSideStep2(CPrint* pPrint)
{
	m_pPrint = pPrint;
	m_iDuplex = -1;
}

//////////////////////////////////////////////////////////////////////
CDblSideStep2::~CDblSideStep2()
{
	DeleteObject(m_hStpFont);
	DeleteObject(m_hOpt1Font);
}

//////////////////////////////////////////////////////////////////////
LRESULT CDblSideStep2::OnFrame(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long lStyle;

	if (m_iDuplex != -1)
	{
		HWND hWnd = GetDlgItem(m_iDuplex + IDC_FRAME1 - 1);
		lStyle = ::GetWindowLong(hWnd, GWL_STYLE);
		lStyle &= ~SS_BLACKFRAME;
		lStyle |= SS_ETCHEDFRAME;
		::SetWindowLong(hWnd, GWL_STYLE, lStyle);

		RECT r;
		::GetClientRect(hWnd, &r);
		POINT Pts[2];
		Pts[0].x = r.left;
		Pts[0].y = r.top;
		Pts[1].x = r.right;
		Pts[1].y = r.bottom;
		::MapWindowPoints(hWnd, m_hWnd, Pts, 2);
		r.left = Pts[0].x;
		r.top = Pts[0].y;
		r.right = Pts[1].x;
		r.bottom = Pts[1].y;
		InvalidateRect(&r);
	}
	else
		SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	lStyle = ::GetWindowLong(hWndCtl, GWL_STYLE);
	lStyle &= ~SS_ETCHEDFRAME;
	lStyle |= SS_BLACKFRAME;
	::SetWindowLong(hWndCtl, GWL_STYLE, lStyle);

	RECT r;
	::GetClientRect(hWndCtl, &r);
	POINT Pts[2];
	Pts[0].x = r.left;
	Pts[0].y = r.top;
	Pts[1].x = r.right;
	Pts[1].y = r.bottom;
	::MapWindowPoints(hWndCtl, m_hWnd, Pts, 2);
	r.left = Pts[0].x;
	r.top = Pts[0].y;
	r.right = Pts[1].x;
	r.bottom = Pts[1].y;
	InvalidateRect(&r);

	m_iDuplex = wID - IDC_FRAME1 + 1;

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideStep2::OnKillActive()
{
	m_Frame1.UnsubclassWindow();
	m_Frame2.UnsubclassWindow();
	m_Frame3.UnsubclassWindow();
	m_Frame4.UnsubclassWindow();
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideStep2::OnSetActive()
{
	m_Frame1.SubclassWindow(GetDlgItem(IDC_FRAME1));
	m_Frame2.SubclassWindow(GetDlgItem(IDC_FRAME2));
	m_Frame3.SubclassWindow(GetDlgItem(IDC_FRAME3));
	m_Frame4.SubclassWindow(GetDlgItem(IDC_FRAME4));

	{
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		::GetObject(GetFont(), sizeof(lf), &lf);

		strcpy(lf.lfFaceName, "Arial"); //"Microsoft Sans Serif");
		lf.lfHeight = -14;
		m_hDlgFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_5).SetFont(m_hDlgFont);
		GetDlgItem(IDC_STATIC_6).SetFont(m_hDlgFont);

		lf.lfHeight = -12;
		m_hOpt1Font = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_WIZSTEP3_OPT1).SetFont(m_hOpt1Font);
		GetDlgItem(IDC_WIZSTEP3_OPT2).SetFont(m_hOpt1Font);
		GetDlgItem(IDC_WIZSTEP3_OPT3).SetFont(m_hOpt1Font);
		GetDlgItem(IDC_WIZSTEP3_OPT4).SetFont(m_hOpt1Font);

		lf.lfHeight = -11;
		m_hDirFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_TOP_1).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_TOP_2).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_TOP_3).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_TOP_4).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_TOP_5).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_TOP_7).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_1).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_2).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_3).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_4).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_5).SetFont(m_hDirFont);
		GetDlgItem(IDC_STATIC_FRONT_6).SetFont(m_hDirFont);

		lf.lfHeight = -16;
		lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		m_hStpFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STEP3).SetFont(m_hStpFont);
	}
	if (m_iDuplex == -1)
		SetWizardButtons(PSWIZB_BACK);
	else
		SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideStep2::OnWizardNext()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
CDblSideEnd::CDblSideEnd(CPrint* pPrint)
{
	m_pPrint = pPrint;
	m_bFinished = false;
}

//////////////////////////////////////////////////////////////////////
CDblSideEnd::~CDblSideEnd()
{
	DeleteObject(m_hStpFont);
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideEnd::OnSetActive()
{
	{
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		::GetObject(GetFont(), sizeof(lf), &lf);

		strcpy(lf.lfFaceName, "Arial"); //"Microsoft Sans Serif");
		lf.lfHeight = -14;
		m_hDlgFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STATIC_7).SetFont(m_hDlgFont);
		GetDlgItem(IDC_STATIC_8).SetFont(m_hDlgFont);

		lf.lfHeight = -16;
		lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		m_hStpFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STEP4).SetFont(m_hStpFont);
	}
	SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CDblSideEnd::OnWizardFinish()
{
	m_bFinished = true;
	return true;
}

