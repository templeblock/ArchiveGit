// posdlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "system.h"

#include "posdlg.h"

#include "utils.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog dialog

CPositionDialog::CPositionDialog(CPmwDoc *pDoc, BOOL duping, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPositionDialog::IDD, pParent)
{
	m_pDoc = pDoc;
	m_duping = duping;

	//{{AFX_DATA_INIT(CPositionDialog)
	m_layout_number = -1;
	//}}AFX_DATA_INIT
}

void CPositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionDialog)
	DDX_Control(pDX, IDC_LAYOUT_PREVIEW, m_preview);
	DDX_Control(pDX, IDC_STANDARD_POSITIONS, m_layout_list);
	DDX_LBIndex(pDX, IDC_STANDARD_POSITIONS, m_layout_number);
	//}}AFX_DATA_MAP
}

int CPositionDialog::DoModal()
{
	SHORT ret = IDABORT;

	if ((m_object = m_pDoc->selected_objects()) == NULL)
	{
	/* Should not happen. */
		return ret;
	}

/* Read the list. */

	if (m_layouts.read_layout_list(m_pDoc, m_duping) == ERRORCODE_None)
	{
	/*
	// Run the dialog.
	*/

		ret = CPmwDialog::DoModal();
	}
	return ret;
}

BEGIN_MESSAGE_MAP(CPositionDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CPositionDialog)
	ON_LBN_DBLCLK(IDC_STANDARD_POSITIONS, OnDblclkStandardPositions)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_STANDARD_POSITIONS, OnSelchangePositions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
// Init the preview.
*/

VOID CPositionDialog::init_preview()
{
	RECT r;

/* Get the preview window max size... */

	m_preview.GetWindowRect(&r);

/* ...and convert to our dialog's coordinate system. */

	ScreenToClient(&r);

/*
// Compute the correctly proportioned view based on the document.
// We may need to take into account the resolution of the screen (as far
// as aspect ratio goes)...
*/

	int xdim = r.right - r.left;
	int ydim = r.bottom - r.top;
	PCOORD new_ydim;

	if ((new_ydim = scale_pcoord(xdim, m_doc_dims.y, m_doc_dims.x)) > ydim)
	{
		xdim = (SHORT)scale_pcoord(ydim, m_doc_dims.x, m_doc_dims.y);
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

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog message handlers

BOOL CPositionDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	m_doc_dims = m_pDoc->get_dimensions();

/* Init the preview. */

	init_preview();

/* Stick all the layout names into the listbox. */

	LayoutEntryPtr entry;

	m_layout_list.AddString(m_duping
					// JRO ? "(Single duplicate)"
               ? GET_PMWAPP()->GetResourceStringPointer (IDS_SING_DUP_P)
					// : "(None)");
               : GET_PMWAPP()->GetResourceStringPointer (IDS_NONE_P));

	m_layout_list.SetRedraw(FALSE);
	for (entry = (LayoutEntryPtr)m_layouts.first_element();
				entry != NULL;
				entry = (LayoutEntryPtr)entry->next_element())
	{
		m_layout_list.AddString(entry->get_name());
	}
	m_layout_list.SetRedraw(TRUE);

	PMGDatabasePtr database = m_pDoc->get_database();
	DB_RECORD_NUMBER old_layout = m_object->get_layout();

	od("old layout: %d\r\n", old_layout);

/*
// Try to find the layout in the list.
*/

	if (m_duping || old_layout <= 0)
	{
		m_layout_list.SetCurSel(0);
	}
	else
	{
		ObjectLayoutPtr cur_layout;

	/* Select the string which matches the current name. */

		if ((cur_layout = (ObjectLayoutPtr)m_object->get_database()->get_record(old_layout, NULL, RECORD_TYPE_ObjectLayout)) != NULL)
		{
			m_layout_list.SelectString(0, cur_layout->get_name());
			od("old name: %s\r\n", (LPCSTR)cur_layout->get_name());
			cur_layout->release();
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPositionDialog::OnDblclkStandardPositions()
{
	OnOK();
}

/**********************/
/* LayoutEntry object */
/**********************/

/*
// Constructor.
*/

LayoutEntry::LayoutEntry(LPSTR n)
		: elements(0, TRUE)
{		
	set_name(n);
}

LayoutEntry::~LayoutEntry()
{
}
/*
// Set the name for this layout entry.
*/

void LayoutEntry::set_name(LPCSTR n)
{
	m_csName = n;
}

PRIVATE BOOL
layout_func(PBOX_PTR box, VOIDPTR data)
{
	LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)data;
	RECT r;

	r.left = (int)box->x0;
	r.top = (int)box->y0;
	r.right = (int)box->x1;
	r.bottom = (int)box->y1;

//	od("Draw %d, %d, %d, %d\r\n", r);

	PatBlt(lpDrawItemStruct->hDC,
							r.left,
 							r.top,
 							r.right-r.left,
 							r.bottom-r.top,
 							PATCOPY);
	return TRUE;
}

void CPositionDialog::preview_layout(LPDRAWITEMSTRUCT lpDrawItemStruct)
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

	SHORT which = m_layout_list.GetCurSel();

	preview_rect.left++;
	preview_rect.top++;
	preview_rect.right--;
	preview_rect.bottom--;

	if (which <= 0)
	{
	/*
	// Show the object's original position (unmoved).
	// This is what they will get of they press OK at this point.
 	*/

		PBOX bound = m_object->get_bound();

	/* Get our world. */

		PBOX world;

		m_pDoc->get_panel_world(&world);

	/* Compute the dimensions of the panel we are sitting on. */

		PPNT panel_dims;

		panel_dims.x = world.x1 - world.x0;
		panel_dims.y = world.y1 - world.y0;

	/* Compute the dimensions of the rect representing the panel. */

		POINT dims;

		dims.x = preview_rect.right - preview_rect.left;
		dims.y = preview_rect.bottom - preview_rect.top;

	/* Compute the rect to draw. */

		RECT r;

		r.left = preview_rect.left
						+ (SHORT)scale_pcoord(bound.x0-world.x0, dims.x, panel_dims.x);
		r.top = preview_rect.top
						+ (SHORT)scale_pcoord(bound.y0-world.y0, dims.y, panel_dims.y);
		r.right = preview_rect.left
						+ (SHORT)scale_pcoord(bound.x1-world.x0, dims.x, panel_dims.x);
		r.bottom = preview_rect.top
						+ (SHORT)scale_pcoord(bound.y1-world.y0, dims.y, panel_dims.y);

	/* Draw the rect. */

		PatBlt(hdc,
					r.left,
					r.top,
					r.right-r.left,
					r.bottom-r.top,
					PATCOPY);
	}
	else
	{
		LayoutEntryPtr entry = (LayoutEntryPtr)m_layouts.element_in_list(which);
		LayoutElementPtr element;

	/* The world is the drawing area. */

		PBOX world;

		world.x0 = preview_rect.left;
		world.y0 = preview_rect.top;
		world.x1 = preview_rect.right;
		world.y1 = preview_rect.bottom;

	/* Get the object original dims. */

		PPNT dims;
		POINT size;

		if (m_object->original_dims(&dims))
		{
			size.x = (SHORT)dims.x;
			size.y = (SHORT)dims.y;
		}
		else
		{
			size.x = size.y = 0;
		}

	/* Generate the element rect. */

		for (element = (LayoutElementPtr)entry->get_elements()->first_element();
					element != NULL;
					element = (LayoutElementPtr)element->next_element())
		{
//			od("Generate %d, %d, %d, %d\r\n", element->record.where);
			element->generate_rects(world, size, 0, layout_func, lpDrawItemStruct);
		}
	}

/* Back with the old. */

	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

void CPositionDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_LAYOUT_PREVIEW:
		{
			preview_layout(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
		}
	}
}

void CPositionDialog::OnSelchangePositions()
{
	m_preview.Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// LayoutEntryList

LayoutEntryList::LayoutEntryList()
	: List(1, TRUE)
{
}

/*
// Read a layout list.
*/

ERRORCODE LayoutEntryList::read_layout_list(LPCSTR layout_file_name)
{
	CHAR line[120];
	ERRORCODE error;

	TextReadFile file(layout_file_name);

	file.set_read_buffer(2048);

	for (;;)
	{
		error = file.read_line(line, sizeof(line), TRF_skip_comments | TRF_skip_blank_lines);

		if (error != ERRORCODE_None)
		{
			if (error == ERRORCODE_Read)
			{
			/* Ran out of file! This is quite alright. */
				error = ERRORCODE_None;
			}
			break;
		}

	/* This is the name of the next layout. Make a new layout node. */

		LayoutEntryPtr entry;

		if ((entry = new LayoutEntry(line)) == NULL)
		{
		/* Ugh! Out of memory. */
			error = ERRORCODE_Memory;
			break;
		}

	/* Add it to us. */

		append(entry);

	/* Now read the element lines until done. */

		for (;;)
		{
			LayoutElementPtr element;

			if ((error = file.read_line(line, sizeof(line))) != ERRORCODE_None)
			{
				break;
			}

			if ((element = new LayoutElement) == NULL)
			{
				error = ERRORCODE_Memory;
				break;
			}

			FLAGS flags;
			RECT where;

			sscanf(line, "%d %d %d %d %d",
							&flags,
							&where.left, &where.top,
							&where.right, &where.bottom);

			element->record.flags = flags;
			element->record.where.left = (SHORT)where.left;
			element->record.where.top = (SHORT)where.top;
			element->record.where.right = (SHORT)where.right;
			element->record.where.bottom = (SHORT)where.bottom;

		/* Add it to the list. */

			entry->add_element(element);

		/* Look for the terminator character. */

			if (line[strlen(line)-1] == '.')
			{
				break;
			}
		}

		if (error != ERRORCODE_None)
		{
			break;
		}
	}

/* Return a measure of our success. */

	return error;
}

/*
// Read a layout list.
*/

ERRORCODE LayoutEntryList::read_layout_list(int nLayoutID)
{
	CString layout_name;

	TRY
	{
		LoadConfigurationString(nLayoutID, layout_name);
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_DoesNotExist;
	}
	END_CATCH_ALL

	return read_layout_list(layout_name);
}

/*
// Read a layout list.
*/

ERRORCODE LayoutEntryList::read_layout_list(CPmwDoc* pDoc, BOOL fDuping /*=FALSE*/)
{
	PMGPageObjectPtr pObject;

	if ((pObject = pDoc->selected_objects()) == NULL)
	{
	/* Should not happen. */
		return ERRORCODE_IntError;
	}

/* Try to load the layout list for the object type. */

	int nLayoutID;

	if (fDuping)
	{
		nLayoutID = pDoc->duplicate_layout_id();
	}
	else
	{
		PMG_OBJECT_TYPE type;

		switch (type = (PMG_OBJECT_TYPE)pObject->type())
		{
			case OBJECT_TYPE_Frame:
			{
				nLayoutID = pDoc->frame_layout_id();
				break;
			}
			case OBJECT_TYPE_Group:
			case OBJECT_TYPE_Graphic:
			case OBJECT_TYPE_Rectangle:
			case OBJECT_TYPE_Line:
			case OBJECT_TYPE_Ellipse:
			case OBJECT_TYPE_WarpText:
			default:
			{
				nLayoutID = pDoc->graphic_layout_id();
				break;
			}
#if 0
			default:
			{
				return ERRORCODE_IllegalType;
			}
#endif
		}
	}

	return read_layout_list(nLayoutID);
}
