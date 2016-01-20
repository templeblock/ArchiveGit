// fontdlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmgfont.h"

#include <stdlib.h>

#ifdef _WIN32
#include <..\src\afximpl.h>
#include <afxpriv.h>
#else
#include <auxdata.h>
#endif

#include "fontdlg.h"
#include "frameobj.h"
#include "ctxp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(void* data);
extern CPalette * near pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CPMWFontDialog dialog

CPMWFontDialog::CPMWFontDialog(CPmwDoc *pDoc, CTextStyle& Style, WORD wSoundID, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPMWFontDialog::IDD, pParent)
{
	char buffer[50];

	if (Style.BaseSize() == UNDEFINED_CFIXED)
	{
		m_original_size = -1;
		buffer[0] = '\0';
	}
	else
	{
		m_original_size = FixedInteger(Style.BaseSize());
		itoa(m_original_size, buffer, 10);
	}

	m_pDoc = pDoc;
	m_font_server = (PMGFontServer*)pDoc->get_font_server();
	m_face = m_font_server->font_record_to_face(Style.Font());

	//{{AFX_DATA_INIT(CPMWFontDialog)
	m_outline = Style.Outline();
	m_shadow = Style.Shadow();
	m_style = 0;
	m_pattern = Style.Fill();
	//}}AFX_DATA_INIT
	m_pointsize = buffer;
	m_color = Style.Color();

	m_nOriginalBold = Style.Bold();
	m_nOriginalItalic = Style.Italic();

	if (m_nOriginalBold == -1 || m_nOriginalItalic == -1)
	{
		m_style = -1;
	}
	else
	{
		if (m_nOriginalBold)
		{
			m_style |= FONT_STYLE_Bold;
		}
		if (m_nOriginalItalic)
		{
			m_style |= FONT_STYLE_Italic;
		}
	}

	m_original_face = m_face;
	m_original_outline = m_outline;
	m_original_shadow = m_shadow;
	m_original_style = m_style;
	m_original_pattern = m_pattern;
	m_original_color = m_color;

	m_previewing = FALSE;

	if (wSoundID != 0)
	{
		set_sound_id(wSoundID);
	}
}

void CPMWFontDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPMWFontDialog)
	DDX_Control(pDX, IDC_FONT_SIZE, m_size_combo);
	DDX_Control(pDX, IDC_FACE_LIST, m_face_list);
	DDX_Control(pDX, IDC_FONT_PREVIEW, m_preview);
	DDX_CBIndex(pDX, IDC_FONT_OUTLINE, m_outline);
	DDX_CBIndex(pDX, IDC_FONT_SHADOW, m_shadow);
	DDX_CBIndex(pDX, IDC_FONT_STYLE, m_style);
	DDX_CBIndex(pDX, IDC_FONT_PATTERN, m_pattern);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		if (!m_previewing)
		{
		/* Get the string. */
			DDX_CBString(pDX, IDC_FONT_SIZE, m_pointsize);
			if (!m_pointsize.IsEmpty())
			{
				int size = atoi(m_pointsize);
				DDV_MinMaxInt(pDX, size, 4, 1000);
			}
		}

		int face_index;

		if ((face_index = m_face_list.GetCurSel()) == -1)
		{
			m_face == -1;
		}
		else
		{
			m_face = (int)m_face_list.GetItemData(face_index);
		}
	}
	else
	{
		CComboBox *combo;

		if ((combo = (CComboBox *)GetDlgItem(IDC_FONT_SIZE)) != NULL)
		{
		/* Set the text for the control. */

			int index;

			if ((index = combo->FindStringExact(-1, m_pointsize)) == CB_ERR)
			{
#ifdef _WIN32
				AfxSetWindowText(combo->GetSafeHwnd(), m_pointsize);
#else
				_AfxSmartSetWindowText(combo->GetSafeHwnd(), m_pointsize);
#endif
			}
			else
			{
				combo->SetCurSel(index);
			}

			combo->LimitText(4);
		}
	}
}

BEGIN_MESSAGE_MAP(CPMWFontDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CPMWFontDialog)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_FONT_COLOR, OnColor)
	ON_CBN_SELCHANGE(IDC_FONT_PATTERN, OnFontChanged)
	ON_CBN_SELCHANGE(IDC_FONT_SHADOW, OnFontChanged)
	ON_CBN_SELCHANGE(IDC_FONT_STYLE, OnFontChanged)
	ON_CBN_SELCHANGE(IDC_FONT_OUTLINE, OnFontChanged)
	ON_LBN_SELCHANGE(IDC_FACE_LIST, OnFontChanged)
	ON_CBN_DBLCLK(IDC_FONT_SIZE, OnDblclkFontSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPMWFontDialog message handlers

void CPMWFontDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_FONT_PREVIEW:
		{
			preview_font(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

void CPMWFontDialog::OnColor()
{
	if (m_pDoc->choose_color(m_color) == IDOK)
	{
		OnFontChanged();
	}
}

BOOL CPMWFontDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	BOOL is_registered = GET_PMWAPP()->user_is_registered();

/* Add all the chooseable faces to the list. */

	FaceEntry* entry;
	int i;

	m_face_list.SetRedraw(FALSE);

	for (entry = typeface_server.face_list.first_face(), i = 0;
					entry != NULL;
					entry = entry->next_face(), i++)
	{
		if (entry->is_chooseable(is_registered))
		{
			int nIndex = m_face_list.InsertString(-1, entry->get_name());

			if (nIndex != -1)
			{
				m_face_list.SetItemData(nIndex, (DWORD)i);
				if (i == m_face)
				{
					m_face_list.SetCurSel(nIndex);
				}
			}
		}
	}

	m_face_list.SetRedraw(TRUE);

/* Get the rect for our preview window. */

	m_preview.GetWindowRect(&m_full_preview_rect);

	m_preview_rect = m_full_preview_rect;
	m_preview_rect.left++;
	m_preview_rect.top++;
	m_preview_rect.right--;
	m_preview_rect.bottom--;
	m_preview.ScreenToClient(&m_preview_rect);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*
// Get the information from the dialog into a style.
*/

BOOL CPMWFontDialog::get_exit_info(CTextStyle& Style)
{
	BOOL fChanged = FALSE;
	int nNewSize;

	if (m_pointsize.IsEmpty())
	{
		nNewSize = -1;
	}
	else
	{
		nNewSize = atoi(m_pointsize);
	}

	if (nNewSize != m_original_size)
	{
		Style.BaseSize(MakeFixed(nNewSize));
		Style.Size(MakeFixed(nNewSize));
		fChanged = TRUE;
	}

	if (m_face != m_original_face)
	{
	/*
 	// Get a database record for this face.
	// This will be a 'temporary' reference which we must free.
	*/

		DB_RECORD_NUMBER f_record = m_font_server->font_face_to_record(m_face);

	/* Set the face in the style. The style gets its own reference. */

		Style.Font(f_record);

	/* Free our temp reference. */

		m_pDoc->get_database()->free_font_record(f_record, TRUE);

		fChanged = TRUE;
	}

	if (m_outline != m_original_outline)
	{
		Style.Outline(m_outline);
		fChanged = TRUE;
	}

	if (m_shadow != m_original_shadow)
	{
		Style.Shadow(m_shadow);
		fChanged = TRUE;
	}

	if (m_style != m_original_style)
	{
		ASSERT(m_style != -1);
		Style.Bold((m_style & FONT_STYLE_Bold) != 0);
		Style.Italic((m_style & FONT_STYLE_Italic) != 0);

		fChanged = TRUE;
	}

	if (m_pattern != m_original_pattern)
	{
		Style.Fill(m_pattern);
		fChanged = TRUE;
	}

	if (m_color != m_original_color)
	{
		Style.Color(m_color);
		fChanged = TRUE;
	}

	if (fChanged)
	{
		Style.UpdateFontMetrics();
	}

	return fChanged;
}

void CPMWFontDialog::OnFontChanged()
{
/* Make the preview window redraw. */
	RECT r = m_preview_rect;

	m_preview.ClientToScreen(&r);
	ScreenToClient(&r);

	InvalidateRect(&r, FALSE);
}

VOID CPMWFontDialog::preview_font(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
/* Put the data into local variables. */

	m_previewing = TRUE;
	UpdateData(TRUE);
	m_previewing = FALSE;

	if (m_face == -1
			|| m_outline == -1
 			|| m_shadow == -1
 			|| m_style == -1
 			|| m_pattern == -1
			|| m_color == UNDEFINED_COLOR)
	{
	/* Ambiguous. */
		return;
	}

	HDC hdc = lpDrawItemStruct->hDC;

/* Get the database. */

	PMGDatabasePtr database = m_pDoc->get_database();

/* We need a text style to show for this font. */

	CTextStyle Style(database);

/*
// Get the entry for this face.
*/

	FaceEntry* entry;

	int face_to_use = m_face;
	int font_number = -1;

	if ((entry = typeface_server.face_list.get_face(face_to_use)) != NULL)
	{
		VARIATION_TYPE vtype = entry->get_variation_type(m_style);

		if (vtype == VARIATION_TYPE_VIRTUAL
						|| vtype == VARIATION_TYPE_REGISTRATION)
		{
		/* We want to use the font preview. */
			PCSTR pn = typeface_server.preview_face_name();
			if (pn != NULL)
			{
				int found_face;
				if ((found_face = typeface_server.find_face(pn, FALSE)) != -1)
				{
					face_to_use = found_face;
					font_number = entry->get_virtual_font_number(m_style);
				}
			}
		}
	}
//	od("font_number is %d (%d)\r\n", font_number, face_to_use);

/*
// Get a database record for this face.
// This will be a 'temporary' reference which we must free.
*/

	DB_RECORD_NUMBER f_record = m_font_server->font_face_to_record(face_to_use);

/* Set the face in the style. The style gets its own reference. */

	m_font_server->SetDefault(&Style);			// For whatever we don't change.

	Style.Font(f_record);

	// Default to 36 point.
	Style.BaseSize(MakeFixed(36));
	Style.Size(Style.BaseSize());

/* Free our temp reference. */

	database->free_font_record(f_record, TRUE);

	Style.Outline(m_outline);
	Style.Shadow(m_shadow);
	Style.Bold((m_style & FONT_STYLE_Bold) != 0);
	Style.Italic((m_style & FONT_STYLE_Italic) != 0);
	Style.Fill(m_pattern);
	Style.Color(m_color);
	Style.UpdateFontMetrics();

/*
// Build the redisplay context.
*/

	RedisplayContext rc;

	rc.destination_hdc = hdc;
	rc.hwnd = lpDrawItemStruct->hwndItem;

	rc.scaled_source_x0 =
 		rc.scaled_source_y0 = 0;

	rc.destination_rect = lpDrawItemStruct->rcItem;

	rc.destination_x0 = 0;
	rc.destination_y0 = 0;

	rc.x_resolution = GetDeviceCaps(hdc, LOGPIXELSX);
	rc.y_resolution = GetDeviceCaps(hdc, LOGPIXELSY);

/*
// Set the resolutions to that of the page.
*/

//	rc.x_resolution = rc.y_resolution = PAGE_RESOLUTION;

	rc.outline_gamma_curve =
		rc.bitmap_gamma_curve = screen_gamma_curve;

	rc.set_check_interrupt(standard_check_interrupt, (void*)&rc);
	rc.terminate_interrupts = TRUE;
	rc.clip_rect = m_preview_rect;

/* Initialize the redisplay context so we can use it. */

	rc.set_info(hdc);
	rc.screen_to_pbox(&m_preview_rect, &rc.source_pbox);

/*
// Create the frame object now.
*/
	FRAMEOBJ_CREATE_STRUCT fcs;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_middle;
	fcs.bound = rc.source_pbox;

	CFrameObject* pObject;

	if ((pObject = database->create_frame_object(&fcs)) != NULL)
	{
		// This object is not on the page!
		pObject->OnPage(FALSE);

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
		/* We substituted the preview font. */

			int ch_base = font_number*4 + '!';

			Text[0] = ch_base++;
			Text[1] = ch_base++;
			Text[2] = ch_base++;
			Text[3] = ch_base++;
		}

	/* Stick in some text. */

		Txp.Init(pObject, 0);
		Txp.SetHorizontalAlignment(ALIGN_center);
		Txp.Style(Style);
		Txp.InsertText(Text, 4);

	/*
	// We have the object.
 	*/

	/* Realize our palette. */

		HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
		HPALETTE hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
		RealizePalette(rc.destination_hdc);

	/* Draw a nice frame around our image. */

		HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(BLACK_PEN));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		Rectangle(hdc,
 			m_preview_rect.left-1,
 			m_preview_rect.top-1,
 			m_preview_rect.right+1,
 			m_preview_rect.bottom+1);
		SelectObject(hdc, hOldBrush);
		SelectObject(hdc, hOldPen);

	/* And draw the object! */

		UpdateStatePtr ustate;

		if ((ustate = pObject->update(&rc, &fcs.bound, &m_preview_rect, NULL, REFRESH_ALL)) != NULL)
		{
		/* See what happened. */
			switch (ustate->type)
			{
				case UPDATE_TYPE_Interrupted:
				{
				/* This should not happen. */
					delete ustate;
				/* Fall through to... */
				}
				case UPDATE_TYPE_Terminated:
				{
				/* Try again later. */
					::InvalidateRect(lpDrawItemStruct->hwndItem, &m_preview_rect, FALSE);
//					od("Invalidate (C) %d, %d, %d, %d\r\n", m_preview_rect);
					break;
				}
				default:
				{
					break;
				}
			}
		}

	/* Select the other palette in. */

		SelectPalette(hdc, hOldPal, FALSE);

		pObject->destroy();			// Remove from the document.
		delete pObject;
	}
}

void CPMWFontDialog::OnDblclkFontSize()
{
	OnOK();
}
