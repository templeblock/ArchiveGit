/*
// $Workfile: PRINTPRJ.CPP $
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
// $Log: /PM8/App/PRINTPRJ.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 14    12/23/98 1:51p Psasse
// Code clean up
// 
// 13    11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 12    10/12/98 1:26p Mwilson
// added craft deluxe configuration
// 
// 11    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 10    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 9     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 8     8/19/98 5:26p Johno
// OnManualPrintDocument() - fixed thumbnail sizing bug, Craft / Print
// conditional
// 
// 7     8/19/98 12:14p Johno
// Changes to CProjectPrintView::OnManualPrintDocument() to extract
// thumbnail
// 
// 6     8/18/98 2:31p Johno
// Changes to CProjectPrintView::OnManualPrintDocument()
// 
// 5     8/11/98 12:23p Johno
// Changes for OnManualPrintDocument()
// 
// 4     8/10/98 6:29p Johno
// Changes for OnManualPrintDocument()
// 
// 3     7/28/98 7:36p Jayn
// Fixed crashing bug and "dead" printer if cancelled out
// 
// 2     1/30/98 4:28p Hforman
// remove #include "pictdlg.h"
// 
//    Rev 1.0   14 Aug 1997 15:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:06   Fred
// Initial revision.
// 
//    Rev 1.3   24 Jul 1997 14:37:42   Jay
// Print project and art improvements (fixes)
// 
//    Rev 1.2   05 Jun 1997 17:10:58   doberthur
// 
// 
//    Rev 1.1   01 Jun 1997 16:29:02   doberthur
// 
// 
// 
//    Rev 1.0   30 May 1997 18:35:42   doberthur
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"

#include <string.h>
#include <direct.h>

#include "pmwdoc.h"
#include "pmwview.h"

#include "pmwprint.h"

#include "objlist.h"
#include "printprj.h"

#include "utils.h"
#include "textflow.h"
#include "ctxp.h"

#include "pararec.h"
#include "framerec.h"
#include "frameobj.h"
#include "projobj.h"
#include "patobj.h"
#include "prevrec.h"

#include "printart.h"
#include "rectobj.h"		// For RectangleObject in OnManualPrintDocument()
#include "typeface.h"	// For typeface_server in SetStyleFace()
#include "pmgfont.h"		// For PMGFontServer in SetStyleFace()
#include "pmwcfg.h"		// For GetConfiguration
#include "COLLFILE.H"	// For CFileThumbnails
#include "browser.h"		// For MakeThumbnail
#include "bmp.h"			// For CreateBMPFromDIB
#include "grafobj.h"		// For GraphicObject
#include "strmcach.h"	// For CStreamCache

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#define CATALOG_BACKGROUND

/////////////////////////////////////////////////////////////////////////////
// PrintProjectEntry structure

PrintProjectEntry::PrintProjectEntry(LPCSTR pszName, LPCSTR pszFriendlyName, LPCSTR pszCategory, USHORT CollectionID, DWORD dwItemNumber)
{
	SetName(pszName);
	SetFriendlyName(pszFriendlyName);
	SetCategory(pszCategory);
	m_uCollectionID = CollectionID;
	m_dwItemNumber = dwItemNumber;
}

PrintProjectEntry::~PrintProjectEntry()
{
}

/////////////////////////////////////////////////////////////////////////////
// CProjectPrintView

SHORT CProjectPrintView::OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments)
{
	CArtPrintDialog *pdlg = (CArtPrintDialog*)pc.m_pPD;

	if (!pdlg->m_csManualPage.IsEmpty())
	{
		return OnManualPrintDocument(pc);
	}

	CPmwDoc *pDoc = GetDocument();

	SHORT bError = FALSE;
   ProjectObject* object = NULL;

	SHORT x, y;
	PCOORD grid_width, grid_height, step_width, step_height, text_height, new_x;
	PBOX pbox;
	BOOL broken;
	int32 current_page, num_pages;
#ifdef SPECIAL_CATALOG
	PCOORD x0 = 0, y0 =  (PAGE_RESOLUTION*3)/4;
#endif

	PrintProjectEntryPtr entry;

#ifdef OLD_CATALOG_STYLE
	CString csName;
#endif

/* Compute the size of the grid and the step. */
	PPNT dims = pDoc->get_dimensions();
	PMGDatabasePtr const database = pDoc->get_database();
	CTextStyle Style(database);

#ifdef CATALOG_BACKGROUND
	PATTERN_CREATE_STRUCT pcs;

	pcs.bound.x0 = 0;
	pcs.bound.y0 = 0;
	pcs.bound.x1 = dims.x;
	pcs.bound.y1 = dims.y;
	pcs.pattern = 0;
	pcs.color = COLOR_WHITE;
	pcs.bcolor = MAKE_COLOR(0, 0, 0, 32);
#endif

#ifdef SPECIAL_CATALOG
	dims.x -= x0;
	dims.y -= y0;

	SHORT const across = 10;
	SHORT const down = 5;
#else
	SHORT const across = pdlg->m_x_count;
	SHORT const down = pdlg->m_y_count;
#endif

	grid_width = dims.x/across;
	grid_height = dims.y/down;

	step_width = scale_pcoord(grid_width, 9, 10);

	Style.SetDefault();
	SHORT base_size;

#ifdef DOUBLE_NAMES
	step_height = scale_pcoord(grid_height, 7, 10);
	text_height = (grid_height - step_height)/2;
	base_size = (SHORT)scale_pcoord(text_height, (72*6)/10, PAGE_RESOLUTION);
#else
	step_height = scale_pcoord(grid_height, 9, 10);
	text_height = grid_height - step_height;
	base_size = (SHORT)scale_pcoord(text_height, (72*2)/3, PAGE_RESOLUTION);
#endif

/* Set the text style. */
	if (base_size > 12)
	{
		base_size = 12;
	}
	Style.Size(MakeFixed(base_size));
	Style.BaseSize(MakeFixed(base_size));
#ifdef DOUBLE_NAMES
	Style.Bold(TRUE);
#endif

	Style.UpdateFontMetrics();

/* Figure out number of pages we will print. */
	num_pages = 
#ifdef SPACE_ON_FIRST
					across +
#endif
					m_picture_list.count();
	{
		SHORT n;

		n = across * down;

		num_pages = (num_pages + n - 1) / n;
	}
	current_page = 0;

   /* Run through all the projects */
	ObjectListPtr list = pDoc->object_list();
	for (broken = FALSE, entry = (PrintProjectEntryPtr)m_picture_list.first_element();
							!broken && entry != NULL; )
	{
	   /* Tell the user what we're doing. */
      printer_update_name(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_BUILDING_PAGE), (VOIDPTR)&pc);

	   /* Make sure the current page is blank. */
		list->destroy_objects();

	   /* Build the next page. */
		BeginWaitCursor();

#ifdef CATALOG_BACKGROUND
		PatternObjectPtr patobj;
		if ((patobj = database->create_pattern_object(&pcs)) != NULL)
		{
			pDoc->append_object(patobj);
		}
#endif
      
      // loop for each project to it's place on the page
#ifdef SPECIAL_CATALOG
		for (pbox.y0 = y0, y = 0;
#else
		for (pbox.y0 = 0, y = 0;
#endif
						y < down && !broken;
						y++, pbox.y0 += grid_height)
		{
#ifdef SPACE_ON_FIRST
			if (current_page == 0 && y == 0)
			{
				continue;
			}
#endif
			pbox.y1 = pbox.y0 + step_height;
#ifdef SPECIAL_CATALOG
			for (pbox.x0 = x0, x = 0;
#else
			for (pbox.x0 = 0, x = 0;
#endif
							x < across;
							x++,
 							pbox.x0 += grid_width,
 							entry = (PrintProjectEntryPtr)entry->next_element())
			{
				CFrameObject* pFrame;
				PPNT p;

			/* Make sure we are on a good node. */
				if (entry == NULL)
				{
				/* All done! */
					y = down;
					break;
				}

			/* Show the name. */
//				multi_context.current_name = node->name;
			/* We have a selected node. Make a graphic for it. */
				pbox.x1 = pbox.x0 + step_width;

			/* Create the projrct object. */
#ifdef OLD_CATALOG_STYLE
				csName = m_chosen_dir;
				csName += entry->GetName();
#endif
            PROJECTOBJECT_CREATE_STRUCT gcs;

#ifdef OLD_CATALOG_STYLE
				gcs.file_name = csName;
#else
				
            // Build the path
            // set the category dir path
            char path[_MAX_PATH];
            strcpy(path, entry->GetName());
 
            gcs.file_name = entry->GetName();
            gcs.friendly_name = entry->GetFriendlyName();
#endif
            if ((object = new ProjectObject((DB_OBJECT_TYPE)0, database)) == NULL)
            {
					broken = TRUE;
               break;
            }
            // create the project object
				if ((object->create(&gcs, pDoc)) == ERRORCODE_None)
				{
			   	/* Get the size of the graphic. */
					if (object->original_dims(&p))
					{
						pDoc->append_object(object);

						dims.x = (PCOORD)step_width;
						dims.y = (PCOORD)step_height;

				   	/* Fit the graphic to its box. */
						if ((new_x = scale_pcoord(dims.y, p.x, p.y)) > dims.x)
						{
							dims.y = scale_pcoord(dims.x, p.y, p.x);
						}
						else
						{
							dims.x = new_x;
						}
				   	/* Set the bound. */
						PBOX bound;
						bound.x0 = (pbox.x0 + pbox.x1 - dims.x)/2;
						bound.x1 = bound.x0 + dims.x;
						bound.y0 = (pbox.y0 + pbox.y1 - dims.y)/2;
						bound.y1 = bound.y0 + dims.y;
						object->set_bound(bound);

						object->calc();
					}
					else
					{
                  if(object)
						   delete object;
						object = NULL;
   					broken = TRUE;
                  break;
					}
				}
            else {
               if(object)
				      delete object;
					object = NULL;
					broken = TRUE;
               break;
            }

			   /* Add a text frame for the name. */
				FRAMEOBJ_CREATE_STRUCT fcs;
				LPCSTR text = entry->GetName();

				fcs.bound.x0 = pbox.x0;
				fcs.bound.x1 = pbox.x1;
				fcs.bound.y0 = pbox.y1;

				fcs.alignment = ALIGN_center;
				fcs.vert_alignment = ALIGN_top;
//				fcs.text = NULL;
				fcs.flags = 0;
				fcs.pStyle = NULL;

#ifdef SPECIAL_CATALOG
				CHAR buffer[100];

				LPCSTR ptr;

				if ((ptr = strrchr(text, '\\')) == NULL)
				{
					ptr = text;
				}
				else
				{
				/* Move off of '\'. */
					ptr++;
				}

				strcpy(buffer, ptr);
				LPSTR foop;
				if ((foop = strrchr(buffer, '.')) != NULL)
				{
				/* Remove extension. */
					*foop = '\0';
				}

				fcs.bound.y1 = fcs.bound.y0+text_height;
				text = buffer;

#elif defined(DOUBLE_NAMES)

				fcs.bound.y1 = fcs.bound.y0+text_height*2;
				CHAR name[64];
				CHAR buffer[100];
				LPCSTR ptr;

				if ((ptr = strrchr(text, '\\')) == NULL)
				{
					ptr = text;
				}
				else
				{
				/* Move off of '\'. */
					ptr++;
				}
				BOOL blanket = FALSE;		
				if (object == NULL)
				{
					blanket = TRUE;
				}
				else
				{
					object->get_name(name, sizeof(name), -1);
					if (stricmp(name, ptr) == 0)
					{
						blanket = TRUE;
						strcpy(name, " ");
					}
				}

				if (blanket)
				{
				/* Blank it. */
					strcpy(name, " ");
				}
				strcpy(buffer, name);
				strcat(buffer, "\n");
				strcat(buffer, ptr);
				text = buffer;
#else
				fcs.bound.y1 = fcs.bound.y0+text_height;
#endif
				if ((pFrame = pDoc->create_frame_object(&fcs)) == NULL)
				{
					broken = TRUE;
					break;
				}

				int nCharacters = strlen(text);
				if (nCharacters != 0)
				{
					CTxp Txp(database);
					Txp.Init(pFrame, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);
					Txp.Style(Style);

					TRY
					{
						CHARACTER* pText = new CHARACTER[nCharacters];
						for (int n = 0; n < nCharacters; n++)
						{
							pText[n] = (CHARACTER)text[n];
						}
#ifdef DOUBLE_NAMES
						// First line is name + CR...
						int nLine1 = strlen(name)+1;
						// ...second line is the rest.
						int nLine2 = nCharacters - nLine1;

						// Insert line 1.
						Txp.InsertText(pText, nLine1);

						// Compute style for line 2:
						// no bold, and 8/10 the size.
						Txp.Style().Bold(FALSE);
						CFixed lSize = Txp.Style().BaseSize();
						lSize = MulDivFixed(lSize, MakeFixed(8), MakeFixed(10));
						Txp.Style().BaseSize(lSize);
						Txp.Style().Size(lSize);
						Txp.Style().UpdateFontMetrics();

#ifndef JUST_PRINT_FRIENDLY_NAME
						// Insert line 2. ,  skip this for friendly name only
						Txp.InsertText(pText+nLine1, nLine2);  
#endif

#else    //  Not DOUBLE_NAMES
						Txp.InsertText(pText, nCharacters);
#endif
						delete [] pText;
					}
					END_TRY
				}

				pDoc->append_object(pFrame);

          //  if(object)
				//   delete object;
			//	object = NULL;
			}
		} // end of for loop on each project document

		EndWaitCursor();

//   	/* See if we broke! */
		if (broken)
		{
			break;
		}

	   /* Print the page. */
		if (list->count() != 0)
		{
		/* We have objects! Print them. */
			++current_page;

			if (bError = CPmwView::OnPrintDocument(pc, current_page, num_pages))
			{
				break;
			}
		}

	} // for each project to print
	return broken ? 0 : bError;
}

void CProjectPrintView::SetStyleFace(CTextStyle& Style, LPCSTR pszFace)
{
	CPmwDoc *pDoc = GetDocument();

	int nFace = typeface_server.find_face(pszFace, FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = ((PMGFontServer*)pDoc->get_font_server())->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			pDoc->get_database()->free_font_record(lFace, TRUE);
		}
	}
}

#define	PN_BOX_BY_LENGTH()	(0.04 * PAGE_RESOLUTION * csPageNumber.GetLength())
//#define	REVERSED_PAGE_NUMBER

SHORT CProjectPrintView::OnManualPrintDocument(PrintContext& pc)
{
	int	Level = GetConfiguration()->ProductLevel();	
	
	BOOL	AGPrint = (Level == CPrintMasterConfiguration::PrintStandard) || (Level == CPrintMasterConfiguration::PrintPremium);
	BOOL  AGCraft = (Level == CPrintMasterConfiguration::CraftStandard || 
							Level == CPrintMasterConfiguration::CraftDeluxe ||
							Level == CPrintMasterConfiguration::CreataParty);
	
	PrintProjectEntry* pEntry;

	CPmwDoc *pDoc = GetDocument();

	CStreamCache StreamCache(pDoc->GetCollectionManager());

	SHORT bError = FALSE;

	CArtPrintDialog *pdlg = (CArtPrintDialog*)pc.m_pPD;
	// Compute the size of the grid and the step.
	//PPNT dims = pDoc->get_dimensions();
	PMGDatabasePtr const pDatabase = pDoc->get_database();
	CTextStyle Style(pDatabase);
	// ASSUMPTION: The following code assumes 0.25" margins on LaserJet.
	PCOORD DeviceLeftMargin = (PCOORD)(0.25*PAGE_RESOLUTION);
	PCOORD DeviceTopMargin = (PCOORD)(0.25*PAGE_RESOLUTION);
	PCOORD DeviceRightMargin = (PCOORD)(0.25*PAGE_RESOLUTION);
	PCOORD DeviceBottomMargin = (PCOORD)(0.25*PAGE_RESOLUTION);

#ifdef LOCALIZE
	// get size info from printart.ini file
	// get page size from printart.ini file
	char WidthString[128];
	char HeightString[128];

	GetPrivateProfileString("PageSize", "InchesWide", "8.0", (LPSTR) WidthString, sizeof(WidthString), "Printart.ini");
	GetPrivateProfileString("PageSize", "InchesHigh", "11.0", (LPSTR) HeightString, sizeof(HeightString), "Printart.ini");

	double fWidth = atof((const char *) WidthString);
	double fHeight = atof((const char *) HeightString);
	PCOORD PageWidth = (PCOORD)(fWidth*PAGE_RESOLUTION);
	PCOORD PageHeight = (PCOORD)(fHeight*PAGE_RESOLUTION);

	int const nAcross = (int) GetPrivateProfileInt("ImageLayout", "NumberAcross", 9, "Printart.ini");
	int const nDown = (int) GetPrivateProfileInt("ImageLayout", "NumberDown", 12, "Printart.ini");
	// X and Y offsets of (0.75, 1.0) center the 7 x 9 image in an 8.5 x 11 page.

	GetPrivateProfileString("PageSize", "PageXOffset", "0.75", (LPSTR) WidthString, sizeof(WidthString), "Printart.ini");
	GetPrivateProfileString("PageSize", "PageYOffset", "1.0", (LPSTR) HeightString, sizeof(HeightString), "Printart.ini");

	fWidth = atof((const char *) WidthString);
	fHeight = atof((const char *) HeightString);
	PCOORD PageXOffset = (PCOORD)(fWidth*PAGE_RESOLUTION);
	PCOORD PageYOffset = (PCOORD)(fHeight*PAGE_RESOLUTION);

	GetPrivateProfileString("PageSize", "PictureTop", "1.05", (LPSTR) WidthString, sizeof(WidthString), "Printart.ini");

	fWidth = atof((const char *) WidthString);
	PCOORD PictureTop = (PCOORD)(fWidth*PAGE_RESOLUTION);

	GetPrivateProfileString("PageSize", "TopMargin", "0.375", (LPSTR) WidthString, sizeof(WidthString), "Printart.ini");
	GetPrivateProfileString("PageSize", "BottomMargin", "0.375", (LPSTR) HeightString, sizeof(HeightString), "Printart.ini");

	fWidth = atof((const char *) WidthString);
	fHeight = atof((const char *) HeightString);
	PCOORD TopMargin = (PCOORD)(fWidth*PAGE_RESOLUTION);
	PCOORD BottomMargin = (PCOORD)(fHeight*PAGE_RESOLUTION);

	GetPrivateProfileString("PageSize", "InnerMargin", "0.75", (LPSTR) WidthString, sizeof(WidthString), "Printart.ini");
	GetPrivateProfileString("PageSize", "OuterMargin", "0.375", (LPSTR) HeightString, sizeof(HeightString), "Printart.ini");

	fWidth = atof((const char *) WidthString);
	fHeight = atof((const char *) HeightString);
	PCOORD InnerMargin = (PCOORD)(fWidth*PAGE_RESOLUTION);
	PCOORD OuterMargin = (PCOORD)(fHeight*PAGE_RESOLUTION);
#else // if not defined LOCALIZE
	// 108 images per page (9 x 12)
//	int const nAcross = 9;
//	int const nDown = 12;

	int const nAcross = 5;
	int const nDown = 6;

	// 7 x 9 in page size.
	PCOORD PageWidth = (PCOORD)(7.0*PAGE_RESOLUTION);
	PCOORD PageHeight = (PCOORD)(9.0*PAGE_RESOLUTION);

	// X and Y offsets of (0.75, 1.0) center the 7 x 9 image in an 8.5 x 11 page.
	PCOORD PageXOffset = (PCOORD)(0.75*PAGE_RESOLUTION);
	PCOORD PageYOffset = (PCOORD)(1.0*PAGE_RESOLUTION);

	PCOORD PictureTop = (PCOORD)(1.05*PAGE_RESOLUTION);
	PCOORD TopMargin = (PCOORD)(0.375*PAGE_RESOLUTION);
	PCOORD BottomMargin = (PCOORD)(0.375*PAGE_RESOLUTION);
	PCOORD InnerMargin = (PCOORD)(0.75*PAGE_RESOLUTION);
	PCOORD OuterMargin = (PCOORD)(0.375*PAGE_RESOLUTION);
#endif

	PCOORD grid_width = (PageWidth-(InnerMargin+OuterMargin))/nAcross;
	PCOORD grid_height = (PageHeight-(PictureTop+BottomMargin))/nDown;
	PCOORD step_width = scale_pcoord(grid_width, 9, 10);

	Style.SetDefault();
	SetStyleFace(Style, "Geneva");

#ifdef DOUBLE_NAMES
	PCOORD step_height = scale_pcoord(grid_height, 7, 10);
	PCOORD text_height = (grid_height - step_height)/2;
	short nBaseSize = (short)scale_pcoord(text_height, (72*6)/10, PAGE_RESOLUTION);
#else
	PCOORD step_height = scale_pcoord(grid_height, 9, 10);
	PCOORD text_height = grid_height - step_height;
	short nBaseSize = (short)scale_pcoord(text_height, (72*2)/3, PAGE_RESOLUTION);
#endif
	// Set the text style.
	if (nBaseSize > 12)
	{
		nBaseSize = 12;
	}

	Style.Size(MakeFixed(nBaseSize));
	Style.BaseSize(MakeFixed(nBaseSize));
	Style.UpdateFontMetrics();

	CTextStyle PageNumberStyle(pDatabase);
	PageNumberStyle.SetDefault();
#ifdef REVERSED_PAGE_NUMBER
	//SetStyleFace(PageNumberStyle, "GenevaNrw");
	SetStyleFace(PageNumberStyle, "Geneva");
	PageNumberStyle.Size(MakeFixed(7));
	PageNumberStyle.BaseSize(MakeFixed(7));
	PageNumberStyle.Bold(TRUE);
	PageNumberStyle.FillForegroundColor(COLOR_WHITE);
	PageNumberStyle.OutlineForegroundColor(COLOR_WHITE);
#else
	SetStyleFace(PageNumberStyle, "Modern");
	PageNumberStyle.Size(MakeFixed(7));
	PageNumberStyle.BaseSize(MakeFixed(7));
	PageNumberStyle.Bold(TRUE);
	PageNumberStyle.FillForegroundColor(COLOR_BLACK);
	PageNumberStyle.OutlineForegroundColor(COLOR_BLACK);
#endif
	PageNumberStyle.UpdateFontMetrics();

	CTextStyle HeaderStyle(pDatabase);
	HeaderStyle.SetDefault();
	SetStyleFace(HeaderStyle, "Modern");
#ifdef LOCALIZE
    int nHeaderSize = GetPrivateProfileInt("Fonts", "HeaderSize", 7, "Printart.ini");
	HeaderStyle.Size(MakeFixed(nHeaderSize));
	HeaderStyle.BaseSize(MakeFixed(nHeaderSize));
#else
	HeaderStyle.Size(MakeFixed(7));
	HeaderStyle.BaseSize(MakeFixed(7));
#endif
	HeaderStyle.Bold(TRUE);
	HeaderStyle.UpdateFontMetrics();

	CTextStyle TitleStyle(pDatabase);
	TitleStyle.SetDefault();
	if(AGCraft)
		SetStyleFace(TitleStyle, "Arial");
	else
		SetStyleFace(TitleStyle, "Fantasy");
#ifdef LOCALIZE
	int nTitleSize = GetPrivateProfileInt("Fonts", "TitleSize", 30, "Printart.ini");
	TitleStyle.Size(MakeFixed(nTitleSize));
	TitleStyle.BaseSize(MakeFixed(nTitleSize));
#else
	TitleStyle.Size(MakeFixed(30));
	TitleStyle.BaseSize(MakeFixed(30));
#endif
	if(!AGCraft)
		TitleStyle.Italic(TRUE);
	TitleStyle.UpdateFontMetrics();

	CTextStyle SpecialTitleStyle(pDatabase);
	SpecialTitleStyle.SetDefault();
	SetStyleFace(SpecialTitleStyle, "Fantasy");
	SpecialTitleStyle.Size(MakeFixed(26));
	SpecialTitleStyle.BaseSize(MakeFixed(26));
	SpecialTitleStyle.Italic(TRUE);
	SpecialTitleStyle.UpdateFontMetrics();
	//
	// Figure out number of pages we will print.
	// This depends on where the catagory breaks are.
	//
	SHORT n = nAcross * nDown;

	CString csCurrentCategory;
	int32 num_pages = 0;
	pEntry = (PrintProjectEntry*)m_picture_list.first_element();
	int nCount;
	while (pEntry != NULL)
	{
      CString csPrintCategory = GetPrintCategory(pEntry->GetCategory());
		if (csCurrentCategory.Compare(csPrintCategory) != 0)
		{
			// If we had a category, then we are changing categories.
			if (!csCurrentCategory.IsEmpty())
			{
				// Changed category. Add in previous pictures.
				num_pages += (nCount + n - 1) / n;
			}

			// Setup for this category.
			csCurrentCategory = csPrintCategory;
			nCount = 0;
		}
		// Accumulate this item.
		nCount++;
      pEntry = (PrintProjectEntry*)pEntry->next_element();
	}
	// Add in the final count.
	num_pages += (nCount + n - 1) / n;

	int32 current_page = 0;
	// Run through all the graphics.
	ObjectListPtr list = pDoc->object_list();
	// Create the properties for our black squares.
	CObjectProperties BlackProperties(pDatabase);
	BlackProperties.SetFillPattern(FillFormatV2::Black);
	BlackProperties.SetFillForegroundColor(COLOR_BLACK);

	BOOL fBroken = FALSE;
	BOOL fFirstPage = TRUE;

	for (pEntry = (PrintProjectEntry*)m_picture_list.first_element();
							!fBroken && pEntry != NULL; )
	{
		// Get the current category and the print category.
		CString csPrintCategory = GetPrintCategory(pEntry->GetCategory());
		CString csHeaderCategory = GetPrintCategory(pEntry->GetCategory(), TRUE);
#ifdef LOCALIZE
		char HeaderString[256];
		char PrintString[256];
	// translate the US category name using the printart.ini file
 		GetPrivateProfileString("Category", csHeaderCategory, csHeaderCategory, (LPSTR) HeaderString, 256, "Printart.ini");
		CharUpper(HeaderString);
		csHeaderCategory = HeaderString;
 		GetPrivateProfileString("Category", csPrintCategory, csPrintCategory, (LPSTR) PrintString, 256, "Printart.ini");
		csPrintCategory = PrintString;

#else
		csHeaderCategory.MakeUpper();
#endif
		BOOL fSpecial = (csPrintCategory.Find('/') != -1);		// Photographs
		// Tell the user what we are doing.
      printer_update_name(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_BUILDING_PAGE), (VOIDPTR)&pc);
	   // Make sure the current page is blank.
		list->destroy_objects();
	   // Build the next page.
		BeginWaitCursor();
		
		CString csPageNumber = pdlg->m_csManualPage;
		//
		// Build the adornment on the top.
		//
		BOOL fRightPage = (pdlg->GetManualPage() & 1);
		if (fRightPage)
		{
			// Right page.
			RectangleObject* pRectangle;
			// Create a square for the top line.
			if ((pRectangle = pDatabase->create_rectangle_object()) != NULL)
			{
				PBOX Bound;
				Bound.x0 = InnerMargin-DeviceLeftMargin+PageXOffset;
				Bound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
				Bound.x1 = (PageWidth-OuterMargin)-DeviceLeftMargin+PageXOffset;
				if(AGCraft)
					Bound.y1 = Bound.y0+(PAGE_RESOLUTION*1)/72;		// 1 pt line.
				else
					Bound.y1 = Bound.y0+(PAGE_RESOLUTION*2)/72;		// 2 pt line.
				pRectangle->set_bound(Bound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}

			PBOX PageNumberBound;
			PageNumberBound.x1 = (PageWidth-OuterMargin)-DeviceLeftMargin+PageXOffset;
			PageNumberBound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
			PageNumberBound.x0 = PageNumberBound.x1-(PCOORD)((0.26*PAGE_RESOLUTION) + PN_BOX_BY_LENGTH());
			PageNumberBound.y1 = PageNumberBound.y0+(PCOORD)(0.26*PAGE_RESOLUTION);

#ifdef REVERSED_PAGE_NUMBER
			// Create a square for the box behind the page number.
			if ((pRectangle = pDatabase->create_rectangle_object()) != NULL)
			{
				pRectangle->set_bound(PageNumberBound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}
#endif
			// Create a text box for the page number.
			FRAMEOBJ_CREATE_STRUCT fcs;

			fcs.bound = PageNumberBound;
			//fcs.alignment = ALIGN_center;
			fcs.alignment = ALIGN_right;
			fcs.vert_alignment = ALIGN_middle;
			fcs.flags = 0;
			fcs.pStyle = NULL;

			CFrameObject* pFrame;
			if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
			{
				int nCharacters = csPageNumber.GetLength();
				if (nCharacters != 0)
				{
					CTxp Txp(pDatabase);
					Txp.Init(pFrame, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);
					Txp.Style(PageNumberStyle);
					Txp.InsertString(csPageNumber);
				}

				pDoc->append_object(pFrame);
			}

			if (!(fFirstPage && fSpecial))
			{
				// Create a text box for the header (next to the page number).
				//fcs.bound.x0 = (PCOORD)(4.0*PAGE_RESOLUTION)-DeviceLeftMargin+PageXOffset;
				fcs.bound.x0 = (PCOORD)(4.0*PAGE_RESOLUTION)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
				fcs.bound.x1 = (PageWidth-(PCOORD)(0.30*PAGE_RESOLUTION)-OuterMargin)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y1 = fcs.bound.y0+(PCOORD)(0.26*PAGE_RESOLUTION);
				
				fcs.bound.x0 -= (PCOORD)PN_BOX_BY_LENGTH();
				fcs.bound.x1 -= (PCOORD)PN_BOX_BY_LENGTH();

				fcs.alignment = ALIGN_right;
				fcs.vert_alignment = ALIGN_middle;
				fcs.flags = 0;
				fcs.pStyle = NULL;

				if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
				{
					if (!csHeaderCategory.IsEmpty())
					{
						CTxp Txp(pDatabase);
						Txp.Init(pFrame, 0);
						Txp.SetHorizontalAlignment(fcs.alignment);
						Txp.Style(HeaderStyle);
						Txp.InsertString(csHeaderCategory);
					}

					pDoc->append_object(pFrame);
				}
			}
			// If this is the first page, print the bigger title as well.
			if (fFirstPage)
			{
				// Create a text box for the header (next to the page number).
				fcs.bound.x0 = InnerMargin-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = (PCOORD)(0.43*PAGE_RESOLUTION)-DeviceTopMargin+PageYOffset;
				fcs.bound.x1 = (PCOORD)(6.5*PAGE_RESOLUTION)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y1 = fcs.bound.y0+(PCOORD)(0.6*PAGE_RESOLUTION);
				fcs.alignment = ALIGN_left;
				fcs.vert_alignment = ALIGN_top;
				fcs.flags = 0;
				fcs.pStyle = NULL;

				if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
				{
					if (!csPrintCategory.IsEmpty())
					{
						CTxp Txp(pDatabase);
						Txp.Init(pFrame, 0);
						Txp.SetHorizontalAlignment(fcs.alignment);
						Txp.Style(fSpecial ? SpecialTitleStyle : TitleStyle);
						Txp.InsertString(csPrintCategory);
					}

					pDoc->append_object(pFrame);
				}
			}
		}
		else
		{
			// Left page.
			RectangleObject* pRectangle;
			// Create a square for the top line.
			if ((pRectangle = pDatabase->create_rectangle_object()) != NULL)
			{
				PBOX Bound;
				Bound.x0 = OuterMargin-DeviceLeftMargin+PageXOffset;
				Bound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
				Bound.x1 = (PageWidth-InnerMargin)-DeviceLeftMargin+PageXOffset;
				if(AGCraft)
					Bound.y1 = Bound.y0+(PAGE_RESOLUTION*1)/72;		// 1 pt line.
				else
					Bound.y1 = Bound.y0+(PAGE_RESOLUTION*2)/72;		// 2 pt line.
				pRectangle->set_bound(Bound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}

			PBOX PageNumberBound;
			PageNumberBound.x0 = OuterMargin-DeviceLeftMargin+PageXOffset;
			PageNumberBound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
			PageNumberBound.x1 = PageNumberBound.x0+(PCOORD)((0.26*PAGE_RESOLUTION) + PN_BOX_BY_LENGTH());
			PageNumberBound.y1 = PageNumberBound.y0+(PCOORD)(0.26*PAGE_RESOLUTION);

#ifdef REVERSED_PAGE_NUMBER
			// Create a square for the box behind the page number.
			if ((pRectangle = pDatabase->create_rectangle_object()) != NULL)
			{
				pRectangle->set_bound(PageNumberBound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}
#endif
			// Create a text box for the page number.
			FRAMEOBJ_CREATE_STRUCT fcs;

			fcs.bound = PageNumberBound;
			//fcs.alignment = ALIGN_center;
			fcs.alignment = ALIGN_left;
			fcs.vert_alignment = ALIGN_middle;
			fcs.flags = 0;
			fcs.pStyle = NULL;

			CFrameObject* pFrame;
			if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
			{
				int nCharacters = csPageNumber.GetLength();
				if (nCharacters != 0)
				{
					CTxp Txp(pDatabase);
					Txp.Init(pFrame, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);
					Txp.Style(PageNumberStyle);
					Txp.InsertString(csPageNumber);
				}

				pDoc->append_object(pFrame);
			}

			if (!(fFirstPage && fSpecial))
			{
				// Create a text box for the header (next to the page number).
				fcs.bound.x0 = (OuterMargin+(PCOORD)(0.30*PAGE_RESOLUTION))-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
				fcs.bound.x1 = (PageWidth-(PCOORD)(4*PAGE_RESOLUTION))-DeviceLeftMargin+PageXOffset;
				fcs.bound.y1 = fcs.bound.y0+(PCOORD)(0.26*PAGE_RESOLUTION);
				
				fcs.bound.x0 += (PCOORD)PN_BOX_BY_LENGTH();
				fcs.bound.x1 += (PCOORD)PN_BOX_BY_LENGTH();
				
				fcs.alignment = ALIGN_left;
				fcs.vert_alignment = ALIGN_middle;
				fcs.flags = 0;
				fcs.pStyle = NULL;

				if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
				{
					if (!csHeaderCategory.IsEmpty())
					{
						CTxp Txp(pDatabase);
						Txp.Init(pFrame, 0);
						Txp.SetHorizontalAlignment(fcs.alignment);
						Txp.Style(HeaderStyle);
						Txp.InsertString(csHeaderCategory);
					}

					pDoc->append_object(pFrame);
				}
			}
			// If this is the first page, print the bigger title as well.
			if (fFirstPage)
			{
				// Create a text box for the header (next to the page number).
				fcs.bound.x0 = PageWidth-(PCOORD)(6.5*PAGE_RESOLUTION)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = (PCOORD)(0.43*PAGE_RESOLUTION)-DeviceTopMargin+PageYOffset;
				fcs.bound.x1 = PageWidth-InnerMargin-DeviceLeftMargin+PageXOffset;
				fcs.bound.y1 = fcs.bound.y0+(PCOORD)(0.6*PAGE_RESOLUTION);
				fcs.alignment = ALIGN_right;
				fcs.vert_alignment = ALIGN_top;
				fcs.flags = 0;
				fcs.pStyle = NULL;

				if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
				{
					if (!csPrintCategory.IsEmpty())
					{
						CTxp Txp(pDatabase);
						Txp.Init(pFrame, 0);
						Txp.SetHorizontalAlignment(fcs.alignment);
						Txp.Style(fSpecial ? SpecialTitleStyle : TitleStyle);
						Txp.InsertString(csPrintCategory);
					}

					pDoc->append_object(pFrame);
				}
			}
		}

		SHORT		x, y;
		PCOORD	new_x;
		PBOX		pbox;

		for (pbox.y0 = PictureTop, y = 0;
						y < nDown && !fBroken;
						y++, pbox.y0 += grid_height)
		{
			pbox.y1 = pbox.y0 + step_height;

			for (pbox.x0 = fRightPage ? InnerMargin : OuterMargin, x = 0;
							x < nAcross;
							x++,
 								pbox.x0 += grid_width,
 								pEntry = (PrintProjectEntry*)pEntry->next_element())
			{
				CFrameObject* pFrame;
				PPNT p;
				// Make sure we are on a good node.
				if (pEntry == NULL)
				{
				/* All done! */
					y = nDown;
					break;
				}
				// We have a selected node. Make a graphic for it.
				pbox.x1 = pbox.x0 + step_width;
				// Create the graphic object.
				BOOL	GotThumbNail = FALSE;
				if(AGCraft)
				{
					TRY
					{
						CFileThumbnails* pFileThumbnails;
						if ((pFileThumbnails = StreamCache.GetThumbnailStream(pEntry->GetCollectionID())) != NULL)
						{
							CThumbnailDataEntry Thumbnail;
							if (pFileThumbnails->Find(pEntry->GetItemNumber(), &Thumbnail) == ERRORCODE_None)
							{
								DWORD dwDataSize = Thumbnail.GetItemSize();
								void* pData = NULL;
								
								if ((pData = Util::HugeAlloc(dwDataSize)) != NULL)
								{
									Thumbnail.Attach(pData, dwDataSize);

									if (pFileThumbnails->Read(dwDataSize) == ERRORCODE_None)
									{
										BOOL bTransparent;
										CRect crThumbnail;

										crThumbnail.SetRectEmpty();
										crThumbnail.right = Thumbnail.m_DataHeader.GetWidth();
										crThumbnail.bottom = Thumbnail.m_DataHeader.GetHeight();


										void* pThumbnailData = CBrowserDialog::MakeThumbnail(pEntry->GetCollectionID(), 
																												pEntry->GetItemNumber(),
																												crThumbnail, 
																												Thumbnail,
																												pData, 
																												&bTransparent);
										if (pThumbnailData != NULL)
										{
												
											ERRORCODE		e;
											GraphicObject	*pObject = NULL;

											e = CreateBMPFromDIB(pEntry->GetFriendlyName(),
																		pDatabase,
																		(BITMAPINFO*)pThumbnailData,
																		pObject);
											
											if (pObject != NULL)
											{
												if (pObject->original_dims(&p))
												{
													pDoc->append_object(pObject);
													
													PPNT dims;

													dims.x = (PCOORD)step_width;
													dims.y = (PCOORD)step_height;
													// Fit the graphic to its box.
													if ((new_x = scale_pcoord(dims.y, p.x, p.y)) > dims.x)
													{
														dims.y = scale_pcoord(dims.x, p.y, p.x);
													}
													else
													{
														dims.x = new_x;
													}
													// Set the bound.
													PBOX bound;

													bound.x0 = (pbox.x0 + pbox.x1 - dims.x)/2-DeviceLeftMargin+PageXOffset;
													bound.x1 = bound.x0 + dims.x;
													bound.y0 = (pbox.y0 + pbox.y1 - dims.y)/2-DeviceTopMargin+PageYOffset;
													bound.y1 = bound.y0 + dims.y;
													pObject->set_bound(bound);

													pObject->calc();

													GotThumbNail = TRUE;
												}
												else
												{
													delete pObject;
												}
											}
											Util::HugeFree(pThumbnailData);
										}
									}

									Thumbnail.Detach();
									Util::HugeFree(pData);
								}
							}
						}				
					}
					END_TRY
				}
				
				if (GotThumbNail == FALSE)
				{
					ProjectObject* object;
					PROJECTOBJECT_CREATE_STRUCT gcs;

					gcs.file_name = pEntry->GetName();
					gcs.friendly_name = pEntry->GetFriendlyName();

					PMGDatabasePtr const database = pDoc->get_database();
					if ((object = new ProjectObject((DB_OBJECT_TYPE)0, database)) == NULL)
					{
						fBroken = TRUE;
						break;
					}

					if ((object->create(&gcs, pDoc)) == ERRORCODE_None)
					{
					/* Get the size of the graphic. */
						if (object->original_dims(&p))
						{
							pDoc->append_object(object);

							PPNT dims;

							dims.x = (PCOORD)step_width;
							dims.y = (PCOORD)step_height;
							// Fit the graphic to its box.
							if ((new_x = scale_pcoord(dims.y, p.x, p.y)) > dims.x)
							{
								dims.y = scale_pcoord(dims.x, p.y, p.x);
							}
							else
							{
								dims.x = new_x;
							}
							// Set the bound.
							PBOX bound;

							bound.x0 = (pbox.x0 + pbox.x1 - dims.x)/2-DeviceLeftMargin+PageXOffset;
							bound.x1 = bound.x0 + dims.x;
							bound.y0 = (pbox.y0 + pbox.y1 - dims.y)/2-DeviceTopMargin+PageYOffset;
							bound.y1 = bound.y0 + dims.y;
							object->set_bound(bound);

							object->calc();
						}
						else
						{
							delete object;
							object = NULL;
						}
					}
				}
				// Add a text frame for the name.
				FRAMEOBJ_CREATE_STRUCT fcs;
				LPCSTR text = pEntry->GetFriendlyName();

				fcs.bound.x0 = pbox.x0-DeviceLeftMargin+PageXOffset;
				fcs.bound.x1 = pbox.x1-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = pbox.y1-DeviceTopMargin+PageYOffset;
				fcs.bound.y1 = fcs.bound.y0+text_height;

				fcs.alignment = ALIGN_center;
				fcs.vert_alignment = ALIGN_top;
				fcs.flags = 0;
				fcs.pStyle = NULL;

				if ((pFrame = pDoc->create_frame_object(&fcs)) == NULL)
				{
					fBroken = TRUE;
					break;
				}

				int nCharacters = strlen(text);
				if (nCharacters != 0)
				{
					CTxp Txp(pDatabase);
					Txp.Init(pFrame, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);
					Txp.Style(Style);
					Txp.InsertString(text);
				}

				pDoc->append_object(pFrame);
			}
		}

		EndWaitCursor();
	   /* See if we broke! */
		if (fBroken)
		{
			break;
		}
	   /* Print the page. */
		if (list->count() != 0)
		{
		   /* We have objects! Print them. */
			current_page++;
			pdlg->SetManualPage(pdlg->GetManualPage() + 1);
			fFirstPage = FALSE;

			if (bError = CPmwView::OnPrintDocument(pc, current_page, num_pages))
			{
				break;
			}
		}
	}

	return fBroken ? -1 : bError;
}
/////////////////////////////////////////////////////////////////////////////
// CProjectPrintDialog dialog
/*
CProjectPrintDialog::CProjectPrintDialog(CPmwDoc *pDoc)
	: CPmwPrint(NULL, pDoc, CProjectPrintDialog::IDD)
{
	//{{AFX_DATA_INIT(CProjectPrintDialog)
	m_per_page_index = 0;
	m_x_count = 1;
	m_y_count = 1;
   m_selPrintRange = 0;
	//}}AFX_DATA_INIT
}
  
void CProjectPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectPrintDialog)
	DDX_Radio(pDX, IDC_CATALOG_1x2, m_per_page_index);
   DDX_Radio(pDX, IDC_ART_PRINT_SEL, m_selPrintRange);
	DDX_Text(pDX, IDC_CATALOG_X, m_x_count);
	DDV_MinMaxInt(pDX, m_x_count, 1, 8);
	DDX_Text(pDX, IDC_CATALOG_Y, m_y_count);
	DDV_MinMaxInt(pDX, m_y_count, 1, 8);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjectPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CProjectPrintDialog)
	ON_BN_CLICKED(IDC_CATALOG_CUSTOM, OnClickedCatalogCustom)
	ON_EN_CHANGE(IDC_CATALOG_X, OnChangeCustom)
	ON_BN_CLICKED(IDC_CATALOG_1x2, OnClickedPerPage)
	ON_EN_CHANGE(IDC_CATALOG_Y, OnChangeCustom)
	ON_BN_CLICKED(IDC_CATALOG_2x2, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_2x3, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_3x3, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_3x4, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_4x5, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_6x8, OnClickedPerPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectPrintDialog message handlers

void CProjectPrintDialog::OnClickedCatalogCustom()
{
	GotoDlgCtrl(GetDlgItem(IDC_CATALOG_X));
}

void CProjectPrintDialog::OnChangeCustom()
{
	if (GetCheckedRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM) != IDC_CATALOG_CUSTOM)
	{
		CWnd *focused = GetFocus();

		if (focused != NULL)
		{
			int id = focused->GetDlgCtrlID();

			if (id == IDC_CATALOG_X || id == IDC_CATALOG_Y)
			{
				CheckRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM, IDC_CATALOG_CUSTOM);
			}
		}
	}
}

void CProjectPrintDialog::update_custom_values()
{
	int checked = GetCheckedRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM);

	if (checked != 0)
	{
		char buffer[64];

		GetDlgItemText(checked, buffer, sizeof(buffer)-1);
		char *p;

		if ((p = strchr(buffer, '(')) != NULL)
		{
			int x, y;

			if (sscanf(p, "(%dx%d)", &x, &y) == 2)
			{
				SetDlgItemInt(IDC_CATALOG_X, x);
				SetDlgItemInt(IDC_CATALOG_Y, y);
			}
		}
	}
}

void CProjectPrintDialog::OnClickedPerPage()
{
	update_custom_values();
}

BOOL CProjectPrintDialog::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(IDC_CATALOG_X)) != NULL)
	{
		edit->LimitText(1);
	}
	if ((edit = (CEdit *)GetDlgItem(IDC_CATALOG_Y)) != NULL)
	{
		edit->LimitText(1);
	}

	update_custom_values();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
*/
/////////////////////////////////////////////////////////////////////////////
// CProjectPrintView

CProjectPrintView::CProjectPrintView(List &picture_list, LPSTR chosen_dir)
		: m_picture_list(picture_list), m_chosen_dir(chosen_dir)
{
}

CProjectPrintView::~CProjectPrintView()
{
}


BEGIN_MESSAGE_MAP(CProjectPrintView, CPmwView)
	//{{AFX_MSG_MAP(CProjectPrintView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectPrintView message handlers

