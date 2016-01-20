/*
// $Workfile: REGISTER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGISTER.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.11   01 Oct 1996 10:39:04   FRED
//  
// 
//    Rev 1.10   26 Jun 1996 07:03:22   FRED
// International Registration
// 
//    Rev 1.9   11 Mar 1996 16:31:06   FRED
// New registration packet
// 
//    Rev 1.8   11 Mar 1996 15:34:32   FRED
// Printed registration form
// 
//    Rev 1.7   13 Dec 1995 12:34:36   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.6   12 Dec 1995 17:55:34   FRED
// Modem Only Version
// 
//    Rev 1.5   07 Dec 1995 14:05:40   FRED
// Added Version
// 
//    Rev 1.4   05 Dec 1995 13:02:36   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.3   28 Nov 1995 16:13:48   FRED
// No Free variations
// 
//    Rev 1.2   28 Nov 1995 08:46:30   FRED
// Phone Switch
// 
//    Rev 1.1   22 Nov 1995 10:26:06   FRED
// Merged in bonus changes from PMW version
// 
//    Rev 1.0   22 Nov 1995 08:52:02   FRED
// Initial revision.
// 
//    Rev 1.18   21 Nov 1995 12:12:26   FRED
// Fixed Email
// 
//    Rev 1.17   20 Nov 1995 16:25:36   FRED
// Removed Title, added Email
// 
//    Rev 1.16   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.15   01 Nov 1995 11:54:46   FRED
// Added Register by Phone
// 
//    Rev 1.14   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.13   31 Oct 1995 12:01:50   FRED
//  
// 
//    Rev 1.12   31 Oct 1995 10:07:18   JAY
// Now uses Windows directory bas default for _tempnam().
// 
//    Rev 1.11   31 Oct 1995 09:23:00   JAY
// Now uses _tempnam() instead of tmpnam().
// Now uses real data from dialog.
// 
//    Rev 1.10   30 Oct 1995 17:21:14   FRED
//  
// 
//    Rev 1.9   30 Oct 1995 15:17:54   JAY
// Removed congrats from by mail.
// 
//    Rev 1.8   30 Oct 1995 14:59:58   JAY
// More fixes.
// 
//    Rev 1.7   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.6   27 Oct 1995 16:13:20   FRED
// More communications stuff.
// 
//    Rev 1.5   26 Oct 1995 10:27:52   FRED
//  
*/ 

// register.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "register.h"
#include "reginfo.h"
#include "sysinfo.h"
#include "byphone.h"
#include "bymail.h"
#include "later.h"
#include "search.h"
#include "send.h"
#include "nomodem.h"
#include "callfail.h"
#include "cantreg.h"
#include "mspacket.h"
#include "util.h"
#include "msregini.h"
#include "propctl.h"

#ifdef NO_FREE_BONUS
#include "congrats.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINT CRegisterDialog::DialogIds[] =
{
	IDD_REGISTER_FOR_BONUS_E,
	IDD_REGISTER_EP,
	IDD_REGISTER_EM,
	IDD_REGISTER_EPM
};

/////////////////////////////////////////////////////////////////////////////
// CRegisterDialog dialog


CRegisterDialog::CRegisterDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CRegisterDialog::DialogId(), pParent)
{
	//{{AFX_DATA_INIT(CRegisterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegisterDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegisterDialog)
	ON_BN_CLICKED(IDC_REGISTER_NOW, OnRegisterNow)
	ON_BN_CLICKED(IDC_REGISTER_LATER, OnRegisterLater)
	ON_BN_CLICKED(IDC_REGISTER_BY_MAIL, OnRegisterByMail)
	ON_BN_CLICKED(IDC_REGISTER_BY_PHONE, OnRegisterByPhone)
	ON_BN_CLICKED(IDC_CANT_REGISTER, OnCantRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterDialog message handlers

BOOL CRegisterDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegisterDialog::OnRegisterNow() 
{
	CSearchingDialog SearchDialog(this);

	/* Perform the modem search. */

	switch (SearchDialog.DoModal())
	{
		case IDOK:
		{
			int nPort = SearchDialog.m_nPort;
			long lSpeed = SearchDialog.m_lSpeed;
			BOOL fRetryRegister;

			do
			{
				fRetryRegister = FALSE;

				CRegistrationInformationDialog Dialog((UINT)-1, this);

				if (Dialog.DoModal() == IDOK)
				{
					CSystemInformationDialog SystemDialog(this);

					if (SystemDialog.DoModal() != IDOK)
					{
						fRetryRegister = TRUE;
					}
					else
					{
						CMsregPacketRegistrationRequest* pRegistrationData = new CMsregPacketRegistrationRequest;

						pRegistrationData->ProductName() = GetApp()->ApplicationName();

						pRegistrationData->AddField("Version", GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Version));
						pRegistrationData->AddField("ParentPartNumber", GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber));
						pRegistrationData->AddField("FirstName", Dialog.m_csFirstName);
						pRegistrationData->AddField("LastName", Dialog.m_csLastName);
						pRegistrationData->AddField("Company", Dialog.m_csCompany);
						pRegistrationData->AddField("Title", "");
						pRegistrationData->AddField("Address1", Dialog.m_csAddress1);
						pRegistrationData->AddField("Address2", Dialog.m_csAddress2);
						pRegistrationData->AddField("City", Dialog.m_csCity);
						pRegistrationData->AddField("Zip", Dialog.m_csZip);
						pRegistrationData->AddField("State", Dialog.m_csState);
						pRegistrationData->AddField("Country", Dialog.m_csCountry);
						pRegistrationData->AddField("Phone", Dialog.m_csPhone);
						pRegistrationData->AddField("Email", Dialog.m_csEmail);
						pRegistrationData->AddField("OfferCode", GetApp()->RegistrationProductCode());
						pRegistrationData->AddField("SystemInformation", SystemDialog.m_fIncludeInformation ? SystemDialog.m_csSystemInformation : "");

						CMsregPacketHeader PacketHeader;

						PacketHeader.CustomerId() = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CustomerNumber);
						PacketHeader.AddPacket(pRegistrationData);
						pRegistrationData = NULL;

						BOOL fSuccess = FALSE;
						CString csFileName;
						CFile cfFile;
						CString csWindowsDir;

						TRY
						{
							if (GetWindowsDirectory(csWindowsDir.GetBuffer(_MAX_PATH), _MAX_PATH) == 0)
							{
								csWindowsDir.Empty();
							}
							else
							{
								csWindowsDir.ReleaseBuffer();
							}

							char* pName = _tempnam((LPSTR)(LPCSTR)csWindowsDir, "REG");
							if (pName != NULL)
							{
								csFileName = pName;
								free(pName);

								if (cfFile.Open(csFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
								{
									PacketHeader.WriteToFile(cfFile);
									fSuccess = TRUE;
								}
							}
						}
						END_TRY

						cfFile.Close();

						if (fSuccess)
						{
							CSendingDialog SendingDialog(nPort, lSpeed, csFileName, this);

							switch (SendingDialog.DoModal())
							{
								case IDOK:
								{
								// The request was successfully transmitted, we should have
								// a response. Check if the user is registered.

									TRY
									{
										if (!cfFile.Open(csFileName, CFile::modeRead|CFile::shareExclusive))
										{
											AfxThrowUserException();
										}

										PacketHeader.ReadFromFile(cfFile);

										CMsregPacketRegistrationResponse* pResponse;

										if ((PacketHeader.Type() != CMsregPacket::BLOCK_PacketHeader)
										 || (PacketHeader.Version() != CMsregPacket::VERSION_PacketHeader)
										 || (PacketHeader.PacketCount() != 1)
										 || ((pResponse = (CMsregPacketRegistrationResponse*)(PacketHeader.GetPacket(0))) == NULL)
										 || (pResponse->Type() != CMsregPacket::BLOCK_RegistrationResponse)
										 || (pResponse->Version() != CMsregPacket::VERSION_RegistrationResponse))
										{
											AfxThrowUserException();
										}

										// Got a response, see if user is registered.

										if (pResponse->Registered() == 0)
										{
											AfxThrowUserException();
										}

										// Got a response, see if user is registered.

										if (pResponse->Registered() == 0)
										{
											AfxThrowUserException();
										}

										// Remember their customer ID.
										GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CustomerNumber, PacketHeader.CustomerId());

#ifndef NO_FREE_BONUS
										// Bonus enable the user.
										GetApp()->BonusEnableUser(MODEM_REGISTRATION_CODE, this);
										ASSERT(GetApp()->UserIsBonusEnabled());
										ASSERT(GetApp()->UserIsRegistered());
#else
										// Mark the user as registered.
										GetApp()->RegisterUser();
										ASSERT(GetApp()->UserIsRegistered());

										// Thank the user!
										CCongratulationsDialog CongratsDialog(this);
										CongratsDialog.DoModal();
#endif

										EndDialog(IDOK);
									}
									CATCH_ALL(e)
									{
										CCallFailedDialog Dialog(this);
										Dialog.DoModal();

										fRetryRegister = TRUE;
									}
									END_CATCH_ALL

									cfFile.Close();

									break;
								}
								case IDCANCEL:
								{
								// They want to go back.
								// We just go back to the register dialog.
									fRetryRegister = TRUE;
									break;
								}
								case IDABORT:
								{
									CCallFailedDialog Dialog(this);
									Dialog.DoModal();

									fRetryRegister = TRUE;
									break;
								}
								default:
								{
									ASSERT(FALSE);
									break;
								}
							}
						}

						TRY
						{
							CFile::Remove(csFileName);
						}
						END_TRY
					}
				}
			} while (fRetryRegister);

			break;
		}
		case IDABORT:
		{
			CNoModemDialog Dialog(this);
			Dialog.DoModal();
			break;
		}
	}
}

void CRegisterDialog::OnRegisterByPhone() 
{
	CRegisterByPhoneDialog Dialog(this);

	if (Dialog.DoModal() == IDOK)
	{
		EndDialog(IDOK);
	}
}

void CRegisterDialog::OnRegisterByMail() 
{
	CRegisterByMailFormDialog Dialog(this);
	if (Dialog.DoModal() == IDOK)
	{
		EndDialog(IDOK);
	}
}

void CRegisterDialog::OnRegisterLater() 
{
	CRegisterLaterDialog Dialog(this);

	if (Dialog.DoModal() == IDOK)
	{
		EndDialog(IDOK);
	}
}

void CRegisterDialog::OnCantRegister() 
{
	CCantRegisterDialog Dialog(this);

	if (Dialog.DoModal() == IDOK)
	{
		EndDialog(IDOK);
	}
}
