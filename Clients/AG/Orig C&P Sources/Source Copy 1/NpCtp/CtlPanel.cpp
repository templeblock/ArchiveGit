//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "CtlPanel.h"
#include "Ctp.h"
#include "WaitDlg.h"
#include "AGPage.h"
#include "DblSide.h"

#define DUPLEX_SUBKEY	"Software\\American Greetings\\Create and Print\\Duplex Printing"

//--------------------------------------------------------------------------//
//	   Duplex 1       Duplex 2       Duplex 3       Duplex 4				//
//	  Front  Back    Front  Back    Front  Back    Front  Back				//
//																			//
//	  ^	^ ^			   ^			  ^	   ^  ^		 ^						//
//	  |	| |			   |  			  |	   |  |		 |						//
//					 |	 |								  |	 |				//
//					 v	 v								  v	 v				//
//																			//
//																			//
// 0 = toward   1 = away from												//
//--------------------------------------------------------------------------//

const static int PaperDirection[2][2][2] = {
{				// duplex type 2 or 4
	{ 0, 1 },	// half-fold vertical { not rotated, rotated }
	{ 1, 0 },	// half-fold horizontal { not rotated, rotated }
},
{				// duplex type 1 or 3
	{ 1, 0 },	// half-fold vertical { not rotated, rotated }
	{ 0, 1 },	// half-fold horizontal { not rotated, rotated }
}
};

#define MAX_COLORS	23
const static COLORREF PickColor[MAX_COLORS] = {
	RGB (0,   0,   0  ),
	RGB (255, 255, 255),
	RGB (255, 204, 204),
	RGB (255, 153, 153),
	RGB (255, 51,  153),
	RGB (255, 0,   0  ),
	RGB (204, 0,   0  ),
	RGB (255, 102, 0  ),
	RGB (255, 204, 153),
	RGB (153, 51,  0  ),
	RGB (255, 255, 0  ),
	RGB (255, 204, 0  ),
	RGB (153, 255, 153),
	RGB (0,   255, 0  ),
	RGB (0,   102, 0  ),
	RGB (51,  153, 153),
	RGB (0,   255, 255),
	RGB (102, 153, 204),
	RGB (0,   0,   255),
	RGB (0,   0,   153),
	RGB (255, 153, 255),
	RGB (255, 0,   255),
	RGB (153, 0,   153)
};

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static void DeleteRegistryDuplex (const char *pszDevice)
{
	HKEY hKey;
	if (::RegOpenKeyEx (HKEY_LOCAL_MACHINE, DUPLEX_SUBKEY, 0, KEY_WRITE, &hKey)
	  == ERROR_SUCCESS)
	{
		::RegDeleteValue (hKey, pszDevice);
		::RegCloseKey (hKey);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static bool GetRegistryDuplex (const char *pszDevice, int &nDuplex)
{
	bool bFound = false;

	HKEY hKey;
	if (::RegOpenKeyEx (HKEY_LOCAL_MACHINE, DUPLEX_SUBKEY, 0, KEY_QUERY_VALUE,
	  &hKey) == ERROR_SUCCESS)
	{
		char szDuplex[10];
		DWORD dwType;
		DWORD dwSize = sizeof (szDuplex);
		if (::RegQueryValueEx (hKey, pszDevice, 0, &dwType, (BYTE *) szDuplex,
		  &dwSize) == ERROR_SUCCESS)
		{
			if (dwType == REG_SZ)
			{
				nDuplex = atoi (szDuplex);
				bFound = true;
			}
		}
		::RegCloseKey (hKey);
	}

	return (bFound);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static void SetRegistryDuplex (const char *pszDevice, int nDuplex)
{
	HKEY hKey;
	DWORD dw;
	if (::RegCreateKeyEx (HKEY_LOCAL_MACHINE, DUPLEX_SUBKEY, 0, REG_NONE,
	  REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw) == ERROR_SUCCESS)
	{
		char szDuplex[10];
		_itoa (nDuplex, szDuplex, 10);
		::RegSetValueEx (hKey, pszDevice, 0, REG_SZ, (BYTE *) szDuplex,
		  lstrlen (szDuplex) + 1);
		::RegCloseKey (hKey);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static UINT APIENTRY PrintHookProc (HWND hWnd, UINT msg, UINT wParam,
  LONG lParam)
{
	static bool *pbSingleFold = NULL;

	switch (msg)
	{
		case WM_INITDIALOG:
		{
			PRINTDLG *ppd = (PRINTDLG *) lParam;
			pbSingleFold = (bool *) ppd->lCustData;
			if (*pbSingleFold)
			{
				::SendDlgItemMessage (hWnd, IDC_SINGLEFOLD, BM_SETCHECK,
				  BST_CHECKED, 0);
			}
			else
			{
				::SendDlgItemMessage (hWnd, IDC_QUARTERFOLD, BM_SETCHECK,
				  BST_CHECKED, 0);
			}

			int n = ::SendDlgItemMessage (hWnd, cmb4, CB_GETCURSEL, 0, 0);
			if (n >= 0)
			{
				char szDevice[100];
				::SendDlgItemMessage (hWnd, cmb4, CB_GETLBTEXT, n,
				  (LPARAM) szDevice);

				int nDuplex;
				::ShowWindow (::GetDlgItem (hWnd, IDC_DBLSIDE),
				  GetRegistryDuplex (szDevice, nDuplex));
			}
			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD (wParam))
			{
				case IDOK:
				{
					if (::SendDlgItemMessage (hWnd, IDC_SINGLEFOLD, BM_GETCHECK,
					  0, 0) == BST_CHECKED)
					{
						*pbSingleFold = true;
					}
					else
					{
						*pbSingleFold = false;
					}

					if (::IsWindowVisible (::GetDlgItem (hWnd, IDC_DBLSIDE)) &&
					  ::IsWindowEnabled (::GetDlgItem (hWnd, IDC_DBLSIDE)) &&
					  ::SendDlgItemMessage (hWnd, IDC_DBLSIDE, BM_GETCHECK,
					  0, 0) == BST_CHECKED)
					{
						int n = ::SendDlgItemMessage (hWnd, cmb4, CB_GETCURSEL,
						  0, 0);
						if (n >= 0)
						{
							char szDevice[100];
							::SendDlgItemMessage (hWnd, cmb4, CB_GETLBTEXT, n,
							  (LPARAM) szDevice);
							DeleteRegistryDuplex (szDevice);
						}
					}
					break;
				}

				case IDC_QUARTERFOLD:
				{
					::EnableWindow (::GetDlgItem (hWnd, IDC_DBLSIDE), false);
					break;
				}

				case IDC_SINGLEFOLD:
				{
					::EnableWindow (::GetDlgItem (hWnd, IDC_DBLSIDE), true);
					break;
				}

				case cmb4:
				{
					if (HIWORD (wParam) == CBN_SELCHANGE)
					{
						int n = ::SendDlgItemMessage (hWnd, cmb4, CB_GETCURSEL,
						  0, 0);
						if (n >= 0)
						{
							char szDevice[100];

							::SendDlgItemMessage (hWnd, cmb4, CB_GETLBTEXT, n,
							  (LPARAM) szDevice);

							int nDuplex;
							::ShowWindow (::GetDlgItem (hWnd, IDC_DBLSIDE),
							  GetRegistryDuplex (szDevice, nDuplex));
						}
					}
					break;
				}

				default:
					break;
			}
		}

		default:
			break;
	}

	return (FALSE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CCtlPanel::CCtlPanel (CCtp *pMainWnd)
{
	m_pMainWnd = pMainWnd;
	m_pAGDoc = NULL;
	m_hDevMode = NULL;
	m_hDevNames = NULL;
	m_nFontHeight = 0;
	m_hDlg = NULL;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CCtlPanel::~CCtlPanel ()
{
	if (m_hDevMode)
		::GlobalFree (m_hDevMode);
	if (m_hDevNames)
		::GlobalFree (m_hDevNames);
	if (m_hDlg)
		::GlobalFree (m_hDlg);
}


#pragma pack(push, 1)

typedef struct
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
	short menu;
	short windowClass;
	short title;
	short pointsize;
} DLGTEMPLATEEX;

#pragma pack(pop)


HWND CCtlPanel::Create (HWND hWndParent)
{
	_ASSERTE (m_hWnd == NULL);
	_Module.AddCreateWndData (&m_thunk.cd, (CDialogImplBase *) this);

	HDC hDC = ::GetDC (hWndParent);
	int nLogPixelsY = ::GetDeviceCaps (hDC, LOGPIXELSY);
	::ReleaseDC (hWndParent, hDC);

	HWND hWnd = NULL;
	if (nLogPixelsY > 96)
	{
		HINSTANCE hInst = _Module.GetResourceInstance ();
		HRSRC hRsrc = ::FindResource (hInst, MAKEINTRESOURCE (CCtlPanel::IDD),
		  RT_DIALOG);
		if (hRsrc)
		{
			HGLOBAL hTemplate = ::LoadResource (hInst, hRsrc);
			DLGTEMPLATEEX *pTemplate = (DLGTEMPLATEEX*) ::LockResource (hTemplate);

			int nSize = ::SizeofResource (hInst, hRsrc);
			m_hDlg = ::GlobalAlloc (GPTR, nSize);
			if (m_hDlg)
			{
				DLGTEMPLATEEX *pNew = (DLGTEMPLATEEX *) ::GlobalLock (m_hDlg);
				memcpy ((BYTE *) pNew, pTemplate, nSize);

				pNew->pointsize = (pNew->pointsize * 96 / nLogPixelsY);

				hWnd = ::CreateDialogIndirectParam (hInst, (DLGTEMPLATE *) pNew,
				  hWndParent, (DLGPROC) CCtlPanel::StartDialogProc, NULL);

				::GlobalUnlock (m_hDlg);
			}
		}
	}
	else
	{
		hWnd = ::CreateDialogParam (_Module.GetResourceInstance(),
		  MAKEINTRESOURCE (CCtlPanel::IDD), hWndParent,
		  (DLGPROC) CCtlPanel::StartDialogProc, NULL);
	}

	_ASSERTE (m_hWnd == hWnd);
	return hWnd;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
FONTARRAY &CCtlPanel::GetFontArray () 
{
	return (m_pMainWnd->GetFontList ().GetFontArray ());
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnInitDialog (UINT /*uMsg*/, WPARAM /*wParam*/,
								LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	SendDlgItemMessage (IDC_PAGE1, BM_SETCHECK, TRUE);

	for (int i = 0; i < MAX_COLORS; i++)
	{
		SendDlgItemMessage (IDC_COLOR, CB_ADDSTRING, 0, (LPARAM) "");
		SendDlgItemMessage (IDC_COLOR, CB_SETITEMDATA, i, PickColor[i]);
	}
	SendDlgItemMessage (IDC_COLOR, CB_SETCURSEL, (WPARAM) -1, 0);

	for (i = 8; i <= 72; i += 2)
	{
		char szPtSize[10];
		SendDlgItemMessage (IDC_PTSIZE, CB_ADDSTRING, 0,
		  (LPARAM) _itoa (i, szPtSize, 10));
	}
	SendDlgItemMessage (IDC_PTSIZE, CB_SETCURSEL, (WPARAM) -1, 0);
	HFONT hFont = (HFONT) GetStockObject (SYSTEM_FONT);
	SendDlgItemMessage (IDC_PTSIZE, WM_SETFONT, (WPARAM) hFont, 0);

	SetFonts ();

	SendDlgItemMessage (IDC_FONT, CB_SETCURSEL, (WPARAM) -1, 0);

	return (1);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnCtlColorDlg (UINT /*uMsg*/, WPARAM wParam,
								  LPARAM /*lParam*/, BOOL &bHandled)
{
	bHandled = TRUE;
	SetBkMode ((HDC) wParam, TRANSPARENT);
	return ((LRESULT) GetStockObject (NULL_BRUSH));
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnCtlColorStatic (UINT /*uMsg*/, WPARAM wParam,
  LPARAM /*lParam*/, BOOL &bHandled)
{
	bHandled = TRUE;
	SetBkMode ((HDC) wParam, TRANSPARENT);
	return ((LRESULT) GetStockObject (NULL_BRUSH));
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnMeasureItem (UINT /*uMsg*/, WPARAM wParam,
								  LPARAM lParam, BOOL &bHandled)
{
	UINT idCtl = wParam;
	LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

	if (idCtl == IDC_FONT)
		m_nFontHeight = lpmis->itemHeight;

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnDrawItem (UINT /*uMsg*/, WPARAM wParam,
							   LPARAM lParam, BOOL &bHandled)
{
	UINT idCtl = wParam;
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

	if (idCtl == IDC_COLOR)
	{
		CAGDC dc (lpdis->hDC);
		HBRUSH hbr = (HBRUSH) GetStockObject (WHITE_BRUSH);
		FillRect (lpdis->hDC, &lpdis->rcItem, hbr);

		if ((int) lpdis->itemID != -1)
		{
			COLORREF clr = (COLORREF) ::SendMessage (lpdis->hwndItem,
			  CB_GETITEMDATA, lpdis->itemID, 0);

			RECT rect = lpdis->rcItem;
			InflateRect (&rect, -4, -2);
			hbr = CreateSolidBrush (clr | PALETTERGB_FLAG);
			FillRect (lpdis->hDC, &rect, hbr);
			DeleteObject (hbr);
			hbr = (HBRUSH) GetStockObject (BLACK_BRUSH);
			FrameRect (lpdis->hDC, &rect, hbr);
		}

		if (lpdis->itemState & ODS_FOCUS || lpdis->itemState & ODS_SELECTED)
		{
			hbr = (HBRUSH) GetStockObject (BLACK_BRUSH);
			FrameRect (lpdis->hDC, &lpdis->rcItem, hbr);
		}
	}
	else if (idCtl == IDC_FONT)
	{
		if ((int) lpdis->itemID != -1)
		{
			int nFont = ::SendMessage (lpdis->hwndItem, CB_GETITEMDATA, lpdis->itemID, 0);

			FONTARRAY &FontArray = GetFontArray ();
			LOGFONT NewFont = FontArray[nFont].lf;
			NewFont.lfHeight = m_nFontHeight;
			NewFont.lfWidth = 0;
			if (NewFont.lfCharSet == SYMBOL_CHARSET)
			{
				lstrcpy (NewFont.lfFaceName, "Arial");
				NewFont.lfCharSet = ANSI_CHARSET;
				NewFont.lfPitchAndFamily = FF_SWISS;
			}

			SaveDC (lpdis->hDC);
			SetTextAlign (lpdis->hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);

			if (lpdis->itemState & ODS_SELECTED)
				SetTextColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
			else
				SetTextColor (lpdis->hDC, GetSysColor (COLOR_WINDOWTEXT));

			if (lpdis->itemState & ODS_SELECTED)
				SetBkColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHT));
			else
				SetBkColor (lpdis->hDC, GetSysColor (COLOR_WINDOW));

			HFONT hFont = CreateFontIndirect (&NewFont);
			HFONT hOldFont = (HFONT) SelectObject (lpdis->hDC, hFont);
			ExtTextOut (lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top,
				ETO_CLIPPED | ETO_OPAQUE, &lpdis->rcItem,
				FontArray[nFont].szFullName,
				lstrlen (FontArray[nFont].szFullName), NULL);
  
			if (lpdis->itemState & ODS_FOCUS)
				DrawFocusRect (lpdis->hDC, &lpdis->rcItem);

			SelectObject (lpdis->hDC, hOldFont);
			DeleteObject (hFont);
			RestoreDC (lpdis->hDC, -1);
		}
	}

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnPage (WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
						   BOOL &bHandled)
{
	if (m_pAGDoc)
	{
		m_pAGDoc->SetCurrentPage (m_PageMap[wID - IDC_PAGE1]);
		m_pMainWnd->NewPage ();
	}

	m_pMainWnd->SetFocus ();

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnPrint (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
							BOOL & /*bHandled*/)
{
	if (m_pAGDoc)
	{
		AGDOCTYPE DocType = m_pAGDoc->GetDocType ();
		bool bSingleFold = (DocType == DOC_CARDHV || DocType == DOC_CARDHH);

		PRINTDLG pd;
		memset (&pd, 0, sizeof (pd));
		pd.lStructSize = sizeof (pd);
		pd.hwndOwner = GetParent ();
		pd.hDevMode = m_hDevMode;
		pd.hDevNames = m_hDevNames;
		pd.hInstance = _Module.GetResourceInstance ();
		pd.lCustData = (DWORD) &bSingleFold;
		pd.lpfnPrintHook = PrintHookProc;
		pd.lpPrintTemplateName = MAKEINTRESOURCE (PRINTDLGORD);
		pd.Flags = PD_ENABLEPRINTTEMPLATE | PD_ENABLEPRINTHOOK;

		if (PrintDlg (&pd))
		{
			DEVNAMES *pDevNames = (DEVNAMES *) GlobalLock (pd.hDevNames);
			DEVMODE *pDevMode = (DEVMODE *) GlobalLock (pd.hDevMode);
			char *pszDriver = ((char *) pDevNames) + pDevNames->wDriverOffset;
			char *pszDevice = ((char *) pDevNames) + pDevNames->wDeviceOffset;
			char *pszOutput = ((char *) pDevNames) + pDevNames->wOutputOffset;

			int nDuplex;
			if (! GetRegistryDuplex (pszDevice, nDuplex))
				nDuplex = -1;

			if (bSingleFold && nDuplex == -1)
			{
				CDblSideIntro	Intro;
				CDblSideStep1	Step1;
				CDblSideStep2	Step2;
				CDblSideEnd		End;

				PROPSHEETPAGE *pPropPages = new PROPSHEETPAGE[4];
				pPropPages[0] = Intro.m_psp;
				pPropPages[1] = Step1.m_psp;
				pPropPages[2] = Step2.m_psp;
				pPropPages[3] = End.m_psp;

				Intro.m_pszDriver = Step1.m_pszDriver = pszDriver;
				Intro.m_pszDevice = Step1.m_pszDevice = pszDevice;
				Intro.m_pszOutput = Step1.m_pszOutput = pszOutput;
				Intro.m_pDevMode = Step1.m_pDevMode = pDevMode;

				PROPSHEETHEADER psh;
				psh.dwSize = sizeof (PROPSHEETHEADER);
				psh.dwFlags = PSH_WIZARD | PSH_PROPSHEETPAGE;
				psh.hwndParent = GetParent ();
				psh.hInstance = NULL;
				psh.hIcon = NULL;
				psh.pszCaption = NULL;
				psh.nPages = 4;
				psh.nStartPage = 0;
				psh.ppsp = pPropPages;
				psh.pfnCallback = NULL;

				::PropertySheet (&psh);
				if (End.IsFinished ())
				{
					nDuplex = Step2.GetSelected ();
					SetRegistryDuplex (pszDevice, nDuplex);
				}

				delete [] pPropPages;
			}

			if (! (bSingleFold && nDuplex == -1))
			{
				CWaitDlg WaitDlg;
				WaitDlg.Create (GetParent());
				WaitDlg.UpdateWindow();

				MSG msg;
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				bool bRotated;
				int nCopies = pd.nCopies;
				while (nCopies-- > 0)
				{
					if (bSingleFold)
					{
						m_pAGDoc->PrintCardSingle (PRINT_OUTSIDE, pszDriver,
						  pszDevice, pszOutput, pDevMode, bRotated);
					}
					else
					{
						m_pAGDoc->PrintCardQuarter (pszDriver, pszDevice,
						  pszOutput, pDevMode);
					}
				}

				WaitDlg.DestroyWindow();

				if (bSingleFold)
				{
					char szFace[10];
					if (nDuplex > 2)
						lstrcpy (szFace, "UP");
					else
						lstrcpy (szFace, "DOWN");

					DWORD dwOrientation = ::DeviceCapabilities (pszDevice,
					  pszOutput, DC_ORIENTATION, NULL, pDevMode);

					bool bHorz = (DocType == DOC_CARDHH || DocType == DOC_CARDFH);
					if (!bHorz && dwOrientation == 270)
						bRotated = !bRotated;

					char szDirection[15];
					int nDirection = PaperDirection[nDuplex % 2][bHorz][bRotated];
					if (nDirection == 1)
						lstrcpy (szDirection, "AWAY FROM");
					else
						lstrcpy (szDirection, "TOWARD");

					char szMsg[256];
					wsprintf (szMsg, "To print the inside of your card, reinsert the page with the printed side %s\nand the front panel of the card %s the printer.\n\n\nClick OK when you are ready to print the inside.",
					  szFace, szDirection);

					if (MessageBox (szMsg, "Print inside", MB_OKCANCEL) == IDOK)
					{
						WaitDlg.Create (GetParent());
						WaitDlg.UpdateWindow();

						MSG msg;
						while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}

						nCopies = pd.nCopies;
						while (nCopies-- > 0)
						{
							m_pAGDoc->PrintCardSingle (PRINT_INSIDE, pszDriver,
							  pszDevice, pszOutput, pDevMode, bRotated);
						}

						WaitDlg.DestroyWindow();
					}
				}
			}

			GlobalUnlock (pd.hDevNames);
			GlobalUnlock (pd.hDevMode);
		}

		m_hDevMode = pd.hDevMode;
		m_hDevNames = pd.hDevNames;
	
		m_pMainWnd->SetFocus ();
	}
	
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtlPanel::SetDoc (CAGDoc *pAGDoc)
{
	m_pAGDoc = pAGDoc;

	AGDOCTYPE DocType = m_pAGDoc->GetDocType ();
	bool bCardType = (DocType == DOC_CARDHV || DocType == DOC_CARDHH ||
	  DocType == DOC_CARDFV || DocType == DOC_CARDFH);

	int nPages = m_pAGDoc->GetNumPages ();
	for (int nPage = 1, nMap = 0; nPage <= nPages; nPage++)
	{
		CAGPage *pPage = m_pAGDoc->GetPage (nPage);
 
		if ((! pPage->IsEmpty ()) || (bCardType && nPage == 4))
		{
			const char *pszPageName = pPage->GetPageName ();
			SetDlgItemText (IDC_PAGE1 + nMap, pszPageName);
			::ShowWindow (GetDlgItem (IDC_PAGE1 + nMap), SW_SHOW);
			m_PageMap[nMap++] = nPage;
		}
	}

	while (nMap < 4)
	{
		::ShowWindow (GetDlgItem (IDC_PAGE1 + nMap), SW_HIDE);
		m_PageMap[nMap++] = 0;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtlPanel::UpdateControls (const CAGText *pText)
{
	scTypeSpecList	tsList;
	int				nNumItems = 0;

	if (pText)
	{
		pText->GetSelTSList (tsList);
		nNumItems = tsList.NumItems ();
	}

	if (nNumItems == 0)
	{
		SendDlgItemMessage (IDC_FONT, CB_SETCURSEL, (WPARAM) -1, 0);
		SendDlgItemMessage (IDC_PTSIZE, CB_SETCURSEL, (WPARAM) -1, 0);
		SendDlgItemMessage (IDC_COLOR, CB_SETCURSEL, (WPARAM) -1, 0);
		SendDlgItemMessage (IDC_LEFT, BM_SETCHECK, FALSE);
		SendDlgItemMessage (IDC_CENTER, BM_SETCHECK, FALSE);
		SendDlgItemMessage (IDC_RIGHT, BM_SETCHECK, FALSE);

		return;
	}

	CAGSpec		*pAGSpec = (CAGSpec *) (tsList[0].ptr ());
	LOGFONT		SpecFont = pAGSpec->m_Font;
	int			nSpecPtSize = abs (pAGSpec->m_Font.lfHeight) * 72 / APP_RESOLUTION;
	COLORREF	SpecColor = pAGSpec->m_Color;
	bool		bFont = true;
	bool		bPtSize = true;
	bool		bColor = true;
	
	if (nNumItems > 1)
	{
		for (int i = 1; i < nNumItems; i++)
		{
			pAGSpec = (CAGSpec *) (tsList[i].ptr ());
			if (lstrcmp (SpecFont.lfFaceName, pAGSpec->m_Font.lfFaceName) != 0
			  || SpecFont.lfWeight != pAGSpec->m_Font.lfWeight
			  || (SpecFont.lfItalic != 0) != (pAGSpec->m_Font.lfItalic != 0))
			{
				bFont = false;
			}

			int	nPtSize = abs (pAGSpec->m_Font.lfHeight) * 72 / APP_RESOLUTION;
			if (nSpecPtSize != nPtSize)
				bPtSize = false;

			if (SpecColor != pAGSpec->m_Color)
				bColor = false;
		}
	}

	if (bFont)
	{
		FONTARRAY &FontArray = GetFontArray ();
		int nFonts = FontArray.size ();
		for (int i = 0; i < nFonts; i++)
		{
			if (lstrcmp (SpecFont.lfFaceName, FontArray[i].lf.lfFaceName) == 0
			  && SpecFont.lfWeight == FontArray[i].lf.lfWeight
			  && (SpecFont.lfItalic != 0) == (FontArray[i].lf.lfItalic != 0))
			{
				break;
			}
		}

		if (i >= nFonts)
			SendDlgItemMessage (IDC_FONT, CB_SETCURSEL, (WPARAM) -1, 0);
		else
		{
			int index = SendDlgItemMessage (IDC_FONT, CB_FINDSTRINGEXACT,
			  (WPARAM) -1, (LPARAM) FontArray[i].szFullName);
			SendDlgItemMessage (IDC_FONT, CB_SETCURSEL, index, 0);
		}
	}
	else
		SendDlgItemMessage (IDC_FONT, CB_SETCURSEL, (WPARAM) -1, 0);

	if (bPtSize)
	{
		char szPtSize[10];
		int nPtSize = abs (SpecFont.lfHeight) * 72 / APP_RESOLUTION;
		int index = SendDlgItemMessage (IDC_PTSIZE, CB_FINDSTRINGEXACT,
		  (WPARAM) -1, (LPARAM) _itoa (nPtSize, szPtSize, 10));
		if (index == CB_ERR)
		{
    		int nItems = SendDlgItemMessage (IDC_PTSIZE, CB_GETCOUNT, 0, 0);
			for (int i = 0; i < nItems; i++)
			{
				char szTemp[20];
				SendDlgItemMessage (IDC_PTSIZE, CB_GETLBTEXT, i,
				  (LPARAM) szTemp);
				if (nPtSize < atoi (szTemp))
				{
					index = SendDlgItemMessage (IDC_PTSIZE, CB_INSERTSTRING, i,
					  (LPARAM) szPtSize);
					break;
				}
			}
			if (i >= nItems)
			{
				index = SendDlgItemMessage (IDC_PTSIZE, CB_ADDSTRING, 0,
				  (LPARAM) szPtSize);
			}
		}
		SendDlgItemMessage (IDC_PTSIZE, CB_SETCURSEL, index, 0);
	}
	else
		SendDlgItemMessage (IDC_PTSIZE, CB_SETCURSEL, (WPARAM) -1, 0);

	if (bColor)
	{
    	int nItems = SendDlgItemMessage (IDC_COLOR, CB_GETCOUNT, 0, 0);
		for (int i = 0; i < nItems; i++)
		{
			COLORREF Color = (COLORREF) SendDlgItemMessage (IDC_COLOR,
			  CB_GETITEMDATA, i, 0);
			if (Color == SpecColor)
				break;
		}

		if (i >= nItems)
		{
			SendDlgItemMessage (IDC_COLOR, CB_INSERTSTRING, 0, (LPARAM) "");
			SendDlgItemMessage (IDC_COLOR, CB_SETITEMDATA, 0, SpecColor);
			i = 0;
		}

		SendDlgItemMessage (IDC_COLOR, CB_SETCURSEL, i, 0);
	}
	else
		SendDlgItemMessage (IDC_COLOR, CB_SETCURSEL, (WPARAM) -1, 0);


	scTypeSpecList tsListPara;
	pText->GetSelParaTSList (tsListPara);
	nNumItems = tsListPara.NumItems ();

	pAGSpec = (CAGSpec *) (tsListPara[0].ptr ());
	eTSJust SpecHorzJust = pAGSpec->m_HorzJust;
	bool	bHorzJust = true;
	
	if (nNumItems > 1)
	{
		for (int i = 1; i < nNumItems; i++)
		{
			pAGSpec = (CAGSpec *) (tsListPara[i].ptr ());
			if (SpecHorzJust != pAGSpec->m_HorzJust)
				bHorzJust = false;
		}
	}

	if (bHorzJust)
	{
		switch (SpecHorzJust)
		{
			case eRagCentered:
				SendDlgItemMessage (IDC_CENTER, BM_SETCHECK, TRUE);
				SendDlgItemMessage (IDC_LEFT, BM_SETCHECK, FALSE);
				SendDlgItemMessage (IDC_RIGHT, BM_SETCHECK, FALSE);
				break;

			case eRagLeft:
				SendDlgItemMessage (IDC_RIGHT, BM_SETCHECK, TRUE);
				SendDlgItemMessage (IDC_LEFT, BM_SETCHECK, FALSE);
				SendDlgItemMessage (IDC_CENTER, BM_SETCHECK, FALSE);
				break;

			case eRagRight:
			default:
				SendDlgItemMessage (IDC_LEFT, BM_SETCHECK, TRUE);
				SendDlgItemMessage (IDC_CENTER, BM_SETCHECK, FALSE);
				SendDlgItemMessage (IDC_RIGHT, BM_SETCHECK, FALSE);
				break;
		}
	}
	else
	{
		SendDlgItemMessage (IDC_LEFT, BM_SETCHECK, FALSE);
		SendDlgItemMessage (IDC_CENTER, BM_SETCHECK, FALSE);
		SendDlgItemMessage (IDC_RIGHT, BM_SETCHECK, FALSE);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnHorzJust (WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
  BOOL &bHandled)
{
	CAGText *pText = m_pMainWnd->GetText ();
	if (pText)
	{
		switch (wID)
		{
			case IDC_LEFT:
				pText->SetHorzJust (eRagRight);
				break;

			case IDC_CENTER:
				pText->SetHorzJust (eRagCentered);
				break;

			case IDC_RIGHT:
				pText->SetHorzJust (eRagLeft);
				break;
		}
	}

	m_pMainWnd->SetFocus ();
	
	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnFont (WORD wNotifyCode, WORD wID, HWND hWndCtl,
BOOL &bHandled)
{
	if (wNotifyCode != CBN_SELCHANGE)
	{
		if (wNotifyCode == CBN_CLOSEUP)
			m_pMainWnd->SetFocus ();

		bHandled = TRUE;
		return (TRUE);
	}

	int nItem = SendDlgItemMessage (IDC_FONT, CB_GETCURSEL, 0, 0);
	if (nItem >= 0)
	{
        CAGText *pText = m_pMainWnd->GetText ();
		if (pText)
		{
			int nFont = SendDlgItemMessage (IDC_FONT, CB_GETITEMDATA, nItem, 0);
			FONTARRAY &FontArray = GetFontArray ();
			pText->SetTypeface (FontArray[nFont].lf);
		}
	}
	m_pMainWnd->SetFocus ();

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnPtSize (WORD wNotifyCode, WORD wID, HWND hWndCtl,
  BOOL &bHandled)
{
	if (wNotifyCode != CBN_SELCHANGE)
	{
		if (wNotifyCode == CBN_CLOSEUP)
			m_pMainWnd->SetFocus ();

		bHandled = TRUE;
		return (TRUE);
	}

	int nItem = SendDlgItemMessage (IDC_PTSIZE, CB_GETCURSEL, 0, 0);
	if (nItem >= 0)
	{
        CAGText *pText = m_pMainWnd->GetText ();
		if (pText)
		{
			char szPtSize[20];
			SendDlgItemMessage (IDC_PTSIZE, CB_GETLBTEXT, nItem,
			  (LPARAM) szPtSize);
			pText->SetPtSize (atoi (szPtSize));
		}
	}
	m_pMainWnd->SetFocus ();

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtlPanel::OnColor (WORD wNotifyCode, WORD wID, HWND hWndCtl,
  BOOL &bHandled)
{
	if (wNotifyCode != CBN_SELCHANGE)
	{
		if (wNotifyCode == CBN_CLOSEUP)
			m_pMainWnd->SetFocus ();

		bHandled = TRUE;
		return (TRUE);
	}

	int nItem = SendDlgItemMessage (IDC_COLOR, CB_GETCURSEL, 0, 0);
	if (nItem >= 0)
	{
        CAGText *pText = m_pMainWnd->GetText ();
		if (pText)
		{
			COLORREF Color = (COLORREF) SendDlgItemMessage (IDC_COLOR,
			  CB_GETITEMDATA, nItem, 0);
			pText->SetColor (Color);
		}
	}
	m_pMainWnd->SetFocus ();

	bHandled = TRUE;
	return (TRUE);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtlPanel::SetFonts ()
{
	SendDlgItemMessage (IDC_FONT, CB_RESETCONTENT, 0, 0);

	FONTARRAY &FontArray = GetFontArray ();
	int nFonts = FontArray.size ();
	for (int i = 0; i < nFonts; i++)
	{
		if (SendDlgItemMessage (IDC_FONT, CB_FINDSTRINGEXACT,
		  (WPARAM) -1, (LPARAM) FontArray[i].szFullName) == CB_ERR)
		{
			int index = SendDlgItemMessage (IDC_FONT, CB_ADDSTRING, 0,
    		  (LPARAM) FontArray[i].szFullName);
			SendDlgItemMessage (IDC_FONT, CB_SETITEMDATA, index, i);
		}
	}
}
