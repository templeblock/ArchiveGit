// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//dlgrotim.cpp  :  CRotateImageDlg implementation

#include "pp.h"
#include "ppafx.h"
#include "ppmodal.h"
#include "dlgrotim.h"
#include "id.h"
#include "ipframe.h"
#include "ctlutil.h"
#include "bsrutil.h"
#include "afxcmn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CRotateImageDlg, CPPViewModalDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANGLE, OnDeltapos)
END_MESSAGE_MAP()

///////////////////////////////Construction and Initialization////////////////////////////
CRotateImageDlg::CRotateImageDlg(CServerView* pView, CWnd* pParent)
	:CPPViewModalDlg(pView, CRotateImageDlg::IDD, pParent)
{

}



BOOL CRotateImageDlg::OnInitDialog()
{
	PreDoModal();
	FRMDATATYPE Type = FDT_RGBCOLOR;
	ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &Type);
	CheckDlgButton(IDC_SMARTSIZE, m_bPrefSmartSize);
	CheckRadioButton(IDC_CWISE, IDC_CCWISE, IDC_CWISE + m_nPrefIsCCW);
	InitDlgItemSpinFixed( GetSafeHwnd(), IDC_ANGLE, m_Angle, NO, 0, TOFIXED( 360 ) );
	ControlEnable(GetSafeHwnd(), IDC_SMARTSIZE, CANDOTRANSPARENCY(Type));
	CenterWindow();

	if(GetDlgItem(IDC_ANGLE)->GetParent()!=this)
		GetDlgItem(IDC_ANGLE)->SetParent(this);
	

	return CPPViewModalDlg::OnInitDialog();
}


BOOL CRotateImageDlg::PreDoModal(LPVOID lpVoid)
{

	m_Angle = 0;
	m_bPrefSmartSize = (BOOL) ::GetDefaultInt("RotateImage_SmartSize", NO);
	m_nPrefIsCCW = ::GetDefaultInt("RotateImage_IsCCW", NO);

	return CPPViewModalDlg::PreDoModal();
}

LPVOID CRotateImageDlg::PostDoModal()
{

	::PutDefaultInt("RotateImage_SmartSize", m_bPrefSmartSize);
	::PutDefaultInt("RotateImage_IsCCW", m_nPrefIsCCW);

	return CPPViewModalDlg::PostDoModal();
}


///////////////////////////////Command Processing and Helpers////////////////////////////
BOOL CRotateImageDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();
	BOOL Bool;

	switch (id)
	{
	case IDC_SMARTSIZE:
		m_bPrefSmartSize = !m_bPrefSmartSize;
		CheckDlgButton(IDC_SMARTSIZE, m_bPrefSmartSize);
		break;

	case IDC_ANGLE:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	break;
		if ( codeNotify != EN_CHANGE )
	break;
		m_Angle = GetDlgItemSpinFixed( hDlg, IDC_ANGLE, &Bool, NO );
	break;

	case IDC_CWISE:
	case IDC_CCWISE:
		CheckRadioButton(IDC_CWISE, IDC_CCWISE, id );
		m_nPrefIsCCW = id - IDC_CWISE;
	break;

   default:
	break;
   }
	return CPPViewModalDlg::OnCommand(wParam, lParam);
}

void CRotateImageDlg::OnCancel()
{
	CPPViewModalDlg::OnCancel();
}

void CRotateImageDlg::OnOK()
{
	m_Angle = m_Angle - ((long)(m_Angle / TOFIXED(360)) * 360);
	if ( !m_Angle )
	{
		PostDoModal();
		OnCancel();
	}
	else
	{
		PostDoModal();
		CPPViewModalDlg::OnOK();
	}
}

void CRotateImageDlg::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPPModalDlg::OnDeltapos(pNMHDR, pResult);
}
