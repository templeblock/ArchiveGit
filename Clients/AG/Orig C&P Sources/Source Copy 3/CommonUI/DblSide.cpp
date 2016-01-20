//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "DblSide.h"
#include "AGDC.h"
#include "AGSym.h"

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static void DrawArrow(CAGDC* pDC, POINT Pt)
{
	POINT Pts[7];
	Pts[0] = Pt;
	Pts[1].x = Pts[0].x - (APP_RESOLUTION / 2);
	Pts[1].y = Pts[0].y + (APP_RESOLUTION / 2);
	Pts[2].x = Pts[1].x + (APP_RESOLUTION / 4);
	Pts[2].y = Pts[1].y;
	Pts[3].x = Pts[2].x;
	Pts[3].y = Pts[2].y + (APP_RESOLUTION / 2);
	Pts[4].x = Pts[3].x + (APP_RESOLUTION / 2);
	Pts[4].y = Pts[3].y;
	Pts[5].x = Pts[4].x;
	Pts[5].y = Pts[4].y - (APP_RESOLUTION / 2);
	Pts[6].x = Pts[5].x + (APP_RESOLUTION / 4);
	Pts[6].y = Pts[5].y;

	HBRUSH hOldBrush = (HBRUSH)::SelectObject(pDC->GetHDC(), ::GetStockObject(NULL_BRUSH));
	HPEN hPen = ::CreatePen(PS_SOLID, pDC->GetDeviceInfo().m_nLogPixelsX / 50, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN) ::SelectObject(pDC->GetHDC(), hPen);

	pDC->Polygon(Pts, 7);

	::SelectObject(pDC->GetHDC(), hOldBrush);
	::SelectObject(pDC->GetHDC(), hOldPen);
	::DeleteObject(hPen);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static void PrintTest(bool bFirstTest, char* pszDriver, char* pszDevice, char* pszOutput, DEVMODE* pDevMode)
{
	short nSaveCopies = -1;

	if (pDevMode)
	{
		pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		pDevMode->dmFields |= DM_ORIENTATION;

		if (pDevMode->dmCopies > 1)
		{
			nSaveCopies = pDevMode->dmCopies;
			pDevMode->dmCopies = 1;
		}
	}

	CAGDC* pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
	CAGDCInfo di = pDC->GetDeviceInfo();

	if (pDevMode && pDevMode->dmOrientation == DMORIENT_PORTRAIT && di.m_nHorzSize > di.m_nVertSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}
	else
	if (pDevMode && pDevMode->dmOrientation == DMORIENT_LANDSCAPE && di.m_nVertSize > di.m_nHorzSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}

	if (!pDC->StartDoc("Create and Print"))
	{
		delete pDC;
		if (pDevMode && nSaveCopies != -1)
			pDevMode->dmCopies = nSaveCopies;
		return;
	}

	if (!pDC->StartPage())
	{
		pDC->AbortDoc();
		delete pDC;
		if (pDevMode && nSaveCopies != -1)
			pDevMode->dmCopies = nSaveCopies;
		return;
	}

	if (bFirstTest)
	{
		POINT Pt = { di.m_PhysPageSize.cx / 2, di.m_nLogPixelsY };
		pDC->DPAtoVPA(&Pt, 1);
		DrawArrow(pDC, Pt);

		char szMsg[] = "Please put this page back into the printer\n\n"
			"with the printed side UP\n\n"
			"and the arrow pointing TOWARD the printer.";

		int nSpecOffset = 0;
		RECT MsgRect = { 0, di.m_nLogPixelsY * 3, di.m_PhysPageSize.cx, di.m_nLogPixelsY * 6 };
		pDC->DPAtoVPA(&MsgRect);

		LOGFONT MsgFont;
		memset(&MsgFont, 0, sizeof(MsgFont));
		lstrcpy(MsgFont.lfFaceName, "Arial");
		MsgFont.lfHeight = -(14 * APP_RESOLUTION / 72);
		CAGSpec MsgSpec(MsgFont, RGB(0, 0, 0), eRagCentered);

		CAGSymText* pAGSymText = new CAGSymText();
		pAGSymText->Create(MsgRect);
		pAGSymText->SetVertJust(eVertTop);
		pAGSymText->SetText(szMsg, lstrlen(szMsg), 1, &MsgSpec, &nSpecOffset);
		pAGSymText->Draw(*pDC);
		delete pAGSymText;
	}
	else
	{
		POINT Pt = { di.m_PhysPageSize.cx / 4, di.m_nLogPixelsY };
		pDC->DPAtoVPA(&Pt, 1);
		DrawArrow(pDC, Pt);

		Pt.x = (di.m_PhysPageSize.cx / 4) * 3;
		Pt.y = di.m_nLogPixelsY;
		pDC->DPAtoVPA(&Pt, 1);
		DrawArrow(pDC, Pt);
	}

	if (!pDC->EndPage())
	{
		pDC->AbortDoc();
		delete pDC;
		if (pDevMode && nSaveCopies != -1)
			pDevMode->dmCopies = nSaveCopies;
		return;
	}

	if (!pDC->EndDoc())
	{
		pDC->AbortDoc();
		delete pDC;
		if (pDevMode && nSaveCopies != -1)
			pDevMode->dmCopies = nSaveCopies;
		return;
	}

	delete pDC;

	if (pDevMode && nSaveCopies != -1)
		pDevMode->dmCopies = nSaveCopies;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CDblSideIntro::CDblSideIntro()
{
	m_pszDriver = NULL;
	m_pszDevice = NULL;
	m_pszOutput = NULL;
	m_pDevMode = NULL;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideIntro::OnSetActive()
{
	CWindow Parent(GetParent());
	Parent.CenterWindow();

	SetWizardButtons(PSWIZB_NEXT);
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideIntro::OnWizardNext()
{
	PrintTest(true, m_pszDriver, m_pszDevice, m_pszOutput, m_pDevMode);
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CDblSideStep1::CDblSideStep1()
{
	m_pszDriver = NULL;
	m_pszDevice = NULL;
	m_pszOutput = NULL;
	m_pDevMode = NULL;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideStep1::OnSetActive()
{
	SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideStep1::OnWizardNext()
{
	PrintTest(false, m_pszDriver, m_pszDevice, m_pszOutput, m_pDevMode);
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CDblSideStep2::OnFrame(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long lStyle;

	if (m_nSelected != -1)
	{
		HWND hWnd = GetDlgItem(m_nSelected + IDC_FRAME1 - 1);
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

	m_nSelected = wID - IDC_FRAME1 + 1;

	bHandled = true;
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideStep2::OnKillActive()
{
	m_Frame1.UnsubclassWindow();
	m_Frame2.UnsubclassWindow();
	m_Frame3.UnsubclassWindow();
	m_Frame4.UnsubclassWindow();
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideStep2::OnSetActive()
{
	m_Frame1.SubclassWindow(GetDlgItem(IDC_FRAME1));
	m_Frame2.SubclassWindow(GetDlgItem(IDC_FRAME2));
	m_Frame3.SubclassWindow(GetDlgItem(IDC_FRAME3));
	m_Frame4.SubclassWindow(GetDlgItem(IDC_FRAME4));

	if (m_nSelected == -1)
		SetWizardButtons(PSWIZB_BACK);
	else
		SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideStep2::OnWizardNext()
{
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideEnd::OnSetActive()
{
	SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
BOOL CDblSideEnd::OnWizardFinish()
{
	m_bFinished = true;
	return true;
}

