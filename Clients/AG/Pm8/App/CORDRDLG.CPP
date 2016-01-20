/*
// $Workfile: CORDRDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORDRDLG.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:40   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:50   JAY
// Reverted. New series.
// 
//    Rev 1.12   17 Nov 1994 14:30:20   JAY
//  
// 
//    Rev 1.11   11 Nov 1994 16:52:22   FRED
//  
// 
//    Rev 1.10   11 Nov 1994 14:42:54   JAY
// 
//    Rev 1.9   10 Nov 1994 17:45:18   JAY
//  
// 
//    Rev 1.8   10 Nov 1994 15:00:04   JAY
//  
// 
//    Rev 1.7   10 Nov 1994 08:09:28   JAY
//  
// 
//    Rev 1.6   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.5   09 Nov 1994 17:16:40   JAY
//  
// 
//    Rev 1.4   08 Nov 1994 18:27:58   FRED
//  
*/

// cordrdlg.cpp : implementation file
//

#include "stdafx.h"
#include "catalog.h"
#include "cataldoc.h"
#include "catalvw.h"

#include "cdlgitem.h"
#include "resource.h"
#include "phoneord.h"
#include "mailord.h"
#include "cordrdlg.h"
#include "cinstall.h"
#include "instdlg.h"
#include "codedlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderFormDialog dialog


COrderFormDialog::COrderFormDialog()
{
	//{{AFX_DATA_INIT(COrderFormDialog)
	m_fAllChecked = FALSE;
	m_fFontsChecked = FALSE;
	m_fGraphicsChecked = FALSE;
	m_fTaxChecked = FALSE;
	//}}AFX_DATA_INIT
}

void COrderFormDialog::DoDataExchange(CDataExchange* pDX)
{
	CItemDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderFormDialog)
	DDX_Control(pDX, IDC_TOTALHIGHLIGHT, m_StaticTotalHighlight);
	DDX_Control(pDX, IDC_TAXHILIGHT, m_StaticTaxHighlight);
	DDX_Control(pDX, IDC_GRAPHICSHIGHLIGHT, m_StaticGraphicsHighlight);
	DDX_Control(pDX, IDC_FONTSHIGHLIGHT, m_StaticFontsHighlight);
	DDX_Control(pDX, IDC_ALLHIGHLIGHT, m_StaticAllHighlight);
	DDX_Control(pDX, IDC_TOTAL, m_EditTotal);
	DDX_Control(pDX, IDC_TAXTOTAL, m_EditTaxTotal);
	DDX_Control(pDX, IDC_GRAPHICSTOTAL, m_EditGraphicsTotal);
	DDX_Control(pDX, IDC_FONTSTOTAL, m_EditFontsTotal);
	DDX_Control(pDX, IDC_ALLTOTAL, m_EditAllTotal);
	DDX_Check(pDX, IDC_ALL, m_fAllChecked);
	DDX_Check(pDX, IDC_FONTS, m_fFontsChecked);
	DDX_Check(pDX, IDC_GRAPHICS, m_fGraphicsChecked);
	DDX_Check(pDX, IDC_SALESTAX, m_fTaxChecked);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrderFormDialog, CItemDialog)
	//{{AFX_MSG_MAP(COrderFormDialog)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_BN_CLICKED(IDC_FONTS, OnFonts)
	ON_BN_CLICKED(IDC_GRAPHICS, OnGraphics)
	ON_BN_CLICKED(IDC_SALESTAX, OnSalestax)
	ON_BN_CLICKED(IDC_ORDERBYMAIL, OnOrderbymail)
	ON_BN_CLICKED(IDC_ORDERBYPHONE, OnOrderbyphone)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COrderFormDialog::UpdateDialog()
{
	UpdateData(TRUE);
	
	long lTotal = 0;
	
	UpdateItem(m_fGraphicsChecked, m_EditGraphicsTotal, m_StaticGraphicsHighlight, lTotal, 1995);
	UpdateItem(m_fFontsChecked, m_EditFontsTotal, m_StaticFontsHighlight, lTotal, 1995);
	UpdateItem(m_fAllChecked, m_EditAllTotal, m_StaticAllHighlight, lTotal, 2995);
	
	long lTax = ((lTotal*825)+5000)/10000;

	UpdateItem(m_fTaxChecked & lTax > 0, m_EditTaxTotal, m_StaticTaxHighlight, lTotal, lTax);
	UpdateItem(TRUE, m_EditTotal, m_StaticTotalHighlight, lTotal, lTotal);

	BOOL fEnable = m_fGraphicsChecked || m_fFontsChecked || m_fAllChecked;

	CWnd* pWnd;

	if ((pWnd = GetDlgItem(IDC_ORDERBYPHONE)) != NULL)
	{
		pWnd->EnableWindow(fEnable);
	}
	if ((pWnd = GetDlgItem(IDC_ORDERBYMAIL)) != NULL)
	{
		pWnd->EnableWindow(fEnable);
	}

}

void COrderFormDialog::UpdateItem(BOOL fSelected, CWnd& wndPrice, CWnd& wndHighlight, long& lTotal, long lPrice)
{
	if (fSelected)
	{
		wndHighlight.ShowWindow(SW_SHOW);
		wndPrice.ShowWindow(SW_SHOW);
		SetPrice(lPrice, &wndPrice);
		lTotal += lPrice;
	}
	else
	{
		wndHighlight.ShowWindow(SW_HIDE);
		wndPrice.ShowWindow(SW_HIDE);
	}
}

void COrderFormDialog::SetPrice(long lPrice, CWnd* pWnd, BOOL fDollarSign /*=TRUE*/, BOOL fBlankIfZero /*=FALSE*/)
{
	if (pWnd != NULL)
	{
		CString csPrice;
		PriceString(csPrice, lPrice, fDollarSign, fBlankIfZero);
		pWnd->SetWindowText(csPrice);
	}
}

void COrderFormDialog::PriceString(CString& csPrice, long lPrice, BOOL fDollarSign /*=TRUE*/, BOOL fBlankIfZero /*=FALSE*/)
{
	char szPrice[20];
		
	if (fBlankIfZero && (lPrice == 0))
	{
		*szPrice = '\0';
	}
		
	else
	{
		long lAbsPrice = (lPrice >= 0) ? lPrice : -lPrice;
			
		sprintf(szPrice, fDollarSign ? "$%ld.%02ld" : "%ld.%02ld", lPrice/100L, lAbsPrice % 100L);
	}
	
	csPrice = szPrice;
}

/////////////////////////////////////////////////////////////////////////////
// COrderFormDialog message handlers

BOOL COrderFormDialog::OnInitDialog()
{
	m_fGraphicsChecked = FALSE;
	m_fFontsChecked = FALSE;
	m_fAllChecked = FALSE;
	
	CItemDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COrderFormDialog::OnAll()
{
	CheckDlgButton(IDC_GRAPHICS, FALSE);
	CheckDlgButton(IDC_FONTS, FALSE);
	
	UpdateDialog();
}

void COrderFormDialog::OnFonts()
{
	CheckDlgButton(IDC_GRAPHICS, FALSE);
	CheckDlgButton(IDC_ALL, FALSE);
	
	UpdateDialog();
}

void COrderFormDialog::OnGraphics()
{
	CheckDlgButton(IDC_FONTS, FALSE);
	CheckDlgButton(IDC_ALL, FALSE);
	
	UpdateDialog();
}

void COrderFormDialog::OnSalestax()
{
	UpdateDialog();
}

void COrderFormDialog::OnOrderbymail()
{
	UpdateData(TRUE);

	COrderByMailInfo Info;

	Info.m_fAllChecked = m_fAllChecked;
	Info.m_fFontsChecked = m_fFontsChecked;
	Info.m_fGraphicsChecked = m_fGraphicsChecked;
	Info.m_fTaxChecked = m_fTaxChecked;

	COrderByMailDialog Dialog(&Info, this);
	Dialog.DoModal();
}

void COrderFormDialog::OnOrderbyphone()
{
	CInstallInfo Info;

	CCatalogView* pView = (CCatalogView*)GetParent();
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CCatalogView)));

	Info.m_pBookData = pView->GetDocument()->GetBookData();
	Info.m_fInstallPictures = m_fGraphicsChecked || m_fAllChecked;
	Info.m_fInstallFonts = m_fFontsChecked || m_fAllChecked;
	Info.m_fTaxChecked = m_fTaxChecked;

//	if (!Info.SetupDirectories())
//	{
//		return;
//	}

	COrderByPhoneDialog Dialog(&Info, this);
	Dialog.DoModal();
}

void COrderFormDialog::OnInstall()
{
	CInstallationData* pData = &(GetApp()->m_InstallationData);

/*
// Special intervention:
//
// See if the user has licensed anything yet.
// If not, put up a dialog which asks for a code up front instead of
// dumping them into the licensing dialog.
//
// This has two good points:
//
// (1) Assuming they have a license code to input, it takes them right to
//     where they need to be (i.e. the "Enter Code" dialog).
//
// (2) Assuming they do NOT have a license code to input, they get
//     information about how to GET one.
*/

	if (pData->LicensedProducts() == 0)
	{
	/*
	// Ask for an access code.
	*/
		CAccessCodeDialog Dialog(pData, this);

		if (Dialog.DoModal() != IDOK)
		{
		/* I guess the user doesn't have one... */
			return;
		}
	}

/*
// Show the installation dialog.
*/

	CInstallInfo Info;

	CCatalogView* pView = (CCatalogView*)GetParent();
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CCatalogView)));

	Info.m_pBookData = pView->GetDocument()->GetBookData();
	Info.m_pInstallationData = pData;

	CInstallDialog Dialog(&Info, this);

	if (Dialog.DoModal() == IDOK)
	{
		if (Info.SetupDirectories())
		{
		// Do the installation!
			CInstaller Installer;

			Installer.DoInstall(Info);
		}
	}
}
