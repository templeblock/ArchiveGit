// objpos.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "objpos.h"
#include "utils.h"

#include "commands.h"

#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectPositionDlg dialog


CObjectPositionDlg::CObjectPositionDlg(CPmwDoc *pDoc, RectPageObjectPtr object, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CObjectPositionDlg::IDD, pParent)
{
	m_pDoc = pDoc;
	m_object = object;
	//{{AFX_DATA_INIT(CObjectPositionDlg)
	m_height = "";
	m_left = "";
	m_top = "";
	m_width = "";
	//}}AFX_DATA_INIT
}

void CObjectPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	if (!pDX->m_bSaveAndValidate)
	{
	/* Convert each to the appropriate variable. */

		double d;
		char buffer[20];

		d = ((double)(m_bound.x0 - m_world.x0))/PAGE_RESOLUTION;
		float_to_string(buffer, d);
		m_left = buffer;

		d = ((double)(m_bound.y0 - m_world.y0))/PAGE_RESOLUTION;
		float_to_string(buffer, d);
		m_top = buffer;

		d = ((double)(m_bound.x1 - m_bound.x0))/PAGE_RESOLUTION;
		float_to_string(buffer, d);
		m_width = buffer;

		d = ((double)(m_bound.y1 - m_bound.y0))/PAGE_RESOLUTION;
		float_to_string(buffer, d);
		m_height = buffer;
	}

	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectPositionDlg)
	DDX_Control(pDX, IDC_SIZE_PREVIEW, m_preview);
	DDX_Text(pDX, IDC_POSITION_HEIGHT, m_height);
	DDX_Text(pDX, IDC_POSITION_LEFT, m_left);
	DDX_Text(pDX, IDC_POSITION_TOP, m_top);
	DDX_Text(pDX, IDC_POSITION_WIDTH, m_width);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
	/*
	// Compute m_bound from the data.
	*/
		double left = atof(m_left);
		double top = atof(m_top);

		m_bound.x0 = (PCOORD)(left*PAGE_RESOLUTION) + m_world.x0;
		m_bound.y0 = (PCOORD)(top*PAGE_RESOLUTION) + m_world.y0;
		m_bound.x1 = (PCOORD)((left + atof(m_width))*PAGE_RESOLUTION) + m_world.x0;
		m_bound.y1 = (PCOORD)((top + atof(m_height))*PAGE_RESOLUTION) + m_world.y0;
	}
}

void CObjectPositionDlg::OnOK()
{
	/*
	// If m_bound is totally off the page, then confirm that the
	// user really wants to do this.
	*/

	if (!IsBoxOnBox(&m_bound, &m_world))
	{
		if (AfxMessageBox(
				"The position values you have specified will place the object entirely off the page. Are you sure you want to use these values?",
				MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2|MB_APPLMODAL) == IDNO)
		{
			GotoDlgCtrl(GetDlgItem(IDC_POSITION_LEFT));
			return;
		}
	}

	CPmwDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CObjectPositionDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CObjectPositionDlg)
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_POSITION_HEIGHT, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_POSITION_LEFT, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_POSITION_TOP, OnChangeAnEdit)
	ON_EN_CHANGE(IDC_POSITION_WIDTH, OnChangeAnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CObjectPositionDlg message handlers

BOOL CObjectPositionDlg::OnInitDialog()
{
/* Get our world for later use. */

	m_pDoc->get_panel_world(&m_world);

/* Initialize the sizes in the string fields. */

	init_sizepos();

/* Set our limits. */

	limit_control(IDC_POSITION_LEFT, 6);
	limit_control(IDC_POSITION_TOP, 6);
	limit_control(IDC_POSITION_WIDTH, 6);
	limit_control(IDC_POSITION_HEIGHT, 6);

/* Do the default initialization. */

	CPmwDialog::OnInitDialog();

/* Init the preview. */

	init_preview();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CObjectPositionDlg::limit_control(UINT id, SHORT digits)
{
	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(id)) != NULL)
	{
		edit->LimitText(digits);
	}
}

/*
// Init the size fields.
*/

VOID CObjectPositionDlg::init_sizepos()
{
/* Get the initial bound. */

	m_bound = m_object->get_bound();
}

/*
// Init the preview.
*/

VOID CObjectPositionDlg::init_preview()
{
	RECT r;

/* Get the document dimensions. */

	PPNT doc_dims = m_pDoc->get_dimensions();

/* Get the preview window max size... */

	m_preview.GetWindowRect(&r);

/* ...and convert to our dialog's coordinate system. */

	ScreenToClient(&r);

/*
// Compute the correctly proportioned view based on the document.
// We may need to take into account the resolution of the screen (as far
// as aspect ratio goes)...
*/

	SHORT xdim = r.right - r.left;
	SHORT ydim = r.bottom - r.top;
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

	m_preview.SetWindowPos(NULL, r.left-1, r.top-1,
							xdim+2, ydim+2,
							SWP_NOZORDER | SWP_NOREDRAW);
}

void CObjectPositionDlg::preview_position(LPDRAWITEMSTRUCT lpDrawItemStruct)
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

/* Select a gray brush for drawing purposes (but are purposes gray?). */

	SelectObject(hdc, GetStockObject(GRAY_BRUSH));

/* Grab the layout so we can show it in the dialog. */

	preview_rect.left++;
	preview_rect.top++;
	preview_rect.right--;
	preview_rect.bottom--;

/*
// Show the object's original position (unmoved).
// This is what they will get of they press OK at this point.
*/

/* Compute the dimensions of the panel we are sitting on. */

	PPNT panel_dims;

	panel_dims.x = m_world.x1 - m_world.x0;
	panel_dims.y = m_world.y1 - m_world.y0;

/* Compute the dimensions of the rect representing the panel. */

	POINT dims;

	dims.x = preview_rect.right - preview_rect.left;
	dims.y = preview_rect.bottom - preview_rect.top;

/* Compute the rect to draw. */

	RECT r;

	r.left = preview_rect.left
					+ (SHORT)scale_pcoord(m_bound.x0-m_world.x0, dims.x, panel_dims.x);
	r.top = preview_rect.top
					+ (SHORT)scale_pcoord(m_bound.y0-m_world.y0, dims.y, panel_dims.y);
	r.right = preview_rect.left
					+ (SHORT)scale_pcoord(m_bound.x1-m_world.x0, dims.x, panel_dims.x);
	r.bottom = preview_rect.top
					+ (SHORT)scale_pcoord(m_bound.y1-m_world.y0, dims.y, panel_dims.y);

/* Draw the rect. */

	PatBlt(hdc,
				r.left,
				r.top,
				r.right-r.left,
				r.bottom-r.top,
				PATCOPY);

/* Back with the old. */

	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

void CObjectPositionDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_SIZE_PREVIEW:
		{
			preview_position(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
		}
	}
}

/*
// The CPmwView handler for type-in position.
// This is here to hopefully make PMWVIEW.CPP smaller.
*/

void CPmwView::OnPositionTypeIn()
{
	SHORT ret;
	CPmwDoc *pDoc = GetDocument();
	RectPageObjectPtr object = (RectPageObjectPtr)pDoc->selected_objects();

	if (object == NULL)
	{
	/* Sanity check. */
		return;
	}

	CObjectPositionDlg pdlg(pDoc, object);

	if ((ret = pdlg.DoModal()) == IDOK)
	{
	/* The user has picked a position. */

		DB_RECORD_NUMBER old_layout = object->get_layout();

	/* Get the new bound. */

		PBOX new_bound = pdlg.get_chosen_bound();

	/* Create the undo/redo command for this action. */

		CCmdChangeSelects* pCommand = new CCmdChangeSelects(IDCmd_TypeInPosition);
		if (!pCommand->Before(pDoc))
		{
			delete pCommand;
			pCommand = NULL;
		}

	/* Layout is now manual (for sure). */

		object->set_new_layout(MANUAL_LAYOUT);

	/* Undraw where it was. */

		pDoc->refresh_object(object);

	/* Turn off selects to set new layout. */

		pDoc->toggle_object(object);

	/* Set the new bound. */

		object->set_bound(new_bound);

	/* Recalc the object. */

		pDoc->calc_object(object);

	/* Restore selects. */

		pDoc->toggle_object(object);

	/* Show in new position. */

		pDoc->refresh_object(object);

		pDoc->resize_project(NULL);

	/*
	// Finish up the undo/redo command.
	*/

		if (pCommand->After())
		{
			pDoc->AddCommand(pCommand, FALSE);
		}
		else
		{
			delete pCommand;
		}
	}
}

void CObjectPositionDlg::OnChangeAnEdit()
{
/* Get the data! */

	UpdateData(TRUE);

/* Make it show the new values. */

	m_preview.Invalidate(FALSE);
}
