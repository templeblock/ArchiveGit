/*
// $Header: /PM8/App/SPACEDLG.CPP 1     3/03/99 6:11p Gbeddow $
//
// Character spacing dialog
//
// $Log: /PM8/App/SPACEDLG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.4   16 Jul 1997 10:15:14   Jay
// Don't memset fcs; it has a constructor
// 
//    Rev 1.3   15 Jan 1997 10:48:52   Jay
// Integration
// 
//    Rev 1.2   08 Jan 1997 16:35:46   Jay
// More spacing changes.
// 
//    Rev 1.1   08 Jan 1997 15:15:40   Jay
// Character spacing
// 
//    Rev 1.0   06 Jan 1997 17:08:38   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "SpaceDlg.h"

#include "ctxp.h"
#include "frameobj.h"

#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette * near pOurPal;

static struct
{
	UINT	m_uID;
	CFixed m_lSpacing;
} StandardSpacing[] =
{
	{ IDC_VERY_LOOSE,	MakeFixed(1.10) },
	{ IDC_LOOSE,		MakeFixed(1.05) },
	{ IDC_NORMAL,		MakeFixed(1.00) },
	{ IDC_TIGHT,		MakeFixed(0.96) },
	{ IDC_VERY_TIGHT,	MakeFixed(0.92) },
};

static const int nStandardSpacings = sizeof(StandardSpacing)/sizeof(StandardSpacing[0]);

/////////////////////////////////////////////////////////////////////////////
// CCharacterSpacingDialog dialog

CCharacterSpacingDialog::CCharacterSpacingDialog(CTextStyle& Style, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CCharacterSpacingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacterSpacingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Style = Style;
	m_Style.BaseSize(MakeFixed(18));
	m_Style.Size(m_Style.BaseSize());
	m_Style.UpdateFontMetrics();

	// Remember the original spacing.
	m_lOriginalSpacing = m_Style.Spacing();
	if (m_lOriginalSpacing == 0)
	{
		m_lOriginalSpacing = MakeFixed(1);
	}

	m_fSelfSet = FALSE;
}

void CCharacterSpacingDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterSpacingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCharacterSpacingDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CCharacterSpacingDialog)
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_SPACING, OnChangeSpacing)
	ON_BN_CLICKED(IDC_VERY_TIGHT, OnVeryTight)
	ON_BN_CLICKED(IDC_VERY_LOOSE, OnVeryLoose)
	ON_BN_CLICKED(IDC_TIGHT, OnTight)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_LOOSE, OnLoose)
	ON_BN_CLICKED(IDC_CUSTOM, OnCustom)
	ON_EN_KILLFOCUS(IDC_SPACING, OnKillfocusSpacing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterSpacingDialog message handlers

void CCharacterSpacingDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch (nIDCtl)
	{
		case IDC_PREVIEW:
		{
			PreviewFont(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

BOOL CCharacterSpacingDialog::OnInitDialog() 
{
	// Do default OnInitDialog.
	CPmwDialog::OnInitDialog();

	// Get the rect for our preview window.
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->GetWindowRect(&m_crFullPreview);
		pPreview->ScreenToClient(&m_crFullPreview);

		m_crPreview = m_crFullPreview;
		m_crPreview.left++;
		m_crPreview.top++;
		m_crPreview.right--;
		m_crPreview.bottom--;
	}

	// Limit our text.
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SPACING);
	if (pEdit != NULL)
	{
		pEdit->LimitText(7);
	}

	// Show the spacing in the edit.
	SetSpacing(m_lOriginalSpacing);

	// Check the radio button that corresponds to this spacing.
	UINT uID = IDC_CUSTOM;
	for (int i = 0; i < nStandardSpacings; i++)
	{
		if (StandardSpacing[i].m_lSpacing == m_lOriginalSpacing)
		{
			uID = StandardSpacing[i].m_uID;
			break;
		}
	}
	CheckDlgButton(uID, TRUE);

	return TRUE;
}

void CCharacterSpacingDialog::SetSpacing(CFixed lSpacing)
{
	// Fix the 0 case.
	if (lSpacing == 0)
	{
		lSpacing = MakeFixed(1);
	}

	// Get the edit so we can set the text.
	CWnd* pEdit = GetDlgItem(IDC_SPACING);

	if (pEdit != NULL)
	{
		// Format the value.
		CString csText;
		TRY
		{
			Util::FormatDouble(MakeDouble(lSpacing)*100.0, -2, csText);
		}
		END_TRY

		m_fSelfSet = TRUE;
		pEdit->SetWindowText(csText);
		m_fSelfSet = FALSE;
	}
}

void CCharacterSpacingDialog::PreviewFont(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hdc = lpDrawItemStruct->hDC;

/* Get the database. */

	PMGDatabasePtr pDatabase = m_Style.Database();

//	od("font_number is %d (%d)\r\n", font_number, face_to_use);

	// Build the redisplay context.
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

	rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&rc);
	rc.terminate_interrupts = TRUE;
	rc.clip_rect = m_crPreview;

/* Initialize the redisplay context so we can use it. */

	rc.set_info(hdc);
	rc.screen_to_pbox(&m_crFullPreview, &rc.source_pbox);

/*
// Create the frame object now.
*/
	FRAMEOBJ_CREATE_STRUCT fcs;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_middle;
	fcs.bound = rc.source_pbox;

	CFrameObject* pObject;

	if ((pObject = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		// This object is not on the page!
		pObject->OnPage(FALSE);

		// Prepare to add some text.
		CTxp Txp(pDatabase);

	/* Stick in some text. */

		Txp.Init(pObject, 0);
		Txp.SetHorizontalAlignment(ALIGN_center);
		Txp.Style(m_Style);
		Txp.InsertString("Sample");

	/*
	// We have the object.
 	*/

		// Realize our palette.
		HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
		HPALETTE hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
		RealizePalette(rc.destination_hdc);

		// Erase the background.

		FramePreview(hdc,
						 (HPEN)GetStockObject(NULL_PEN),
						 (HBRUSH)GetStockObject(WHITE_BRUSH));

		// And draw the object!

		UpdateStatePtr ustate;

		if ((ustate = pObject->update(&rc, &fcs.bound, &m_crPreview, NULL, REFRESH_ALL)) != NULL)
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
					::InvalidateRect(lpDrawItemStruct->hwndItem, &m_crPreview, FALSE);
//					od("Invalidate (C) %d, %d, %d, %d\r\n", m_preview_rect);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		// Draw a nice frame around our image.
		FramePreview(hdc,
						 (HPEN)GetStockObject(BLACK_PEN),
						 (HBRUSH)GetStockObject(NULL_BRUSH));

	/* Select the other palette in. */

		SelectPalette(hdc, hOldPal, FALSE);

		pObject->destroy();			// Remove from the document.
		delete pObject;
	}
}

void CCharacterSpacingDialog::FramePreview(HDC hdc, HPEN hPen, HBRUSH hBrush)
{
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc,
			m_crPreview.left-1,
			m_crPreview.top-1,
			m_crPreview.right+1,
			m_crPreview.bottom+1);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

void CCharacterSpacingDialog::OnChangeSpacing() 
{
	CFixed lNewSpacing = GetSpacingValue();

	if (lNewSpacing != m_Style.Spacing())
	{
		m_Style.Spacing(lNewSpacing);
		m_Style.UpdateFontMetrics();

		RefreshPreview();
	}

   if (!m_fSelfSet)
   {
		CheckRadioButton(IDC_VERY_LOOSE, IDC_CUSTOM, IDC_CUSTOM);
   }
}

CFixed CCharacterSpacingDialog::GetSpacingValue(void)
{
	CString csText;

	TRY
	{
		Util::GetDlgItemText(this, IDC_SPACING, csText);
	}
	END_TRY

	CFixed lSpacing = MakeFixed(atof(csText)/100.0);

	if (lSpacing == 0)
	{
		lSpacing = m_lOriginalSpacing;
	}

	if (lSpacing < MIN_CHARACTER_SPACING)
	{
		lSpacing = MIN_CHARACTER_SPACING;
	}

	if (lSpacing > MAX_CHARACTER_SPACING)
	{
		lSpacing = MAX_CHARACTER_SPACING;
	}
	return lSpacing;
}

void CCharacterSpacingDialog::RefreshPreview(void)
{
/* Make the preview window redraw. */
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		RECT r = m_crPreview;

		pPreview->ClientToScreen(&r);
		ScreenToClient(&r);

		InvalidateRect(&r, FALSE);
	}
}

void CCharacterSpacingDialog::OnVeryLoose() 
{
	OnRadioChange(IDC_VERY_LOOSE);
}

void CCharacterSpacingDialog::OnLoose() 
{
	OnRadioChange(IDC_LOOSE);
}

void CCharacterSpacingDialog::OnNormal() 
{
	OnRadioChange(IDC_NORMAL);
}

void CCharacterSpacingDialog::OnTight() 
{
	OnRadioChange(IDC_TIGHT);
}

void CCharacterSpacingDialog::OnVeryTight() 
{
	OnRadioChange(IDC_VERY_TIGHT);
}

void CCharacterSpacingDialog::OnCustom() 
{
	CWnd* pEdit = GetDlgItem(IDC_SPACING);
	if (pEdit != NULL)
	{
		pEdit->SetFocus();
	}
}

void CCharacterSpacingDialog::OnRadioChange(UINT uID)
{
	for (int i = 0; i < nStandardSpacings; i++)
	{
		if (StandardSpacing[i].m_uID == uID)
		{
			SetSpacing(StandardSpacing[i].m_lSpacing);
			break;
		}
	}
}

void CCharacterSpacingDialog::OnKillfocusSpacing() 
{
	// Seems strange, but it works.
	SetSpacing(GetSpacingValue());
}
