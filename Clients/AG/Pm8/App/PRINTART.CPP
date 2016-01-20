/*
// $Workfile: PRINTART.CPP $
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
// $Log: /PM8/App/PRINTART.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 11    12/23/98 1:51p Psasse
// Code clean up
// 
// 10    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 9     8/18/98 2:30p Johno
// Added GetManualPage() and SetManualPage() - allows for text in manual
// page string
// 
// 8     8/13/98 2:29p Cboggio
// If title font size is set to 0 in the printart.ini file then no title
// will be shown
// 
// 7     8/11/98 12:24p Johno
// Changes for CProjectPrintView::OnManualPrintDocument()
// 
// 6     8/10/98 6:28p Johno
// GetPrintCategory() no longer static for use in
// CProjectPrintView::OnManualPrintDocument()
// 
// 5     8/05/98 9:51a Cboggio
// Change Image Indexi printing function for localization: Added
// flexibility and ability to display multi-lingual category names if
// required.
// 
// 4     8/03/98 4:18p Hforman
// fixed printing of PMO graphics
// 
// 3     5/22/98 11:18a Fredf
// Ported 4.0 code over to print image index.
// 
//    Rev 1.7   03 Oct 1997 15:44:24   Jay
// Variations in manual print code.
// 
//    Rev 1.6   24 Sep 1997 16:11:16   Fred
// Centers 7 x 9 page in 8.5 x 11
// 
//    Rev 1.5   24 Sep 1997 10:48:58   Fred
// 3/4 inch inner margin
// 
//    Rev 1.4   24 Sep 1997 09:08:28   Fred
// 7 by 9 manual printing
// 
//    Rev 1.3   02 Sep 1997 17:36:30   Jay
// New manual print support
// 
// 2     4/15/98 5:58p Hforman
// remove unused include
// 
//    Rev 1.0   14 Aug 1997 15:24:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:04   Fred
// Initial revision.
// 
//    Rev 1.2   05 Jun 1997 17:11:20   doberthur
// 
// 
//    Rev 1.1   23 May 1997 14:12:32   doberthur
// 
// 
//    Rev 1.0   22 May 1997 10:59:50   doberthur
// Initial revision.
// 
*/

// printart.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#include <string.h>
#include <direct.h>

#include "pmwdoc.h"
#include "pmwview.h"

#include "pmwprint.h"

#include "objlist.h"
#include "printart.h"

#include "utils.h"
#include "textflow.h"
#include "ctxp.h"

#include "pararec.h"
#include "framerec.h"
#include "frameobj.h"
#include "grafobj.h"
#include "patobj.h"

#include "typeface.h"
#include "pmgfont.h"
#include "rectobj.h"
#include "sortarry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PrintArtEntry structure

PrintArtEntry::PrintArtEntry(LPCSTR pszName, LPCSTR pszFriendlyName, LPCSTR pszCategory /*=NULL*/)
{
	SetName(pszName);
   SetFriendlyName(pszFriendlyName);
	SetCategory(pszCategory);
}

PrintArtEntry::~PrintArtEntry()
{
}

/////////////////////////////////////////////////////////////////////////////
// CArtPrintView

SHORT CArtPrintView::OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments)
{
	CArtPrintDialog *pdlg = (CArtPrintDialog*)pc.m_pPD;

	if (!pdlg->m_csManualPage.IsEmpty())
	{
		return OnManualPrintDocument(pc);
	}

	CPmwDoc *pDoc = GetDocument();

	SHORT bError = FALSE;

	SHORT x, y;
	PCOORD new_x;
	PBOX pbox;
	BOOL broken;
	int32 num_pages;

   /* Compute the size of the grid and the step. */

	PPNT dims = pDoc->get_dimensions();
	PMGDatabasePtr const database = pDoc->get_database();
	CTextStyle Style(database);

	SHORT const across = pdlg->m_x_count;
	SHORT const down = pdlg->m_y_count;

	PCOORD grid_width = dims.x/across;
	PCOORD grid_height = dims.y/down;
	PCOORD step_width = scale_pcoord(grid_width, 9, 10);

	Style.SetDefault();

#ifdef DOUBLE_NAMES
	PCOORD step_height = scale_pcoord(grid_height, 7, 10);
	PCOORD text_height = (grid_height - step_height)/2;
	short nBaseSize = (short)scale_pcoord(text_height, (72*6)/10, PAGE_RESOLUTION);
#else
	PCOORD step_height = scale_pcoord(grid_height, 9, 10);
	PCOORD text_height = grid_height - step_height;
	short nBaseSize = (short)scale_pcoord(text_height, (72*2)/3, PAGE_RESOLUTION);
#endif

   /* Set the text style. */
	if (nBaseSize > 12)
	{
		nBaseSize = 12;
	}

	Style.Size(MakeFixed(nBaseSize));
	Style.BaseSize(MakeFixed(nBaseSize));
#ifdef DOUBLE_NAMES
	Style.Bold(TRUE);
#endif

	Style.UpdateFontMetrics();

   /* Figure out number of pages we will print. */
	num_pages = m_picture_list.count();

	SHORT n = across * down;
	num_pages = (num_pages + n - 1) / n;

	int32 current_page = 0;

   /* Run through all the graphics. */

	ObjectListPtr list = pDoc->object_list();
	PrintArtEntryPtr entry;

	for (broken = FALSE, entry = (PrintArtEntryPtr)m_picture_list.first_element();
							!broken && entry != NULL; )
	{
	   /* Tell the user what we're doing. */
      printer_update_name(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_BUILDING_PAGE), (VOIDPTR)&pc);

	   /* Make sure the current page is blank. */
		list->destroy_objects();

	   /* Build the next page. */
		BeginWaitCursor();

		for (pbox.y0 = 0, y = 0;
						y < down && !broken;
						y++, pbox.y0 += grid_height)
		{
			pbox.y1 = pbox.y0 + step_height;

			for (pbox.x0 = 0, x = 0;
							x < across;
							x++,
 								pbox.x0 += grid_width,
 								entry = (PrintArtEntryPtr)entry->next_element())
			{
				CFrameObject* pFrame;

			/* Make sure we are on a good node. */
				if (entry == NULL)
				{
				/* All done! */
					y = down;
					break;
				}

			/* We have a selected node. Make a graphic for it. */
				pbox.x1 = pbox.x0 + step_width;

				// Create the graphic object.
				GRAPHIC_CREATE_STRUCT* pGCS = pDoc->get_gcs();

				pDoc->SetImportPictureName(entry->GetName(), entry->GetFriendlyName());
				pGCS->proof = TRUE;
				pGCS->embed_source = TRUE;
				pGCS->pSourceDevice = NULL;

				ERRORCODE nRetVal;
				if ((nRetVal = pDoc->create_chosen_picture()) == ERRORCODE_None)
				{
					/* Get the size of the graphic. */
					PBOX Bound;
					pDoc->GetSelectBound(&Bound);

					dims.x = (PCOORD)step_width;
					dims.y = (PCOORD)step_height;

					/* Fit the graphic to its box. */
					PPNT p;
					p.x = Bound.Width();
					p.y = Bound.Height();
					if ((new_x = scale_pcoord(dims.y, p.x, p.y)) > dims.x)
						dims.y = scale_pcoord(dims.x, p.y, p.x);
					else
						dims.x = new_x;

					/* Set the bound. */
					PBOX newBound;
					newBound.x0 = (pbox.x0 + pbox.x1 - dims.x)/2;
					newBound.x1 = newBound.x0 + dims.x;
					newBound.y0 = (pbox.y0 + pbox.y1 - dims.y)/2;
					newBound.y1 = newBound.y0 + dims.y;
					
					// If PMO, we may have multiple objects
					PMGPageObjectPtr pObject = pDoc->selected_objects();
					while (pObject != NULL)
					{
						PBOX unrotbound = ((RectPageObject*)pObject)->get_unrotated_bound();
						unrotbound.x0 -= Bound.x0;
						unrotbound.x1 -= Bound.x0;
						unrotbound.y0 -= Bound.y0;
						unrotbound.y1 -= Bound.y0;

						unrotbound.x0 = scale_pcoord(unrotbound.x0, dims.x, p.x);
						unrotbound.x1 = scale_pcoord(unrotbound.x1, dims.x, p.x);
						unrotbound.y0 = scale_pcoord(unrotbound.y0, dims.y, p.y);
						unrotbound.y1 = scale_pcoord(unrotbound.y1, dims.y, p.y);

						unrotbound.x0 += newBound.x0;
						unrotbound.x1 += newBound.x0;
						unrotbound.y0 += newBound.y0;
						unrotbound.y1 += newBound.y0;

						((RectPageObject*)pObject)->set_unrotated_bound(unrotbound);
						pObject->calc();

						pObject = pObject->next_selected_object();
					}

					pDoc->deselect_all();
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

#ifdef DOUBLE_NAMES

				fcs.bound.y1 = fcs.bound.y0+text_height*2;

				CString name;
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
				if (nRetVal != ERRORCODE_None)
				{
					blanket = TRUE;
				}
				else
				{
					name = entry->GetFriendlyName();
					if (stricmp(name, ptr) == 0)
						blanket = TRUE;
					else if (name.IsEmpty())
						name = ptr;
				}

				if (blanket)
				{
					/* Blank it. */
					name = " ";
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
			}
		}

		EndWaitCursor();

	   /* See if we broke! */
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
	}

	return broken ? -1 : bError;
}

void CArtPrintView::SetStyleFace(CTextStyle& Style, LPCSTR pszFace)
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

CString GetPrintCategory(LPCSTR pszCategory, BOOL fHeader)
{
	CMlsStringArray csaCategory(pszCategory, '/');
	if (csaCategory.GetAt(0).CompareNoCase("Photographs") == 0)
	{
		if (fHeader)
		{
			// Return full string for photographs.
			return pszCategory;
		}
		else
		{
			// Return subcategory for photographs.
			return pszCategory;
//			return csaCategory.GetAt(0) + '/' + csaCategory.GetAt(1);
//			return csaCategory.GetAt(1);
		}
	}
	return csaCategory.GetAt(0);
}

// Special variant of the print document function.
// This separates out the special manual printing code (duplication to avoid
// a bunch of "if"s).

SHORT CArtPrintView::OnManualPrintDocument(PrintContext& pc)
{
	PrintArtEntry* pEntry;

	CPmwDoc *pDoc = GetDocument();

	SHORT bError = FALSE;

	SHORT x, y;
	PCOORD new_x;
	PBOX pbox;

	CArtPrintDialog *pdlg = (CArtPrintDialog *)pc.m_pPD;

	int nPage = atoi(pdlg->m_csManualPage);

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
	int const nAcross = 9;
	int const nDown = 12;

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
	SetStyleFace(PageNumberStyle, "GenevaNrw");
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
	SetStyleFace(TitleStyle, "Fantasy");
#ifdef LOCALIZE
    int nTitleSize = GetPrivateProfileInt("Fonts", "TitleSize", 30, "Printart.ini");
	TitleStyle.Size(MakeFixed(nTitleSize));
	TitleStyle.BaseSize(MakeFixed(nTitleSize));
#else
	TitleStyle.Size(MakeFixed(30));
	TitleStyle.BaseSize(MakeFixed(30));
#endif
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
	pEntry = (PrintArtEntryPtr)m_picture_list.first_element();
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
      pEntry = (PrintArtEntryPtr)pEntry->next_element();
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

	for (pEntry = (PrintArtEntryPtr)m_picture_list.first_element();
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

		if (nTitleSize > 0)
			csPrintCategory = PrintString;
		else
			csPrintCategory = "";

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

		//
		// Build the adornment on the top.
		//

		BOOL fRightPage = (nPage & 1);
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
				Bound.y1 = Bound.y0+(PAGE_RESOLUTION*2)/72;		// 2 pt line.
				pRectangle->set_bound(Bound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}

			PBOX PageNumberBound;
			PageNumberBound.x1 = (PageWidth-OuterMargin)-DeviceLeftMargin+PageXOffset;
			PageNumberBound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
			PageNumberBound.x0 = PageNumberBound.x1-(PCOORD)(0.26*PAGE_RESOLUTION);
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
			fcs.alignment = ALIGN_center;
			fcs.vert_alignment = ALIGN_middle;
			fcs.flags = 0;
			fcs.pStyle = NULL;

			CFrameObject* pFrame;
			if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
			{
				CString csPageNumber;
				csPageNumber.Format("%d", nPage);

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
				fcs.bound.x0 = (PCOORD)(4.0*PAGE_RESOLUTION)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
				fcs.bound.x1 = (PageWidth-(PCOORD)(0.30*PAGE_RESOLUTION)-OuterMargin)-DeviceLeftMargin+PageXOffset;
				fcs.bound.y1 = fcs.bound.y0+(PCOORD)(0.26*PAGE_RESOLUTION);
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
				Bound.y1 = Bound.y0+(PAGE_RESOLUTION*2)/72;		// 2 pt line.
				pRectangle->set_bound(Bound);
				pRectangle->SetObjectProperties(BlackProperties);
				pDoc->append_object(pRectangle);
			}

			PBOX PageNumberBound;
			PageNumberBound.x0 = OuterMargin-DeviceLeftMargin+PageXOffset;
			PageNumberBound.y0 = TopMargin-DeviceTopMargin+PageYOffset;
			PageNumberBound.x1 = PageNumberBound.x0+(PCOORD)(0.26*PAGE_RESOLUTION);
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
			fcs.alignment = ALIGN_center;
			fcs.vert_alignment = ALIGN_middle;
			fcs.flags = 0;
			fcs.pStyle = NULL;

			CFrameObject* pFrame;
			if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
			{
				CString csPageNumber;
				csPageNumber.Format("%d", nPage);

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

		for (pbox.y0 = PictureTop, y = 0;
						y < nDown && !fBroken;
						y++, pbox.y0 += grid_height)
		{
			pbox.y1 = pbox.y0 + step_height;

			for (pbox.x0 = fRightPage ? InnerMargin : OuterMargin, x = 0;
							x < nAcross;
							x++,
 								pbox.x0 += grid_width,
 								pEntry = (PrintArtEntryPtr)pEntry->next_element())
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
				GraphicObjectPtr object;
				GRAPHIC_CREATE_STRUCT gcs;

				gcs.m_csFileName = pEntry->GetName();
            gcs.m_csFriendlyName = pEntry->GetFriendlyName();

				if ((object = pDatabase->create_graphic_object(&gcs)) != NULL)
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
			nPage++;
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
// CArtPrintDialog dialog

CArtPrintDialog::CArtPrintDialog(CPmwDoc *pDoc, BOOL fManualPrint /*=FALSE*/)
	: CPmwPrint(NULL, pDoc, CArtPrintDialog::IDD)
{
	//{{AFX_DATA_INIT(CArtPrintDialog)
	m_per_page_index = 0;
	m_x_count = 1;
	m_y_count = 1;
   m_selPrintRange = 0;
	//}}AFX_DATA_INIT
	m_csManualPage = _T("");
	m_fManualPrint = fManualPrint;
}
  
void CArtPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArtPrintDialog)
	DDX_Radio(pDX, IDC_CATALOG_1x2, m_per_page_index);
	DDX_Text(pDX, IDC_CATALOG_X, m_x_count);
	DDV_MinMaxInt(pDX, m_x_count, 1, 8);
	DDX_Text(pDX, IDC_CATALOG_Y, m_y_count);
	DDV_MinMaxInt(pDX, m_y_count, 1, 8);
   DDX_Radio(pDX, IDC_ART_PRINT_SEL, m_selPrintRange);
	//}}AFX_DATA_MAP
	if (m_fManualPrint)
	{
		DDX_Text(pDX, IDC_MANUAL_PAGE, m_csManualPage);
	}
}

BEGIN_MESSAGE_MAP(CArtPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CArtPrintDialog)
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
// CArtPrintDialog message handlers

void CArtPrintDialog::OnClickedCatalogCustom()
{
	GotoDlgCtrl(GetDlgItem(IDC_CATALOG_X));
}

void CArtPrintDialog::OnChangeCustom()
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

void CArtPrintDialog::update_custom_values()
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

void CArtPrintDialog::OnClickedPerPage()
{
	update_custom_values();
}

BOOL CArtPrintDialog::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	if (m_fManualPrint)
	{
		GetDlgItem(IDC_MANUAL_PAGE_TEXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MANUAL_PAGE)->ShowWindow(SW_SHOW);
	}

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

int
CArtPrintDialog::GetManualPage(void)
{
	// Find the first number in the string
	for (int i = 0; i < m_csManualPage.GetLength(); i ++)
	{
		if ((m_csManualPage[i] >= '0') && (m_csManualPage[i] <= '9'))
		{
			LPCSTR	p = (LPCSTR)m_csManualPage;
			return atoi(&p[i]);
		}
	}
	return 0;
}

void
CArtPrintDialog::SetManualPage(int Page)
{
	int		i;
	CString	Pre;
	// Collect string before number
	for (i = 0; i < m_csManualPage.GetLength(); i ++)
	{
		if ((m_csManualPage[i] >= '0') && (m_csManualPage[i] <= '9'))
			break;
		Pre += m_csManualPage[i];
	}
	// Skip over number
	for (i; i < m_csManualPage.GetLength(); i ++)
	{
		if ((m_csManualPage[i] < '0') || (m_csManualPage[i] > '9'))
			break;
	}

	Pre += "%d"; 
	// Collsect string after number
	for (i; i < m_csManualPage.GetLength(); i ++)
		Pre += m_csManualPage[i];

	m_csManualPage.Format(Pre, Page);
}

/////////////////////////////////////////////////////////////////////////////
// CArtPrintView

CArtPrintView::CArtPrintView(List &picture_list, LPSTR chosen_dir)
		: m_picture_list(picture_list), m_chosen_dir(chosen_dir)
{
}

CArtPrintView::~CArtPrintView()
{
}


BEGIN_MESSAGE_MAP(CArtPrintView, CPmwView)
	//{{AFX_MSG_MAP(CArtPrintView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArtPrintView message handlers
/////////////////////////////////////////////////////////////////////////////
// CArtPrintDoc

IMPLEMENT_SERIAL(CArtPrintDoc, CPmwDoc, 0 /* schema number*/ )

CArtPrintDoc::CArtPrintDoc()
{
}

CArtPrintDoc::~CArtPrintDoc()
{
}

BEGIN_MESSAGE_MAP(CArtPrintDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CArtPrintDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

VOID CArtPrintDoc::add_hidden_view(CArtPrintView *pView)
{
	m_viewList.AddTail(pView);
	pView->set_document(this);
}

/////////////////////////////////////////////////////////////////////////////
// CArtPrintDoc commands

