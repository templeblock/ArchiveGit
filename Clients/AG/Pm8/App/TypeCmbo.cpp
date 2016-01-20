// TypeCmbo.cpp : implementation file
//

#include <stdafx.h>

#include "pmw.h"
#include "TypeCmbo.h"
#include "file.h"
#include "text.h"
#include "typeface.h"
#include "pmwini.h"
#include "pmwdoc.h"
#include "pmgfont.h"
#include "frameobj.h"
#include "ctxp.h"
#include "pmwview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo

CTypefaceCombo::CTypefaceCombo() :
	CBarMRUCombo(0, SECTION_FontsMRU, ENTRY_FontMRU, MAX_FONTS_MRU)
{
	m_nTimer = 0;
	m_bShowPreview = FALSE;
	m_pPreviewWnd = NULL;
}

CTypefaceCombo::~CTypefaceCombo()
{
	DestroyPreview();
}

BOOL CTypefaceCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (INHERITED::Create(dwStyle, rect, pParentWnd, nID))
	{
		m_pPreviewWnd = new CFontPreviewWnd(this);
		HidePreview();
		return TRUE;
	}
	return FALSE;
}

void CTypefaceCombo::BuildList()
{
	BOOL is_registered = GET_PMWAPP()->user_is_registered();

	// Add all the chooseable faces to the list.
	FaceEntry* entry;
	int i;

	// First the best
	int nFaceIndex;
	for (i = 0; i  < m_nMRUItems; i++)
	{
		if ((nFaceIndex = typeface_server.face_list.find_face(m_RecentItemList[i])) != -1)
		{
			entry = typeface_server.face_list.get_face(nFaceIndex);
			if (entry->is_chooseable(is_registered))
			{
				int nIndex = InsertString(-1, entry->get_name());

				if (nIndex != -1)
				{
					SetItemData(nIndex, (DWORD)nFaceIndex);
				}
			}
		}
	}

	// Now the rest
	for (entry = typeface_server.face_list.first_face(), i = 0;
		entry != NULL;
		entry = entry->next_face(), i++)
	{
		if (entry->is_chooseable(is_registered))
		{
			int nIndex = InsertString(-1, entry->get_name());

			if (nIndex != -1)
			{
				SetItemData(nIndex, (DWORD)i);
			}
		}
	}
}

// Preview window
void CTypefaceCombo::ShowPreview()
{
	if (GetDroppedState())
	{
		UpdatePreview();
		MovePreview();
		m_pPreviewWnd->ShowWindow(SW_SHOWNOACTIVATE);
	}
}

void CTypefaceCombo::HidePreview()
{
	m_pPreviewWnd->ShowWindow(SW_HIDE);
}

void CTypefaceCombo::MovePreview()
{
	// Move the window to be next to the box
	CRect rct;
	GetClientRect(rct);
	ClientToScreen(rct);
	rct.OffsetRect(rct.Width(), 0);
	m_pPreviewWnd->SetWindowPos(&wndTop, rct.TopLeft().x, rct.TopLeft().y+30, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
}

void CTypefaceCombo::UpdatePreview()
{
	m_pPreviewWnd->SetFontID(GetItemData(m_DrawItemStruct.itemID));
	m_pPreviewWnd->Invalidate();
}

void CTypefaceCombo::DestroyPreview()
{
	if (m_pPreviewWnd != NULL)
	{
		m_pPreviewWnd->DestroyWindow();
		delete m_pPreviewWnd;
		m_pPreviewWnd = NULL;
	}
}

void CTypefaceCombo::UpdateMRU()
{
	// Make sure preview goes AWAY
	HidePreview();

	CBarMRUCombo::UpdateMRU();
}

BEGIN_MESSAGE_MAP(CTypefaceCombo, CBarMRUCombo)
	//{{AFX_MSG_MAP(CTypefaceCombo)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo message handlers

void CTypefaceCombo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// See if we need to set the preview creation timer
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0 &&
		(lpDrawItemStruct->itemID != m_DrawItemStruct.itemID || !m_pPreviewWnd->IsWindowVisible()))
	{
		m_DrawItemStruct = *lpDrawItemStruct;
		if (!m_pPreviewWnd->IsWindowVisible())
		{
			// Kill the last timer
			if (m_nTimer)
			{
				KillTimer(m_nTimer);
				m_nTimer = 0;
			}

			// Set the timer for creation (for a delay)
			if ((m_nTimer = SetTimer(1, 750, NULL)) != 0)
			{
				// All set...
				m_bShowPreview = TRUE;
			}
			else
			{
				// Failed to create timer-- do preview now
				ShowPreview();
			}
		}
		else
		{
			// Update with new font
			UpdatePreview();
		}
	}
	
	CBarMRUCombo::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CTypefaceCombo::OnTimer(UINT nIDEvent) 
{
	// May need to show a preview window
	if (m_bShowPreview)
	{
		ShowPreview();
		m_bShowPreview = FALSE;
	}
}

void CTypefaceCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CBarMRUCombo::OnKillFocus(pNewWnd);	
	HidePreview();
}

void CTypefaceCombo::OnCloseup() 
{
	HidePreview();
}

/////////////////////////////////////////////////////////////////////////////
// CFontPreviewWnd dialog


CFontPreviewWnd::CFontPreviewWnd(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CFontPreviewWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	CSize szWnd(240, 80);
	if(GET_PMWAPP()->LargeFontMode())
		szWnd.cy = 100;

	CWnd::CreateEx(WS_EX_DLGMODALFRAME,
		 ::AfxRegisterWndClass(CS_SAVEBITS,
		 AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		 NULL,
		 WS_BORDER | WS_POPUP,
		 0, 0, szWnd.cx, szWnd.cy,
		 pParent->GetSafeHwnd(),
		 (HMENU)0);
}


BEGIN_MESSAGE_MAP(CFontPreviewWnd, CWnd)
	//{{AFX_MSG_MAP(CFontPreviewWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontPreviewWnd message handlers


void CFontPreviewWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Draw the background
	CRect rct;
	GetClientRect(rct);
    dc.FillSolidRect(&rct, RGB(255, 255, 255));

	// Get the active view
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)GET_PMWAPP()->m_pMainWnd;
	CMDIChildWnd *pChild = 
             (CMDIChildWnd *) pFrame->GetActiveFrame();
	CPmwView *pView = (CPmwView *)pChild->GetActiveView();

	// Find the current document
    CPmwDoc* pDoc = (CPmwDoc*)(pChild->GetActiveDocument());
	ASSERT(pDoc != NULL);
	
	// Get the database
	PMGDatabasePtr database = pDoc->get_database();

	// Get the current style
	CTextStyle Style(database);
	CTxp* pTxp = pView->GetTxp();
	Style = pTxp->Style();

	// If text color is white, make it black so we can see it
	FillFormatV1& Fill = Style.Fill();
	if (Fill.m_ForegroundColor == COLOR_WHITE)
	{
		Fill.m_ForegroundColor = COLOR_BLACK;
	}

	// Get the entry for this face.
	FaceEntry* entry;
	int face_to_use = m_nFontID;
	int font_number = -1;

	if ((entry = typeface_server.face_list.get_face(face_to_use)) != NULL)
	{
		int nVariation = 0;
		if (Style.Bold())
		{
			nVariation |= FONT_STYLE_Bold;
		}
		if (Style.Italic())
		{
			nVariation |= FONT_STYLE_Italic;
		}
		VARIATION_TYPE vtype = entry->get_variation_type(nVariation);

		if (vtype == VARIATION_TYPE_VIRTUAL || vtype == VARIATION_TYPE_REGISTRATION)
		{
			// We want to use the preview font for "virtual" (PMW.FFL) fonts. Check if
			// the font has an entry in the preview font.
			font_number = entry->get_preview_font_number(nVariation);
			if (font_number != -1)
			{
				// Set the font number back to -1 in case we fail. It will
				// be reloaded if we can sucessfully use the preview font.
				font_number = -1;

				// We want to use the font preview. Try to switch to the preview font.
				PCSTR pn = typeface_server.preview_face_name();
				if (pn != NULL)
				{
					int found_face;
					if ((found_face = typeface_server.find_face(pn, FALSE)) != -1)
					{
						face_to_use = found_face;
						font_number = entry->get_preview_font_number(nVariation);
					}
				}
			}
		}
	}

	// Get a database record for this face.
	// This will be a 'temporary' reference which we must free.
	PMGFontServer *pFontServer = (PMGFontServer*)pDoc->get_font_server();
	DB_RECORD_NUMBER f_record = pFontServer->font_face_to_record(face_to_use);

	// Set the face in the style. The style gets its own reference.
	Style.Font(f_record);

	// Default to 36 point.
	Style.BaseSize(MakeFixed(36));
	Style.Size(Style.BaseSize());
	Style.UpdateFontMetrics();

	// Free our temp reference.
	database->free_font_record(f_record, TRUE);

	// Build the redisplay context.
	RedisplayContext rc;

	rc.destination_hdc = dc.m_hDC;
	rc.hwnd = m_hWnd;

	rc.scaled_source_x0 =
 		rc.scaled_source_y0 = 0;

	rc.destination_rect = rct;

	rc.destination_x0 = 0;
	rc.destination_y0 = 0;

	rc.x_resolution = GetDeviceCaps(dc.m_hDC, LOGPIXELSX);
	rc.y_resolution = GetDeviceCaps(dc.m_hDC, LOGPIXELSY);

	//rc.outline_gamma_curve = rc.bitmap_gamma_curve = screen_gamma_curve;

	//rc.set_check_interrupt(standard_check_interrupt, (void*)&rc);
	rc.terminate_interrupts = TRUE;
	rc.clip_rect = rct;

	// Initialize the redisplay context so we can use it
	rc.set_info(dc.m_hDC);
	rc.screen_to_pbox(&rct, &rc.source_pbox);

	// Create the frame object now.
	FRAMEOBJ_CREATE_STRUCT fcs;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_middle;
	fcs.bound = rc.source_pbox;

	CFrameObject* pObject;

	if ((pObject = database->create_frame_object(&fcs)) != NULL)
	{
		// This object is not on the page!
		pObject->OnPage(FALSE);

		// Nor is it selected!
		pObject->remove_select_flags(SELECT_FLAG_boundary);

		// Prepare to add some text.
		CTxp Txp(database);

		CHARACTER Text[4];

		if (font_number == -1)
		{
			Text[0] = 'T';
			Text[1] = 'y';
			Text[2] = 'p';
			Text[3] = 'e';
		}
		else
		{
			// We substituted the preview font
			int ch_base = font_number*4 + '!';

			Text[0] = ch_base++;
			Text[1] = ch_base++;
			Text[2] = ch_base++;
			Text[3] = ch_base++;
		}

		// Stick in some text
		Txp.Init(pObject, 0);
		Txp.SetHorizontalAlignment(ALIGN_center);
		Txp.Style(Style);
		Txp.InsertText(Text, 4);

		// Select our palette in.
		CPalette* pOldPalette = dc.SelectPalette(pOurPal, FALSE);
		if (pOldPalette != NULL)
		{
			dc.RealizePalette();
		}

		// And draw the object!
		UpdateStatePtr ustate;
		if ((ustate = pObject->update(&rc, &fcs.bound, &rct, NULL, REFRESH_ALL)) != NULL)
		{
			// See what happened
			switch (ustate->type)
			{
				case UPDATE_TYPE_Interrupted:
				{
					// This should not happen
					delete ustate;
					// Fall through to...
				}
				case UPDATE_TYPE_Terminated:
				{
					// Try again later
					InvalidateRect(&rct, FALSE);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		pObject->destroy();			// Remove from the document.
		delete pObject;

		// Select our palette back out.
		if (pOldPalette != NULL)
		{
			dc.SelectPalette(pOldPalette, FALSE);
		}
	}
	
	// Do not call CBarMRUCombo::OnPaint() for painting messages
}
