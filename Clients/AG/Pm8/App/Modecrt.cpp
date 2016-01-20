/*
// $Header: /PM8/App/MODECRT.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode creation routines.
//
// $Log: /PM8/App/MODECRT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 32    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 31    1/27/99 1:41p Lwilson
// Updated create_state_normal() to create path components for all drawing
// objects except lines.
// 
// 30    1/25/99 9:58a Lwilson
// Initial path component support
// 
// 29    10/28/98 11:39a Mwilson
// fixed IPF when undoing a canceled create table
// 
// 28    10/21/98 5:30p Jayn
// set_arrow_tool before table create dialog.
// 
// 27    8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 26    8/10/98 4:02p Hforman
// fix aspect ratio if image was cropped at creation
// 
// 25    7/24/98 4:57p Rlovejoy
// Clear flip flags on border before editing.
// 
// 24    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 23    6/02/98 3:00p Jayn
// 
// 22    5/29/98 6:38p Jayn
// Insert column, etc.
// 
// 21    5/14/98 12:38p Johno
// Use MakeInvisible() instead of Dlg.SetHotProperties()
// 
// 20    4/13/98 4:52p Johno
// Use new undo string for hot spot creation
// 
// 19    4/08/98 6:25p Jayn
// Eliminates hyperlink in new frame creation style.
// 
// 18    3/27/98 5:44p Johno
// Added ASSERT in CPmwView::create_state_dragging for  Dialog returning
// IDC_HYPER_REMOVE
// 
// 17    3/26/98 10:21a Johno
// Changed hyplnkdlg.h to hyplkdlg.h
// 
// 16    3/25/98 6:35p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 15    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 14    3/11/98 5:51p Johno
// Use in HyperlinkData struct CHyperlinkDialog
// 
// 13    3/10/98 4:16p Johno
// Changed Hyperlink handling in create_state_dragging
// 
// 12    3/06/98 5:46p Johno
// Use new SetObjectHyperlink in create_state_dragging
// 
// 11    3/04/98 5:43p Johno
// Changes for OBJECT_TYPE_Hyperlink
// 
// 10    2/25/98 3:51p Hforman
// size border to full panel (on click) in create_state_dragging()
// 
// 9     2/12/98 3:58p Hforman
// added param to ShowBorderEditor()
// 
// 8     2/11/98 6:18p Hforman
// add ReleaseCapture() call before bringing up Border Editor
// 
// 7     1/09/98 6:50p Hforman
// modify param to ShowBorderEditor()
// 
// 6     1/09/98 11:07a Hforman
// add call to ShowBorderEditor()
// 
// 5     1/08/98 3:55p Hforman
// 
// 4     1/06/98 6:50p Hforman
// border stuff
// 
// 3     12/17/97 10:47a Jstoner
// table project incremental update
// 
// 2     12/16/97 9:18a Jstoner
// added table support
// 
//    Rev 1.0   14 Aug 1997 15:22:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:12   Fred
// Initial revision.
// 
//    Rev 1.11   13 Jun 1997 16:46:32   Jay
// Resizes project; sets MANUAL_LAYOUT.
// 
//    Rev 1.10   05 Jun 1997 08:52:00   Jay
// Won't create flipped frames.
// 
//    Rev 1.9   22 Apr 1997 12:59:20   Jay
// "Single-click" creation needed a calc.
// 
//    Rev 1.8   18 Apr 1997 13:41:50   Jay
// Default stretching
// 
//    Rev 1.7   16 Apr 1997 09:37:28   Jay
// More text box enhancements.
// 
//    Rev 1.6   11 Mar 1997 16:13:50   Jay
// Extensions to the workspace
// 
//    Rev 1.5   21 Feb 1997 15:09:10   Jay
// New mode handling; crop mode support
// 
//    Rev 1.4   19 Feb 1997 16:53:14   Jay
// Snapping
// 
//    Rev 1.3   28 Jan 1997 14:10:22   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.2   27 Jan 1997 13:55:18   Jay
// Default object attributes and size.
// 
//    Rev 1.1   24 Jan 1997 12:39:10   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.0   17 Jan 1997 12:13:38   Jay
// New creation tools.
*/
#include "stdafx.h"
ASSERTNAME

#include "system.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "brdrobj.h"
#include "drawobj.h"
#include "compobj.h"
#include "oleobj.h"
#include "frameobj.h"
#include "framerec.h"
#include "pathrec.h"
#include "shapfile.h"
#include "textflow.h"
#include "util.h"
#include "utils.h"
#include "HyplkDlg.h"
#include "cntritem.h"
#include "tblprpd.h"

#include "CreateData.h"

// Framework support
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "PathInterface.h"
#include "Path.h"

// Serif drawing object class id.
// {614c7740-57c8-11ce-a50e-000021336621}
static const CLSID BASED_CODE DrawPlusCLSID = 
{ 0x614c7740, 0x57c8, 0x11ce, { 0xa5, 0x0e, 0x0, 0x0, 0x21, 0x33, 0x66, 0x21 } };

/*
// Abort the create.
*/

void CPmwView::AbortCreate(BOOL fMoving /*=TRUE*/)
{
	// Get rid of any object we may be in the process of creating.
	if (m_pCreatedObject != NULL)
	{
		CPmwDoc* pDoc = GetDocument();

		// Abort the drag.
		if (fMoving)
		{
			PPNT p;
			m_pCreatedObject->move_handle(DRAG_STATE_abort, p, OBJECT_HANDLE_LR, 0, NULL);
		}

		// Toggle the object off.
		pDoc->deselect_all();

		// Remove the object from the document list.
		pDoc->detach_object(m_pCreatedObject);

		// Delete any of the object's dependents in the document.
		m_pCreatedObject->destroy();

		// Free the object from memory.
		delete m_pCreatedObject;
		m_pCreatedObject = NULL;
	}
	set_arrow_tool();
}

/*
// Initialization state.
*/

BOOL CPmwView::create_state_init(EVENT_INFO *info)
{
	ASSERT(m_pCreatedObject == NULL);
	m_pCreatedObject = NULL;

//	deselect_all();

	UINT uCursor;

	switch (m_nTypeToCreate)
	{
		case OBJECT_TYPE_Hyperlink:
		{
         uCursor = IDC_RECT_CREATE_CURSOR;
         m_uUndoCreate = IDCmd_CreateHotspot;
         break;
      }
      case OBJECT_TYPE_Rectangle:
		{
			uCursor = IDC_RECT_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_CreateRectangle;
			break;
		}
		case OBJECT_TYPE_Ellipse:
		{
			uCursor = IDC_ELLIPSE_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_CreateEllipse;
			break;
		}
		case OBJECT_TYPE_Line:
		{
			uCursor = IDC_LINE_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_CreateLine;
			break;
		}
		case OBJECT_TYPE_Frame:
		{
			uCursor = IDC_TEXT_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_CreateTextBox;
			break;
		}
      case OBJECT_TYPE_DrawingObject:
      {
			uCursor = IDC_GENERAL_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_AddShape;
         break;
      }
      case OBJECT_TYPE_Table:
      {
			uCursor = IDC_GENERAL_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_CreateTable;
         break;
      }
      case OBJECT_TYPE_Border:
      {
			uCursor = IDC_GENERAL_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_BorderAdd;
         break;
      }
      case OBJECT_TYPE_Serif:
      {
			uCursor = IDC_GENERAL_CREATE_CURSOR;
			m_uUndoCreate = IDCmd_AddDrawingObject;
         break;
      }
		default:
		{
			ASSERT(FALSE);
			set_arrow_tool();
			return TRUE;
		}
	}
	set_cursor(AfxGetApp()->LoadCursor(uCursor));

	edit_state = STATE_NORMAL;

	return create_state_normal(info);				/* Do it now, too. */
}

/*
// Normal state.
*/

BOOL CPmwView::create_state_normal(EVENT_INFO *info)
{
	if (check_escape_event(info))
	{
		AbortCreate();
	}
	else if (info->flags & MK_LBUTTON)
	{
		CPmwDoc* pDoc = GetDocument();

		// Deselect all selected objects.
		deselect_all();

		PPNT Here;
		rc.screen_to_ppnt(info->current_xy, &Here);
		SnapPosition(&Here);

		// Create the object.
		switch (m_nTypeToCreate)
		{
			case OBJECT_TYPE_Frame:
			{
				FRAMEOBJ_CREATE_STRUCT fcs;
				fcs.bound.x0 = Here.x;
				fcs.bound.y0 = Here.y;
				fcs.bound.x1 = Here.x;
				fcs.bound.y1 = Here.y;

				fcs.alignment = (ALIGN_TYPE)m_Txp.Style().Alignment();
//				fcs.vert_alignment = (VERT_ALIGN_TYPE)m_Txp.FrameAttributes().m_nVerticalAlignment;
				fcs.vert_alignment = ALIGN_top;

				m_pCreatedObject = pDoc->create_frame_object(&fcs);

				if (m_pCreatedObject != NULL)
				{
					CFrameObject* pFrameObject = (CFrameObject*)m_pCreatedObject;
					CTxp Txp(pDoc->get_database());

					/* Stick in some text. */
					Txp.Init(pFrameObject, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);

					// Get a style to use for creation. We want to use the global Txp.
					// Do not copy over a hyperlink attribute.
					CTextStyle Style = m_Txp.Style();
					Style.HyperlinkStyle(0);
					pFrameObject->SetTextStyle(&Style);
				}

				break;
			}
			case OBJECT_TYPE_Serif:
			{
				CPmwCntrItem* pItem = NULL;
				PMGDatabase* pDatabase = pDoc->get_database();

				TRY
				{
					CPmwDoc* pDoc = GetDocument();
					ASSERT_VALID(pDoc);

					// Create new item connected to this document.
					pItem = new CPmwCntrItem(pDoc);
					ASSERT_VALID(pItem);

					// Initialize the item using the Draw Plus CLSID.
					if (!pItem->CreateNewItem(DrawPlusCLSID))
						AfxThrowMemoryException();  // any exception will do
					ASSERT_VALID(pItem);

					deselect_all();

					// Create the OLE object to contain this.
					OLEOBJECT_CREATE_STRUCT ocs;

					CSize czExtent;
					pItem->GetExtent(&czExtent);
					ocs.m_OriginalSize.x = scale_pcoord(czExtent.cx, PAGE_RESOLUTION, 2540);
					ocs.m_OriginalSize.y = scale_pcoord(czExtent.cy, PAGE_RESOLUTION, 2540);

					ocs.m_Bound.x0 = Here.x;
					ocs.m_Bound.y0 = Here.y;
					ocs.m_Bound.x1 = Here.x;
					ocs.m_Bound.y1 = Here.y;
					
					ocs.m_pItem = pItem;

					m_pCreatedObject = (OleObject*)pDatabase->new_object(m_nTypeToCreate, &ocs);
					if (m_pCreatedObject == NULL)
					{
						ThrowErrorcodeException(pDatabase->last_creation_error());
					}

					// Hook us the other way.
					pItem->Object((OleObject*)m_pCreatedObject);

//					pItem->DoVerb(OLEIVERB_OPEN, this);
					m_fStuckTool = FALSE;
				}
				CATCH(CException, e)
				{
					if (m_pCreatedObject != NULL)
					{
						delete m_pCreatedObject;
						m_pCreatedObject = NULL;
					}
					if (pItem != NULL)
					{
						ASSERT_VALID(pItem);
						pItem->Delete();
					}

					AfxMessageBox(IDP_FAILED_TO_CREATE);
				}
				END_CATCH
				break;
			}
			case OBJECT_TYPE_Ellipse:
			case OBJECT_TYPE_Rectangle:
			case OBJECT_TYPE_DrawingObject:
			{
				const int kDefaultPathSize = ::InchesToLogicalUnits( 2.0 );
				PMGDatabase* pDatabase = pDoc->get_database();
				RPath rPath;

				if (OBJECT_TYPE_Ellipse == m_nTypeToCreate)
				{
					EPathOperatorArray	opArray;	// the RPath operations
					RIntPointArray			ptArray;	// the RPath points

					RIntRect rRect( 0, 0, kDefaultPathSize, kDefaultPathSize );
					RIntPoint pt = rRect.GetCenterPoint();
					RIntPoint offset( pt.m_x * 11.0 / 20.0, pt.m_y * 11.0 / 20.0 );

					opArray.InsertAtEnd( kMoveTo );
					ptArray.InsertAtEnd( RIntPoint( pt.m_x, rRect.m_Top ) );

  					opArray.InsertAtEnd( kBezier );
					opArray.InsertAtEnd( (EPathOperator) 4 );

					ptArray.InsertAtEnd( RIntPoint( pt.m_x + offset.m_x, rRect.m_Top ) );
					ptArray.InsertAtEnd( RIntPoint( rRect.m_Right, pt.m_y - offset.m_y ) );
					ptArray.InsertAtEnd( RIntPoint( rRect.m_Right, pt.m_y ) );

					ptArray.InsertAtEnd( RIntPoint( rRect.m_Right, pt.m_y + offset.m_y ) );
					ptArray.InsertAtEnd( RIntPoint( pt.m_x + offset.m_x, rRect.m_Bottom ) );
					ptArray.InsertAtEnd( RIntPoint( pt.m_x, rRect.m_Bottom ) );

					ptArray.InsertAtEnd( RIntPoint( pt.m_x - offset.m_x, rRect.m_Bottom ) );
					ptArray.InsertAtEnd( RIntPoint( rRect.m_Left, pt.m_y + offset.m_y ) );
					ptArray.InsertAtEnd( RIntPoint( rRect.m_Left, pt.m_y ) );

					ptArray.InsertAtEnd( RIntPoint( rRect.m_Left, pt.m_y - offset.m_y ) );
					ptArray.InsertAtEnd( RIntPoint( pt.m_x - offset.m_x, rRect.m_Top ) );
					ptArray.InsertAtEnd( RIntPoint( pt.m_x, rRect.m_Top ) );

					opArray.InsertAtEnd( kClose );
					opArray.InsertAtEnd( kEnd );

					rPath.Define( opArray, ptArray );
				}
				else if (OBJECT_TYPE_Rectangle == m_nTypeToCreate)
				{
					RIntRect rRect( 0, 0, kDefaultPathSize, kDefaultPathSize );
					rPath.Define( rRect );
				}
				else
				{
					CShapeEntry* pShapeEntry = GetShapeEntry();
					ASSERT(pShapeEntry != NULL);

//					CFixedRect rect;
//					pShapeEntry->m_Path.BoundingBox( &rect );

					R2dTransform xform;
					if (!pShapeEntry->m_Path.GetRPath( rPath, xform ))
						break;
				}

				// Create a new shape component
				YComponentBoundingRect rRect = RRealRect( rPath.GetBoundingRect() );
				RComponentDocument* pNewDoc = ComponentObject::CreateNewComponent( kPathComponent, rRect );

				try
				{
					RComponentView* pNewView = (RComponentView *) pNewDoc->GetActiveView();
					IPathInterface* pInterface = (IPathInterface *) pNewView->GetInterface( kPathInterfaceId );
					TpsAssert( pInterface, "Could not obtain path interface to component!" );

					pInterface->SetPath( rPath );
					delete pInterface;

					CComponentData cCreationData( pNewDoc );
					m_pCreatedObject = (PMGPageObject*) pDatabase->new_object(
						OBJECT_TYPE_Component, (void *) &cCreationData );
				}
				catch (...)
				{
					delete pNewDoc;
					break;
				}

				// Fall Through
			}
			default:
			{
				PMGDatabase* pDatabase = pDoc->get_database();

				// Create the correct object.
				if (!m_pCreatedObject)
					m_pCreatedObject = (PMGPageObject*)pDatabase->new_object(m_nTypeToCreate, NULL);

				// Set the bound to be where we are now.
				// We assume this is a RectPageObject (everything is as of now).
				if (m_pCreatedObject != NULL)
				{
					PBOX Bound;
					Bound.x0 = Here.x;
					Bound.y0 = Here.y;
					Bound.x1 = Here.x;
					Bound.y1 = Here.y;
					((RectPageObject*)m_pCreatedObject)->set_bound(Bound);
				}
				break;
			}
		}

		if (m_pCreatedObject == NULL)
		{
			// Failed to create. Force a release.
			edit_state = STATE_RELEASE;
		}
		else
		{
			// Apply the properties to this object.
			m_pCreatedObject->SetObjectProperties(GetDocument()->GetDefaultObjectProperties());

			// We have the object. Begin dragging it.
			m_pCreatedObject->calc();

			// Set the panel for the object.
			m_pCreatedObject->set_panel(pDoc->get_current_panel());

			// Add us to the document...
			pDoc->append_object(m_pCreatedObject);

			// Select us.
			pDoc->select_object(m_pCreatedObject);

			// Begin the drag.
			m_pCreatedObject->move_handle(DRAG_STATE_begin, Here, OBJECT_HANDLE_LR, info->flags, NULL);

			edit_state = CREATE_STATE_DRAGGING;
		}
	}
	return TRUE;
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CPmwView::create_state_release(EVENT_INFO *info)
{
	if (!(info->flags & MK_LBUTTON))
	{
	/* Done waiting! */
		set_arrow_tool();
	}

	return TRUE;
}

BOOL CPmwView::create_state_dragging(EVENT_INFO *info)
{
	if (check_escape_event(info))
	{
		// We want to get out of here.
		AbortCreate();
		return TRUE;
	}

	// Compute where we are.

	PPNT p;

	rc.screen_to_ppnt(info->current_xy, &p);
	SnapPosition(&p);

	if (info->flags & MK_LBUTTON)
	{
		if (info->new_position)
		{
			// Continue the drag.
			m_pCreatedObject->move_handle(DRAG_STATE_continue, p, OBJECT_HANDLE_LR, info->flags, NULL);
		}
	}
	else
	{
		// Finish the drag.
		m_pCreatedObject->move_handle(DRAG_STATE_end, p, OBJECT_HANDLE_LR, info->flags, NULL);
		m_pCreatedObject->set_new_layout(MANUAL_LAYOUT);
		m_pCreatedObject->calc();

		CPmwDoc* pDoc = GetDocument();

		PBOX Bound = m_pCreatedObject->get_bound();
		if (Bound.x1 - Bound.x0 < rc.redisplay_x_pixel*4
				&& Bound.y1 - Bound.y0 < rc.redisplay_y_pixel*4)
		{
			// Too small! Make it bigger.

			PPNT p;
			switch (m_pCreatedObject->type())
			{
				case OBJECT_TYPE_Frame:
				{
					PBOX world;

					pDoc->get_panel_world(&world);

					IntersectBox(&world, &world, &rc.source_pbox);

					p.x = ((world.x1-world.x0)*2)/3;
					p.y = ((world.y1-world.y0)*1)/3;
					break;
				}
				case OBJECT_TYPE_Border:
				{
					// size to full page
					PBOX world;
					pDoc->get_panel_world(&world);
					Bound.x0 = world.x0;
					Bound.y0 = world.y0;
					p.x = world.x1 - world.x0;
					p.y = world.y1 - world.y0;
					break;

				}
				default:
				{
					if (!m_pCreatedObject->original_dims(&p))
					{
						p.x = PAGE_RESOLUTION;
						p.y = PAGE_RESOLUTION;
					}
					else
					{
						// image may have been cropped, so scale appropriately
						CDoubleRect& rect = m_pCreatedObject->GetVisibleRect();
						p.x = PCOORD((double)p.x * rect.Width());
						p.y = PCOORD((double)p.y * rect.Height());
					}
					break;
				}
			}
			Bound.x1 = Bound.x0 + p.x;
			Bound.y1 = Bound.y0 + p.y;
			pDoc->toggle_object(m_pCreatedObject);
			((RectPageObject*)m_pCreatedObject)->set_unrotated_bound(Bound);
			m_pCreatedObject->calc();
			pDoc->toggle_object(m_pCreatedObject);
		}

		// Make us show up right.
		pDoc->refresh_object(m_pCreatedObject);
		if (m_nTypeToCreate != OBJECT_TYPE_Table)
			pDoc->AddCreateCommand(m_uUndoCreate);

		// And resize the project in case it goes off the edge of a banner.
		pDoc->resize_project();

		// Handle the initial stretch state for a text frame.
		// It seems really ugly having this here...
		if (m_nTypeToCreate == OBJECT_TYPE_Frame)
		{
			GET_PMWAPP()->ShowHelpfulHint("Adding Text Box");
			// If stretching is turned on, set it now.
			if (m_fDefaultStretching)
			{
				CFrameRecord* pFrame = NULL;
				TRY
				{
					CFrameObject* pObject = (CFrameObject*)m_pCreatedObject;
					pFrame = pObject->LockFrameRecord();

					pFrame->FrameFlags(pFrame->FrameFlags() | FRAME_FLAG_stretch_text);

					PBOX Bound = pFrame->ObjectBounds();
					PPNT StretchDims;
					StretchDims.x = Bound.x1 - Bound.x0;
					StretchDims.y = Bound.y1 - Bound.y0;
					pFrame->StretchDims(StretchDims);

					CTextFlow TextFlow(pObject, 0, -1);
					TextFlow.FlowText();
				}
				END_TRY

				if (pFrame != NULL)
				{
					pFrame->release();
				}
			}

			// If we ended up flipped, un-flip us.
			FLAGS Flags = m_pCreatedObject->get_flags();
			if (Flags & (OBJECT_FLAG_xflipped|OBJECT_FLAG_yflipped))
			{
				pDoc->toggle_selects();
				if (Flags & OBJECT_FLAG_xflipped) m_pCreatedObject->xflip();
				if (Flags & OBJECT_FLAG_yflipped) m_pCreatedObject->yflip();
				pDoc->toggle_selects();
			}
		}
		else if (m_nTypeToCreate == OBJECT_TYPE_Table)
		{
			ReleaseCapture();
			set_arrow_tool();
			m_fStuckTool = FALSE;
			CTableCreateDialog Dialog((CTableObject*)m_pCreatedObject);
			if (Dialog.DoModal() == IDOK)
			{
				pDoc->AddCreateCommand(m_uUndoCreate);
				// Set the properties for the table.
				pDoc->SetModifiedFlag();
				pDoc->refresh_object(m_pCreatedObject);
			}
			else
			{
				// Get rid of the unformatted table.
				PMGPageObject* pObject = pDoc->selected_objects();
				pDoc->deselect_all();
				pDoc->detach_object(pObject);
				pDoc->get_database()->delete_object(pObject);
			}
		}

		// And we are done.
		m_pCreatedObject = NULL;

		// Done with this object. What now?
		if (m_fStuckTool)
		{
			// Start over within this tool (mode).
			edit_state = STATE_INIT;
		}
		else
		{
			// Go back to arrow tool state.
			set_arrow_tool();

			switch (m_nTypeToCreate)
			{
				case OBJECT_TYPE_Frame:
				{
					edit_frame((CFrameObject*)pDoc->selected_objects(), NULL);
					break;
				}
				case OBJECT_TYPE_Border:
				{
					ReleaseCapture();	// let border editor have the capture!
					BorderObject* pObject = (BorderObject*)pDoc->selected_objects();
					pObject->remove_flags(OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped); // No flipping to start
					pObject->ShowBorderEditor(TRUE);
					break;
				}
            // We made a hot spot object; deal with it...
            case OBJECT_TYPE_Hyperlink:
            {
               ReleaseCapture();	// let CHyperlinkDialog have the capture too!
               CHyperlinkDialog  Dlg(this);

               PMGPageObject* prect =  pDoc->selected_objects();
               // Make this thing transparent 
               // (the drawing code should highlight it)
               prect->MakeInvisible();
               // The user change his mind?
               PMGDatabase* pDatabase;
               HyperlinkData  Data;
               Dlg.SetData(&Data, pDoc);
               
               switch (Dlg.DoModal())
               {
                  case IDCANCEL:
                  pDoc->deselect_all();   // Get rid of object
                  pDoc->detach_object(prect);
                  pDatabase = pDoc->get_database();
                  pDatabase->delete_object(prect);
                  break;

                  case IDOK:
                  Dlg.GetData(&Data);
                  prect->SetHyperlinkRecord(&Data);
                  pDoc->refresh_object(prect);
                  pDoc->SetModifiedFlag();
                  break;

                  case IDC_HYPER_REMOVE:
// This should not be available
// to a new hot spot
ASSERT(FALSE);
                  prect->KillHyperlinkRecord();
                  pDoc->SetModifiedFlag();
                  break;
               }
            }
            break;

				case OBJECT_TYPE_Serif:
				{
               OleObject* pObject = (OleObject*)pDoc->selected_objects();
					ASSERT(pObject != NULL);
					if (pObject != NULL)
					{

						// Convert bound dimensions to HIMETRIC.
						PBOX Bound = pObject->get_bound();
						CSize czExtent;
						czExtent.cx = scale_pcoord(Bound.x1-Bound.x0, 2540, PAGE_RESOLUTION);
						czExtent.cy = scale_pcoord(Bound.y1-Bound.y0, 2540, PAGE_RESOLUTION);
						((CPmwCntrItem*)pObject->Item())->SetExtent(czExtent);
//						((CPmwCntrItem*)pObject->Item())->SetExtentAndRects();

						// Go and edit the object.
						Util::PostWmCommand(this, ID_EDIT_DRAWING_OBJECT, NULL, 0);
					}
					break;
				}

				default:
				{
					break;
				}
			}
		}

		// Handle anything else after a position change.
		AfterPositionChange();
	}

	return TRUE;
}

/*
// The states for the text create mode.
*/

CPmwView::STATE_HANDLER CPmwView::create_state[] =
{
	CPmwView::create_state_init,
	CPmwView::create_state_normal,
	CPmwView::create_state_release,
	CPmwView::create_state_dragging
};

#define	CREATE_STATE_COUNT			(sizeof(create_state)/sizeof(create_state[0]))

/*
// The create mode handler.
*/

void CPmwView::mode_create(EVENT_INFO *info)
{
	if (info->event >= EVENT_ENTERING_MODE)
	{
		// Special mode event.
		return;
	}

	// Process a normal event.
	SHORT state = edit_state;

	if (state > STATE_BASE_END)
	{
		if (state >= CREATE_STATE_BASE)
		{
		/* One of our special states! */
			state -= (CREATE_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = CREATE_STATE_COUNT;
		}
	}

	if (state < CREATE_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*create_state[state])(info);
	}
}
