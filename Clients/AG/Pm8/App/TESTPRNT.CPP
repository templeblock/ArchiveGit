/*
// $Workfile: TESTPRNT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TESTPRNT.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     5/25/98 11:46a Cboggio
// Added inches to centimetre conversion for localization
// 
//    Rev 1.0   14 Aug 1997 15:26:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:42   Fred
// Initial revision.
// 
//    Rev 1.7   12 Nov 1996 15:59:30   johno
// Moved strings to resource DLL
// 
//    Rev 1.6   28 Oct 1996 10:36:42   johno
// Moved strings to resource file
// 
//    Rev 1.5   15 Aug 1996 19:09:00   Fred
// Fixed bug where A and B were not printing
// 
//    Rev 1.4   31 Jul 1996 11:56:56   Fred
// 16-bit compatibility
// 
//    Rev 1.3   26 Jun 1996 14:23:18   Jay
//  
// 
//    Rev 1.2   07 May 1996 12:45:06   Jay
// From PMW2
// 
//    Rev 1.9   26 Apr 1996 14:23:50   JAY
// New 'scratch' template for internal use
// 
//    Rev 1.8   17 Apr 1996 12:00:22   JAY
// Fixed test print crash under 32-bit build.
// 
//    Rev 1.7   06 Oct 1995 11:59:54   JAY
// New print code, etc.
// 
//    Rev 1.5   30 Aug 1995 11:40:00   FRED
// Test print
// 
//    Rev 1.4   29 Aug 1995 09:37:02   FRED
//  
// 
//    Rev 1.3   28 Aug 1995 20:32:36   FRED
// Print alignment
// 
//    Rev 1.2   28 Aug 1995 15:28:34   FRED
//  
// 
//    Rev 1.1   27 Aug 1995 12:35:30   FRED
// New fill-in-fields stuff
// 
//    Rev 1.0   26 Aug 1995 09:41:16   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "cfixed.h"
#include "printer.h"
#include "pmwdoc.h"
#include "testprnt.h"
#include "pmwview.h"
#include "paper.h"
#include "patobj.h"
#include "frameobj.h"
#include "pmgfont.h"
#include "typeface.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestDialog dialog


CPrinterAlignmentTestDialog::CPrinterAlignmentTestDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPrinterAlignmentTestDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrinterAlignmentTestDialog)
	m_dLeftToA = 0;
	m_dBottomToA = 0;
	m_dLeftToB = 0;
	m_dTopToA = 0;
	m_dTopToB = 0;
	m_dRightToB = 0;
	//}}AFX_DATA_INIT
	
	m_nCurrentOrientation = ORIENTATION_Uninitialized;
	m_nSelectedOrientation = ORIENTATION_Tall;
	
	GetCurrentMargins();
}

void CPrinterAlignmentTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	if (pDX->m_bSaveAndValidate)
	{
		//{{AFX_DATA_MAP(CPrinterAlignmentTestDialog)
		DDX_Text(pDX, IDC_LEFT_TO_A, m_dLeftToA);
		DDX_Text(pDX, IDC_BOTTOM_TO_A, m_dBottomToA);
		DDX_Text(pDX, IDC_LEFT_TO_B, m_dLeftToB);
		DDX_Text(pDX, IDC_TOP_TO_A, m_dTopToA);
		DDX_Text(pDX, IDC_TOP_TO_B, m_dTopToB);
		DDX_Text(pDX, IDC_RIGHT_TO_B, m_dRightToB);
		//}}AFX_DATA_MAP
		
		CPmwApp* pApp = GET_PMWAPP();
		if (pApp != NULL)
		{
			if (pApp->GetPrinterInfo(GetPageOrientation()) != NULL)
			{
				CFixedPoint PageSize = pApp->GetPrinterInfo(GetPageOrientation())->PaperSize();
				
				if (GetPageOrientation() == LANDSCAPE)
				{
					CFixed t = PageSize.x;
					PageSize.x = PageSize.y;
					PageSize.y = t;
				}

				pDX->PrepareEditCtrl(IDC_LEFT_TO_A);
				
				// Do a bunch of checks to make sure the numbers make sense.
				
				if (
							((m_dTopToA+m_dBottomToA) > MakeDouble(PageSize.y))
						|| ((m_dRightToB+m_dLeftToB) > MakeDouble(PageSize.x))
						|| (m_dLeftToA < m_dRightToB)
						|| (m_dLeftToA > MakeDouble(PageSize.x)-m_dRightToB)
						|| (m_dTopToB < m_dTopToA)
						|| (m_dTopToB > MakeDouble(PageSize.y)-m_dBottomToA)
					)
				{
					AfxMessageBox(IDS_BAD_PRINTER_VALS);
									  
					pDX->Fail();
				}
			}
		}
		
		SetCurrentMargins();
	}
	else
	{
		CString csValue;
		
		TRY
		{
#ifdef LOCALIZE
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dLeftToA), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_LEFT_TO_A)->SetWindowText(csValue);
			
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dLeftToB), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_LEFT_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dRightToB), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_RIGHT_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dTopToB), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_TOP_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dTopToA), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_TOP_TO_A)->SetWindowText(csValue);
			
			Util::FormatDouble(CurrentLocale.ConvertFromInches(m_dBottomToA), -(CurrentLocale.ConvertPrecisionFromInches(8)), csValue);
			GetDlgItem(IDC_BOTTOM_TO_A)->SetWindowText(csValue);
#else
			Util::FormatDouble(m_dLeftToA, -8, csValue);
			GetDlgItem(IDC_LEFT_TO_A)->SetWindowText(csValue);
			
			Util::FormatDouble(m_dLeftToB, -8, csValue);
			GetDlgItem(IDC_LEFT_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(m_dRightToB, -8, csValue);
			GetDlgItem(IDC_RIGHT_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(m_dTopToB, -8, csValue);
			GetDlgItem(IDC_TOP_TO_B)->SetWindowText(csValue);
			
			Util::FormatDouble(m_dTopToA, -8, csValue);
			GetDlgItem(IDC_TOP_TO_A)->SetWindowText(csValue);
			
			Util::FormatDouble(m_dBottomToA, -8, csValue);
			GetDlgItem(IDC_BOTTOM_TO_A)->SetWindowText(csValue);
#endif
		}
		END_TRY
	}
}

BEGIN_MESSAGE_MAP(CPrinterAlignmentTestDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CPrinterAlignmentTestDialog)
	ON_EN_SETFOCUS(IDC_BOTTOM_TO_A, OnSetfocusMeasurement)
	ON_BN_CLICKED(IDC_PRINT_TEST_PAGE, OnPrintTestPage)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_DRAWITEM()
	ON_EN_SETFOCUS(IDC_LEFT_TO_A, OnSetfocusMeasurement)
	ON_EN_SETFOCUS(IDC_LEFT_TO_B, OnSetfocusMeasurement)
	ON_EN_SETFOCUS(IDC_RIGHT_TO_B, OnSetfocusMeasurement)
	ON_EN_SETFOCUS(IDC_TOP_TO_A, OnSetfocusMeasurement)
	ON_EN_SETFOCUS(IDC_TOP_TO_B, OnSetfocusMeasurement)
	ON_BN_CLICKED(IDC_TALL, OnClickOrientation)
	ON_BN_CLICKED(IDC_WIDE, OnClickOrientation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPrinterAlignmentTestDialog::UpdatePreview(void)
{
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		// Check if orientation has changed.
		if (m_nSelectedOrientation != m_nCurrentOrientation)
		{
			pPreview->MoveWindow((m_nSelectedOrientation == ORIENTATION_Tall) ? &m_crTallPageBounds : &m_crWidePageBounds, TRUE);
			m_nCurrentOrientation = m_nSelectedOrientation;
		}
		
		pPreview->Invalidate();
	}
}

void CPrinterAlignmentTestDialog::GetCurrentMargins(void)
{
	CPmwApp* pApp = GET_PMWAPP();
	if (pApp != NULL)
	{
		CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetPageOrientation());
		
		if (pPrinterInfo != NULL)
		{
			CFixedRect Margins = pPrinterInfo->Margins();
			CFixedPoint Offset = pPrinterInfo->OriginOffset();
			CFixedPoint PageSize = pPrinterInfo->PaperSize();
			CFixedRect PhysicalMargins = pPrinterInfo->PhysicalMargins();
			
			if (m_nSelectedOrientation == ORIENTATION_Wide)
			{
				pPrinterInfo->OrientRect(&Margins);
				pPrinterInfo->OrientRect(&PhysicalMargins);
				
				CFixed t;

				t = Offset.x;
				Offset.x = Offset.y;
				Offset.y = t;
				
				t = PageSize.x;
				PageSize.x = PageSize.y;
				PageSize.y = t;
			}

			m_dTopToA = MakeDouble(Margins.Top);
			m_dBottomToA = MakeDouble(Margins.Bottom);
			m_dLeftToB = MakeDouble(Margins.Left);
			m_dRightToB = MakeDouble(Margins.Right);
			m_dLeftToA = (MakeDouble(PageSize.x)/2.0-MakeDouble(Offset.x))+(m_dLeftToB-MakeDouble(PhysicalMargins.Left));
			m_dTopToB = (MakeDouble(PageSize.y)/2.0-MakeDouble(Offset.y))+(m_dTopToA-MakeDouble(PhysicalMargins.Top));
		}
	}
}

void CPrinterAlignmentTestDialog::SetCurrentMargins(void)
{
	CPmwApp* pApp = GET_PMWAPP();
	if (pApp != NULL)
	{
		CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetPageOrientation());
		if (pPrinterInfo != NULL)
		{
			CFixedPoint PageSize = pPrinterInfo->PaperSize();
			CFixedRect PhysicalMargins = pPrinterInfo->PhysicalMargins();
			
			if (m_nSelectedOrientation == ORIENTATION_Wide)
			{
				CFixed t = PageSize.x;
				PageSize.x = PageSize.y;
				PageSize.y = t;
				
				pPrinterInfo->OrientRect(&PhysicalMargins);
			}
			
			CFixedRect Margins;
			Margins.Left = MakeFixed(m_dLeftToB);
			Margins.Top = MakeFixed(m_dTopToA);
			Margins.Right = MakeFixed(m_dRightToB);
			Margins.Bottom = MakeFixed(m_dBottomToA);
			
			CFixedPoint Offset;
			Offset.x = MakeFixed(MakeDouble(PageSize.x)/2.0-m_dLeftToA)+(Margins.Left-PhysicalMargins.Left);
			Offset.y = MakeFixed(MakeDouble(PageSize.y)/2.0-m_dTopToB)+(Margins.Top-PhysicalMargins.Top);
			
			if (m_nSelectedOrientation == ORIENTATION_Wide)
			{
				CFixed t = Offset.x;
				Offset.x = Offset.y;
				Offset.y = t;
				
				pPrinterInfo->OrientRect(&Margins, TRUE);
			}
			
			pPrinterInfo->Margins(Margins);
			pPrinterInfo->Offset(Offset);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestDialog message handlers

BOOL CPrinterAlignmentTestDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_LeftToA.SubclassDlgItem(IDC_LEFT_TO_A, this);
	m_LeftToB.SubclassDlgItem(IDC_LEFT_TO_B, this);
	m_RightToB.SubclassDlgItem(IDC_RIGHT_TO_B, this);
	m_TopToB.SubclassDlgItem(IDC_TOP_TO_B, this);
	m_TopToA.SubclassDlgItem(IDC_TOP_TO_A, this);
	m_BottomToA.SubclassDlgItem(IDC_BOTTOM_TO_A, this);
	
	((CEdit*)GetDlgItem(IDC_LEFT_TO_A))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_LEFT_TO_B))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_RIGHT_TO_B))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_TOP_TO_B))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_TOP_TO_A))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_BOTTOM_TO_A))->LimitText(10);
	
	if (m_nSelectedOrientation == ORIENTATION_Tall)
	{
		CheckDlgButton(IDC_TALL, TRUE);
	}
	else
	{
		CheckDlgButton(IDC_WIDE, TRUE);
	}

	// figure out the positions for the tall and	wide page previews.
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		CRect crPage;
		pPreview->GetWindowRect(&crPage);
		ScreenToClient(&crPage);

		int nXCenter = (crPage.left+crPage.right)/2;
		int nYCenter = (crPage.top+crPage.bottom)/2;
		
		int nXSize, nYSize;
		
		// Compute tall bounds.
		
		nYSize = crPage.Height();
		nXSize = (int)((((long)nYSize)*8)/10);
		
		m_crTallPageBounds.SetRect(
			nXCenter - (nXSize/2),
			crPage.top,
			nXCenter + (nXSize/2),
			crPage.bottom);
			
		// Compute wide bounds.
		
		nXSize = crPage.Width();
		nYSize = (int)((((long)nXSize)*8)/10);
		
		m_crWidePageBounds.SetRect(
			crPage.left,
			nYCenter - (nYSize/2),
			crPage.right,
			nYCenter + (nYSize/2));
	}
	
	UpdatePreview();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrinterAlignmentTestDialog::OnSetfocusMeasurement()
{
	UpdatePreview();
}

void CPrinterAlignmentTestDialog::OnPrintTestPage()
{
	CPmwApp* pApp = GET_PMWAPP();
	if (pApp != NULL)
	{
		// Create a poster project.
		
		pApp->last_new_info.type = PROJECT_TYPE_Poster;
		pApp->last_new_info.subtype = 0;
		pApp->last_new_info.orientation = GetPageOrientation();
		
		// Paper info is printer info.
		*(pApp->last_new_info.m_pPaperInfo) = *(pApp->GetPaperInfo(pApp->last_new_info.orientation));
			
		CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(pApp->last_new_info.orientation);
		CFixedRect Margins = pPrinterInfo->PhysicalMargins();
			
		CPaperInfo* pPaperInfo = pApp->last_new_info.m_pPaperInfo;
		pPaperInfo->SetMarginStyle(CPaperInfo::MARGINS_Minimal, FALSE);
		pPaperInfo->SetPhysicalMargins(Margins);

		CFixedPoint Origin;
		Origin.x = MakeFixed(0);
		Origin.y = MakeFixed(0);
		pPaperInfo->OriginOffset(Origin);

		WORD wFlags = CPmwDoc::m_wGlobalDocumentFlags;
		CPmwDoc::m_wGlobalDocumentFlags |= CPmwDoc::FLAG_TestPrintDocument;
		CPmwDoc* pDoc = (CPmwDoc*)(pApp->m_pScratchTemplate->OpenDocumentFile(NULL, FALSE));
		CPmwDoc::m_wGlobalDocumentFlags = wFlags;

		if (pDoc != NULL)
		{
#ifndef WIN32
			pDoc->DoNewDocument(-1, FALSE);
#endif
			PPNT Dims = pDoc->get_dimensions();

			CFixedPoint PageSize = pPrinterInfo->PaperSize();
			PPNT Center;

			if (pApp->last_new_info.orientation == LANDSCAPE)
			{
				CFixed t = PageSize.x;
				PageSize.x = PageSize.y;
				PageSize.y = t;
				
				pPrinterInfo->OrientRect(&Margins);
			}
			
			Center.x = (PCOORD)MulFixed((PageSize.x/2)-Margins.Left, PAGE_RESOLUTION);
			Center.y = (PCOORD)MulFixed((PageSize.y/2)-Margins.Top, PAGE_RESOLUTION);
			
			// Create A line
			
			PatternObjectPtr object;
		
		/* Fill out the pattern create struct. */
		
			PATTERN_CREATE_STRUCT pcs;
		
			pcs.bound.x0 = Center.x;
			pcs.bound.y0 = -PAGE_RESOLUTION;
			pcs.bound.x1 = pcs.bound.x0+1;
			pcs.bound.y1 = Dims.y+PAGE_RESOLUTION;
			pcs.pattern = 0;	// solid
			pcs.color = COLOR_WHITE;
			pcs.bcolor = COLOR_BLACK;
		
		/* Create a pattern object. */
		
			if ((object = pDoc->create_pattern_object(&pcs)) != NULL)
			{
			/* Add it to the page list. */
			
				pDoc->append_object(object);
				
				// Create B line
				
			/* Fill out the pattern create struct. */
			
				pcs.bound.x0 = -PAGE_RESOLUTION;
				pcs.bound.y0 = Center.y;
				pcs.bound.x1 = Dims.x+PAGE_RESOLUTION;
				pcs.bound.y1 = pcs.bound.y0+1;
				pcs.pattern = 0;	// solid
				pcs.color = COLOR_WHITE;
				pcs.bcolor = COLOR_BLACK;
			
			/* Create a pattern object. */
			
				if ((object = pDoc->create_pattern_object(&pcs)) != NULL)
				{
				/* Add it to the page list. */
				
					pDoc->append_object(object);
					
					PMGDatabase* pDatabase = pDoc->get_database();
					
					CTxp Txp(pDatabase);

				/* Add a text frame for the name. */
	
					FRAMEOBJ_CREATE_STRUCT fcs;
					CFrameObject* pFrame;
	
					fcs.bound.x0 = Center.x+(PAGE_RESOLUTION/8);
					fcs.bound.x1 = Dims.x;
					fcs.bound.y0 = 0;
					fcs.bound.y1 = Center.y;
	
					fcs.alignment = ALIGN_left;
					fcs.vert_alignment = ALIGN_middle;
					fcs.flags = 0;
					fcs.pStyle = NULL;

					CTextStyle Style(pDatabase);
					Style.SetDefault();
			
					// Set the face to "Geneva".
					int nFace = typeface_server.find_face("Geneva", FALSE);
					if (nFace != -1)
					{
						PMGFontServer* pFontServer = (PMGFontServer*)pDoc->get_font_server();
			
						DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
						if (lFace != 0)
						{
							Style.Font(lFace);
							pDatabase->free_font_record(lFace, TRUE);
						}
					}
					Style.Size(MakeFixed(36));
					Style.BaseSize(MakeFixed(36));
					Style.UpdateFontMetrics();

					if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
					{
						pDoc->append_object(pFrame);

						CHARACTER c;

						Txp.Init(pFrame, 0);
						Txp.SetHorizontalAlignment(fcs.alignment);
						Txp.Style(Style);
						c = 'A';
						Txp.InsertText(&c, 1);
		
   					fcs.bound.x0 = 0;
  						fcs.bound.x1 = Center.x;
						fcs.bound.y0 = Center.y;
						fcs.bound.y1 = Dims.y;
		
						fcs.alignment = ALIGN_center;
						fcs.vert_alignment = ALIGN_top;
						fcs.flags = 0;
						fcs.pStyle = NULL;
	
						if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
						{
							pDoc->append_object(pFrame);

							Txp.Init(pFrame, 0);
							Txp.SetHorizontalAlignment(fcs.alignment);
							Txp.Style(Style);
							c = 'B';
							Txp.InsertText(&c, 1);
		
							
							// Print the Poster!
							
							POSITION Position = pDoc->GetFirstViewPosition();
							ASSERT(Position != NULL);
							if (Position != NULL)
							{
								CPmwView* pView = (CPmwView*)(pDoc->GetNextView(Position));
								ASSERT(pView != NULL);
								if (pView != NULL)
								{
									// JRO pDoc->SetTitle("Test Page");
                           pDoc->SetTitle(GET_PMWAPP()->GetResourceStringPointer (IDS_TEST_PAGE));
									pView->PrintNoDialog();
								}
							}
						}
					}
				}
			}
				
			pDoc->OnCloseDocument();
		}
	}
}

void CPrinterAlignmentTestDialog::OnReset()
{
	CPmwApp* pApp = GET_PMWAPP();
	if (pApp != NULL)
	{
		CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetPageOrientation());
		if (pPrinterInfo != NULL)
		{
			CFixedRect Margins = pPrinterInfo->PhysicalMargins();
			CFixedPoint PageSize = pPrinterInfo->PaperSize();
			
			if (GetPageOrientation() == LANDSCAPE)
			{
				CFixed t = PageSize.x;
				PageSize.x = PageSize.y;
				PageSize.y = t;
				
				pPrinterInfo->OrientRect(&Margins);
			}
			
			m_dLeftToA = MakeDouble(PageSize.x)/2.0;
			m_dTopToA = MakeDouble(Margins.Top);
			m_dBottomToA = MakeDouble(Margins.Bottom);
			m_dTopToB = MakeDouble(PageSize.y)/2.0;
			m_dLeftToB = MakeDouble(Margins.Left);
			m_dRightToB = MakeDouble(Margins.Right);
			
			UpdateData(FALSE);
		}
	}
}

void CPrinterAlignmentTestDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_PREVIEW)
	{
		CPen cpBlack;
		CPen cpRed;
		CBrush cbRed;
		CFont cfFont;
		int nContext = 0;
		CDC* pDC = NULL;
		
		TRY
		{
			if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
			{
				CRect crPage = lpDrawItemStruct->rcItem;
				if (!crPage.IsRectEmpty())
				{
					pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
					if (pDC != NULL)
					{
						Util::Draw3dBorder(
							pDC,
							crPage,
							GetSysColor(COLOR_BTNHILIGHT),
							GetSysColor(COLOR_BTNSHADOW),
							GetSysColor(COLOR_BTNTEXT),
							GetSysColor(COLOR_BTNTEXT),
							GetSysColor(COLOR_WINDOW),
							2,
							TRUE,
							TRUE);
							
						nContext = pDC->SaveDC();
						if (nContext > 0)
						{
							if (cpBlack.CreatePen(PS_SOLID, 0, RGB(0,0,0)))
							{
								if (pDC->SelectObject(&cpBlack) != NULL)
								{
									pDC->MoveTo(crPage.left+10, crPage.top+10);
									pDC->LineTo(crPage.right-10, crPage.top+10);
									pDC->LineTo(crPage.right-10, crPage.bottom-10);
									pDC->LineTo(crPage.left+10, crPage.bottom-10);
									pDC->LineTo(crPage.left+10, crPage.top+10);
									
									pDC->MoveTo((crPage.left+crPage.right)/2, crPage.top+25);
									pDC->LineTo((crPage.left+crPage.right)/2, crPage.bottom-25);
									
									pDC->MoveTo(crPage.left+25, (crPage.top+crPage.bottom)/2);
									pDC->LineTo(crPage.right-25, (crPage.top+crPage.bottom)/2);
									
									LOGFONT lfFont;
									
									lfFont.lfHeight = -14;
									lfFont.lfWidth = 0;
									lfFont.lfEscapement = 0;
									lfFont.lfOrientation = 0;
									lfFont.lfWeight = 700;
									lfFont.lfItalic = 0;
									lfFont.lfUnderline = 0;
									lfFont.lfStrikeOut = 0;
									lfFont.lfCharSet = DEFAULT_CHARSET;
									lfFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
									lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
									lfFont.lfQuality = DEFAULT_QUALITY;
									lfFont.lfPitchAndFamily = DEFAULT_PITCH;
									strcpy(lfFont.lfFaceName, "Arial");
									
									if (cfFont.CreateFontIndirect(&lfFont))
									{
										if (pDC->SelectObject(&cfFont) != NULL)
										{
											pDC->SetBkMode(TRANSPARENT);
											pDC->SetTextColor(RGB(0,0,0));
											
											pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
											pDC->ExtTextOut((crPage.left+crPage.right)/2+3, crPage.top+10+(crPage.bottom-crPage.top)/4+5, 0, NULL, "A", 1, NULL);
												
											pDC->SetTextAlign(TA_CENTER|TA_TOP);
											pDC->ExtTextOut(crPage.left+10+(crPage.right-crPage.left)/4, (crPage.top+crPage.bottom)/2, 0, NULL, "B", 1, NULL);
											
											if (cpRed.CreatePen(PS_SOLID, 0, RGB(255,0,0)))
											{
												if (pDC->SelectObject(&cpRed) != NULL)
												{
													int nX0, nY0, nX1, nY1;
													BOOL fDrawIndicator = TRUE;
													
													if (GetFocus() == &m_LeftToA)
													{
														nX0 = crPage.left+10;
														nX1 = (crPage.left+crPage.right)/2;
														nY0 = nY1 = crPage.top+10+(crPage.bottom-crPage.top)/4;
													}
													else if (GetFocus() == &m_LeftToB)
													{
														nX0 = crPage.left+10;
														nX1 = crPage.left+25;
														nY0 = nY1 = (crPage.top+crPage.bottom)/2;
													}
													else if (GetFocus() == &m_RightToB)
													{
														nX0 = crPage.right-25;
														nX1 = crPage.right-10;
														nY0 = nY1 = (crPage.top+crPage.bottom)/2;
													}
													else if (GetFocus() == &m_TopToB)
													{
														nY0 = crPage.top+10;
														nY1 = (crPage.top+crPage.bottom)/2;
														nX0 = nX1 = crPage.left+10+(crPage.right-crPage.left)/4;
													}
													else if (GetFocus() == &m_TopToA)
													{
														nY0 = crPage.top+10;
														nY1 = crPage.top+25;
														nX0 = nX1 = (crPage.left+crPage.right)/2;
													}
													else if (GetFocus() == &m_BottomToA)
													{
														nY0 = crPage.bottom-25;
														nY1 = crPage.bottom-10;
														nX0 = nX1 = (crPage.left+crPage.right)/2;
													}
													else
													{
														fDrawIndicator = FALSE;
													}
													
													if (fDrawIndicator)
													{
														pDC->MoveTo(nX0, nY0);
														pDC->LineTo(nX1, nY1);
														
														if (cbRed.CreateSolidBrush(RGB(255,0,0)))
														{
															if (pDC->SelectObject(&cbRed) != NULL)
															{
																CPoint Points[3];
																
																if (nY0 == nY1)
																{
																	Points[0].x = nX0;
																	Points[0].y = nY0;
																	
																	Points[1].x = nX0+3;
																	Points[1].y = nY0-3;
																	
																	Points[2].x = nX0+3;
																	Points[2].y = nY0+3;
																	
																	pDC->Polygon(&(Points[0]), 3);
																	
																	Points[0].x = nX1;
																	Points[0].y = nY1;
																	
																	Points[1].x = nX1-3;
																	Points[1].y = nY1-3;
																	
																	Points[2].x = nX1-3;
																	Points[2].y = nY1+3;
																	
																	pDC->Polygon(&(Points[0]), 3);
																}
																else
																{
																	Points[0].x = nX0;
																	Points[0].y = nY0;
																	
																	Points[1].x = nX0-3;
																	Points[1].y = nY0+3;
																	
																	Points[2].x = nX0+3;
																	Points[2].y = nY0+3;
																	
																	pDC->Polygon(&(Points[0]), 3);
																	
																	Points[0].x = nX1;
																	Points[0].y = nY1;
																	
																	Points[1].x = nX1-3;
																	Points[1].y = nY1-3;
																	
																	Points[2].x = nX1+3;
																	Points[2].y = nY1-3;
																	
																	pDC->Polygon(&(Points[0]), 3);
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
						}
					}
				}
			}
		}
		END_TRY
		
		if (pDC != NULL)
		{
			if (nContext > 0)
			{
				pDC->RestoreDC(nContext);
				nContext = 0;
			}
				
			pDC = NULL;
		}
	}
	else
	{
		CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestMeasurementEdit

CPrinterAlignmentTestMeasurementEdit::CPrinterAlignmentTestMeasurementEdit()
{
}

CPrinterAlignmentTestMeasurementEdit::~CPrinterAlignmentTestMeasurementEdit()
{
}

BEGIN_MESSAGE_MAP(CPrinterAlignmentTestMeasurementEdit, CEdit)
	//{{AFX_MSG_MAP(CPrinterAlignmentTestMeasurementEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestMeasurementEdit message handlers

void CPrinterAlignmentTestMeasurementEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CPrinterAlignmentTestMeasurementEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DOWN)
	{
		CDialog* pParent = (CDialog*)GetParent();
		if (pParent != NULL)
		{
			if (GetFocus() == pParent->GetDlgItem(IDC_BOTTOM_TO_A))
			{
				pParent->GotoDlgCtrl(pParent->GetDlgItem(IDC_LEFT_TO_A));
			}
			else
			{
				pParent->NextDlgCtrl();
			}
		}
	}
	else if (nChar == VK_UP)
	{
		CDialog* pParent = (CDialog*)GetParent();
		if (pParent != NULL)
		{
			if (GetFocus() == pParent->GetDlgItem(IDC_LEFT_TO_A))
			{
				pParent->GotoDlgCtrl(pParent->GetDlgItem(IDC_BOTTOM_TO_A));
			}
			else
			{
				pParent->PrevDlgCtrl();
			}
		}
	}
	else
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CPrinterAlignmentTestMeasurementEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	
	CString csText;
	GetWindowText(csText);
	
	if (Util::ValidDouble(csText))
	{
		double dNumber = atof(csText);
		Util::FormatDouble(dNumber, -5, csText);
		SetWindowText(csText);
	}
}

void CPrinterAlignmentTestDialog::OnClickOrientation()
{
	m_nSelectedOrientation = IsDlgButtonChecked(IDC_TALL) ? ORIENTATION_Tall : ORIENTATION_Wide;
	UpdatePreview();
	
	GetCurrentMargins();
	UpdateData(FALSE);
}
