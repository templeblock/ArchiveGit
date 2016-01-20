// whereru.cpp : implementation file
//

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/WHERERU.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:10   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:42:04   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

#include "stdafx.h"
#include "upgrade.h"
#include "msregini.h"
#include "util.h"
#include "propctl.h"
#include "whereru.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhereAreYouDialog dialog


CWhereAreYouDialog::CWhereAreYouDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CWhereAreYouDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhereAreYouDialog)
	//}}AFX_DATA_INIT

	m_pInternationalData = NULL;
}


void CWhereAreYouDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhereAreYouDialog)
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		// Delete any exiting parameters.
		delete m_pInternationalData;
		m_pInternationalData = NULL;

		// Get the current country.
		WORD wCountryId = m_CountryList.GetCountryId();
		if (wCountryId != 0)
		{
			// Save the selection.
			GetApp()->UserIniFile().WriteInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CallingFrom, wCountryId);
			
			// Get the registration parameters for the country.
			TRY
			{
				m_pInternationalData = new CInternationalData;
			}
			END_TRY
			if (m_pInternationalData != NULL)
			{
				if (!GetApp()->GetInternationalData(wCountryId, m_pInternationalData))
				{
					delete m_pInternationalData;
					m_pInternationalData = NULL;
				}

				// Make sure we can at least register by modem and mail.
				if (m_pInternationalData != NULL)
				{
					if (!m_pInternationalData->m_Registration.m_Modem.Exists()
					 || !m_pInternationalData->m_Registration.m_Mail.Exists())
					{
						CString csCountry;
						CString csFormat;
						CString csMessage;
						TRY
						{
							GetApp()->GetCountryName(wCountryId, csCountry);
							csFormat = "Registration from %s is not currently available";
							csMessage.Format(csFormat, (LPCSTR)csCountry);
							AfxMessageBox(csMessage);
						}
						END_TRY

						delete m_pInternationalData;
						m_pInternationalData = NULL;

						AfxThrowUserException();
					}
				}
			}
		}

		if (m_pInternationalData == NULL)
		{
			AfxMessageBox("Can't get registration parameters for selected country.");
			AfxThrowUserException();
		}
	}
}


BEGIN_MESSAGE_MAP(CWhereAreYouDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CWhereAreYouDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhereAreYouDialog message handlers

BOOL CWhereAreYouDialog::OnInitDialog() 
{
	m_CountryList.SubclassDlgItem(IDC_COUNTRY_LIST, this);

	// Restore any previous calling location selection.
	m_CountryList.Fill(GetApp()->UserIniFile().GetInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CallingFrom, 0));
	
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
