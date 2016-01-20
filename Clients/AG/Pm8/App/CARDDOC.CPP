/*
// $Workfile: CARDDOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
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
// $Log: /PM8/App/CARDDOC.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 23    2/01/99 2:02p Johno
// Backgrounds now flip
// 
// 22    2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 21    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 20    1/27/99 1:48p Johno
// Compile update
// 
// 19    1/14/99 3:25p Johno
// No more m_fNormalHTML
// 
// 18    1/04/99 3:30p Johno
// Constuctor sets m_fNormalHTML to FALSE
// 
// 17    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 16    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 15    8/05/98 12:01p Mwilson
// removed include of filcmprs.h.  It was causing an error.
// 
// 14    7/07/98 3:24p Rlovejoy
// Made copyright work on notecards.
// 
// 13    6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 12    6/18/98 4:05p Mwilson
// added new project info for remote printing of half fold cards as
// quarter fold cards
// 
// 11    6/12/98 4:36p Mwilson
// moved handler for personal delivery from doc to view
// 
// 10    6/09/98 10:33a Mwilson
// updates to remote fulfillment
// 
// 9     6/08/98 4:08p Rlovejoy
// Accomodate various "copyright" placement types.
// 
// 8     6/01/98 1:25p Mwilson
// more personal devlivery stuff
// 
// 7     5/20/98 5:20p Mwilson
// added handler for remote fulfillment
// 
// 6     4/30/98 5:50p Fredf
// Tweaks to feature that places copyright stamp on back of card,
// 
// 5     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 4     4/24/98 3:10p Rlovejoy
// Added call to calc() for proper text display.
// 
// 3     4/23/98 4:00p Rlovejoy
// Added DoPostOpen() to perform copyright placement.
// 
// 2     4/18/98 4:24p Fredf
// T-Shirt printing.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "carddoc.h"
#include "cardview.h"
#include "action.h"
#include "paper.h"
#include "pmwcfg.h"
#include "utils.h"
#include "pmwcoll.h"
#include "util.h"
#include "grpobj.h"
#include "docdib.h"	// For CDIBInfo
#include "bkgobj.h"

#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define  FLIPPED  (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)

/*
// The card project panels.
*/

static PANEL_INFO
SideFoldPanels[] =
{
   // Front
   {
      {    0,    0,  500,  500 },
      {    0,    0, 1000, 1000 },
      FLIPPED,
      1
   },
   // Inside
   {
      {    0,  500, 1000, 1000 },
      {    0, 1000,    0,    0 },
      0,
      1
   },
   // Back
   {
      {  500,    0, 1000,  500 },
      { 1000,    0,    0, 1000 },
      FLIPPED,
      1
   }
},

TopFoldPanels[] =
{
   // Front
   {
      {    0,    0,  500,  500 },
      {    0,    0, 1000, 1000 },
      FLIPPED,
      1
   },
   // Inside
   {
      {  500,    0, 1000, 1000 },
      { 1000,    0,    0,    0 },
      0,
      1
   },
   // Back
   {
      {    0,  500,  500, 1000 },
      {    0, 1000, 1000,    0 },
      0,
      1
   }
},

SideHFasQFFoldPanels[] =
{
   // Front
   {
      {    0,    0,  500,  500 },
      {    0,    0, 0, 1000 },
      FLIPPED,
      1
   },
   // Inside
   {
      {    0,  500, 1000, 1000 },
      {    0, 1000,    0,    0 },
      0,
      1
   },
   // Back
   {
      {  500,    0, 1000,  500 },
      { 0,    0,    0, 1000 },
      FLIPPED,
      1
   }
},

TopHFasQFFoldPanels[] =
{
   // Front
   {
      {    0,    0,  500,  500 },
      {    0,    0, 1000, 1000 },
      FLIPPED,
      1
   },
   // Inside
   {
      {  500,    0, 1000, 1000 },
      { 1000,    0,    0,    0 },
      0,
      1
   },
   // Back
   {
      {    0,  500,  500, 1000 },
      {    0, 1000, 1000,    0 },
      0,
      1
   }
},

#if 1
TopHalfFoldPanels[] =
{
   // Front
   {
      {    0,    0, 1000,  500 },
      {    0,    0,    0,    0 },         // No margins
      FLIPPED,
      1,
      0           // Page index 0
   },
   // Inside
   {
      {    0,    0, 1000, 1000 },
      {    0,    0,    0,    0 },
      0,
      1,
      1           // Page index 1
   },
   // Back
   {
      {    0,  500, 1000, 1000 },
      {    0,    0,    0,    0 },         // No margins
      0,
      1,
      0           // Page index 0
   }
},

SideHalfFoldPanels[] =
{
   // Front
   {
      {  500,    0, 1000, 1000 },
      {    0,    0,    0,    0 },         // No margins
      0,
      1,
      0           // Page index 0
   },
   // Inside
   {
      {    0,    0, 1000, 1000 },
      {    0,    0,    0,    0 },
      0,
      1,
      1,          // Page index 1
   },
   // Back
   {
      {    0,    0,  500, 1000 },
      {    0,    0,    0,    0 },         // No margins
      0,
      1,
      0           // Page index 0
   }
};
#else
TopHalfFoldPanels[] =
{
   // Front
   {
      {    0,    0, 1000,  500 },
      {    0,    0,    0, 1000 },
      FLIPPED,
      1,
      0           // Page index 0
   },
   // Inside
   {
      {    0,    0, 1000, 1000 },
      {    0,    0,    0,    0 },
      0,
      1,
      1           // Page index 1
   },
   // Back
   {
      {    0,  500, 1000, 1000 },
      {    0, 1000,    0,    0 },
      0,
      1,
      0           // Page index 0
   }
},

SideHalfFoldPanels[] =
{
   // Front
   {
      {  500,    0, 1000, 1000 },
      { 1000,    0,    0,    0 },
      0,
      1,
      0           // Page index 0
   },
   // Inside
   {
      {    0,    0, 1000, 1000 },
      {    0,    0,    0,    0 },
      0,
      1,
      1,          // Page index 1
   },
   // Back
   {
      {    0,    0,  500, 1000 },
      {    0,    0, 1000,    0 },
      0,
      1,
      0           // Page index 0
   }
};
#endif

/*
// The side-fold card project info.
*/

ProjectInfo CCardDoc::m_SideFoldInfo =
{
   PROJECT_TYPE_Card,
   CARD_SideFold,
   PORTRAIT,
   sizeof(SideFoldPanels)/sizeof(SideFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   SideFoldPanels
};


/*
// The top-fold card project info.
*/

ProjectInfo CCardDoc::m_TopFoldInfo =
{
   PROJECT_TYPE_Card,
   CARD_TopFold,
   PORTRAIT,
   sizeof(TopFoldPanels)/sizeof(TopFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   TopFoldPanels
};

/////////////////////////////////////////////////////////////////////////////
// CCardDoc

IMPLEMENT_SERIAL(CCardDoc, CPmwDoc, 0 /* schema number*/ )

CCardDoc::CCardDoc()
{
   // Default to something.
   info = m_SideFoldInfo;

   current_panel = -2;
   memset(&panel_group, 0, sizeof(panel_group));

	for (int i = 0; i < 3; i ++)
		m_pPrintingBackground[i] = NULL;
}

CCardDoc::~CCardDoc()
{
	KillPrintingBackgrounds();
}

BEGIN_MESSAGE_MAP(CCardDoc, CPmwDoc)
   //{{AFX_MSG_MAP(CCardDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ProjectInfo* CCardDoc::SideFoldInfo(void)
{
	if(CPmwView::m_bPrintingHFasQF)
		m_SideFoldInfo.pip = SideHFasQFFoldPanels;
	else
		m_SideFoldInfo.pip = SideFoldPanels;

	return &m_SideFoldInfo;
}

ProjectInfo* CCardDoc::TopFoldInfo(void)
{
	if(CPmwView::m_bPrintingHFasQF)
		m_TopFoldInfo.pip = TopHFasQFFoldPanels;
	else
		m_TopFoldInfo.pip = TopFoldPanels;

	return &m_TopFoldInfo;
}

/*
// The "new" document handler for cards.
// This needs to create all the card groups (one for each panel).
*/

BOOL CCardDoc::OnNewDocument()
{
   ProjectInfo* pInfo;
   switch (GET_PMWAPP()->last_new_info.subtype)
   {
      case CARD_TopFold:
      {
         pInfo = TopFoldInfo();
         break;
      }
      case CARD_SideFold:
      {
         pInfo = SideFoldInfo();
         break;
      }
      default:
      {
         // Invalid card type.
         ASSERT(FALSE);
         return FALSE;
      }
   }
   info = *pInfo;

   // Do the standard new document.
   if (!CPmwDoc::OnNewDocument())
      return FALSE;

   // Create the panels.
   if (!CreatePanels())
   {
      return FALSE;
   }

   // Let the configuration have its way with our back panel.
   GetConfiguration()->SupportsCardBack(FALSE, this);

   return TRUE;
}

BOOL CCardDoc::CreatePanels(void)
{
   for (SHORT panel = 0; panel < info.number_of_panels; panel++)
   {
      GROUP_CREATE_STRUCT gcs;

      // Set the correct page for this panel.
      GotoPage(info.pip[panel].m_nPageIndex, FALSE);

      // Get the panel world for this panel.
      get_panel_world(&gcs.original_bound, panel);

      GroupObjectPtr object;

      if ((object = (GroupObjectPtr)get_database()->create_group_object(&gcs)) == NULL)
      {
         return FALSE;
      }

      // Set up the rest of the peculiars for this group.
      object->set_panel(panel);
      object->set_primary_action(ACTION_EDIT_PANEL);
      object->set_select_flags(SELECT_FLAG_boundary
                                 | SELECT_FLAG_solo_select);

      panel_group[panel] = object;

      append_object(object);
      object->calc();
   }
   return TRUE;
}

/*
// Remove the group bits from all root level objects under this group.
*/

void CCardDoc::RemoveGroupBits(GroupObject* pGroup)
{
   for (PageObject* pObject = pGroup->object_list()->first_object();
        pObject != NULL;
        pObject = pObject->next_object())
   {
      ((PMGPageObject*)pObject)->remove_flags(OBJECT_FLAG_grouped);
   }
}

/*
// Fix the panel values for the objects in this group.
// The panel to use is the panel of the group.
*/

void CCardDoc::FixPanelValues(GroupObject* pGroup)
{
   int nPanel = pGroup->get_panel();
   for (PageObject* pObject = pGroup->object_list()->first_object();
        pObject != NULL;
        pObject = pObject->next_object())
   {
      ((PMGPageObject*)pObject)->set_panel(nPanel);
      if (pObject->type() == OBJECT_TYPE_Group)
      {
         FixPanelValues((GroupObject*)pObject);
      }
   }
}

void CCardDoc::FlipProject(FLAGS Flags)
{
	int nPanels = number_of_panels();

	for (int nPanel = 0; nPanel < nPanels; nPanel++)
	{
		FlipCardPanel(nPanel, Flags);
	}
}

/*
// Flip all card panels which are normally flipped (via PANEL_INFO).
*/

void CCardDoc::FlipCardPanels(void)
{
   int nPanels = number_of_panels();

   for (int nPanel = 0; nPanel < nPanels; nPanel++)
   {
      FLAGS Flags = info.pip[nPanel].flags;
      if (Flags & FLIPPED)
      {
         FlipCardPanel(nPanel, Flags);
      }
   }
}

void CCardDoc::FlipCardPanel(int nPanel, FLAGS Flags)
{
   ObjectList* pObjects = panel_group[nPanel]->object_list();

   PMGPageObjectPtr pObject;
   PBOX world;

   get_panel_world(&world, nPanel);

   for (pObject = (PMGPageObjectPtr)pObjects->last_object();
               pObject != NULL;
               pObject = (PMGPageObjectPtr)pObject->previous_object())
   {
   /* To alternate position. */

      if (Flags & OBJECT_FLAG_xflipped)
      {
         pObject->xflip();
      }
      if (Flags & OBJECT_FLAG_yflipped)
      {
         pObject->yflip();
      }

      if (pObject->get_layout() <= 0)
      {
         PBOX bound = ((RectPageObjectPtr)pObject)->get_unrotated_bound();
         PCOORD tmp;

         if (Flags & OBJECT_FLAG_xflipped)
         {
            tmp = world.x1 - (bound.x0 - world.x0);
            bound.x0 = world.x1 - (bound.x1 - world.x0);
            bound.x1 = tmp;
         }

         if (Flags & OBJECT_FLAG_yflipped)
         {
            tmp = world.y1 - (bound.y0 - world.y0);
            bound.y0 = world.y1 - (bound.y1 - world.y0);
            bound.y1 = tmp;
         }

         ((RectPageObjectPtr)pObject)->set_unrotated_bound(bound);
      }
      calc_object(pObject);
   }
   panel_group[nPanel]->establish(FALSE);
   RemoveGroupBits(panel_group[nPanel]);
}

/*
// Set the panel for this document.
// This plugs in the appropriate object list.
// panel numbers:
//    -2 = saving or printing; panels need to match the PANEL_INFO.
//    -1 = full page view; panels are all unflipped.
//   >=0 = normal panels; panels are all unflipped.
*/

BOOL CCardDoc::set_panel(SHORT panel)
{
/* Make sure we're not already set. */

   if (current_panel == panel)
   {
      return FALSE;
   }

/* See if we need to unflip anyone. */

   BOOL fCurrentFlipped = (current_panel != -2);
   BOOL fNewFlipped = (panel != -2);

   if (fCurrentFlipped != fNewFlipped)
   {
      panel_flags = fNewFlipped ? FLIPPED : 0;
      FlipCardPanels();
   }
/*
// Make sure the group is established correctly.
// We establish now since we may have edited (changed) some objects which
// would have made them out of sync with the main panel group.
*/
   if (current_panel >= 0)
   {
      panel_group[current_panel]->establish(FALSE);
      RemoveGroupBits(panel_group[current_panel]);
      FixPanelValues(panel_group[current_panel]);
   }
/* Set the panel. */
   current_panel = panel;
/* Set the object list. */
   if (current_panel < 0)
   {
      page_objects = document->load_page(-1);
      set_portion(NULL);
   }
   else
   {
      PBOX world;
   /* View just this panel. */
      GotoPage(info.pip[current_panel].m_nPageIndex, FALSE);
      page_objects = panel_group[current_panel]->object_list();

      get_panel_world(&world);
      set_portion(&world);
   }

   return TRUE;
}

int CCardDoc::DefaultTextSize(int nDefault)
{
// Go with half the size.
   if ((nDefault /= 2) < 8)
   {
      nDefault = 8;
   }
   return nDefault;
}
/*
// The "open" document handler for cards.
// This will locate all the card groups and store them in the structure.
*/
BOOL CCardDoc::OnOpenDocument(const char *pszPathName)
{                                       
   if (!CPmwDoc::OnOpenDocument(pszPathName))
      return FALSE;

   return LocatePanels();
}

void CCardDoc::DoPostOpen(void)
{
	// Retrieve a collection for attaching a copyright object
	IPathManager* pPathManager = GetPathManager();
	CString csFile;
	CString csCollection;
	if (pPathManager->BindPath(m_csBrowserPath, csFile, &csCollection) == PBT_CollectionItem)
	{
		// Find a collection with the given path
		CPMWCollection* pCollection = GetCollectionManager()->FindCollection(csCollection);

		if (pCollection != NULL)
		{
			// Retrieve copyright info
			enum CopyrightVariants nCopyrightType = COPYRIGHT_Default;
			PROJECT_TYPE ProjectType = get_project_type();
			ORIENTATION Orientation = get_orientation();
			if (Orientation == PORTRAIT)
			{
				nCopyrightType = (ProjectType == PROJECT_TYPE_Card || ProjectType == PROJECT_TYPE_NoteCard) 
					? COPYRIGHT_QuarterFoldT : COPYRIGHT_HalfFoldW;
			}
			else if (Orientation == LANDSCAPE)
			{
				nCopyrightType = (ProjectType == PROJECT_TYPE_Card || ProjectType == PROJECT_TYPE_NoteCard) 
					? COPYRIGHT_QuarterFoldW : COPYRIGHT_HalfFoldT;
			}
			SCopyright Copyright = pCollection->GetCopyright(nCopyrightType);
			if (Copyright.csCopyrightName.IsEmpty())
			{
				// Try default...
				Copyright = pCollection->GetCopyright(COPYRIGHT_Default);
			}

			// Load the .pmo with copyright info and put it on back panel
			if (!Copyright.csCopyrightName.IsEmpty())
			{
				// Check if the copyright file exists. We do this here so that if the
				// file doesn't exist we can just move on without giving the user an
				// error message.
				CString csFileName = pPathManager->LocatePath((LPCSTR)Copyright.csCopyrightName);
				if (Util::FileExists(csFileName))
				{
					set_panel(CARD_PANEL_Back);

					SetImportPictureName(csFileName, "");
					if (create_chosen_picture() == ERRORCODE_None)
					{
						// Center the copyright object
						PBOX SelectedBound;
						GetSelectBound(&SelectedBound);

						// Get dimensions of panel and copyright object
						PBOX BackBound;
						get_panel_world(&BackBound);

						// Do that thing
						long nPanelWidth = BackBound.x1 - BackBound.x0;
						long nPanelHeight = BackBound.y1 - BackBound.y0;
						long nSelectedWidth = SelectedBound.x1 - SelectedBound.x0;
						long nSelectedHeight = SelectedBound.y1 - SelectedBound.y0;
						long nRawAdjustedW = (long)(nPanelWidth * (Copyright.rctAbstractBound.right - Copyright.rctAbstractBound.left));
						long nRawAdjustedH = (long)(nPanelHeight * (Copyright.rctAbstractBound.bottom - Copyright.rctAbstractBound.top));
						
						PBOX NewBound;
						NewBound.x0 = BackBound.x0 + (long)(Copyright.rctAbstractBound.left * nPanelWidth);					
						NewBound.y0 = BackBound.y0 + (long)(Copyright.rctAbstractBound.top * nPanelHeight);
						if (nRawAdjustedW < nRawAdjustedH)
						{
							// Assign width
							NewBound.x1 = NewBound.x0 + nRawAdjustedW;
							
							// Compute height & center along y
							long height = nSelectedHeight * nRawAdjustedW / nSelectedWidth;
							NewBound.y0 += (nRawAdjustedH - height) / 2;
							NewBound.y1 = NewBound.y0 + height;
						}
						else
						{
							// Compute width & center along x
							long width = nSelectedWidth * nRawAdjustedH / nSelectedHeight;
							NewBound.x0 += (nRawAdjustedW - width) / 2;
							NewBound.x1 = NewBound.x0 + width;

							// Assign height
							NewBound.y1 = NewBound.y0 + nRawAdjustedH;
						}


						// Compute new position for each selected object
						RectPageObjectPtr pObject = (RectPageObjectPtr)selected_objects();
						while (pObject != NULL)
						{
							PBOX Bound = pObject->get_unrotated_bound();
							long nWidth = NewBound.x1 - NewBound.x0;
							long nHeight = NewBound.y1 - NewBound.y0;
							Bound.x0 = NewBound.x0 + (Bound.x0 - SelectedBound.x0) * nWidth / nSelectedWidth;
							Bound.y0 = NewBound.y0 + (Bound.y0 - SelectedBound.y0) * nHeight / nSelectedHeight;
							Bound.x1 = NewBound.x1 - (SelectedBound.x1 - Bound.x1) * nWidth / nSelectedWidth;
							Bound.y1 = NewBound.y1 - (SelectedBound.y1 - Bound.y1) * nHeight / nSelectedHeight;

							// Store & advance
							pObject->set_unrotated_bound(Bound);
							pObject->calc();
							pObject = (RectPageObjectPtr)pObject->next_selected_object();
						}

						// Return to pristine state
						deselect_all();
					}

					set_panel(CARD_PANEL_Front);
				}
			}
		}
	}
}

BOOL CCardDoc::LocatePanels(void)
{
/*
// Locate all the panels.
*/

   DWORD dwPages = NumberOfPages();
   for (DWORD dwPage = 0; dwPage < dwPages; dwPage++)
   {
      GotoPage(dwPage, FALSE);

      for (PMGPageObjectPtr object = (PMGPageObjectPtr)page_objects->first_object();
               object != NULL;
               object = (PMGPageObjectPtr)object->next_object())
      {
         if (object->type() == OBJECT_TYPE_Group)
         {
            SHORT panel = object->get_panel();

         /* Double check the group number, just in case! */

            if (panel < info.number_of_panels)
            {
               panel_group[panel] = (GroupObjectPtr)object;

            /* Be sure this is correct. */

               object->set_primary_action(ACTION_EDIT_PANEL);
               object->set_select_flags(SELECT_FLAG_boundary
                                          | SELECT_FLAG_solo_select);

               RemoveGroupBits((GroupObjectPtr)object);
               FixPanelValues((GroupObjectPtr)object);
            }
         }
      }
   }

/* Make sure we got all of our groups. */

   for (SHORT i = 0; i < info.number_of_panels; i++)
   {
      if (panel_group[i] == NULL)
      {
         return FALSE;
      }
   }

/* Let the configuration have its way with our back panel. */

   GetConfiguration()->SupportsCardBack(FALSE, this);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCardDoc commands
void
CCardDoc::KillPrintingBackgrounds(void)
{
	for (int i = 0; i < 3; i ++)
	{
		if (m_pPrintingBackground[i] != NULL)
		{
			delete m_pPrintingBackground[i];
			m_pPrintingBackground[i] = NULL;
		}
	}
}

CBackgroundObject*
CCardDoc::GetPrintingBackground(int pb)
{
	if ((pb < 0) || (pb > 2))
		return NULL;
	
	if (m_pPrintingBackground[pb] == NULL)
	{
		CBackgroundObject	*bo = new CBackgroundObject(OBJECT_TYPE_PageBackground, get_database());

		m_pPrintingBackground[pb] = bo;
	}
	
	return m_pPrintingBackground[pb];
}

CPageProperties * 
CCardDoc::GetPageProperties() 
{ 
	return GetPageProperties(get_current_panel());
}

CPageProperties * 
CCardDoc::GetPageProperties(int pp)
{
	if ((pp >= 0) && (pp < number_of_panels()))
	{
		GroupObjectPtr pg = panel_group[pp];
		return pg->object_list()->GetPageProperties(); 
	}
	return NULL;
}

void
CCardDoc::AddBackgroundForDrawing(CPtrList &List)
{
	SHORT cp = get_current_panel();
	if (cp == -2)
	{
		for (int i = 0; i < number_of_panels(); i ++)
		{
			AddBackgroundForDrawing(List, i);
		}
	}
	else
	{
		KillPrintingBackgrounds();
		INHERITED::AddBackgroundForDrawing(List);
	}
}

void
CCardDoc::AddBackgroundForDrawing(CPtrList &List, int idx)
{
	CPageProperties *pp = GetPageProperties(idx);
	if (pp != NULL)
	{
		if (pp->HasBackground())
		{
			CBackgroundObject	*pb = GetPrintingBackground(idx);
			if (pb != NULL)
			{
				PBOX  pboxObjectDims;
				get_panel_world(&pboxObjectDims, idx);
				
				pb->set_bound(pboxObjectDims);
				pb->SetBackgroundColor(pp->GetBackgroundColor());
				FLAGS Flags = info.pip[idx].flags;
				pb->SetTextureRecNum(pp->GetTextureRecNum(), (Flags & FLIPPED) == FLIPPED);
		
				List.AddTail(pb);
			}
		}
	}
}

void CCardDoc::MergePanelWithPanel(GroupObject* pNewGroup, GroupObject* pOldGroup)
{
   ObjectListPtr pOldList = pOldGroup->object_list();
   ObjectListPtr pNewList = pNewGroup->object_list();

   PMGPageObjectPtr pNext;
   for (PMGPageObjectPtr pObject = (PMGPageObjectPtr)pOldList->first_object();
            pObject != NULL;
            pObject = pNext)
   {
      pNext = (PMGPageObjectPtr)pObject->next_object();

      pOldList->detach(pObject);
      pNewList->append(pObject);
   }

   pNewGroup->establish(FALSE);
   RemoveGroupBits(pNewGroup);
   FixPanelValues(pNewGroup);
}

void CCardDoc::RenumberPanel(GroupObject* pGroup, int nPanel, BOOL fRemoveLayout)
{
   pGroup->set_panel(nPanel);

   for (PMGPageObjectPtr pObject = (PMGPageObjectPtr)pGroup->object_list()->first_object();
            pObject != NULL;
            pObject = (PMGPageObjectPtr)pObject->next_object())
   {
      pObject->set_panel(nPanel);
      if (fRemoveLayout)
      {
         pObject->set_new_layout(MANUAL_LAYOUT);
      }
   }
}

BOOL CCardDoc::MergeOldPanels(void)
{
   GroupObjectPtr OldPanelGroup[4];    /* One for each panel. */
   PMGPageObjectPtr pObject;

   for (pObject = (PMGPageObjectPtr)page_objects->first_object();
            pObject != NULL;
            pObject = (PMGPageObjectPtr)pObject->next_object())
   {
   /* Let's be really picky about badly-formed cards. */
      if (pObject->type() != OBJECT_TYPE_Group)
      {
         return FALSE;
      }

      int nPanel = pObject->get_panel();

   /* Double check the group number, just in case! */

      if (nPanel >= 4)
      {
      /* Let's be really picky about badly-formed cards. */
         return FALSE;
      }

   /* Remember the old group. */

      OldPanelGroup[nPanel] = (GroupObjectPtr)pObject;
   }

/*
// Verify that we got all of our panels.
*/

   for (int nPanel = 0; nPanel < 4; nPanel++)
   {
      if (OldPanelGroup[nPanel] == NULL)
      {
         return (nPanel == 3);   // Only three? Maybe this is a new card?
      }
   }

/*
// We now have all four panels of an "original" card. We need to reduce
// this to three in the following way:
//
// The front panel stays the same.
// We merge the two inner panels.
// The last panel is renumbered to 2 (from 3).
*/

   /*
   // Merge the two inner panels together.
   // Since the panel definition has changed so radically, we remove all
   // "standard" layouts from the objects since they were based on a panel
   // world which no longer exists. This keeps everything in the same place;
   // it all just becomes "manual".
   */

   PBOX FrontBound;
   PBOX InsideBound;
   PBOX BackBound;
   CPaperInfo* pPaperInfo = m_pPaperInfo;

   if (m_pPaperInfo->SlotDims().x == MakeFixed(0)
       || m_pPaperInfo->SlotDims().y == MakeFixed(0))
   {
      m_pPaperInfo = GET_PMWAPP()->GetPaperInfo(get_orientation());
   }
   get_panel_world(&FrontBound, CARD_PANEL_Front);
   get_panel_world(&InsideBound, CARD_PANEL_Inside);
   get_panel_world(&BackBound, CARD_PANEL_Back);
   m_pPaperInfo = pPaperInfo;

   OldPanelGroup[CARD_PANEL_Front]->set_bound(FrontBound);

   GroupObject* pOldRight = OldPanelGroup[CARD_PANEL_OldInsideRight];
   GroupObject* pOldLeft = OldPanelGroup[CARD_PANEL_OldInsideLeft];

   pOldLeft->set_bound(InsideBound);

   MergePanelWithPanel(pOldLeft, pOldRight);
   RenumberPanel(OldPanelGroup[CARD_PANEL_OldInsideLeft], CARD_PANEL_Inside, TRUE);

   // Get rid of the empty old inner right group.
   page_objects->detach(pOldRight);
   pOldRight->destroy();
   delete pOldRight;

   // Renumber the back panel.

   RenumberPanel(OldPanelGroup[CARD_PANEL_OldBack], CARD_PANEL_Back, FALSE);

   // Make sure the back panel is sized correctly.

   OldPanelGroup[CARD_PANEL_OldBack]->set_bound(BackBound);
   OldPanelGroup[CARD_PANEL_OldBack]->establish(FALSE);
   RemoveGroupBits(OldPanelGroup[CARD_PANEL_OldBack]);
   FixPanelValues(OldPanelGroup[CARD_PANEL_OldBack]);
   return TRUE;
}

/*
// Process the action of an object for a card.
*/

VOID CCardDoc::process_action(CView* view, PMG_OBJECT_ACTION action, PMGPageObjectPtr object, LPVOID data, PPNT p)
{
   switch (action)
   {
      case ACTION_EDIT_PANEL:
      {
         ((CCardView *)view)->set_card_panel_view(object->get_panel());
         break;
      }
      default:
      {
         CPmwDoc::process_action(view, action, object, data, p);
         break;
      }
   }
}

/*
// We need to make sure the panels are flipped around right for saving.
*/

BOOL CCardDoc::OnSaveDocument(const char *pszPathName)
{
   BOOL ret;
   SHORT panel_save = current_panel;

/* Make sure all panels are flipped correctly. */

   set_panel(-2);
   ret = CPmwDoc::OnSaveDocument(pszPathName);
   set_panel(panel_save);

   return ret;
}

VOID CCardDoc::size_to_paper(VOID)
{
   SHORT panel_save = current_panel;

/* Make sure all panels are flipped correctly. */

   set_panel(-2);
   CPmwDoc::size_to_paper();
   set_panel(panel_save);

   reset_views();
}

BOOL CCardDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{
   SHORT panel_save = current_panel;

/* Make sure all panels are flipped correctly. */

   set_panel(CARD_PANEL_Front);
   BOOL fTerminated = CPmwDoc::DrawPreview(re, rc);
   set_panel(panel_save);

   return fTerminated;
}

void CCardDoc::GetPreviewBound(PBOX* pBound)
{
   get_panel_world(pBound, CARD_PANEL_Front);
}

void CCardDoc::GoToFirstPage()
{
	/* Used for templates only */

	ASSERT(number_of_panels() > 0);
	set_panel(0);
}


LPCSTR CCardDoc::PanelTitle(int nPanel)
{
   switch (nPanel)
   {
      case CARD_PANEL_Front:
      {
         // JRO return "Front";
         return GET_PMWAPP()->GetResourceStringPointer (IDS_FRONT);
      }
      case CARD_PANEL_Inside:
      {
         // JRO return "Inside";
         return GET_PMWAPP()->GetResourceStringPointer (IDS_INSIDE);
      }
      case CARD_PANEL_Back:
      {
         // JRO return "Back";
         return GET_PMWAPP()->GetResourceStringPointer (IDS_BACK);
      }
      default:
      {
         return NULL;
      }
   }
}

BOOL CCardDoc::ReadDocument(void)
{
   if (!CPmwDoc::ReadDocument())
   {
      return FALSE;
   }
/*
// Setup the main card groups.
// Brochures are handled later.
*/

   if (document->get_project_type() != PROJECT_TYPE_Brochure)
   {
      switch (document->GetProjectSubtype())
      {
         case CARD_Unknown:
         {
         /*
         // This is an old card. We assume old cards are side fold.
         // We need to fix up the groups to make three instead of four.
         */
            if (!MergeOldPanels())
            {
               return FALSE;
            }

         /* We are now side-fold. */
            document->SetProjectSubtype(CARD_SideFold);
         // Fall through to...
         }
         case CARD_SideFold:
         {
            info = *SideFoldInfo();
            break;
         }
         case CARD_TopFold:
         {
            info = *TopFoldInfo();
            break;
         }
      }
   }

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHalfCardDoc

/*
// The side-fold half-fold card project info.
*/

ProjectInfo CHalfCardDoc::m_SideFoldInfo =
{
   PROJECT_TYPE_HalfCard,
   CARD_SideFold,
   PORTRAIT,
   sizeof(SideHalfFoldPanels)/sizeof(SideHalfFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   SideHalfFoldPanels
};

/*
// The top-fold half-fold card project info.
*/

ProjectInfo CHalfCardDoc::m_TopFoldInfo =
{
   PROJECT_TYPE_HalfCard,
   CARD_TopFold,
   PORTRAIT,
   sizeof(TopHalfFoldPanels)/sizeof(TopHalfFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   TopHalfFoldPanels
};

IMPLEMENT_SERIAL(CHalfCardDoc, CCardDoc, 0 /* schema number*/ )

CHalfCardDoc::CHalfCardDoc()
{
   // Default to something.
   info = m_SideFoldInfo;
}

CHalfCardDoc::~CHalfCardDoc()
{
}

/*
// Create the two pages for our document.
*/

ERRORCODE CHalfCardDoc::CreatePages(void)
{
   ERRORCODE error;
   DB_RECORD_NUMBER lPage;

   if ((error = document->new_page(&lPage)) == ERRORCODE_None)
   {
      // Create the other page.
      error = document->new_page(&lPage);
   }
   return error;
}

BOOL CHalfCardDoc::OnOpenDocument(const char *pszPathName)
{
   if (!CPmwDoc::OnOpenDocument(pszPathName))
   {
      return FALSE;
   }

   if (document->get_project_type() == PROJECT_TYPE_Brochure)
   {
      // Assign the fold type.
      // These fold types are the "normal" ones.
      if (document->get_orientation() == PORTRAIT)
      {
         info = *TopFoldInfo();
      }
      else
      {
         info = *SideFoldInfo();
      }

      // Convert from brochure to half-fold card.
      return ConvertFromBrochure();
   }

   // Locate the panels on the card.
   return LocatePanels();
}

void
CHalfCardDoc::AddBackgroundForDrawing(CPtrList &List)
{
	SHORT cp = get_current_panel();
	if (cp == -2)
	{
		DWORD pi = CurrentPageIndex();
		if (pi == 0)
		{
			INHERITED::AddBackgroundForDrawing(List, 0);
			INHERITED::AddBackgroundForDrawing(List, 2);
		}
		else
		{
			INHERITED::AddBackgroundForDrawing(List, 1);
		}
	}
	else
	{
		KillPrintingBackgrounds();
		INHERITED::AddBackgroundForDrawing(List);
	}
}

BEGIN_MESSAGE_MAP(CHalfCardDoc, CCardDoc)
   //{{AFX_MSG_MAP(CHalfCardDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

ProjectInfo* CHalfCardDoc::SideFoldInfo(void)
{
   return &m_SideFoldInfo;
}

ProjectInfo* CHalfCardDoc::TopFoldInfo(void)
{
   return &m_TopFoldInfo;
}

/*
// Return the area of a pbox.
*/

static double BoxArea(const PBOX* pBox)
{
   return ((double)(pBox->x1-pBox->x0)) * ((double)(pBox->y1-pBox->y0));
}

/*
// Return the coverage of this object on the panel.
//  0.0 means the object is not on the panel.
//  1.0 means the object is entirely on the panel.
//  In between means what it seems that it should.
*/

static double ComputeCoverage(PBOX* pBound, PBOX* pPanel)
{
   PBOX Intersection;
   if (IntersectBox(&Intersection, pBound, pPanel))
   {
      double dNumerator = BoxArea(&Intersection);
      double dDenominator = BoxArea(pBound);
      if (dDenominator == 0.0)
      {
         ASSERT(dNumerator == 0.0);
         return 0.0;
      }
      return dNumerator/dDenominator;
   }
   else
   {
      return 0.0;
   }
}

/*
// Does a box cover part of a graphic?
*/

static BOOL CoversGraphic(ObjectList* pList, PBOX* pBound)
{
   BOOL fCovers = FALSE;
   // We start at the end in this loop. Maybe that will be useful someday.
   for (PMGPageObject* pObject = (PMGPageObjectPtr)pList->last_object();
            pObject != NULL;
            pObject = (PMGPageObjectPtr)pObject->previous_object())
   {
      if (pObject->type() == OBJECT_TYPE_Graphic)
      {
         // Object is a graphic. Get its bound.
         PBOX GBound = pObject->get_bound();

         // Do they intersect?
         if (IntersectBox(&GBound, &GBound, pBound))
         {
            fCovers = TRUE;
            break;
         }
      }
   }

   return fCovers;
}

/*
// Move all the objects of a panel.
*/

void CHalfCardDoc::MovePanel(int nPanel, PCOORD lDeltaX, PCOORD lDeltaY)
{
   ObjectList* pList = panel_group[nPanel]->object_list();

   // Run through and move each object.
   for (PMGPageObject* pObject = (PMGPageObjectPtr)pList->first_object();
            pObject != NULL;
            pObject = (PMGPageObjectPtr)pObject->next_object())
   {
      PBOX Bound = ((RectPageObject*)pObject)->get_unrotated_bound();
      Bound.x0 += lDeltaX;
      Bound.y0 += lDeltaY;
      Bound.x1 += lDeltaX;
      Bound.y1 += lDeltaY;
      ((RectPageObject*)pObject)->set_unrotated_bound(Bound);
   }
}

/*
// Convert an old brochure-style half-fold card to the real thing.
*/

BOOL CHalfCardDoc::ConvertFromBrochure(void)
{
   if (!CreatePanels())
   {
      return FALSE;
   }

   //
   // Handle the inside objects.
   //

   // Go to the inside page.
   GotoPage(info.pip[CARD_PANEL_Inside].m_nPageIndex, FALSE);

   // Add all the objects to the panel group.
   GroupObject* pGroup = panel_group[CARD_PANEL_Inside];
   ASSERT(pGroup != NULL);
   ObjectList* pList = pGroup->object_list();
   ASSERT(pList != NULL);

   // Run through and move each object.
   PMGPageObject* pObject;
   PMGPageObject* pNext;
   for (pObject = (PMGPageObjectPtr)page_objects->first_object();
            pObject != NULL;
            pObject = pNext)
   {
      // Remember the next object.
      pNext = (PMGPageObjectPtr)pObject->next_object();

      if (pObject == pGroup)
      {
         // Skip the group object.
         continue;
      }

      // Move the object across lists.
      page_objects->detach(pObject);
      pList->append(pObject);
   }

   //
   // Handle the outside (front/back) objects.
   //

   // Go to the outside page.
   GotoPage(info.pip[CARD_PANEL_Front].m_nPageIndex, FALSE);
   ASSERT(info.pip[CARD_PANEL_Front].m_nPageIndex == info.pip[CARD_PANEL_Back].m_nPageIndex);

   // Get the panel groups for front and back.
   GroupObject* pFrontGroup = panel_group[CARD_PANEL_Front];
   GroupObject* pBackGroup = panel_group[CARD_PANEL_Back];
   ASSERT(pFrontGroup != NULL);
   ASSERT(pBackGroup != NULL);

   // Get the object lists.
   ObjectList* pFrontList = pFrontGroup->object_list();
   ASSERT(pFrontList != NULL);
   ObjectList* pBackList = pBackGroup->object_list();
   ASSERT(pBackList != NULL);

   // Get the panel bound for each panel (front and back).
   PBOX FrontBound;
   get_panel_world(&FrontBound, CARD_PANEL_Front);
   PBOX BackBound;
   get_panel_world(&BackBound, CARD_PANEL_Back);
   
   // Top-fold card brochures were designed wrong.
   // Reverse the front and back panels.
   if (FoldType() == CARD_TopFold)
   {
      // Swap the bounds used for comparison.
      PBOX Tmp = FrontBound;
      FrontBound = BackBound;
      BackBound = Tmp;
   }

   //
   // We need to loop on all objects and assign them to some panel.
   //

   for (pObject = (PMGPageObjectPtr)page_objects->first_object();
            pObject != NULL;
            pObject = pNext)
   {
      // Remember the next object.
      pNext = (PMGPageObjectPtr)pObject->next_object();

      // Skip the group objects.
      if (pObject == pFrontGroup || pObject == pBackGroup)
      {
         continue;
      }

      // Figure out which panel it goes on.
      PBOX Bound = pObject->get_bound();
      BOOL fToFront = FALSE;
      BOOL fToBack = FALSE;

      // Our first test will be for white boxes. White boxes are often placed
      // to cover part of a graphic. These boxes may or may not be on the
      // particular panel they're supposed to be on.
      // So we try to find the graphic they're covering.
      // Also, we do this for all shapes since it may not be a box.

      if (pObject->type() == OBJECT_TYPE_DrawingObject)
      {
			DrawingObject* pDrawingObject = (DrawingObject*)pObject;
         DrawingObjectRecord& Record = pDrawingObject->Record();

         // See if it has a white fill.
         if (pDrawingObject->GetFill().IsVisible() && (pDrawingObject->GetFill().m_ForegroundColor == COLOR_WHITE))
         {
            // We want to see if this object is being used to cover
            // a picture. If so, it goes on that panel. 

            if (CoversGraphic(pFrontList, &Bound))
            {
               fToFront = TRUE;
            }
            if (CoversGraphic(pBackList, &Bound))
            {
               fToBack = TRUE;
            }
         }
      }

      // If we have not assigned the object yet, move to test two.
      if (!(fToFront || fToBack))
      {
         /*
         // Our second test will be one of coverage.
         // The idea is to take the area of the object overlap for each panel
         // and see which one is bigger. Hopefully, there'll be a clear winner.
         // Compute coverage scores.
         */
         double dFrontScore = ComputeCoverage(&Bound, &FrontBound);
         double dBackScore = ComputeCoverage(&Bound, &BackBound);

         // If the object is more on one panel than the other (plus a factor),
         // we have a winner.
         if (dFrontScore >= 1.5*dBackScore)
         {
            fToFront = TRUE;
         }
         else if (dBackScore >= 1.5*dFrontScore)
         {
            fToBack = TRUE;
         }
         else
         {
            // No clear winner. Put it on both.
            fToFront = fToBack = TRUE;
         }
      }

      // Done with our tests. We should have decided by now.
      ASSERT(fToFront || fToBack);

      // If it needs to go on both, we need to duplicate it.
      BOOL fDuplicate = fToFront && fToBack;

      // Handle a front object.
      if (fToFront)
      {
         // Put the object on the front list.
         page_objects->detach(pObject);
         pFrontList->append(pObject);
         pObject->set_panel(CARD_PANEL_Front);
      }

      // Handle a back object.
      if (fToBack)
      {
         // Put the object on the back list.
         if (fDuplicate)
         {
            PMGPageObject* pOriginalObject = pObject;
            pObject = (PMGPageObject*)pObject->duplicate();
            if (pObject == NULL)
            {
               return FALSE;
            }
            pObject->calc();
				// We know for sure these extend across panels.
				// Crop them if we can.
				AutoCropGraphic(pOriginalObject);

            pObject->set_panel(CARD_PANEL_Back);      // Do it early for auto crop.
				AutoCropGraphic(pObject);
         }
         else
         {
            // Unhook it from the main list.
            page_objects->detach(pObject);
         }
         pBackList->append(pObject);
         pObject->set_panel(CARD_PANEL_Back);
      }
   }

   // Top-fold card brochures were designed wrong (though consistently).
   // Flip the front and back panels.
   if (FoldType() == CARD_TopFold)
   {
      // Compute the object deltas. (Remember, front and back are reversed.)
      PCOORD lDelta = FrontBound.y0 - BackBound.y0;

      MovePanel(CARD_PANEL_Front, 0, -lDelta);
      MovePanel(CARD_PANEL_Back, 0, lDelta);

      FlipCardPanel(CARD_PANEL_Front, FLIPPED);
      FlipCardPanel(CARD_PANEL_Back, FLIPPED);
   }

   //
   // Run through all panels and make sure everything is set as required.
   //
   for (int nPanel = 0; nPanel < info.number_of_panels; nPanel++)
   {
      panel_group[nPanel]->establish(FALSE);
      RemoveGroupBits(panel_group[nPanel]);
      FixPanelValues(panel_group[nPanel]);
   }

   // Do a test.
   if (panel_group[CARD_PANEL_Front]->object_list()->count() == 0)
   {
      AfxMessageBox("The front of the card is blank!");
   }

   // Set the correct project type.
   ASSERT(info.project_type == PROJECT_TYPE_HalfCard);
   document->set_project_type(info.project_type);
   document->SetProjectSubtype(info.project_sub_type);
   document->modified();

   return TRUE;
}

void CHalfCardDoc::AutoCropGraphic(PMGPageObject* pObject)
{
	if (pObject->get_rotation() == 0.0)	// Keep it simple
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Graphic:
			case OBJECT_TYPE_DrawingObject:
			case OBJECT_TYPE_Rectangle:
			case OBJECT_TYPE_Ellipse:
			{
				// Crop it!
				PBOX Bound = pObject->get_bound();
				PBOX World;
				get_panel_world(&World, pObject->get_panel());
				double dWidth = (double)(Bound.x1 - Bound.x0);
				double dHeight = (double)(Bound.y1 - Bound.y0);

				CDoubleRect& VisibleRect = pObject->GetVisibleRect();
				PBOX NewBound = Bound;
				BOOL fChanged = FALSE;
				if (Bound.x0 < World.x0)
				{
					VisibleRect.left = ((double)(World.x0 - Bound.x0))/dWidth;
					NewBound.x0 = World.x0;
					fChanged = TRUE;
				}
				if (Bound.y0 < World.y0)
				{
					VisibleRect.top = ((double)(World.y0 - Bound.y0))/dHeight;
					NewBound.y0 = World.y0;
					fChanged = TRUE;
				}
				if (Bound.x1 > World.x1)
				{
					VisibleRect.right = ((double)(World.x1 - Bound.x0))/dWidth;
					NewBound.x1 = World.x1;
					fChanged = TRUE;
				}
				if (Bound.y1 > World.y1)
				{
					VisibleRect.bottom = ((double)(World.y1 - Bound.y0))/dHeight;
					NewBound.y1 = World.y1;
					fChanged = TRUE;
				}
				if (fChanged)
				{
					// Set the new bound back.
					((RectPageObject*)pObject)->set_bound(NewBound);
					pObject->calc();
				}
				break;
			}
			default:
			{
				// Do nothing;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNoteCardDoc

/*
// The side-fold card project info.
*/

ProjectInfo CNoteCardDoc::m_SideFoldInfo =
{
   PROJECT_TYPE_NoteCard,
   CARD_SideFold,
   PORTRAIT,
   sizeof(SideFoldPanels)/sizeof(SideFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   SideFoldPanels
};

/*
// The top-fold card project info.
*/

ProjectInfo CNoteCardDoc::m_TopFoldInfo =
{
   PROJECT_TYPE_NoteCard,
   CARD_TopFold,
   PORTRAIT,
   sizeof(TopFoldPanels)/sizeof(TopFoldPanels[0]),
   IDS_CARD_FRAME_LAYOUT,
   IDS_CARD_GRAPHIC_LAYOUT,
   IDS_CARD_DUPLICATE_LAYOUT,
   TopFoldPanels
};

IMPLEMENT_SERIAL(CNoteCardDoc, CCardDoc, 0 /* schema number*/ )

CNoteCardDoc::CNoteCardDoc()
{
   // Default to something.
   info = m_SideFoldInfo;
}

CNoteCardDoc::~CNoteCardDoc()
{
}

BEGIN_MESSAGE_MAP(CNoteCardDoc, CCardDoc)
   //{{AFX_MSG_MAP(CNoteCardDoc)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

ProjectInfo* CNoteCardDoc::SideFoldInfo(void)
{
   return &m_SideFoldInfo;
}

ProjectInfo* CNoteCardDoc::TopFoldInfo(void)
{
   return &m_TopFoldInfo;
}


