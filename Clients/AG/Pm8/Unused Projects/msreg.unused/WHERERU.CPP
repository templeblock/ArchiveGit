// whereru.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
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
							csFormat.LoadString(IDS_RegistrationNotAvailable);
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
			AfxMessageBox(IDS_CantGetRegistrationInformation);
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
