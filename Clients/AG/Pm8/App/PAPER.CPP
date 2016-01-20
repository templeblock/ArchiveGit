/*
// Paper info.
//
// $Header: /PM8/App/PAPER.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/PAPER.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 4     12/09/97 8:31a Jstoner
// modified desktop so that label outline is dotted and shows thru objects
// drawn on top of desktop
// affects label and sticker projects
// 
// 3     12/08/97 9:27a Jstoner
// dashed lines for label slots on desktop
// 
// 2     12/04/97 10:58a Jstoner
// added parameter to drawpage which allows label and page outlines to be
// drawn on top of all objects
// 
//    Rev 1.0   14 Aug 1997 15:23:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:34   Fred
// Initial revision.
// 
//    Rev 1.3   20 Jul 1997 13:21:46   Jay
// Fixes for label preview highlight
// 
//    Rev 1.2   31 Jan 1997 15:50:52   Jay
// New desktop background color
// 
//    Rev 1.1   31 Jul 1996 11:58:32   Fred
// 16-bit compatibility
// 
//    Rev 1.0   14 Mar 1996 13:45:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:52   FRED
// Initial revision.
// 
//    Rev 1.11   13 Sep 1995 11:33:38   JAY
// New 32-bit stuff
// 
//    Rev 1.10   28 Aug 1995 20:31:22   FRED
// Print alignment
// 
//    Rev 1.9   11 Aug 1995 10:32:40   JAY
// Fixed a bug in ComputeMargins.
// 
//    Rev 1.8   10 Aug 1995 12:59:36   JAY
// Highlight parameter to DrawSlot().
// 
//    Rev 1.7   09 Aug 1995 15:20:24   JAY
// Added a paremeter to DrawPage3D.
// 
//    Rev 1.6   31 Jul 1995 10:22:54   JAY
// Added InitAsLabels().
// 
//    Rev 1.5   28 Jul 1995 12:28:14   JAY
// Added some access functions.
// 
//    Rev 1.4   21 Jul 1995 17:16:12   JAY
// Now calls ComputeMargins() after a read to make self up-to-date.
// 
//    Rev 1.3   20 Jul 1995 16:24:36   JAY
// Made the inner hole on audio tape labels have a black outline.
// 
//    Rev 1.2   20 Jul 1995 15:54:32   JAY
// Tweaked the special displays a little bit.
// 
//    Rev 1.1   20 Jul 1995 12:48:54   JAY
// Support for drawing, labels, etc.
// 
//    Rev 1.0   30 Jun 1995 16:33:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "paper.h"
#include "util.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

/*
// Constructor.
*/

CPaperInfo::CPaperInfo()
{
	memset(&m_Record, 0, sizeof(m_Record));
#if 0
	// Some default parameters.
	InitAsSheet(8500, 11000,
					250, 250,
					8000, 10500,
					1000, 1000,
					PAPER_CutSheet);
#endif
}

/*
// Return the page area of a slot.
*/

CFixedRect CPaperInfo::SlotBounds(int nSlot) const
{
	CFixedRect Bounds;
	int nSlots = Slots();

	if (nSlots == 0)
	{
		ASSERT(FALSE);
		Bounds.Left = 0;
		Bounds.Top = 0;
	}
	else
	{
		if (nSlot < 0 || nSlot >= Slots())
		{
			ASSERT(FALSE);
			nSlot = 0;
		}

		int x = nSlot % m_Record.m_nSlotsAcross;
		int y = nSlot / m_Record.m_nSlotsAcross;

		Bounds.Left = m_Record.m_SlotOrigin.x + x*m_Record.m_SlotStep.x;
		Bounds.Top = m_Record.m_SlotOrigin.y + y*m_Record.m_SlotStep.y;

		if (m_Record.m_nHalfPageRows != 0 && y >= m_Record.m_nHalfPageRows)
		{
			Bounds.Top += m_Record.m_HalfPagePerfSkip;
		}
	}

	Bounds.Right = Bounds.Left + m_Record.m_SlotDims.x;
	Bounds.Bottom = Bounds.Top + m_Record.m_SlotDims.y;

	return Bounds;
}

/*
// Set the margin style.
*/

void CPaperInfo::SetMarginStyle(int nMarginStyle, BOOL fRecompute /*=TRUE*/)
{
	m_Record.m_nMarginStyle = nMarginStyle;
	if (fRecompute)
	{
		ComputeMargins();
	}
}

/*
// Set the physical margins.
*/

void CPaperInfo::SetPhysicalMargins(const CFixedRect& PhysicalMargins, BOOL fRecompute /*=TRUE*/)
{
	m_Record.m_PhysicalMargins = PhysicalMargins;
	if (fRecompute)
	{
		ComputeMargins();
	}
}

/*
// Set this up to describe a single sheet of paper.
// This is useful for describing what's in a printer.
*/

void CPaperInfo::InitAsSheet(
		const CFixedPoint& Size,
		const CFixedPoint& Offset,
		const CFixedRect& Margins,
		int nType,
		int nMarginStyle /*=MARGINS_Equal*/)
{
	memset(&m_Record, 0, sizeof(m_Record));

 	m_Record.m_nType = nType;
	m_Record.m_nOrientation = PAPER_Portrait;
	m_Record.m_nClass = CLASS_Sheet;

	m_Record.m_nSlotClass = SLOT_Rectangular;
	m_Record.m_nSlotsAcross = 1;
	m_Record.m_nSlotsDown = 1;

	m_Record.m_PaperDims.x =
		m_Record.m_SlotDims.x = Size.x;
	m_Record.m_PaperDims.y =
		m_Record.m_SlotDims.y = Size.y;

	m_Record.m_OriginOffset = Offset;
	
	/* Compute the physical margins. */

	m_Record.m_PhysicalMargins = Margins;

	/* We don't require any margins. */

//	m_Record.m_MinimumMargins.Left =
//		m_Record.m_MinimumMargins.Top =
//		m_Record.m_MinimumMargins.Right =
//		m_Record.m_MinimumMargins.Bottom = MakeFixed(0);

	m_Record.m_nMarginStyle = nMarginStyle;

	/* Other assorted trivialities. */

//	m_Record.m_SlotStep.x = 0;
//	m_Record.m_SlotStep.y = 0;
//	m_Record.m_Radius = 0;
//	m_Record.m_HalfPagePerfSkip = 0;
//	m_Record.m_nHalfPageRows = 0;

	/* Now go through and compute the margins to use. */

	ComputeMargins();

/*
// For a full page slot from a printer page, we use some internal knowledge.
// The origin of the coordinate system is where the left and top margins
// intersect. In order to make this come out as (0, 0), we say our slot begins
// at the negative of the left and top margins.
// This could also be handled (I guess) by setting the origin to 0 and using
// the origin offset. However, I don't think this is what we will want
// eventually even though they are the same at this point (in my mind).
*/

//	m_Record.m_SlotOrigin.x = -m_Record.m_Margins.Left;
//	m_Record.m_SlotOrigin.y = -m_Record.m_Margins.Top;
}

void CPaperInfo::InitAsLabels(CFixed Width, CFixed Height,
										CFixed SlotWidth, CFixed SlotHeight,
										int nAcross, int nDown,
										CFixed SlotOriginX, CFixed SlotOriginY,
										CFixed SlotXStep, CFixed SlotYStep)
{
	memset(&m_Record, 0, sizeof(m_Record));

 	m_Record.m_nType = PAPER_CutSheet;
	m_Record.m_nOrientation = PAPER_Portrait;
	m_Record.m_nClass = CLASS_Labels;

	m_Record.m_nSlotClass = SLOT_Rectangular;
	m_Record.m_nSlotsAcross = nAcross;
	m_Record.m_nSlotsDown = nDown;

	m_Record.m_PaperDims.x = Width;
	m_Record.m_PaperDims.y = Height;

	m_Record.m_SlotDims.x = SlotWidth;
	m_Record.m_SlotDims.y = SlotHeight;

	/* Compute the physical margins. */

//	m_Record.m_PhysicalMargins.Left =
//		m_Record.m_PhysicalMargins.Top =
//		m_Record.m_PhysicalMargins.Right =
//		m_Record.m_PhysicalMargins.Bottom = MakeFixed(0);

	/* We don't require any margins. */

//	m_Record.m_MinimumMargins.Left =
//		m_Record.m_MinimumMargins.Top =
//		m_Record.m_MinimumMargins.Right =
//		m_Record.m_MinimumMargins.Bottom = MakeFixed(0);

	m_Record.m_nMarginStyle = MARGINS_Symmetric;

//	m_Record.m_OriginOffset.x = 0;
//	m_Record.m_OriginOffset.y = 0;

	/* Other assorted trivialities. */

	m_Record.m_SlotOrigin.x = SlotOriginX;
	m_Record.m_SlotOrigin.y = SlotOriginY;
	m_Record.m_SlotStep.x = SlotXStep;
	m_Record.m_SlotStep.y = SlotYStep;
//	m_Record.m_Radius = 0;
//	m_Record.m_HalfPagePerfSkip = 0;
//	m_Record.m_nHalfPageRows = 0;

	/* Now go through and compute the margins to use. */

	ComputeMargins();
}

/*
// Compute the margins given everything that has been set.
*/

void CPaperInfo::ComputeMargins(void)
{
/*
// We start with the physical margins. These are the true minimum margins
// we can support.
*/

	CFixedRect Margins = m_Record.m_PhysicalMargins;

/*
// Intersect this with the artificial minimum margins specified elsewhere.
*/

	if (Margins.Left < m_Record.m_MinimumMargins.Left)
	{
		Margins.Left = m_Record.m_MinimumMargins.Left;
	}
	if (Margins.Right < m_Record.m_MinimumMargins.Right)
	{
		Margins.Right = m_Record.m_MinimumMargins.Right;
	}
	if (Margins.Top < m_Record.m_MinimumMargins.Top)
	{
		Margins.Top = m_Record.m_MinimumMargins.Top;
	}
	if (Margins.Bottom < m_Record.m_MinimumMargins.Bottom)
	{
		Margins.Bottom = m_Record.m_MinimumMargins.Bottom;
	}

/*
// And handle symmetry.
*/

	if (m_Record.m_nMarginStyle != MARGINS_Minimal)
	{
	/* We want some kind of symmetry. (We have at least MARGINS_Symmetric)*/

		CFixed LeftRight = __max(Margins.Left, Margins.Right);
		CFixed TopBottom = __max(Margins.Top, Margins.Bottom);

		if (m_Record.m_nMarginStyle == MARGINS_Equal)
		{
		/* Make left/right margins be equal to top/bottom margins. */
			if (LeftRight < TopBottom)
			{
				LeftRight = TopBottom;
			}
			else
			{
				TopBottom = LeftRight;
			}
		}

		Margins.Left = Margins.Right = LeftRight;
		Margins.Top = Margins.Bottom = TopBottom;
	}

/*
// Now some special case handling.
// If the margins exceed the slot dimensions at all, then we're in trouble.
// So, if that happens, we just ignore everything and set them to 1/4 of
// the dimension that causes trouble. It's not perfect, but it prevents
// project dimensions of 0 or negative.
*/

	if (Margins.Left + Margins.Right >= m_Record.m_SlotDims.x)
	{
		Margins.Left = Margins.Right = m_Record.m_SlotDims.x >> 2;
	}

	if (Margins.Top + Margins.Bottom >= m_Record.m_SlotDims.y)
	{
		Margins.Top = Margins.Bottom = m_Record.m_SlotDims.y >> 2;
	}

/* Finally, send it home. */

	m_Record.m_Margins = Margins;

/* And update the page margins. */

	UpdatePageValues();
}

/*
// Update the values stored at page resolution.
// If a paper info is written to file, these will not be stored since they
// can easily be computed. However, we don't necessarily want to compute
// them EVERY time.
*/

void CPaperInfo::UpdatePageValues(void)
{
	/*
	// A little trick with MulFixed to help convert from 65536 dpi to
	// PAGE_RESOLUTION.
	// We technically want MulDivFixed(Left,
	//											  MakeFixed(PAGE_RESOLUTION),
	//											  MakeFixed(65536));
	// So, divide the second and third parameters by 65536 (which has no
	// effect on the result). This reduces the third parameter to 1, and
	// reduces the MulDivFixed to a MulFixed since division by 1 is useless.
	*/

	m_PageMargins.x0 = (PCOORD)MulFixed(m_Record.m_Margins.Left, PAGE_RESOLUTION);
	m_PageMargins.y0 = (PCOORD)MulFixed(m_Record.m_Margins.Top, PAGE_RESOLUTION);
	m_PageMargins.x1 = (PCOORD)MulFixed(m_Record.m_Margins.Right, PAGE_RESOLUTION);
	m_PageMargins.y1 = (PCOORD)MulFixed(m_Record.m_Margins.Bottom, PAGE_RESOLUTION);

	/*
	// Compute the width and height.
	*/

	CFixed Width = m_Record.m_SlotDims.x
						- (m_Record.m_Margins.Left + m_Record.m_Margins.Right);
	CFixed Height = m_Record.m_SlotDims.y
						- (m_Record.m_Margins.Top + m_Record.m_Margins.Bottom);

	m_ProjectDims.x = (PCOORD)MulFixed(Width, PAGE_RESOLUTION);
	m_ProjectDims.y = (PCOORD)MulFixed(Height, PAGE_RESOLUTION);
}

static void SwapPoint(CFixedPoint& p)
{
	CFixed t = p.x;
	p.x = p.y;
	p.y = t;
}

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
void CPaperInfo::DrawPage(CDC* pDC, const CRect& crClient, BOOL fLandscape /*=FALSE*/,
							CRect* pDrawnRect /*=NULL*/, short drawmode)
{
	pDC->SaveDC();

	BOOL bScale = drawmode == DRAW_PREVIEW;
	BOOL bPaintPage = (drawmode == DRAW_PREVIEW || drawmode == DRAW_BACKGROUND);

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
//	if (bPaintPage)
		pDC->Rectangle(nXOrigin,
					 nYOrigin,
					 nXOrigin + nScaledPageWidth,
					 nYOrigin + nScaledPageHeight);
	
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
//		int nScaledRadius = FixedInteger(MulFixed(SlotCornerRadius(), lScale));

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
					DrawSlot(pDC->GetSafeHdc(), crDraw, fLandscape, TRUE, NULL, RGB(255, 255, 255), !bPaintPage);

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
	else
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

			CFixed x = Origin.x;
			int nColumns = SlotsAcross();

			for (int nColumn = 0; nColumn < nColumns; nColumn++)
			{
				crDraw.left = nXOrigin + FixedInteger(MulFixed(x, lScale));
				crDraw.right = nXOrigin + FixedInteger(MulFixed(x+Slot.x, lScale));

				//if (!bPaintPage)
					DrawSlot(pDC->GetSafeHdc(), crDraw, fLandscape, TRUE, NULL, RGB(255, 255, 255), !bPaintPage);

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

void CPaperInfo::DrawPage3D(CDC* pDC, const CRect& crClient, int nGap, BOOL fLandscape /*=FALSE*/, CRect* pDrawnRect /*=NULL*/, CRect* pEraseRect /*=NULL*/)
{
	CRect crDrawn;
	CRect crIn = crClient;
	CRect crErase = (pEraseRect == NULL) ? crClient : *pEraseRect;

	crIn.InflateRect(-(3+nGap), -(3+nGap));

	DrawPage(pDC,
			   crIn,
			   fLandscape,
			   &crDrawn);

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

/*
// Draw a slot to a DC.
// if bDashed == TRUE, slot will be drawn with dot pen and filled with a NULL brush
*/


void CPaperInfo::DrawSlot(HDC hDC, const CRect& crSlot, BOOL fLandscape /*=FALSE*/, BOOL fSave /*=TRUE*/, COLORREF* pHighlightColor /*=NULL*/, COLORREF clPage /*=RGB(255, 255, 255)*/, BOOL bDashed)
{


	if (fSave)
	{
		::SaveDC(hDC);
	}

	CFixedPoint Slot = SlotDims();
	if (fLandscape)
	{
		SwapPoint(Slot);
	}

	int nClass = SlotClass();

	if (pHighlightColor != NULL)
	{
		HBRUSH hBrush;

		if ((hBrush = CreateSolidBrush(*pHighlightColor)) != NULL)
		{
			FillRect(hDC, crSlot, hBrush);
			DeleteObject(hBrush);
		}
	}

	HPEN hDotPen = NULL;

	if (!bDashed)
	{
   SelectObject(hDC, GetStockObject(pHighlightColor == NULL ? WHITE_BRUSH : NULL_BRUSH));		// NULL_BRUSH
	 SelectObject(hDC, GetStockObject(BLACK_PEN));
	}
	else
	{
		SelectObject(hDC, GetStockObject(NULL_BRUSH));
		hDotPen = CreatePen(PS_DOT, 1, RGB(0,0,0));
		ASSERT (hDotPen != NULL);
		if (hDotPen != NULL)
		 SelectObject(hDC, hDotPen);
		
		
	}


	switch (nClass)
	{
		case SLOT_Rectangular:
		{
			Rectangle(hDC,
						 crSlot.left,
						 crSlot.top,
						 crSlot.right,
						 crSlot.bottom);
			break;
		}
		case SLOT_Rounded:
		{
			CFixed lRadius = SlotCornerRadius();
			int nRadiusX = (int)MulDivFixed(lRadius, crSlot.Width(), Slot.x);
			int nRadiusY = (int)MulDivFixed(lRadius, crSlot.Height(), Slot.y);

			RoundRect(hDC,
						 crSlot.left,
						 crSlot.top,
						 crSlot.right,
						 crSlot.bottom,
						 nRadiusX*2, nRadiusY*2);
			break;
		}
		case SLOT_Circular:
		{
			/* draw the page itself */

			Ellipse(hDC,
					  crSlot.left,
					  crSlot.top,
					  crSlot.right,
					  crSlot.bottom);

			break;
		}
		case SLOT_Avery5198:
		{
		/* Avery #5198 audio tape label */

			CFixed lGap = MakeFixed(1)/8;
			int nGapX = (int)MulDivFixed(lGap, crSlot.Width(), Slot.x);
			int nGapY = (int)MulDivFixed(lGap, crSlot.Height(), Slot.y);

			// Computations for the inner part that is removed.
			int n916x = (int)MulDivFixed(MakeFixed(9)/16, crSlot.Width(), Slot.x);
			int n58x = (int)MulDivFixed(MakeFixed(5)/8, crSlot.Width(), Slot.x);
			int n58y = (int)MulDivFixed(MakeFixed(5)/8, crSlot.Height(), Slot.y);

			// Draw the boundary (a square with cut top corners).
			POINT p[7];
			p[0].x = crSlot.left+nGapX;
			p[0].y = crSlot.top;
			p[1].x = crSlot.right-nGapX;
			p[1].y = crSlot.top;
			p[2].x = crSlot.right;
			p[2].y = crSlot.top+nGapY;
			p[3].x = crSlot.right;
			p[3].y = crSlot.bottom;
			p[4].x = crSlot.left;
			p[4].y = crSlot.bottom;
			p[5].x = crSlot.left;
			p[5].y = crSlot.top+nGapY;
			p[6].x = p[0].x;
			p[6].y = p[0].y;

			BOOL fSuccess = FALSE;
#if 0
			HRGN rgnLabel = CreatePolygonRgn(p, 7, WINDING);
			if (rgnLabel != NULL)
			{
				HRGN rgnHole = CreateRoundRectRgn(crSlot.left+n916x,
															 crSlot.top+n58y,
															 crSlot.right-n916x,
															 crSlot.top+2*n58y,
															 n58x, n58y);
				if (rgnHole != NULL)
				{
					if (CombineRgn(rgnLabel, rgnLabel, rgnHole, RGN_DIFF) != ERROR)
					{
						PaintRgn(hDC, rgnLabel);
						FrameRgn(hDC,
									rgnLabel,
									(HBRUSH)GetStockObject(BLACK_BRUSH),
									1,
									1);
						fSuccess = TRUE;
					}
					DeleteObject(rgnHole);
				}
				DeleteObject(rgnLabel);
			}
#endif

			if (!fSuccess)
			{
				// Oh well, do it the old way. A little flicker, maybe...
				Polygon(hDC, p, 7);
			}

			// Draw the inner area that is removed.
			HBRUSH hBrush = NULL;

			if ((hBrush = CreateSolidBrush(clPage)) != NULL)
			{
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
				if (hOldBrush != NULL)
				{
					RoundRect(hDC,
								 crSlot.left+n916x,
								 crSlot.top+n58y,
								 crSlot.right-n916x,
								 crSlot.top+2*n58y,
								 n58x, n58y);
					
					SelectObject(hDC, hOldBrush);
				}
				DeleteObject(hBrush);
			}
			break;
		}
		case SLOT_Avery5385:
		case SLOT_Avery5386:
		{
		/*
		// Rotary cards.
		*/

		/*
		// Draw the little "nubs" on the bottom.
		*/

			int n14x = (int)MulDivFixed(MakeFixed(1)/4, crSlot.Width(), Slot.x);
			int n18x = n14x/2;
			int n12y = (int)MulDivFixed(MakeFixed(1)/2, crSlot.Height(), Slot.y);
			CFixed lOffset = (SlotClass() == SLOT_Avery5385)
										? MakeFixed(3)/2 : MakeFixed(7)/4;
			int nOffset = (int)MulDivFixed(lOffset, crSlot.Width(), Slot.x);
			int nLeft1 = crSlot.left + nOffset - n18x/2;
			int nRight1 = nLeft1 + n18x;
			int nRight2 = crSlot.right - nOffset + n18x/2;
			int nLeft2 = nRight2 - n18x;
			int nHalfY = crSlot.bottom - n12y/2;
			int nTopY = crSlot.bottom - n12y;

		/* Draw the boundary (a square with cut top corners). */

			POINT p[] =
			{
				{ crSlot.left, crSlot.top },
				{ crSlot.right, crSlot.top },
				{ crSlot.right, crSlot.bottom },
				// Right slit.
				{ nRight2, crSlot.bottom },
				{ nRight2, nHalfY },
				{ nRight2+n18x/2, nHalfY },
				{ nRight2+n18x/2, nTopY },
				{ nLeft2-n18x/2, nTopY },
				{ nLeft2-n18x/2, nHalfY },
				{ nLeft2, nHalfY },
				{ nLeft2, crSlot.bottom },
				// Left slit
				{ nRight1, crSlot.bottom },
				{ nRight1, nHalfY },
				{ nRight1+n18x/2, nHalfY },
				{ nRight1+n18x/2, nTopY },
				{ nLeft1-n18x/2, nTopY },
				{ nLeft1-n18x/2, nHalfY },
				{ nLeft1, nHalfY },
				{ nLeft1, crSlot.bottom },
				// Rest of box.
				{ crSlot.left, crSlot.bottom },
				{ crSlot.left, crSlot.top } 
			};

			Polygon(hDC, p, sizeof(p)/sizeof(p[0]));

			break;
		}
	}

	if (fSave);
	{
		::RestoreDC(hDC, -1);
	}
	if (hDotPen != NULL)
		DeleteObject(hDotPen);
}


/*
// Routines to put this guy away and bring him back.
*/

ERRORCODE CPaperInfo::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		error = pDevice->read_cstring(m_csName);
		ComputeMargins();
	}
	return error;
}

ERRORCODE CPaperInfo::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		error = pDevice->write_cstring(m_csName);
	}
	return error;
}

WORD CPaperInfo::SizeofData(StorageDevice* pDevice)
{
	return pDevice->size_record(sizeof(m_Record))
			+ pDevice->size_cstring(m_csName);
}
