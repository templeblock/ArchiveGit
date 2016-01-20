// prntsize.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwprint.h"
#include "prntsize.h"

#include "utils.h"
#include "paper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintSize dialog

CPrintSize::CPrintSize(CPmwDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPrintSize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintSize)
	m_inches_wide = 0;
	m_inches_tall = 0;
	m_sheets_wide = 0;
	m_inches_wide = 0;
	m_scale = 100.0;
	m_sheets_wide = 0;
	m_sheets_tall = 0;
	m_min_inches_wide = 0.0;
	m_max_inches_wide = 0.0;
	m_min_inches_tall = 0.0;
	m_max_inches_tall = 0.0;
	m_min_sheets_wide = 0;
	m_max_sheets_wide = 0;
	m_min_sheets_tall = 0;
	m_max_sheets_tall = 0;
	m_min_scale = 0.0;
	m_max_scale = 0.0;
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;

	m_active_item_id = 0;
	m_active_item_changed = FALSE;

	m_last_active_item_id = 0;
	m_last_active_item_changed = FALSE;

	m_ignore_change_focus = FALSE;
}

void CPrintSize::limit_control(UINT id, SHORT digits)
{
	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(id)) != NULL)
	{
		edit->LimitText(digits);
	}
}

void CPrintSize::OnOK()
{
	if (ChangeFocus(0))
	{
		CPmwDialog::OnOK();
	}
}

void CPrintSize::DoDataExchange(CDataExchange* pDX)
{
	/*
	// Only used for shoving the initial data values into the
	// dialog. Input values are recorded as the fields are
	// validated when the user moves from field to field.
	*/

	if (!pDX->m_bSaveAndValidate)
	{
		CString csScale;
		CString csInchesTall;
		CString csInchesWide;
		
		TRY
		{
			CPmwDialog::DoDataExchange(pDX);
		
			limit_control(IDC_SHEETS_TALL, PRINT_SIZE_SHEETS_LIMIT);
			limit_control(IDC_SHEETS_WIDE, PRINT_SIZE_SHEETS_LIMIT);
			limit_control(IDC_INCHES_TALL, PRINT_SIZE_INCHES_LIMIT);
			limit_control(IDC_INCHES_WIDE, PRINT_SIZE_INCHES_LIMIT);
			limit_control(IDC_PRINT_SCALE, PRINT_SIZE_PERCENT_LIMIT);

			FormatDouble(m_scale, PRINT_SIZE_PERCENT_PRECISION, csScale);
			FormatDouble(m_inches_tall, PRINT_SIZE_INCHES_PRECISION, csInchesTall);
			FormatDouble(m_inches_wide, PRINT_SIZE_INCHES_PRECISION, csInchesWide);

			//{{AFX_DATA_MAP(CPrintSize)
			DDX_Control(pDX, IDC_SIZE_PREVIEW, m_preview);
			DDX_Text(pDX, IDC_SHEETS_TALL, m_sheets_tall);
			DDX_Text(pDX, IDC_SHEETS_WIDE, m_sheets_wide);
			DDX_Text(pDX, IDC_PRINT_SCALE, csScale);
			DDX_Text(pDX, IDC_INCHES_TALL, csInchesTall);
			DDX_Text(pDX, IDC_INCHES_WIDE, csInchesWide);
			//}}AFX_DATA_MAP
		}
		CATCH (CException, e)
		{
			csScale.Empty();
			csInchesTall.Empty();
			csInchesWide.Empty();
			THROW_LAST();
		}
		END_CATCH
	}
}

BEGIN_MESSAGE_MAP(CPrintSize, CPmwDialog)
	//{{AFX_MSG_MAP(CPrintSize)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SIZE_SHOW, OnClickedSizeShow)
	ON_EN_CHANGE(IDC_INCHES_WIDE, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_INCHES_TALL, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_SHEETS_WIDE, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_SHEETS_TALL, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_PRINT_SCALE, OnChangeAnEdit)
	ON_EN_SETFOCUS(IDC_INCHES_WIDE, OnSetfocusInchesWide)
	ON_EN_SETFOCUS(IDC_INCHES_TALL, OnSetfocusInchesTall)
	ON_EN_SETFOCUS(IDC_SHEETS_WIDE, OnSetfocusSheetsWide)
	ON_EN_SETFOCUS(IDC_SHEETS_TALL, OnSetfocusSheetsTall)
	ON_EN_SETFOCUS(IDC_PRINT_SCALE, OnSetfocusPrintScale)
	ON_EN_SETFOCUS(IDC_SECRET, OnSetfocusSecret1)
	ON_EN_SETFOCUS(IDC_SECRET2, OnSetfocusSecret2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
// Compute tile dimensions.
*/

PUBLIC VOID
compute_tile_dimensions(CPmwDoc *pDoc, double scale,
				PCOORD *width, PCOORD *height,
 				PCOORD *first_width, PCOORD *first_height)
{
	PCOORD xsize, ysize;				/* for comparison purposes */
	PPNT dims = pDoc->get_dimensions();

	scale /= 100.0;

/*
// Continuous paper:
//		first sheet is paper height - top margin
//		middle sheet is paper height
//		last sheet is paper height - bottom margin
//
// Cut sheet paper:
//		all sheets are paper height - (top + bottom margins) - overlap
*/

/*
// Establish x and y sizes.
*/

	xsize = (PCOORD)((double)dims.x*scale);
	ysize = (PCOORD)((double)dims.y*scale);

/* If landscape, x and y are reversed. */

	if (pDoc->get_orientation() == LANDSCAPE)
	{
		PCOORD tmp = xsize;
		xsize = ysize;
		ysize = tmp;
	}

/*
// Get a pointer to the app.
*/

	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

/*
// Calculate the width of a tile.
// Center drawn cut marks on overlap area.
*/

	CFixed lTileOverlap = DivFixed(TILE_OVERLAP, 1000);

	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(pDoc->get_orientation());

	CFixed lFirstTileWidth, lFirstTileHeight;
	CFixed lTileWidth, lTileHeight;

	CFixedPoint Dims = pPaperInfo->SlotDims();
	CFixedRect Margins = pPaperInfo->Margins();

	lFirstTileWidth =
		lTileWidth = Dims.x - (Margins.Left + Margins.Right);

	if (DivFixed(xsize, PAGE_RESOLUTION) > lTileWidth)
	{
		lFirstTileWidth -= lTileOverlap/2;
		lTileWidth -= lTileOverlap;
	}

/*
// Calculate the height of a tile.
*/

	lTileHeight = Dims.y;
	lFirstTileHeight = Dims.y - Margins.Top;

/*
// Cut sheet paper has special considerations.
// Center drawn cut marks on overlap area.
*/

	if (pPaperInfo->Type() == CPaperInfo::PAPER_CutSheet)
	{
		lFirstTileHeight -= Margins.Bottom;
		lTileHeight = lFirstTileHeight;
		if (DivFixed(ysize, PAGE_RESOLUTION) > lTileHeight)
		{
			lFirstTileHeight -= DivFixed(TILE_OVERLAP/2, 1000);
			lTileHeight -= lTileOverlap;
		}
	}

/*
// Account for the scaling factor.
*/

	CFixed lScale = MakeFixed(scale);

	lTileWidth = DivFixed(lTileWidth, lScale);
	lTileHeight = DivFixed(lTileHeight, lScale);
	lFirstTileWidth = DivFixed(lFirstTileWidth, lScale);
	lFirstTileHeight = DivFixed(lFirstTileHeight, lScale);

/*
// Scale tile dimensions to page resolution.
*/

	PCOORD tile_width, tile_height, first_tile_width, first_tile_height;

	tile_width = MulFixed(lTileWidth, PAGE_RESOLUTION);
	tile_height = MulFixed(lTileHeight, PAGE_RESOLUTION);
	first_tile_width = MulFixed(lFirstTileWidth, PAGE_RESOLUTION);
	first_tile_height = MulFixed(lFirstTileHeight, PAGE_RESOLUTION);

/*
// If landscape, reverse tile orientation.
*/

	if (pDoc->get_orientation() == LANDSCAPE)
	{
		PCOORD tmp;

		tmp = tile_height;
		tile_height = tile_width;
		tile_width = tmp;

		tmp = first_tile_height;
		first_tile_height = first_tile_width;
		first_tile_width = tmp;
	}
	*width = tile_width;
	*height = tile_height;
	*first_width = first_tile_width;
	*first_height = first_tile_height;
}

/*
// Preview the size settings.
*/

PUBLIC VOID
draw_print_preview(LPDRAWITEMSTRUCT lpDrawItemStruct,
				CPmwDoc *pDoc,
				SHORT sheets_wide,
				SHORT sheets_tall,
				double scale)
{
	HDC hdc = lpDrawItemStruct->hDC;
	RECT preview_rect = lpDrawItemStruct->rcItem;

/* Draw a nice frame around our image. */

	HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(BLACK_PEN));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));

	Rectangle(hdc,
			preview_rect.left,
			preview_rect.top,
			preview_rect.right,
			preview_rect.bottom);

/* Select a gray brush for cut-mark drawing. */

	SelectObject(hdc, GetStockObject(GRAY_BRUSH));

	RECT r;

	r.left = preview_rect.left + 1;
	r.top = preview_rect.top + 1;
	r.right = preview_rect.right - 1 - r.left;		// Width!
	r.bottom = preview_rect.bottom - 1 - r.top;		// Height!

	PCOORD tile_width, tile_height, first_tile_width, first_tile_height;

	compute_tile_dimensions(pDoc, scale,
						&tile_width, &tile_height,
						&first_tile_width, &first_tile_height);

	PPNT dims = pDoc->get_dimensions();

/* Draw vertical marks. */

	if (sheets_wide != 1)
	{
	/* Center first cut mark on overlap. */
		SHORT first_tile_x = (SHORT)scale_pcoord(first_tile_width,
									 							r.right*10,
									 							dims.x);
	/* Skip by tile width minus overlap. */
		SHORT tile_x = (SHORT)scale_pcoord(tile_width,
									 							r.right*10,
									 							dims.x);

	/* Make sure we're going to advance. */

		if (tile_x == 0)
		{
			tile_x++;
		}

	/* Draw them all. */

		SHORT right = r.right*10;
		for (SHORT x = 1, i = r.left*10 + first_tile_x;
 						x < sheets_wide && i < right;
 						x++, i += tile_x)
		{
			PatBlt(hdc, i/10, r.top, 1, r.bottom, PATCOPY);
		}
	}

/* Draw horizontal marks. */

	if (sheets_tall != 1)
	{
	/* Center first cut mark on overlap. */
		SHORT first_tile_y = (SHORT)scale_pcoord(first_tile_height,
									 							r.bottom*10,
									 							dims.y);
	/* Skip by tile width minus overlap. */
		SHORT tile_y = (SHORT)scale_pcoord(tile_height,
									 							r.bottom*10,
									 							dims.y);

	/* Make sure we're going to advance. */

		if (tile_y == 0)
		{
			tile_y++;
		}

	/* Draw them all. */

		SHORT bottom = r.bottom*10;
		for (SHORT x = 1, i = r.top*10 + first_tile_y;
 								x < sheets_tall && i < bottom;
								x++, i += tile_y)
		{
			PatBlt(hdc, r.left, i/10, r.right, 1, PATCOPY);
		}
	}

	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

/////////////////////////////////////////////////////////////////////////////
// CPrintSize message handlers

void CPrintSize::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_SIZE_PREVIEW:
		{
			draw_print_preview(lpDrawItemStruct, m_pDoc, m_sheets_wide, m_sheets_tall, m_scale);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

/*
// Compute x tiles from the size.
*/

VOID CPrintSize::compute_x_tiles_from_size()
{
	CFixed x;
	CFixed tile_gap = m_Tile.x - m_TileOverlap.x;

	x = m_Size.x-m_TileOverlap.x;
	if (m_TileOverlap.x == 0)
	{
		x += (m_PaperMargins.Left+m_PaperMargins.Right);
	}

	CFixed SheetsWide = DivFixed(x, tile_gap);

	m_sheets_wide_rounded_down = FixedInteger(SheetsWide);
	if (m_sheets_wide_rounded_down < 1)
	{
		m_sheets_wide_rounded_down = 1;
	}

	m_sheets_wide = FixedInteger(SheetsWide+0x0ffff);
	if (m_sheets_wide < 1)
	{
		m_sheets_wide = 1;
	}
}

/*
// Compute y tiles from the size.
*/

VOID CPrintSize::compute_y_tiles_from_size()
{
	CFixed y;
	CFixed tile_gap = m_Tile.y - m_TileOverlap.y;

	y = m_Size.y-m_TileOverlap.y;
	if (m_TileOverlap.y == 0)
	{
		y += (m_PaperMargins.Left+m_PaperMargins.Right);
	}
	
	CFixed SheetsTall = DivFixed(y, tile_gap);

	m_sheets_tall_rounded_down = FixedInteger(SheetsTall);
	if (m_sheets_tall_rounded_down < 1)
	{
		m_sheets_tall_rounded_down = 1;
	}

	m_sheets_tall = FixedInteger(SheetsTall+0x0ffff);
	if (m_sheets_tall < 1)
	{
		m_sheets_tall = 1;
	}
}

/*
// Compute scale from the x dimensions.
*/

VOID CPrintSize::compute_x_scale()
{
	m_scale = 100.0*m_inches_wide
					/((double)m_doc_dims.x/(double)PAGE_RESOLUTION);
}

/*
// Compute scale from the y dimensions.
*/

VOID CPrintSize::compute_y_scale()
{
	m_scale = 100.0*m_inches_tall
					/((double)m_doc_dims.y/(double)PAGE_RESOLUTION);
}

double round_double_up(double d, UINT p)
{
	double m;
	         
	m = 1.0;
	
	while (p-- > 0)
	{
		m *= 10.0;
	}
	
	return ceil(d*m)/m;
}

double round_double_down(double d, UINT p)
{
	double m;
	         
	m = 1.0;
	
	while (p-- > 0)
	{
		m *= 10.0;
	}
	
	return floor(d*m)/m;
}

/*
// Set the sizes from scale.
*/

VOID CPrintSize::update_min_max_values()
{
	/*
	// Save the current scale so we can restore it later.
	*/
	
	double save_scale = m_scale;
	
	/*
	// Compute minimum and maximum scaling factors.
	*/
	
	if (m_pDoc->get_project_type() != PROJECT_TYPE_Banner)
	{
		if (m_doc_dims.x >= m_doc_dims.y)
		{
			m_min_scale = 100.0*PRINT_SIZE_INCHES_MIN/((double)m_doc_dims.y/(double)PAGE_RESOLUTION);
			m_max_scale = 100.0*PRINT_SIZE_INCHES_MAX/((double)m_doc_dims.x/(double)PAGE_RESOLUTION);
		}
	
		else
		{
			m_min_scale = 100.0*PRINT_SIZE_INCHES_MIN/((double)m_doc_dims.x/(double)PAGE_RESOLUTION);
			m_max_scale = 100.0*PRINT_SIZE_INCHES_MAX/((double)m_doc_dims.y/(double)PAGE_RESOLUTION);
		}
	}

	else
	{
		m_min_scale = 100.0*PRINT_SIZE_INCHES_MIN/((double)m_doc_dims.y/(double)PAGE_RESOLUTION);
		m_max_scale = 100.0*PRINT_SIZE_INCHES_MAX/((double)m_doc_dims.y/(double)PAGE_RESOLUTION);
	}
	
	/*
	// Compute minimums.
	*/
		
	m_scale = m_min_scale;
	
	update_size_from_scale();
	
	m_min_inches_wide = round_double_up(m_inches_wide, PRINT_SIZE_INCHES_PRECISION);
	m_min_inches_tall = round_double_up(m_inches_tall, PRINT_SIZE_INCHES_PRECISION);
	
	m_min_sheets_wide = m_sheets_wide_rounded_down;
	m_min_sheets_tall = m_sheets_tall_rounded_down;
	
	m_min_scale = round_double_up(m_min_scale, PRINT_SIZE_PERCENT_PRECISION);
	
	/*
	// Compute maximums.
	*/
	
	m_scale = m_max_scale;
	
	update_size_from_scale();
	
	m_max_inches_wide = round_double_down(m_inches_wide, PRINT_SIZE_INCHES_PRECISION);
	m_max_inches_tall = round_double_down(m_inches_tall, PRINT_SIZE_INCHES_PRECISION);
	
	m_max_sheets_wide = m_sheets_wide_rounded_down;
	m_max_sheets_tall = m_sheets_tall_rounded_down;
	
	m_max_scale = round_double_down(m_max_scale, PRINT_SIZE_PERCENT_PRECISION);
	
	/*
	// Restore previous scale.
	*/
	
	m_scale = save_scale;
	update_size_from_scale();
}

/*
// Set the sizes from scale.
*/

VOID CPrintSize::update_size_from_scale()
{
	double scale = m_scale/100.0;

/* Compute inches (double). */

	m_inches_wide = (double)m_doc_dims.x*scale/(double)PAGE_RESOLUTION;
	m_inches_tall = (double)m_doc_dims.y*scale/(double)PAGE_RESOLUTION;

/* Compute size in inches * 1000 (PCOORD). */

	m_Size.x = MakeFixed(m_inches_wide);
	m_Size.y = MakeFixed(m_inches_tall);

/* Compute tiles. */

	compute_x_tiles_from_size();
	compute_y_tiles_from_size();
}

VOID CPrintSize::init_tile_sizes()
{
/* Get the document dimensions. */

	m_doc_dims = m_pDoc->get_dimensions();

/* Get the paper info from the app. */

	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(m_pDoc->get_orientation());

	m_PaperMargins = pPaperInfo->Margins();

/* Compute the dimensions of a tile. */

	CFixedPoint SlotDims = pPaperInfo->SlotDims();

	CFixedPoint Overlap;
	CFixedPoint Tile;


	Tile.x = SlotDims.x - (m_PaperMargins.Left+m_PaperMargins.Right);
	Tile.y = SlotDims.y;

	Overlap.x =
		Overlap.y = DivFixed(TILE_OVERLAP, 1000);

	if (pPaperInfo->Type() == CPaperInfo::PAPER_Continuous)
	{
		Overlap.y = 0;
	}
	else
	{
	/* Tiles are all fixed size (reduced by margins). */
		Tile.y -= (m_PaperMargins.Top + m_PaperMargins.Bottom);
	}

	if (m_pDoc->get_orientation() == PORTRAIT)	
	{
		m_Tile = Tile;
		m_TileOverlap = Overlap;
	}
	else
	{
		m_Tile.x = Tile.y;
		m_Tile.y = Tile.x;
		m_TileOverlap.x = Overlap.y;
		m_TileOverlap.y = Overlap.x;
	}
}

PUBLIC VOID
size_print_preview(PPNT doc_dims, CWnd *dialog, CWnd *preview)
{
	RECT r;

/* Get the preview window max size... */

	preview->GetWindowRect(&r);

/* ...and convert to our dialog's coordinate system. */

	dialog->ScreenToClient(&r);
	
/*
// Compute the correctly proportioned view based on the document.
// We may need to take into account the resolution of the screen (as far
// as aspect ratio goes)...
*/

	int xdim = r.right - r.left;
	int ydim = r.bottom - r.top;
	PCOORD new_ydim;

	if ((new_ydim = scale_pcoord(xdim, doc_dims.y, doc_dims.x)) > ydim)
	{
		xdim = (SHORT)scale_pcoord(ydim, doc_dims.x, doc_dims.y);
	}
	else
	{
		ydim = (SHORT)new_ydim;
	}

	r.left = (r.left + r.right - xdim)/2;
	r.top = (r.top + r.bottom - ydim)/2;

/* Set the window position. */

	preview->SetWindowPos(NULL, r.left-1, r.top-1,
							xdim+2, ydim+2,
							SWP_NOZORDER | SWP_NOREDRAW);
}

/*
// Init the preview.
*/

VOID CPrintSize::init_preview()
{
	size_print_preview(m_doc_dims, this, &m_preview);
}

BOOL CPrintSize::OnInitDialog()
{
/* Initialize the tile sizes. */

	init_tile_sizes();
	
/* Compute the min/max validation limits */

	update_min_max_values();

/* Create the other values from the scale. */

	update_size_from_scale();

/* Initialize the dialog to make the controls available. */

	CPmwDialog::OnInitDialog();

/* Init the preview. */

	init_preview();

	return TRUE;
}

VOID CPrintSize::update_all_but(UINT this_id)
{
	CString s;
	
	TRY
	{
		CDataExchange cDX(this, FALSE);
	
		if (this_id != IDC_SHEETS_TALL)
		{
			DDX_Text(&cDX, IDC_SHEETS_TALL, m_sheets_tall);
		}
		if (this_id != IDC_SHEETS_WIDE)
		{
			DDX_Text(&cDX, IDC_SHEETS_WIDE, m_sheets_wide);
		}
		if (this_id != IDC_PRINT_SCALE)
		{
			FormatDouble(m_scale, PRINT_SIZE_PERCENT_PRECISION, s);
			DDX_Text(&cDX, IDC_PRINT_SCALE, s);
		}
		if (this_id != IDC_INCHES_TALL)
		{
			FormatDouble(m_inches_tall, PRINT_SIZE_INCHES_PRECISION, s);
			DDX_Text(&cDX, IDC_INCHES_TALL, s);
		}
		if (this_id != IDC_INCHES_WIDE)
		{
			FormatDouble(m_inches_wide, PRINT_SIZE_INCHES_PRECISION, s);
			DDX_Text(&cDX, IDC_INCHES_WIDE, s);
		}
	
		m_preview.Invalidate(FALSE);
	}
	CATCH (CException, e)
	{
		s.Empty();
		THROW_LAST();
	}
	END_CATCH
}

BOOL CPrintSize::ChangeFocus(UINT wID)
{
	if ((!m_ignore_change_focus) && (m_active_item_id != wID))
	{
		// Remember where we were.
		m_last_active_item_id = m_active_item_id;
		m_last_active_item_changed = m_active_item_changed;

		// Switch to the new place (0 means non-edit).
		m_active_item_id = wID;

		if (CheckLastActiveItem() == PRINT_SIZE_CHECK_ERROR)
		{
			m_active_item_id = m_last_active_item_id;
			m_active_item_changed = m_last_active_item_changed;

			if (m_active_item_id != 0)
			{
				GotoDlgCtrl(GetDlgItem(m_active_item_id));
			}

			return FALSE;
		}

		m_active_item_changed = FALSE;

		/*
		// Special case handling when checking from some
		// non-edit button. In this case, the focus may
		// not have actually moved to the button. For
		// example, pressing ALT-O to push "Show". Of
		// course the user may have cliked on Show in
		// which case the focus did move. The following
		// hacks try to distinguish between these kinds
		// of cases.
		*/

		if ((m_active_item_id == 0)
		 && (m_last_active_item_id != 0)
		 && (GetFocus() == GetDlgItem(m_last_active_item_id)))
		{
			// The focus did not move, leave it active (but unchanged).
			m_active_item_id = m_last_active_item_id;
		}
	}

	return TRUE;
}

void CPrintSize::OnSetfocusInchesWide()
{
	ChangeFocus(IDC_INCHES_WIDE);
}

void CPrintSize::OnSetfocusInchesTall()
{
	ChangeFocus(IDC_INCHES_TALL);
}

void CPrintSize::OnSetfocusSheetsWide()
{
	ChangeFocus(IDC_SHEETS_WIDE);
}

void CPrintSize::OnSetfocusSheetsTall()
{
	ChangeFocus(IDC_SHEETS_TALL);
}

void CPrintSize::OnSetfocusPrintScale()
{
	ChangeFocus(IDC_PRINT_SCALE);
}

void CPrintSize::OnSetfocusSecret1()
{
	BOOL bForward = (m_active_item_id != 0);

	if (ChangeFocus(0))
	{
		if (bForward)
		{
			NextDlgCtrl();
		}
		else
		{
			PrevDlgCtrl();
		}
	}
}

void CPrintSize::OnSetfocusSecret2()
{
	BOOL bForward = (m_active_item_id == 0);

	if (ChangeFocus(0))
	{
		if (bForward)
		{
			NextDlgCtrl();
		}
		else
		{
			PrevDlgCtrl();
		}
	}
}

/*
// Validate the last active edit item.
*/

PRINTSIZECHECKRESULT CPrintSize::CheckLastActiveItem(void)
{
	PRINTSIZECHECKRESULT Result;
	
	Result = PRINT_SIZE_CHECK_UNCHANGED;
	
	switch (m_last_active_item_id)
	{
		case IDC_INCHES_WIDE:
		{
			Result = CheckDouble(IDC_INCHES_WIDE, m_inches_wide, m_min_inches_wide, m_max_inches_wide, PRINT_SIZE_INCHES_PRECISION);
			if (Result == PRINT_SIZE_CHECK_CHANGED)
			{
				m_Size.x = MakeFixed(m_inches_wide);
				compute_x_scale();
			}
			
			break;
		}
		
		case IDC_INCHES_TALL:
		{
			Result = CheckDouble(IDC_INCHES_TALL, m_inches_tall, m_min_inches_tall, m_max_inches_tall, PRINT_SIZE_INCHES_PRECISION);
			if (Result == PRINT_SIZE_CHECK_CHANGED)
			{
				m_Size.y = MakeFixed(m_inches_tall);
				compute_y_scale();
			}
			
			break;
		}
		
		case IDC_SHEETS_WIDE:
		{
			Result = CheckInt(IDC_SHEETS_WIDE, m_sheets_wide, m_min_sheets_wide, m_max_sheets_wide);
			if (Result == PRINT_SIZE_CHECK_CHANGED)
			{
				m_Size.x = m_Tile.x*m_sheets_wide;
				if (m_TileOverlap.x == 0)
				{
					m_Size.x -= (m_PaperMargins.Left+m_PaperMargins.Right);
				}
				else
				{
					m_Size.x -= (m_sheets_wide-1)*m_TileOverlap.x;
				}
				m_inches_wide = MakeDouble(m_Size.x);
				compute_x_scale();
			}
			
			break;
		}
		
		case IDC_SHEETS_TALL:
		{
			Result = CheckInt(IDC_SHEETS_TALL, m_sheets_tall, m_min_sheets_tall, m_max_sheets_tall);
			if (Result == PRINT_SIZE_CHECK_CHANGED)
			{
				m_Size.y = m_Tile.y*m_sheets_tall;
				if (m_TileOverlap.y == 0)
				{
					m_Size.y -= (m_PaperMargins.Top+m_PaperMargins.Bottom);
				}
				else
				{
					m_Size.y -= (m_sheets_tall-1)*m_TileOverlap.y;
				}
				m_inches_tall = MakeDouble(m_Size.y);
				compute_y_scale();
			}
			
			break;
		}
		
		case IDC_PRINT_SCALE:
		{
			Result = CheckDouble(IDC_PRINT_SCALE, m_scale, m_min_scale, m_max_scale, PRINT_SIZE_PERCENT_PRECISION);
			break;
		}
	}

	if (Result == PRINT_SIZE_CHECK_CHANGED)
	{	
		update_size_from_scale();
		update_all_but(m_last_active_item_id);
	}
	
	return Result;
}
	
PRINTSIZECHECKRESULT CPrintSize::CheckDouble(UINT id, double &value, double min, double max, SHORT precision)
{
	PRINTSIZECHECKRESULT Result;
	
	Result = PRINT_SIZE_CHECK_UNCHANGED;

	TRY
	{
		if (m_last_active_item_changed)
		{	
			/* get the current value */
	
			CDataExchange cDX(this, TRUE);
			DDX_Text(&cDX, id, value);
		
			/* if the user entered something, reformat the number for specified precision */

			CString s;
			cDX.m_bSaveAndValidate = FALSE;
			FormatDouble(value, precision, s);
			DDX_Text(&cDX, id, s);
			Result = PRINT_SIZE_CHECK_CHANGED;
		
			/* validate value */

			if ((value < min) || (value > max))
			{
				CString csMin;
				CString csMax;
				FormatDouble(min, precision, csMin);
				FormatDouble(max, precision, csMax);
			
				char szMessage[256];
				wsprintf(
					szMessage,
					// JRO "Please enter a value between %s and %s.",
               GET_PMWAPP()->GetResourceStringPointer (IDS_ENTER_VAL_S),
					(const char far *)csMin,
					(const char far *)csMax);
				m_ignore_change_focus = TRUE;
				AfxMessageBox(szMessage);
				m_ignore_change_focus = FALSE;
				Result = PRINT_SIZE_CHECK_ERROR;
			}
		}
	}
	CATCH_ALL(e)
	{
		Result = PRINT_SIZE_CHECK_ERROR;
	}
	END_CATCH_ALL

	return Result;
}

PRINTSIZECHECKRESULT CPrintSize::CheckInt(UINT id, int &value, int min, int max)
{
	PRINTSIZECHECKRESULT Result;
	
	Result = PRINT_SIZE_CHECK_UNCHANGED;
	
	TRY
	{
		if (m_last_active_item_changed)
		{
			/* get the current value */
	
			CDataExchange cDX(this, TRUE);
			DDX_Text(&cDX, id, value);
			Result = PRINT_SIZE_CHECK_CHANGED;
		
			/* validate value */

			if ((value < min) || (value > max))
			{
				char szMessage[256];
				wsprintf(
					szMessage,
					// JRO "Please enter a value between %d and %d.",
               GET_PMWAPP()->GetResourceStringPointer (IDS_ENTER_VAL_D),
					min,
					max);
				m_ignore_change_focus = TRUE;
				AfxMessageBox(szMessage);
				m_ignore_change_focus = FALSE;
				Result = PRINT_SIZE_CHECK_ERROR;
			}
		}
	}
	CATCH_ALL(e)
	{
		Result = PRINT_SIZE_CHECK_ERROR;
	}
	END_CATCH_ALL

	return Result;
}

void CPrintSize::OnClickedSizeShow()
{
/*
// This has more to do with getting the focus off an edit field, but let's
// do an invalidate to make it flash anyway!
*/

	if (ChangeFocus(0))
	{
		m_preview.Invalidate();
	}
}

void CPrintSize::OnChangeAnEdit()
{
	m_active_item_changed = TRUE;
}
