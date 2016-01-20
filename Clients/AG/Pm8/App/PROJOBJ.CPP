/*
// $Workfile: PROJOBJ.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROJOBJ.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     7/28/98 7:36p Jayn
// Fixed crashing bug and "dead" printer if cancelled out
// 
//    Rev 1.0   14 Aug 1997 15:25:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:10   Fred
// Initial revision.
// 
//    Rev 1.4   24 Jul 1997 17:35:38   Jay
// Fix to allow file-handle-limited 16-bit version to work.
// 
//    Rev 1.3   24 Jul 1997 14:37:42   Jay
// Print project and art improvements (fixes)
// 
//    Rev 1.2   08 Jul 1997 12:09:10   Jay
// Wasn't closing the document when done.
// 
//    Rev 1.1   05 Jun 1997 17:11:38   doberthur
// 
// 
// 
//    Rev 1.0   30 May 1997 18:34:48   doberthur
// Initial revision.
// 
*/


#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmgobj.h"
#include "utils.h"
#include "cntritem.h"
#include "util.h"
#include "projobj.h"

extern CPalette* pOurPal;

/*****************************************************************************/
/*                                ProjectObject                                  */
/*****************************************************************************/

/*
// The creator for an project object.
*/

ERRORCODE ProjectObject::create( VOIDPTR creation_data, CPmwDoc *pDoc)
{
	if (creation_data != NULL)
	{
		PROJECTOBJECT_CREATE_STRUCT* ocs = (PROJECTOBJECT_CREATE_STRUCT*)creation_data;

	   set_bound(ocs->Bound);

      m_file_name = ocs->file_name;
      m_friendly_name = ocs->friendly_name;

		if (!OpenDocument())
		{
			return ERRORCODE_Open;
		}

		m_pItem->GetPreviewBound(&m_Bound);

#ifndef WIN32
		CloseDocument();
#endif

	}
	return ERRORCODE_None;
}

BOOL ProjectObject::OpenDocument(void)
{
	if (m_pItem == NULL)
	{
		// No document yet.
      TRACE("ProjObj: Open document '%s'\n", (LPCSTR)m_file_name);
		if (GetGlobalPathManager()->GetPathBindingType(m_file_name) == PBT_CollectionItem)
		{
			// Need to copy doc to working dir in order to open.
			CString csDestPath;
			CString csName;
			CString csExtension;
			CString csFinalName;

			CString csPath =  m_file_name;

			AfxGetApp()->BeginWaitCursor();
			ReadOnlyFile Source(csPath);

			Util::SplitPath(csPath, NULL, NULL, &csName, &csExtension);
			// The destination is in the transfer directory.
			csDestPath = GET_PMWAPP()->GetTransferDirectory();
			Util::AppendBackslashToPath(csDestPath);
			csDestPath += csName;
			csDestPath += csExtension;

			// We have to copy the data out of the content database to its
			// own file in order to open it.
			StorageFile Dest(csDestPath);

			if (copy_file(&Source, &Dest) == ERRORCODE_None)
			{
				Dest.flush();
				csFinalName = GetGlobalPathManager()->ExpandPath(Dest.get_name());
				m_pItem = (CPmwDoc*)GET_PMWAPP()->OpenHiddenDocument(csFinalName);
			}

			Dest.flush();
			Dest.zap();

			AfxGetApp()->EndWaitCursor();
		}
		else
		{
			// Open the document with the original name.

			CString csPath = GetGlobalPathManager()->LocatePath(m_file_name);
			m_pItem = (CPmwDoc*)GET_PMWAPP()->OpenHiddenDocument(csPath);
		}
	}

	return (m_pItem != NULL);
}

void ProjectObject::CloseDocument(void)
{
	if (m_pItem != NULL)
	{
      TRACE("ProjObj: Close document '%s'\n", (LPCSTR)m_file_name);
		m_pItem->OnCloseDocument();
		m_pItem = NULL;
	}
}

/*
// The constructor
*/

ProjectObject::ProjectObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner)
{
	m_pItem = NULL;
}

ProjectObject::~ProjectObject()
{
	CloseDocument();
}

//
UpdateStatePtr ProjectObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, 
                                     UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	if (!OpenDocument())
	{
		// Just leave.
		return NULL;
	}

   struct REFRESH_EXTENT re;

   re.refresh_type = refresh_type;
   re.my_object = NULL;
   re.update_object = NULL;
   re.update_state = ustate;
   re.extent = *extent; 

   RedisplayContext NewRC = *rc;

   // Compute destination rect for this object in 'r'.
   PBOX Bound = get_bound();
   PBOX b = Bound;
   rc->pbox_to_screen(&b, TRUE);
   RECT r;
   rc->convert_pbox(&b, &r, NULL);


   NewRC.destination_rect = r;
   NewRC.destination_x0 = r.left;
   NewRC.destination_y0 = r.top;
   NewRC.clip_rect = r;

	m_pItem->GetPreviewBound(&NewRC.source_pbox);
   re.extent = NewRC.source_pbox;
   
   PCOORD SDX = NewRC.source_pbox.x1 - NewRC.source_pbox.x0;
   PCOORD DDX = Bound.x1 - Bound.x0;

   while (SDX > 16384 || DDX > 16384)
   {
      SDX >>= 1;
      DDX >>= 1;
   }

   if (SDX == 0) SDX++;
   if (DDX == 0) DDX++;

   NewRC.SetScaling((USHORT)DDX, (USHORT)SDX);
   NewRC.scaled_source_x0 = NewRC.source_pbox.x0 * NewRC.x_resolution;
   NewRC.scaled_source_y0 = NewRC.source_pbox.y0 * NewRC.y_resolution;

   m_pItem->DrawPreview(&re, &NewRC);

#ifndef WIN32
	CloseDocument();
#endif

   return NULL;
}

 
/*
// Recalc after a size or movement.
*/

void ProjectObject::calc(PBOX_PTR panel_world /*=NULL*/, FLAGS panel_flags /*=0*/)
{
	RectPageObject::calc(panel_world, panel_flags);
}


/*
// Return the original dimensions of this object.
*/

BOOL ProjectObject::original_dims(PPNT_PTR p)
{
   p->x = m_Bound.x1 - m_Bound.x0;
   p->y = m_Bound.y1 - m_Bound.y0;
	return TRUE;
}

BOOL ProjectObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
	strncpy(buffer, m_friendly_name, size-1);

/* Make sure it's terminated. */
	buffer[size-1] = '\0';
	return TRUE;
}

