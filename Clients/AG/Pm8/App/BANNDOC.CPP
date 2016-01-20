// banndoc.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "banndoc.h"
#include "action.h"
#include "utils.h"
#include "paper.h"
#include "objlist.h"

#include "framerec.h"
#include "frameobj.h"
#include "tblobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBannerDoc

IMPLEMENT_SERIAL(CBannerDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The banner project panel.
*/

PRIVATE PANEL_INFO
vertical_banner_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{  0,   2000, 0,    2000 },		/* Double margins. */
		0,
		2
	}
};

PRIVATE PANEL_INFO
banner_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{  2000,   0, 2000,    0 },		/* Double margins. */
		0,
		2
	}
};

/*
// The banner project info.
*/

ProjectInfo CBannerDoc::StandardInfo =
{
	PROJECT_TYPE_Banner,
	0,
	LANDSCAPE,
	sizeof(banner_panels)/sizeof(banner_panels[0]),
	IDS_BANNER_FRAME_LAYOUT,
	IDS_BANNER_GRAPHIC_LAYOUT,
	IDS_BANNER_DUPLICATE_LAYOUT,
	banner_panels
};

CBannerDoc::CBannerDoc()
{
	info = StandardInfo;
	info.project_sub_type = 1;
	banner_frame = NULL;
}

BOOL CBannerDoc::OnNewDocument()
{
	info.project_sub_type = GET_PMWAPP()->last_new_info.subtype;

	if(info.project_sub_type == BANNER_Vertical)
		info.pip = vertical_banner_panels;

	if (!CPmwDoc::OnNewDocument())
		return FALSE;

/*
// Create the default banner frame.
*/

	FRAMEOBJ_CREATE_STRUCT fcs;

	get_panel_world(&fcs.bound, 0);
	if(info.project_sub_type == BANNER_Vertical)
	{
		fcs.bound.x0 = MulDiv(2000, PAGE_RESOLUTION, 1000);
		//fcs.bound.x1 = MulDiv(1000, PAGE_RESOLUTION, 1000);
		fcs.bound.y0 = MulDiv(vertical_banner_panels[0].panel_margin.top, PAGE_RESOLUTION, 1000);

		fcs.flags = FRAME_FLAG_stretch_frame
 							| FRAME_FLAG_ystretch_frame				
							| FRAME_FLAG_use_white_space
 							| FRAME_FLAG_fit_text
 							| FRAME_FLAG_no_initial_break;

		//fcs.text = "Banner";
		fcs.alignment = ALIGN_center;
		fcs.vert_alignment = ALIGN_top;

		fcs.pStyle = NULL;
		fcs.LeftMargin =
			fcs.TopMargin =
			fcs.RightMargin =
			fcs.BottomMargin = (PCOORD)scale_number(PAGE_RESOLUTION, 5, 100);
	}
	else
	{
		fcs.bound.x0 = MulDiv(banner_panels[0].panel_margin.left, PAGE_RESOLUTION, 1000);
		
		fcs.flags = FRAME_FLAG_stretch_frame
 							| FRAME_FLAG_use_white_space
 							| FRAME_FLAG_fit_text
 							| FRAME_FLAG_no_initial_break;

		//	fcs.text = NULL;
		fcs.alignment = ALIGN_left;
		fcs.vert_alignment = ALIGN_middle;

		fcs.pStyle = NULL;
		fcs.LeftMargin =
		fcs.TopMargin =
		fcs.RightMargin =
		fcs.BottomMargin = (PCOORD)scale_number(PAGE_RESOLUTION, 5, 100);

	}
	if ((banner_frame = get_database()->create_frame_object(&fcs)) != NULL)
	{
	/* Panel so we can find it. */
		banner_frame->set_panel(MAIN_BANNER_PANEL);

	/* Select flags. */
		banner_frame->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_solo_select);

	/* Primary action. */
		banner_frame->set_primary_action(ACTION_DEFAULT_EDIT);

	/* Secondary action. */
		banner_frame->set_secondary_action(ACTION_DO_NOTHING);

		append_object(banner_frame);

	/* Compute our default banner point size. */

		SHORT size;
		PPNT dims = get_dimensions();

		if(info.project_sub_type == BANNER_Vertical)
		{
			size = (SHORT)scale_pcoord(
							dims.x - (PCOORD)PAGE_RESOLUTION,
							(90*3)/4,
							PAGE_RESOLUTION);
		}
		else
		{
			size = (SHORT)scale_pcoord(
							dims.y - (PCOORD)PAGE_RESOLUTION,
							(72*3)/4,
							PAGE_RESOLUTION);
		}

		size -= (size % 10);

	/* Set the default style for the banner frame. */

		CTextStyle Style(get_database());

		Style.SetDefault();
		Style.Size(MakeFixed(size));
		Style.BaseSize(MakeFixed(size));
		Style.UpdateFontMetrics();
		
		//Txp.SetHorizontalAlignment(fcs.alignment);

//		Style.Alignment(ALIGN_center);
//		Style.set_vertical_alignment(ALIGN_middle);

		banner_frame->SetTextStyle(&Style);
	}

	return TRUE;
}

BOOL CBannerDoc::OnOpenDocument(const char *pszPathName)
{                                       
	if (!CPmwDoc::OnOpenDocument(pszPathName))
		return FALSE;

/*
// Find the main banner frame.
*/

	if(document->GetProjectSubtype() == BANNER_Vertical)
	{
		info.pip = vertical_banner_panels;
	}

	PMGPageObjectPtr object;

	for (object = (PMGPageObjectPtr)page_objects->first_object();
				object != NULL;
				object = (PMGPageObjectPtr)object->next_object())
	{
		if (object->type() == OBJECT_TYPE_Frame
					&& object->get_panel() == MAIN_BANNER_PANEL)
		{
			banner_frame = (CFrameObject*)object;
			break;
		}
	}

	return TRUE;
}

CBannerDoc::~CBannerDoc()
{
}

/*
// Get the main frame for this project.
// This is the main banner frame.
*/

CFrameObject* CBannerDoc::get_main_frame(VOID)
{
	return banner_frame;
}

/*
// Resize the project after a frame resizes itself.
// The frame coming in MUST be the banner frame (what else could it be?)
*/

BOOL CBannerDoc::resize_project(CFrameObject* frame)
{
	BOOL fRet = FALSE;

	CPmwView* pView = GetCurrentView();
	if (pView != NULL
			&& pView->EditingText()
			&& pView->GetTxp()->Object() != NULL
			&& pView->GetTxp()->Object()->get_panel() == OBJECT_IN_CELL_PANEL)
	{
		// Just call the base class to handle the table.
		return CPmwDoc::resize_project(frame);
	}

	PCOORD left_size, frame_size, right_size;
	PCOORD top_size, bottom_size;
	PCOORD bound_x0, bound_x1,right_edge;
	PCOORD bound_y0, bound_y1,bottom_edge;
	PMGPageObjectPtr object;
	FLAGS flags;
	PCOORD max_x1, max_y1;

//	od("in banndoc resize_project()\r\n");

//	if (frame == NULL)
	{
		frame = banner_frame;			/* No confusion. */
	}

	if (frame == NULL)
	{
	/* Nothing to do. */
		return FALSE;
	}
/* ...and compute its size. */

	PBOX frame_bound = frame->get_bound();

	if(document->GetProjectSubtype() == BANNER_Vertical)
		frame_size = frame_bound.y1 - frame_bound.y0;
	else
		frame_size = frame_bound.x1 - frame_bound.x0;

/*
// Compute the size of the graphics on the end.
*/

	flags = 0;
	max_x1 = 0;
	max_y1 = 0;

	for (object = (PMGPageObjectPtr)page_objects->first_object();
				object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (object != frame)
		{
			FLAGS object_flags = object->get_flags();
			if (object->get_layout() <= 0)
			{
				if(document->GetProjectSubtype() == BANNER_Vertical)
				{
					// This has no layout.
					PCOORD y1 = object->get_bound().y1;

					if (y1 > max_y1)
					{
						max_y1 = y1;
					}

				}else{
					// This has no layout.
					PCOORD x1 = object->get_bound().x1;

					if (x1 > max_x1)
					{
						max_x1 = x1;
					}
				}
			}
			flags |= object_flags;
		}
	}

/*
// Assume no space on the ends.
*/

	if(document->GetProjectSubtype() == BANNER_Vertical)
	{
		top_size = MulDiv(vertical_banner_panels[0].panel_margin.top, PAGE_RESOLUTION, 1000);
		bottom_size = MulDiv(vertical_banner_panels[0].panel_margin.bottom, PAGE_RESOLUTION, 1000);

		max_y1 += bottom_size;				/* Add the margin. */

	/* Make sure the banner is at least as big as the page. */

		PPNT Dims = GET_PMWAPP()->GetPaperInfo(get_orientation())->GetProjectDims();

		if (max_y1 < Dims.x)
		{
			max_y1 = Dims.x;
		}

		if (max_x1 < Dims.y)
		{
			max_x1 = Dims.y;
		}

		PPNT dims = get_dimensions();

	/* Add space for any special end objects. */

		if (flags & OBJECT_FLAG_flow_right)
		{
			top_size += dims.x + top_size;
		}

		if (flags & OBJECT_FLAG_flow_left)
		{
			bottom_size += dims.x + bottom_size;
		}

		bound_y0 = top_size;
		bound_y1 = bound_y0 + frame_size;
		bottom_edge = bound_y1 + bottom_size;

		if (bottom_edge < max_y1)
		{
			bottom_edge = max_y1;
		}

	//	od("x0: %ld, x1: %ld, right: %ld\r\n", bound_x0, bound_x1, right_edge);

		
		if(frame_bound.x1 < max_x1)
		{
			frame_bound.x1 = max_x1;
			fRet = TRUE;
		}

		if (bottom_edge != dims.y
				|| frame_bound.y0 != bound_y0
				|| frame_bound.y1 != bound_y1)
		{
		/*
		// The project needs to change size.
		*/

			dims.y = bottom_edge;
			document->set_dimensions(dims);

			frame_bound.y0 = bound_y0;
			frame_bound.y1 = bound_y1;

			//GetCurrentView()->GetTxp()->SetHorizontalAlignment(ALIGN_center);

			fRet = TRUE;
		}
	
	}
	else
	{
		left_size = MulDiv(banner_panels[0].panel_margin.left, PAGE_RESOLUTION, 1000);
		right_size = MulDiv(banner_panels[0].panel_margin.right, PAGE_RESOLUTION, 1000);

		max_x1 += right_size;				/* Add the margin. */

	/* Make sure the banner is at least as big as the page. */

		PPNT Dims = GET_PMWAPP()->GetPaperInfo(get_orientation())->GetProjectDims();

		if (max_x1 < Dims.y)
		{
			max_x1 = Dims.y;
		}

		PPNT dims = get_dimensions();

	/* Add space for any special end objects. */

		if (flags & OBJECT_FLAG_flow_right)
		{
			left_size += dims.y + left_size;
		}

		if (flags & OBJECT_FLAG_flow_left)
		{
			right_size += dims.y + right_size;
		}

		bound_x0 = left_size;
		bound_x1 = bound_x0 + frame_size;
		right_edge = bound_x1 + right_size;

		if (right_edge < max_x1)
		{
			right_edge = max_x1;
		}

	//	od("x0: %ld, x1: %ld, right: %ld\r\n", bound_x0, bound_x1, right_edge);

		if (right_edge != dims.x
				|| frame_bound.x0 != bound_x0
				|| frame_bound.x1 != bound_x1)
		{
		/*
		// The project needs to change size.
		*/

			dims.x = right_edge;
			document->set_dimensions(dims);

			frame_bound.x0 = bound_x0;
			frame_bound.x1 = bound_x1;

			fRet = TRUE;
		}
	}
	frame->set_bound(frame_bound);
	frame->calc();

/* Recalc any graphic objects so they can adjust. */

	for (object = (PMGPageObjectPtr)page_objects->first_object();
				object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (object != frame)
		{
			calc_object(object);
//				od("Calc'd and got bound of %ld, %ld, %ld, %ld\r\n", object->get_bound());
		}
	}

/* Update the whole window since the project changed size. */

	set_portion();
	reset_views();

	return fRet;
}

BEGIN_MESSAGE_MAP(CBannerDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CBannerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBannerDoc commands

PMGPageObject* CBannerDoc::PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate)
{
	ASSERT(fDuplicate);
	return PasteBackgroundAsPicture(pObject, pOldWorld, pBounds);
}

/////////////////////////////////////////////////////////////////////////////
// Select next or previous object for a banner.

void CBannerDoc::SelectNextObject(void)
{
	if (CanSelectNextObject())
	{
		PMGPageObject* pObject = selected_objects();
		do
		{
			if (pObject == NULL)
			{
				// Nothing selected. Select first object.
				pObject = (PMGPageObject*)object_list()->first_object();
			}
			else if (pObject->next_selected_object() == NULL
						&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
			{
				// One object selected. Select the next object.
				pObject = (PMGPageObject*)pObject->next_object();
			}
			else
			{
				// More than one object selected. Reset.
				pObject = NULL;
			}
		} while (pObject == banner_frame);

		// Select the new object.
		deselect_all();
		if (pObject != NULL)
		{
			select_object(pObject);
		}
	}
}

BOOL CBannerDoc::CanSelectNextObject(void)
{
	return object_list()->first_object() != banner_frame
			|| banner_frame->next_object() != NULL;
}

void CBannerDoc::SelectPreviousObject(void)
{
	if (CanSelectPreviousObject())
	{
		PMGPageObject* pObject = selected_objects();

		do
		{
			if (pObject == NULL)
			{
				// Nothing selected. Select last object.
				pObject = (PMGPageObject*)object_list()->last_object();
			}
			else if (pObject->next_selected_object() == NULL
						&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
			{
				// One object selected. Select previous object.
				pObject = (PMGPageObject*)pObject->previous_object();
			}
			else
			{
				// More than one object selected. Reset.
				pObject = NULL;
			}
		} while (pObject == banner_frame);

		// Select the new object.
		deselect_all();
		if (pObject != NULL)
		{
			select_object(pObject);
		}
	}
}

BOOL CBannerDoc::CanSelectPreviousObject(void)
{
	return object_list()->first_object() != banner_frame
			|| banner_frame->next_object() != NULL;
}
