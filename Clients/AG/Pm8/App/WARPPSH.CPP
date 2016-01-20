s/*
// $Header: /PM8/App/WARPPSH.CPP 1     3/03/99 6:13p Gbeddow $
//
// $Log: /PM8/App/WARPPSH.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.7   28 Mar 1997 17:11:14   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.6   17 Mar 1997 14:48:22   Jay
// Warp shape palette and support.
// 
//    Rev 1.5   16 Jan 1997 09:01:50   Fred
// Switch to warp field data record
// 
//    Rev 1.4   23 Jul 1996 08:07:52   Fred
// 4.2 changes
// 
//    Rev 1.3   28 Jun 1996 17:32:22   Jay
// OnPage() routine.
// 
//    Rev 1.2   27 Jun 1996 13:26:14   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 14:23:16   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:35:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:36   FRED
// Initial revision.
// 
//    Rev 1.16   20 Dec 1995 09:56:22   JAY
// 32-bit fixes
// 
//    Rev 1.15   04 Sep 1995 14:00:04   FRED
// Fixed bug with inspecting preview window
// 
//    Rev 1.14   04 Sep 1995 12:32:48   FRED
//  
// 
//    Rev 1.13   04 Sep 1995 11:46:20   JAY
// 
//    Rev 1.12   03 Sep 1995 15:54:58   FRED
//  
// 
//    Rev 1.11   02 Sep 1995 15:04:54   FRED
// More changes to make help work
// 
//    Rev 1.10   21 Aug 1995 09:34:46   FRED
// Fixed bug
// 
//    Rev 1.9   19 Aug 1995 17:49:30   FRED
// Maximum line sizes
// 
//    Rev 1.8   09 Aug 1995 13:12:52   JAY
// Misc changes.
// 
//    Rev 1.7   22 Jun 1995 12:54:48   FRED
// Fixed sound context bug.
// 
//    Rev 1.6   09 Jun 1995 07:44:34   FRED
// Sound Manager changes
// 
//    Rev 1.5   05 Jun 1995 11:34:44   JAY
//  
// 
//    Rev 1.4   17 May 1995 14:26:24   FRED
// Better warp text object preview.
// 
// 
//    Rev 1.3   12 May 1995 14:55:20   FRED
// More warp effects preview stuff
// 
//    Rev 1.2   12 May 1995 10:34:42   FRED
// Warp text preview
// 
//    Rev 1.1   02 May 1995 15:39:38   JAY
// More smart fields stuff.
// 
//    Rev 1.0   27 Apr 1995 13:30:08   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwdlg.h"
#include "warppsh.h"
#include "warpobj.h"
#include "typeface.h"
#include "pmgfont.h"
#include "framerec.h"

#include <afxpriv.h>

extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette * near pOurPal;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheetPreviewButton

CWarpTextSheetPreviewButton::CWarpTextSheetPreviewButton()
{
}

CWarpTextSheetPreviewButton::~CWarpTextSheetPreviewButton()
{
}

BEGIN_MESSAGE_MAP(CWarpTextSheetPreviewButton, CButton)
	//{{AFX_MSG_MAP(CWarpTextSheetPreviewButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheetPreviewButton message handlers

void CWarpTextSheetPreviewButton::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWarpTextSheetPreviewButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheet

IMPLEMENT_DYNAMIC(CWarpTextSheet, CPmwPropertySheet)

CWarpTextSheet::CWarpTextSheet(WarpTextObject* pObject, CPmwView* pView, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwPropertySheet(nIDCaption, pParentWnd, iSelectPage),
		m_TextPage(pView),
		m_ShapePage(pView),
		m_EffectsPage(pView)
{
	m_pPreviewObject = NULL;
	m_pObject = pObject;
	AddPages();
}

CWarpTextSheet::~CWarpTextSheet()
{
	if (m_pPreviewObject != NULL)
	{
		m_TextPage.Object(NULL);
		m_ShapePage.Object(NULL);
		m_EffectsPage.Object(NULL);
		
		m_pPreviewObject->destroy();
		delete m_pPreviewObject;
		m_pPreviewObject = NULL;
	}
}


BEGIN_MESSAGE_MAP(CWarpTextSheet, CPmwPropertySheet)
	//{{AFX_MSG_MAP(CWarpTextSheet)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK, OnOK)
#ifndef WIN32
	ON_WM_CREATE()
#endif
END_MESSAGE_MAP()

void CWarpTextSheet::AddPages(void)
{
	AddPage(&m_TextPage);
	AddPage(&m_ShapePage);
	AddPage(&m_EffectsPage);
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheet message handlers

#ifdef WIN32
BOOL CWarpTextSheet::OnInitDialog()
{
	m_TextPage.Object(m_pObject);
	CPmwPropertySheet::OnInitDialog();
#else
int CWarpTextSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_TextPage.Object(m_pObject);
	
	// Add the preview window to the property sheet.

	if (CPmwPropertySheet::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
#endif
	
	// Create the warp text object that will be used for the preview.

	PMGDatabasePtr pDatabase = m_pObject->get_database();
	PMGFontServer* pFontServer = (PMGFontServer*)(pDatabase->get_font_server());
	
	WARPTEXTOBJECT_CREATE_STRUCT cs;
	memset(&cs, 0, sizeof(cs));

	if ((m_pPreviewObject = pDatabase->create_warp_text_object(&cs)) != NULL)
	{
		m_pPreviewObject->OnPage(FALSE);

		CFrameRecord* pFrame = m_pPreviewObject->LockFrameRecord();
		if (pFrame != NULL)
		{
			pFrame->VerticalAlignment(ALIGN_middle);
			pFrame->release();
		}

      m_pPreview = (CButton*)(m_TextPage.GetDlgItem(IDC_PREVIEW));
		m_pPreview->GetWindowRect(m_crPreview);
		ScreenToClient(m_crPreview);
		m_pPreview->SetParent(this);
		m_pPreview->SetWindowPos(&wndTop, m_crPreview.left, m_crPreview.top, 0, 0, SWP_NOSIZE);

		m_PreviewButton.SubclassWindow(m_pPreview->GetSafeHwnd());

		HWND hwndFrame;
		CRect crPreviewFrame(m_crPreview);
		crPreviewFrame.InflateRect(1, 1);
		hwndFrame = ::CreateWindow(
			"static",
			"",
			WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|SS_BLACKFRAME,
			crPreviewFrame.left,
			crPreviewFrame.top,
			crPreviewFrame.right-crPreviewFrame.left,
			crPreviewFrame.bottom-crPreviewFrame.top,
			GetSafeHwnd(),
			NULL,
			AfxGetInstanceHandle(),
			NULL);
			
		::SetWindowPos(hwndFrame, m_pPreview->GetSafeHwnd(), crPreviewFrame.left, crPreviewFrame.top, 0, 0, SWP_NOSIZE);
		
		CWnd* pLabel = m_TextPage.GetDlgItem(IDC_PREVIEW_LABEL);
		CRect crLabel;
		pLabel->GetWindowRect(crLabel);
		ScreenToClient(crLabel);
		pLabel->SetParent(this);
		pLabel->SetWindowPos(&wndTop, crLabel.left, crLabel.top, 0, 0, SWP_NOSIZE);
		
		// convert m_crPreview to control coordinates
		
		m_crPreview.right = m_crPreview.Width();
		m_crPreview.left = 0;
		m_crPreview.bottom = m_crPreview.Height();
		m_crPreview.top = 0;
		
		// build redisplay context
		
		CDC *pDC = m_pPreview->GetDC();
		
		if (pDC != NULL)
		{
			HDC hDC = pDC->GetSafeHdc();
	
			m_RedisplayContext.destination_hdc = hDC;
			m_RedisplayContext.hwnd = m_pPreview->GetSafeHwnd();
		
			m_RedisplayContext.scaled_source_x0 =
		 		m_RedisplayContext.scaled_source_y0 = 0;
		
			m_RedisplayContext.destination_rect = m_crPreview;
		
			m_RedisplayContext.destination_x0 = 0;
			m_RedisplayContext.destination_y0 = 0;
		
			m_RedisplayContext.x_resolution = GetDeviceCaps(hDC, LOGPIXELSX);
			m_RedisplayContext.y_resolution = GetDeviceCaps(hDC, LOGPIXELSY);
		
			m_RedisplayContext.outline_gamma_curve =
				m_RedisplayContext.bitmap_gamma_curve = screen_gamma_curve;
		
			m_RedisplayContext.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&m_RedisplayContext);
			m_RedisplayContext.terminate_interrupts = TRUE;
			
			CRect crClipped(m_crPreview);
			m_RedisplayContext.clip_rect = crClipped;
	
			// Initialize the redisplay context so we can use it.
	
			m_RedisplayContext.set_info(hDC);
			
			m_pPreview->ReleaseDC(pDC);

			// Initialize the preview frame.
				
			m_pPreviewObject->assign(*m_pObject);
			
			// Save real object line widths because we are going to
			// clip them for the preview.
			
			m_EffectsPage.m_nInitialTextLineWidth = m_pObject->Record().m_nTextLineWidth;
			m_EffectsPage.m_nInitialBackgroundLineWidth = m_pObject->Record().m_nBackgroundLineWidth;
			
			// Clip the preview line widths to 4 points.
		
			if (m_pPreviewObject->Record().m_nTextLineWidth > 4)
			{
				m_pPreviewObject->Record().m_nTextLineWidth = 4;
			}
			
			if (m_pPreviewObject->Record().m_nBackgroundLineWidth > 4)
			{
				m_pPreviewObject->Record().m_nBackgroundLineWidth = 4;
				
				long lWidth = (long)(m_pPreviewObject->Record().m_nBackgroundLineWidth*1800L)/72L;
				
				PBOX Margins;
				Margins.x0 =
					Margins.y0 =
					Margins.x1 =
					Margins.y1 = lWidth;

				m_pPreviewObject->Margins(Margins);
			}
		
			CRect crBounds(m_crPreview);
			crBounds.InflateRect(-6,-6);
			
			PBOX Bounds;
			m_RedisplayContext.screen_to_pbox(crBounds, &Bounds);
			m_RedisplayContext.source_pbox = Bounds;
			
			m_pPreviewObject->set_bound(Bounds);
			
			// we need a text style

			CTextStyle Style(pDatabase);
			
			int nFaceIndex = typeface_server.find_face("Arial", TRUE);
			DB_RECORD_NUMBER Face = pFontServer->font_face_to_record(nFaceIndex);

			Style.SetDefault();

			if (Face > 0)
			{
				Style.Font(Face);
			}

			Style.Bold(TRUE);
			Style.UpdateFontMetrics();

			CTxp Txp(pDatabase);
			TRY
			{
				Txp.Init(m_pPreviewObject, 0);
				Txp.SetHorizontalAlignment(ALIGN_center);
				Txp.SelectAll();
				// ALIGN_middle.
				Txp.Style(Style);
				CHARACTER c[3] = { 'A', 'B', 'C'};
				Txp.InsertText(c, sizeof(c)/sizeof(CHARACTER));
			}
			END_TRY
			/* Don't show us as flipped regardless of the original. */
			m_pPreviewObject->remove_flags(OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped);

			m_pPreviewObject->calc();

			m_ShapePage.Object(m_pPreviewObject);
			m_EffectsPage.Object(m_pPreviewObject);
		}
	}

#ifdef WIN32
	return TRUE;
#else
 	return 0;
#endif
}

void CWarpTextSheet::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_PREVIEW:
		{
			DrawPreview(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwPropertySheet::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

void CWarpTextSheet::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_pPreviewObject != NULL)
	{
		// Fill in the redisplay context.
	
		HDC hDC = lpDrawItemStruct->hDC;
	
		m_RedisplayContext.destination_hdc = hDC;
		m_RedisplayContext.hwnd = lpDrawItemStruct->hwndItem;
	
		m_RedisplayContext.destination_rect = lpDrawItemStruct->rcItem;
	
		m_RedisplayContext.x_resolution = GetDeviceCaps(hDC, LOGPIXELSX);
		m_RedisplayContext.y_resolution = GetDeviceCaps(hDC, LOGPIXELSY);
	
		// Initialize the redisplay context so we can use it.
	
		m_RedisplayContext.set_info(hDC);
	
		// Realize our palette.
	
		HPALETTE hOurPalette = (HPALETTE)pOurPal->GetSafeHandle();
		HPALETTE hOldPalette = SelectPalette(m_RedisplayContext.destination_hdc, hOurPalette, FALSE);
		RealizePalette(m_RedisplayContext.destination_hdc);
		
		// Draw the object.
	
		UpdateStatePtr UpdateState;
	
		if ((UpdateState = m_pPreviewObject->update(&m_RedisplayContext, &(m_RedisplayContext.source_pbox), m_crPreview, NULL, REFRESH_ALL)) != NULL)
		{
			// See what happened.
			
			switch (UpdateState->type)
			{
				case UPDATE_TYPE_Interrupted:
				{
					// This should not happen.
					delete UpdateState;
					// Fall through to...
				}
				case UPDATE_TYPE_Terminated:
				{
					// Try again later.
					::InvalidateRect(lpDrawItemStruct->hwndItem, m_crPreview, FALSE);
					break;
				}
				default:
				{
					break;
				}
			}
		}
	
		// Select the old palette back in.
	
		SelectPalette(hDC, hOldPalette, FALSE);
	}
}

void CWarpTextSheet::OnOK()
{
	if (m_pObject != NULL)
	{
		// copy over text
		
		m_TextPage.GetText();
		
		if (m_pPreviewObject != NULL)
		{
			// copy over warp field
			
			CWarpField* pWarpField = m_pPreviewObject->GetWarpField();
			DB_RECORD_NUMBER lWarpFieldNumber = 0;
					
			PMGDatabase* pDatabase = m_pPreviewObject->get_database();
			if (pWarpField != NULL)
			{
				WarpField_CreateStruct CreateStruct;
						
				CreateStruct.m_pszName = pWarpField->Name();
				pWarpField->OriginalBounds()->ToData(CreateStruct.m_BoundsData);
						
				lWarpFieldNumber = pDatabase->new_warp_field_record(&CreateStruct);
			}
			
			m_pObject->set_new_warp_field(lWarpFieldNumber);
			
			if (lWarpFieldNumber > 0)
			{
				// Decrement our creation reference count.
				pDatabase->free_warp_field_record(lWarpFieldNumber);
			}

			// copy over margins
			
			m_pObject->Margins(m_pPreviewObject->Margins());
			
			// copy over effects
			
			m_pObject->Record().m_clTextFillColor = m_pPreviewObject->Record().m_clTextFillColor;
			m_pObject->Record().m_fTextFillColorExists = m_pPreviewObject->Record().m_fTextFillColorExists;
			m_pObject->Record().m_nTextLineWidth = m_EffectsPage.m_nTextLineWidth;
			m_pObject->Record().m_clTextLineColor = m_pPreviewObject->Record().m_clTextLineColor;
			m_pObject->Record().m_fTextLineColorExists = m_pPreviewObject->Record().m_fTextLineColorExists;
			m_pObject->Record().m_clTextShadowColor = m_pPreviewObject->Record().m_clTextShadowColor;
			m_pObject->Record().m_fTextShadowColorExists = m_pPreviewObject->Record().m_fTextShadowColorExists;
			m_pObject->Record().m_clBackgroundFillColor = m_pPreviewObject->Record().m_clBackgroundFillColor;
			m_pObject->Record().m_fBackgroundFillColorExists = m_pPreviewObject->Record().m_fBackgroundFillColorExists;
			m_pObject->Record().m_nBackgroundLineWidth = m_EffectsPage.m_nBackgroundLineWidth;
			m_pObject->Record().m_clBackgroundLineColor = m_pPreviewObject->Record().m_clBackgroundLineColor;
			m_pObject->Record().m_fBackgroundLineColorExists = m_pPreviewObject->Record().m_fBackgroundLineColorExists;
			m_pObject->Record().m_clBackgroundShadowColor = m_pPreviewObject->Record().m_clBackgroundShadowColor;
			m_pObject->Record().m_fBackgroundShadowColorExists = m_pPreviewObject->Record().m_fBackgroundShadowColorExists;
		}
		
		m_pObject->calc();
	}
	
	CPmwPropertySheet::OnOK();
}
