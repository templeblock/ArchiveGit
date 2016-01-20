/*
// $Workfile: NEWSDOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWSDOC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 9     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 8     8/19/98 9:06a Mwilson
// added project info
// 
// 7     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 6     8/06/98 4:32p Mwilson
// added new craft project type
// 
// 5     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 4     3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 3     3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 2     3/10/98 5:24p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:22:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:22   Fred
// Initial revision.
// 
//    Rev 1.9   16 Jul 1997 10:14:58   Jay
// Don't memset fcs; it has a constructor
// 
//    Rev 1.8   03 Apr 1997 14:31:18   Jay
// Resource cleanup
// 
//    Rev 1.7   28 Mar 1997 17:10:48   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.6   28 Oct 1996 10:23:06   johno
// Moved strings to resource file
// 
//    Rev 1.5   08 Oct 1996 08:07:06   Fred
//  
// 
//    Rev 1.4   04 Oct 1996 10:44:24   Jay
// Improvements, etc.
// 
//    Rev 1.3   02 Oct 1996 08:28:52   Jay
// Picture reference newsletter builder.
// 
//    Rev 1.2   01 Sep 1996 14:04:30   Fred
// Next page when on last page brings up add pages dialog
// 
//    Rev 1.1   01 Sep 1996 13:44:32   Fred
// Always builds album thumbnail from first page
// 
//    Rev 1.0   23 Jul 1996 10:39:56   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "pmwdoc.h"
#include "newsdoc.h"
#include "pmwini.h"

#include "sortarry.h"
#include "typeface.h"
#include "objlist.h"
#include "frameobj.h"
#include "grafobj.h"
#include "patobj.h"
#include "pmgfont.h"
#include "typeface.h"
#include "ctxp.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewsletterDoc

IMPLEMENT_SERIAL(CNewsletterDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The newsletter project panel.
*/

static PANEL_INFO
newsletter_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

/*
// The newsletter project info.
*/

ProjectInfo CNewsletterDoc::StandardInfo =
{
	PROJECT_TYPE_Newsletter,
	0,
	PORTRAIT,
	sizeof(newsletter_panels)/sizeof(newsletter_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	newsletter_panels
};

CNewsletterDoc::CNewsletterDoc()
{
	info = StandardInfo;
}

CNewsletterDoc::~CNewsletterDoc()
{
}

BOOL CNewsletterDoc::OnNewDocument()
{
	if (!CPmwDoc::OnNewDocument())
		return FALSE;

	// Set the "sided" variable.
	document->SetSided(GET_PMWAPP()->last_new_info.fDoubleSided
							 ? DocumentRecord::DoubleSided
							 : DocumentRecord::SingleSided);

	if (!UpdateMasterPages())
	{
		return FALSE;
	}

#ifdef BUILD_PICTURE_NEWSLETTER
	// See if we want to make the special "picture" newsletter.
	if (GET_PMWAPP()->GetProfileInt(SECTION_Debug, "PictureNewsletter", 0))
	{
		BuildPictureNewsletter();
	}
#endif

	return TRUE;
}

BOOL CNewsletterDoc::OnOpenDocument(const char *pszPathName)
{
	if (!CPmwDoc::OnOpenDocument(pszPathName))
	{
		return FALSE;
	}

	if (!UpdateMasterPages())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CNewsletterDoc::UpdateMasterPages(void)
{
	// Fixup the "sided-ness" for the document before proceeding.
	if (document->GetSided() == DocumentRecord::NotSided)
	{
		document->SetSided(DocumentRecord::DoubleSided);
	}

	// Make sure we have two master pages to play with.
	ASSERT(document->GetSided() != DocumentRecord::NotSided);

	if (document->NumberOfMasterPages() == 0)
	{
		// We need to create two master pages.
		if (document->NewMasterPage() != ERRORCODE_None
			 || document->NewMasterPage() != ERRORCODE_None)
		{
			return FALSE;
		}
	}
	// We should have two master pages.
	ASSERT(document->NumberOfMasterPages() == 2);
	return TRUE;
}

BOOL CNewsletterDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{
	// Preview the first page.
	DWORD dwSavePage = CurrentPageIndex();
	DWORD dwSaveMaster = MasterPageIndex();
	GotoPage(0);
	BOOL fTerminated = CPmwDoc::DrawPreview(re, rc);
	if (dwSavePage != (DWORD)-1)
	{
		GotoPage(dwSavePage);
	}
	else if (dwSaveMaster != (DWORD)-1)
	{
		GotoMasterPage(dwSaveMaster);
	}

	return fTerminated;
}

CString CNewsletterDoc::GetPageString(DB_RECORD_NUMBER lPage /*=0*/)
{
	if (lPage == 0)
	{
		lPage = CurrentPageRecord();
	}

	CString csValue;
	// Generate a page index.
	if (EditingMasterPage() && lPage == CurrentPageRecord())
	{
		// Return a dummy page thing for the master page.
		csValue = "#";
	}
	else
	{
		// Compute a page number.
		DWORD dwIndex = document->IndexOfPage(lPage);
		if (dwIndex == (DWORD)-1)
		{
			// Not in the main page list (like a master page).
			// Use the current page number.
			dwIndex = document->IndexOfPage(CurrentPageRecord());
		}
		if (dwIndex != (DWORD)-1)
		{
			csValue.Format("%d", dwIndex+1);
		}
	}

	return csValue;
}

BEGIN_MESSAGE_MAP(CNewsletterDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CNewsletterDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsletterDoc commands

BOOL CNewsletterDoc::CanPageNext(void)
{
	if (EditingMasterPage())
	{
		return GetSided() == DocumentRecord::DoubleSided
			&& MasterPageIndex() < NumberOfMasterPages()-1;
	}
	else
	{
		// We allow the user to click off the last page since that
		// brings up the add pages dialog.
		return CurrentPageIndex() < NumberOfPages();
	}
}

void CNewsletterDoc::DoPageNext(void)
{
	if (EditingMasterPage())
	{
		DWORD dwIndex = MasterPageIndex();
		ASSERT(dwIndex < NumberOfMasterPages()-1);
		ASSERT(GetSided() == DocumentRecord::DoubleSided);
		GotoMasterPage(dwIndex+1);
	}
	else
	{
		DWORD dwCurrentPage = CurrentPageIndex();
		if (dwCurrentPage < NumberOfPages()-1)
		{
			LoadPage(GetPage(dwCurrentPage+1));
		}
		else
		{
			// Ask the user if they want to add some pages.
			Util::SendWmCommand(AfxGetMainWnd(), ID_ADD_PAGE, NULL, 0);
		}
	}
}

BOOL CNewsletterDoc::CanPageBack(void)
{
	if (EditingMasterPage())
	{
		return GetSided() == DocumentRecord::DoubleSided
					&& MasterPageIndex() > 0;
	}
	else
	{
		return CPmwDoc::CanPageBack();
	}
}

void CNewsletterDoc::DoPageBack(void)
{
	if (EditingMasterPage())
	{
		DWORD dwIndex = MasterPageIndex();
		ASSERT(dwIndex != 0);
		ASSERT(GetSided() == DocumentRecord::DoubleSided);
		GotoMasterPage(dwIndex-1);
	}
	else
	{
		CPmwDoc::DoPageBack();
	}
}

void CNewsletterDoc::GoToFirstPage()
{
	/* Used for templates only */

	ASSERT(NumberOfPages() > 0);
	LoadPage(GetPage(0));
}


/////////////////////////////////////////////////////////////////////////////
// Special "picture" newsletter code.
 
#ifdef BUILD_PICTURE_NEWSLETTER
void CNewsletterDoc::BuildPictureNewsletter(void)
{
	CStdioFile File;
	CString csName;
	CString csLine;
	CString csCategory;
	CString csBaseDir;
	CString csDir;
	CString csFileName;
	CString csFriendlyName;
	int nBasePage = 3;
	CMlsStringArray csaFields;
	CMlsStringArray csaSubfields;

	int nInclusionColumn = GET_PMWAPP()->GetProfileInt(SECTION_Debug, "PictureNewsletterColumn", 1);
	BOOL fBonusPack = (nInclusionColumn == 2);
	nInclusionColumn += 3;		// Base column

	PMGDatabase* pDatabase = get_database();
	PMGFontServer* pFontServer = (PMGFontServer*)get_font_server();
	int nPanel = get_current_panel();

	// Setup the label style.
	CTextStyle LabelStyle(pDatabase);

	LabelStyle.SetDefault();
	int nFace = typeface_server.find_face("GenevaNrw", FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			LabelStyle.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	LabelStyle.Size(MakeFixed(6.3));
	LabelStyle.BaseSize(MakeFixed(6.3));
	LabelStyle.UpdateFontMetrics();
	PCOORD LabelHeight = (LabelStyle.Ascend() + LabelStyle.Descend())*2;

	const int nAcross = fBonusPack ? 5 : 6;
	const int nDown = fBonusPack ? 5 : 7;
	const PCOORD StepAcross = (PAGE_RESOLUTION*33/32);
	const PCOORD StepDown = (PAGE_RESOLUTION*15/16) + LabelHeight;

	int nX, nY;

	GRAPHIC_CREATE_STRUCT gcs;
	gcs.proof = TRUE;

	FRAMEOBJ_CREATE_STRUCT fcs;

	PBOX PageBound;

	// Now do the build!
	TRY
	{
		// Build the base picture directory.
		csBaseDir = "[[H]]\\PICTURES";		// Leave unresolved (i.e. dynamic).

		// Build the name of the picture text file.
		csName = GetPathManager()->ExpandPath("MANPICT.TXT");

		// Open the picture text file.
		if (File.Open(csName, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
		{
			// Read the header line.
			if (File.ReadString(csLine))
			{
				// Read lines until done.
				while (File.ReadString(csLine))
				{
					// Skip empty lines.
					if (csLine.IsEmpty())
					{
						continue;
					}

					// Break the line into fields.
					csaFields.SetNames(csLine, '\t');

					// See if this line is included.
					if (csaFields.GetSize() <= nInclusionColumn
						 || !atoi(csaFields.GetAt(nInclusionColumn)))
					{
						// Not included. Skip it.
						continue;
					}

					// If the first field is non-empty, then this is a category line.
					if (csaFields.GetAt(0).IsEmpty())
					{
						// Another picture!
						// If we have run out of room on this page,
						// move to a new page.

						if (nX == 0 && nY == nDown)
						{
							// We need to add another page now!
							BuildPicturesPage(fBonusPack, csCategory, nBasePage, PageBound);

							// Start back at the upper left.
							nX = 0;
							nY = 0;
						}

						// Add the picture.

						Util::ConstructPath(csFileName, csDir, csaFields.GetAt(1));
						gcs.file_name = csFileName;
						csFriendlyName = csaFields.GetAt(2);
						Util::TrimQuotes(csFriendlyName);
						gcs.friendly_name = csFriendlyName;

						GraphicObjectPtr pGraphic;

						if ((pGraphic = pDatabase->create_graphic_object(&gcs)) != NULL)
						{
							pGraphic->set_panel(nPanel);

							// Compute full bounds.
							PBOX PictureBound;
							PictureBound.x0 = PageBound.x0 + nX*StepAcross;
							PictureBound.y0 = PageBound.y0 + (PAGE_RESOLUTION*6/8) + nY*StepDown;
							PictureBound.x1 = PictureBound.x0 + StepAcross;
							PictureBound.y1 = PictureBound.y0 + StepDown - LabelHeight;

							PPNT Dims;
							if (!pGraphic->original_dims(&Dims))
							{
								Dims.x = Dims.y = PAGE_RESOLUTION;
							}

							// Compute correct aspect bounds.
							PPNT FullDims;
							FullDims.x = PictureBound.x1 - PictureBound.x0 - PAGE_RESOLUTION/8;
							FullDims.y = PictureBound.y1 - PictureBound.y0;
							PPNT NewDims;
							if ((NewDims.y = scale_pcoord(FullDims.x, Dims.y, Dims.x)) <= FullDims.y)
							{
								// Use this!
								NewDims.x = FullDims.x;
							}
							else
							{
								NewDims.x = scale_pcoord(FullDims.y, Dims.x, Dims.y);
								ASSERT(NewDims.x <= FullDims.x);
								NewDims.y = FullDims.y;
							}

							PBOX Bound;
							Bound.x0 = (PictureBound.x0 + PictureBound.x1 - NewDims.x)/2;
							Bound.y0 = (PictureBound.y0 + PictureBound.y1 - NewDims.y)/2;
							Bound.x1 = Bound.x0 + NewDims.x;
							Bound.y1 = Bound.y0 + NewDims.y;

							pGraphic->set_bound(Bound);
							pGraphic->calc();

							append_object(pGraphic);

							// Add the text frame.
							fcs.bound.x0 = PictureBound.x0;
							fcs.bound.x1 = PictureBound.x1;
							fcs.bound.y0 = PictureBound.y1;
							fcs.bound.y1 = fcs.bound.y0 + LabelHeight;

							fcs.alignment = ALIGN_center;
							fcs.vert_alignment = ALIGN_top;
						//	fcs.flags = 0;
						//	fcs.pStyle = NULL;

							CFrameObject* pFrame;

							if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
							{
								append_object(pFrame);

								CTxp Txp(pDatabase);

								// Set its panel.
								pFrame->set_panel(nPanel);
								LabelStyle.Alignment(fcs.alignment);
								pFrame->SetParagraphStyle(&LabelStyle);

								TRY
								{
									Txp.Init(pFrame, 0);

									// Put in the first line of text.
									Txp.Style(LabelStyle);
									Txp.InsertString(gcs.friendly_name);

									pFrame->calc();
								}
								END_TRY
							}
						}

						if (++nX == nAcross)
						{
							nX = 0;
							nY++;
						}
					}
					else
					{
						// Another category!

						// Insert a page, if necessary, to start the category
						// on the right side.

						if ((NumberOfPages() & 1) != 0
								&& page_objects->count() != 0)
						{
							BuildPicturesPage(fBonusPack, csCategory, nBasePage, PageBound);
							// Add the "empty" text.
							BuildEmptyPictures();
						}

						// Break the first field into subfields.
						// Subfield 0: Category friendly name
						// Subfield 1: Category subdirectory

						csaSubfields.SetNames(csaFields.GetAt(0), '|');
						csCategory = csaSubfields.GetAt(0);
						Util::ConstructPath(csDir, csBaseDir, csaSubfields.GetAt(1));

						BuildPicturesPage(fBonusPack, csCategory, nBasePage, PageBound);

						nX = 0;
						nY = 0;
 					}
				}
			}
		}
		else
		{
			// Print the open failure error.
			CString csMessage;
			csMessage.Format("Unable to open file '%s' for reading.", csName);
			AfxMessageBox(csMessage);
		}
	}
	CATCH_ALL(e)
	{
		AfxMessageBox(IDS_EXCEPTION_READING_FILE);
	}
	END_CATCH_ALL
}

void CNewsletterDoc::BuildPicturesPage(BOOL fBonusPack, LPCSTR pszName, int nBasePage, PBOX& PageBound)
{
	DWORD dwPages = NumberOfPages();
	if (page_objects->count() != 0)
	{
		// We need to create a new page to play with.
		AddPage(dwPages);
		GotoPage(dwPages++);
	}

	// Assert that we have an empty page at this point.
	ASSERT(page_objects->count() == 0);

	// Change this to flip the left/right sense of pages.
	BOOL fLeft = (dwPages & 1) == 0;

	// Set the target printer margins.
	PBOX PrinterMargins;
	PrinterMargins.x0 = PAGE_RESOLUTION/4;
	PrinterMargins.y0 = PAGE_RESOLUTION/4;
	PrinterMargins.x1 = PAGE_RESOLUTION/4;
	PrinterMargins.y1 = PAGE_RESOLUTION/4;

	// Margins are non-symmetric depending on side.
	PCOORD LeftMargin;
	PCOORD TopMargin;
	PCOORD RightMargin;
	PCOORD BottomMargin;

	// If this is the Bonus Pack, bump the sides in.
	const PCOORD LeftPadding = PAGE_RESOLUTION*3/4;
	const PCOORD TopPadding = PAGE_RESOLUTION;
	const PCOORD RightPadding = PAGE_RESOLUTION*3/4;
	const PCOORD BottomPadding = PAGE_RESOLUTION;

	if (fBonusPack)
	{
		LeftMargin = fLeft ? (PAGE_RESOLUTION*3/4) : (PAGE_RESOLUTION*16/16);
		TopMargin = PAGE_RESOLUTION*13/16;
		RightMargin = fLeft ? (PAGE_RESOLUTION*16/16) : (PAGE_RESOLUTION*3/4);
		BottomMargin = PAGE_RESOLUTION*1/2;

		// Bump the sides in by the padding.
		LeftMargin += LeftPadding;
		TopMargin += TopPadding;
		RightMargin += RightPadding;
		BottomMargin += BottomPadding;
	}
	else
	{
		LeftMargin = fLeft ? (PAGE_RESOLUTION*16/16) : (PAGE_RESOLUTION*20/16);
		TopMargin = PAGE_RESOLUTION*13/16;
		RightMargin = fLeft ? (PAGE_RESOLUTION*20/16) : (PAGE_RESOLUTION*16/16);
		BottomMargin = PAGE_RESOLUTION*1/2;
	}

	// Compute the page margins to build to.
	PBOX PageMargins;
	PageMargins.x0 = LeftMargin - PrinterMargins.x0;
	PageMargins.y0 = TopMargin - PrinterMargins.y0;
	PageMargins.x1 = RightMargin - PrinterMargins.x1;
	PageMargins.y1 = BottomMargin - PrinterMargins.y1;

	PageBound.x0 = PageMargins.x0;
	PageBound.y0 = PageMargins.y0;
	PageBound.x1 = get_dimensions().x - PageMargins.x1;
	PageBound.y1 = get_dimensions().y - PageMargins.y1;

	//
	// Construct the objects on the page.
	//

	PMGDatabase* pDatabase = get_database();
	PMGFontServer* pFontServer = (PMGFontServer*)get_font_server();
	int nPanel = get_current_panel();

	CFrameObject* pFrame;

	// Construct the header.
	CTextStyle Style(pDatabase);
	Style.SetDefault();

	int nFace = typeface_server.find_face("Gourmand", FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	Style.Size(MakeFixed(20));
	Style.BaseSize(MakeFixed(20));
	Style.Bold(TRUE);
	Style.Italic(TRUE);
	Style.UpdateFontMetrics();

	// Add the text frame.
	FRAMEOBJ_CREATE_STRUCT fcs;
	memset(&fcs, 0, sizeof(fcs));

	fcs.bound.x0 = PageBound.x0;
	fcs.bound.x1 = PageBound.x1;
	fcs.bound.y0 = PageBound.y0;
	fcs.bound.y1 = fcs.bound.y0 + Style.Ascend() + Style.Descend();

	fcs.alignment = ALIGN_right;
	fcs.vert_alignment = ALIGN_top;
//	fcs.flags = 0;
//	fcs.pStyle = NULL;

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		append_object(pFrame);

		CTxp Txp(pDatabase);

		// Set its panel.
		pFrame->set_panel(nPanel);
		Style.Alignment(fcs.alignment);
		pFrame->SetParagraphStyle(&Style);

		TRY
		{
			Txp.Init(pFrame, 0);

			// Put in the first line of text.
			Txp.Style(Style);
			Txp.InsertString(pszName);

			pFrame->calc();
		}
		END_TRY
	}

	PBOX bound;
	bound.x0 = PageBound.x0;
	bound.x1 = PageBound.x1;
	bound.y0 = fcs.bound.y1;
	bound.y1 = bound.y0 + (PAGE_RESOLUTION*4/72);		// 4 pt line.
	CreatePattern(bound);

	// Construct the footer

	Style.Size(MakeFixed(10));
	Style.BaseSize(MakeFixed(10));
	Style.Bold(FALSE);
	Style.Italic(TRUE);
	Style.LeftMargin(MakeFixed(.4));
	Style.RightMargin(MakeFixed(.4));
	Style.UpdateFontMetrics();

	// Make the section description.
	fcs.bound.x0 = PageBound.x0;
	fcs.bound.x1 = PageBound.x1;
	fcs.bound.y1 = PageBound.y1;
	fcs.bound.y0 = fcs.bound.y1 - (Style.Ascend() + Style.Descend());

	fcs.alignment = fLeft ? ALIGN_left : ALIGN_right;
	fcs.vert_alignment = ALIGN_bottom;
//	fcs.flags = 0;
//	fcs.pStyle = NULL;

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		append_object(pFrame);

		CTxp Txp(pDatabase);

		// Set its panel.
		pFrame->set_panel(nPanel);

		Style.Alignment(fcs.alignment);
		pFrame->SetParagraphStyle(&Style);

		TRY
		{
			Txp.Init(pFrame, 0);
			Txp.SetHorizontalAlignment(fcs.alignment);

			// Put in the first line of text.
			Txp.Style(Style);
			Txp.InsertString("Appendix C:  Graphics");

			pFrame->calc();
		}
		END_TRY
	}

	// Make the page number.
	Style.SetDefault();

	nFace = typeface_server.find_face("Geneva", FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	Style.Size(MakeFixed(10));
	Style.BaseSize(MakeFixed(10));
	Style.Bold(TRUE);
	Style.UpdateFontMetrics();

	fcs.bound.x0 = PageBound.x0;
	fcs.bound.x1 = PageBound.x1;
	fcs.bound.y1 = PageBound.y1;
	fcs.bound.y0 = fcs.bound.y1 - (Style.Ascend() + Style.Descend());

	fcs.alignment = fLeft ? ALIGN_left : ALIGN_right;
	fcs.vert_alignment = ALIGN_bottom;
//	fcs.flags = 0;
//	fcs.pStyle = NULL;

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		append_object(pFrame);

		CTxp Txp(pDatabase);

		// Set its panel.
		pFrame->set_panel(nPanel);

		Style.Alignment(fcs.alignment);
		pFrame->SetParagraphStyle(&Style);

		CString csPage;

		TRY
		{
			Txp.Init(pFrame, 0);
			Txp.SetHorizontalAlignment(fcs.alignment);

			// Put in the first line of text.
			Txp.Style(Style);
			csPage.Format("C-%d", (int)(dwPages+nBasePage-1));
			Txp.InsertString(csPage);

			pFrame->calc();
		}
		END_TRY
	}

	bound.x0 = PageBound.x0;
	bound.x1 = PageBound.x1;
	bound.y1 = fcs.bound.y0;
	bound.y0 = bound.y1 -	1;		//(PAGE_RESOLUTION*1/72);		// 1 pt line.
	CreatePattern(bound);

	// If this is the Bonus Pack manual, add crop marks.
	//

	if (fBonusPack)
	{
		PBOX Margins;
		Margins.x0 = LeftPadding - PrinterMargins.x0;
		Margins.y0 = TopPadding - PrinterMargins.y0;
		Margins.x1 = RightPadding - PrinterMargins.x1;
		Margins.y1 = BottomPadding - PrinterMargins.y1;

		PBOX PrintBound;
		PrintBound.x0 = Margins.x0;
		PrintBound.y0 = Margins.y0;
		PrintBound.x1 = get_dimensions().x - Margins.x1;
		PrintBound.y1 = get_dimensions().y - Margins.y1;

		// Upper left
		// Horizontal
		bound.x1 = PrintBound.x0 - PAGE_RESOLUTION/8;
		bound.x0 = bound.x1 - PAGE_RESOLUTION/4;
		bound.y1 = PrintBound.y0;
		bound.y0 = bound.y1 - 1;
		CreatePattern(bound);
		// Vertical
		bound.x1 = PrintBound.x0;
		bound.x0 = bound.x1 - 1;
		bound.y1 = PrintBound.y0 - PAGE_RESOLUTION/8;
		bound.y0 = bound.y1 - PAGE_RESOLUTION/4;
		CreatePattern(bound);

		// Upper right
		// Horizontal
		bound.x0 = PrintBound.x1 + PAGE_RESOLUTION/8;
		bound.x1 = bound.x0 + PAGE_RESOLUTION/4;
		bound.y1 = PrintBound.y0;
		bound.y0 = bound.y1 - 1;
		CreatePattern(bound);
		// Vertical
		bound.x0 = PrintBound.x1;
		bound.x1 = bound.x0 + 1;
		bound.y1 = PrintBound.y0 - PAGE_RESOLUTION/8;
		bound.y0 = bound.y1 - PAGE_RESOLUTION/4;
		CreatePattern(bound);

		// Lower left
		// Horizontal
		bound.x1 = PrintBound.x0 - PAGE_RESOLUTION/8;
		bound.x0 = bound.x1 - PAGE_RESOLUTION/4;
		bound.y0 = PrintBound.y1;
		bound.y1 = bound.y0 + 1;
		CreatePattern(bound);
		// Vertical
		bound.x1 = PrintBound.x0;
		bound.x0 = bound.x1 - 1;
		bound.y0 = PrintBound.y1 + PAGE_RESOLUTION/8;
		bound.y1 = bound.y0 + PAGE_RESOLUTION/4;
		CreatePattern(bound);

		// Lower right
		// Horizontal
		bound.x0 = PrintBound.x1 + PAGE_RESOLUTION/8;
		bound.x1 = bound.x0 + PAGE_RESOLUTION/4;
		bound.y0 = PrintBound.y1;
		bound.y1 = bound.y0 + 1;
		CreatePattern(bound);
		// Vertical
		bound.x0 = PrintBound.x1;
		bound.x1 = bound.x0 + 1;
		bound.y0 = PrintBound.y1 + PAGE_RESOLUTION/8;
		bound.y1 = bound.y0 + PAGE_RESOLUTION/4;
		CreatePattern(bound);
	}
}

void CNewsletterDoc::CreatePattern(PBOX Bound)
{
	// Add the line under the header.
	PATTERN_CREATE_STRUCT pcs;
	memset(&pcs, 0, sizeof(pcs));
	pcs.bound = Bound;
	pcs.pattern = 0;
	pcs.color = COLOR_WHITE;
	pcs.bcolor = COLOR_BLACK;

	// Create the object.

	PatternObject* pPattern;
	if ((pPattern = get_database()->create_pattern_object(&pcs)) != NULL)
	{
		pPattern->set_panel(get_current_panel());
		// Add it to the list.

		append_object(pPattern);
	}
}

void CNewsletterDoc::BuildEmptyPictures(void)
{
	PMGDatabase* pDatabase = get_database();
	PMGFontServer* pFontServer = (PMGFontServer*)get_font_server();
	int nPanel = get_current_panel();

	CFrameObject* pFrame;

	// Construct the header.
	CTextStyle Style(pDatabase);
	Style.SetDefault();

	int nFace = typeface_server.find_face("Gourmand", FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	Style.Size(MakeFixed(10));
	Style.BaseSize(MakeFixed(10));
	Style.UpdateFontMetrics();

	// Add the text frame.
	FRAMEOBJ_CREATE_STRUCT fcs;
	memset(&fcs, 0, sizeof(fcs));

	fcs.bound.x0 = 0;
	fcs.bound.y0 = 0;
	fcs.bound.x1 = get_dimensions().x;
	fcs.bound.y1 = get_dimensions().y;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_middle;
//	fcs.flags = 0;
//	fcs.pStyle = NULL;

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		append_object(pFrame);

		CTxp Txp(pDatabase);

		// Set its panel.
		pFrame->set_panel(nPanel);
		Style.Alignment(fcs.alignment);
		pFrame->SetParagraphStyle(&Style);

		TRY
		{
			Txp.Init(pFrame, 0);

			// Put in the first line of text.
			Txp.Style(Style);
			Txp.InsertString("This page intentionally left blank.");

			pFrame->calc();
		}
		END_TRY
	}
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CTShirtDoc

ProjectInfo CTShirtDoc::StandardInfo =
{
	PROJECT_TYPE_TShirt,
	0,
	PORTRAIT,
	sizeof(newsletter_panels)/sizeof(newsletter_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	newsletter_panels
};

IMPLEMENT_SERIAL(CTShirtDoc, CNewsletterDoc, 0 /* schema number*/ )


CTShirtDoc::CTShirtDoc()
{
	info = StandardInfo;
}

CTShirtDoc::~CTShirtDoc()
{
}

BEGIN_MESSAGE_MAP(CTShirtDoc, CNewsletterDoc)
	//{{AFX_MSG_MAP(CTShirtDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTShirtDoc commands

void CTShirtDoc::GoToFirstPage()
{
	/* Used for templates only */

	LoadPage(GetPage(0));
}


ProjectInfo CCraftDoc::StandardInfo =
{
	PROJECT_TYPE_Craft,
	0,
	PORTRAIT,
	sizeof(newsletter_panels)/sizeof(newsletter_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	newsletter_panels
};

IMPLEMENT_SERIAL(CCraftDoc, CNewsletterDoc, 0 /* schema number*/ )

CCraftDoc::CCraftDoc()
{
	info = StandardInfo;
}

CCraftDoc::~CCraftDoc()
{
}

BEGIN_MESSAGE_MAP(CCraftDoc, CNewsletterDoc)
	//{{AFX_MSG_MAP(CCraftDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


