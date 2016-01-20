#include "stdafx.h"
#include "Print.h"
#include "Ctp.h"
#include "DocWindow.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "AGText.h"
#include "DblSide.h"
#include "WaitDlg.h"
#include "HTMLDialog.h"
#include "RegKeys.h"
#include "PaperFeed.h"
#include "PaperFeedDlg.h"
#include "EnvelopesDlg.h"
#include "Image.h"


#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define THIN_MARGIN  INCHES(0.25)
#define THICK_MARGIN INCHES(0.375)

extern CString g_szAppName;
//////////////////////////////////////////////////////////////////////
//	m_iDuplex = 1	m_iDuplex = 2	m_iDuplex = 3	m_iDuplex = 4	//
//	Printer prints	Printer prints	Printer prints	Printer prints	//
//	side facing up	side facing up	side facing dn	side facing dn	//
//																	//
//	^ ^ ^ 			  ^				^     ^  ^		^				//
//	| | | 			  |				|     |  |		|				//
//					|   |							      |  |		//
//					V   V							      V  V		//
//																	//
//	Front			Front			Front Back		Front Back		//
//					very unlikely					very unlikely	//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CPrint::CPrint(CDocWindow* pDocWindow)
{
	m_pDocWindow = pDocWindow;
	m_pCtp = NULL;
	m_pAGDoc = NULL;
	m_PrintDocType = DOC_DEFAULT;
	m_hDevNames = NULL;
	m_hDevMode = NULL;
	m_pDevMode = NULL;
	m_iPaperType = 0;
	m_bForceDuplexTest = false;
	m_bForceBorderless = false;

	m_iDuplex = -1;
	m_iPaperFeedCode = CPaperFeed::Default();
	::SetRect(&m_MarginsPortrait, THIN_MARGIN, THICK_MARGIN, THIN_MARGIN, THICK_MARGIN);
	::SetRect(&m_MarginsLandscape, THICK_MARGIN, THIN_MARGIN, THICK_MARGIN, THIN_MARGIN);
}

//////////////////////////////////////////////////////////////////////
CPrint::~CPrint()
{
    DeleteObject(m_hHdrFont1);
    DeleteObject(m_hHdrFont2);

    if (m_hDevNames)
		::GlobalFree(m_hDevNames);
	if (m_hDevMode)
		::GlobalFree(m_hDevMode);
	if (m_pDevMode)
		delete [] m_pDevMode;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsDocDuplex()
{
	return (
		m_PrintDocType == DOC_HALFCARD	||
		m_PrintDocType == DOC_NOTECARD	||
		m_PrintDocType == DOC_BROCHURE	||
		m_PrintDocType == DOC_POSTCARD	||
		m_PrintDocType == DOC_TRIFOLD	||
		m_PrintDocType == DOC_CDBOOKLET);
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsCustomPaper(AGDOCTYPE PrintDocType)
{
	if (PrintDocType == DOC_DEFAULT)
		PrintDocType  = m_PrintDocType;
	return (
		PrintDocType == DOC_HOLIDAYCARD	||
		PrintDocType == DOC_NOTECARD	||
		PrintDocType == DOC_PHOTOCARD);
}

//////////////////////////////////////////////////////////////////////
bool CPrint::ShowDuplexTest()
{
	return IsDocDuplex() || IsCustomPaper();
}

//////////////////////////////////////////////////////////////////////
bool CPrint::NeedDuplexTest()
{
	return ShowDuplexTest() && (m_iDuplex < 0 || m_bForceDuplexTest);
}

//////////////////////////////////////////////////////////////////////
void CPrint::LoadDeviceValuesFromRegistry(LPCTSTR strDevice)
{
	CRegKey regkey;

	// Load the duplex code for the selected printer
	m_iDuplex = -1;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_DUPLEX) == ERROR_SUCCESS)
	{
		DWORD dwLength = MAX_PATH;
		char strValue[MAX_PATH];
		if (regkey.QueryStringValue(strDevice, strValue, &dwLength) == ERROR_SUCCESS)
			m_iDuplex = atoi(strValue);
	}

	// Load the paper feed code for the selected printer
	m_iPaperFeedCode = CPaperFeed::Default();
	if (m_iDuplex >= 0)
	{ // Inherit the face up/down setting if available
		WORD wID = (m_iDuplex > 2 ? IDC_PAPERFEED_FACE_DOWN : IDC_PAPERFEED_FACE_UP);
		CPaperFeed::SetFace(m_iPaperFeedCode, wID);
	}
	if (regkey.Open(REGKEY_APPROOT, REGKEY_PAPERFEED) == ERROR_SUCCESS)
	{
		DWORD dwLength = MAX_PATH;
		char strValue[MAX_PATH];
		if (regkey.QueryStringValue(strDevice, strValue, &dwLength) == ERROR_SUCCESS)
		{
			m_iPaperFeedCode = atoi(strValue);
			CPaperFeed::Verify(m_iPaperFeedCode);
		}
	}
}

//////////////////////////////////////////////////////////////////////
bool CPrint::Start()
{
	if (!m_pDocWindow)
		return false;

	if (!m_pCtp)
		return false;

	if (!m_pAGDoc)
		return false;

	m_PrintDocType = m_pAGDoc->GetDocType();
	m_iPaperType = 0;
	m_bForceDuplexTest = false;

	PRINTDLG pd;
	memset(&pd, 0, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = m_pDocWindow->m_hWnd;
	pd.hInstance = _AtlBaseModule.GetResourceInstance();
	pd.lCustData = (DWORD)this;
	pd.lpfnPrintHook = CPrint::PrintHookProc;
    pd.lpPrintTemplateName = MAKEINTRESOURCE(IDD_PRINT);
	pd.Flags = PD_ENABLEPRINTTEMPLATE | PD_ENABLEPRINTHOOK;
	pd.nCopies = 1;

	// Save a few values before the print dialog
	CString strSavedDevice = m_strDevice;
	int iSavedPaperFeedCode = m_iPaperFeedCode;

	// Bring up the print dialog
	int nCopies = ShowDialog(&pd);

	// See if the user cancelled
	if (nCopies <= 0)
	{
		m_pDocWindow->SetFocus();
		return false;
	}

	// Update the margin info (m_MarginsPortrait and m_MarginsLandscape) using the new m_pDevMode
	ComputeMargins();

	// Back from the print dialog box
	bool bMarginsChanged = (!strSavedDevice.IsEmpty() && strSavedDevice != m_strDevice) || (iSavedPaperFeedCode != m_iPaperFeedCode);
	if (bMarginsChanged)
		m_pDocWindow->PageInvalidate();

	CAGText* pText = m_pDocWindow->GetText();
	if (pText)
		pText->ShowSelection(false);

	bool bDoneDialog = false;
	do // A convenient block we can break out of
	{
		// See if we need to run the duplex printing wizard
		if (NeedDuplexTest())
		{
			m_bForceDuplexTest = false;

			CDblSideIntro Intro(this);
			CDblSideStep1 Step1(this);
			CDblSideStep2 Step2(this);
			CDblSideEnd End(this);

			PROPSHEETPAGE* pPropPages = new PROPSHEETPAGE[4];
			pPropPages[0] = Intro.m_psp;
			pPropPages[1] = Step1.m_psp;
			pPropPages[2] = Step2.m_psp;
			pPropPages[3] = End.m_psp;

			PROPSHEETHEADER psh;
			psh.dwSize = sizeof(PROPSHEETHEADER);
			psh.dwFlags = PSH_WIZARD | PSH_PROPSHEETPAGE;
			psh.hwndParent = m_pDocWindow->m_hWnd;
			psh.hInstance = NULL;
			psh.hIcon = NULL;
			psh.pszCaption = NULL;
			psh.nPages = 4;
			psh.nStartPage = 0;
			psh.ppsp = pPropPages;
			psh.pfnCallback = NULL;

			::PropertySheet(&psh);
			delete [] pPropPages;

			// If the user cancels, get out
			if (!End.IsFinished())
				break;

			m_iDuplex = Step2.GetDuplexCode();
			CRegKey regkey;
			if (regkey.Create(REGKEY_APPROOT, REGKEY_DUPLEX) == ERROR_SUCCESS)
			{
				CString strValue;
				strValue.Format("%d", m_iDuplex);
				bool bOK = (regkey.SetStringValue(m_strDevice, strValue) == ERROR_SUCCESS);
			}

			if (m_iDuplex >= 0)
			{ // Inherit the face up/down setting if available
				WORD wID = (m_iDuplex > 2 ? IDC_PAPERFEED_FACE_DOWN : IDC_PAPERFEED_FACE_UP);
				CPaperFeed::SetFace(m_iPaperFeedCode, wID);
			}
		}

		// If we still need the duplex info, get out
		if (NeedDuplexTest())
			break;

		CWaitDlg WaitDlg(m_pCtp);
		WaitDlg.Create(m_pDocWindow->m_hWnd);
		WaitDlg.UpdateWindow();

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		bool bPortraitPaper = true;
		bDoneDialog = DoPass(NULL/*pPrintDC*/, 1, bPortraitPaper);

		WaitDlg.DestroyWindow();

		// See if we need to do duplex printing
		if (IsDocDuplex())
		{
			// Get the URL with print animation for display now
			bool bContinuePrinting = ContinuePrinting(bPortraitPaper);
			// If the user doesn't want the next page, get out
			if (!bContinuePrinting)
			{
				bDoneDialog = false;
				break;
			}

			WaitDlg.Create(m_pDocWindow->m_hWnd);
			WaitDlg.UpdateWindow();

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			bDoneDialog = DoPass(NULL/*pPrintDC*/, 2, bPortraitPaper);

			WaitDlg.DestroyWindow();
		}

		bool bIsCardWithExtraPages = (m_pAGDoc->GetNumPages() > 4 && m_pAGDoc->NeedsCardBack());
		if (bIsCardWithExtraPages)
		{
			CWaitDlg WaitDlg(m_pCtp);
			WaitDlg.Create(m_pDocWindow->m_hWnd);
			WaitDlg.UpdateWindow();

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			bDoneDialog = DoPass(NULL/*pPrintDC*/, 3, bPortraitPaper);

			WaitDlg.DestroyWindow();
		}

	} while (0);

	if (pText)
		pText->ShowSelection(true);

	if (bDoneDialog)
	{
		bool bShowConfirmationDialog = true;
		CString strMemberNumber = m_pCtp->GetContextMemberNumber();
		if (strMemberNumber.IsEmpty())
			bShowConfirmationDialog = false;

		CString strItemInfo = m_pCtp->GetContextItemInfo();
		if (strItemInfo.IsEmpty() || (strItemInfo.Find("|||") >= 0))
			bShowConfirmationDialog = false;

		if (bShowConfirmationDialog)
		{
			CString strDBItem = m_pCtp->GetDBItem();
			if (strDBItem.IsEmpty())
				strDBItem = "0";

			CString strCpHost = m_pCtp->GetContextCpHost();
			CString strSrcURL = strCpHost + "cpprint.pd?";
			strSrcURL += "memnum="   + strMemberNumber;
			strSrcURL += "&";
			strSrcURL += "iteminfo=" + strItemInfo;
			strSrcURL += "&";
			strSrcURL += "item="     + strDBItem;
			strSrcURL += "&";
			strSrcURL += "copies="   + String("%d", nCopies);

			if (m_pAGDoc->FindSymbolAnywhereByID(IDR_ENVELOPE_SEND, 0/*dwFlags*/))
			{
				strSrcURL += "&";
				strSrcURL += "envelope=1";
			}

			CHtmlDialog dlg(strSrcURL, false/*bRes*/, m_pDocWindow->m_hWnd);
			dlg.SetWindowSize(583, 482);// JHC//dlg.SetWindowSize(567, 405); // Add (27, 65) to the size of the gif
			dlg.SetResizable(true);

			CWaitCursor Wait;
			dlg.DoModal();
		}

		if (!m_pCtp->AutoPrintEnabled())
			m_pDocWindow->LaunchEnvelopeWizard(m_PrintDocType, true/*bAtPrintTime*/);
	}

	m_pDocWindow->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::ContinuePrinting(bool bPortraitPaper)
{
	bool bPortrait = m_pAGDoc->IsPortrait();
	bool bToward = (GetPaperDirection(bPortraitPaper) == 1 ? true : false);
	bool bFaceUp = (m_iDuplex > 2 ? true : false);

	// get the temp directory
	char szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);
	CString strTempDir(szTempPath);
	CString strTempFile(szTempPath);

	// thumb sizes
	SIZE MaxSizeT = {75, 75};

	int nPages = m_pAGDoc->GetNumPages();
	CAGPage* pPage = m_pAGDoc->GetPage(0);
	if (!pPage)
		return false;

	// create the thumbs
	strTempFile += (bToward ? "1.jpg" : "2.jpg");
	pPage->CreateThumb(MaxSizeT, strTempFile, "JPG");

	pPage = ((nPages>1) ? m_pAGDoc->GetPage(3) : NULL);
	if (pPage)
	{
		strTempFile = strTempDir;
		strTempFile += (bToward ? "2.jpg" : "1.jpg");
		pPage->CreateThumb(MaxSizeT, strTempFile, "JPG");
	}

	// extract html file
	HINSTANCE hInstance = _AtlBaseModule.GetResourceInstance();
	strTempFile = strTempDir;
	strTempFile += "PrintFinalStep.htm";
	WriteResourceToFile(hInstance, "PRINTFINALSTEP.HTM", RT_HTML, strTempFile);
	CString strURL = strTempFile;

	// extract the swf file
	bool bTopFeed = (CPaperFeed::GetFeed(m_iPaperFeedCode) == IDC_PAPERFEED_TOP);
	CString strSWFFile;
	if (bFaceUp)
		strSWFFile = bTopFeed ? "Top.swf" : "Bottom.swf";
	else
		strSWFFile = bTopFeed ? "TopFlip.swf" : "BottomFlip.swf";

	strTempFile = strTempDir;
	strTempFile += "printer_animation.swf";
	strSWFFile.MakeUpper();
	WriteResourceToFile(hInstance, strSWFFile, RT_HTML, strTempFile);

	// display the animation now
	char* pDirection = (bToward ? "TOWARD" : "AWAY FROM");
	char* pFace = (bFaceUp ? "UP" : "DOWN");

	CHtmlDialog dlg(strURL, false, ::GetActiveWindow());
	dlg.SetWindowSize(500, 460);
	dlg.SetResizable(false);
	dlg.SetScrollbars(false);
	dlg.SetValue("title", g_szAppName);
	dlg.SetValue("face", pFace);
	dlg.SetValue("direction", pDirection);
	dlg.DoModal();
	CString strYES = dlg.GetReturnString();

	// cleanup the temp dir
	strTempFile = strTempDir;
	strTempFile += "1.jpg";
	::DeleteFile(strTempFile);

	strTempFile = strTempDir;
	strTempFile += "2.jpg";
	::DeleteFile(strTempFile);

	strTempFile = strTempDir;
	strTempFile += "printer_animation.swf";
	::DeleteFile(strTempFile);

	::DeleteFile(strURL);

	return (strYES == "YES");
}

//////////////////////////////////////////////////////////////////////
int CPrint::GetPaperDirection(bool bPortraitPaper)
{
	static const int PaperDirection[2][2][2] = // [m_iDuplex % 2][bPortraitPaper][bRotated]
	{ // On 2nd side, is the front panel is (0 = away from, 1 = toward) the printer
		{ // m_iDuplex 2 or 4 - unlikely that these occur because the printer would have to print bottom up
			{ 0, 1 },	// landscape paper { not rotated, rotated }
			{ 1, 0 },	// portrait paper  { not rotated, rotated }
		},
		{ // m_iDuplex 1 or 3
			{ 1, 0 },	// landscape paper { not rotated, rotated }
			{ 0, 1 },	// portrait paper  { not rotated, rotated }
		}
	};

	DWORD dwOrientation = ::DeviceCapabilities(m_strDevice, m_strPort, DC_ORIENTATION, NULL, m_pDevMode);
	bool bClockwise = (dwOrientation == 270);
	bool bRotated = (!bPortraitPaper && bClockwise);
	return PaperDirection[m_iDuplex % 2][bPortraitPaper][bRotated];
}

//////////////////////////////////////////////////////////////////////
bool CPrint::MakePdf(LPCSTR strDocFileName, LPCSTR strFileName)
{
	if (!m_pDocWindow)
		return false;

	if (!m_pCtp)
		return false;

	if (!m_pAGDoc)
		return false;

	m_PrintDocType = m_pAGDoc->GetDocType();
	m_iPaperType = 0;
	m_bForceDuplexTest = false;

	double fSheetWidth = CAGDocSheetSize::GetWidth(); // pPaperTemplate->fPageWidth
	double fSheetHeight = CAGDocSheetSize::GetHeight(); //pPaperTemplate->fPageHeight

#ifdef _DEBUG
	CAGPdfDC* pPdfDC = new CAGPdfDC(strDocFileName, strFileName, m_pCtp->GetContextUser(), fSheetWidth, fSheetHeight);
	if (!pPdfDC)
		return false;

	CAGText* pText = m_pDocWindow->GetText();
	if (pText)
		pText->ShowSelection(false);

	bool bPortraitPaper = true;
	bool bDoneDialog = DoPass(pPdfDC, 1, bPortraitPaper);
	if (IsDocDuplex())
			bDoneDialog = DoPass(pPdfDC, 2, bPortraitPaper);

	if (pText)
		pText->ShowSelection(true);

	m_pDocWindow->SetFocus();

	delete pPdfDC;
#endif _DEBUG

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::DoPass(CAGPrintDC* pPrintDC, int iPass, bool& bPortraitPaper)
{
	// All possible page orders: negative value means we rotate it 180 degrees
	const int QuarterSideFoldOrder[] = {-1, -4, 2, 3};
	const int QuarterTopFoldOrder[] = {-1, 2, 4, 3};

	const int HalfSideFoldPass1Order[] = {-1, -4};
	const int HalfSideFoldPass2Order[] = {-3, -2};
	const int HalfTopFoldPass1Order[] = {-1, 4};
	const int HalfTopFoldPass2Order[] = {-3, -2};

	const int CDInsertSideFoldPass1Order[] = {4, 1};
	const int CDInsertSideFoldPass2Order[] = {2, 3};
	const int CDInsertTopFoldPass1Order[] = {-4, 1};
	const int CDInsertTopFoldPass2Order[] = {2, 3};

	const int TrifoldPortraitPass1Order[] = {-1, -6, -5};
	const int TrifoldPortraitPass2Order[] = {-4, -3, -2};
	const int TrifoldLandscapePass1Order[] = {-1, -6, -5};
	const int TrifoldLandscapePass2Order[] = {-4, -3, -2};

	const int BrochureSideFoldPass1Order[] = {1};
	const int BrochureSideFoldPass2Order[] = {-2};
	const int BrochureTopFoldPass1Order[] = {1};
	const int BrochureTopFoldPass2Order[] = {2};

	const int PostcardPass1Order[] = {1};
	const int PostcardPass2Order[] = {2};

	const int* pPageOrder = NULL;
	int nPages = m_pAGDoc->GetNumPages();
	bool bPortrait = m_pAGDoc->IsPortrait();
	bool bMirror = false;
	bool bCenterOnly = false;
	int iStart = 0;

	const PaperTemplate* pPaperTemplate = NULL;
	int nTemplates = CPaperTemplates::GetTemplate(m_PrintDocType, m_iPaperType, &pPaperTemplate);

	switch (m_PrintDocType)
	{
		case DOC_BANNER:
		case DOC_BUSINESSCARD:
		case DOC_CDLABEL:
		case DOC_ENVELOPE:
		case DOC_FULLSHEET:
		case DOC_GIFTNAMECARD:
		case DOC_HOLIDAYCARD:
		case DOC_LABEL:
		case DOC_PHOTOCARD:
		{
			break;
		}
		case DOC_BROCHURE:
		{
			bool bSideFold = bPortrait;
			if (iPass == 1)
				pPageOrder = (bSideFold ? &BrochureSideFoldPass1Order[0] : &BrochureTopFoldPass1Order[0]);
			else
				pPageOrder = (bSideFold ? &BrochureSideFoldPass2Order[0] : &BrochureTopFoldPass2Order[0]);
			nPages = 1; // Half of 2
			break;
		}
		case DOC_CDBOOKLET:
		{
			bool bSideFold = bPortrait;
			if (iPass == 1)
				pPageOrder = (bSideFold ? &CDInsertSideFoldPass1Order[0] : &CDInsertTopFoldPass1Order[0]);
			else
				pPageOrder = (bSideFold ? &CDInsertSideFoldPass2Order[0] : &CDInsertTopFoldPass2Order[0]);
			nPages = 2; // Half of 4
			break;
		}
		case DOC_HALFCARD:
		{
			bool bSideFold = bPortrait;
			if (iPass == 1)
			{
				pPageOrder = (bSideFold ? &HalfSideFoldPass1Order[0] : &HalfTopFoldPass1Order[0]);
				nPages = 2; // Half of 4
			}
			else
			if (iPass == 2)
			{
				pPageOrder = (bSideFold ? &HalfSideFoldPass2Order[0] : &HalfTopFoldPass2Order[0]);
				nPages = 2; // Half of 4
			}
			else
			if (iPass == 3)
			{
				nTemplates = CPaperTemplates::GetTemplate(DOC_FULLSHEET, 0, &pPaperTemplate);
				bPortrait = m_pAGDoc->IsPortrait(4); // Redefine portrait to match the first extra page (page 4)
				pPageOrder = NULL;
				nPages -= 4;
				iStart = 4;
				bCenterOnly = true;
			}
			break;
		}
		case DOC_NOTECARD:
		{
			bool bSideFold = bPortrait;
			if (iPass == 1)
			{
				pPageOrder = (bSideFold ? &HalfSideFoldPass1Order[0] : &HalfTopFoldPass1Order[0]);
				nPages = 2; // Half of 4
			}
			else
			if (iPass == 2)
			{
				pPageOrder = (bSideFold ? &HalfSideFoldPass2Order[0] : &HalfTopFoldPass2Order[0]);
				nPages = 2; // Half of 4
			}
			else
			if (iPass == 3)
			{
				nTemplates = CPaperTemplates::GetTemplate(DOC_FULLSHEET, 0, &pPaperTemplate);
				bPortrait = m_pAGDoc->IsPortrait(4); // Redefine portrait to match the first extra page (page 4)
				pPageOrder = NULL;
				nPages -= 4;
				iStart = 4;
				bCenterOnly = true;
			}
			break;
		}
		case DOC_IRONON:
		{
			bMirror = true;
			break;
		}
		case DOC_QUARTERCARD:
		{
			bool bSideFold = bPortrait;
			if (iPass == 1)
			{
				pPageOrder = (bSideFold ? &QuarterSideFoldOrder[0] : &QuarterTopFoldOrder[0]);
				nPages = 4;
			}
			else
			if (iPass == 3)
			{
				nTemplates = CPaperTemplates::GetTemplate(DOC_FULLSHEET, 0, &pPaperTemplate);
				bPortrait = m_pAGDoc->IsPortrait(4); // Redefine portrait to match the first extra page (page 4)
				pPageOrder = NULL;
				nPages -= 4;
				iStart = 4;
				bCenterOnly = true;
			}
			break;
		}
		case DOC_POSTCARD:
		{
			if (iPass == 1)
				pPageOrder = &PostcardPass1Order[0];
			else
				pPageOrder = &PostcardPass2Order[0];
			nPages = 1; // Half of 2
			break;
		}
		case DOC_TRIFOLD:
		{
			if (iPass == 1)
				pPageOrder = (bPortrait ? &TrifoldPortraitPass1Order[0] : &TrifoldLandscapePass1Order[0]);
			else
				pPageOrder = (bPortrait ? &TrifoldPortraitPass2Order[0] : &TrifoldLandscapePass2Order[0]);
			nPages = 3; // Half of 6
			break;
		}
	}

	if (!pPaperTemplate)
		return false;

	bool bReverseOrientation = (m_PrintDocType != DOC_ENVELOPE ? pPaperTemplate->bReverseOrientation : (CPaperFeed::GetEdge(m_iPaperFeedCode) == IDC_PAPERFEED_EDGE_TOP));
	bPortraitPaper = (bReverseOrientation ? !bPortrait : bPortrait); // This is returned to the caller

	if (!m_pDevMode)
		return false;

	DEVMODE DevModeSaved = *m_pDevMode;

	// Always set the width, height, and orientation
	m_pDevMode->dmOrientation = (bPortraitPaper ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE);
	m_pDevMode->dmFields |= DM_ORIENTATION;

	// Figure out the device copy count
	bool bCanUseDeviceCopies = (nPages == pPaperTemplate->nRows * pPaperTemplate->nCols);
	int nCopies = (bCanUseDeviceCopies ? 1 : m_pDevMode->dmCopies);
	int nDeviceCopies = (bCanUseDeviceCopies ? m_pDevMode->dmCopies : 1);

	m_pDevMode->dmCopies = nDeviceCopies;
	m_pDevMode->dmFields |= DM_COPIES;

	bool bMediaTypeDefined = !!(m_pDevMode->dmFields & DM_MEDIATYPE);
	bool bPrintQualityDefined = !!(m_pDevMode->dmFields & DM_PRINTQUALITY);
	bool bNeedsPhotoPaper = (m_PrintDocType == DOC_HOLIDAYCARD || m_PrintDocType == DOC_PHOTOCARD);

	if (bNeedsPhotoPaper)
	{
		if (!bMediaTypeDefined || (bMediaTypeDefined && m_pDevMode->dmMediaType == DMMEDIA_STANDARD))
		{
			m_pDevMode->dmMediaType = DMMEDIA_GLOSSY;
			m_pDevMode->dmFields |= DM_MEDIATYPE;
			bMediaTypeDefined = true;
		}

#ifdef NOTUSED //j Not sure I want to be overriding the user's settings just yet
		if (!bPrintQualityDefined || (bPrintQualityDefined && m_pDevMode->dmPrintQuality == DMRES_MEDIUM))
		{
			m_pDevMode->dmPrintQuality = DMRES_HIGH;
			m_pDevMode->dmFields |= DM_PRINTQUALITY;
			bPrintQualityDefined = true;
		}
#endif NOTUSED //j
	}

	// A hack to fix a bug in some HP printer drivers in which a paper sensor error occurs when
	// the Paper Type (media type) is set to Automatic and custom paper is inserted
	// Change it to anything else to get around it
 	if (bMediaTypeDefined && m_pDevMode->dmMediaType == 1041/*HP PhotoSmart Automatic*/ && IsCustomPaper())
	{
		if (bNeedsPhotoPaper)
		{
			m_pDevMode->dmMediaType = 1011/*HP Premium Plus Photo Paper*/; //j DMMEDIA_GLOSSY
			m_pDevMode->dmPrintQuality = DMRES_HIGH;
			m_pDevMode->dmFields |= DM_PRINTQUALITY;
		}
		else
			m_pDevMode->dmMediaType = 1005/*HP Premium Paper*/; //j DMMEDIA_STANDARD;
	}

	// A hack to fix a bug in some HP printer drivers in which a color reversal occurs when
	// printing transparent images
	if (bPrintQualityDefined && m_pDevMode->dmPrintQuality == DMRES_HIGH && m_pAGDoc->HasTransparentImages())
	{
		CString str = m_strDevice;
		str.MakeLower();
		bool bMatch = str.Find("deskjet ") >= 0 && (
				str.Find("3300") >= 0 || // Malibu New
				str.Find("3320") >= 0 || // Malibu
				str.Find("3400") >= 0 || // Malibu New
				str.Find("3420") >= 0 || // Malibu
				str.Find("3425") >= 0 || // Malibu
				str.Find("5500") >= 0 || // Malibu New
				str.Find("5550") >= 0 || // Malibu
				str.Find("5551") >= 0 || // Malibu
				str.Find("9600") >= 0 || // Malibu
				str.Find("9650") >= 0 || // Malibu
				str.Find("9670") >= 0 || // Malibu
				str.Find("9680") >= 0 || // Malibu
				0);
		if (!bMatch)
			 bMatch = str.Find("officejet ") >= 0 && (
				str.Find("4100") >= 0 || // Crossbow New
				str.Find("4110") >= 0 || // Crossbow
				str.Find("4200") >= 0 || // Crossbow
				str.Find("6100") >= 0 || // Malibu New
				str.Find("6110") >= 0 || // Malibu
				0);
		if (!bMatch)
			 bMatch = str.Find("psc ") >= 0 && (
				str.Find("1000") >= 0 || // Crossbow
				str.Find("1100") >= 0 || // Crossbow
				str.Find("1200") >= 0 || // Crossbow New
				str.Find("1210") >= 0 || // Crossbow
				str.Find("2100") >= 0 || // Malibu New
				str.Find("2105") >= 0 || // Malibu
				str.Find("2110") >= 0 || // Malibu
				str.Find("2150") >= 0 || // Malibu
				str.Find("2170") >= 0 || // Malibu
				str.Find("2175") >= 0 || // Malibu
				str.Find("2200") >= 0 || // Malibu New
				str.Find("2205") >= 0 || // Malibu
				str.Find("2210") >= 0 || // Malibu
				0);
		if (!bMatch)
			 bMatch = str.Find("photosmart ") >= 0 && (
				str.Find("100")  >= 0 ||
				str.Find("130")  >= 0 ||
				str.Find("230")  >= 0 ||
				str.Find("7100") >= 0 || // Malibu New
				str.Find("7150") >= 0 || // Malibu
				str.Find("7300") >= 0 || // Malibu New
				str.Find("7350") >= 0 || // Malibu
				str.Find("7500") >= 0 || // Malibu New
				str.Find("7550") >= 0 || // Malibu
				0);
		if (bMatch)
		{
			m_pDevMode->dmPrintQuality = DMRES_MEDIUM;
			m_pDevMode->dmFields |= DM_PRINTQUALITY;
			bPrintQualityDefined = true;
		}
	}

	// SetPaperSize stuffs m_pDevMode with the required paper size
	SetPaperSize(pPaperTemplate, m_pAGDoc, false/*bSilent*/);

	CAGPrintDC* pDC = pPrintDC;
	if (!pDC)
		pDC = new CAGPrintDC(m_strDriver, m_strDevice, m_strPort, m_pDevMode);

	*m_pDevMode = DevModeSaved; // Restore the DevMode before we leave

	if (!pDC)
		return false;

	if (!pDC->StartDoc(g_szAppName))
	{
		if (pDC != pPrintDC) delete pDC;
		return false;
	}

	bool bSent = Send(pPageOrder, iStart, nPages, *pPaperTemplate, bPortraitPaper, bMirror, bCenterOnly, nCopies, pDC);
	if (!pDC->EndDoc())
		pDC->AbortDoc();

	if (pDC != pPrintDC) delete pDC;
	return bSent;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::Send(const int* pPageOrder, int iStart, int nPages, const PaperTemplate& PaperTemplate, bool bPortraitPaper, bool bMirror, bool bCenterOnly, int nCopies, CAGPrintDC* pDC)
{
	int nTotalImpressions = nCopies * nPages;
	int nRows = (bPortraitPaper ? PaperTemplate.nRows : PaperTemplate.nCols);
	int nCols = (bPortraitPaper ? PaperTemplate.nCols : PaperTemplate.nRows);

	CAGDIBSectionDC* pMirrorDC = (bMirror ? CreateMirrorDib(pDC) : NULL);

	bool bSuccess = true;
	int nImpressions = 0;
	while (nImpressions < nTotalImpressions)
	{
		if (!pDC->StartPage())
		{
			CMessageBox::Message("Failed to start the print job");
			pDC->AbortDoc();
			bSuccess = false;
			break;
		}

		for (int r = 0; r < nRows && nImpressions < nTotalImpressions; r++)
		{
			for (int c = 0; c < nCols && nImpressions < nTotalImpressions; c++)
			{
				int nPageToPrint = nImpressions;
				if (iStart)
				{
					int nWraps = nImpressions / nPages;
					nPageToPrint += (iStart * (nWraps + 1));
				}

				bool bRotate180 = false;
				if (pPageOrder)
				{
					int n = *(pPageOrder + (nPageToPrint % nPages));
					if (n < 0)
					{
						bRotate180 = true;
						n = -n;
					}
					nPageToPrint = (n-1);
				}

				nPageToPrint %= m_pAGDoc->GetNumPages();
				CAGPage* pPage = m_pAGDoc->GetPage(nPageToPrint);

				RECT DestRect = {0,0,0,0};

				bool bTemplateRotated = ((PaperTemplate.fWidth <= PaperTemplate.fHeight) != pPage->IsPortrait());
				if (!bTemplateRotated)
				{
					double fLeft = PaperTemplate.fLeft + (c * PaperTemplate.fWidthWithGap);
					double fTop  = PaperTemplate.fTop + (r * PaperTemplate.fHeightWithGap);
					DestRect.left   = INCHES(fLeft);
					DestRect.top    = INCHES(fTop);
					DestRect.right  = INCHES(fLeft + PaperTemplate.fWidth);
					DestRect.bottom = INCHES(fTop + PaperTemplate.fHeight);
				}
				else
				{
					double fLeft = PaperTemplate.fTop + (c * PaperTemplate.fHeightWithGap);
					double fTop  = PaperTemplate.fLeft + (r * PaperTemplate.fWidthWithGap);
					DestRect.left   = INCHES(fLeft);
					DestRect.top    = INCHES(fTop);
					DestRect.right  = INCHES(fLeft + PaperTemplate.fHeight);
					DestRect.bottom = INCHES(fTop + PaperTemplate.fWidth);
				}

				// Get the paper size (DC size) in tenths of a millimeter
				CAGDCInfo Info = pDC->GetDeviceInfo();
				int x = (Info.m_Size.cx * 254) / Info.m_iResolutionX;
				int y = (Info.m_Size.cy * 254) / Info.m_iResolutionY;
				int xPaperSize = min(x, y);
				int yPaperSize = max(x, y);

				// Get the paper size required by the document in tenths of a millimeter
				SIZE PaperSize = {0,0};
				m_pAGDoc->GetPaperSize(PaperSize, PaperTemplate.fCustomWidth, PaperTemplate.fCustomHeight);
				int xPaperSizeRequired = min(PaperSize.cx, PaperSize.cy);
				int yPaperSizeRequired = max(PaperSize.cx, PaperSize.cy);

				int xPaperExtra = xPaperSize - xPaperSizeRequired;
				int yPaperExtra = yPaperSize - yPaperSizeRequired;
				bool bMatch = (abs(xPaperExtra) <= 20 && abs(yPaperExtra) <= 20);
				if (!bMatch)
				{
					int iDeviceWidth  = INCHES(Info.m_Size.cx, Info.m_iResolutionX);
					int iDeviceHeight = INCHES(Info.m_Size.cy, Info.m_iResolutionX);

					if (m_PrintDocType == DOC_ENVELOPE)
						OrientEnvelope(bPortraitPaper, DestRect, iDeviceWidth, iDeviceHeight, bRotate180);
					else
					if (IsCustomPaper())
						OrientCustomPaper(bPortraitPaper, DestRect, iDeviceWidth, iDeviceHeight, INCHES(PaperTemplate.fCustomWidth), INCHES(PaperTemplate.fCustomHeight));
				}

				SIZE PageSize = {0,0};
				pPage->GetPageSize(PageSize);
				RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
				CAGMatrix ViewMatrix;
				if (bCenterOnly)
					ViewMatrix.Center(DestRect, PageRect);
				else
					ViewMatrix.ScaleToFit(DestRect, PageRect);

				if (bRotate180)
				{
					ViewMatrix.Scale(-1.0/*fScaleX*/, -1.0/*fScaleY*/,
						(DestRect.left + DestRect.right) / 2,
						(DestRect.top + DestRect.bottom) / 2);
				}

				pDC->SetViewingMatrix(ViewMatrix);
				pDC->SetClipToView(PageSize, false/*bIncludeRawDC*/, NULL/*pViewRect*/);

				bool bMirrorError = false;
				if (pMirrorDC)
				{
					bMirrorError = true;
					if (nImpressions != 0)
						pMirrorDC->Clear();
					pPage->Draw(*pMirrorDC, 0/*dwFlags*/);
					BITMAPINFOHEADER* pDIB = pMirrorDC->GetDibHeader();
					if (!pDIB)
						CMessageBox::Message("Failed to get mirrored bitmap");
					BYTE* pBits = pMirrorDC->GetBits();
					if (!pBits)
						CMessageBox::Message("Failed to get mirrored bits");
					if (pDIB && pBits)
					{
						BITMAPINFOHEADER* pDIBNew = CDib::Orient(pDIB, 0/*iRotateDirection*/, true/*bFlipX*/, false/*bFlipY*/, pBits);
						if (!pDIBNew)
							CMessageBox::Message("Failed to perform the mirroring operation");
						if (pDIBNew)
						{							
							int DstWidth  =  pDC->GetDeviceInfo().m_Size.cx;
							int DstHeight =  pDC->GetDeviceInfo().m_Size.cy;
							int DstLeft   = -pDC->GetDeviceInfo().m_Offset.cx;
							int DstTop    = -pDC->GetDeviceInfo().m_Offset.cy;
							int SrcWidth  =  pMirrorDC->GetDeviceInfo().m_Size.cx;
							int SrcHeight =  pMirrorDC->GetDeviceInfo().m_Size.cy;
							int SrcLeft   =  0;
							int SrcTop    =  0;
							int iLinesCopied = ::StretchDIBits(pDC->GetHDC(), DstLeft, DstTop, DstWidth, DstHeight,
								SrcLeft, SrcTop, SrcWidth, SrcHeight, DibPtr(pDIBNew), (BITMAPINFO*)pDIBNew, DIB_RGB_COLORS, SRCCOPY);
							if (iLinesCopied < 0)
								CMessageBox::Message("Failed to output the mirroring bitmap");
							if (iLinesCopied >= 0)
								bMirrorError = false;
							free(pDIBNew);
						}
					}
				}

				if (!pMirrorDC || bMirrorError)
					pPage->Draw(*pDC, 0/*dwFlags*/);

				nImpressions++;
			}
		}

		if (!pDC->EndPage())
		{
			pDC->AbortDoc();
			bSuccess = false;
			break;
		}
	}

	if (pMirrorDC)
		delete pMirrorDC;

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
void CPrint::OrientEnvelope(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, bool& bRotate180)
{
	int iPos = CPaperFeed::GetPos(m_iPaperFeedCode);
	int iFace = CPaperFeed::GetFace(m_iPaperFeedCode);
	int iEdge = CPaperFeed::GetEdge(m_iPaperFeedCode);

	// Convert to face up coordinates
	if (iFace == IDC_PAPERFEED_FACE_DOWN)
	{
		if (iPos  == IDC_PAPERFEED_POS_LEFT)	iPos  = IDC_PAPERFEED_POS_RIGHT; else
		if (iPos  == IDC_PAPERFEED_POS_RIGHT)	iPos  = IDC_PAPERFEED_POS_LEFT;
		if (iEdge == IDC_PAPERFEED_EDGE_LEFT)	iEdge = IDC_PAPERFEED_EDGE_RIGHT; else
		if (iEdge == IDC_PAPERFEED_EDGE_RIGHT)	iEdge = IDC_PAPERFEED_EDGE_LEFT;
	}

	// If the right edge faces the printer, we must rotate 180
	bRotate180 = (iEdge == IDC_PAPERFEED_EDGE_RIGHT);

	int ox = 0;
	int oy = 0;

	// Special case if the top faces the printer
	if (iEdge != IDC_PAPERFEED_EDGE_TOP)
	{
		int iWidth = WIDTH(DestRect);
		int iHeight = HEIGHT(DestRect);

		// Assume that the narrow side of the envelope enters the printer first
		int ds = min(iDeviceWidth, iDeviceHeight) - min(iWidth, iHeight);

		if (bPortraitPaper)
		{
			if (iPos == IDC_PAPERFEED_POS_LEFT)		ox = 0;		else
			if (iPos == IDC_PAPERFEED_POS_CENTER)	ox = ds/2;	else
			if (iPos == IDC_PAPERFEED_POS_RIGHT)	ox = ds;

			oy = 0;
		}
		else
		{
			if (iPos == IDC_PAPERFEED_POS_LEFT)		oy = ds;	else
			if (iPos == IDC_PAPERFEED_POS_CENTER)	oy = ds/2;	else
			if (iPos == IDC_PAPERFEED_POS_RIGHT)	oy = 0;

			ox = 0;
		}
	}

	::OffsetRect(&DestRect, -DestRect.left, -DestRect.top);
	::OffsetRect(&DestRect, ox, oy);
}

//////////////////////////////////////////////////////////////////////
void CPrint::OrientCustomPaper(bool bPortraitPaper, RECT& DestRect, int iDeviceWidth, int iDeviceHeight, int iWidth, int iHeight)
{
	int iPos = CPaperFeed::GetPos(m_iPaperFeedCode);

	// If we print on the side facing down, flip the position
	if (m_iDuplex > 2)
	{
		if (iPos == IDC_PAPERFEED_POS_LEFT)
			iPos = IDC_PAPERFEED_POS_RIGHT;
		else
		if (iPos == IDC_PAPERFEED_POS_RIGHT)
			iPos = IDC_PAPERFEED_POS_LEFT;
	}

	int ox = 0;
	int oy = 0;

	// Assume that the narrow side of the paper enters the printer first
	int ds = min(iDeviceWidth, iDeviceHeight) - min(iWidth, iHeight);

	if (bPortraitPaper)
	{
		if (iPos == IDC_PAPERFEED_POS_LEFT)		ox = 0;		else
		if (iPos == IDC_PAPERFEED_POS_CENTER)	ox = ds/2;	else
		if (iPos == IDC_PAPERFEED_POS_RIGHT)	ox = ds;

		oy = 0;
	}
	else
	{
		if (iPos == IDC_PAPERFEED_POS_LEFT)		oy = ds;	else
		if (iPos == IDC_PAPERFEED_POS_CENTER)	oy = ds/2;	else
		if (iPos == IDC_PAPERFEED_POS_RIGHT)	oy = 0;

		ox = 0;
	}

	::OffsetRect(&DestRect, ox, oy);
}

//////////////////////////////////////////////////////////////////////
CAGDIBSectionDC* CPrint::CreateMirrorDib(const CAGPrintDC* pDC)
{
	CAGDCInfo DeviceInfo = pDC->GetDeviceInfo();

	// Because mirrroring the output consumes so much memory,
	// protect against an unreasonably high print resolution
	const kMaxResolution = 300;

	if (DeviceInfo.m_iResolutionX > kMaxResolution)
	{
		CMessageBox::Message(String("Downsizing the mirrored resolution from %d to %d", DeviceInfo.m_iResolutionX, kMaxResolution));

		double fReductionFactor = (double)kMaxResolution / DeviceInfo.m_iResolutionX;
		DeviceInfo.m_Size.cx = dtoi(fReductionFactor * DeviceInfo.m_Size.cx);
		DeviceInfo.m_iResolutionX = kMaxResolution;
	}
	if (DeviceInfo.m_iResolutionY > kMaxResolution)
	{
		double fReductionFactor = (double)kMaxResolution / DeviceInfo.m_iResolutionY;
		DeviceInfo.m_Size.cy = dtoi(fReductionFactor * DeviceInfo.m_Size.cy);
		DeviceInfo.m_iResolutionY = kMaxResolution;
	}

	BITMAPINFO bi;
	::ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth  = DeviceInfo.m_Size.cx;
	bi.bmiHeader.biHeight = DeviceInfo.m_Size.cy;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biXPelsPerMeter = dtoi(39.37 * DeviceInfo.m_iResolutionX);
	bi.bmiHeader.biYPelsPerMeter = dtoi(39.37 * DeviceInfo.m_iResolutionY);
	bi.bmiHeader.biClrUsed = 0;

	CAGDIBSectionDC* pMirrorDC = new CAGDIBSectionDC(bi, DIB_RGB_COLORS, pDC->GetHDC());
	if (!pMirrorDC)
	{
		CMessageBox::Message("Failed to create the mirror device context");
		return NULL;
	}

	if (!pMirrorDC->GetHDC())
	{
		CMessageBox::Message("Failed to get the handle to the mirror device context");
		delete pMirrorDC;
		return NULL;
	}

	return pMirrorDC;
}

//////////////////////////////////////////////////////////////////////
int CPrint::FindPaperSizeMatch(SIZE& PaperSize, int iBorderless, WORD* pw, POINT* ps, PN* pn, int nPapers)
{
	// Match the required paper size with ONE of the printer's paper sizes
	int xPaperSizeRequired = min(PaperSize.cx, PaperSize.cy);
	int yPaperSizeRequired = max(PaperSize.cx, PaperSize.cy);

	for (int i=0; i<nPapers; i++)
	{
		#ifdef _DEBUG
			ATLTRACE("Paper Type %d (%d,%d): %s\n", pw[i], ps[i].x, ps[i].y, pn[i].name);
		#endif _DEBUG

		bool bBorderlessTemplate = (CString(pn[i].name).Find("orderless") >= 0); // leave off the 'b' to avoid case-sensitivity
		bool bBorderlessMatch = (iBorderless < 0 /*don't care*/ ? true : (iBorderless > 0) == bBorderlessTemplate);
		if (!bBorderlessMatch)
			continue;

		// Get the paper size (DC size) in tenths of a millimeter
		int x = ps[i].x;
		int y = ps[i].y;
		int xPaperSize = min(x, y);
		int yPaperSize = max(x, y);

		int xPaperExtra = xPaperSize - xPaperSizeRequired;
		int yPaperExtra = yPaperSize - yPaperSizeRequired;

		bool bMatch = (abs(xPaperExtra) <= 20 && abs(yPaperExtra) <= 20);
		if (bMatch)
		{
			int iPaperIndex = pw[i];
			PaperSize.cx = x;
			PaperSize.cy = y;
			return iPaperIndex;
		}
	}
	
	return -1;
}

//////////////////////////////////////////////////////////////////////
int CPrint::FindPaperSizeClosest(SIZE& PaperSize, int iBorderless, WORD* pw, POINT* ps, PN* pn, int nPapers)
{
	// Match the required paper size with the CLOSEST printer's paper sizes
	int xPaperSizeRequired = min(PaperSize.cx, PaperSize.cy);
	int yPaperSizeRequired = max(PaperSize.cx, PaperSize.cy);
	int iTarget = -1;
	int xLowestScore = 999999;
	int yLowestScore = 999999;

	for (int i=0; i<nPapers; i++)
	{
		bool bBorderlessTemplate = (CString(pn[i].name).Find("orderless") >= 0); // leave off the 'b' to avoid case-sensitivity
		bool bBorderlessMatch = (iBorderless < 0 /*don't care*/ ? true : (iBorderless > 0) == bBorderlessTemplate);
		if (!bBorderlessMatch)
			continue;

		// Get the paper size (DC size) in tenths of a millimeter
		int x = ps[i].x;
		int y = ps[i].y;
		int xPaperSize = min(x, y);
		int yPaperSize = max(x, y);

		int xPaperExtra = xPaperSize - xPaperSizeRequired;
		int yPaperExtra = yPaperSize - yPaperSizeRequired;
		bool bBigEnough = (xPaperExtra >= -20 && yPaperExtra >= -20);
		if (!bBigEnough)
			continue;

		int xScore = abs(xPaperExtra);
		int yScore = abs(yPaperExtra);
		if (xScore < xLowestScore || (xScore == xLowestScore && yScore < yLowestScore))
		{
			iTarget = i;
			xLowestScore = xScore;
			yLowestScore = yScore;
		}
	}
	
	if (iTarget >= 0)
	{
		int iPaperIndex = pw[iTarget];
		PaperSize.cx = ps[iTarget].x;
		PaperSize.cy = ps[iTarget].y;
		return iPaperIndex;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsBorderlessSupported(LPCSTR strDevice, LPCSTR strPort)
{ // Works for HP, Dell/Lexmark - not for Epson or Canon
	// Note: strPort must be the correct one for the passed device or the results will be unpredictable

	// Get the number of paper sizes supported by the driver
	int n1 = ::DeviceCapabilities(strDevice, strPort, DC_PAPERS, NULL, m_pDevMode);
	int n2 = ::DeviceCapabilities(strDevice, strPort, DC_PAPERSIZE, NULL, m_pDevMode);
	int nPapers = min(n1, n2);
	if (nPapers <= 0)
		return false;

	// Get the arrays of values that define the paper names
	PN* pn = new PN[nPapers];
	::DeviceCapabilities(strDevice, strPort, DC_PAPERNAMES, (char*)pn, m_pDevMode);

	bool bIsBorderlessSupported = IsBorderlessSupported(pn, nPapers);
	delete [] pn;
	return bIsBorderlessSupported;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsBorderlessSupported(PN* pn, int nPapers)
{ // Works for HP, Dell/Lexmark - not for Epson or Canon
	for (int i=0; i<nPapers; i++)
	{
		bool bBorderlessTemplate = (CString(pn[i].name).Find("orderless") >= 0); // leave off the 'b' to avoid case-sensitivity
		if (bBorderlessTemplate)
			return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////
// SetPaperSize sets the required paper size into the devmode structure
bool CPrint::SetPaperSize(const PaperTemplate* pPaperTemplate, CAGDoc* pAGDoc, bool bSilent)
{
	if (!m_pDevMode)
		return false;

	if (!pAGDoc)
		return SetDevModeFullSheet();

	if (!pPaperTemplate)
	{
		int nTemplates = CPaperTemplates::GetTemplate(m_pAGDoc->GetDocType(), 0/*iPaperType*/, &pPaperTemplate);
		if (!pPaperTemplate)
			return SetDevModeFullSheet();
	}

	// Get the number of paper sizes supported by the driver
	int n1 = ::DeviceCapabilities(m_strDevice, m_strPort, DC_PAPERS, NULL, m_pDevMode);
	int n2 = ::DeviceCapabilities(m_strDevice, m_strPort, DC_PAPERSIZE, NULL, m_pDevMode);
	int nPapers = min(n1, n2);
	if (nPapers <= 0)
		return SetDevModeFullSheet();

	// Get the arrays of values that define the paper names
	PN* pn = new PN[nPapers];
	::DeviceCapabilities(m_strDevice, m_strPort, DC_PAPERNAMES, (char*)pn, m_pDevMode);

	// Compute whether borderless is supported for this device -  Works for HP, Dell/Lexmark - not for Epson or Canon
	bool bBorderlessSupported = IsBorderlessSupported(pn, nPapers);

	// Decide whether we should search the paper sizes to select the exact size
	// Under normal circumstances we shouldn't bother because it could screw up envelopes
	bool bBorderless = IsDriverBorderless() || (m_bForceBorderless && bBorderlessSupported);
	bool bCustomPaper = IsCustomPaper();
	bool bDoPaperSearch = bBorderless || bCustomPaper;
	if (!bDoPaperSearch)
	{
		delete [] pn;
		return SetDevModeFullSheet();
	}

	// Get the arrays of values that define the paper sizes and names
	WORD* pw = new WORD[nPapers];
	::DeviceCapabilities(m_strDevice, m_strPort, DC_PAPERS, (char*)pw, m_pDevMode);

	POINT* ps = new POINT[nPapers];
	::DeviceCapabilities(m_strDevice, m_strPort, DC_PAPERSIZE, (char*)ps, m_pDevMode);

	CString str = m_strDevice;
	str.MakeLower();
	bool bIsHpPrinter = (str.Find("hp ") >= 0);

	// Get the paper size required by the document in tenths of a millimeter
	SIZE PaperSize = {0,0};
	pAGDoc->GetPaperSize(PaperSize, pPaperTemplate->fCustomWidth, pPaperTemplate->fCustomHeight);

	// Search the paper templates with the same borderless attribute for the exact size
	int iPaperIndex = FindPaperSizeMatch(PaperSize, bBorderless, pw, ps, pn, nPapers);

	// If nothing was found, and we are using custom paper, default to a full sheet
	if (iPaperIndex < 0 && bCustomPaper)
	{
		// To avoid the HP Custom size bug that causes stretching, or Epson problems that cause clipping,
		// look for a full sheet, instead of Custom 4x8's and 6x8's
		// Also, to avoid paper sensor errors on HP printers, select A5 (5.8 x 8.3) and not LTR
		PaperSize.cx = dtoi(254.0 * (bIsHpPrinter ? A5W : L1W));
		PaperSize.cy = dtoi(254.0 * (bIsHpPrinter ? A5H : L1H));
	}

	// If nothing was found, search again with the same borderless attribute for the closest size
	if (iPaperIndex < 0)
		iPaperIndex = FindPaperSizeClosest(PaperSize, bBorderless, pw, ps, pn, nPapers);

	// If nothing was found, search again without regard to the borderless attribute for the exact size 
	if (iPaperIndex < 0)
	{
		if (!bSilent && bBorderless && bIsHpPrinter)
			CMessageBox::Message(String("Your printer driver does not support borderless printing for this paper size.\n\nYour card will print normally."));

		iPaperIndex = FindPaperSizeMatch(PaperSize, -1/*don't care*/, pw, ps, pn, nPapers);
	}

	// If nothing was found, search again without regard to the borderless attribute for the closest size 
	if (iPaperIndex < 0)
		iPaperIndex = FindPaperSizeClosest(PaperSize, -1/*don't care*/, pw, ps, pn, nPapers);

	delete [] pw;
	delete [] ps;
	delete [] pn;

	// If we still didn't find a match, we need to define a custom size
	if (iPaperIndex < 0)
	{
		//j Not all NT/XP printer drivers seem to respond to the following method of setting custom paper sizes
		//j HP confirmed that they don't  
		//j	if (SetDevModeSizeNT((LPSTR)(LPCSTR)m_strDevice, PaperSize))
		//j		return true;
		//j
		//j iPaperIndex = DMPAPER_USER;
		//j Since we are not sure if the DMPAPER_USER method works for all drivers and OS's, default to the full sheet size for now

		return SetDevModeFullSheet();
	}

	return SetDevModeSize(iPaperIndex, PaperSize);
}

//////////////////////////////////////////////////////////////////////
bool CPrint::SetDevModeFullSheet()
{
	int iPaperIndex = (CAGDocSheetSize::GetMetric() ? DMPAPER_A4 : DMPAPER_LETTER);

	m_pDevMode->dmPaperSize = iPaperIndex;
	m_pDevMode->dmFields |= DM_PAPERSIZE;

	m_pDevMode->dmPaperWidth = 0;
	m_pDevMode->dmFields &= ~DM_PAPERWIDTH;

	m_pDevMode->dmPaperLength = 0;
	m_pDevMode->dmFields &= ~DM_PAPERLENGTH;

	m_pDevMode->dmFields &= ~DM_FORMNAME;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsHpMombiDriver()
{
	// Get the printer handle
	PRINTER_DEFAULTS pd;
	::ZeroMemory(&pd, sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;

	HANDLE hPrinter = NULL;
	bool bOK = !!::OpenPrinter((LPSTR)(LPCSTR)m_strDevice, &hPrinter, &pd);
	if (!hPrinter) // Unable to open printer, won’t be able to get printer data
		return false;

	//	Query the driver to see it it is an HP Mombi driver; it just has to have any "ConvertTicketModule" data
	DWORD dwType = 0;
	BYTE cData = 0;
	DWORD dwSizeNeeded = 0;
	DWORD dwResult = ::GetPrinterData(hPrinter, "ConvertTicketModule", &dwType, (LPBYTE)&cData, sizeof(cData), &dwSizeNeeded);
	bool bIsHpMombi = (dwResult == ERROR_MORE_DATA && dwType == REG_SZ);

#ifdef NOTUSED
	dwResult = ::GetPrinterDriver(hPrinter, NULL, 6/*LevelReturned*/, NULL, 0, &dwSizeNeeded);
	DWORD dwBufferSize = dwSizeNeeded;
	BYTE* pBuffer = new BYTE[dwBufferSize];
	::ZeroMemory(pBuffer, dwBufferSize);
	dwResult = ::GetPrinterDriver(hPrinter, NULL, 6/*LevelReturned*/, pBuffer, dwBufferSize, &dwSizeNeeded);
	DRIVER_INFO_6* pdi = (DRIVER_INFO_6*)pBuffer;
	delete pBuffer;
#endif NOTUSED

	::ClosePrinter(hPrinter);
	return bIsHpMombi;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::SetDevModeSize(int iPaperIndex, const SIZE& PaperSize)
{
	// Setup the devmode variables for desired paper index and/or size
	if (iPaperIndex <= 0)
		return SetDevModeFullSheet();

	m_pDevMode->dmPaperSize = iPaperIndex;
	m_pDevMode->dmFields |= DM_PAPERSIZE;

	if (IsHpMombiDriver())
	{
		m_pDevMode->dmPaperWidth = 0;
		m_pDevMode->dmFields &= ~DM_PAPERWIDTH;

		m_pDevMode->dmPaperLength = 0;
		m_pDevMode->dmFields &= ~DM_PAPERLENGTH;
	}
	else
	{
		m_pDevMode->dmPaperWidth = (short)min(PaperSize.cx, PaperSize.cy);
		m_pDevMode->dmFields |= DM_PAPERWIDTH;

		m_pDevMode->dmPaperLength = (short)max(PaperSize.cx, PaperSize.cy);
		m_pDevMode->dmFields |= DM_PAPERLENGTH;
	}

	m_pDevMode->dmFields &= ~DM_FORMNAME;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::SetDevModeSizeNT(LPSTR pstrDevice, const SIZE& PaperSize)
{
	//j Not all NT/XP printer drivers seem to respond to the following method of setting custom paper sizes
	//j HP confirmed that they don't  
	//j So return false, and force the non-NT method of setting custom paper sizes to be performed instead
	return false;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	bool bNT = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	if (!bNT)
		return false;

	// If Windows NT, add a custom form (paper size), and setup the devmode variables to access it
	PRINTER_DEFAULTS pd;
	::ZeroMemory(&pd, sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = NULL;
	pd.pDevMode = m_pDevMode;

	HANDLE hPrinter = NULL;
	if (!::OpenPrinter(pstrDevice, &hPrinter, &pd))
		return false;

	CString strFormName = String("%s %0.3G x %0.3G", g_szAppName, (double)PaperSize.cx/254, (double)PaperSize.cy/254);
	LPSTR pFormName = (LPSTR)(LPCSTR)strFormName;
	BOOL bOK = ::DeleteForm(hPrinter, pFormName);

	FORM_INFO_1 formInfo;
	formInfo.Flags = FORM_USER /*| FORM_PRINTER*/; // Associate the form with either the user or printer in the registry
	formInfo.pName = pFormName;
	formInfo.Size.cx = PaperSize.cx * 100;
	formInfo.Size.cy = PaperSize.cy * 100;
	formInfo.ImageableArea.left = 0;
	formInfo.ImageableArea.top = 0;
	formInfo.ImageableArea.right = formInfo.Size.cx;
	formInfo.ImageableArea.bottom = formInfo.Size.cy;
	bOK = ::AddForm(hPrinter, 1/*dwLevel*/, (LPBYTE)&formInfo);
	if (!bOK) // Will fail with permissions error on network printers
	{
		::ClosePrinter(hPrinter);
		return false;
	}

	strcpy((LPSTR)m_pDevMode->dmFormName, pFormName);
	m_pDevMode->dmFields |= DM_FORMNAME;
	m_pDevMode->dmFields &= ~DM_PAPERSIZE;
	m_pDevMode->dmFields &= ~DM_PAPERWIDTH;
	m_pDevMode->dmFields &= ~DM_PAPERLENGTH;

	::ClosePrinter(hPrinter);

	return true;
}
//////////////////////////////////////////////////////////////////////
void CPrint::GetMargins(CAGDoc* pAGDoc, RECT& Margins)
{
	::SetRectEmpty(&Margins);

	if (!pAGDoc)
		return; // NULL margins

	// If the document does NOT print to the paper's edge, pass back NULL margins
	AGDOCTYPE PrintDocType = pAGDoc->GetDocType();
	switch (PrintDocType)
	{
		case DOC_BUSINESSCARD:
		case DOC_CDBOOKLET:
		case DOC_CDLABEL:
		case DOC_GIFTNAMECARD:
		case DOC_LABEL:
		case DOC_POSTCARD:
		case DOC_HOLIDAYCARD:
		case DOC_PHOTOCARD:
		{
			return; // NULL margins
		}
	}

	enum { eNoLeft = 1, eNoRight = 2, eNoTop = 4, eNoBottom = 8, eRotate180 = 16 };

	const int HalfSideFoldMarginChanges[] = {
		/*page 1*/	eNoLeft  | eRotate180,
		/*page 2*/	eNoRight | eRotate180,
		/*page 3*/	eNoLeft  | eRotate180,
		/*page 4*/	eNoRight | eRotate180,
	};

	const int HalfTopFoldMarginChanges[] = {
		/*page 1*/	eNoTop    | eRotate180,
		/*page 2*/	eNoBottom | eRotate180,
		/*page 3*/	eNoTop    | eRotate180,
		/*page 4*/	eNoTop,
	};

	const int QuarterSideFoldMarginChanges[] = {
		/*page 1*/	eNoLeft  | eNoTop | eRotate180,
		/*page 2*/	eNoRight | eNoTop,
		/*page 3*/	eNoLeft  | eNoTop,
		/*page 4*/	eNoRight | eNoTop | eRotate180,
	};

	const int QuarterTopFoldMarginChanges[] = {
		/*page 1*/	eNoLeft  | eNoTop | eRotate180,
		/*page 2*/	eNoLeft  | eNoBottom,
		/*page 3*/	eNoLeft  | eNoTop,
		/*page 4*/	eNoRight | eNoTop,
	};

	const int TrifoldPortraitMarginChanges[] = {
		/*page 1*/	eNoLeft            | eRotate180,
		/*page 2*/	eNoRight		   | eRotate180,
		/*page 3*/	eNoLeft | eNoRight | eRotate180,
		/*page 4*/	eNoLeft            | eRotate180,
		/*page 5*/	eNoRight           | eRotate180,
		/*page 6*/	eNoLeft | eNoRight | eRotate180,
	};

	const int TrifoldLandscapeMarginChanges[] = {
		/*page 1*/	eNoTop             | eRotate180,
		/*page 2*/	eNoBottom          | eRotate180,
		/*page 3*/	eNoTop | eNoBottom | eRotate180,
		/*page 4*/	eNoTop             | eRotate180,
		/*page 5*/	eNoBottom          | eRotate180,
		/*page 6*/	eNoTop | eNoBottom | eRotate180,
	};

	bool bReverseOrientation = false;
	int nPage = pAGDoc->GetCurrentPageNum();
	bool bPortrait = pAGDoc->IsPortrait(nPage);
	bool bSideFold = bPortrait;
	int MarginChanges = 0;

	switch (PrintDocType)
	{
		case DOC_BANNER:
		case DOC_FULLSHEET:
		{
			bReverseOrientation = false;
			break;
		}
		case DOC_BROCHURE:
		{
			bReverseOrientation = false;
			break;
		}
		case DOC_ENVELOPE:
		{
			bReverseOrientation = (CPaperFeed::GetEdge(m_iPaperFeedCode) == IDC_PAPERFEED_EDGE_TOP);
			break;
		}
		case DOC_IRONON:
		{
			bReverseOrientation = false;
			break;
		}
		case DOC_HALFCARD:
		{
			bReverseOrientation = true;
			int i = nPage % 4;
			MarginChanges = (bSideFold ? HalfSideFoldMarginChanges[i] : HalfTopFoldMarginChanges[i]);
			break;
		}
		case DOC_HOLIDAYCARD:
		{
			bReverseOrientation = true;
			break;
		}
		case DOC_NOTECARD:
		{
			bReverseOrientation = true;
			int i = nPage % 4;
			MarginChanges = (bSideFold ? HalfSideFoldMarginChanges[i] : HalfTopFoldMarginChanges[i]);
			break;
		}
		case DOC_PHOTOCARD:
		{
			bReverseOrientation = true;
			break;
		}
		case DOC_QUARTERCARD:
		{
			bReverseOrientation = false;
			int i = nPage % 4;
			MarginChanges = (bSideFold ? QuarterSideFoldMarginChanges[i] : QuarterTopFoldMarginChanges[i]);
			break;
		}
		case DOC_TRIFOLD:
		{
			bReverseOrientation = true;
			int i = nPage % 6;
			MarginChanges = (bPortrait ? TrifoldPortraitMarginChanges[i] : TrifoldLandscapeMarginChanges[i]);
			break;
		}
	}

	bool bPortraitPaper = (bReverseOrientation ? !bPortrait : bPortrait);

	if (PrintDocType == DOC_ENVELOPE)
	{
		int iPos = CPaperFeed::GetPos(m_iPaperFeedCode);
		int iFace = CPaperFeed::GetFace(m_iPaperFeedCode);
		int iEdge = CPaperFeed::GetEdge(m_iPaperFeedCode);

		// Convert to face up coordinates
		if (iFace == IDC_PAPERFEED_FACE_DOWN)
		{
			if (iPos  == IDC_PAPERFEED_POS_LEFT)	iPos  = IDC_PAPERFEED_POS_RIGHT; else
			if (iPos  == IDC_PAPERFEED_POS_RIGHT)	iPos  = IDC_PAPERFEED_POS_LEFT;
			if (iEdge == IDC_PAPERFEED_EDGE_LEFT)	iEdge = IDC_PAPERFEED_EDGE_RIGHT; else
			if (iEdge == IDC_PAPERFEED_EDGE_RIGHT)	iEdge = IDC_PAPERFEED_EDGE_LEFT;
		}

		// If the right edge faces the printer, we must rotate 180
		if (iEdge == IDC_PAPERFEED_EDGE_RIGHT)
			MarginChanges |= eRotate180;

		if (iEdge == IDC_PAPERFEED_EDGE_TOP)
		{
			switch (iPos)
			{
				case IDC_PAPERFEED_POS_LEFT:	MarginChanges |= (eNoBottom | (bPortraitPaper ? eNoRight : eNoLeft));				break;
				case IDC_PAPERFEED_POS_CENTER:	MarginChanges |= (eNoBottom | eNoLeft | eNoRight);									break;
				case IDC_PAPERFEED_POS_RIGHT:	MarginChanges |= (eNoBottom | (bPortraitPaper ? eNoLeft : eNoRight));				break;
			}
		}
		else
		if (iEdge == IDC_PAPERFEED_EDGE_LEFT)
		{
			switch (iPos)
			{
				case IDC_PAPERFEED_POS_LEFT:	MarginChanges |= (eNoRight | (bPortraitPaper ? eNoBottom : eNoTop));				break;
				case IDC_PAPERFEED_POS_CENTER:	MarginChanges |= (eNoRight | eNoBottom | (bPortraitPaper ? eNoLeft : eNoTop));		break;
				case IDC_PAPERFEED_POS_RIGHT:	MarginChanges |= (eNoBottom | (bPortraitPaper ? eNoLeft : eNoRight));				break;
			}
		}
		else
		if (iEdge == IDC_PAPERFEED_EDGE_RIGHT)
		{
			switch (iPos)
			{
				case IDC_PAPERFEED_POS_LEFT:	MarginChanges |= (eNoLeft | (bPortraitPaper ? eNoTop : eNoBottom));					break;
				case IDC_PAPERFEED_POS_CENTER:	MarginChanges |= (eNoLeft | eNoTop | (bPortraitPaper ? eNoRight : eNoBottom));		break;
				case IDC_PAPERFEED_POS_RIGHT:	MarginChanges |= (eNoTop | (bPortraitPaper ? eNoRight : eNoLeft));					break;
			}
		}
	}

	if (IsCustomPaper(PrintDocType))
	{
		int iPos = CPaperFeed::GetPos(m_iPaperFeedCode);

		// If we print on the side facing down, flip the position
		if (m_iDuplex > 2)
		{
			if (iPos == IDC_PAPERFEED_POS_LEFT)
				iPos = IDC_PAPERFEED_POS_RIGHT;
			else
			if (iPos == IDC_PAPERFEED_POS_RIGHT)
				iPos = IDC_PAPERFEED_POS_LEFT;
		}

		switch (iPos)
		{
			case IDC_PAPERFEED_POS_LEFT:	MarginChanges |= (eNoRight | (bPortraitPaper ? eNoBottom : eNoTop));				break;
			case IDC_PAPERFEED_POS_CENTER:	MarginChanges |= (eNoRight | eNoBottom | (bPortraitPaper ? eNoLeft : eNoTop));		break;
			case IDC_PAPERFEED_POS_RIGHT:	MarginChanges |= (eNoBottom | (bPortraitPaper ? eNoLeft : eNoRight));				break;
		}
	}

	Margins = (bPortraitPaper ? m_MarginsPortrait : m_MarginsLandscape);
	if (MarginChanges & eRotate180)
	{
		SWAP(Margins.left, Margins.right);
		SWAP(Margins.top, Margins.bottom);
	}

	if (MarginChanges & eNoLeft)	Margins.left   = 0;
	if (MarginChanges & eNoTop)		Margins.top    = 0;
	if (MarginChanges & eNoRight)	Margins.right  = 0;
	if (MarginChanges & eNoBottom)	Margins.bottom = 0;
}

//////////////////////////////////////////////////////////////////////
void CPrint::ActivateNewDoc()
{
	if (!m_pDocWindow)
		return;

	m_pCtp = m_pDocWindow->m_pCtp;
	m_pAGDoc = m_pDocWindow->GetDoc();
	m_PrintDocType = (m_pAGDoc ? m_pAGDoc->GetDocType() : DOC_DEFAULT);

	int nCopies = ShowDialog(NULL/*pPrintDlg*/);

	// Update the margin info (m_MarginsPortrait and m_MarginsLandscape) using the new m_pDevMode
	ComputeMargins();
}

//////////////////////////////////////////////////////////////////////
int CPrint::ShowDialog(PRINTDLG* pPrintDlg)
{
	PRINTDLG pd;
	if (pPrintDlg)
    {
		pd = *pPrintDlg;
        pd.hDevMode = m_hDevMode;
	    pd.hDevNames = m_hDevNames;
    }
	else
	{
		// We will silently get the printer defaults
		memset(&pd, 0, sizeof(pd));
		pd.lStructSize = sizeof(pd);
    	pd.hDevMode = NULL;
	    pd.hDevNames = NULL;
		pd.Flags = PD_RETURNDEFAULT;
	}

	

	// Exit if an error occurs
	// If silently getting the printer defaults, an error will occur if we already have a m_hDevMode and a m_hDevNames
	// Otherwise an error will occur if the user cancels the dialog or there is no printer installed
	if (!::PrintDlg(&pd))
	{
		DWORD dwCommonError = CommDlgExtendedError();
		return 0/*nCopies*/;
	}

	// Hold on to the new settings; ::PrintDlg should reuse or free any previous memory
	m_hDevNames = pd.hDevNames;
	m_hDevMode = pd.hDevMode;

	// Make handy local copies of the 3 DEVNAMES fields
	m_strDevice.Empty();
	m_strDriver.Empty();
	m_strPort.Empty();
	if (m_hDevNames)
	{
		DEVNAMES* pDevNames = (DEVNAMES*)::GlobalLock(m_hDevNames);
		if (pDevNames)
		{
			m_strDevice = ((char*)pDevNames) + pDevNames->wDeviceOffset;
			m_strDriver = ((char*)pDevNames) + pDevNames->wDriverOffset;
			m_strPort   = ((char*)pDevNames) + pDevNames->wOutputOffset;
			::GlobalUnlock(m_hDevNames);
		}
	}

	LoadDeviceValuesFromRegistry(m_strDevice);

	// Make a handy local copy of the DEVMODE structure (both the public and private data)
	// In order to prevent the printer driver from crashing when
	// accessing the DEVMODE, we make our own copy on the heap
	if (m_pDevMode)
	{
		delete [] m_pDevMode;
		m_pDevMode = NULL;
	}

	if (!m_hDevMode)
		return 0/*nCopies*/;

	DEVMODE* pDevMode = (DEVMODE*)::GlobalLock(m_hDevMode);
	if (pDevMode)
	{
		int iSize = ::GlobalSize(m_hDevMode);
		m_pDevMode = (DEVMODE*)new char[iSize];
		if (m_pDevMode)
			memcpy(m_pDevMode, pDevMode, iSize);

		::GlobalUnlock(m_hDevMode);
		pDevMode = NULL;
	}

	if (!m_pDevMode)
		return 0/*nCopies*/;

	return m_pDevMode->dmCopies;
}

//////////////////////////////////////////////////////////////////////
void CPrint::ComputeMargins()
{
	if (!m_pDevMode)
		return;

	DEVMODE DevModeSaved = *m_pDevMode;

	if (m_pAGDoc)
	{
		// SetPaperSize stuffs m_pDevMode with the required paper size
		SetPaperSize(NULL/*pPaperTemplate*/, m_pAGDoc, true/*bSilent*/);
	}

	m_pDevMode->dmOrientation = DMORIENT_PORTRAIT;
	m_pDevMode->dmFields |= DM_ORIENTATION;
	::SetRect(&m_MarginsPortrait, THIN_MARGIN, THICK_MARGIN, THIN_MARGIN, THICK_MARGIN);
	GetDriverMargins(m_MarginsPortrait);

	SWAP(m_pDevMode->dmPaperWidth, m_pDevMode->dmPaperLength);

	m_pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
	m_pDevMode->dmFields |= DM_ORIENTATION;
	::SetRect(&m_MarginsLandscape, THICK_MARGIN, THIN_MARGIN, THICK_MARGIN, THIN_MARGIN);
	GetDriverMargins(m_MarginsLandscape);

	*m_pDevMode = DevModeSaved; // Restore the DevMode before we leave
}

//////////////////////////////////////////////////////////////////////
void CPrint::GetDriverMargins(RECT& rMargins)
{
	HDC hDC = ::CreateIC(m_strDriver, m_strDevice, m_strPort, m_pDevMode);
	if (!hDC)
		return;

	int iResolutionX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	int iResolutionY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	int iPageWidth	 = ::GetDeviceCaps(hDC, PHYSICALWIDTH);
	int iPageHeight	 = ::GetDeviceCaps(hDC, PHYSICALHEIGHT);

	RECT r;
	r.left	 = ::GetDeviceCaps(hDC, PHYSICALOFFSETX);
	r.top	 = ::GetDeviceCaps(hDC, PHYSICALOFFSETY);
	r.right	 = iPageWidth - ::GetDeviceCaps(hDC, HORZRES) - r.left;
	r.bottom = iPageHeight - ::GetDeviceCaps(hDC, VERTRES) - r.top;

	::DeleteDC(hDC);

	rMargins.left   = INCHES(r.left,   iResolutionX);
	rMargins.right  = INCHES(r.right,  iResolutionX);
	rMargins.top    = INCHES(r.top,    iResolutionY);
	rMargins.bottom = INCHES(r.bottom, iResolutionY);
}

//////////////////////////////////////////////////////////////////////
bool CPrint::IsDriverBorderless()
{
	CRect Margins;
	GetDriverMargins(Margins);
	bool bBorderless = Margins.left <= 5 && Margins.right <= 5 && Margins.top <= 5 && Margins.bottom <= 5;
	return bBorderless;
}

//////////////////////////////////////////////////////////////////////
static void DrawArrow(CAGPrintDC* pDC, POINT Pt)
{
	POINT Pts[7];
	Pts[0] = Pt;
	Pts[1].x = Pts[0].x - INCHES(0.5);
	Pts[1].y = Pts[0].y + INCHES(0.5);
	Pts[2].x = Pts[1].x + INCHES(0.25);
	Pts[2].y = Pts[1].y;
	Pts[3].x = Pts[2].x;
	Pts[3].y = Pts[2].y + INCHES(0.5);
	Pts[4].x = Pts[3].x + INCHES(0.5);
	Pts[4].y = Pts[3].y;
	Pts[5].x = Pts[4].x;
	Pts[5].y = Pts[4].y - INCHES(0.5);
	Pts[6].x = Pts[5].x + INCHES(0.25);
	Pts[6].y = Pts[5].y;

	HBRUSH hOldBrush = (HBRUSH)::SelectObject(pDC->GetHDC(), ::GetStockObject(NULL_BRUSH));
	HPEN hPen = ::CreatePen(PS_SOLID, pDC->GetDeviceInfo().m_iResolutionX / 50, RGB(0,0,0));
	HPEN hOldPen = (HPEN) ::SelectObject(pDC->GetHDC(), hPen);

	pDC->Polygon(Pts, 7);

	::SelectObject(pDC->GetHDC(), hOldBrush);
	::SelectObject(pDC->GetHDC(), hOldPen);
	::DeleteObject(hPen);
}

//////////////////////////////////////////////////////////////////////
void CPrint::DoubleSidedTest(bool bFirstTest)
{
	if (m_strDriver.IsEmpty() || m_strDevice.IsEmpty() || m_strPort.IsEmpty() || !m_pDevMode)
		return;

	// Always set the width, height, and orientation
	DEVMODE DevModeSaved = *m_pDevMode;

	m_pDevMode->dmOrientation = DMORIENT_PORTRAIT;
	m_pDevMode->dmFields |= DM_ORIENTATION;

	m_pDevMode->dmCopies = 1;
	m_pDevMode->dmFields |= DM_COPIES;

	// High quality is not necessary
	m_pDevMode->dmMediaType = DMMEDIA_STANDARD;
	m_pDevMode->dmFields |= DM_MEDIATYPE;
	m_pDevMode->dmPrintQuality = DMRES_LOW;
	m_pDevMode->dmFields |= DM_PRINTQUALITY;

	SetDevModeFullSheet();

	CAGPrintDC* pDC = new CAGPrintDC(m_strDriver, m_strDevice, m_strPort, m_pDevMode);

	*m_pDevMode = DevModeSaved; // Restore the DevMode before we leave

	if (!pDC)
		return;

	const CAGDCInfo& DeviceInfo = pDC->GetDeviceInfo();

	do // A convenient block we can break out of
	{
		if (!pDC->StartDoc(g_szAppName + " Paper Feed Print Test"))
			break;

		if (!pDC->StartPage())
		{
			pDC->AbortDoc();
			break;
		}

		if (bFirstTest)
		{
			POINT Pt = { DeviceInfo.m_Size.cx / 2, DeviceInfo.m_iResolutionY };
			pDC->GetDeviceMatrix().Inverse().Transform(&Pt, 1, false);
			DrawArrow(pDC, Pt);

			RECT MsgRect = { 0, DeviceInfo.m_iResolutionY * 3, DeviceInfo.m_Size.cx, DeviceInfo.m_iResolutionY * 6 };
			pDC->GetDeviceMatrix().Inverse().Transform((POINT*)&MsgRect, 2, false);

			LOGFONT Font;
			memset(&Font, 0, sizeof(Font));
			Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
			lstrcpy(Font.lfFaceName, "Arial");
			Font.lfHeight = -POINTUNITS(14);

			CAGSymText* pAGSymText = new CAGSymText(ST_TEXT);
			pAGSymText->Create(MsgRect);
			pAGSymText->SetVertJust(eVertTop);

			char pRawText[] = "Please put this page back into the printer\n\n"
				"with the printed side UP\n\n"
				"and the arrow pointing TOWARD the printer.";

			// Initialize the type specs
			CAGSpec* pSpecs[] = { new CAGSpec(
				Font,			// LOGFONT& Font
				eRagCentered,	// eTSJust HorzJust
				0,				// short sLineLeadPct
				LT_None,		// int LineWidth
				CLR_NONE,		// COLORREF LineColor
				FT_Solid,		// FillType Fill
				RGB(0,0,0),		// COLORREF FillColor
				CLR_NONE)		// COLORREF FillColor2
			};

			const int pOffsets[] = {0};
			pAGSymText->SetText(pRawText, 1, pSpecs, pOffsets);
			pAGSymText->Draw(*pDC, 0/*dwFlags*/);
			delete pAGSymText;
		}
		else
		{
			POINT Pt = { DeviceInfo.m_Size.cx / 4, DeviceInfo.m_iResolutionY };
			pDC->GetDeviceMatrix().Inverse().Transform(&Pt, 1, false);
			DrawArrow(pDC, Pt);

			Pt.x = (DeviceInfo.m_Size.cx / 4) * 3;
			Pt.y = DeviceInfo.m_iResolutionY;
			pDC->GetDeviceMatrix().Inverse().Transform(&Pt, 1, false);
			DrawArrow(pDC, Pt);
		}

		if (!pDC->EndPage())
		{
			pDC->AbortDoc();
			break;
		}

		if (!pDC->EndDoc())
		{
			pDC->AbortDoc();
			break;
		}

	} while (0);

	delete pDC;
}

//////////////////////////////////////////////////////////////////////
bool CPrint::HandleDeviceChange(HWND hWnd, int& iLastDeviceChecksum)
{
	// See if the device name and port changed (compare checksums)
	char strDevice[100];
	strDevice[0] = '\0';
	::GetDlgItemText(hWnd, stc11, strDevice, sizeof(strDevice));

	char strPort[100];
	strPort[0] = '\0';
	::GetDlgItemText(hWnd, stc14, strPort, sizeof(strPort));

	int iDeviceChecksum = 0;
	int i = 0;
	while (1)
	{
		BYTE byte = strDevice[i++];
		if (!byte)
			break;
		iDeviceChecksum += byte;
	}

	i = 0;
	while (1)
	{
		BYTE byte = strPort[i++];
		if (!byte)
			break;
		iDeviceChecksum += byte;
	}

	if (iLastDeviceChecksum == iDeviceChecksum)
		return false;

	// The device/port has changed!
	iLastDeviceChecksum = iDeviceChecksum;

	// Compute whether borderless is supported for this device - Works for HP, Dell/Lexmark - not for Epson or Canon
	bool bShowBorderless = IsBorderlessSupported(strDevice, strPort);
	::ShowWindow(::GetDlgItem(hWnd, IDC_BORDERLESS), (bShowBorderless ? SW_SHOW : SW_HIDE));
	return true;
}
//////////////////////////////////////////////////////////////////////
void CPrint::InitWindow(HWND hWnd)
{
    if (!::IsWindow(hWnd))
        return;
    
	SIZE PageSize = {0,0};
	m_pAGDoc->GetPageSize(PageSize);

	::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
	::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE2), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
	::EnableWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (m_iDuplex >= 0));
	::CheckDlgButton(hWnd, IDC_DBLSIDE, (m_iDuplex < 0 ? BST_CHECKED : BST_UNCHECKED));

	::CheckDlgButton(hWnd, IDC_BORDERLESS, (m_bForceBorderless ? BST_CHECKED : BST_UNCHECKED));

	if (CAGDocSheetSize::GetMetric())
	{
		char szText[MAX_PATH];
		CString strText;

		::GetWindowText(::GetDlgItem(hWnd, IDC_HALFFOLD), szText, sizeof(szText));
		strText = szText;
		strText += " A4";
		::SetWindowText(::GetDlgItem(hWnd, IDC_HALFFOLD), strText);

		::GetWindowText(::GetDlgItem(hWnd, IDC_QUARTERFOLD), szText, sizeof(szText));
		strText = szText;
		strText += " A4";
		::SetWindowText(::GetDlgItem(hWnd, IDC_QUARTERFOLD), strText);
	}

	if (m_PrintDocType == DOC_HALFCARD)
	{
		::CheckDlgButton(hWnd, IDC_HALFFOLD, BST_CHECKED);
		::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERTYPE), SW_HIDE);
		::SetWindowText(::GetDlgItem(hWnd, IDC_PAPERTYPE_LABEL), "Card Size");
	}
	else if (m_PrintDocType == DOC_QUARTERCARD)
	{
		::CheckDlgButton(hWnd, IDC_QUARTERFOLD, BST_CHECKED);
		::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERTYPE), SW_HIDE);
		::SetWindowText(::GetDlgItem(hWnd, IDC_PAPERTYPE_LABEL), "Card Size");
	}
	else
	{
		::ShowWindow(::GetDlgItem(hWnd, IDC_HALFFOLD), SW_HIDE);
		::ShowWindow(::GetDlgItem(hWnd, IDC_QUARTERFOLD), SW_HIDE);
		CPaperTemplates::StuffCombo(m_PrintDocType, 0, NULL, GetDlgItem(hWnd, IDC_PAPERTYPE), PageSize.cx, PageSize.cy, m_pCtp->IsHpUI()/*bIgnoreCompetitors*/);
		::SetWindowText(::GetDlgItem(hWnd, IDC_PAPERTYPE_LABEL), "Paper Type");
	}



    // Create new fonts
     LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    ::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

    strcpy(lf.lfFaceName, "Microsoft Sans Serif");
    lf.lfHeight = -13;
    lf.lfWeight = FW_BOLD;
    m_hHdrFont1 = ::CreateFontIndirect(&lf);

    lf.lfHeight = -12;
	lf.lfWeight = FW_SEMIBOLD;
	m_hHdrFont2 = ::CreateFontIndirect(&lf);
	


    // Select Printer Group
    m_lblSelectPrintGroup.InitDialog(::GetDlgItem(hWnd, IDC_SELECTPRINTER_GROUP));
    m_lblSelectPrintGroup.SetFontBold(true);
    m_lblSelectPrintGroup.SetFontSize(13);
    m_lblSelectPrintGroup.SetTextColor(RGB(0x00,0x00,0xFF));
	m_lblSelectPrintGroup.SetFontName("Microsoft Sans Serif");
	m_lblSelectPrintGroup.SetBackgroundColor(CLR_NONE);

    // Copies group
    m_lblCopiesGroup.InitDialog(::GetDlgItem(hWnd, IDC_COPIES_GROUP));
    m_lblCopiesGroup.SetFontBold(true);
    m_lblCopiesGroup.SetFontSize(13);
    m_lblCopiesGroup.SetTextColor(RGB(0x00,0x00,0xFF));
	m_lblCopiesGroup.SetFontName("Microsoft Sans Serif");
	m_lblCopiesGroup.SetBackgroundColor(CLR_NONE);

     // Paper Type group
    m_lblPaperTypeGroup.InitDialog(::GetDlgItem(hWnd, IDC_PAPERTYPE_LABEL));
    m_lblPaperTypeGroup.SetFontBold(true);
    m_lblPaperTypeGroup.SetFontSize(13);
    m_lblPaperTypeGroup.SetTextColor(RGB(0x00,0x00,0xFF));
	m_lblPaperTypeGroup.SetFontName("Microsoft Sans Serif");
	m_lblPaperTypeGroup.SetBackgroundColor(CLR_NONE);


     // Envelope loading Preview Group
    CString szCaption, szMsg;
    if (m_PrintDocType != DOC_ENVELOPE)
    {
        szCaption.LoadString(IDS_PRINTORIENT_PAPERGROUP);
        szMsg.LoadString(IDS_PRINTORIENT_PAPER);
    }
    else
    {
        szCaption.LoadString(IDS_PRINTORIENT_ENVELOPEGROUP);
        szMsg.LoadString(IDS_PRINTORIENT_ENVELOPE);
    }
    HWND hwndGroup = ::GetDlgItem(hWnd, IDC_ENVELOPEPREVIEW_GROUP);
    HWND hwndMsg = ::GetDlgItem(hWnd, IDC_ENVELOPEPREVIEW_STATIC);
    CWindow(hwndGroup).SetWindowText(szCaption);
    CWindow(hwndMsg).SetWindowText(szMsg);

    m_lblEnvelopePreviewGroup.InitDialog(hwndGroup);
    m_lblEnvelopePreviewGroup.SetFontBold(true);
    m_lblEnvelopePreviewGroup.SetFontSize(13);
    m_lblEnvelopePreviewGroup.SetTextColor(RGB(0x00,0x00,0xFF));
	m_lblEnvelopePreviewGroup.SetFontName("Microsoft Sans Serif");
	m_lblEnvelopePreviewGroup.SetBackgroundColor(CLR_NONE);

    // Printer Settings Group
    m_lblPrinterSettingsGroup.InitDialog(::GetDlgItem(hWnd, IDC_PRINTSETTINGS_GROUP));
    m_lblPrinterSettingsGroup.SetFontBold(true);
    m_lblPrinterSettingsGroup.SetFontSize(13);
    m_lblPrinterSettingsGroup.SetTextColor(RGB(0x00,0x00,0xFF));
	m_lblPrinterSettingsGroup.SetFontName("Microsoft Sans Serif");
	m_lblPrinterSettingsGroup.SetBackgroundColor(CLR_NONE);

    // Paper Load radio button header
    m_lblPaperLoadHeader.InitDialog(::GetDlgItem(hWnd, IDC_PAPERLOAD_STATIC));
    //m_lblPaperLoadHeader.SetFontBold(true);
    m_lblPaperLoadHeader.SetFontSize(12);
    m_lblPaperLoadHeader.SetTextColor(RGB(0x64,0x96,0xC8));
	m_lblPaperLoadHeader.SetFontName("Microsoft Sans Serif");
	m_lblPaperLoadHeader.SetBackgroundColor(CLR_NONE);

    // Paper Align radio button header
    m_lblPaperAlignHeader.InitDialog(::GetDlgItem(hWnd, IDC_PAPERALIGN_STATIC));
    //m_lblPaperAlignHeader.SetFontBold(true);
    m_lblPaperAlignHeader.SetFontSize(12);
    m_lblPaperAlignHeader.SetTextColor(RGB(0x64,0x96,0xC8));
	m_lblPaperAlignHeader.SetFontName("Microsoft Sans Serif");
	m_lblPaperAlignHeader.SetBackgroundColor(CLR_NONE);

    // Envelope Load radio button header
    m_lblEnvelopeLoadHeader.InitDialog(::GetDlgItem(hWnd, IDC_ENVELOPELOADS_STATIC));
    //m_lblEnvelopeLoadHeader.SetFontBold(true);
    m_lblEnvelopeLoadHeader.SetFontSize(12);
    m_lblEnvelopeLoadHeader.SetTextColor(RGB(0x64,0x96,0xC8));
	m_lblEnvelopeLoadHeader.SetFontName("Microsoft Sans Serif");
	m_lblEnvelopeLoadHeader.SetBackgroundColor(CLR_NONE);

    // Envelope Edge radio button header
    m_lblEnvelopeEdgeHeader.InitDialog(::GetDlgItem(hWnd, IDC_ENVELOPEEDGE_STATIC));
    //m_lblEnvelopeEdgeHeader.SetFontBold(true);
    m_lblEnvelopeEdgeHeader.SetFontSize(12);
    m_lblEnvelopeEdgeHeader.SetTextColor(RGB(0x64,0x96,0xC8));
	m_lblEnvelopeEdgeHeader.SetFontName("Microsoft Sans Serif");
	m_lblEnvelopeEdgeHeader.SetBackgroundColor(CLR_NONE);


    // Initialize orientation. 
    InitializeOrientation(hWnd);   
    CWindow(hWnd).CenterWindow();

 
    return;
}
//////////////////////////////////////////////////////////////////////
bool CPrint::InitializeOrientation(HWND hWnd)
{
    if (!::IsWindow(hWnd))
        return false;
  
    // Paper Feed (Top/Bottom)
    ::CheckRadioButton(hWnd, IDC_PAPERFEED_BOTTOM, IDC_PAPERFEED_TOP, CPaperFeed::GetFeed(m_iPaperFeedCode)); 
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_BOTTOM), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_TOP), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    
    // Paper Feed Alignment
    int nShow = SW_HIDE;
    if (IsCustomPaper() || (m_PrintDocType == DOC_ENVELOPE))
        nShow = SW_SHOW;
        
    ::CheckRadioButton(hWnd, IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode)); 
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_LEFT), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_CENTER), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_RIGHT), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);  
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_LEFT), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_CENTER), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_POS_RIGHT), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERALIGN_STATIC), nShow);

    
    if (DOC_ENVELOPE == m_PrintDocType)
        nShow = SW_SHOW;
    else
        nShow = SW_HIDE;
      
     // Paper Orientation
    ::CheckRadioButton(hWnd, IDC_PAPERFEED_FACE_DOWN, IDC_PAPERFEED_FACE_UP, CPaperFeed::GetFace(m_iPaperFeedCode));
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_FACE_DOWN), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_FACE_UP), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_FACE_UP), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_FACE_DOWN), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_ENVELOPELOADS_STATIC), nShow);
        

    // Envelope Feed Alignment - enabled only for envelope type.
    ::CheckRadioButton(hWnd, IDC_PAPERFEED_EDGE_LEFT, IDC_PAPERFEED_EDGE_TOP, CPaperFeed::GetEdge(m_iPaperFeedCode));
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_LEFT), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_TOP), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_RIGHT), WM_SETFONT, (WPARAM)m_hHdrFont2, 0);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_LEFT), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_TOP), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_PAPERFEED_EDGE_RIGHT), nShow);
    ::ShowWindow(::GetDlgItem(hWnd, IDC_ENVELOPEEDGE_STATIC), nShow);
    

    return true;
}
//////////////////////////////////////////////////////////////////////
bool CPrint::HandleOrientation(HWND hWnd, WORD wID)
{
    if (!::IsWindow(hWnd))
        return false;


    switch (wID)
    {
        // Printer Orientation
        case IDC_PAPERFEED_TOP:
        case IDC_PAPERFEED_BOTTOM:
        {
            CPaperFeed::SetFeed(m_iPaperFeedCode, wID);
            ::CheckRadioButton(hWnd, IDC_PAPERFEED_TOP, IDC_PAPERFEED_BOTTOM, CPaperFeed::GetFeed(m_iPaperFeedCode));
            break;
        }
        // Paper Orientation
        case IDC_PAPERFEED_FACE_UP:
        case IDC_PAPERFEED_FACE_DOWN:
        {
            CPaperFeed::SetFace(m_iPaperFeedCode, wID);
	        ::CheckRadioButton(hWnd, IDC_PAPERFEED_FACE_DOWN, IDC_PAPERFEED_FACE_UP, CPaperFeed::GetFace(m_iPaperFeedCode));
            break;
        }
        //Paper Alignment
        case IDC_PAPERFEED_POS_LEFT:
        case IDC_PAPERFEED_POS_CENTER:
        case IDC_PAPERFEED_POS_RIGHT:
        {
            CPaperFeed::SetPos(m_iPaperFeedCode, wID);
	        ::CheckRadioButton(hWnd, IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode));
            break;
        }
        // Envelope Feed Alignment
        case IDC_PAPERFEED_EDGE_LEFT:
        case IDC_PAPERFEED_EDGE_TOP:
        case IDC_PAPERFEED_EDGE_RIGHT:
        {
            CPaperFeed::SetEdge(m_iPaperFeedCode, wID);
	        ::CheckRadioButton(hWnd, IDC_PAPERFEED_EDGE_LEFT, IDC_PAPERFEED_EDGE_TOP, CPaperFeed::GetEdge(m_iPaperFeedCode));
            break;
        }

        default:
            return false;
    }

    UpdateOrientationImages(hWnd);

    return true;
}
//////////////////////////////////////////////////////////////////////
bool CPrint::UpdateOrientationImages(HWND hWnd)
{
    // Update all images.
    int nEnvelopeImageID = CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode);
    int nCustomPaperImageID = CPaperFeed::GetPositionIcon(m_iPaperFeedCode);
    int nImageID = nCustomPaperImageID;
    if (DOC_ENVELOPE == m_PrintDocType)
         nImageID = nEnvelopeImageID;
  

    CImage Image(_AtlBaseModule.GetResourceInstance(), nImageID, "GIF");
	HBITMAP hBitmap = Image.GetBitmapHandle();
    LRESULT lres = ::SendDlgItemMessage(hWnd, IDC_PRINTORIENT_IMAGE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
    
    return true;
}
//////////////////////////////////////////////////////////////////////
bool CPrint::SaveOrientation(HWND hWnd)
{
    int iDevice = ::SendDlgItemMessage(hWnd, cmb4, CB_GETCURSEL, 0, 0);
    if (iDevice < 0)
    	return false;
    
    char strDevice[100];
    strDevice[0] = '\0';
    ::SendDlgItemMessage(hWnd, cmb4, CB_GETLBTEXT, iDevice, (LPARAM)strDevice);
    
    CRegKey regkey;
    if (regkey.Create(REGKEY_APPROOT, REGKEY_PAPERFEED) == ERROR_SUCCESS)
    {
    	CString strValue;
    	strValue.Format("%d", m_iPaperFeedCode);
    	bool bOK = (regkey.SetStringValue(strDevice, strValue) == ERROR_SUCCESS);
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////
UINT CPrint::PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam)
{
	#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
	static int iLastDeviceChecksum;

	switch (msg)
	{
		case WM_CTLCOLOREDIT:
		{ // This message is received when the UI is updating after a printer combo change
			HandleDeviceChange(hWnd, iLastDeviceChecksum);
			break;
		}
		
		case WM_INITDIALOG:
		{
			iLastDeviceChecksum = 0;
	        HandleDeviceChange(hWnd, iLastDeviceChecksum);
            InitWindow(hWnd);
			break;
		}
        case WM_PAINT:
		{
			UpdateOrientationImages(hWnd);
		}
		case WM_COMMAND:
		{
            WORD wID = LOWORD(wParam);
			switch (wID)
			{
				case IDOK:
				{
					int iIndex = ::SendDlgItemMessage(hWnd, IDC_PAPERTYPE, CB_GETCURSEL, 0, 0);
					m_iPaperType = ::SendDlgItemMessage(hWnd, IDC_PAPERTYPE, CB_GETITEMDATA, iIndex, 0);
					if (m_iPaperType < 0)
					{
						SIZE PageSize = {0,0};
						m_pAGDoc->GetPageSize(PageSize);
						m_iPaperType = CPaperTemplates::FindSizeMatch(m_PrintDocType, PageSize);
						if (m_iPaperType < 0)
							m_iPaperType = 0;
					}

					m_bForceDuplexTest = !!::IsDlgButtonChecked(hWnd, IDC_DBLSIDE);
					m_bForceBorderless = !!::IsDlgButtonChecked(hWnd, IDC_BORDERLESS);
                    SaveOrientation(hWnd);
					break;
				}

				case IDCANCEL:
				{
					// Reload the default device values in case they were changed
					LoadDeviceValuesFromRegistry(m_strDevice);
					break;
				}

				case IDC_QUARTERFOLD:
				{
					m_PrintDocType = DOC_QUARTERCARD;
					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE2), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::EnableWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (m_iDuplex >= 0));
					break;
				}

				case IDC_HALFFOLD:
				{
					m_PrintDocType = DOC_HALFCARD;
					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE2), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::EnableWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (m_iDuplex >= 0));
					break;
				}

				case cmb4: // The printer "Name" combobox
				{
					if (HIWORD(wParam) != CBN_SELCHANGE)
						break;

					int iDevice = ::SendDlgItemMessage(hWnd, cmb4, CB_GETCURSEL, 0, 0);
					if (iDevice < 0)
						break;

					char strDevice[100];
					strDevice[0] = '\0';
					::SendDlgItemMessage(hWnd, cmb4, CB_GETLBTEXT, iDevice, (LPARAM)strDevice);

					LoadDeviceValuesFromRegistry(strDevice);

					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::ShowWindow(::GetDlgItem(hWnd, IDC_DBLSIDE2), (ShowDuplexTest() ? SW_SHOW : SW_HIDE));
					::EnableWindow(::GetDlgItem(hWnd, IDC_DBLSIDE), (m_iDuplex >= 0));
					::CheckDlgButton(hWnd, IDC_DBLSIDE, (m_iDuplex < 0 ? BST_CHECKED : BST_UNCHECKED));

					bool bIsEnvelope = (m_PrintDocType == DOC_ENVELOPE);
					int idIcon = (bIsEnvelope ? CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode) : CPaperFeed::GetPositionIcon(m_iPaperFeedCode));
					::SendMessage(::GetDlgItem(hWnd, IDC_PAPERFEED_ACTIVE), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(idIcon)));
					break;
				}

                case IDC_PAPERFEED_TOP:
                case IDC_PAPERFEED_BOTTOM:
                case IDC_PAPERFEED_FACE_UP:
                case IDC_PAPERFEED_FACE_DOWN:
                case IDC_PAPERFEED_POS_LEFT:
                case IDC_PAPERFEED_POS_CENTER:
                case IDC_PAPERFEED_POS_RIGHT:
                case IDC_PAPERFEED_EDGE_LEFT:
                case IDC_PAPERFEED_EDGE_TOP:
                case IDC_PAPERFEED_EDGE_RIGHT:
			    {
                    HandleOrientation(hWnd, wID);
                    break;
                }
				default:
					break;
			}
		}

		default:
			break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
UINT APIENTRY CPrint::PrintHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static CPrint* m_pPrint = NULL;

	if (msg == WM_INITDIALOG)
	{
		PRINTDLG* pPrintDlg = (PRINTDLG*)lParam;
		if (pPrintDlg)
			m_pPrint = (CPrint*)pPrintDlg->lCustData;
	}
	
	return (m_pPrint ? m_pPrint->PrintHookProc(hWnd, msg, wParam) : S_OK);
}

#ifdef NOTUSED //j
	#include "HpMarconi.h"

	CHpMarconi Marconi(m_strDevice);
	bool bOK;
	DWORD dwValue = -1;
	bOK = Marconi.Command(m_pDevMode, HPDJ_PDM_CMD_GET, HPDJ_PDMID_MINIMIZEDMARGIN, dwValue);
	bOK = Marconi.Command(m_pDevMode, HPDJ_PDM_CMD_GET, HPDJ_PDMID_CAN_DO_AUTODUPLEX, dwValue);

	WCHAR strData[MAX_PATH];
	bOK = Marconi.Command(m_pDevMode, HPDJ_PDM_CMD_ENUM, HPDJ_PDMID_MEDIA_TYPE, dwValue, strData);
	while (bOK)
		bOK = Marconi.Enumerate(m_pDevMode, dwValue, strData);
#endif NOTUSED //j
