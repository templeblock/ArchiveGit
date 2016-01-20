/*
// $Header: /PM8/App/PMGDB.CPP 1     3/03/99 6:09p Gbeddow $
//
// PMG database object routines.
// This file contains methods for PMG database objects
//
// $Log: /PM8/App/PMGDB.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 23    2/26/99 3:44p Johno
// CopyObjectList() now copies everything; objects, page properties, guide
// information
// 
// 22    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 21    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 20    6/30/98 3:02p Johno
// Added SetHyperlinkData()
// 
// 19    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 18    5/29/98 6:38p Jayn
// Insert column, etc.
// 
// 17    3/31/98 11:44a Jayn
// Hyperlink record reference count fixes.
// 
// 16    3/31/98 10:15a Johno
// Changes for reference counts
// 
// 15    3/27/98 5:45p Johno
// Reference count stuff for hyperlinks
// 
// 14    3/25/98 6:36p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 13    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 12    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 11    3/11/98 5:52p Johno
// Use HyperlinkData struct
// 
// 10    3/10/98 4:10p Johno
// Added NewHyperlinkRecord, FreeHyperlinkRecord
// 
// 9     3/05/98 6:29p Johno
// New register_creation_method for RECORD_TYPE_Hyperlink
// 
// 8     3/04/98 5:45p Johno
// Added register_object_creation_method(OBJECT_TYPE_Hyperlink,
// RectangleObject::create);
// 
// 7     1/21/98 9:19a Jstoner
// cell/table stuff
// 
// 6     1/07/98 8:15a Jstoner
// added cell object creation 
// 
// 5     1/05/98 4:54p Hforman
// add Border Object stuff
// 
// 4     12/16/97 9:18a Jstoner
// added table support
// 
// 3     12/11/97 10:45a Jstoner
// added tableobject type 
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.0   14 Aug 1997 15:23:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:46   Fred
// Initial revision.
// 
//    Rev 1.25   20 Jul 1997 13:38:38   Jay
// CopyObjectList now calcs dup'd objects.
// 
//    Rev 1.24   29 May 1997 08:45:46   Fred
// Converts old title text boxes to frame objects
// 
//    Rev 1.23   26 Mar 1997 15:18:48   Fred
// Removed old geronimo stuff
// 
//    Rev 1.22   10 Oct 1996 14:30:34   Jay
// More time stamps
// 
//    Rev 1.21   29 Jul 1996 14:58:18   Jay
// Frame outline control
// 
//    Rev 1.20   28 Jul 1996 11:53:22   Jay
// Got rid of calendar and brochure info records.
// 
//    Rev 1.19   28 Jul 1996 10:16:34   Jay
// Added brochure record.
// 
//    Rev 1.18   26 Jul 1996 11:38:56   Jay
// Page stuff
// 
//    Rev 1.17   22 Jul 1996 18:06:56   Jay
// Put in missing release()
// 
//    Rev 1.16   16 Jul 1996 11:25:06   Jay
// Linked frame copy support
// 
//    Rev 1.15   10 Jul 1996 10:25:20   Jay
// Text states no longer allocated.
// 
//    Rev 1.14   08 Jul 1996 17:51:34   Jay
// Linked frames, etc.
// 
//    Rev 1.13   03 Jul 1996 14:30:28   Jay
//  
// 
//    Rev 1.12   02 Jul 1996 08:55:06   Jay
//  
// 
//    Rev 1.11   28 Jun 1996 16:18:16   Jay
//  
// 
//    Rev 1.10   26 Jun 1996 14:21:44   Jay
//  
// 
//    Rev 1.9   26 Jun 1996 09:06:52   Jay
//  
// 
//    Rev 1.8   24 Jun 1996 18:06:08   Jay
//  
// 
//    Rev 1.7   19 Jun 1996 17:32:12   Jay
//  
// 
//    Rev 1.6   24 May 1996 09:26:58   Jay
// New text code.
// 
//    Rev 1.5   25 Apr 1996 12:16:26   Jay
//  
// 
//    Rev 1.4   23 Apr 1996 08:07:32   Jay
// More new stuff
// 
//    Rev 1.3   12 Apr 1996 13:07:22   Jay
// Added new frame registration (though commented out).
// 
//    Rev 1.2   12 Apr 1996 12:59:50   Jay
// Added new text types.
// 
//    Rev 1.1   11 Apr 1996 15:35:56   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:40   FRED
// Initial revision.
// 
//    Rev 2.14   27 Sep 1995 15:25:40   JAY
// Added CDocument member to database.
// 
//    Rev 2.13   07 Jul 1995 16:10:46   JAY
// Misc changes.
// 
//    Rev 2.12   13 May 1995 11:14:18   JAY
//  
// 
//    Rev 2.11   11 May 1995 09:37:50   JAY
// Changed warp field CreateStruct to an external class.
// 
//    Rev 2.10   09 May 1995 08:05:24   JAY
// Added EllipseObject and LineObject support.
// 
//    Rev 2.9   04 May 1995 17:16:06   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 2.8   02 May 1995 15:37:50   JAY
// Improved macro support.
// 
//    Rev 2.7   01 May 1995 15:24:48   JAY
// Restructured the macro stuff
// 
//    Rev 2.6   01 May 1995 08:43:16   JAY
// Access to macro list record.
// 
//    Rev 2.5   27 Apr 1995 13:21:34   FRED
//  
// 
//    Rev 2.4   19 Apr 1995 14:46:36   FRED
// Added preliminary support for warped text object
// 
//    Rev 2.3   19 Apr 1995 12:31:54   JAY
//  
// 
//    Rev 2.2   29 Mar 1995 08:59:02   JAY
// Lots of changes
// 
//    Rev 2.1   09 Mar 1995 08:27:12   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:02   JAY
// Initial revision.
// 
//    Rev 1.14   30 Jan 1995 09:17:22   JAY
// Now duplicates layout elements when creating a new layout record.
// 
//    Rev 1.13   30 Nov 1994 16:33:14   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.12   04 Aug 1994 07:32:34   JAY
// Large model and 32-bit changes
// 
//    Rev 1.11   14 Jun 1994 16:50:40   JAY
// No longer manages a Geronimo object (it's no longer needed).
// 
//    Rev 1.10   09 Jun 1994 11:12:10   JAY
// Hooks for GraphicCache
// 
//    Rev 1.9   01 Jun 1994 16:41:50   JAY
// Moved some code down into Database proper to make it more useful.
// 
//    Rev 1.8   31 May 1994 12:40:00   JAY
// Added prep_graphic_file() hook to the external world.
// 
//    Rev 1.7   23 May 1994 08:36:10   JAY
// new_graphic_record() now takes a GRAPHIC_CREATE_STRUCT pointer, not just
// the filename.
// Added registration of embedded file type.
// 
//    Rev 1.6   11 May 1994 16:33:28   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.5   31 Mar 1994 16:03:24   JAY
// Added argument to missing picture handler
// 
//    Rev 1.4   09 Mar 1994 09:22:08   MACDUFF
// Added ExtraPixels param to refresh extents
// 
//    Rev 1.3   30 Jan 1994 09:49:04   JAY
//  
// 
//    Rev 1.2   25 Jan 1994 15:18:24   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:05:30   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:44   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "pmgdb.h"
#include "geronimo.h"
#include "tfengine.h"

#include "calrec.h"
#include "prevrec.h"
#include "docrec.h"
#include "fontrec.h"
#include "pagerec.h"
#include "framerec.h"
#include "pararec.h"
#include "layrec.h"
#include "arrayrec.h"
#include "efilerec.h"
#include "macrorec.h"
#include "namerec.h"
#include "warpfld.h"
#include "pathrec.h"
#include "textrec.h"
#include "stylerec.h"
#include "hyperrec.h"
#include "comprec.h"
#include "fontsrv.h"
#include "macrosrv.h"

#include	"patobj.h"
#include	"grafobj.h"
#include	"grpobj.h"
#include	"frameobj.h"
#include	"backobj.h"
#include	"dateobj.h"
#include	"calobj.h"
//#include "warpobj.h"
#include "drawobj.h"
#include "rectobj.h"
#include "elliobj.h"
#include "lineobj.h"
#include "brdrobj.h"
#include "tblobj.h"
#include "textflow.h"
#include "celobj.h"
#include "oleobj.h"
#include "compobj.h"

/*****************************************************************************/
/*                          PMG Database methods                             */
/*****************************************************************************/

/*
// The constructor for the PMG database.
*/

PMGDatabase::PMGDatabase(StorageDevicePtr device, FontServerPtr fs)
			: ObjectDatabase(device),
			  font_server(fs),
			  m_pMacroServer(NULL),
 			  refresh_notify(NULL),
 			  missing_graphic_handler(NULL),
			  object_toggle_handler(NULL),
			  load_string_handler(NULL),
			  document_modified_handler(NULL),
			  m_graphic_cache(NULL),
			  m_pDocument(NULL),
			  m_lCurrentPage(0),
			  m_fShowFrameOutlines(TRUE)
{
/* Record creation methods. */

	register_creation_method(RECORD_TYPE_ProjectPreview, ProjectPreviewRecord::create);
	register_creation_method(RECORD_TYPE_Document, DocumentRecord::create);
	register_creation_method(RECORD_TYPE_FontData, FontDataRecord::create);
	register_creation_method(RECORD_TYPE_Page, PageRecord::create);
	register_creation_method(RECORD_TYPE_Graphic, GraphicRecord::create);
	register_creation_method(RECORD_TYPE_OldFrame, FrameRecord::create);
	register_creation_method(RECORD_TYPE_Paragraph, ParagraphRecord::create);
	register_creation_method(RECORD_TYPE_ObjectLayout, ObjectLayoutRecord::create);
	register_creation_method(RECORD_TYPE_CalendarInfo, CalendarInfoRecord::create);
	//UNUSED2
	register_creation_method(RECORD_TYPE_EmbeddedFile, EmbeddedFileRecord::create);
	register_creation_method(RECORD_TYPE_MacroList, MacroListRecord::create);
	register_creation_method(RECORD_TYPE_NameList, NameListRecord::create);
	register_creation_method(RECORD_TYPE_WarpField, WarpFieldRecord::create);
	register_creation_method(RECORD_TYPE_OutlinePath, OutlinePathRecord::create);
	register_creation_method(RECORD_TYPE_Array, CArrayRecord::create);
	register_creation_method(RECORD_TYPE_ArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_RecordArray, CRecordArrayRecord::create);
	register_creation_method(RECORD_TYPE_RecordArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_CharacterArray, CCharacterArrayRecord::create);
	register_creation_method(RECORD_TYPE_CharacterArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_WordArray, CWordArrayRecord::create);
	register_creation_method(RECORD_TYPE_WordArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_StyleArray, CStyleArrayRecord::create);
	register_creation_method(RECORD_TYPE_StyleArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_ParagraphArray, CParagraphArrayRecord::create);
	register_creation_method(RECORD_TYPE_ParagraphArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_LineArray, CLineArrayRecord::create);
	register_creation_method(RECORD_TYPE_LineArrayBlock, CArrayElementRecord::create);
	register_creation_method(RECORD_TYPE_Text, CTextRecord::create);
	register_creation_method(RECORD_TYPE_Frame, CFrameRecord::create);
	register_creation_method(RECORD_TYPE_TextStyle, CTextStyleRecord::create);
	register_creation_method(RECORD_TYPE_Hyperlink, HyperlinkRecord::create);
	register_creation_method(RECORD_TYPE_Component, ComponentRecord::create);

/* Object creation methods. */

	register_object_creation_method(OBJECT_TYPE_Graphic, GraphicObject::create);
	register_object_creation_method(OBJECT_TYPE_Pattern, PatternObject::create);
	register_object_creation_method(OBJECT_TYPE_Group, GroupObject::create);
	register_object_creation_method(OBJECT_TYPE_OldFrame, CFrameObject::create);
	register_object_creation_method(OBJECT_TYPE_Background, BackgroundObject::create);
	register_object_creation_method(OBJECT_TYPE_Calendar, CalendarObject::create);
	register_object_creation_method(OBJECT_TYPE_OldCalendarDate, CalendarDateObject::create);
	register_object_creation_method(OBJECT_TYPE_OldWarpText, CFrameObject::create);
	register_object_creation_method(OBJECT_TYPE_Rectangle, RectangleObject::create);
	register_object_creation_method(OBJECT_TYPE_Ellipse, EllipseObject::create);
	register_object_creation_method(OBJECT_TYPE_Line, LineObject::create);
	register_object_creation_method(OBJECT_TYPE_DrawingObject, DrawingObject::create);
	register_object_creation_method(OBJECT_TYPE_OleObject, OleObject::create);
	register_object_creation_method(OBJECT_TYPE_Frame, CFrameObject::create);
	register_object_creation_method(OBJECT_TYPE_CalendarDate, CalendarDateObject::create);
	register_object_creation_method(OBJECT_TYPE_WarpText, CFrameObject::create);
	register_object_creation_method(OBJECT_TYPE_Table, CTableObject::create);
	register_object_creation_method(OBJECT_TYPE_Border, BorderObject::create);
	register_object_creation_method(OBJECT_TYPE_Cell, CCellObject::create);
	register_object_creation_method(OBJECT_TYPE_Hyperlink, RectangleObject::create);
	register_object_creation_method(OBJECT_TYPE_Serif, OleObject::create);
	register_object_creation_method(OBJECT_TYPE_Component, ComponentObject::create);
}

/////////////////////////////////////////////////////////////////////////////
// Graphic record management.

/*
// Create a new graphic record.
*/

DB_RECORD_NUMBER PMGDatabase::new_graphic_record(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	DB_RECORD_NUMBER g_record;
	GraphicPtr graphic;
	DocumentPtr document;

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &m_last_error, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the graphic in the existing array. */

	LPCSTR name = (gcs == NULL) ? NULL : (LPCSTR)gcs->m_csFileName;

	if ((g_record = document->find_graphic(name)) == 0)
	{
	/*
	//	Create a new GRAPHIC_OBJECT data structure.
	// The graphic will initialize itself.
	*/

	   if ((graphic = create_graphic(gcs)) != NULL)
		{
		/* Get the record number. */

			g_record = graphic->Id();

		/* Insert the graphic number into the document graphic array. */

			document->add_graphic(g_record);
		}
	}
	else
	{
		/*
		//	Get the record so we can lock it.
		*/

		graphic = (GraphicPtr)get_record(g_record, &m_last_error, RECORD_TYPE_Graphic);
	}

/* Release the document. */

	document->release();

	if (graphic == NULL)
	{
		return 0;
	}

/* Update the reference count. */

	graphic->record.reference_count++;

/* We're done with it. */

	graphic->release(TRUE);

	return g_record;
}

/*
// Increment the reference count for a graphic record.
*/

ERRORCODE PMGDatabase::inc_graphic_record(DB_RECORD_NUMBER g_record)
{
	if (g_record <= 0)
	{
		return ERRORCODE_None;
	}

	GraphicPtr graphic;
	ERRORCODE error;

	if ((graphic = (GraphicPtr)get_record(g_record, &error, RECORD_TYPE_Graphic)) != NULL)
	{
		graphic->record.reference_count++;
		graphic->release(TRUE);
	}
	return error;
}

/*
// Free a graphic record.
*/

ERRORCODE PMGDatabase::free_graphic_record(DB_RECORD_NUMBER g_record)
{
	GraphicPtr graphic;
	DocumentPtr document;
	ERRORCODE error;

	if (g_record <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the graphic so we can access it.
*/

	if ((graphic = (GraphicPtr)get_record(g_record, &error, RECORD_TYPE_Graphic)) != NULL)
	{
		/*
		// Only free the data if the reference count goes to zero.
		*/

		if (--(graphic->record.reference_count) == 0)
		{
		/* Time to free the graphic. */

	      graphic->release();

		/* Remove us from the document's graphic list. */

			if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
			{
				document->remove_graphic(g_record);
				document->release();

			/* Delete the record from the database. */

		      delete_record(g_record);
			}
		}
		else
		{
			graphic->release(TRUE);
		}
	}
	return error;
}

/*
// Handle a missing graphic.
*/

BOOL PMGDatabase::handle_missing_graphic(GraphicPtr graphic, GRAPHIC_CREATE_STRUCT_PTR gcs, int16 *index)
{
	od("Handle missing graphic %s\r\n", (LPCSTR)graphic->m_csFileName);
	if (missing_graphic_handler != NULL)
	{
		return missing_graphic_handler(missing_graphic_data, graphic, gcs, index);
	}
	return FALSE;
}

/*
// Update a graphic record.
*/

void PMGDatabase::update_graphic_record(LPCSTR pszName)
{
	// Try to lock the document so we can access it.
	// This should always work, but we will check for error anyway.

	DocumentPtr pDocument;
	if ((pDocument = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &m_last_error, RECORD_TYPE_Document)) != NULL)
	{
		// Try to find the graphic in the existing array.

		DB_RECORD_NUMBER g_record;
		if ((g_record = pDocument->find_graphic(pszName)) != 0)
		{
			//	Get the record so we can lock it.

			GraphicPtr pGraphic;
			if ((pGraphic = (GraphicPtr)get_record(g_record, &m_last_error, RECORD_TYPE_Graphic)) != NULL)
			{
				pGraphic->reinit();
				pGraphic->release(TRUE);
			}
		}
		pDocument->release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Layout record management.

/*
// Create a new layout record.
// The elements of the list become owned by the layout when done.
*/

DB_RECORD_NUMBER PMGDatabase::new_layout_record(LPSTR name, SHORT type, ListPtr list)
{
	DB_RECORD_NUMBER l_record;
	ObjectLayoutPtr layout;
	DocumentPtr document;

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the layout in the existing array. */

	if ((l_record = document->find_layout(name, type)) == 0)
	{
	/*
	//	Create a new ObjectLayout data structure.
	*/

	   if ((layout = create_object_layout()) != NULL)
		{
		/* Copy the elements over (duplicating them). */

			ListElementPtr element;

			for (element = list->first_element();
					element != NULL;
					element = element->next_element())
			{
				LayoutElementPtr pOldElement = (LayoutElementPtr)element;
				LayoutElementPtr pNewElement;

				if ((pNewElement = new LayoutElement) == NULL)
				{
					return ERRORCODE_Memory;
				}

				pNewElement->record.flags = pOldElement->record.flags;
				pNewElement->record.where = pOldElement->record.where;
				pNewElement->record.offset = pNewElement->record.offset;

				layout->add_element(pNewElement);
			}

		/* Set the name. */

			layout->set_name(name);

		/* Set the type. */

			layout->set_type(type);

		/* Get the record number. */

			l_record = layout->Id();

		/* Insert the layout number into the document layout array. */

			document->add_layout(l_record);
		}
	}
	else
	{
		/*
		//	Get the record so we can lock it.
		*/

		layout = (ObjectLayoutPtr)get_record(l_record, NULL, RECORD_TYPE_ObjectLayout);
	}

/* Release the document. */

	document->release();

	if (layout == NULL)
	{
		return 0;
	}

/* Update the reference count. */

	layout->inc_reference_count();

/* We modified it. */
/* We're done with it. */

	layout->release(TRUE);

	return l_record;
}

/*
// Increment the reference count for an object layout record.
*/

ERRORCODE PMGDatabase::inc_layout_record(DB_RECORD_NUMBER l_record)
{
	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

	ObjectLayoutPtr layout;
	ERRORCODE error;

	if ((layout = (ObjectLayoutPtr)get_record(l_record, &error, RECORD_TYPE_ObjectLayout)) != NULL)
	{
		layout->inc_reference_count();
		layout->release(TRUE);
	}
	return error;
}

/*
// Free a layout record.
*/

ERRORCODE PMGDatabase::free_layout_record(DB_RECORD_NUMBER l_record)
{
	ObjectLayoutPtr layout;
	DocumentPtr document;
	ERRORCODE error;

	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the layout so we can access it.
*/

	if ((layout = (ObjectLayoutPtr)get_record(l_record, &error, RECORD_TYPE_ObjectLayout)) != NULL)
	{
		/*
		// Only free the data if the reference count goes to zero.
		*/

		if (layout->dec_reference_count() == 0)
		{
		/* Time to free the layout. */

			layout->release();

		/* Remove us from the document's layout list. */

			if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
			{
				document->remove_layout(l_record);
				document->release();

			/* Delete the record from the database. */

		      delete_record(l_record);
			}
		}
		else
		{
			layout->release(TRUE);
		}
	}
	return error;
}

/////////////////////////////////////////////////////////////////////////////
// Warp field record management.

/*
// Create a new warp field record.
*/

DB_RECORD_NUMBER PMGDatabase::new_warp_field_record(WarpField_CreateStruct* pCreateStruct)
{
	DB_RECORD_NUMBER l_record;
	WarpFieldRecord* pRecord;
	DocumentPtr document;

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the warp field in the existing array. */

	if ((l_record = document->find_warp_field(pCreateStruct->m_pszName)) == 0)
	{
	/*
	//	Create a new WarpFieldRecord data structure.
	*/

	   if ((pRecord = create_warp_field(pCreateStruct)) != NULL)
		{
		/* Get the record number. */

			l_record = pRecord->Id();

		/* Insert the warp field number into the document warp field array. */

			document->add_warp_field(l_record);
		}
	}
	else
	{
		/*
		//	Get the record so we can lock it.
		*/

		pRecord = (WarpFieldRecord*)get_record(l_record, NULL, RECORD_TYPE_WarpField);
	}

/* Release the document. */

	document->release();

	if (pRecord == NULL)
	{
		return 0;
	}

/* Update the reference count. */

	pRecord->inc_reference_count();

/* We modified it. */
/* We're done with it. */

	pRecord->release(TRUE);

	return l_record;
}

/*
// Increment the reference count for an warp field record.
*/

ERRORCODE PMGDatabase::inc_warp_field_record(DB_RECORD_NUMBER l_record)
{
	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

	WarpFieldRecord* pRecord;
	ERRORCODE error;

	if ((pRecord = (WarpFieldRecord*)get_record(l_record, &error, RECORD_TYPE_WarpField)) != NULL)
	{
		pRecord->inc_reference_count();
		pRecord->release(TRUE);
	}
	return error;
}

/*
// Free a warp field record.
*/

ERRORCODE PMGDatabase::free_warp_field_record(DB_RECORD_NUMBER l_record)
{
	WarpFieldRecord* pRecord;
	DocumentPtr document;
	ERRORCODE error;

	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the warp field so we can access it.
*/

	if ((pRecord = (WarpFieldRecord*)get_record(l_record, &error, RECORD_TYPE_WarpField)) != NULL)
	{
		/*
		// Only free the data if the reference count goes to zero.
		*/

		if (pRecord->dec_reference_count() == 0)
		{
		/* Time to free the warp field. */

			pRecord->release();

		/* Remove us from the document's warp field list. */

			if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
			{
				document->remove_warp_field(l_record);
				document->release();

			/* Delete the record from the database. */

		      delete_record(l_record);
			}
		}
		else
		{
			pRecord->release(TRUE);
		}
	}
	return error;
}

/////////////////////////////////////////////////////////////////////////////
// Outline path record management.

/*
// Create a new outline path record.
*/

DB_RECORD_NUMBER PMGDatabase::new_outline_path_record(OutlinePathCreateStruct* pCreateStruct)
{
	DB_RECORD_NUMBER l_record;
	OutlinePathRecord* pRecord;
	DocumentPtr document;

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the outline path in the existing array. */

	if ((l_record = document->find_outline_path(pCreateStruct->m_pszName)) == 0)
	{
	/*
	//	Create a new OutlinePathRecord data structure.
	*/

	   if ((pRecord = create_outline_path(pCreateStruct)) != NULL)
		{
		/* Get the record number. */

			l_record = pRecord->Id();

		/* Insert the outline path number into the document outline path array. */

			document->add_outline_path(l_record);
		}
	}
	else
	{
		/*
		//	Get the record so we can lock it.
		*/

		pRecord = (OutlinePathRecord*)get_record(l_record, NULL, RECORD_TYPE_OutlinePath);
	}

/* Release the document. */

	document->release();

	if (pRecord == NULL)
	{
		return 0;
	}

/* Update the reference count. */

	pRecord->inc_reference_count();

/* We modified it. */
/* We're done with it. */

	pRecord->release(TRUE);

	return l_record;
}

/*
// Increment the reference count for an outline path record.
*/

ERRORCODE PMGDatabase::inc_outline_path_record(DB_RECORD_NUMBER l_record)
{
	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

	OutlinePathRecord* pRecord;
	ERRORCODE error;

	if ((pRecord = (OutlinePathRecord*)get_record(l_record, &error, RECORD_TYPE_OutlinePath)) != NULL)
	{
		pRecord->inc_reference_count();
		pRecord->release(TRUE);
	}
	return error;
}

/*
// Free an outline path record.
*/

ERRORCODE PMGDatabase::free_outline_path_record(DB_RECORD_NUMBER l_record)
{
	OutlinePathRecord* pRecord;
	DocumentPtr document;
	ERRORCODE error;

	if (l_record <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the outline path so we can access it.
*/

	if ((pRecord = (OutlinePathRecord*)get_record(l_record, &error, RECORD_TYPE_OutlinePath)) != NULL)
	{
		/*
		// Only free the data if the reference count goes to zero.
		*/

		if (pRecord->dec_reference_count() == 0)
		{
		/* Time to free the record. */

			pRecord->release();

		/* Remove us from the document's list. */

			if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
			{
				document->remove_outline_path(l_record);
				document->release();

			/* Delete the record from the database. */

		      delete_record(l_record);
			}
		}
		else
		{
			pRecord->release(TRUE);
		}
	}
	return error;
}

/////////////////////////////////////////////////////////////////////////////
// Font record management.

/*
// Create a new font record.
*/

DB_RECORD_NUMBER PMGDatabase::new_font_record(LPSTR name, BOOL temp, LPERRORCODE error)
{
	DB_RECORD_NUMBER f_record;
	FontDataPtr font_data;
	DocumentPtr document;
	ERRORCODE _error;

	if (error == NULL)
	{
		error = &_error;
	}

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, error, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the font in the existing array. */

	f_record = document->find_font_data(name);

/* Release the document. */

	document->release();

/* Did we find it? */

	if (f_record == 0)
	{
	/*
	//	Create a new FontDataRecord data structure.
	*/

	   if ((font_data = create_font_data(name)) == NULL)
		{
			*error = last_creation_error();
	      return 0;
		}

	/* Get the record number. */

		f_record = font_data->Id();

	/* Insert the graphic number into the document font data array. */

		document->add_font_data(f_record);

		font_data->release();
	}

/* Increment the reference count. */

	inc_font_record(f_record, temp);

	return f_record;
}

/*
// Free a font data record.
*/

ERRORCODE PMGDatabase::free_font_record(DB_RECORD_NUMBER f_record, BOOL temp)
{
	FontDataPtr fontdata;
	DocumentPtr document;
	ERRORCODE error;

	if (f_record <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the font data so we can access it.
*/

	if ((fontdata = (FontDataPtr)get_record(f_record, &error, RECORD_TYPE_FontData)) != NULL)
	{
	/*
	// Only free the data if the reference count goes to zero.
	*/
		ULONG count = fontdata->dec(temp);

		fontdata->release();

		if (count == 0)
		{
		/* Time to free the font data. */

		/* Remove us from the document's font data list. */

			if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
			{
				document->remove_font_data(f_record);
				document->release();

			/* Delete the record from the database. */

		      delete_record(f_record);
			}
		}
	}
	return error;
}

/*
// Increment the reference count for a font data record.
*/

ERRORCODE PMGDatabase::inc_font_record(DB_RECORD_NUMBER f_record, BOOL temp)
{
	if (f_record <= 0)
	{
		return ERRORCODE_None;
	}

	FontDataPtr fontdata;
	ERRORCODE error;

	if ((fontdata = (FontDataPtr)get_record(f_record, &error, RECORD_TYPE_FontData)) != NULL)
	{
		fontdata->inc(temp);
		fontdata->release();
	}
	return error;
}

ERRORCODE PMGDatabase::font_data_name(DB_RECORD_NUMBER f_record, LPSTR name)
{
	name[0] = '\0';

	if (f_record == 0)
	{
		return ERRORCODE_BadParameter;
	}

	FontDataPtr fontdata;
	ERRORCODE error;

	if ((fontdata = (FontDataPtr)get_record(f_record, &error, RECORD_TYPE_FontData)) != NULL)
	{
		strcpy(name, fontdata->get_face_name());
		fontdata->release();
	}
	return error;
}
/////////////////////////////////////////////////////////////////////////////
// Hyperlink record management.
DB_RECORD_NUMBER 
PMGDatabase::NewHyperlinkRecord(HyperlinkData* p, BOOL temp /*FALSE*/)
{
	TRACE("New hyperlink record: %s {%d} (%08lx)", (LPCSTR)p->FilePathOrURL, temp, this);
	DB_RECORD_NUMBER  recid = 0;
	HyperlinkRecord   *pHyperlink;
	DocumentPtr       document;

// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.

	if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &m_last_error, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

	//	Create a new Hyperlink
	// It will initialize itself.

	if ((pHyperlink = create_Hyperlink_record()) != NULL)
	{
	// Get the record number. 

		recid = pHyperlink->Id(); 

	// Insert the Hyperlink number into the document Hyperlink array. 

		document->AddHyperlink(recid);
      pHyperlink->SetData(p);
      
      pHyperlink->release();
	}

// Release the document.

	document->release();
   
   IncrementHyperlinkRecord(recid, temp);

	TRACE(" -> %ld\n", recid);
   
   return recid;
}

ERRORCODE 
PMGDatabase::IncrementHyperlinkRecord(DB_RECORD_NUMBER h_record, BOOL temp/*FALSE*/)
{
	if (h_record <= 0)
	{
		return ERRORCODE_None;
	}

	TRACE("IncrementHyperlinkRecord: %ld {%d} (%08lx)", h_record, temp, this);

	HyperlinkRecord   *phyperdata;
	ERRORCODE error;

	if ((phyperdata = (HyperlinkRecord*)get_record(h_record, &error, RECORD_TYPE_Hyperlink)) != NULL)
	{
		phyperdata->inc(temp);
		phyperdata->release();
	}
	TRACE("\n");
	return error;
}

ERRORCODE 
PMGDatabase::FreeHyperlinkRecord(DB_RECORD_NUMBER h_record, BOOL temp)
{
	HyperlinkRecord   *phyperdata;
   DocumentPtr       document;
	ERRORCODE         error;

	if (h_record <= 0)
	{
		return ERRORCODE_None;
	}

	TRACE("FreeHyperlinkRecord: %ld {%d} (%08lx)", h_record, temp, this);

	if ((phyperdata = (HyperlinkRecord*)get_record(h_record, &error, RECORD_TYPE_Hyperlink)) != NULL)
	{
	   // Only free the data if the reference count goes to zero.
		ULONG count = phyperdata->dec(temp);

		phyperdata->release();
		
      if (count == 0)
		{
	      // Remove us from the document's Hyperlink list.
	      if ((document = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
	      {
		      ULONG count = phyperdata->dec(temp);

		      document->RemoveHyperlink(h_record);
		      document->release();
	         // Delete the record from the database.
				TRACE(" [Delete]");
		      delete_record(h_record);
	      }
      }
   }
	TRACE("\n");
	
   return error;
}

BOOL
PMGDatabase::GetHyperlinkData(DB_RECORD_NUMBER RecordNumber, HyperlinkData &Data)
{
   HyperlinkRecord   *pRec;
   if ((pRec = (HyperlinkRecord*)get_record(RecordNumber, NULL, RECORD_TYPE_Hyperlink)) != NULL)
   {
      pRec->GetData(&Data);
      pRec->release();
      return TRUE;
   }

   return FALSE;
}

BOOL
PMGDatabase::SetHyperlinkData(DB_RECORD_NUMBER RecordNumber, HyperlinkData &Data)
{
   HyperlinkRecord   *pRec;
   if ((pRec = (HyperlinkRecord*)get_record(RecordNumber, NULL, RECORD_TYPE_Hyperlink)) != NULL)
   {
      pRec->SetData(&Data);
      pRec->release();
      return TRUE;
   }

   return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// Text Style record management.

/*
// Create a new text style record.
*/

DB_RECORD_NUMBER PMGDatabase::NewTextStyleRecord(LPCSTR pszName, int nType, CTextStyle* pStyle /*=NULL*/)
{
	DB_RECORD_NUMBER lRecord;
	CTextStyleRecord* pRecord;
	DocumentPtr pDocument;
	ERRORCODE* pError = NULL;
	ERRORCODE error;

	/* Legacy code. Remove if we definitely don't want to return the error. */
	if (pError == NULL)
	{
		pError = &error;
	}

/*
// Try to lock the document so we can access it.
// This should always work, but we'll check for error anyway.
*/

	if ((pDocument = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, pError, RECORD_TYPE_Document)) == NULL)
	{
		return 0;
	}

/* Try to find the text style in the existing array. */

	lRecord = pDocument->FindTextStyle(pszName, nType);

/* Did we find it? */

	if (lRecord == 0)
	{
	/*
	//	Create a new CTextStyleRecord data structure.
	*/

		CTextStyleRecord::CCreationData Data;

		Data.m_pszName = pszName;
		Data.m_wType = (WORD)nType;
		Data.m_pStyle = pStyle;
		
	   if ((pRecord = (CTextStyleRecord*)new_record(RECORD_TYPE_TextStyle, &Data)) == NULL)
		{
			*pError = last_creation_error();

		/* Release the document. */
			pDocument->release();

	      return 0;
		}

	/* Get the record number. */

		lRecord = pRecord->Id();

	/* Insert the text style number into the document text style array. */

		pDocument->AddTextStyle(lRecord);

		pRecord->release();
	}

/* Release the document. */

	pDocument->release();

/* Increment the reference count. */

	IncTextStyleRecord(lRecord);

	return lRecord;
}

/*
// Free a text style record.
*/

ERRORCODE PMGDatabase::FreeTextStyleRecord(DB_RECORD_NUMBER lRecord)
{
	CTextStyleRecord* pRecord;
	ERRORCODE error;

	if (lRecord <= 0)
	{
		return ERRORCODE_None;
	}

/*
// Try to lock the text style so we can access it.
*/

	if ((pRecord = (CTextStyleRecord*)get_record(lRecord, &error, RECORD_TYPE_TextStyle)) != NULL)
	{
	/*
	// Just decrement the reference count. These guys are only deleted by the end user.
	*/
		pRecord->Decrement();
		pRecord->release();
	}
	return error;
}

/*
// Increment the reference count for a text style record.
*/

ERRORCODE PMGDatabase::IncTextStyleRecord(DB_RECORD_NUMBER lRecord)
{
	if (lRecord <= 0)
	{
		return ERRORCODE_None;
	}

	CTextStyleRecord* pRecord;
	ERRORCODE error;

	if ((pRecord = (CTextStyleRecord*)get_record(lRecord, &error, RECORD_TYPE_TextStyle)) != NULL)
	{
		pRecord->Increment();
		pRecord->release();
	}
	return error;
}

ERRORCODE PMGDatabase::TextStyleName(DB_RECORD_NUMBER lRecord, LPSTR pName)
{
	pName[0] = '\0';

	if (lRecord == 0)
	{
		return ERRORCODE_BadParameter;
	}

	CTextStyleRecord* pRecord;
	ERRORCODE error;

	if ((pRecord = (CTextStyleRecord*)get_record(lRecord, &error, RECORD_TYPE_TextStyle)) != NULL)
	{
		strcpy(pName, pRecord->Name());
		pRecord->release();
	}
	return error;
}

DB_RECORD_NUMBER PMGDatabase::ImportTextStyleRecord(DB_RECORD_NUMBER lBaseStyle, PMGDatabase* pSourceDatabase)
{
	if (pSourceDatabase == this)
	{
		// Databases are the same. Increment the style.
		IncTextStyleRecord(lBaseStyle);
	}
	else
	{
		// Database to database transfer.
		// We need to transfer the text style by name, type, etc.

		CTextStyleRecord* pStyleRecord = (CTextStyleRecord*)pSourceDatabase->get_record(lBaseStyle, NULL, RECORD_TYPE_TextStyle);
		if (pStyleRecord == NULL)
		{
			ASSERT(lBaseStyle == 0);
			// Error recovery?
			lBaseStyle = 0;
		}
		else
		{
			lBaseStyle = NewTextStyleRecord(pStyleRecord->Name(),
													  pStyleRecord->Type(),
													  &pStyleRecord->Style());
			pStyleRecord->release();
		}
	}
	return lBaseStyle;
}

/////////////////////////////////////////////////////////////////////////////
// Story record management.

/*
// Create a new story record.
*/

CTextRecord* PMGDatabase::NewStory(void)
{
	CTextRecord* pRecord;

   if ((pRecord = (CTextRecord*)new_record(RECORD_TYPE_Text, NULL)) != NULL)
	{
		AddStory(pRecord->Id(), FALSE);
	}
	return pRecord;
}

/*
// Delete a story.
*/

void PMGDatabase::DeleteStory(DB_RECORD_NUMBER lStory)
{
	if (lStory > 0)
	{
		RemoveStory(lStory);
		delete_record(lStory, RECORD_TYPE_Text);
	}
}

/*
// Add a story to the story list.
*/

void PMGDatabase::AddStory(DB_RECORD_NUMBER lStory, BOOL fCheckExistence /*=TRUE*/)
{
	/*
	// Try to lock the document so we can access it.
	// This should always work, but we'll check for error anyway.
	*/
	DocumentPtr pDocument;

	if ((pDocument = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document)) != NULL)
	{
		if (!fCheckExistence || pDocument->FindStory(lStory) == (DWORD)-1)
		{
			pDocument->AddStory(lStory);
		}
		pDocument->release();
	}
}

/*
// Remove a story from the story list.
*/

void PMGDatabase::RemoveStory(DB_RECORD_NUMBER lStory)
{
	/*
	// Try to lock the document so we can access it.
	// This should always work, but we'll check for error anyway.
	*/

	DocumentPtr pDocument;

	if ((pDocument = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document)) != NULL)
	{
		pDocument->RemoveStory(lStory);
		pDocument->release();
	}
}

/*
// Increment the reference count for a component record.
*/

ERRORCODE PMGDatabase::inc_component_record(DB_RECORD_NUMBER recno)
{
	ERRORCODE error = ERRORCODE_None;

	if (recno)
	{
		ComponentRecord* component = (ComponentRecord*) get_record( recno, &error, RECORD_TYPE_Component );

		if (component)
		{
			component->inc_record();
			component->release( TRUE );
		}
	}

	return error;
}

/*
// Free a component record.
*/

ERRORCODE PMGDatabase::free_component_record(DB_RECORD_NUMBER recno)
{
	ERRORCODE error = ERRORCODE_None;

	if (recno)
	{
		ComponentRecord* component = (ComponentRecord*) get_record( recno, &error, RECORD_TYPE_Component );

		if (component)
		{
			int nRefCnt = component->free_record();
			component->release( TRUE );

			if (nRefCnt == 0)
			{
				/* Delete the record from the database. */
				delete_record( recno );
			}

		}
	}

	return error;
}

/////////////////////////////////////////////////////////////////////////////

/*
// Delete a paragraph record.
// This will account for all font references in the paragraph's style and
// style changes.
*/

VOID PMGDatabase::delete_paragraph(DB_RECORD_NUMBER p_record)
{
	ParagraphPtr paragraph;
	ERRORCODE error;

	if ((paragraph = (ParagraphPtr)get_record(p_record, &error, RECORD_TYPE_Paragraph)) != NULL)
	{
	/* First, handle all style changes. */

		SHORT change_count = paragraph->number_of_changes();

		for (SHORT i = 0; i < change_count; i++)
		{
			STYLE_CHANGE_PTR scp = paragraph->get_style_change(i);

			if (scp->token == TOKEN_Face)
			{
			/* We need to remove this reference. */
				free_font_record((DB_RECORD_NUMBER)scp->new_value, FALSE);
			}
		}

	/*
 	// Handle the font reference in the style.
	// The style in the paragraph is not temporary. That means that the
	// style must be explicitly destroyed.
 	*/

		paragraph->get_style().destroy();

	/* Release the paragraph in preparation for the delete. */

		paragraph->release();

	/* Finally, delete the record. */

		delete_record(p_record, RECORD_TYPE_Paragraph);
	}
//	return error;				// Someday!
}

/*
// Return the last offset in a paragraph.
*/

C_INDEX PMGDatabase::last_offset_in_paragraph(DB_RECORD_NUMBER p_record)
{
	ParagraphPtr paragraph;
	C_INDEX offset = -1;

	if ((paragraph = (ParagraphPtr)get_record(p_record, NULL, RECORD_TYPE_Paragraph)) != NULL)
	{
		offset = paragraph->number_of_characters();
		paragraph->release();
	}
	return offset;
}

/*
// Return the paragraph record of a frame.
// Returns 0 on error.
*/

DB_RECORD_NUMBER PMGDatabase::paragraph_of_frame(DB_RECORD_NUMBER f_record)
{
	FramePtr frame;
	DB_RECORD_NUMBER p_record = 0;

	if ((frame = (FramePtr)get_record(f_record, NULL, RECORD_TYPE_Frame)) != NULL)
	{
		p_record = frame->get_paragraph();
		frame->release();
	}
	return p_record;
}

/*
// Do a refresh notify.
*/

VOID PMGDatabase::do_refresh_notify(
	PBOX_PTR area,
	REFRESH_TYPE type,
	PMGPageObjectPtr object,
	LPRECT lprExtraPixels)
{
	if (refresh_notify != NULL)
	{
		refresh_notify(refresh_data, area, type, object, lprExtraPixels);
	}
}

/*
// Add a refresh for an object.
*/

VOID PMGDatabase::refresh_object(PMGPageObjectPtr object)
{
	PBOX bound;

	object->get_refresh_bound(&bound);

	do_refresh_notify(&bound, REFRESH_REDRAW, NULL);
}

/*
// Delete an object, noting its disappearance.
*/

VOID PMGDatabase::delete_object(PMGPageObjectPtr object)
{
	refresh_object(object);

	object->destroy();

	delete object;
}

/*
// Load a string from the resource file (assumed).
*/

LPSTR PMGDatabase::load_string(DWORD which)
{
	return load_string_handler == NULL
				? NULL
				: load_string_handler(load_string_data, which);
}

/*
// Say the document has been modified.
*/

VOID PMGDatabase::document_modified()
{
	if (document_modified_handler != NULL)
	{
		document_modified_handler(document_modified_data);
	}
}

/*
// Toggle an object.
*/

VOID PMGDatabase::toggle_object(PMGPageObjectPtr object, SHORT which)
{
	if (object_toggle_handler != NULL)
	{
		object_toggle_handler(object_toggle_data, object, which);
	}
}

/*
// Macro list support.
*/

MacroListRecord* PMGDatabase::GetMacroList(void)
{
	ERRORCODE error;

/*
// Defer to the macro server's record if it has one.
*/

	if (m_pMacroServer != NULL)
	{
		MacroListRecord* pMacroList = m_pMacroServer->GetMacroList();
		if (pMacroList != NULL)
		{
		/*
		// Increment the lock count and return us.
		// Note that the database may not be us!
		*/
			return (MacroListRecord*)pMacroList->GetDatabase()->get_record(pMacroList->Id(), NULL);
		}
	}

/*
// Failsafe: look in the document.
*/

	DocumentPtr pDocument;

	if ((pDocument = (DocumentPtr)get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) != NULL)
	{
		DB_RECORD_NUMBER mRecord = pDocument->get_macro_list_record();
		pDocument->release();

		if (mRecord != 0)
		{
			return (MacroListRecord*)get_record(mRecord, NULL, RECORD_TYPE_MacroList);
		}
	}
	return NULL;
}

/*
// Find a frame object given an object list and a frame record number.
*/

CFrameObject* PMGDatabase::FindFrameObject(DB_RECORD_NUMBER lFrame, ObjectListPtr pList)
{
	// Look though this list for the frame object.
	for (PageObject* pObject = pList->first_object();
		  pObject != NULL;
		  pObject = pObject->next_object())
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Frame:
			case OBJECT_TYPE_CalendarDate:
			{
				CFrameObject* pFrameObject = (CFrameObject*)pObject;
				if (pFrameObject->FrameRecord() == lFrame)
				{
					// Found it!
					return pFrameObject;
				}
				break;
			}
			case OBJECT_TYPE_Group:
			case OBJECT_TYPE_Calendar:
			case OBJECT_TYPE_Table:
			case OBJECT_TYPE_Cell:
			{
				CFrameObject* pFrameObject = FindFrameObject(lFrame, ((GroupObject*)pObject)->object_list());
				if (pFrameObject != NULL)
				{
					// Found it!
					return pFrameObject;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
		  
	return NULL;
}

/*
// Find a frame object given a frame.
// This will change the current page.
*/

CFrameObject* PMGDatabase::FindFrameObject(DB_RECORD_NUMBER lFrame)
{
	CFrameObject* pObject = NULL;
	ERRORCODE error;

	CFrameRecord* pFrame = (CFrameRecord*)get_record(lFrame, &error, RECORD_TYPE_Frame);
	if (pFrame == NULL)
	{
		// Unable to lock the passed frame (see 'error' for the reason why).
		ASSERT(FALSE);
	}
	else
	{
		DB_RECORD_NUMBER lPage = pFrame->Page();
		if (lPage > 0)
		{
			DocumentRecord* pDocument = (DocumentRecord*)get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document);
			if (pDocument != NULL)
			{
				ObjectList* pList = pDocument->load_page(lPage);
				if (pList != NULL)
				{
					pObject = FindFrameObject(lFrame, pList);
				}
				pDocument->release();
			}
		}
		pFrame->release();
	}
	return pObject;
}

/*
// Map the records of the frames that were just copied.
*/

void PMGDatabase::MapCopiedFrames(PMGPageObject* pOldObject, PMGPageObject* pNewObject, CMapPtrToPtr& Map)
{
	if (pOldObject->type() == pNewObject->type())
	{
		switch (pOldObject->type())
		{
			case OBJECT_TYPE_Frame:
			{
				Map.SetAt((void*)((CFrameObject*)pOldObject)->FrameRecord(), (void*)((CFrameObject*)pNewObject)->FrameRecord());
				break;
			}
			case OBJECT_TYPE_Group:
			{
				pOldObject = (PMGPageObject*)(((GroupObject*)pOldObject)->object_list()->first_object());
				pNewObject = (PMGPageObject*)(((GroupObject*)pNewObject)->object_list()->first_object());
				while (pOldObject != NULL)
				{
					if (pNewObject == NULL)
					{
						// Object lists do not match!
						ASSERT(FALSE);
						break;
					}
					MapCopiedFrames(pOldObject, pNewObject, Map);
					pOldObject = (PMGPageObject*)pOldObject->next_object();
					pNewObject = (PMGPageObject*)pNewObject->next_object();
				}
				break;
			}
			default:
			{
				// Other type of object. Nothing to do.
				break;
			}
		}
	}
	else
	{
		// Corresponding object type is not same!
		ASSERT(FALSE);
	}
}

/*
// Re-link frames after a paste or copy.
*/

void PMGDatabase::LinkCopiedFrames(PMGDatabase* pOldDatabase, CMapPtrToPtr& Map, BOOL fFlowText)
{
	DB_RECORD_NUMBER lLastFrame = 0;

	while (Map.GetCount() != 0)
	{
		POSITION Pos = Map.GetStartPosition();
		if (Pos != NULL)
		{
			void* Key;
			void* Value;
			Map.GetNextAssoc(Pos, Key, Value);

			DB_RECORD_NUMBER lOldFrame = (DB_RECORD_NUMBER)Key;
			DB_RECORD_NUMBER lNewFrame = (DB_RECORD_NUMBER)Value;
			if (lOldFrame == lLastFrame)
			{
				// He did not go away!
				// We check this to keep from looping forever.
				ASSERT(FALSE);
				Map.RemoveKey(Key);
			}
			else
			{
				lLastFrame = lOldFrame;
				// Get the text record for this frame.
				// We will then march through all the frames and relink them.
				CFrameRecord* pFrame = (CFrameRecord*)pOldDatabase->get_record(lOldFrame, NULL, RECORD_TYPE_Frame);
				if (pFrame == NULL)
				{
					Map.RemoveKey(Key);
				}
				else
				{
					// Lock the text record.
					CTextRecord* pOldText = pFrame->LockTextRecord();
					pFrame->release();

					// If we have the text record, then we can begin.
					if (pOldText != NULL)
					{
						CFrameRecord* pOldFrame = NULL;

						CTextRecord* pNewText = NULL;
						CFrameRecord* pLastNewFrame = NULL;
						CFrameRecord* pNewFrame = NULL;

						BOOL fCopyText = TRUE;
						DB_RECORD_NUMBER lOldLink = pOldText->FirstFrame();

						ERRORCODE error;

						TRY
						{
							while (lOldLink != 0)
							{
								// Lock the frame whether it is in the map or not.
								ASSERT(pOldFrame == NULL);
								if ((pOldFrame = (CFrameRecord*)pOldDatabase->get_record(lOldLink, &error, RECORD_TYPE_Frame)) == NULL)
								{
									ThrowErrorcodeException(error);
								}

								// See if this frame is in the map.
								void* pValue;
								ASSERT(pOldFrame->Id() == lOldLink);

								if (Map.Lookup((void*)lOldLink, pValue))
								{
									Map.RemoveKey((void*)lOldLink);
									// This frame is in the map. It needs to get hooked
									// up.
									if ((pNewFrame = (CFrameRecord*)get_record((DB_RECORD_NUMBER)pValue, &error, RECORD_TYPE_Frame)) == NULL)
									{
										ThrowErrorcodeException(error);
									}

									// Lock the text record if we have not yet.
									if (pNewText == NULL)
									{
										// Lock the text record now.
										ASSERT(pLastNewFrame == NULL);
										if ((pNewText = pNewFrame->LockTextRecord(&error)) == NULL)
										{
											ThrowErrorcodeException(error);
										}
									}
									else
									{
										// Validate!
										ASSERT(pLastNewFrame != NULL);
									}

									// Do the link.

									if (pLastNewFrame == NULL)
									{
										// We should already be linked! This is our
										// text record, after all.
										ASSERT(pNewText->FirstFrame() == pNewFrame->Id());
										ASSERT(pNewFrame->PreviousFrame() == 0);
										ASSERT(pNewFrame->NextFrame() == 0);
									}
									else
									{
										// Link us after the last frame.
										// We need to get rid of our text.
										pNewFrame->DeleteTextRecord();

										// We become part of this text.
										pNewFrame->AttachTextRecord(pNewText->Id());

										// Do the link.
										pNewFrame->PreviousFrame(pLastNewFrame->Id());
										pLastNewFrame->NextFrame(pNewFrame->Id());
										ASSERT(pNewFrame->NextFrame() == 0);

										// Get rid of the last frame. Last is now us.
										pLastNewFrame->release(TRUE);
									}

									// For next time.
									pLastNewFrame = pNewFrame;
									pNewFrame = NULL;
								}
								else
								{
									fCopyText = FALSE;
								}

								// Advance to the next frame.
								lOldLink = pOldFrame->NextFrame();
								pOldFrame->release();
								pOldFrame = NULL;
							}
						}
						CATCH_ALL(e)
						{
							fCopyText = FALSE;
						}
						END_CATCH_ALL

						// Finished copying frames.
						// Release any remaining frame records.
						if (pOldFrame != NULL)
						{
							pOldFrame->release();
						}
						if (pNewFrame != NULL)
						{
							pNewFrame->release(TRUE);
						}

						// Handle the text record.
						if (pNewText != NULL)			// We locked the text...
						{
							if (pNewText->FirstFrame() != 0)
							{
								// If all frames were copied and we had more than
								// one frame, we need to copy the source text over.
								ASSERT(pLastNewFrame != NULL);
								if (fCopyText && pNewText->FirstFrame() != pLastNewFrame->Id())
								{
									// All frames were copied. Duplication did not
									// copy the original text. Copy it over now.
									pNewText->assign(*pOldText);
								}

								if (fFlowText)
								{
									// Flow text now to update everything.
									CTextFlow TextFlow(pNewText->FirstFrame(),
															 this,
															 0, -1);
									TextFlow.FlowText();
								}
							}
							else
							{
								// First frame of text record is zero!
								ASSERT(FALSE);
							}
							pNewText->release(TRUE);
						}

						if (pLastNewFrame != NULL)
						{
							pLastNewFrame->release(TRUE);
						}
						pOldText->release();
					}
				}
			}
		}
	}
}

void PMGDatabase::CopyObjectList(ObjectList* pDestList, ObjectList* pSourceList, DB_RECORD_NUMBER lPage /*=0*/)
{
	CMapPtrToPtr Map;

	pDestList->SetPageProperties(pSourceList->GetPageProperties());	
	pDestList->CopyGuides(pSourceList);

	for (PMGPageObject* pObject = (PMGPageObject*)pSourceList->first_object();
			pObject != NULL;
			pObject = (PMGPageObject*)pObject->next_object())
	{
		PMGPageObject* pNewObject;

		if ((pNewObject = (PMGPageObject*)pObject->duplicate()) != NULL)
		{
			MapCopiedFrames(pObject, pNewObject, Map);
			pDestList->append(pNewObject);

			// Set the page if one is passed in.
			if (lPage != 0)
			{
				pNewObject->Page(lPage);
			}
			pNewObject->calc();
		}
	}

	// Relink the copied frames.
	LinkCopiedFrames(this, Map, TRUE);
}

/*
// General initialization. Where should this go?
*/

BOOL init_document_engine(GetFontCallback pCallback, LPCSTR pszDllPath)
{
/*
// Create the system heap used by the database routines.
// Create 'geronimo' for the outline graphics code.
// Create the TrueType engine for the font server.
*/

	return InitializeFontEngine(pCallback, pszDllPath);
}

/*
// General de-initialization. Where should this go?
*/

VOID exit_document_engine()
{
	DeinitializeFontEngine();
}

