/*
// $Workfile: photoprj.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/photoprj.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     2/24/99 9:14p Psasse
// fixed a memory leak
// 
// 3     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     1/13/99 1:41p Psasse
// initial add
// 
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "photoprj.h"
#include "file.h"
#include "util.h"
#include "sortarry.h"

#define NUM_SUBSECTIONS 3
//#define READ_FROM_SPREADSHEET

static void SwapWord(WORD* w)
{
#ifdef _WIN32
	BYTE tmp = ((BYTE*)w)[0];
	((BYTE*)w)[0] = ((BYTE*)w)[1];
	((BYTE*)w)[1] = tmp;
#else
	_asm
	{
		les	bx, dword ptr w
		mov	ax, es:[bx]
		xchg	ah, al
		mov	es:[bx], ax
	}
#endif
}


BOOL CPhotoPrjList::ReadList(LPCSTR pFileName)
{
	BOOL fSuccess = FALSE;
#ifdef READ_FROM_SPREADSHEET
#ifndef WIN32
	// This should not compile.
	>>> "CStdioFile has problems in 16-bit!") <<<
#endif
	//pFileName = GetGlobalPathManager()->LocatePath(pFileName);

	CString csPath;
	CString csDrive, csDirectory, csFile;
	CString csLine;
	CStdioFile File;
	CMlsStringArray csaFields;
	CLabelData* pLabelData[3];
	CPhotoPrjData* pPhotoPrjData = NULL;

	pLabelData[0] = pLabelData[1] = pLabelData[2] = NULL;

	TRY
	{
		Util::SplitPath(pFileName, &csDrive, &csDirectory, &csFile, NULL);
		Util::ConstructPath(csPath, csDrive, csDirectory, csFile, ".TXT");

		TRACE("Open spreadsheet %s for label names.\n", pFileName);

		// Open file.
		if (File.Open(csPath, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
		{
			// Read the format line.
			if (File.ReadString(csLine))
			{
				int counter = 0;
				
				// Now read the rest of the lines.
				while (File.ReadString(csLine))
				{
					csaFields.SetNames(csLine, '\t');
					
					
				   if(counter == 0)
					{
						pPhotoPrjData = new CPhotoPrjData;
						TRACE("new CPhotoPrjData in ReadList\n");
					}

					pLabelData[counter] = new CPhotoLabelData;
					TRACE("new CPhotoLabelData in ReadList\n");

					if (pLabelData[counter]->ReadFromFields(csaFields))
					{
						if (pLabelData[counter]->Orientation() != CPaperInfo::PAPER_OrientationInvalid)
						{
							pPhotoPrjData->m_pSubAreaData[counter] = (CPhotoLabelData*)pLabelData[counter];

						}
					}


					if(counter == (NUM_SUBSECTIONS - 1))
					{
						m_cpaPhotoProjects.Add(pPhotoPrjData);
							
						for (int z = 0; z < NUM_SUBSECTIONS; z++){
							// If we have a label now, delete it!
							pLabelData[z] = NULL;
							delete pLabelData[z];
							TRACE("deleting pLabelData in ReadList\n");
							pLabelData[z] = NULL;
						}

						pPhotoPrjData = NULL;
						delete pPhotoPrjData;
						TRACE("deleting pPhotoPrjData in ReadList\n");
						pPhotoPrjData = NULL;

						//reset the counter
						counter = 0;
					}
					else
						counter++;

				}
			}
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	
	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{
		if(pLabelData[x] != NULL)
		delete pLabelData[x];
		TRACE("deleting pLabelData in ReadList cleanup\n");
		pLabelData[x] = NULL;
	}

	TRACE("Got %d labels\n", Labels());

	StorageFile OutFile(pFileName);

	DWORD dwID = m_dwStandardId;

	CListHeader Header;

	memset(&Header, 0, sizeof(Header));
	Header.m_wVersion = Version;
	Header.m_wCount = PhotoProjects();

	if (OutFile.initialize() == ERRORCODE_None
		 && OutFile.write(&dwID, sizeof(dwID)) == ERRORCODE_None
		 && OutFile.write_record(&Header, sizeof(Header)) == ERRORCODE_None)
	{
		/* Write out each entry. */
		WORD w;
		for (w = 0; w < Header.m_wCount; w++)
		{
			CPhotoPrjData* pPhotoProject = PhotoProject(w);
			
			for (int y = 0; y < NUM_SUBSECTIONS; y++)
			{
				if (pPhotoProject == NULL
					 || pPhotoProject->m_pSubAreaData[y]->WriteData(&OutFile) != ERRORCODE_None)
				{
					break;
				}
			}
		}

		if (w == Header.m_wCount)
		{
			fSuccess = TRUE;
		}
	}
	if (!fSuccess)
	{
	/* Get rid of the file. */
		OutFile.flush();
		OutFile.zap();
		//AfxMessageBox(IDS_NO_WRITE_LABEL_FILE);
      AfxMessageBox("Insert IDS_NO_WRITE_LABEL_FILE here");
	}

#else
/*
// Read the field names into our cache.
*/
	CString FileName = GetGlobalPathManager()->LocatePath(pFileName);
	CLabelData* pLabelData[3];
	CPhotoPrjData* pPhotoPrjData = NULL;
	pLabelData[0] = pLabelData[1] = pLabelData[2] = NULL;

	TRACE("Open file %s for label names.\n", FileName);
	ReadOnlyFile File(FileName);

	DWORD dwID;
	if (File.read(&dwID, sizeof(dwID)) == ERRORCODE_None)
	{
		if (dwID == m_dwStandardId || dwID == m_dwSwappedId)
		{
		/* We have a potentially valid file. */
			CListHeader Header;

			if (File.read_record(&Header, sizeof(Header)) == ERRORCODE_None)
			{
				if (dwID == m_dwSwappedId)
				{
				/*
				// We are swapped. Swap the header.
				// Is is assumed that the objects handle their own swapping
				// to native format.
				*/
					SwapWord(&Header.m_wVersion);
					SwapWord(&Header.m_wCount);
				}

				// Read all the labels.

				int counter = 0;

				WORD w;
				for (w = 0; w < Header.m_wCount * NUM_SUBSECTIONS; w++)
				{

				   if(counter == 0)
					{
						pPhotoPrjData = new CPhotoPrjData;
						TRACE("new CPhotoPrjData in ReadList\n");					
					}

					if (pPhotoPrjData->m_pSubAreaData[counter]->ReadData(&File) != ERRORCODE_None)
					{
						delete pPhotoPrjData;
						TRACE("deleting pPhotoPrjData in ReadList\n");
						pPhotoPrjData = NULL;
						break;
					}


               ASSERT(pPhotoPrjData->m_pSubAreaData[counter]->Orientation() != CPaperInfo::PAPER_OrientationInvalid);
					if(counter == (NUM_SUBSECTIONS - 1))
					{
						m_cpaPhotoProjects.Add(pPhotoPrjData);
							
						//reset the counter
						counter = 0;
					}
					else
						counter++;
				}
				if (w == Header.m_wCount)
				{
					ASSERT(PhotoProjects() == (int)Header.m_wCount);
					TRACE("Got %d photoprojects\n", PhotoProjects());

					fSuccess = TRUE;
				}
			}
		}
	}


#endif

	return fSuccess;
}

CPhotoPrjData* CPhotoPrjList::FindPhotoProject(const CString& strPhotoProject)
{
	for (int nPhoto = 0; nPhoto < m_cpaPhotoProjects.GetSize(); nPhoto++)
	{
		CPhotoPrjData* pPhotoPrj = (CPhotoPrjData*)m_cpaPhotoProjects[nPhoto];
		if (pPhotoPrj->m_pSubAreaData[0]->GetName() == strPhotoProject)
		{
			return pPhotoPrj;
		}
	}

	return NULL;
}

CPhotoPrjList::~CPhotoPrjList()
{
	int nPhotoProjects = PhotoProjects();

	for (int nPhotoProject = 0; nPhotoProject < nPhotoProjects; nPhotoProject++)
	{
		if(m_cpaPhotoProjects.GetSize() > 0)
		{
			if(PhotoProject(nPhotoProject))
				delete PhotoProject(nPhotoProject);

		}
	}
}


CPhotoPrjData::CPhotoPrjData()
{
	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{
		m_pSubAreaData[x] = new CPhotoLabelData();
		TRACE("new CPhotoLabelData in Constructor\n");
	}
}

CPhotoPrjData::~CPhotoPrjData()
{

	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{
		if(m_pSubAreaData[x])
		{
		  delete m_pSubAreaData[x];
		  m_pSubAreaData[x] = NULL;
		}
	}
}

static void SwapPoint(CFixedPoint& p)
{
	CFixed t = p.x;
	p.x = p.y;
	p.y = t;
}

//Class CPhotoLabelData
///////////////////////////////////////////////////////////////////////////
//
// DrawPage()
//
// drawmode has 3 possible values:
// DRAW_PREVIEW = draw page with solid pen and fill with white
// DRAW_BACKGROUND = draw white page
// DRAW_FOREGROUND = draw page with dot pen and fill with NULL brush
//
///////////////////////////////////////////////////////////////////////////
CPhotoLabelData::CPhotoLabelData()
{
}

CPhotoLabelData::~CPhotoLabelData()
{
}

void CPhotoLabelData::DrawPage(CDC* pDC, const CRect& crClient, BOOL fLandscape /*=FALSE*/,
							CRect* pDrawnRect /*=NULL*/, short drawmode, BOOL fEraseBackground)
{
	pDC->SaveDC();

	BOOL bScale = drawmode == DRAW_PREVIEW;
	BOOL bPaintPage = FALSE; //(drawmode == DRAW_PREVIEW || drawmode == DRAW_BACKGROUND);

	const CFixed lTractorWidth = MakeFixed(7)/16;			/* 7/16" */
	const CFixed lPinFeedHoleRadius = MakeFixed(1)/8;		/* 1/8"  */
	const CFixed lPinFeedHoleOffset = MakeFixed(1)/2;		/* 1/2"  */
	const int nMinBorder = 4;

	/*
	// Unpack some variables for faster access.
	*/

	int nWidth = crClient.Width();
	int nHeight = crClient.Height();

	CBrush WhiteBrush;
	WhiteBrush.CreateStockObject(WHITE_BRUSH);
	CBrush BlackBrush;
	BlackBrush.CreateStockObject(BLACK_BRUSH);
	CBrush NullBrush;
	NullBrush.CreateStockObject(NULL_BRUSH);
	CPen BlackPen;
	BlackPen.CreateStockObject(BLACK_PEN);
	CPen WhitePen;
	WhitePen.CreateStockObject(WHITE_PEN);

	CFixedPoint Page = PaperDims();
	BOOL fTractor = (Type() == PAPER_Continuous);

	// Tractor feed will have more X reduction.
	CFixed lExtraXScale = fTractor ? lTractorWidth*2 : 0;
	CFixed lExtraYScale = 0;

	CFixed lScale;

	// Get the scale factor for the page.
	lScale = MakeFixed(((nWidth < nHeight) ? nWidth : nHeight) - nMinBorder);

	// Compute the larger dimension.
	CFixed lScaleDenom;

	if (Page.x + lExtraXScale > Page.y + lExtraYScale)
	{
		lScaleDenom = Page.x + lExtraXScale;
	}
	else
	{
		lScaleDenom = Page.y + lExtraYScale;
	}

	lScale = DivFixed(lScale, lScaleDenom);

	if (fLandscape)
	{
		SwapPoint(Page);
	}

	int nScaledPageWidth, nScaledPageHeight, nXOrigin, nYOrigin = 0;

	if (bScale)
	{
		// Compute the origin (upper left hand corner) to center the page in the view.
		nScaledPageWidth = FixedInteger(MulFixed(Page.x, lScale));
		nScaledPageHeight = FixedInteger(MulFixed(Page.y, lScale));
		nXOrigin = (crClient.left + crClient.right - nScaledPageWidth)/2;
		nYOrigin = (crClient.top + crClient.bottom - nScaledPageHeight)/2;
	}
	else
	{
		nScaledPageWidth = nWidth; //FixedInteger(MulFixed(Page.x, lScale));
		nScaledPageHeight = nHeight; //FixedInteger(MulFixed(Page.y, lScale));
		nXOrigin = crClient.left; //+ crClient.right - nScaledPageWidth)/2;
		nYOrigin = crClient.top; // + crClient.bottom - nScaledPageHeight)/2;
		lScale = DivFixed(MakeFixed(nWidth), Page.x);
	}

	int oldrop2 = pDC->GetROP2();

	if (bPaintPage)
		pDC->SelectObject(&WhiteBrush);
	else 
	{
		pDC->SelectObject(&NullBrush);
		pDC->SetROP2(R2_MERGEPENNOT);
		
	}
	
	if (drawmode == DRAW_BACKGROUND)
		pDC->SelectObject(&WhitePen);	
	else
		pDC->SelectObject(&BlackPen);

	// Paint the page boundary.
	CBrush* pOldBrush = NULL;

	if (fEraseBackground)
	{

		if (!bPaintPage)
			pOldBrush = pDC->SelectObject(&WhiteBrush);
	//}
	
	pDC->Rectangle(nXOrigin,
						 nYOrigin,
						 nXOrigin + nScaledPageWidth,
						 nYOrigin + nScaledPageHeight);
	
	//if (fEraseBackground)
	//{
		if (!bPaintPage)
			pDC->SelectObject(pOldBrush);
	}

	// restore original ROP2 value
	if (!bPaintPage)
		pDC->SetROP2(oldrop2);

	if (pDrawnRect != NULL)
	{
		pDrawnRect->left = nXOrigin;
		pDrawnRect->top = nYOrigin;
		pDrawnRect->right = nXOrigin+nScaledPageWidth;
		pDrawnRect->bottom = nYOrigin+nScaledPageHeight;
	}

	if (drawmode != DRAW_BACKGROUND)
	if (fLandscape)
	{
		if (fTractor)
		{
			// Paint the tractor feed pin holes.
			pDC->SelectObject(&BlackBrush);
			CFixed x = lPinFeedHoleOffset/2;
			int y0 = nYOrigin + FixedInteger(MulFixed(lTractorWidth/2, lScale));
			int y1 = nYOrigin + FixedInteger(MulFixed(Page.y-lTractorWidth/2, lScale));
			int nScaledHoleRadius = FixedInteger(MulFixed(lPinFeedHoleRadius, lScale));

			int nHalfTractor = FixedInteger(MulFixed(lTractorWidth, lScale)/2);
			do
			{
				int nThisX = nXOrigin + FixedInteger(MulFixed(x, lScale));
				pDC->Ellipse(nThisX - nScaledHoleRadius, y0 - nScaledHoleRadius,
							  nThisX + nScaledHoleRadius, y0 + nScaledHoleRadius);
				pDC->Ellipse(nThisX - nScaledHoleRadius, y1 - nScaledHoleRadius,
							  nThisX + nScaledHoleRadius, y1 + nScaledHoleRadius);
				x += lPinFeedHoleOffset;
			} while (x < Page.x);
		}
		pDC->SelectObject(&NullBrush);

		// Loop through the slots.

		CFixedPoint Slot = SlotDims();

		SwapPoint(Slot);

		int nScaledWidth = FixedInteger(MulFixed(Slot.x, lScale));
		int nScaledHeight = FixedInteger(MulFixed(Slot.y, lScale));

		CFixedPoint Step = SlotStep();
		SwapPoint(Step);
		CFixedPoint Origin = SlotOrigin();
		SwapPoint(Origin);

		CFixed x = Origin.x;

		int nRows = SlotsDown();
		for (int nRow = 0; nRow < nRows; nRow++)
		{
			CRect crDraw;
			crDraw.left = nXOrigin + FixedInteger(MulFixed(x, lScale));
			crDraw.right = nXOrigin + FixedInteger(MulFixed(x+Slot.x, lScale));

			CFixed y = Origin.y;
			int nColumns = SlotsAcross();

			for (int nColumn = 0; nColumn < nColumns; nColumn++)
			{
				crDraw.top = nYOrigin + FixedInteger(MulFixed(y, lScale));
				crDraw.bottom = nYOrigin + FixedInteger(MulFixed(y+Slot.y, lScale));

				if (!bPaintPage)
					DrawSlot(pDC->GetSafeHdc(), crDraw, fLandscape, TRUE, NULL, RGB(255, 255, 255), FALSE);

				// Next row (y position).
				y += Step.y;
			}
			// Next column (x position).
			x += Step.x;

			if (nRow+1 == HalfPageRows())
			{
				CFixed lSkip = HalfPagePerfSkip();

				// Paint the perforation half way down the skip size.

				int nHalfX = nXOrigin + FixedInteger(MulFixed(x + lSkip/2, lScale));

				CPen DotPen;
				DotPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));

				pDC->SelectObject(&DotPen);
				pDC->MoveTo(nHalfX, nYOrigin);
				pDC->LineTo(nHalfX, nYOrigin+nScaledPageHeight);
				pDC->SelectObject(&BlackPen);

				x += lSkip;
			}
		}
	}
	else  //portrait
	{
		if (fTractor)
		{
			// Paint the tractor feed pin holes.
			pDC->SelectObject(&BlackBrush);
			CFixed y = lPinFeedHoleOffset/2;
			int x0 = nXOrigin + FixedInteger(MulFixed(lTractorWidth/2, lScale));
			int x1 = nXOrigin + FixedInteger(MulFixed(Page.x-lTractorWidth/2, lScale));
			int nScaledHoleRadius = FixedInteger(MulFixed(lPinFeedHoleRadius, lScale));

			int nHalfTractor = FixedInteger(MulFixed(lTractorWidth, lScale)/2);
			do
			{
				int nThisY = nYOrigin + FixedInteger(MulFixed(y, lScale));
				pDC->Ellipse(x0 - nScaledHoleRadius, nThisY - nScaledHoleRadius,
							  x0 + nScaledHoleRadius, nThisY + nScaledHoleRadius);
				pDC->Ellipse(x1 - nScaledHoleRadius, nThisY - nScaledHoleRadius,
							  x1 + nScaledHoleRadius, nThisY + nScaledHoleRadius);
				y += lPinFeedHoleOffset;
			} while (y < Page.y);
		}
		pDC->SelectObject(&NullBrush);

		// Loop through the slots.

		CFixedPoint Slot = SlotDims();
		int nScaledWidth = FixedInteger(MulFixed(Slot.x, lScale));
		int nScaledHeight = FixedInteger(MulFixed(Slot.y, lScale));
//		int nScaledRadius = FixedInteger(MulFixed(SlotCornerRadius(), lScale));

		CFixedPoint Step = SlotStep();
		CFixedPoint Origin = SlotOrigin();

		CFixed y = Origin.y;

		int nRows = SlotsDown();
		for (int nRow = 0; nRow < nRows; nRow++)
		{
			CRect crDraw;
			crDraw.top = nYOrigin + FixedInteger(MulFixed(y, lScale));
			crDraw.bottom = nYOrigin + FixedInteger(MulFixed(y+Slot.y, lScale));

			crDraw.top += (OriginOffset()).y;
			crDraw.bottom += (OriginOffset()).y;

			CFixed x = Origin.x;
			int nColumns = SlotsAcross();

			for (int nColumn = 0; nColumn < nColumns; nColumn++)
			{
				crDraw.left = nXOrigin + FixedInteger(MulFixed(x, lScale));
				crDraw.right = nXOrigin + FixedInteger(MulFixed(x+Slot.x, lScale));

				//if (!bPaintPage)
					DrawSlot(pDC->GetSafeHdc(), crDraw, fLandscape, TRUE, NULL, RGB(255, 255, 255), FALSE);

				// Next column (x position).
				x += Step.x;
			}
			// Next row (y position).
			y += Step.y;

			if (nRow+1 == HalfPageRows())
			{
				CFixed lSkip = HalfPagePerfSkip();

				// Paint the perforation half way down the skip size.

				int nHalfY = nYOrigin + FixedInteger(MulFixed(y + lSkip/2, lScale));

				CPen DotPen;
				DotPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));

				pDC->SelectObject(&DotPen);
				pDC->MoveTo(nXOrigin, nHalfY);
				pDC->LineTo(nXOrigin+nScaledPageWidth, nHalfY);
				pDC->SelectObject(&BlackPen);

				y += lSkip;
			}
		}
	}
	pDC->RestoreDC(-1);
}

void CPhotoLabelData::DrawPage3D(CDC* pDC, const CRect& crClient, int nGap, BOOL fLandscape /*=FALSE*/, CRect* pDrawnRect /*=NULL*/, CRect* pEraseRect /*=NULL*/, BOOL fEraseBackground)
{
	CRect crDrawn;
	CRect crIn = crClient;
	CRect crErase = (pEraseRect == NULL) ? crClient : *pEraseRect;

	crIn.InflateRect(-(3+nGap), -(3+nGap));

	DrawPage(pDC,
			   crIn,
			   fLandscape,
			   &crDrawn,
				DRAW_PREVIEW,
				fEraseBackground);

	if (pDrawnRect != NULL)
	{
		*pDrawnRect = crDrawn;
	}

	// Draw a white border around the image.
	CBrush cbWhite;
	cbWhite.CreateStockObject(WHITE_BRUSH);

	crDrawn.InflateRect(nGap, nGap);
	CRect crFill(crDrawn.left, crDrawn.top, crDrawn.right, crDrawn.top+nGap);
	pDC->FillRect(crFill, &cbWhite);
	crFill.top = crDrawn.bottom-nGap;
	crFill.bottom = crDrawn.bottom;
	pDC->FillRect(crFill, &cbWhite);

	crFill = CRect(crDrawn.left, crDrawn.top+nGap, crDrawn.left+nGap, crDrawn.bottom-nGap);
	pDC->FillRect(crFill, &cbWhite);
	crFill.left = crDrawn.right-nGap;
	crFill.right = crDrawn.right;
	pDC->FillRect(crFill, &cbWhite);

	// Inflate the rect all the way out for the 3D border draw.
	crDrawn.InflateRect(3, 3);

	// Erase the areas of the preview window that are not covered
	// by the actual preview.

	CBrush cbBackground;
	if (cbBackground.CreateSolidBrush(GetSysColor(COLOR_BTNFACE)))
	{
		if (crErase.left < crDrawn.left)
		{
			CRect crFill(crErase.left, crErase.top, crDrawn.left, crErase.bottom);
			pDC->FillRect(crFill, &cbBackground);
		}

		if (crErase.top < crDrawn.top)
		{
			CRect crFill(crDrawn.left, crErase.top, crDrawn.right, crDrawn.top);
			pDC->FillRect(crFill, &cbBackground);
		}

		if (crErase.right > crDrawn.right)
		{
			CRect crFill(crDrawn.right, crErase.top, crErase.right, crErase.bottom);
			pDC->FillRect(crFill, &cbBackground);
		}

		if (crErase.bottom > crDrawn.bottom)
		{
			CRect crFill(crDrawn.left, crDrawn.bottom, crDrawn.right, crErase.bottom);
			pDC->FillRect(crFill, &cbBackground);
		}
	}
	// Draw a simple 3D border around the preview area.

	Util::Draw3dBorder(
		pDC,
		crDrawn,
		GetSysColor(COLOR_BTNHILIGHT),
		GetSysColor(COLOR_BTNSHADOW),
		GetSysColor(COLOR_BTNFACE),
		GetSysColor(COLOR_BTNTEXT),
		RGB(255,255,255),
		2,
		TRUE,
		FALSE,
		FALSE);
}
