/*
// $Workfile: PROPCTL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/PROPCTL.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.8   04 Oct 1996 15:30:18   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.7   01 Oct 1996 10:38:18   FRED
//  
// 
//    Rev 1.6   30 Aug 1996 09:19:38   FRED
// Fixed bad reg code crash
// 
//    Rev 1.5   26 Jun 1996 07:05:24   FRED
// International Registration
// 
//    Rev 1.4   22 Mar 1996 08:51:54   FRED
// Placeholders in MSREG.INI
// Mail registration form uses Configuration/Bonus from MSREG.INI
// 
//    Rev 1.3   14 Dec 1995 11:07:54   FRED
// Changes for VC++ 4.0
// 
//    Rev 1.2   06 Dec 1995 14:51:50   FRED
// 
//    Rev 1.1   06 Dec 1995 14:45:34   JAY
// Now works correctly with CTL3D, etc.
// 
//    Rev 1.1   13 Nov 1995 16:07:22   JAY
// Fixed text control color bug
// 
//    Rev 1.0   10 Nov 1995 09:41:16   FRED
// Initial revision.
// 
//    Rev 1.5   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.4   26 Oct 1995 10:27:54   FRED
//  
*/ 

#include "stdafx.h"

#include "msregx.h"
#include "propctl.h"

static UINT uTextFormats[] =
{
	DT_LEFT | DT_WORDBREAK,									// SS_LEFT
	DT_CENTER | DT_WORDBREAK,								// SS_CENTER
	DT_RIGHT | DT_WORDBREAK,								// SS_RIGHT
	(UINT)-1,													// SS_ICON
	(UINT)-1,													// SS_BLACKRECT
	(UINT)-1,													// SS_GRAYRECT
	(UINT)-1,													// SS_WHITERECT
	(UINT)-1,													// SS_BLACKFRAME
	(UINT)-1,													// SS_GRAYFRAME
	(UINT)-1,													// SS_WHITEFRAME
	(UINT)-1,													// Unknown (0x0a)
	DT_LEFT | DT_SINGLELINE | DT_TOP | DT_NOCLIP,	// SS_SIMPLE
	DT_LEFT | DT_SINGLELINE | DT_TOP,					// SS_LEFTNOWORDWRAP
	(UINT)-1,													// Unknown (0x0d)
	(UINT)-1,													// Unknown (0x0e)
	(UINT)-1														// Unknown (0x0f)
};

///////////////////////////////////////////////////////////
//
// CMlsPropertyControl
//

WNDPROC CMlsPropertyControl::m_pfnSuperWndProc = NULL;

CMlsPropertyControl::CMlsPropertyControl()
{
}

CMlsPropertyControl::~CMlsPropertyControl()
{
}

void CMlsPropertyControl::PostNcDestroy()
{
	delete this;
}

BOOL CheckForChoiceKey(const CString& csKey, LPCSTR pszPrefix, BOOL fChoice, CString& csValue)
{
	BOOL fResult = FALSE;

	TRY
	{
		int nKeyLength = csKey.GetLength();
		int nPrefixLength = strlen(pszPrefix);

		if (nKeyLength > nPrefixLength+1)
		{
			if ((strnicmp(csKey, pszPrefix, nPrefixLength) == 0)
			 && (csKey[nPrefixLength] == '?'))
			{
				int nIndex = csKey.Find(':');
				if (!fChoice)
				{
					if (nIndex == -1)
					{
						csValue.Empty();
					}
					else
					{
						csValue = csKey.Mid(nIndex+1);
					}
				}
				else
				{
					if (nIndex == -1)
					{
						csValue = csKey.Mid(nPrefixLength+1);
					}
					else
					{
						csValue = csKey.Mid(nPrefixLength+1, nIndex-(nPrefixLength+1));
					}
				}

				fResult = TRUE;
			}
		}
	}
	END_TRY

	return fResult;
}

BOOL CheckForPhoneNumberKey(const CString& csKey, LPCSTR pszName, CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, CString& csValue)
{
	BOOL fResult = FALSE;

	TRY
	{
		if (csKey.CompareNoCase(pszName) == 0)
		{
			csValue = GetApp()->MakePhoneNumber(pInternationalData, pPhoneNumber);
		}
	}
	END_TRY

	return fResult;
}
	
BOOL CheckForRegistrationCodeKey(const CString& csKey, CString& csValue)
{
	BOOL fResult = FALSE;

	TRY
	{
		if (csKey.CompareNoCase("RegistrationCode") == 0)
		{
			GetApp()->RegistrationProductCode(csValue);
			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}
	
BOOL CheckForDepartmentCodeKey(const CString& csKey, CString& csValue)
{
	BOOL fResult = FALSE;

	CString csRegistrationCode;
	CString csFormat;
	TRY
	{
		if (csKey.CompareNoCase("Department") == 0)
		{
			csFormat.LoadString(IDS_DepartmentName);
			GetApp()->RegistrationProductCode(csRegistrationCode);
			csValue.Format(csFormat, csRegistrationCode);
			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}
	
BOOL CheckForWebSiteKey(const CString& csKey, LPCSTR pszValue, CString& csValue)
{
	BOOL fResult = FALSE;

	TRY
	{
		if (csKey.CompareNoCase("WebSite") == 0)
		{
			if (pszValue != NULL)
			{
				csValue = pszValue;
			}
			else
			{
				GetApp()->GetWebSite(0, csValue);
			}
			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}
	
void PerformIniKeySubstitution(CString& csValue)
{
	CString csText;
	CString csKey;
	CString csKeyValue;
	CString csKeyNoShort;

	TRY
	{
		// Replace any placeholder in the INI file string:
		//
		//		@Key@ - Replaced with value of Configuration/Key
		//
		//			If Key does not exist, and Key ends with "Short",
		//			the we try Key without "Short" removed.
				
		static char cbDefault[] = "foobarbaz";
		static char cbShort[] = "Short";

		int nKey;
		
		csText.Empty();
		
		while ((nKey = csValue.Find('@')) != -1)
		{
			// Found starting @, get the key name that follows it.
			
			csText += csValue.Left(nKey);
			csValue = csValue.Mid(nKey+1);
			
			// Find the end of the key (trailing @)
			
			nKey = csValue.Find('@');
			
			if (nKey == -1)
			{
				// There is no trailing '@', don't do any further replacement.
				
				csText += '@';
				csText += csValue;
				csValue.Empty();
			}
			
			else
			{
				// Found the trailing @, get the key name.
				
				csKey = csValue.Left(nKey);
				csValue = csValue.Mid(nKey+1);
			
				//		csText has everything before starting @
				//		csKey has key name (without starting and trailing @'s)
				//		csValue has everything after trailing @
				
				// Attempt to look up the key in the [Configuration]
				// section of MSREG.INI
			
				csKeyValue = GetApp()->IniFile().GetString("Configuration", (LPCSTR)csKey, cbDefault);
				
				if (csKeyValue == cbDefault)
				{
					// The key was not in the [Configuration] section. If the key name
					// ends with "Short", then strip off the "Short" and try again.
					
					int nShort = csKey.Find(cbShort);
					
					if ((nShort > 0) && (nShort == (int)(csKey.GetLength()-strlen(cbShort))))
					{
						// Attempt to look up the "long" version of the
						// key in the [Configuration] section of MSREG.INI
						
						csKeyNoShort = csKey.Left(nShort);
						csKeyValue = GetApp()->IniFile().GetString("Configuration", (LPCSTR)csKeyNoShort, cbDefault);
					}
				}
			
				// Check for special keys.
				if (csKeyValue == cbDefault)
				{
					CInternationalData* pInternationalData = GetApp()->GetInternationalData();
					if (pInternationalData != NULL)
					{
						// We have international information, so we can do substitution on
						// keys which require the real international information.

						if (!CheckForChoiceKey(csKey, "FreeModem", pInternationalData->m_Registration.m_Modem.m_fFree, csKeyValue))
						{
							if (!CheckForChoiceKey(csKey, "FreePhone", pInternationalData->m_Registration.m_Phone.m_fFree, csKeyValue))
							{
								if (!CheckForChoiceKey(csKey, "FreeFax", pInternationalData->m_Registration.m_Fax.m_fFree, csKeyValue))
								{
									if (!CheckForPhoneNumberKey(csKey, "ModemNumber", pInternationalData, &(pInternationalData->m_Registration.m_Modem), csKeyValue))
									{
										if (!CheckForPhoneNumberKey(csKey, "PhoneNumber", pInternationalData, &(pInternationalData->m_Registration.m_Phone), csKeyValue))
										{
											if (!CheckForPhoneNumberKey(csKey, "FaxNumber", pInternationalData, &(pInternationalData->m_Registration.m_Fax), csKeyValue))
											{
												if (!CheckForWebSiteKey(csKey, pInternationalData->m_szWebSite, csKeyValue))
												{
													if (!CheckForRegistrationCodeKey(csKey, csKeyValue))
													{
														if (!CheckForDepartmentCodeKey(csKey, csKeyValue))
														{
															// No substitutions made.
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						// We don't have any international information, probably because we
						// have not yet asked the user where there are. There are some places
						// such as the congratulations dialog where certain substitutions are
						// still made with the default values.
						if (!CheckForWebSiteKey(csKey, NULL, csKeyValue))
						{
							// No substitutions made.
						}
					}
				}

				// Check if we finally have a value.
				if (csKeyValue == cbDefault)
				{
					// No, don't replace this key.
					csText += '@';
					csText += csKey;
					csText += '@';
				}
				else
				{
					// If the replacement value is empty, we 
					// may want to eat a space.
					if (csKeyValue.IsEmpty())
					{
						BOOL fTextSpace = (!csText.IsEmpty()) && (csText[csText.GetLength()-1] == ' ');
						BOOL fValueSpace = (!csValue.IsEmpty()) && (csValue[0] == ' ');

						if ((fValueSpace && fTextSpace)
						 || (fValueSpace && csText.IsEmpty()))
						{
							csValue = csValue.Mid(1);
						}
						if (fTextSpace && csValue.IsEmpty())
						{
							csText = csText.Left(csText.GetLength()-1);
						}
					}
					else
					{
						// Use the replacement value.
						csText += csKeyValue;
					}
				}
			}
		}
		
		csText += csValue;
		csValue = csText;
	}
	END_TRY
}

void CMlsPropertyControl::SubclassPropertyControls(CWnd* pWnd)
{
	CString csText;

	TRY
	{
		if (pWnd != NULL)
		{
			pWnd->GetWindowText(csText);
			PerformIniKeySubstitution(csText);
			pWnd->SetWindowText(csText);

			CWnd* pChild = pWnd->GetWindow(GW_CHILD);

			while (pChild != NULL)
			{
				CWnd* pNext = pChild->GetWindow(GW_HWNDNEXT);

			/* Process the child if it's a static control. */

				static const TCHAR szStatic[] = _T("static");
				TCHAR szCompare[sizeof(szStatic)+1];
				::GetClassName(pChild->GetSafeHwnd(), szCompare, sizeof(szCompare));

				if (lstrcmpi(szCompare, szStatic) == 0
						&& uTextFormats[pChild->GetStyle() & 0x0F] != (UINT)-1)
				{
					pChild->GetWindowText(csText);

					CMlsColorTextControl* pColorTextControl = new CMlsColorTextControl;

					if (pColorTextControl != NULL)
					{
						if (pColorTextControl->Properties()->ExtractProperties(csText))
						{
							int nID = pChild->GetDlgCtrlID();
							DWORD dwStyle = pChild->GetStyle();
							CRect crWindow;
							pChild->GetWindowRect(crWindow);
							pWnd->ScreenToClient(crWindow);

							pChild->DestroyWindow();

							pColorTextControl->Create(NULL,
															  "",
															  dwStyle,
															  crWindow,
															  pWnd,
															  nID);
							pChild = pColorTextControl;
						}
						else
						{
							delete pColorTextControl;
						}

						pColorTextControl = NULL;

						PerformIniKeySubstitution(csText);
						pChild->SetWindowText(csText);
					}
				}
				pChild = pNext;
			}
		}
	}
	END_TRY
}

///////////////////////////////////////////////////////////
//
// CMlsColorTextControl
//

BEGIN_MESSAGE_MAP(CMlsColorTextControl, CMlsPropertyControl)
	//{{AFX_MSG_MAP(CMlsColorTextControl)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

CMlsColorTextControl::CMlsColorTextControl()
{
	m_hBackgroundBrush = NULL;
}

CMlsColorTextControl::~CMlsColorTextControl()
{
	if (m_hBackgroundBrush != NULL)
	{
		::DeleteObject(m_hBackgroundBrush);
		m_hBackgroundBrush = NULL;
	}
}

HBRUSH CMlsColorTextControl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clText;
	Properties()->AsColor(0, clText, GetSysColor(COLOR_WINDOWTEXT));

	COLORREF clBackground;
	Properties()->AsColor(1, clBackground, GetSysColor(COLOR_WINDOW));

	if ((m_hBackgroundBrush == NULL) || (clBackground != m_clBackgroundBrush))
	{
		if (m_hBackgroundBrush != NULL)
		{
			::DeleteObject(m_hBackgroundBrush);
			m_hBackgroundBrush = NULL;
		}

		m_clBackgroundBrush = clBackground;
		m_hBackgroundBrush = ::CreateSolidBrush(m_clBackgroundBrush);
	}

	if (m_hBackgroundBrush != NULL)
	{
		if (GrayCtlColor(pDC->m_hDC, pWnd->GetSafeHwnd(), nCtlColor, m_hBackgroundBrush, clText))
		{
			return m_hBackgroundBrush;
		}
	}

	return NULL;
}

BOOL CMlsColorTextControl::OnChildNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
#ifdef WIN32
	if (uMsg == WM_CTLCOLORSTATIC)
	{
		if (pLResult != NULL)
		{
			return (*pLResult = (LRESULT)OnCtlColor(CDC::FromHandle((HDC)wParam), CWnd::FromHandle((HWND)lParam), CTLCOLOR_STATIC)) != NULL;
		}
	}
#else
	if (uMsg == WM_CTLCOLOR)
	{
		if (pLResult != NULL)
		{
			return (*pLResult = (LRESULT)(WORD)OnCtlColor(CDC::FromHandle((HDC)wParam), CWnd::FromHandle((HWND)LOWORD(lParam)), (int)HIWORD(lParam))) != NULL;
		}
	}
#endif

	return FALSE;
}

void CMlsColorTextControl::OnPaint() 
{
	DWORD dwStyle = GetStyle();
	UINT uFormat = uTextFormats[dwStyle & 0x0f];

	CPaintDC dc(this); // device context for painting

	CString csText;
	CRect crText;
	CBrush cbBackground;

	COLORREF clText;
	Properties()->AsColor(0, clText, GetSysColor(COLOR_WINDOWTEXT));

	COLORREF clBackground;
	Properties()->AsColor(1, clBackground, GetSysColor(COLOR_WINDOW));

	TRY
	{
		GetWindowText(csText);
		GetClientRect(crText);
		CFont* pFont = GetParent()->GetFont();

		COLORREF clOldText = dc.SetTextColor(clText);
		COLORREF clOldBackground = dc.SetBkColor(clBackground);

	/*
	// Draw the background.
	*/

		if (cbBackground.CreateSolidBrush(clBackground))
		{
			dc.FillRect(crText, &cbBackground);
		}

	/*
	// Draw the text.
	*/

		CFont* pOldFont = dc.SelectObject(pFont);
		if (pOldFont != NULL)
		{
			dc.DrawText(csText,
							csText.GetLength(),
							crText,
							uFormat);

			dc.SelectObject(pOldFont);
		}

		dc.SetBkColor(clOldBackground);
		dc.SetTextColor(clOldText);
	}
	END_TRY
	// Do not call CMlsPropertyControl::OnPaint() for painting messages
}

LRESULT CMlsColorTextControl::OnSetText(WPARAM, LPARAM)
{
	LRESULT lResult = Default();

	InvalidateRect(NULL);

	return lResult;
}
