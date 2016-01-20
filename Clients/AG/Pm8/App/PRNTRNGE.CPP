// prntrnge.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwprint.h"
#include "prntsize.h"
#include "prntrnge.h"

#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define SCALE_UNIT	10

/////////////////////////////////////////////////////////////////////////////
// CPrintRange dialog

CPrintRange::CPrintRange(CPmwDoc *pDoc, double scale, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPrintRange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintRange)
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_scale = scale;

	m_tile_info.compute(m_pDoc, m_scale/100.0);
	m_local_tiles = NULL;

	m_total_tiles = m_tile_info.m_x_tiles*m_tile_info.m_y_tiles;

	m_orientation = m_pDoc->get_orientation();
}

CPrintRange::~CPrintRange()
{
	delete [] m_local_tiles;
}

void CPrintRange::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintRange)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintRange, CPmwDialog)
	//{{AFX_MSG_MAP(CPrintRange)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_TILES_ALL, OnClickedTilesAll)
	ON_BN_CLICKED(IDC_TILES_NONE, OnClickedTilesNone)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CTilesButton::UWM_CHILDBUTTONDOWN, on_child_button_down)
	ON_MESSAGE(CTilesButton::UWM_CHILDMOUSEMOVE, on_child_mouse_move)
END_MESSAGE_MAP()

VOID CPrintRange::init_preview()
{
	m_preview.SubclassDlgItem(IDC_SIZE_PREVIEW, this);

	size_print_preview(m_pDoc->get_dimensions(), this, &m_preview);

/* Generate the tile dimensions for the view. */

	PCOORD tile_width, tile_height, first_tile_width, first_tile_height;
	PPNT dims = m_pDoc->get_dimensions();

	compute_tile_dimensions(m_pDoc, m_scale,
						&tile_width, &tile_height,
						&first_tile_width, &first_tile_height);

 	m_preview.GetWindowRect(&m_preview_r);
	ScreenToClient(&m_preview_r);

	m_preview_r.left++;
	m_preview_r.top++;
	m_preview_r.right--;
	m_preview_r.bottom--;

	int preview_width = m_preview_r.right - m_preview_r.left;
	int preview_height = m_preview_r.bottom - m_preview_r.top;

	m_tile_x = (SHORT)scale_pcoord(tile_width, preview_width*SCALE_UNIT, dims.x);
	m_tile_y = (SHORT)scale_pcoord(tile_height, preview_height*SCALE_UNIT, dims.y);
	m_first_tile_x = (SHORT)scale_pcoord(first_tile_width, preview_width*SCALE_UNIT, dims.x);
	m_first_tile_y = (SHORT)scale_pcoord(first_tile_height, preview_height*SCALE_UNIT, dims.y);
}

VOID CPrintRange::preview_tiles(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	draw_print_preview(lpDrawItemStruct, m_pDoc,
			m_tile_info.m_x_tiles, m_tile_info.m_y_tiles, m_scale);

	draw_tiles(TRUE, lpDrawItemStruct->hDC);
}

VOID CPrintRange::draw_tiles(SHORT value, HDC hdc)
{
/*
// Now show each tile that is selected.
// Do something goofy here!
*/

	RECT r_save = m_preview_r;

	if (hdc != NULL)
	{
	/* DrawItem handler. Map to preview window since the hDC is that way. */
		ClientToScreen(&m_preview_r);
		m_preview.ScreenToClient(&m_preview_r);
	}

	int index;

	for (index = 0; index < m_total_tiles; index++)
	{
		POINT p;

		if (value == -1 || m_local_tiles[index] == (BYTE)value)
		{
			if (m_orientation == LANDSCAPE)
			{
			/* Landscape. */
				p.x = index % m_tile_info.m_x_tiles;
				p.y = index / m_tile_info.m_x_tiles;
			}
			else
			{
			/* Portrait. */
				p.x = index / m_tile_info.m_y_tiles;
				p.y = index % m_tile_info.m_y_tiles;
			}

		/* Update the tile. */

			update_tile(p, index, hdc);
		}
	}

	if (hdc != NULL)
	{
		m_preview_r = r_save;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPrintRange message handlers

void CPrintRange::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_SIZE_PREVIEW:
		{
			preview_tiles(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

BOOL CPrintRange::OnInitDialog()
{
/* Make us be centered (for appearances sake). */

	CPmwDialog::OnInitDialog();

/* Init the preview. */

	init_preview();

/* Allocate the local tile memory. */

	TRY
		m_local_tiles = new BYTE[m_total_tiles];
	END_TRY
	if (m_local_tiles == NULL)
	{
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_tiles == NULL)
	{
	/* Zero all selections. */
		memset(m_local_tiles, 0, m_total_tiles);
		m_tiles_chosen = 0;
	}
	else
	{
	/* Copy the existing selections. */
		memcpy(m_local_tiles, m_tiles, m_total_tiles);
	}

	GET_PMWAPP()->ShowHelpfulHint("Which tile dialog", this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPrintRange::point_to_tile(POINT p, PPOINT tile, int *index)
{
	if (p.x < 0
 				|| p.x >= m_preview_r.right-m_preview_r.left
				|| p.y < 0
 				|| p.y >= m_preview_r.bottom-m_preview_r.top)
	{
		return FALSE;
	}

	p.x *= SCALE_UNIT;
	p.y *= SCALE_UNIT;

	if (p.x < m_first_tile_x)
	{
		tile->x = 0;
	}
	else
	{
		p.x -= m_first_tile_x;
		tile->x = 1 + p.x/m_tile_x;
	}

	if (p.y < m_first_tile_y)
	{
		tile->y = 0;
	}
	else
	{
		p.y -= m_first_tile_y;
		tile->y = 1 + p.y/m_tile_y;
	}

	if (m_orientation == LANDSCAPE)
	{
	/* Tiles run left to right first, then top to bottom. */
		*index = tile->y * m_tile_info.m_x_tiles + tile->x;
	}
	else
	{
	/* Tiles run top to bottom first, then left to right. */
		*index = tile->x * m_tile_info.m_y_tiles + tile->y;
	}
	return TRUE;
}

VOID CPrintRange::update_tile(POINT p, int index, HDC in_hdc)
{
	RECT r;

	if (p.x == 0)
	{
		r.left = 0;
		r.right = m_first_tile_x;
	}
	else
	{
		r.left = m_first_tile_x + SCALE_UNIT;
		if (p.x != 1)
		{
			r.left += (p.x-1)*m_tile_x;
		}
		r.right = r.left + m_tile_x - SCALE_UNIT;
	}

	if (p.y == 0)
	{
		r.top = 0;
		r.bottom = m_first_tile_y;
	}
	else
	{
		r.top = m_first_tile_y + SCALE_UNIT;
		if (p.y != 1)
		{
			r.top += (p.y-1)*m_tile_y;
		}
		r.bottom = r.top + m_tile_y - SCALE_UNIT;
	}

	r.left /= SCALE_UNIT;
	r.top /= SCALE_UNIT;
	r.right /= SCALE_UNIT;
	r.bottom /= SCALE_UNIT;

	r.left += m_preview_r.left;
	r.top += m_preview_r.top;
	r.right += m_preview_r.left;
	r.bottom += m_preview_r.top;

	if (r.bottom > m_preview_r.bottom)
	{
		r.bottom = m_preview_r.bottom;
	}

	if (r.right > m_preview_r.right)
	{
		r.right = m_preview_r.right;
	}

	r.left++;
	r.top++;
	r.right--;
	r.bottom--;

/* Toggle the rectangle. */

	HDC hdc;
	HWND hWnd = GetSafeHwnd();

	if ((hdc = in_hdc) == NULL)
	{
		hdc = ::GetDC(hWnd);
	}

	if (hdc != NULL)
	{
		PatBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top,
						m_local_tiles[index] ? BLACKNESS : WHITENESS);

		if (in_hdc == NULL)
		{
			::ReleaseDC(hWnd, hdc);
		}
	}
}

VOID CPrintRange::set_tile(POINT p, int index)
{
	if (m_local_tiles[index] != m_draw_value)
	{
		m_local_tiles[index] = m_draw_value;
		update_tile(p, index);

		if (m_draw_value)
		{
		/* Added one. */
			m_tiles_chosen++;
		}
		else
		{
		/* Removed one. */
			m_tiles_chosen--;
		}
	}
}

LRESULT CPrintRange::on_child_button_down(WPARAM wparam, LPARAM lparam)
{
	POINT p;
	int index;

	if (point_to_tile(CPoint(lparam), &p, &index))
	{
//		od("Child button down: %d, (%d, %d) -> (%d, %d)\r\n", wparam, lparam, p);

		m_draw_value = !m_local_tiles[index];		// Set the draw value

		set_tile(p, index);
	}
	return 0;
}

LRESULT CPrintRange::on_child_mouse_move(WPARAM wparam, LPARAM lparam)
{
	POINT p;
	int index;

	if (point_to_tile(CPoint(lparam), &p, &index))
	{
		set_tile(p, index);
//		od("Child mouse move: %d, (%d, %d) -> (%d, %d)\r\n", wparam, lparam, p);
	}
	return 0;
}

void CPrintRange::OnOK()
{
	CPmwDialog::OnOK();

	if (m_local_tiles != NULL)
	{
	/* Handle updating the chosen tiles variables. */

		delete [] m_tiles;

		if (m_tiles_chosen == 0 || m_tiles_chosen == m_total_tiles)
		{
		/* Assume this means all! */
			m_tiles = NULL;
		}
		else
		{
			m_tiles = m_local_tiles;
			m_local_tiles = NULL;		// So it won't get freed.
		}
	}
}

void CPrintRange::OnClickedTilesAll()
{
	memset(m_local_tiles, TRUE, m_total_tiles);
	m_tiles_chosen = m_total_tiles;
	draw_tiles(TRUE);
}

void CPrintRange::OnClickedTilesNone()
{
	memset(m_local_tiles, FALSE, m_total_tiles);
	m_tiles_chosen = 0;
	draw_tiles(FALSE);
}
